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
#ifndef _H_STREAM_CONNECTOR_BLOCK_PROCESSOR
#define _H_STREAM_CONNECTOR_BLOCK_PROCESSOR

#include "au/statistics/Cronometer.h"
#include "engine/Buffer.h"


#include "engine/ProcessItem.h"

#include "BufferList.h"
#include "samson/module/Operation.h"

namespace stream_connector {
class StreamConnector;
class Channel;

/*
 *
 * Class BufferProcessor
 *
 * Element in charge of getting all the blocks from a particular input inside a channel
 * and emit output blocks
 *
 */

class BufferProcessor : public samson::SplitterEmitter {
  Channel *channel;                      // Item to send data
  samson::Splitter *splitter;            // Splitter instance
  std::string splitter_name;             // Splitter name

  // Cronometer to indicate time since last process
  au::Cronometer cronometer;

  // Internal buffer to be processed by splitter
  char *buffer;
  size_t max_size;
  size_t size;

  // Buffer used to emit output produced by the splitter
  engine::BufferPointer output_buffer_;

public:

  BufferProcessor(Channel *_channel);
  ~BufferProcessor();

  void push(engine::BufferPointer bufer);
  void flush();

  // SplitterEmitter interface
  void emit(char *data, size_t length);

  // Flush accumulated buffer at the output of splitter
  void flushOutputBuffer();

private:

  void process_intenal_buffer(bool finish);
};
}


#endif  // ifndef _H_STREAM_CONNECTOR_BLOCK_PROCESSOR
