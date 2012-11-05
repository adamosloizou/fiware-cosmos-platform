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

#ifndef _H_STREAM_CONNECTOR_LOG_MANAGER
#define _H_STREAM_CONNECTOR_LOG_MANAGER

#include "au/containers/Queue.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/tables/Table.h"

namespace stream_connector {
// Log system for channel - item - connection

class Log {
public:

  Log(std::string name, std::string type, std::string message);

  std::string getNameAndMessage();
  std::string getType();
  void writeOnScreen();

  time_t time() const {
    return time_;
  }

  std::string type() const {
    return type_;
  }

  std::string name() const {
    return name_;
  }

  std::string message() const {
    return message_;
  }

private:

  time_t time_;
  std::string type_;
  std::string name_;
  std::string message_;
};


class LogManager {
public:

  LogManager();
  void log(au::SharedPointer<Log> log);

  au::tables::Table *getLogsTable(size_t limit);

private:

  au::Token token_;
  au::Queue<Log> logs_;
};
}  // End of namespace stream_connector

#endif  // ifndef _H_STREAM_CONNECTOR_LOG_MANAGER
