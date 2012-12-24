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

#include "au/ThreadManager.h"

#include "BufferProcessor.h"
#include "FileDescriptorConnection.h"
#include "StreamConnector.h"


extern size_t buffer_size;
extern size_t input_buffer_size;  // Size of the chunks to read

namespace stream_connector {
const size_t FileDescriptorConnection::kMinInputbufferSize = 1024;
const size_t FileDescriptorConnection::kMaxInputbufferSize = 100 * 1024 * 1024;

void *run_FileDescriptorConnection(void *p) {
  // Recover the correct pointer
  FileDescriptorConnection *connection = ( FileDescriptorConnection * )p;

  // Main run
  connection->run();

  return NULL;
}

FileDescriptorConnection::FileDescriptorConnection(Adaptor *_item, ConnectionType _type, std::string _name)
  : Connection(_item, _type, _name) {
  // Keep pointer to file descriptor
  file_descriptor_ = NULL;

  // By default, thread is not running
  thread_running_ =  false;

  // Init counter of connections
  num_connections_ = 0;

  // Default input buffer size ( the minimum one )
  input_buffer_size = kMinInputbufferSize;
}

void FileDescriptorConnection::start_connection() {
  connect();       // Get a file descriptor and create background thread
}

void FileDescriptorConnection::stop_connection() {
  // Stop thread in the background
  if (file_descriptor_) {
    file_descriptor_->Close();
  }
  while (thread_running_) {
    usleep(100000);
  }
}

void FileDescriptorConnection::connect() {
  if (thread_running_ || file_descriptor_) {
    return;
  }

  // Update the counter of connections
  num_connections_++;
  cronometer_reconnection_.Reset();       // Start chronometer to do not restart immediately

  // Get file descriptor and start background thread
  file_descriptor_ = getFileDescriptor();
  if (file_descriptor_) {
    // Create the thread
    thread_running_ = true;
    pthread_t t;
    au::Singleton<au::ThreadManager>::shared()->AddThread("StreamConnectorConnection", &t, NULL,
                                                          run_FileDescriptorConnection,
                                                          this);
  }
}

FileDescriptorConnection::~FileDescriptorConnection() {
  if (file_descriptor_) {
    file_descriptor_->Close();
    delete file_descriptor_;
  }
}

void FileDescriptorConnection::review_connection() {
  if (!file_descriptor_) {
    set_as_connected(false);

    if (cronometer_reconnection_.seconds() < 3) {
      return;            // Wait a little bit
    }
    // Reconnect if possible
    connect();

    return;
  }

  // If we are disconnected, wait until thread finish and delete it to reconnect
  if (file_descriptor_->IsClosed()) {
    set_as_connected(false);
    if (thread_running_) {  // Still waiting for the threads to finish
      return;
    }
    delete file_descriptor_;
    file_descriptor_ = NULL;
    return;
  }

  // Set connection based on file descriptor
  set_as_connected(!file_descriptor_->IsClosed());
}

void FileDescriptorConnection::run_as_output() {
  while (true) {
    if (!file_descriptor_) {
      LM_X(1, ("Internal error"));  // Container to keep a retained version of buffer
    }
    engine::BufferPointer buffer = getNextBufferToSent();

    if (buffer != NULL) {
      au::Status s = file_descriptor_->partWrite(buffer->data(), buffer->size(), "streamConnectorConnection");

      if (s != au::OK) {
        return;             // Just quit
      }
    } else {
      // Sleep a little bit before checking again
      usleep(100000);
    }
  }
}

void FileDescriptorConnection::run_as_input() {
  // Read from stdin and push blocks to the samson_connector
  while (true) {
    if (!file_descriptor_) {
      LM_X(1, ("Internal error"));  // Get a buffer
    }

    // If memory usage is too high wait
    au::Cronometer cronometer;
    while (engine::Engine::memory_manager()->memory_usage() > 1) {
      usleep(100000);
      if (cronometer.seconds() > 2) {
        cronometer.Reset();
        double mem = engine::Engine::memory_manager()->memory_usage();
        LOG_SW(("Not reading from input connection '%s' since memory usage is %s",
                fullname().c_str(),
                au::str_percentage(mem).c_str()));
      }
    }

    engine::BufferPointer buffer = engine::Buffer::Create("stdin buffer", input_buffer_size);


    size_t read_size = 0;
    au::Status s;
    {
      au::Cronometer c;
      s = file_descriptor_->partRead(buffer->data()
                                     , input_buffer_size
                                     , "read connector connections"
                                     , 300
                                     , &read_size);

      // adapt size of input buffer
      if (c.seconds() < 0.1) {
        input_buffer_size *= 2;
      } else if (c.seconds() > 3) {
        input_buffer_size /= 2;
      }

      // Max by total memory available
      size_t max_mem = engine::Engine::memory_manager()->memory() / 10;
      if (input_buffer_size > max_mem) {
        input_buffer_size = max_mem;
      }

      if (input_buffer_size < kMinInputbufferSize) {
        input_buffer_size = kMinInputbufferSize;
      }

      if (input_buffer_size > kMaxInputbufferSize) {
        input_buffer_size = kMaxInputbufferSize;
      }
    }

    // If we have read something...
    if (read_size > 0) {
      // Push input buffer
      buffer->set_size(read_size);
      pushInputBuffer(buffer);
    }

    // If last read is not ok...
    if (s != au::OK) {
      // Close fd
      file_descriptor_->Close();

      // Flush whatever we have..
      flushInputBuffers();

      // Quit main threads
      return;
    }
  }
}

void FileDescriptorConnection::run() {
  if (type() == connection_input) {
    run_as_input();
  } else {
    run_as_output();  // Mark as non thread_running
  }
  thread_running_ = false;
}

std::string FileDescriptorConnection::getStatus() {
  std::ostringstream output;

  if (!file_descriptor_) {
    output <<  "Not connected";
  } else {
    if (file_descriptor_->IsClosed()) {
      output << "Closing connection";
    } else {
      output << "Connected";
    } output << au::str(" [fd:%d]", file_descriptor_->fd());
  }

  if (type() == connection_input) {
    output << au::str(" [buffer:%s]", au::str(input_buffer_size).c_str());
  }
  return output.str();
}

SimpleFileDescriptorConnection::SimpleFileDescriptorConnection(Adaptor *_item
                                                               , ConnectionType _type
                                                               , std::string _name
                                                               , au::FileDescriptor *file_descriptor) :
  FileDescriptorConnection(_item, _type, _name) {
  // Keep the file descriptor
  file_descriptor_ = file_descriptor;
}

au::FileDescriptor *SimpleFileDescriptorConnection::getFileDescriptor() {
  // Mark component as finished
  if (file_descriptor_ == NULL) {
    set_as_finished();         // It is over
    return NULL;
  }

  // Return the provided file descriptor just once.
  au::FileDescriptor *return_file_descriptor = file_descriptor_;
  file_descriptor_ = NULL;
  return return_file_descriptor;
}
}
