#ifndef _H_BLOCK_LIST
#define _H_BLOCK_LIST

/* ****************************************************************************
 *
 * FILE                      BlockList.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * List of blocks: A tool for managing groups of blocks in queues, queue_operations, states, etc..
 *
 */


#include <ostream>                   // std::ostream
#include <string>                    // std::string

#include "au/Cronometer.h"      // au::cronometer
#include "au/containers/SharedPointer.h"
#include "au/containers/list.h"      // au::list
#include "au/containers/map.h"       // au::map

#include "engine/Buffer.h"      // engine::Buffer

#include "engine/NotificationListener.h"           // engine::NotificationListener

#include "samson/common/KVFile.h"
#include "samson/common/coding.h"    // FullKVInfo
#include "samson/common/samson.pb.h"  // network::

#include "samson/stream/Block.h"
#include "samson/stream/BlockInfo.h"  // BlockInfo

/**
 *
 * BlockList: container Blocks.
 *
 * Note:   All the nodes are contained in at least one BlockList.
 * If a block is not contained in any of them, it is automatically removed from the Block Manager
 *
 */


namespace samson {
class Info;

namespace stream {
class Block;
class Queue;
class StreamManager;
class QueueItem;
class BlockMatrix;


/*
 *
 * BlockRef
 *
 *
 * Reference for a block and a particular range of key-values
 * Information about number of key-values and size is also included
 *
 */

class BlockRef {
public:

  BlockRef(BlockPointer block, KVRange range, KVInfo info);
  ~BlockRef();

  BlockPointer block();
  size_t block_id();
  KVInfo info();
  KVRange range();
  au::SharedPointer<KVFile> file();

  // Accumulate content
  void append(BlockInfo&);

  // Review BlockReference
  void review(au::ErrorManager& error);

private:

  BlockPointer block_;         // Pointer to the block
  KVRange range_;       // Range associated with this block
  KVInfo info_;                // Size and # of kvs to be processed

  // Extra information anout hg organitzation
  au::SharedPointer<KVFile> file_;
};

class BlockList {
public:

  au::list< BlockRef > blocks;  // List of blocks references

  BlockList(std::string name = "no_name", size_t task_id = ( size_t)-1) {
    name = name;
    task_id_  = task_id;       // Task is the order of priority
    lock_in_memory_ = false;       // By default no lock in memory
    priority_ = 0;             // Default priority level
  }

  ~BlockList();

  // Simple add or remove blocks
  void add(BlockRef *blobk_ref);
  void remove(BlockRef *block_ref);

  // Remove all the blocks contained in the list
  void clearBlockList();

  // Get information
  size_t getNumBlocks();

  void lock_content_in_memory();

  // Get information
  size_t task_id();
  int priority();

  // Get information about content included in this list
  BlockInfo getBlockInfo();

  // Review blocks to verify number of key-values
  void ReviewBlockReferences(au::ErrorManager& error);

  // string for debug blocks
  std::string str_blocks();

private:

  std::string name_;           // Name of this block list ( for debugging )
  size_t task_id_;             // Order of the task if really a task
  bool lock_in_memory_;        // Lock in memory
  int priority_;               // Priority level for blocks that are not involved in tasks
};
}
}

#endif  // ifndef _H_BLOCK_LIST
