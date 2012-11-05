#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>                            // std::set
#include <sstream>
#include <string>

#include "au/string/S.h"
#include "au/string/StringUtilities.h"                    // au::Format
#include "engine/ProcessItem.h"           // engine::ProcessItem
#include "samson/common/MessagesOperations.h"
#include "samson/stream/ProcessIsolated.h"
#include "samson/stream/Block.h"          // samson::Stream::Block
#include "samson/stream/BlockInfo.h"      // struct BlockInfo
#include "samson/stream/BlockList.h"      // stream::BlockList
#include "samson/stream/WorkerTaskBase.h"  // parent class samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;
namespace stream {
class Block;

/*
 
 WorkerTask : Main class fot all the tasks based on third party operations
 
 */

class WorkerTask : public ::samson::ProcessIsolated {
  public:
    // Constructor
    WorkerTask( SamsonWorker *samson_worker
               , size_t id
               , const gpb::StreamOperation& stream_operation
               , Operation *operation
               , KVRange range );

    ~WorkerTask();

    // Virtual methods of samson::ProcessIsolated
    virtual void initProcessIsolated();
    virtual void generateKeyValues(samson::ProcessWriter *writer);
    virtual void generateTXT(TXTWriter *writer);

    // Method to process output buffer ( in Engine main thread )
    void processOutputBuffers();

    // Commit command to use when this operation finish
    std::string commit_command();

    // Get information of the current process
    std::string getStatus();

    // Virtual methods from WorkerTaskBase
    virtual std::string str();

  
  private:
  
    // Specific function to execute map, reduce, parser operations
    void generateKeyValues_parser(samson::ProcessWriter *writer);
    void generateKeyValues_map(samson::ProcessWriter *writer);
    void generateKeyValues_reduce(samson::ProcessWriter *writer);

    // Information about the operation to run
    gpb::StreamOperation *stream_operation_;

    // Operation to be used here ( form ModulesManager )
    Operation *operation_;

    // Range to apply this operation
    KVRange range_;
  
    // Pointer to samson worker to create new blocks
    SamsonWorker *samson_worker_;
};


}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK
