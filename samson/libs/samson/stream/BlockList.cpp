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
#include "samson/stream/BlockList.h"  // Own interface

#include <sstream>

#include "au/file.h"                 // au::sizeOfFile

#include "engine/ProcessManager.h"  // engine::ProcessManager

#include "logMsg/logMsg.h"           // LOG_SM

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/SamsonSetup.h"
#include "samson/common/coding.h"  // KVInfo
#include "samson/module/ModulesManager.h"
#include "samson/network/NetworkInterface.h"
#include "samson/stream/Block.h"     // samson::stream::Block
#include "samson/stream/BlockManager.h"   // samson::stream::BlockManager
#include "samson/stream/WorkerTask.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
BlockList::~BlockList() {
  clearBlockList();
}

void BlockList::clearBlockList() {
  // Make sure I am not in any list in the blocks I am retaining...
  au::list<BlockRef>::iterator it_blocks;   // List of blocks references
  for (it_blocks = blocks_.begin(); it_blocks != blocks_.end(); it_blocks++) {
    BlockRef *block_ref = *it_blocks;
    BlockPointer block = block_ref->block();
    block->remove_block_list(this);
  }

  // Remove all reference contained in this list
  blocks_.clearList();
}

void BlockList::Add(BlockRef *block_ref) {
  // Insert this block in my list
  blocks_.push_back(block_ref);

  // Update information in the block ( for sorting )
  block_ref->block()->add_block_list(this);
}

void BlockList::Remove(BlockRef *block_ref) {
  // Remove this block from my list of blocks
  blocks_.remove(block_ref);

  // Update information in the block ( for sorting )
  block_ref->block()->remove_block_list(this);
}

void BlockList::lock_content_in_memory() {
  au::list<BlockRef>::iterator it_blocks;   // List of blocks references
  for (it_blocks = blocks_.begin(); it_blocks != blocks_.end(); it_blocks++) {
    BlockRef *block_ref = *it_blocks;
    BlockPointer block = block_ref->block();
    if (!block->is_content_in_memory()) {
      LM_X(1, ("Internal error"));
    }
    block->lock_content_in_memory(this);
  }
}

size_t BlockList::getNumBlocks() {
  return blocks_.size();
}

size_t BlockList::task_id() {
  return task_id_;
}

int BlockList::priority() {
  return priority_;
}

BlockInfo BlockList::getBlockInfo() {
  BlockInfo block_info;

  au::list<BlockRef>::iterator it;
  for (it = blocks_.begin(); it != blocks_.end(); it++) {
    BlockRef *block_ref = *it;
    block_ref->append(block_info);
  }

  return block_info;
}

void BlockList::ReviewBlockReferences(au::ErrorManager& error) {
  au::list<BlockRef>::iterator it;
  for (it = blocks_.begin(); it != blocks_.end(); it++) {
    (*it)->review(error);
    if (error.HasErrors()) {
      return;
    }
  }
}

std::string BlockList::str_blocks() {
  if (blocks_.size() == 0) {
    return "empty";
  }

  std::ostringstream output;
  output << "[ ";
  au::list<BlockRef>::iterator it;
  for (it = blocks_.begin(); it != blocks_.end(); it++) {
    output << str_block_id((*it)->block_id()) << " ";
  }
  output << "]";
  return output.str();
}

bool BlockList::ContainsBlock(size_t block_id) {
  au::list<BlockRef>::iterator it;
  for (it = blocks_.begin(); it != blocks_.end(); it++) {
    if ((*it)->block_id() == block_id) {
      return true;
    }
  }
  return false;
}
}
}

