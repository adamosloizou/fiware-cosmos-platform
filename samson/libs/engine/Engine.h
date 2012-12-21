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
* FILE            Engine.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Engine: Singlenton object with all the main funcitonalities of the engine library
*
* ****************************************************************************/

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <math.h>
#include <pthread.h>

#include <iostream>                              // std::cout
#include <list>
#include <set>                                   // std::set
#include <string>

#include "au/statistics/ActivityMonitor.h"
#include "au/tables/Table.h"

#include "au/Thread.h"
#include "au/ThreadManager.h"
#include "au/containers/list.h"                  // au::list
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/mutex/Token.h"                      // au::Token
#include "au/mutex/TokenTaker.h"
#include "au/statistics/Cronometer.h"            // au::Cronometer

#include "engine/EngineElementCollection.h"
#include "engine/NotificationListener.h"         // engine::EngineNotification
#include "engine/NotificationListenersManager.h"  // engine::NotificationListenersManager


namespace au {
class Error;
class Token;
}

/**
 * \brief Namespace for engine library
 * @author Andreu Urruela, Gregorio Escalada & Ken Zangelin
 */

namespace engine {
class EngineElement;
class ProcessItem;
class DiskOperation;
class DiskManager;
class ProcessManager;
class MemoryManager;
class Notification;

// ---------------------------------------------------
//
// EngineElementCollection
//
// Main Engine platform
// ---------------------------------------------------


class Engine : public au::Thread {
public:

  virtual ~Engine();

  // Static methods to init and close engine ( all included )
  static void InitEngine(int num_cores, size_t memory, int num_disk_operations);
  static void StopEngine();
  static bool IsEngineWorking();

  // Accessorts to main components
  static Engine *shared();
  static DiskManager *disk_manager();
  static MemoryManager *memory_manager();
  static ProcessManager *process_manager();

  // Function to add a simple foreground tasks to this runloop
  void Add(EngineElement *element);

  // Methods to add notifictions
  void notify(Notification *notification);
  void notify(Notification *notification, int seconds);   // Repeated notification
  void notify_extra(Notification *notification);

  // Debug information
  int GetNumElementsInEngineStack();
  double GetMaxWaitingTimeInEngineStack();
  std::string GetTableOfEngineElements();      // Get information about current elements in engine
  au::statistics::ActivityMonitor *activity_monitor();      // Return activity monitor to print some statistics

private:


  Engine();     // Private constructor ( see Init static method )

  /**
   *
   * RunThread
   *
   * \brief Main loop to process engine-elements ( normal, periodic and extra )
   *
   * Try to get the next element in the repeat_elements list
   * if not there , try normal elements...
   * if not, run extra elements and loop again...
   *
   */
  void RunThread();

  // Methods to register and unregister listsners ( used from class NotificationListener )
  void AddListener(NotificationListener *object);
  void AddListenerToChannel(NotificationListener *object, const char *channel);
  void RemoveListener(NotificationListener *object);

  // Really sent a notification to required targets
  void Send(Notification *notification);

  // Run a particular engine element
  void RunElement(EngineElement *running_element);
  void InternRunElement(EngineElement *running_element);

  // Common engine instance
  static Engine *engine_;
  static MemoryManager *memory_manager_;
  static DiskManager *disk_manager_;
  static ProcessManager *process_manager_;

  // Collection of Engine Elements to be executed
  EngineElementCollection engine_element_collection_;

  // Management of listeners for notifications
  NotificationListenersManager notification_listeners_manager_;

  // Statistics
  au::statistics::ActivityMonitor activity_monitor_;

  // Internal counter for debuggin
  size_t counter_;

  // Total up time
  au::Cronometer uptime_;

  friend class NotificationListener;
  friend class NotificationElement;
};

// Handy methods to add notifications
void notify(const char *notification_name);
void notify(const char *notification_name, double period);
void notify_extra(const char *notification_name);
}
#endif  // ifndef _H_SAMSON_ENGINE
