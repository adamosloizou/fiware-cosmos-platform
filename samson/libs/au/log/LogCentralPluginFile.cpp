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


#include "au/log/LogCentralPluginFile.h"  // Own interface

namespace au {
LogCentralPluginFile::LogCentralPluginFile(std::string file_name) : LogCentralPlugin("file") {
  set_local_file(file_name);
}

void LogCentralPluginFile::set_local_file(const std::string& file_name) {
  // Close previous if any
  local_file_descriptor_.Reset();

  // Save te name of the local file
  local_file_ = file_name;

  // Open local file ( if possible )
  int fd = open(local_file_.c_str(), O_WRONLY | O_CREAT, 0644);
  if (fd >= 0) {
    local_file_descriptor_.Reset(new FileDescriptor("local_log", fd));
  } else {
    local_file_descriptor_.Reset();
    // LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file_.c_str()));
  }
}

void LogCentralPluginFile::Emit(SharedPointer<Log> log) {
  // It was not possible, so use disk
  if (local_file_descriptor_ != NULL) {
    if (!log->Write(local_file_descriptor_.shared_object())) {
      // Local log?
    }
  }
}

std::string LogCentralPluginFile::status() {
  return au::str("LocalFile: %s", local_file_.c_str());
}
}