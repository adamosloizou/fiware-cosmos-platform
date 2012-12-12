/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "samson/network/CommonNetwork.h"  // Own interface

#include <set>
#include <vector>

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"

#include "samson/common/Logs.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/gpb_operations.h"
#include "samson/network/NetworkConnection.h"

namespace samson {
CommonNetwork::CommonNetwork(NodeIdentifier my_node_identifier) :
  token_("CommonNetwork") {
  // Identify myself
  node_identifier_ = my_node_identifier;

  LOG_V(logs.network_connection, ("CommonNetwork created for node %s", node_identifier_.str().c_str()));

  // No cluster information at the moment
  cluster_information_ = NULL;

  // Listen and create notifications for network manager review
  listen("notification_network_manager_review");
  engine::Engine::shared()->notify(new engine::Notification("notification_network_manager_review"), 1);

  listen("send_to_all_delilahs");   // Listen notification to send packets
  listen(notification_send_packet);   // Listen to send packet
}

void CommonNetwork::remove_cluster_information() {
  cluster_information_.Reset(NULL);
  review_connections();
}

void CommonNetwork::set_cluster_information(au::SharedPointer<gpb::ClusterInfo> cluster_information) {
  au::TokenTaker tt(&token_);

  if (cluster_information == NULL) {
    return;
  }

  // Not update if we have a newer version of this cluster info
  if (cluster_information_ != NULL) {
    if (cluster_information_->version() >= cluster_information->version()) {
      return;
    }
  }

  // Replace intern cluster information
  engine::Engine::shared()->notify(new engine::Notification("notification_cluster_info_changed"));
  cluster_information_ = cluster_information;

  // If I am a worker, update all my delilah connections
  if (node_identifier_.node_type == WorkerNode) {
    PacketPointer ci_packet(new Packet(Message::ClusterInfoUpdate));
    ci_packet->message->mutable_cluster_info()->CopyFrom(*cluster_information_);
    ci_packet->from = node_identifier_;

    LOG_V(logs.network_connection, ("Sending cluster info version %lu update packages to all delilahs",
                                    cluster_information_->version()));

    SendToAllDelilahs(ci_packet);
  }

  // Review connections ( based on this new cluster setup )
  review_connections();
}

void CommonNetwork::review_connections() {
  au::TokenTaker tt(&token_);

  NetworkManager::Review();   // Check pending packets to be removed after 1 minute disconnected

  // Check workers to be connected to
  if (cluster_information_ == NULL) {
    LOG_V(logs.network_connection, ("No cluster information available. Not possible to review connections"));
    return;
  }

  int num_workers = cluster_information_->workers_size();
  LOG_V(logs.network_connection, ("Review connections to all workers (cluster with %d workers)", num_workers));
  for (int i = 0; i < num_workers; i++) {
    size_t worker_id = cluster_information_->workers(i).worker_id();
    std::string name = NodeIdentifier(WorkerNode, worker_id).getCodeName();
    std::string host = cluster_information_->workers(i).worker_info().host();
    int port = cluster_information_->workers(i).worker_info().port();

    LOG_V(logs.network_connection, ("Checking connection %s worker_id=%lu at %s:%d"
                                    , name.c_str()
                                    , worker_id
                                    , host.c_str()
                                    , port));

    // Discard for lower id or me ( if I am a worker )
    if (node_identifier_.node_type == WorkerNode) {
      if (node_identifier_.id < worker_id) {
        LOG_V(logs.network_connection,
              ("Not adding connection with worker %lu (%s:%d) since my id is lower", worker_id, host.c_str(), port));
        continue;
      }
      if (node_identifier_.id == worker_id) {
        LOG_V(logs.network_connection,
              ("Not adding connection with worker %lu (%s:%d) since this is me", worker_id, host.c_str(), port));
        continue;
      }
    }

    if (NetworkManager::IsConnected(name)) {
      LOG_V(logs.network_connection, ("Worker %lu ( %s ) already connected.", worker_id, name.c_str()));
    } else {
      LOG_V(logs.network_connection, ("Worker %lu ( %s ) not connected. Trying to connect to %s:%d..."
                                      , worker_id
                                      , name.c_str()
                                      , host.c_str()
                                      , port));

      addWorkerConnection(worker_id, host, port);
    }
  }

  // Close old connections
  std::vector<std::string> connection_names = GetAllConnectionNames();

  for (size_t i = 0; i < connection_names.size(); i++) {
    std::string connection_name = connection_names[i];

    // Get the node identifier associated to this connection ( inspecting the name )
    NodeIdentifier node_identifier(connection_name);

    if (node_identifier.node_type == WorkerNode) {
      if (!isWorkerIncluded(cluster_information_.shared_object(), node_identifier.id)) {
        LOG_V(logs.network_connection,
              ("Removing connection %s since this worker is not included in the cluster any more"
               , connection_name.c_str()));
        Remove(connection_names[i]);
      }
    }
  }
}

void CommonNetwork::notify(engine::Notification *notification) {
  if (notification->isName(notification_send_packet)) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet> ("packet");

    if (packet == NULL) {
      LOG_SW(("Notification notification_send_packet without packet"));
      return;
    }

    // Send real packet
    Send(packet);
  }

  if (notification->isName("send_to_all_delilahs")) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet> ("packet");

    if (packet != NULL) {
      SendToAllDelilahs(packet);
    } else {
      LOG_SW(("Notification send_to_all_delilahs without packet"));
    }
    return;
  }

  review_connections();
}

std::string CommonNetwork::str() {
  std::ostringstream output;

  output << "-----------------------------------------------\n";
  output << "NetworkManager (" << node_identifier_.str() << ") \n";
  output << "-----------------------------------------------\n";
  output << NetworkManager::str();
  output << "-----------------------------------------------\n";

  return output.str();
}

// Add a new connection to a worker

Status CommonNetwork::addWorkerConnection(size_t worker_id, std::string host, int port) {
  NodeIdentifier node_identifier = NodeIdentifier(WorkerNode, worker_id);

  LOG_V(logs.network_connection, ("**** Adding connection for worker %lu at %s:%d"
                                  , worker_id
                                  , host.c_str()
                                  , port));

  // Name for this connection
  std::string name = node_identifier.getCodeName();

  // Check if we already have this connection
  if (NetworkManager::IsConnected(name)) {
    return Error;
  }

  // Init connection
  au::SocketConnection *socket_connection;
  au::Status s = au::SocketConnection::Create(host, port, &socket_connection);

  // If there is an error, just return the error
  if (s != au::OK) {
    LOG_SW(("Unable to connect with %s:%d (%s)", host.c_str(), port, au::status(s)));
    return Error;   // Generic error
  }

  // Sync send hello message
  // Sent hello packages to make sure the other end identify ourselves
  au::SharedPointer<Packet> hello_packet(new Packet(Message::Hello));
  gpb::Hello *pb_hello = hello_packet->message->mutable_hello();
  node_identifier_.fill(pb_hello->mutable_node_identifier());

  if (hello_packet->write(socket_connection, NULL) != au::OK) {
    LOG_SW(("Not possible to send hello message to %s:%d (%s)", host.c_str(), port, au::status(s)));
    return Error;
  }

  // Create network connection with this socket
  NetworkManager::AddConnection(node_identifier, socket_connection);

  return OK;
}

void CommonNetwork::Send(const PacketPointer& packet) {
  if (packet->msgCode == Message::Hello) {
    LOG_W(logs.worker, ("Not allowed to send hello message outside initial handshake"));
    return;
  }

  // Set me as "from" identifier
  packet->from = node_identifier_;

  if (packet->to == node_identifier_) {
    schedule_receive(packet);    // Local loop
    return;
  }

  // Push a packet to a connection or eventually keep in queue to see if it connects back soon ;)
  LOG_V(logs.out_messages, ("Sending packet %s to %s"
                            , packet->str().c_str()
                            , packet->to.str().c_str()));
  NetworkManager::Send(packet);
}

void CommonNetwork::SendToAllWorkers(const PacketPointer& packet, std::set<size_t>& workers) {
  au::TokenTaker tt(&token_);

  for (int i = 0; i < cluster_information_->workers_size(); i++) {
    size_t worker_id = cluster_information_->workers(i).worker_id();
    PacketPointer new_paket(new Packet(packet.shared_object()));
    new_paket->to = NodeIdentifier(WorkerNode, worker_id);
    Send(new_paket);

    workers.insert(worker_id);
  }
}

// Receive a packet from a connection
void CommonNetwork::receive(NetworkConnection *connection, const PacketPointer& packet) {
  LOG_V(logs.network_connection, ("RECEIVED from %s: PACKET %s\n", connection->name().c_str(), packet->str().c_str()));

  if (packet->msgCode == Message::Hello) {
    LOG_SW(("Received a hello packet once connection is identified. Ignoring..."));
    return;   // Ignore hello message here
  }

  if (packet->msgCode == Message::ClusterInfoUpdate) {
    if (node_identifier_.node_type == WorkerNode) {
      LOG_SW(("ClusterInfoUpdate packet received at a worker node from connection %s. Closing connection"
              , connection->node_identifier().str().c_str()));
      connection->Close();
      return;
    }   // This is managed as a normal message in delilah
  }
  // Check we do now receive messages from unidenfitied node elements
  if (connection->node_identifier().node_type == UnknownNode) {
    LOG_SW(("Packet %s received from a non-identified node %s. Closing connection"
            , packet->str().c_str()
            , connection->node_identifier().str().c_str()));
    connection->Close();
    return;
  }

  schedule_receive(packet);
}

NodeIdentifier CommonNetwork::getMynodeIdentifier() {
  return node_identifier_;
}

au::tables::Table *CommonNetwork::getClusterConnectionsTable() {
  au::TokenTaker tt(&token_);

  au::tables::Table *table = new au::tables::Table(au::StringVector("Worker", "Host", "Status"));

  if (cluster_information_ != NULL) {
    table->setTitle("Cluster");

    for (int i = 0; i < cluster_information_->workers_size(); i++) {
      size_t worker_id = cluster_information_->workers(i).worker_id();
      std::string name = NodeIdentifier(WorkerNode, worker_id).getCodeName();
      std::string host = cluster_information_->workers(i).worker_info().host();
      int port = cluster_information_->workers(i).worker_info().port();

      au::StringVector values;
      values.push_back(au::str("%lu", worker_id));
      values.push_back(au::str("%s:%d", host.c_str(), port));

      NodeIdentifier ni = NodeIdentifier(WorkerNode, worker_id);
      std::string connection_name = ni.getCodeName();

      if (ni == node_identifier_) {
        values.push_back("me");
      } else {
        values.push_back(NetworkManager::GetStatusForConnection(connection_name));
      }
      table->addRow(values);
    }
  } else {
    table->setTitle("Not cluster defined");
  }

  return table;
}

size_t CommonNetwork::get_rate_in() {
  return NetworkManager::GetRateIn();
}

size_t CommonNetwork::get_rate_out() {
  return NetworkManager::GetRateOut();
}

void CommonNetwork::schedule_receive(PacketPointer packet) {
  // Create a notification containing the packet we have just received
  engine::Notification *notification = new engine::Notification(notification_packet_received);

  notification->dictionary().Set<Packet> ("packet", packet);
  engine::Engine::shared()->notify(notification);
}

size_t CommonNetwork::cluster_information_version() {
  if (cluster_information_ == NULL) {
    return static_cast<size_t>(-1);
  }
  return cluster_information_->version();
}

std::string CommonNetwork::getClusterConnectionStr() {
  if (cluster_information_ == NULL) {
    return "Disconnected";
  } else {
    return au::str("Cluster v %d / %d nodes"
                   , static_cast<int>(cluster_information_->version())
                   , static_cast<int>(cluster_information_->workers_size()));
  }
}

std::string CommonNetwork::getClusterSetupStr() {
  if (cluster_information_ == NULL) {
    return "Delilah is not connected to any SAMSON cluster\n";
  }

  au::tables::Table table("Worker|Host|Connected");
  table.setTitle(au::str("Cluster setup ( version %lu )", cluster_information_->version()));
  for (int i = 0; i < cluster_information_->workers_size(); i++) {
    size_t worker_id = cluster_information_->workers(i).worker_id();

    au::StringVector values;
    values.Push(worker_id);

    std::string host = au::str("%s:%d (rest :%d)", cluster_information_->workers(i).worker_info().host().c_str(),
                               cluster_information_->workers(i).worker_info().port(),
                               cluster_information_->workers(i).worker_info().port_web());
    values.push_back(host);

    if (IsConnected(NodeIdentifier(WorkerNode, worker_id).getCodeName())) {
      values.push_back("yes");
    } else {
      values.push_back("no");
    }
    table.addRow(values);
  }
  return table.str();
}

std::string CommonNetwork::getClusterAssignationStr() {
  au::tables::Table table("ProcessUnit|Worker|Replicas");

  table.setTitle("Assignation");
  for (int i = 0; i < cluster_information_->process_units_size(); i++) {
    int hg_begin = cluster_information_->process_units(i).hg_begin();
    int hg_end = cluster_information_->process_units(i).hg_end();

    size_t worker_id = cluster_information_->process_units(i).worker_id();

    au::StringVector values;
    values.push_back(au::str("%d-%d", hg_begin, hg_end));
    values.Push(worker_id);

    std::ostringstream replicas;

    for (int r = 0; r < cluster_information_->process_units(i).replica_worker_id_size(); r++) {
      replicas << cluster_information_->process_units(i).replica_worker_id(r) << " ";
    }

    values.Push(replicas.str());

    table.addRow(values);
  }
  return table.str();
}

// Get a cluster information packet
PacketPointer CommonNetwork::getClusterInfoPacket() {
  au::TokenTaker tt(&token_);

  // Update cluster information to delilah....
  PacketPointer ci_packet(new Packet(Message::ClusterInfoUpdate));

  ci_packet->message->mutable_cluster_info()->CopyFrom(*cluster_information_);
  return ci_packet;
}

// Get my node identifier
NodeIdentifier CommonNetwork::node_identifier() {
  return node_identifier_;
}
}
