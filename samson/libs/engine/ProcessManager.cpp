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

#include "engine/ProcessManager.h"

#include <sys/time.h>
#include <time.h>

#include "au/ThreadManager.h"
#include "au/containers/SharedPointer.h"
#include "au/log/LogCommon.h"
#include "au/mutex/TokenTaker.h"   // au::TokenTake
#include "au/singleton/Singleton.h"
#include "au/string/Descriptors.h"  // au::Descriptors
#include "au/string/xml.h"         // au::xml...
#include "engine/DiskOperation.h"  // engine::DiskOperation
#include "engine/Engine.h"         // engine::Engine
#include "engine/EngineElement.h"  // engine::EngineElement
#include "engine/Logs.h"
#include "engine/Notification.h"   // engine::Notification
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement
#include "engine/ProcessItem.h"    // engine::ProcessItem
#include "logMsg/logMsg.h"         // LM_X
#include "logMsg/traceLevels.h"

namespace engine {
void *ProcessManager_run_worker(void *p) {
  ProcessManager *process_manager = reinterpret_cast<ProcessManager *>(p);

  std::string thread_description = au::GetThreadId(pthread_self());

  LOG_D(logs.process_manager,  ("Thread for ProcessManager worker %s", thread_description.c_str()));

  process_manager->run_worker();
  return NULL;
}

ProcessManager::ProcessManager(int max_num_procesors) :
  token_("engine::ProcessManager"), num_procesors_(0), max_num_procesors_(max_num_procesors), stopped_(false) {
}

ProcessManager::~ProcessManager() {
}

void ProcessManager::Stop() {
  {
    au::TokenTaker tt(&token_);
    items_.Clear();
  }
  stopped_ = true;   // Flag to notify all background process to finish

  // Wait all background workers
  au::Cronometer cronometer;
  while (true) {
    if (num_procesors_ == 0) {
      return;
    }
    usleep(100000);
    {
      // We are getting blocked here, because run_worker() is slept waiting for items
      au::TokenTaker tt(&token_);
      tt.WakeUpAll();
    }
    if (cronometer.seconds() > 1) {
      cronometer.Reset();
      LOG_SW(("Waiting for background threads of engine::ProcessManager, still %d num_procesors_", num_procesors_));
    }
  }
}

void ProcessManager::notify(Notification *notification) {
  LM_E(("Wrong notification at ProcessManager [Listener %lu] %s",
        engine_id(), notification->GetDescription().c_str()));
}

void ProcessManager::Add(au::SharedPointer<ProcessItem> item, size_t listenerId) {
  // Protect multi-thread access
  au::TokenTaker tt(&token_);

  // Make sure items always come with at least one listener id
  item->AddListener(listenerId);

  // Insert in the list of items
  items_.Push(item);

  // Check number of background processors
  if (!stopped_) {
    while (num_procesors_ < max_num_procesors_) {
      pthread_t t;
      au::Singleton<au::ThreadManager>::shared()->AddThread("background_worker", &t, 0, ProcessManager_run_worker, this);
      num_procesors_++;
    }
  }

  // Wake up all background threads if necessary
  tt.WakeUpAll();
}

void ProcessManager::Cancel(au::SharedPointer<ProcessItem> item) {
  // We can only cancel items if they are in the list of pending items
  if (items_.Contains(item)) {
    // If still in the queue of items
    items_.ExtractAll(item);

    // Set this process with an error
    item->error().set("ProcessItem canceled");

    // Notify this using the notification Mechanism
    Notification *notification = new Notification(notification_process_request_response);
    notification->AddEngineListeners(item->listeners());

    // Add the item as an object in the internal dictionary
    notification->dictionary().Set<ProcessItem> ("process_item", item);

    notification->environment().Add(item->environment());
    notification->environment().Set("error", "Canceled");

    Engine::shared()->notify(notification);
  }
}

int ProcessManager::num_used_procesors() {
  au::TokenTaker tt(&token_);

  return static_cast<int>(running_items_.size());
}

int ProcessManager::max_num_procesors() {
  return max_num_procesors_;
}

void ProcessManager::run_worker() {
  while (true) {
    // Check if too many background workers are running
    {
      au::TokenTaker tt(&token_);
      if (stopped_ || (num_procesors_ > max_num_procesors_)) {
        num_procesors_--;
        return;
      }
    }

    // Take the next item to be executed
    au::SharedPointer<ProcessItem> item;
    {
      au::TokenTaker tt(&token_);
      item = items_.Pop();
      if (item != NULL) {
        running_items_.Insert(item);
      } else {
        tt.Stop();   // Block until main thread wake me up
        continue;
      }
    }

    // Init cronometer for this process item
    item->StartActivity();

    // Run the process
    item->run();

    // Stop cronometer for this process item
    item->StopActivity();

    // Remove from the box of running elements
    {
      au::TokenTaker tt(&token_);
      running_items_.Erase(item);
    }

    // Notify this using the notification Mechanism
    Notification *notification = new Notification(notification_process_request_response);

    // Add item itself as an object inside the notification
    notification->dictionary().Set<ProcessItem> ("process_item", item);

    // Add targets to be notified
    notification->AddEngineListeners(item->listeners());

    // Add enviroment variables
    notification->environment().Add(item->environment());

    // Extra error environment if necessary
    if (item->error().IsActivated()) {
      notification->environment().Set("error", item->error().GetMessage());   // Add the notification to the main engine
    }
    Engine::shared()->notify(notification);
  }
}
}
