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
* FILE            Object.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* NotificationListener is the base class for any object that should receive notification from engine
*      It can receive notifications
*      It can listen notifications channels
*
* ****************************************************************************/

#ifndef _H_ENGINE_OBJECT
#define _H_ENGINE_OBJECT

#include <string>   // size_t

namespace engine {
class Notification;

/**
 * Base class for any element in the engine library to receive notifications
 */

class NotificationListener {
public:

  // Constructor & Destructor
  NotificationListener();
  virtual ~NotificationListener();

  // Accesors
  size_t engine_id();

protected:

  // Start listening a particular notification
  void listen(const char *notification_name);

  // Method to receive a particular notification
  virtual void notify(Notification *notification);

private:

  // Unique identifier of this listener
  size_t engine_id_;

  // Class the managers this objects
  friend class NotificationListenersManager;
};
}

#endif  // ifndef _H_ENGINE_OBJECT
