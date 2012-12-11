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
* FILE            Buffer.h
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Buffer is a memory region allocated and controlled by MemoryManager
* It can be requested and used by any element in the application
* It can only be destroyed calling "destroyBuffer" at MemoryMamager
* It comes with some usufull function to interact with its contect
*
* ****************************************************************************/

#ifndef _ENGINE_BUFFER_H_
#define _ENGINE_BUFFER_H_

#include <cstdlib>                       // malloc, ...
#include <cstring>                       // size_t
#include <fstream>                       // std::ifstream
#include <set>
#include <string>

#include "au/ErrorManager.h"
#include "au/containers/SharedPointer.h"
#include "au/mutex/Token.h"
#include "engine/Notification.h"
#include "engine/NotificationListener.h"  // engine::EngineNotificationObject
#include "engine/SimpleBuffer.h"         // engine::SimpleBuffer
namespace engine {
class Buffer {
  Buffer(const std::string& name, size_t max_size);

public:
  static au::SharedPointer<Buffer> Create(const std::string& name, size_t max_size);
  ~Buffer();

  // Get the maximum size of the buffer
  size_t max_size() const;

  // Get used size of this buffer ( not necessary the maximum )
  size_t size() const;
  void set_size(size_t size);

  // Get internal name for debugging
  std::string name() const;

  // Get a description of the buffer ( debugging )
  std::string str() const;

  // Environment variable
  au::Environment& environment() {
    return environment_;
  }

  // Set internal name for debugging
  void set_name(const std::string& name);

  // Write content to this buffer
  bool Write(const char *input_buffer, size_t input_size);

  // Skip some space without writing anything ( usefull to fix-size headers )
  bool SkipWrite(size_t size);

  // Write on the buffer the maximum possible ammount of data
  void Write(std::ifstream &inputStream);

  // Get available space to write with "write call"
  size_t GetAvailableSizeToWrite() const;

  // Reading content from the buffer
  // ------------------------------------------

  // Skip some space without reading
  size_t SkipRead(size_t size);

  // Read command
  size_t Read(char *output_buffer, size_t output_size);

  // Get pending bytes to be read
  size_t GetAvailableSizeToRead() const;

  // Manual manipulation of data
  // ------------------------------------------

  char *data();

  // Interface with SimpleBuffer ( simplified view of the internal buffer )
  // ------------------------------------------

  SimpleBuffer GetSimpleBuffer();
  SimpleBuffer GetSimpleBufferAtOffset(size_t offset);

  // Special functions
  // ------------------------------------------

  // Remove the last characters of an unfinished line and put them in buffer.
  int RemoveLastUnfinishedLine(char *& buffer, size_t& buffer_size);

  // Read a file and write to this buffer
  void WriteFromFile(const std::string& file_name, au::ErrorManager& error);

  // Access to tags

private:

  size_t buffer_id_;     // Unique identifier of this buffer
  char *data_;           // Buffer of data
  size_t max_size_;      // Maximum size of this buffer
  std::string name_;     // Internal name for debugging

  /**
   * Current size used in this buffer
   * This is the one that should be used when transmitting the buffer across the network
   * This variable is updated with calls to "write" or set manually in set_size
   */

  size_t size_;

  /**
   * Internal variable used for the read process
   */

  size_t offset_;

  au::Environment environment_;

  static size_t static_buffer_id_;
};
// Buffer Shared Pointer
typedef au::SharedPointer<Buffer>   BufferPointer;
}

#endif  // ifndef _ENGINE_BUFFER_H_
