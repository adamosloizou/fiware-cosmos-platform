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

/* ****************************************************************************
*
* FILE            SamsonClient .cpp
*
* AUTHOR          Andreu Urruela
*
* PROJECT         Samson
*
* DATE            7/14/11
*
* DESCRIPTION
*
* ****************************************************************************/

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"         // LmtModuleManager
#include "parseArgs/paConfig.h"

#include "au/ThreadManager.h"
#include "au/log/LogMain.h"
#include "au/string/StringUtilities.h"  // au::Format

#include "engine/DiskManager.h"         // engine::DiskManager
#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"       // engine::MemoryManager
#include "engine/Notification.h"        // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager


#include "samson/common/Logs.h"
#include "samson/common/NotificationMessages.h"  // notification_review_timeOut_SamsonPushBuffer
#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
#include "samson/common/samsonDirectories.h"
#include "samson/common/samsonVars.h"

#include "samson/module/ModulesManager.h"       // samson::ModulesManager

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Packet.h"


#include "SamsonClient.h"                       // Own interface

#include "samson/delilah/DataSource.h"          // samson::DataSource

namespace samson {
SamsonClient::SamsonClient(std::string connection_type) {
  connection_type_ = connection_type;
  delilah_ = new Delilah("client");
  delilah_->data_receiver_interface = this;  // By default, I am the default receiver

  LM_V(("SamsonClient: Delilah client with id %s", au::code64_str(delilah_->delilah_id()).c_str()));
}

bool SamsonClient::connect(const std::vector<std::string>& hosts) {
  // Try connection with all provided hosts
  for (size_t i = 0; i < hosts.size(); ++i) {
    if (connect(hosts[i])) {
      return true;
    }
  }

  return false;
}

bool SamsonClient::connect(const std::string& host) {
  au::ErrorManager error;
  bool c = delilah_->connect(host, &error);

  if (error.HasErrors()) {
    LOG_SW(("Unable to connect to %s: %s", host.c_str(), error.GetLastError().c_str()));
  }
  return c;
}

SamsonClient::~SamsonClient() {
  // Remove delilah instance
  delete delilah_;
}

void SamsonClient::set_receiver_interface(DelilahLiveDataReceiverInterface *interface) {
  delilah_->data_receiver_interface = interface;
}

void SamsonClient::general_init(size_t memory, size_t load_buffer_size) {
  std::string samson_home    = SAMSON_HOME_DEFAULT;
  std::string samson_working = SAMSON_WORKING_DEFAULT;

  char *env_samson_working = getenv("SAMSON_WORKING");
  char *env_samson_home = getenv("SAMSON_HOME");

  if (env_samson_working) {
    samson_working = env_samson_working;
  }
  if (env_samson_home) {
    samson_home = env_samson_home;  // Init setup
  }

  au::Singleton<samson::SamsonSetup>::shared()->SetWorkerDirectories(samson_home, samson_working);

  // Change the values for this parameters
  au::Singleton<samson::SamsonSetup>::shared()->Set("general.memory", au::str("%lu", memory));
  au::Singleton<samson::SamsonSetup>::shared()->Set("load.buffer_size", au::str("%lu", load_buffer_size));

  // Init Engine, DiskManager, ProcessManager and Memory manager
  int num_cores = au::Singleton<samson::SamsonSetup>::shared()->GetInt("general.num_processess");
  engine::Engine::InitEngine(num_cores, memory, 1);
}

void SamsonClient::general_close() {
  // Wait all threads to finish
  au::Singleton<au::ThreadManager>::shared()->wait("SamsonClient");

  // Destroy all singletons
  au::Singleton<ModulesManager>::DestroySingleton();

  // Close engine
  engine::Engine::StopEngine();
}

void SamsonClient::receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
  // Accumulate buffers of data in this buffer container
  buffer_container_.Push(queue, buffer);
}

size_t SamsonClient::push(engine::BufferPointer buffer, const std::string& queue) {
  LOG_V(logs.client, ("SamsonClient: Pushing buffer %s to %lu queues", au::str(buffer->size()).c_str(), queue.size()));

  std::vector<std::string> queues;
  queues.push_back(queue);
  return push(buffer, queues);
}

size_t SamsonClient::push(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  if (buffer == NULL) {
    return 0;
  }

  push_rate_.Push(buffer->size());     // Update statistics
  return delilah_->push_txt(buffer, queues);
}

bool SamsonClient::isFinishedPushingData() {
  return (delilah_->GetPendingSizeToPush() == 0);
}

void SamsonClient::waitFinishPushingData() {
  while (true) {
    if (isFinishedPushingData()) {
      return;
    } else {
      usleep(200000);
    }
    LOG_SV(("Waiting delilah to finish push process. Still pending %s",
            au::str(delilah_->GetPendingSizeToPush()).c_str()));
  }
}

void SamsonClient::connect_to_queue(std::string queue, bool flag_new, bool flag_remove) {
  // Update queue management in samsonClient in the same way as in delilah.
  // Old strategy with delilah_->sendWorkerCommand("connect_to_queue") was not working
  size_t id = delilah_->AddPopComponent(queue, "", false, false);

  LOG_V(logs.delilah_components, ("AddPopComponent for queue:'%s' returned id:%lu", queue.c_str(), id));
}

SamsonClientBlockInterface *SamsonClient::getNextBlock(std::string queue) {
  // Pop intern element from the reception queue
  engine::BufferPointer intern_buffer = buffer_container_.Pop(queue);

  if (intern_buffer != NULL) {
    return new SamsonClientBlock(intern_buffer);
  } else {
    return NULL;
  }
}

bool SamsonClient::connection_ready() {
  return delilah_->isConnected();
}

size_t SamsonClient::GetPendingSizeToPush() {
  return delilah_->GetPendingSizeToPush();
}

void SamsonClient::waitUntilFinish() {
  waitFinishPushingData();
}
}
