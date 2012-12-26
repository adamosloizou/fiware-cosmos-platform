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


#include "au/log/LogContainer.h"  // Own interface


#include "au/log/LogQuery.h"

namespace au {
LogContainer::LogContainer() : token_("LogContainer") {
  max_num_logs_  = 100000000;      // Limit of logs
}

void LogContainer::Push(au::SharedPointer<Log> log) {
  au::TokenTaker tt(&token_);

  // Put in the list & check not too many logs are inside
  logs_.push_back(log);
  while (logs_.size() > max_num_logs_) {
    logs_.pop_front();  // Limit number of logs in memory
  }
  // Process in the counter to get statistics
  log_counter_.Process(log);
}

void LogContainer::Clear() {
  au::TokenTaker tt(&token_);

  logs_.clear();
  log_counter_.Clear();
}

size_t LogContainer::size() const {
  au::TokenTaker tt(&token_);

  return logs_.size();
}

void LogContainer::Process(LogQuery *log_query) {
  std::list<LogPointer>::iterator iter;
  for (iter = logs_.begin(); iter != logs_.end(); ++iter) {
    log_query->Process(*iter);
  }
}
}