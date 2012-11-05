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
#include "samson/stream/WorkerTaskBase.h"      // Own interface
#include <list>

#include "logMsg/logMsg.h"      // LM_M
#include "samson/common/coding.h"
#include "samson/stream/Block.h"              // samson::Stream::Block
#include "samson/stream/BlockManager.h"       // samson::stream::BlockManager
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
WorkerTaskBase::WorkerTaskBase(::samson::SamsonWorker* samson_worker, size_t id, const std::string& concept , bool simple_task ) :
  block_list_container_(au::str("block lists for task %lu", id), id), activity_monitor_("definition") {
  samson_worker_ = samson_worker;
  id_ = id; // Not assigned at the moment
  ready_ = false; // By default it is not ready
  task_state_ = "Init"; // Set initial state
  concept_ = concept;
  worker_task_finished_ = false;
    simple_task_ = simple_task;

  // Environment for this tasks
  environment_.Set("system.task_id", id);
}

bool WorkerTaskBase::is_ready() {
  if (ready_) {
    return true;
  }

  // Check if all the input blocks are in memory
  ready_ = block_list_container_.is_content_in_memory();

  if (ready_) {
    // Lock all content in memory
    block_list_container_.lock_content_in_memory();

    // Change state to ready...
    SetTaskState("ready");
  }

  return ready_;
}


void WorkerTaskBase::SetTaskState(const std::string& task_state) {
  task_state_ = task_state;
}

void WorkerTaskBase::AddInput(int channel, BlockPointer block, KVRange range, KVInfo info) {
  std::string block_list_name = au::str("input_%d", channel);
  BlockList *block_list = block_list_container_.getBlockList(block_list_name);

  block_list->add(new BlockRef(block, range, info));
}

void WorkerTaskBase::AddOutput(int channel, BlockPointer block, KVRange range, KVInfo info) {
  std::string block_list_name = au::str("output_%d", channel);
  BlockList *block_list = block_list_container_.getBlockList(block_list_name);

  block_list->add(new BlockRef(block, range, info));

  // Keep the list of generated output blocks
  output_block_ids_.push_back(block->block_id());
}

void WorkerTaskBase::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  if (visualization.get_flag("data")) {
    add(record, "id", worker_task_id(), "left,different");
    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Input %d", i), block_info.strShort(), "left,different");
    }
    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Output %d", i), block_info.strShort(), "left,different");
    }

    return;
  }

  if (visualization.get_flag("blocks")) {
    add(record, "id", id_, "left,different");

    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
      add(record, au::str("Input %d", i), block_list->str_blocks(), "left,different");
    }
    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
      add(record, au::str("Output %d", i), block_list->str_blocks(), "left,different");
    }

    return;
  }

  add(record, "id", worker_task_id(), "left,different");
  add(record, "concept", concept(), "left,different");

  add(record, "waiting", ProcessItem::waiting_time_seconds(), "f=time,different");
  add(record, "running ", ProcessItem::running_time_seconds(), "f=time,different");
  add(record, "progress ", progress(), "f=percentage,different");

  if (worker_task_finished_) {
    add(record, "finished ", "Yes", "different");
  } else {
    add(record, "finished ", "No", "different");
  }

  add(record, "error ", error().GetMessage(), "different");

  if (ProcessItem::finished()) {
    add(record, "state", "finished", "left,different");
  } else if (ProcessItem::running()) {
    add(record, "state", "running", "left,different");
  } else {
    add(record, "state", task_state(), "left,different");
  }

  /*
   * // Collect information from inputs / outputs
   * add( record , "input_0" , getBlockList("input_0")->getBlockInfo().strShort() , "different");
   * add( record , "all_inputs" , getUniqueBlockInfo().strShort()  , "different");
   */
}

bool WorkerTaskBase::IsWorkerTaskFinished() const {
  return worker_task_finished_;
}

std::string WorkerTaskBase::str_inputs()  const {
  return block_list_container_.str_inputs();
}
std::string WorkerTaskBase::str_outputs() const {
  return block_list_container_.str_outputs();
}

void WorkerTaskBase::SetWorkerTaskFinished() {
  worker_task_finished_ = true;
}

void WorkerTaskBase::SetWorkerTaskFinishedWithError(const std::string& error_message) {
  error_.set(error_message);
  worker_task_finished_ = true;
}

std::string WorkerTaskBase::generate_commit_command(const std::vector<std::string>& inputs,
                                                    const std::vector<std::string>& outputs) {

  // Get the commit command for this stream operation
  CommitCommand commit_command;

  // Input elements
  for (size_t c = 0; c < inputs.size(); c++) {
    BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", c));
    au::list<BlockRef>::iterator it;
    for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
      BlockRef *block_ref = *it;

      commit_command.RemoveBlock(inputs[c], block_ref->block_id(), block_ref->block_size(),
                                 block_ref->block()->getKVFormat(), block_ref->range(), block_ref->info());
    }
  }

  // Output elements
  for (size_t c = 0; c < outputs.size(); c++) {
    BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", c));
    au::list<BlockRef>::iterator it;
    for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
      BlockRef *block_ref = *it;

      commit_command.AddBlock(outputs[c], block_ref->block_id(), block_ref->block_size(),
                              block_ref->block()->getKVFormat(), block_ref->range(), block_ref->info());
    }
  }

  // Gerate the commit command
  return commit_command.GetCommitCommand();
}
void WorkerTaskBase::setActivity(const std::string& name) {
  activity_monitor_.StartActivity(name);
}

std::string WorkerTaskBase::GetActivitySummary() {
  activity_monitor_.StopActivity();
  return activity_monitor_.GetSummary();
}
}
}
