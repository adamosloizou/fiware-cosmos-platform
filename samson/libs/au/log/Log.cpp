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
// Own interface
#include "au/log/Log.h"

#include "au/log/LogCentral.h"


namespace au {
const char *log_reseved_words[] =
{ "node",       "host", "type",   "channel",  "pid",       "tid",
  "DATE"
  ,             "date", "TIME",   "time",     "timestamp",
  "time_unix",
  "line",       "exec",
  "exec_short",
  "file"
  ,             "text", "text80", "function", NULL };


void Log::Set(const std::string& field_name, const std::string& field_value) {
  if (fields_.find(field_name) == fields_.end()) {
    fields_.insert(std::pair<std::string, std::string>(field_name, field_value));
  }
}

bool Log::Read(au::FileDescriptor *fd) {
  // LM_V(("Reading log header of %lu bytes" ,sizeof(LogHeader)));
  LogHeader header;
  au::Status s = fd->partRead(&header, sizeof(LogHeader), "log header", 300);

  if (s != au::OK) {
    return false;   // Just quit
  }
  if (!header.checkMagicNumber()) {
    return false;
  }



  // Read fixed data
  // LM_V(("Reading fix log data of %lu bytes" ,sizeof(LogData)));
  s = fd->partRead(&log_data_, sizeof(LogData), "log data", 300);

  if (s != au::OK) {
    return false;   // Just quit
  }
  // Read strings
  size_t string_length = header.dataLen - sizeof(LogData);
  au::TemporalBuffer buffer(string_length);
  // LM_V(("Reading strings data of %lu bytes" ,string_length ));
  s = fd->partRead(buffer.data(), string_length, "log_strings", 100);
  if (s != au::OK) {
    return false;
  }

  // Process strings
  AddStrings(buffer.data(), string_length);
  return true;
}

bool Log::Write(au::FileDescriptor *fd) {
  // LM_V(("Writing %s" , str().c_str() ));
  LogHeader header;

  header.setMagicNumber();
  size_t strings_size = GetStringsSize();
  header.dataLen = sizeof(LogData) + strings_size;

  // Total message to be writted
  TemporalBuffer buffer(sizeof(LogHeader) + sizeof(LogData) + strings_size);

  size_t offset = 0;
  memcpy(buffer.data() + offset, &header, sizeof(LogHeader));
  offset += sizeof(LogHeader);
  memcpy(buffer.data() + offset, &log_data_, sizeof(LogData));
  offset += sizeof(LogData);
  CopyStrings(buffer.data() + offset);

  // Write log at once
  au::Status s = fd->partWrite(buffer.data(), buffer.size(), "log", 1, 1, 0);
  return (s == au::OK);
}

std::string Log::str() const {
  std::ostringstream output;

  output << "Line:" << log_data_.line << ",";
  output << au::str("Channel:%d", log_data_.channel) << ",";
  output << "Time:" << log_data_.tv.tv_sec << "(" << log_data_.tv.tv_usec << "),";
  output << "TimeZone:" << log_data_.timezone << ",";

  output << "[ ";
  std::map<std::string, std::string>::const_iterator it_fields;
  for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
    output << it_fields->first << ":" << it_fields->second << " ";
  }
  output << "}";

  return output.str();
}

bool Log::Match(Pattern& pattern) const {
  std::map<std::string, std::string>::const_iterator it_fields;
  for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
    std::string value = it_fields->second;
    if (pattern.match(value)) {
      return true;
    }
  }
  return false;
}

bool Log::Match(SimplePattern& pattern) const {
  std::map<std::string, std::string>::const_iterator it_fields;
  for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
    std::string value = it_fields->second;
    if (pattern.match(value)) {
      return true;
    }
  }
  return false;
}

int Log::channel() const {
  return log_data_.channel;
}

int Log::level() const {
  return log_data_.level;
}

// Get information from this log
std::string Log::Get(std::string name) const {
  if (name == "type") {
    return GetLogLevel(log_data_.level);
  }

  if (name == "host") {
    return Get("host", "");
  }
  if (name == "channel") {
    return Get("channel_name");
  }

  if (name == "pid") {
    return au::str("%d", log_data_.pid);
  }
  if (name == "tid") {
    return au::str("%d", log_data_.tid);
  }
  if (name == "DATE") {
    char line_tmp[80];
    struct tm tmP;
    gmtime_r(&log_data_.tv.tv_sec, &tmP);
    strftime(line_tmp, 80, DATE_FORMAT, &tmP);
    return std::string(line_tmp);
  }
  if (name == "date") {
    struct tm timeinfo;
    char buffer_time[1024];
    localtime_r(&log_data_.tv.tv_sec, &timeinfo);
    strftime(buffer_time, 1024, "%d/%m/%Y", &timeinfo);
    return std::string(buffer_time);
  }
  if (name == "TIME") {
    struct tm timeinfo;
    char buffer_time[1024];
    localtime_r(&log_data_.tv.tv_sec, &timeinfo);
    strftime(buffer_time, 1024, "%X", &timeinfo);
    return std::string(buffer_time) + au::str("(%d)", log_data_.tv.tv_usec);
  }

  if (name == "time") {
    struct tm timeinfo;
    char buffer_time[1024];
    localtime_r(&log_data_.tv.tv_sec, &timeinfo);
    strftime(buffer_time, 1024, "%X", &timeinfo);
    return std::string(buffer_time);
  }

  if (name == "timestamp") {
    return Get("date") + " " + Get("time");
  }

  if (name == "time_unix") {
    return au::str("%lu", log_data_.tv.tv_sec);
  }

  if (name == "line") {
    return au::str("%d", log_data_.line);
  }

  if (name == "text80") {
    std::string t = Get("text", "");
    if (t.length() > 80) {
      return t.substr(0, 80);
    } else {
      return t;
    }
  }

  if (name == "exec_short") {
    std::string exec = Get("exec");
    size_t pos = exec.find_last_of("/");
    if (pos == std::string::npos) {
      return exec;
    }
    return exec.substr(pos + 1);
  }

  // General look up in the strings...
  std::map<std::string, std::string>::const_iterator it_fields = fields_.find(name);
  if (it_fields != fields_.end()) {
    return it_fields->second;
  }

  // If not recognized as a field, just return the name
  // This is useful for formatting a line
  return name;
}

std::string Log::Get(const std::string& name, const std::string& default_value) const {
  std::map<std::string, std::string>::const_iterator it_fields = fields_.find(name);
  if (it_fields == fields_.end()) {
    return default_value;
  } else {
    return it_fields->second;
  }
}

size_t Log::SerialitzationSize() {
  return sizeof(LogHeader) + sizeof(LogMsg) + GetStringsSize();
}

size_t Log::GetStringsSize() const {
  size_t total = 0;

  std::map<std::string, std::string>::const_iterator it_fields;
  for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
    total += it_fields->first.length();
    total++;

    total += it_fields->second.length();
    total++;
  }

  return total;
}

void Log::CopyStrings(char *data) {
  size_t pos = 0;

  std::map<std::string, std::string>::iterator it_fields;
  for (it_fields = fields_.begin(); it_fields != fields_.end(); it_fields++) {
    memcpy(&data[pos], it_fields->first.c_str(), it_fields->first.length() + 1);
    pos += it_fields->first.length();
    pos++;

    memcpy(&data[pos], it_fields->second.c_str(), it_fields->second.length() + 1);
    pos += it_fields->second.length();
    pos++;
  }
}

void Log::AddStrings(char *strings, size_t len) {
  std::vector<std::string> vector_strings;

  size_t pos = 0;
  for (size_t i = 0; i < len; i++) {
    if (strings[i] == '\0') {
      vector_strings.push_back(&strings[pos]);
      pos = i + 1;
    }
  }

  // LM_V(("Readed %lu strins" , vector_strings.size() ));

  // Add to the map
  for (size_t i = 0; i < vector_strings.size() / 2; i++) {
    std::string field_name = vector_strings[2 * i];
    std::string field_value = vector_strings[2 * i + 1];

    // LM_V(("Added %s=%s" , field_name.c_str() , field_value.c_str() ));

    // Add field
    Set(field_name, field_value);
  }
}

LogData& Log::log_data() {
  return log_data_;
}

void Log::SetNewSession() {
  log_data_.line = 0;
  log_data_.channel = -1;   // Mark for the new session
  log_data_.tv.tv_sec = time(NULL);
  log_data_.tv.tv_usec = 0;
  log_data_.timezone = 0;
  log_data_.pid = 0;

  Set("new_session", "yes");
}

bool Log::IsNewSession() const {
  return Get("new_session", "no") == "yes";
}

au::SharedPointer<au::tables::Table> getTableOfFields() {
  au::SharedPointer<au::tables::Table>table(new au::tables::Table("filed|description,left"));

  table->addRow(au::StringVector("node", "Name of the element in a distributed environment"));
  table->addRow(au::StringVector("host", "Host where trace was generated ( only in log server)"));
  table->addRow(au::StringVector("type", "One letter type of message W, W, E, X, T..."));
  table->addRow(au::StringVector("channel", "Name of the log channel ( example system )"));
  table->addRow(au::StringVector("pid", "Process identifier"));
  table->addRow(au::StringVector("tid", "Thread identifier"));
  table->addRow(au::StringVector("data,DATE", "Date of the log in different formats"));
  table->addRow(au::StringVector("time,TIME", "Time of the log in different formats"));
  table->addRow(au::StringVector("timestamp,time_unix", "Complete timestamp of the log in different formats"));
  table->addRow(au::StringVector("exec", "Name of the exec file that generated the log"));
  table->addRow(au::StringVector("exec_short", "Same as exec but without full path"));


  table->addRow(au::StringVector("function", "Name of the function"));
  table->addRow(au::StringVector("line", "Number of line where the trace was generated"));
  table->addRow(au::StringVector("text", "Text of the log"));

  return table;
}

// Fancy functions to get the color of this log on screen
au::Color Log::GetColor() {
  if (log_data_.level <= LOG_LEVEL_ERROR) {
    return au::BoldRed;
  }

  if (log_data_.level == LOG_LEVEL_WARNING) {
    return au::BoldMagenta;
  }

  if (log_data_.level == LOG_LEVEL_VERBOSE) {
    return au::BoldCyan;
  }

  if (log_data_.level == LOG_LEVEL_MESSAGE) {
    return au::Yellow;
  }

  return au::Normal;
}

int Log::GetLogLevel(const std::string& str_log_level) {
  if (str_log_level == "E") {
    return LOG_LEVEL_ERROR;
  }
  if (str_log_level == "W") {
    return LOG_LEVEL_WARNING;
  }
  if (str_log_level == "M") {
    return LOG_LEVEL_MESSAGE;
  }

  if (str_log_level == "V") {
    return LOG_LEVEL_VERBOSE;
  }

  if (str_log_level == "D") {
    return LOG_LEVEL_DEBUG;
  }
  if (str_log_level == "*") {
    return LOG_LEVEL_ALL;
  }
  if (str_log_level == "all") {
    return LOG_LEVEL_ALL;
  }

  // Any other case, 0
  return 0;
}

std::string Log::GetLogLevel(int log_level) {
  if (log_level >= LOG_LEVEL_ALL) {
    return "*";
  }

  switch (log_level) {
    case LOG_LEVEL_ERROR_EXIT: return "X";

    case LOG_LEVEL_ERROR: return "E";

    case LOG_LEVEL_WARNING: return "W";

    case LOG_LEVEL_MESSAGE: return "M";

    case LOG_LEVEL_VERBOSE: return "V";

    case LOG_LEVEL_DEBUG: return "D";

    default:
      return "?";

      break;
  }
}

std::vector< au::SharedPointer<Log> > readLogFile(std::string file_name, au::ErrorManager& error) {
  std::vector< au::SharedPointer<Log> > logs;
  int fd = open(file_name.c_str(), O_RDONLY);

  if (fd < 0) {
    error.AddError(au::str("Not possible to open file %s", file_name.c_str()));
    return logs;
  }

  // File descriptor to read logs
  FileDescriptor file_descriptor("reading log file", fd);

  while (true) {
    au::SharedPointer<Log> log(new Log());
    if (log->Read(&file_descriptor)) {
      logs.push_back(log);
    } else {
      break;
    }
  }

  return logs;
}
}
