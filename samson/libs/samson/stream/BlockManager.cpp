#include "samson/stream/BlockManager.h"

#include <dirent.h>
#include <sys/types.h>

#include "au/ExecesiveTimeAlarm.h"
#include "au/file.h"
#include "au/log/LogMain.h"

#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification

#include "logMsg/traceLevels.h"

#include "samson/common/KVHeader.h"
#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
#include "samson/stream/Block.h"
#include "samson/stream/BlockList.h"


/*
 * Note: The idea is now schedule all read and write operations until max size
 * and wait for scheduled read and write goes down to 0
 */

#define notification_review_block_manager "notification_review_block_manager"

namespace samson {
namespace stream {
class BlockSorter {
    const au::Dictionary<size_t, Block>& blocks_;

  public:
    BlockSorter(const au::Dictionary<size_t, Block>& blocks) :
      blocks_(blocks) {
    }

    bool operator()(size_t block_id1, size_t block_id2) const {
      au::SharedPointer<Block> block_1 = blocks_.Get(block_id1);
      au::SharedPointer<Block> block_2 = blocks_.Get(block_id2);

      return Block::compare(block_1.shared_object(), block_2.shared_object());
    }
};

BlockManager *blockManager = NULL;

void BlockManager::init() {
  if (blockManager) {
    LM_X(1, ("Error with init BlockManager (double init)"));
  }
  blockManager = new BlockManager();
}

BlockManager::BlockManager() :
  token_("BlockManager") {
  // Default values for read/write scheduling
  scheduled_write_size_ = 0;
  scheduled_read_size_ = 0;

  // Maximum amount of memory
  size_t total_memory = au::Singleton<SamsonSetup>::shared()->GetUInt64("general.memory");
  max_memory_ = static_cast<double>(total_memory) * 0.8;   // 80% of memory for block manager

  // Recover files from disk
  RecoverBlocksFromDisks();

  // Notification to review block manager
  listen(notification_review_block_manager);
  engine::Engine::shared()->notify(new engine::Notification(notification_review_block_manager), 1);
}

BlockManager::~BlockManager() {
}

void BlockManager::CreateBlock(size_t block_id, engine::BufferPointer buffer) {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  if (buffer == NULL) {
    LM_X(1, ("Internal error"));
  }

  buffer->set_name_and_type(au::str("Buffer for block %lu", block_id), "block");
  buffer->SetTag("block_manager");

  BlockPointer block(new Block(block_id, buffer));

  if( blocks_.Get(block_id) != NULL )
    LM_X(1, ("Internal error. Trying to add block %lu twice" , block_id ));
  
  // Add this block
  block_ids_.push_back(block_id);
  blocks_.Set(block_id, block);
}

void BlockManager::RemoveBlocksIfNecessary(const std::set<size_t>& all_block_ids ) {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  std::list<size_t>::iterator it;
  for (it = block_ids_.begin(); it != block_ids_.end();) {
    size_t block_id = *it;
    BlockPointer block = blocks_.Get(block_id);

    // Check if it is included in data model
    if (all_block_ids.find(block_id) != all_block_ids.end()) {
      it++;
      continue;
    }

    // Do not remove blocks while reading or writing...
    if (!block->canBeRemoved()) {
      it++;
      continue;
    }
    
    // Remove this block
    it = block_ids_.erase(it);
    blocks_.Extract(block_id);
    ScheduleRemoveOperation(block);

    engine::BufferPointer buffer = block->buffer();
    if (buffer != NULL)
      buffer->RemoveTag("block_manager");
  }
}

std::set<size_t> BlockManager::GetBlockIds() {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  std::set<size_t> block_ids;
  std::list<size_t>::iterator it;
  for (it = block_ids_.begin(); it != block_ids_.end(); it++) {
    block_ids.insert(*it);
  }
  return block_ids;
}

void BlockManager::destroy() {
  if (!blockManager) {
    LM_W(("Error destroying a non-initialized BlockManager"));
    return;
  }

  delete blockManager;
  blockManager = NULL;
}

BlockManager *BlockManager::shared() {
  if (!blockManager) {
    LM_X(1, ("Init BlockManager before using it"));
  }
  return blockManager;
}

void BlockManager::notify(engine::Notification *notification) {
  if (notification->isName(notification_review_block_manager)) {
    Review();
  } else if (notification->isName(notification_disk_operation_request_response)) {
    std::string type = notification->environment().Get("type", "-");
    size_t operation_size = notification->environment().Get("operation_size", 0);
    size_t block_id = notification->environment().Get("block_id", (size_t) 0);

    if (type == "remove") {
      return;   // nothing to do ( this avoid warning of block not found
    }
    // Recover the block
    BlockPointer block = blocks_.Get(block_id);

    if (block == NULL) {
      LM_W(("Notification for non existing block %lu.Ignoring...", block_id));
      return;
    }

    LM_T(LmtBlockManager, ("Received a disk notification with type %s and size %s"
            , type.c_str()
            , au::str(operation_size).c_str()));

    if (type == "write") {
      scheduled_write_size_ -= operation_size;
      block->state_ = Block::ready;
      Review();
    } else if (type == "read") {
      scheduled_read_size_ -= operation_size;
      block->state_ = Block::ready;
      Review();
    }
  } else {
    LM_W(("Unknown notification at BlockManager"));
  }
}

void BlockManager::Review() {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  // Internal consistency
  if (blocks_.size() != block_ids_.size())
    LM_X(1, ("Internal error in BLockManager: Different number of blocks and blocks ids (%lu != %lu)"
            , blocks_.size(), block_ids_.size()));

  // Get setup parameter to control review of BlockManager...
  size_t max_scheduled_write_size = au::Singleton<SamsonSetup>::shared()->GetUInt64("stream.max_scheduled_write_size");
  size_t max_scheduled_read_size = au::Singleton<SamsonSetup>::shared()->GetUInt64("stream.max_scheduled_read_size");

  // No schedule new operations until all the previous one have finished
  if (scheduled_read_size_ > 0) {
    return;
  }
  if (scheduled_write_size_ > 0) {
    return;
  }

  LM_T(LmtBlockManager, ("Reviewing block manager"));

  // If no blocks, nothing to do...
  if (blocks_.size() == 0) {
    return;
  }

  // --------------------------------------------------------------------------------
  // Sort by priorities
  // --------------------------------------------------------------------------------

  Sort();

  // --------------------------------------------------------------------------------
  // Find the blocks that should be in memory ( all until "limit_block" )
  // --------------------------------------------------------------------------------

  LM_T(LmtBlockManager, ("Detect limit_block"));
  size_t accumulated_memory = 0;
  size_t last_block_id_in_memory = *block_ids_.begin();
  std::list<size_t>::iterator b;
  for (b = block_ids_.begin(); b != block_ids_.end(); b++) {
    size_t block_id = *b;
    BlockPointer block = blocks_.Get(block_id);

    if (block == NULL) {
      LM_X(1, ("Internal error"));
    }
    if ((accumulated_memory + block->getSize()) > max_memory_) {
      break;
    }
    accumulated_memory += block->getSize();
    last_block_id_in_memory = block_id;
  }

  LM_T(LmtBlockManager, ("Block limit %lu", last_block_id_in_memory ));

  // --------------------------------------------------------------------------------
  // Free memory of blocks that are not suppouse to be on memory
  // --------------------------------------------------------------------------------
  {
    LM_T(LmtBlockManager, ("Free blocks in memory under block_limit"));
    std::list<size_t>::reverse_iterator b;
    for (b = block_ids_.rbegin(); b != block_ids_.rend(); b++) {
      // Considering this block
      size_t block_id = *b;
      BlockPointer block = blocks_.Get(block_id);

      // Stop when arrive to the limit block ( this should be in memory )
      if (block_id == last_block_id_in_memory) {
        break;
      }
      if (block->state() == Block::ready) {
        // Both on disk and on memory
        LM_T(LmtBlockManager, ("Free block:'%s'", block->str().c_str()));

        block->buffer()->RemoveTag("block_manager");

        // Free block
        block->freeBlock();
      }
    }
  }

  // --------------------------------------------------------------------------------
  // Schedule write operations
  // --------------------------------------------------------------------------------

  if (scheduled_write_size_ < max_scheduled_write_size) {
    LM_T(LmtBlockManager, ("Schedule write operations"));
    // Lock for new write operations...
    std::list<size_t>::reverse_iterator b;
    for (b = block_ids_.rbegin(); b != block_ids_.rend(); b++) {
      // Considering this block
      size_t block_id = *b;
      BlockPointer block = blocks_.Get(block_id);

      if (block->state() == Block::on_memory) {
        LM_T(LmtBlockManager, ("Schedule write for block:'%s'", block->str().c_str()));

        // Schedule write
        ScheduleWriteOperation(block);

        LM_T(LmtBlockManager, ("Write block %lu", block->block_id()));

        // No continue for more writes
        if (scheduled_write_size_ >= max_scheduled_write_size) {
          break;
        }
      }
    }
  }

  // --------------------------------------------------------------------------------
  // Schedule read operations
  // --------------------------------------------------------------------------------

  // Schedule new reads operations ( high priority elements ) if available memory
  if (scheduled_read_size_ < max_scheduled_read_size) {
    LM_T(LmtBlockManager, ("Schedule read operations"));
    // Lock for new write operations...
    std::list<size_t>::iterator b;
    for (b = block_ids_.begin(); b != block_ids_.end(); b++) {
      // Considering this block
      size_t block_id = *b;
      BlockPointer block = blocks_.Get(block_id);

      if (block->state() == Block::on_disk) {
        // Needed to be loaded...
        LM_T(LmtBlockManager, ("Trying to read block'%s'", block->str().c_str()));

        // Read the block
        ScheduleReadOperation(block);

        LM_T(LmtBlockManager, ("Scheduling read for block:'%s'", block->str().c_str()));

        // No continue for more writes
        if (scheduled_read_size_ > max_scheduled_read_size) {
          break;
        }
      }

      // No schedule read operations over the block limit
      if (block_id == last_block_id_in_memory) {
        LM_T(LmtBlockManager, ("Stops looking for read, because block_limit reached"));
        break;   // Not schedule reads on blocks that are not suppose to be on memory
      }
    }
  }
}

BlockPointer BlockManager::GetBlock(size_t block_id) {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  return blocks_.Get(block_id);
}

au::SharedPointer<gpb::Collection> BlockManager::GetCollectionOfBlocks(const Visualization& visualization) {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  Sort();

  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("blocks");

  size_t accumulated_size = 0;
  std::list<size_t>::iterator b;
  for (b = block_ids_.begin(); b != block_ids_.end(); b++) {
    // Considering this block
    size_t block_id = *b;
    BlockPointer block = blocks_.Get(block_id);

    accumulated_size += block->getSize();
    block->fill(collection->add_record(), visualization, accumulated_size);
  }

  return collection;
}

void BlockManager::CreateBlockFromDisk(const std::string& fileName) {
  size_t block_id = au::Singleton<SamsonSetup>::shared()->block_id_from_filename(fileName);

  if (block_id == 0) {
    LM_W(("Not possible to get ids for file %s to recover block", fileName.c_str()));
    return;
  }

  FILE *file = fopen(fileName.c_str(), "r");
  if (!file) {
    LM_W(("Not possible to open file %s to recover block", fileName.c_str()));
    return;
  }

  // Read header from the file...
  KVHeader header;

  int r = fread(&header, sizeof(KVHeader), 1, file);
  if (r != 1) {
    fclose(file);
    LM_W(("Not possible to read header for file %s", fileName.c_str()));
    return;
  }

  size_t fileSize = au::sizeOfFile(fileName);

  // Check file-size
  if (!header.CheckTotalSize(fileSize)) {
    LM_W(("Not correct size (%lu) while recovering block_id %lu from file %s"
          , fileSize
          , block_id
          , fileName.c_str()));

    fclose(file);
    return;
  }

  fclose(file);

  // Create the block
  BlockPointer block(new Block(block_id, &header));

  // Add this block
  {
    au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks
    block_ids_.push_back(block_id);
    blocks_.Set(block_id, block);
  }
}

void BlockManager::RecoverBlocksFromDisks() {
  // Recover all the blocks in current blocks directory
  std::string blocks_dir = au::Singleton<SamsonSetup>::shared()->blocks_directory();
  DIR *dp;
  struct dirent *dirp;

  std::vector<std::string> file_names;

  if ((dp = opendir(blocks_dir.c_str())) != NULL) {
    while ((dirp = readdir(dp)) != NULL) {
      std::string fileName = dirp->d_name;

      // Full path of the file
      std::string path = au::path_from_directory(blocks_dir, dirp->d_name);

      file_names.push_back(path);
    }
    closedir(dp);
  }
  for (size_t i = 0; i < file_names.size(); i++) {
    AU_LM_M(("Recovering data from file %lu/%lu %s", i + 1, file_names.size(), file_names[i].c_str()));

    struct ::stat info;
    stat(file_names[i].c_str(), &info);

    if (S_ISREG(info.st_mode)) {
      // Trying to recover block from disk....
      CreateBlockFromDisk(file_names[i]);
    }
  }
  Review();
}

void BlockManager::ScheduleRemoveOperation(BlockPointer block) {
  au::SharedPointer<engine::DiskOperation> operation(
                                                     engine::DiskOperation::newRemoveOperation(block->file_name(),
                                                                                               engine_id()));
  operation->environment.Set("block_id", block->block_id());

  engine::Engine::disk_manager()->Add(operation);
}

void BlockManager::ScheduleReadOperation(BlockPointer block) {
  // Only make sense if block is only on disk
  if (block->state() != Block::on_disk) {
    LM_W(("Called schedule_read_operation for a block (%lu) that is in another state %s"
            , block->block_id(), block->str_state().c_str()));
  }
  if (block->buffer() != NULL) {
    // No problem since previous buffer is auytomatically released in buffer_container
    LM_W(("There is an unused buffer of data in a block with state = on_disk"));   // Allocate a buffer ( it is retained since we are the creators )
  }
  size_t block_id = block->block_id();
  size_t size = block->getSize();

  // Alloc the buffer for the read operation
  block->buffer_ = engine::Buffer::Create(au::str("Buffer for block %lu", block_id), "block", size);
  block->buffer_->set_size(size);
  block->buffer_->SetTag("block_manager");

  // Read operation over this buffer
  std::string fileName = block->file_name();

  engine::DiskOperation *o = engine::DiskOperation::newReadOperation(fileName, 0
                                                                     , size
                                                                     , block->buffer()->GetSimpleBuffer()
                                                                     , engine_id());
  au::SharedPointer<engine::DiskOperation> operation(o);

  operation->environment.Set("block_id", block_id);
  operation->environment.Set("operation_size", size);
  engine::Engine::disk_manager()->Add(operation);

  scheduled_read_size_ += size;

  // Change the state to reading
  block->state_ = Block::reading;
}

void BlockManager::ScheduleWriteOperation(BlockPointer block) {
  // Only make sense if block is only on memory
  if (block->state() != Block::on_memory) {
    LM_W(("Called schedule_read_operation for a block (%lu) that is in another state %s"
            , block->block_id(), block->str_state().c_str()));   // Operation for writing
  }
  engine::BufferPointer buffer = block->buffer();
  size_t block_id = block->block_id();
  size_t size = block->getSize();

  if (block->getHeader().info.size + sizeof(KVHeader) != size) {
    LM_X(1, ("Internal error: Not valid block"));
  }
  std::string fileName = block->file_name();
  if (buffer == NULL) {
    LM_X(1, ("Internal error"));
  }
  engine::DiskOperation *o = engine::DiskOperation::newWriteOperation(buffer, fileName, engine_id());
  au::SharedPointer<engine::DiskOperation> operation(o);
  operation->environment.Set("block_id", block_id);
  operation->environment.Set("operation_size", size);

  engine::Engine::disk_manager()->Add(operation);

  scheduled_write_size_ += size;

  // Change state to writing
  block->state_ = Block::writing;
}

void BlockManager::ResetBlockManager() {
  au::TokenTaker tt(&token_);   // Mutex protection for the list of blocks

  // Clear dictionary and list of ids
  block_ids_.clear();
  blocks_.clear();

  LM_TODO(("Remove all files at BlockManager"));
}

void BlockManager::Sort() {
  // au::ExecesiveTimeAlarm alarm("BlockManager::_review");
  {
    au::ExecesiveTimeAlarm alarm("BlockManager::sort", 0.10);

    // Sort list of blocks according to a defined criteria
    block_ids_.sort(BlockSorter(blocks_));

    // Check sort
    size_t previous_task = 0;
    std::list<size_t>::iterator iterator;
    for (iterator = block_ids_.begin(); iterator != block_ids_.end(); iterator++) {
      BlockPointer block = blocks_.Get(*iterator);
      size_t task = block->min_task_id();
      if (task < previous_task) {
        LM_X(1, ("Error sorting blocks in blcok manager"));
      }
      previous_task = task;
    }
    // std::sort( block_ids_.begin() , block_ids_.end(), BlockSorter(blocks_) );
  }
}
  
  bool BlockManager::CheckBlocks( const std::set<size_t>& block_ids )
  {
    std::set<size_t>::const_iterator it;
    for (it = block_ids.begin() ; it != block_ids.end() ; it++ )
      if( GetBlock(*it) == NULL )
        return false;
    return true;
  }

}
}
