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


#include "ReadFile.h"       // Own interface
#include "logMsg/logMsg.h"         // LM_M ...

namespace engine {
ReadFile::ReadFile(const std::string& file_name) {
  file_name_ = file_name;
  file_ = fopen(file_name_.c_str(), "r");
  offset_ = 0;
}

ReadFile::~ReadFile() {
  Close();
}

int ReadFile::Seek(size_t offset) {
  if (!file_) {
    return 1;
  }

  if (offset_ == offset) {
    return 0;       // Correct... just do not move
  }

  // Set the current offset
  offset_ = offset;

  if (fseek(file_, offset, SEEK_SET) != 0) {
    Close();
    return 3;
  }

  // Everything corrent
  return 0;
}

int ReadFile::Read(char *read_buffer, size_t size) {
  if (size == 0) {
    return 0;
  }

  if (fread(read_buffer, size, 1, file_) == 1) {
    offset_ += size;     // Move the offser according to the read bytes
    return 0;
  } else {
    return 1;       // Error at reading
  }
}

bool ReadFile::IsValid() const {
  return (file_ != NULL);
}

void ReadFile::Close() {
  if (file_) {
    fclose(file_);
    file_ = NULL;
    offset_ = 0;
  }
}

std::string ReadFile::file_name() const {
  return file_name_;
}

size_t ReadFile::offset() const {
  return offset_;
}
}