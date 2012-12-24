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
#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>                            // std::set
#include <sstream>
#include <string>

#include "au/string/S.h"
#include "au/string/StringUtilities.h"    // au::Format
#include "engine/ProcessItem.h"           // engine::ProcessItem
#include "samson/common/MessagesOperations.h"
#include "samson/stream/Block.h"          // samson::Stream::Block
#include "samson/stream/BlockInfo.h"      // struct BlockInfo
#include "samson/stream/BlockList.h"      // stream::BlockList
#include "samson/stream/ProcessIsolated.h"
#include "samson/stream/WorkerTaskBase.h"  // parent class samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;
namespace stream {
class Block;

/**
 * \brief Main class fot all tasks based on third party operations (map, reduce, parse, ...)
 */

class WorkerTask : public ::samson::ProcessIsolated {
public:
  // Constructor
  WorkerTask(SamsonWorker *samson_worker
             , size_t id
             , const gpb::StreamOperation& stream_operation
             , Operation *operation
             , KVRange range);

  ~WorkerTask();

  // Virtual methods of samson::ProcessIsolated
  virtual void initProcessIsolated();
  virtual void generateKeyValues(samson::ProcessWriter *writer);
  virtual void generateTXT(TXTWriter *writer);

  // Method to process output buffer ( in Engine main thread )
  std::vector<size_t> ProcessOutputBuffers();

  // Commit command to use when this operation finish
  std::string commit_command();

  // Get information of the current process
  std::string getStatus();

  // Virtual methods from WorkerTaskBase
  virtual std::string str();

  // Name of the stream operation
  std::string stream_operation_name() const {
    return stream_operation_->name();
  }

  BlockInfo GetStateDataInfo() const;
  BlockInfo GetInputDataInfo() const;
  BlockInfo GetOutputDataInfo() const;

  // Commit to data model
  void commit();

  // Set this worker task as defrag
  void SetDefragTask(const KVFormat& format) {
    defrag_job_ = true;
    defrag_format = format;

    addOutput(format);
    // addOutput(KVFormat("system.Value", "system.Value"));    // Add an additional output for log
  }

private:

  // Specific function to execute map, reduce, parser operations
  void generateKeyValues_parser(samson::ProcessWriter *writer);
  void generateKeyValues_map(samson::ProcessWriter *writer);
  void generateKeyValues_reduce(samson::ProcessWriter *writer);
  void generateKeyValuesDefrag(samson::ProcessWriter *writer);

  // Information about the operation to run
  gpb::StreamOperation *stream_operation_;

  // Operation to be used in the background thread
  // andreu: It cannot be used in the platform process since module can be removed in engine thread
  Operation *operation_;

  // Information to collect statistics
  int state_input_channel_;
  int num_input_channels_;
  int num_output_channels_;

  // Range to apply this operation
  KVRange range_;

  // Pointer to samson worker to create new blocks
  SamsonWorker *samson_worker_;

  // Specific data for defrag operation
  bool defrag_job_;
  KVFormat defrag_format;
};
}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK
