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

#include <string>

#include "logMsg/logMsg.h"

#include "BufferProcessor.h"  // Own interface
#include "StreamConnector.h"

#include "samson/module/ModulesManager.h"

#include "Adaptor.h"
#include "common.h"

extern size_t buffer_size;

namespace stream_connector {
BufferProcessor::BufferProcessor(Channel *_channel) {
  channel = _channel;
  splitter_name = channel->getSplitter();

  // Internal buffer ( to be used as input by the splitter )
  buffer = (char *)malloc(buffer_size);
  max_size = buffer_size;
  size = 0;

  splitter = NULL;

  if (splitter_name != "") {
    samson::Operation *operation = au::Singleton<samson::ModulesManager>::shared()->GetOperation(splitter_name);

    if (!operation || (operation->getType() != samson::Operation::splitter)) {
      LOG_SW(("Invalid splitter %s. Not using any splitter", splitter_name.c_str()));
    } else {
      // Get instace of the operation
      splitter = (samson::Splitter *)operation->getInstance();
      if (!splitter) {
        LOG_SW(("Error getting instance of the splitter %s. Not using any splitter", splitter_name.c_str()));
      }
    }
  }
}

BufferProcessor::~BufferProcessor() {
  // Free allocted buffer
  if (buffer) {
    free(buffer);
  }
}

// SplitterEmitter interface
void BufferProcessor::emit(char *data, size_t length) {
  // If not possible to write in the current buffer, just flush content
  {
    if (output_buffer_ != NULL) {
      if (output_buffer_->size() + length > output_buffer_->max_size()) {
        flushOutputBuffer();
      }
    }
  }

  // Recover buffer to write output
  if (output_buffer_ == NULL) {
    size_t output_buffer_size = std::max(length, (size_t)64000000 - sizeof(samson::KVHeader));       // Minimum 64Mbytes buffer
    output_buffer_ = engine::Buffer::Create("output_splitter", output_buffer_size);
  }

  // Write in the buffer
  if (!output_buffer_->Write(data, length)) {
    LM_X(1, ("Internal error"));
  }
  if (output_buffer_->size() == output_buffer_->max_size()) {
    flushOutputBuffer();
  }
}

void BufferProcessor::flushOutputBuffer() {
  if (output_buffer_ != NULL) {
    channel->push(output_buffer_);
    output_buffer_ = NULL;
  }
}

void BufferProcessor::process_intenal_buffer(bool finish) {
  // If no splitter, no process
  if (!splitter) {
    return;   // If no splitter, we never create an outputBuffer
  }
  if (size == 0) {
    return;
  }

  // Pointer to data that has not been used in splitter
  char *nextData;

  // Split buffer to know what to push
  int c = splitter->split(buffer, size, finish, &nextData, this);

  if (c == 0) {
    // Flush pending data generated here
    flushOutputBuffer();

    // Data to be skip after process
    if (nextData) {
      if ((nextData < buffer) || (nextData > (buffer + size))) {
        LOG_SW((
                 "Splitter %s has returned a wrong nextData pointer when processing a buffer of %s. Skipping this buffer",
                 splitter_name.c_str(),
                 au::str(max_size).c_str()));
        size = 0;
      } else {
        size_t skip_size = nextData - buffer;

        // Move data at the beginning of the buffer
        memmove(buffer, buffer + skip_size, size - skip_size);
        size -= skip_size;
      }
    } else {
      size = 0;   // if nextData is NULL Ignore all input data
    }
  } else {
    if (size >= max_size) {
      LOG_SW(("Splitter %s is not able to split a full buffer %s. Skipping this buffer", splitter_name.c_str(),
              au::str(max_size).c_str()));
      size = 0;
    } else {
      return;   // Not enoutght data for splitter... this is OK if the buffer is not full
    }
  }
}

void BufferProcessor::push(engine::BufferPointer input_buffer) {
  if (!splitter) {
    // Directly to the item
    channel->push(input_buffer);
    return;
  }

  size_t pos_in_input_buffer = 0;   // Readed so far
  while (pos_in_input_buffer < input_buffer->size()) {
    size_t copy_size = std::min(max_size - size, input_buffer->size() - pos_in_input_buffer);

    memcpy(buffer + size, input_buffer->data() + pos_in_input_buffer, copy_size);
    size += copy_size;
    pos_in_input_buffer += copy_size;


    // Process internal buffer every 2 seconds or when internal buffer is full
    if ((size == max_size) || (cronometer.seconds() > 2)) {
      // Reset cronometer
      cronometer.Reset();

      // Process internal buffer to push blocks at the output
      process_intenal_buffer(false);
    }
  }
}

void BufferProcessor::flush() {
  process_intenal_buffer(true);     // Process internal buffer with the "finish" flag activated
}
}
