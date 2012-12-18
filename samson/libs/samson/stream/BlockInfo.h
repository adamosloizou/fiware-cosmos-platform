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
#ifndef _H_BLOCK_INFO
#define _H_BLOCK_INFO

#include <string>           // std::string

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/module/KVFormat.h"

namespace samson {
/**
 * Structure used to report information about blocks
 * This belongs to the old system of samson 0.6 where blocks where managed in each worker
 * It is still used just to report data contained by operations
 **/

struct BlockInfo {
  FullKVInfo info;          // Number of key-values and data size
  int num_blocks;           // Number of blocks

  size_t size;              // Size of the blocks ( including headers )
  size_t size_on_memory;    // Size contained in memory
  size_t size_on_disk;      // Size contained on disk
  size_t size_locked;       // Size locked in memory
  KVFormat format;          // Common format for all the information
  time_t min_time;          // Min time of all blocks
  time_t max_time;          // Max time of all blocks

  // Default constructor to initialize all values
  BlockInfo();

  /**
   * \brief Append a block of data
   */
  void AppendBlock(FullKVInfo _info) {
    info.append(_info);
    num_blocks++;
  }

  /**
   * \brief Append from another block info
   */
  void Append(BlockInfo block_info) {
    info.append(block_info.info);
    num_blocks += block_info.num_blocks;
  }

  // Push information
  // ----------------------------------------------------------------
  void push(KVFormat _format);
  void pushTime(time_t time);

  // Get information
  // ----------------------------------------------------------------

  time_t min_time_diff();
  time_t max_time_diff();
  double getOverhead();
  bool isContentOnMemory();
  bool isContentOnDisk();
  double onMemoryPercentadge();
  double onDiskPercentadge();
  double lockedPercentadge();

  size_t average_block_size() {
    return info.size / num_blocks;
  }

  // Debug strings
  // ----------------------------------------------------------------
  std::string str();
  std::string strShort();
  std::string strShortInfo();
};
}

#endif  // ifndef _H_BLOCK_INFO
