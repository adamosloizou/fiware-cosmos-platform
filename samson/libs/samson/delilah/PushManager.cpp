
#include "samson/common/MessagesOperations.h"
#include "samson/delilah/Delilah.h"
#include "samson/network/Packet.h"

#include "PushManager.h"  // Own interface

namespace samson {
// ----------------------------------------------------
//
// PushItem
//
// ----------------------------------------------------

PushItem::PushItem(Delilah *delilah, size_t push_id
                   , engine::BufferPointer buffer
                   , const std::vector<std::string>& queues) {
  // Keep a pointer to delilah
  delilah_ = delilah;

  // Retain the buffer, just in case we have to resent
  buffer_ = buffer;

  // Identifier of this push block
  push_id_ = push_id;

  // Copy queues
  for (size_t i = 0; i < queues.size(); i++) {
    queues_.push_back(queues[i]);
  }

  // Default state
  state_ = init;
}

bool PushItem::IsFinished() {
  return ( state_ == completed );
}

bool PushItem::IsReadyForCommit() {
  return (state_ == ready_for_commit );
}

void PushItem::Review() {
  if (state_ == completed) {
    return;   // Nothing to do
  }
  if (state_ == init) {
    worker_id_ = delilah_->network->getRandomWorkerId();        // Get a random worker id to push content
    if (worker_id_ == (size_t)-1) {
      return;     // no worker available...
    }
    cronometer_.Reset();

    // Packet to send buffer to worker
    PacketPointer packet(new Packet(Message::PushBlock));
    packet->to = NodeIdentifier(WorkerNode, worker_id_);
    packet->message->set_push_id(push_id_);
    packet->set_buffer(buffer_);
    for (size_t i = 0; i < queues_.size(); i++) {
      packet->message->add_queue(queues_[i]);
    }

    // Send packet and wait for the answer
    delilah_->network->Send(packet);

    // Change state to waiting push confirmation
    state_ = waiting_push_confirmation;

    return;
  }

  // Check if my worker is away and come back to init state if so
  if (!delilah_->network->IsWorkerConnected(worker_id_)) {
    LM_W(("We are not connected to worker %lu anymore. Reseting push operation...", worker_id_ ));
    state_ = init;
    return;
  }

  // Is worker is not part of the cluster
  if (!delilah_->network->IsWorkerInCluster(worker_id_)) {
    LM_W(("Worker %lu is not part of the cluster anymore. Reseting push operation...", worker_id_ ));
    state_ = init;
    return;
  }


  if (cronometer_.seconds() > 30) {
    LM_W(("[%lu] Push item timeout 30 seconds. Reseting...", push_id_ ));
    // Reset by time
    state_ = init;
  }
}

void PushItem::ResetPushItem() {
  if (state_ == completed) {
    return;
  }
  LM_W(("[%lu] Reset Push item", push_id_ ));
  state_ = init;
}

// A Push response has been received from worker
void PushItem::receive(Message::MessageCode msgCode, size_t worker_id, au::ErrorManager& error) {
  if (worker_id != worker_id_) {
    LM_W(("Push[%lu] Received message %s from worker %lu in a push item while my worker id is %lu. Ignoring..."
          , push_id_
          , Message::messageCode(msgCode)
          , worker_id
          , worker_id_ ));
    return;
  }


  if (error.IsActivated()) {
    LM_W(("Push[%lu] Error received in a push operation %s.Reseting...", push_id_, error.GetMessage().c_str()));
    ResetPushItem();
    return;
  }

  if (state_ == init) {
    LM_W(("Push[%lu] Received message %s in a push item while in init mode. Ignoring...", push_id_,
          Message::messageCode(msgCode)));
    return;
  }

  if (state_ == waiting_push_confirmation) {
    if (msgCode != Message::PushBlockResponse) {
      LM_W(("Push[%lu] Recieved wrong message (%s) from worker while waiting for distribution confirmation"
            , push_id_
            , Message::messageCode(msgCode)));
      state_ = init;
      return;
    }

    // Change state to ready for commit
    state_ = ready_for_commit;

    return;
  }

  if (state_ == ready_for_commit) {
    LM_W(("Push[%lu] Recieved wrong message (%s) from worker while being ´ready for commit´"
          , push_id_
          , Message::messageCode(msgCode)));
    state_ = init;
    return;
  }

  if (state_ == waiting_commig_confirmation) {
    if (msgCode != Message::PushBlockCommitResponse) {
      LM_W(("Push[%lu] Recieved wrong message (%s) from worker while waiting for commit confirmation"
            , push_id_
            , Message::messageCode(msgCode)));
      state_ = init;
      return;
    }

    // Change state to completed
    state_ = completed;
  }
}

void PushItem::SendCommit() {
  if (state_ != ready_for_commit) {
    return;
  }

  // Send the commit message
  PacketPointer packet(new Packet(Message::PushBlockCommit));
  packet->to = NodeIdentifier(WorkerNode, worker_id_);
  packet->message->set_push_id(push_id_);
  packet->set_buffer(buffer_);

  delilah_->network->Send(packet);

  state_ = waiting_commig_confirmation;
}

size_t PushItem::push_id() {
  return push_id_;
}

size_t PushItem::time() {
  return cronometer_.seconds();
}

size_t PushItem::size() {
  if (buffer_ != NULL) {
    return buffer_->size();
  } else {
    return 0;
  }
}

std::string PushItem::str() {
  switch (state_) {
    case init: return au::str("[%lu] Uninitialized", push_id_);

    case waiting_push_confirmation: return au::str("[%lu] Waiting distribution confirmation from worker %lu"
                                                   , push_id_, worker_id_);

    case ready_for_commit: return "[%lu] Ready for commit";

    case waiting_commig_confirmation: return au::str("[%lu] Waiting commit confirmation from worker %lu"
                                                     , push_id_, worker_id_);

    case completed: return au::str("[%lu] Finalized", push_id_);
  }

  return "Error";
}

std::string PushItem::str_buffer_info() {
  if (buffer_ != NULL) {
    return au::str(buffer_->size(), "B");
  } else {
    return "No buffer";
  }
}

// ----------------------------------------------------
//
// PushManager
//
// ----------------------------------------------------

PushManager::PushManager(Delilah *delilah) {
  delilah_ = delilah;
  item_id_ = 1;
}

void PushManager::receive(Message::MessageCode msgCode, size_t worker_id, size_t push_id, au::ErrorManager& error) {
  PushItem *item = items_.findInMap(push_id);

  if (item) {
    item->receive(msgCode, worker_id, error);
  } else {
    LM_W(("PushBlock response associated with an item (%lu) not found.", push_id ));    // Comit ready push operations and remove old connections
  }

  Review();
}

size_t PushManager::Push(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  LM_V(("PushManager: pushing buffer %s to %lu queues ( delilah %s )"
        , au::str(buffer->size()).c_str()
        , queues.size()
        , au::code64_str(delilah_->get_delilah_id()).c_str()
        ));

  size_t item_id = item_id_++;
  PushItem *item = new PushItem(delilah_, item_id, buffer, queues);
  items_.insertInMap(item_id, item);

  // First review to send packet to the server
  item->Review();

  return item_id;
}

void PushManager::Review() {
  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); ) {      // Review and remove finished items
    PushItem *item = it->second;
    item->Review();     // Review item

    if (item->IsFinished()) {
      // Notification to inform that this push_id has finished
      engine::Notification *notification  = new engine::Notification("push_operation_finished");
      notification->environment().Set("push_id", it->first);
      notification->environment().Set("size",  item->size());
      engine::Engine::shared()->notify(notification);

      items_.erase(it++);
      delete item;   // Remove item itself
    } else {
      ++it;
    }
  }

  // Commit in order
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;

    if (item->IsReadyForCommit()) {
      item->SendCommit();
    } else {
      break;
    }
  }
}

void PushManager::ResetAllItems() {
  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;
    item->ResetPushItem();
  }
}

au::tables::Table *PushManager::getTableOfItems() {
  au::tables::Table *table = new au::tables::Table("Id|Buffer|Worker|Time|State");

  table->setTitle("push operations");

  au::map<size_t, PushItem>::iterator it;
  for (it = items_.begin(); it != items_.end(); it++) {
    PushItem *item = it->second;

    au::StringVector values;
    values.Push(item->push_id());
    values.Push(item->str_buffer_info());
    values.Push(item->worker_id_);
    values.Push(au::str_time(item->time()));
    values.Push(item->str());

    table->addRow(values);
  }

  return table;
}

size_t PushManager::get_num_items() {
  return items_.size();
}
}
