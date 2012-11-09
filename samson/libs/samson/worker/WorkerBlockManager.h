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
#ifndef WORKER_BLOCK_MANAGER_H_
#define WORKER_BLOCK_MANAGER_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "au/containers/map.h"
#include "au/containers/set.h"
#include "au/containers/SharedPointer.h"

#include "engine/Buffer.h"
#include "engine/NotificationListener.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "samson/worker/BlockRequest.h"

/*
 *
 * WorkerBlockManager
 *
 * Manager for all block related operations in workers
 *
 * Push operations from delilahs
 * Distributions between workers
 * Block requests between workers
 *
 *
 */

namespace samson {
  class SamsonWorker;
  namespace stream{
    class DefragTask;
  }
  
  class WorkerBlockManager {
  public:
    
    WorkerBlockManager(SamsonWorker *samson_worker) {
      samson_worker_ = samson_worker;
    }
    ~WorkerBlockManager() {}
    
    /*
     \brief Create a block adding to the block manager
     Output of any operation in this worker create blocks using the method
     */
    size_t CreateBlock( engine::BufferPointer buffer );

    /*
     \brief Notify that a block request response message has been received
     This will effectivelly add a new block to the local block manager
     */
    void ReceivedBlockRequestResponse(size_t block_id, size_t worker_id , bool error);

    // Add block requests
    void RequestBlocks(const std::set<size_t>& pending_block_ids);
    void RequestBlock(size_t block_id);

    // Add a block-break request
    void AddBlockBreak( const std::string& queue_name, size_t block_id , const std::vector<KVRange>& ranges );
    
    // Review all kind of elements
    void Review();
    
    // Receive a push block from delilah
    void ReceivedPushBlock(  size_t delilah_id
                           , size_t push_id
                           , engine::BufferPointer buffer
                           , const std::vector<std::string>& queues );
    
    // General reset command ( worker has disconnected )
    void Reset();
    
    // Remove request for all blocks not belonmging to data model any more
    void RemoveRequestIfNecessary( const std::set<size_t>& all_block_ids );
    
    // Collections for all internal elements
    au::SharedPointer<gpb::Collection> GetCollectionForBlockRequests(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForBlockDefrags( const Visualization& visualization);
    
  private:
    
    SamsonWorker *samson_worker_;
    au::map<size_t, BlockRequest> block_requests_;   // Block requests sent by this worker
    au::Dictionary<std::string, stream::DefragTask > defrag_tasks_;
  };
  
}

#endif   // ifndef WORKER_BLOCK_MANAGER_H_
