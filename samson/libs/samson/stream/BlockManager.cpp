#include <sys/types.h>
#include <dirent.h>


#include "au/file.h"
#include "au/ExecesiveTimeAlarm.h"
#include "logMsg/traceLevels.h"

#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup

#include "samson/common/KVHeader.h"

#include "Block.h"          
#include "BlockList.h"      
#include "BlockManager.h"   

/*
 Note: The idea is now schedule all read and write operations until max size
 and wait for scheduled read and write goes down to 0
 */

#define notification_review_block_manager "notification_review_block_manager"

namespace samson {
  namespace stream {

    class BlockSorter
    {
      
      const au::Dictionary<size_t, Block>& blocks_;
      
    public:
      
      BlockSorter( const au::Dictionary<size_t, Block>& blocks )
      : blocks_( blocks )
      {
      }
      
      bool operator()(size_t block_id1, size_t block_id2) const
      {
        au::SharedPointer<Block> block_1 = blocks_.Get( block_id1 );
        au::SharedPointer<Block> block_2 = blocks_.Get( block_id2 );
        
        return Block::compare( block_1.shared_object() , block_2.shared_object() );
      }
      
    };
    
    BlockManager *blockManager = NULL;
    
    void BlockManager::init()
    {
      if( blockManager )
        LM_X(1, ("Error with init BlockManager (double init)"));
      
      blockManager = new BlockManager();
      blockManager->recover_blocks_from_disks(); // Recover blocks from disk
    }
    
    
    BlockManager::BlockManager()
    {
      // Default values for read/write scheduling
      scheduled_write_size = 0;
      scheduled_read_size = 0;
      
      // Maximum amount of memory
      max_memory = (double)SamsonSetup::shared()->getUInt64("general.memory")*0.8;  // 60% of memory for block manager
      
      // Notification to review block manager
      listen(notification_review_block_manager);
      engine::Engine::shared()->notify( new engine::Notification(notification_review_block_manager) , 1 );
    }
    
    BlockManager::~BlockManager()
    {
    }
    
    void BlockManager::create_block( size_t block_id , engine::BufferPointer buffer )
    {
      if( buffer == NULL )
        LM_X(1, ("Internal error"));
      
      BlockPointer block( new Block( block_id , buffer ) );
      
      // Add this block
      block_ids_.push_back(block_id);
      blocks_.Set(block_id, block);
      
      // Review where this block goes...
      review();
    }
    
    void BlockManager::create_block( size_t block_id , KVHeader* header )
    {
      BlockPointer block( new Block( block_id, header ) );
      
      // Add this block
      block_ids_.push_back(block_id);
      blocks_.Set(block_id, block);

      // Review where this block goes...
      review();
    }
    
    void BlockManager::RemoveBlocksNotIncluded( const std::set<size_t>& blocks_ids )
    {
      std::list<size_t>::iterator it;
      for ( it = block_ids_.begin() ; it != block_ids_.end() ; )
      {
        // Recover block
        size_t block_id = *it;
        BlockPointer block = blocks_.Get(block_id);
        
        // Get creation time ( do not remove young blocks )
        size_t time = block->creation_time();

        // Check if this block is in the provided list
        bool isUsed = blocks_ids.find( block_id ) != blocks_ids.end();
        
        if(  block->canBeRemoved() && !isUsed && (time > 60*10 ) )
        {
          // Remove this block
          it = block_ids_.erase(it);
          blocks_.Extract(block_id);
          
          schedule_remove_operation( block );
        }
        else
          it++;
      }
    }
    
    std::set<size_t> BlockManager::GetBlockIds()
    {
      std::set<size_t> block_ids;
      std::list<size_t>::iterator it;
      for ( it = block_ids_.begin() ; it != block_ids_.end() ; it++ )
        block_ids.insert( *it );
      return block_ids;
    }
    
    std::set<size_t> BlockManager::GetPendingBlockIds( const std::set<size_t>& block_ids )
    {
      std::set<size_t> pending_block_ids;
      std::set<size_t>::iterator it;
      for (it = block_ids.begin() ; it != block_ids.end() ; it++ )
      {
        if( !blocks_.ContainsKey( *it ) )
          pending_block_ids.insert( *it );
      }
      return pending_block_ids;
    }
    
    void BlockManager::destroy()
    {
      if( !blockManager )
      {
        LM_W(("Error destroying a non-initialized BlockManager"));
        return;
      }
      
      delete blockManager;
      blockManager = NULL;
    }
    
    BlockManager* BlockManager::shared()
    {
      if( !blockManager )
        LM_X(1, ("Init BlockManager before using it"));
      return blockManager;
    }
    
    void BlockManager::notify( engine::Notification* notification )
    {
      if ( notification->isName( notification_review_block_manager ) )
      {
        review();
      } 
      else if ( notification->isName( notification_disk_operation_request_response ) )
      {
        
        std::string type      = notification->environment().get("type", "-");
        size_t operation_size = notification->environment().get("operation_size", 0 );
        size_t block_id       = notification->environment().get("block_id", 0 );
        
        // Recover the block
        BlockPointer block = blocks_.Get(block_id);

        
        if( block == NULL )
        {
          LM_W(("Notification for non existing block %lu.Ignoring..." , block_id));
          return;
        }
        
        LM_T(LmtBlockManager,("Received a disk notification with type %s and size %s" 
                              , type.c_str() 
                              , au::str( operation_size ).c_str() ));
        
        if(  type == "write" )
        {
          scheduled_write_size -= operation_size;
          block->state_ = Block::ready;
          review();
        }
        else if( type == "read" )
        {
          scheduled_read_size -= operation_size;
          block->state_ = Block::ready;
          review();
        }
        else if( type == "remove" )
        {
          // Nothing to do here...
        }
      }
      else 
      {
        LM_W(("Unknown notification at BlockManager"));
      }
      
    }
    
    void BlockManager::review()
    {
      // Get setup parameter to control review of BlockManager...
      size_t max_scheduled_write_size = SamsonSetup::shared()->getUInt64("stream.max_scheduled_write_size");
      size_t max_scheduled_read_size = SamsonSetup::shared()->getUInt64("stream.max_scheduled_read_size");
      
      // No schedule new operations until all the previous one have finished
      if( scheduled_read_size > 0 )
        return;
      if( scheduled_write_size > 0 )
        return;
      
      LM_T( LmtBlockManager , ("Reviewing block manager"));
      
      // If no blocks, nothing to do...
      if ( blocks_.size() == 0 )
        return;

      // --------------------------------------------------------------------------------
      // Sort by priorities
      // --------------------------------------------------------------------------------
      
      //au::ExecesiveTimeAlarm alarm("BlockManager::_review");
      {
        au::ExecesiveTimeAlarm alarm("BlockManager::sort" , 0.10 );
        
        // Sort list of blocks according to a defined criteria
        block_ids_.sort( BlockSorter(blocks_) );
        //std::sort( block_ids_.begin() , block_ids_.end(), BlockSorter(blocks_) );
      }
      
      // --------------------------------------------------------------------------------
      // Find the blocks that should be in memory ( all until "limit_block" )
      // --------------------------------------------------------------------------------
      
      LM_T( LmtBlockManager , ("Detect limit_block"));
      size_t accumulated_memory = 0;
      size_t last_block_id_in_memory = *block_ids_.begin();
      std::list<size_t>::iterator b;
      for ( b = block_ids_.begin() ; b != block_ids_.end() ; b++ )
      {
        size_t block_id = *b;
        BlockPointer block = blocks_.Get(block_id);
        
        if( block == NULL )
          LM_X(1,("Internal error"));

        if( (accumulated_memory + block->getSize()) > max_memory )
          break;
        
        accumulated_memory += block->getSize();
        last_block_id_in_memory = block_id;
      }
      
      LM_T(LmtBlockManager,("Block limit %lu" , last_block_id_in_memory ));
      
      // --------------------------------------------------------------------------------
      
      // --------------------------------------------------------------------------------
      // Free memory of blocks that are not suppouse to be on memory
      // --------------------------------------------------------------------------------
      {
        LM_T(LmtBlockManager,("Free blocks in memory under block_limit"));
        std::list<size_t>::reverse_iterator b;
        for ( b = block_ids_.rbegin() ; b != block_ids_.rend() ; b++ )
        {
          // Considering this block
          size_t block_id = *b;
          BlockPointer block =  blocks_.Get(block_id);
          
          // Stop when arrive to the limit block ( this should be in memory )
          if( block_id == last_block_id_in_memory )
            break;
          
          if( block->state() == Block::ready )  // Both on disk and on memory
          {
            LM_T(LmtBlockManager,("Free block:'%s'", block->str().c_str()));
            
            // Free block
            block->freeBlock();
          }
        }
      }
      
      // --------------------------------------------------------------------------------
      // Schedule write operations
      // --------------------------------------------------------------------------------
      
      if( scheduled_write_size < max_scheduled_write_size )
      {
        LM_T( LmtBlockManager , ("Schedule write operations"));
        // Lock for new write operations...
        std::list<size_t>::reverse_iterator b;
        for ( b = block_ids_.rbegin() ; b != block_ids_.rend() ; b++ )
        {
          // Considering this block
          size_t block_id = *b;
          BlockPointer block =  blocks_.Get(block_id);
          
          if( block->state() == Block::on_memory )
          {
            LM_T(LmtBlockManager,("Schedule write for block:'%s'", block->str().c_str()));
            
            // Schedule write
            schedule_write_operation(block);
            
            LM_T(LmtBlockManager,("Write block %lu" , block->get_block_id() ));
            
            // No continue for more writes
            if ( scheduled_write_size >= max_scheduled_write_size )
              break;
          }
        }
      }
      
      // --------------------------------------------------------------------------------
      // Schedule read operations
      // --------------------------------------------------------------------------------
      
      // Schedule new reads operations ( high priority elements ) if available memory
      if( scheduled_read_size < max_scheduled_read_size )
      {
        LM_T( LmtBlockManager , ("Schedule read operations"));
        // Lock for new write operations...
        std::list<size_t>::iterator b;
        for ( b = block_ids_.begin() ; b != block_ids_.end() ; b++ )
        {
          
          // Considering this block
          size_t block_id = *b;
          BlockPointer block =  blocks_.Get(block_id);
          
          if( block->state() == Block::on_disk ) // Needed to be loaded...
          {
            LM_T(LmtBlockManager,("Trying to read block'%s'", block->str().c_str()));
            
            // Read the block
            schedule_read_operation(block);
            
            LM_T(LmtBlockManager,("Scheduling read for block:'%s'", block->str().c_str()));
            
            // No continue for more writes
            if( scheduled_read_size > max_scheduled_read_size )
              break;
          }
          
          // No schedule read operations over the block limit
          if( block_id == last_block_id_in_memory )
          {
            LM_T(LmtBlockManager, ("Stops looking for read, because block_limit reached"));
            break; // Not schedule reads on blocks that are not suppose to be on memory
          }
        }
      }
      
    }
    
    BlockPointer BlockManager::getBlock( size_t block_id )
    {
      return blocks_.Get(block_id);
    }
    
    void BlockManager::update( BlockInfo &block_info )
    {
      std::list<size_t>::iterator b;
      for ( b = block_ids_.begin() ; b != block_ids_.end() ; b++ )
      {
        // Considering this block
        size_t block_id = *b;
        BlockPointer block =  blocks_.Get(block_id);
        
        block->update( block_info );
      }
    }
    
    gpb::Collection* BlockManager::getCollectionOfBlocks( const Visualization& visualization )
    {
      // Sort the blocks to make sure it reflecs real order
      block_ids_.sort( BlockSorter(blocks_) );
      
      gpb::Collection* collection = new gpb::Collection();
      collection->set_name("blocks");
      
      std::list<size_t>::iterator b;
      for ( b = block_ids_.begin() ; b != block_ids_.end() ; b++ )
      {
        // Considering this block
        size_t block_id = *b;
        BlockPointer block =  blocks_.Get(block_id);
        
        block->fill( collection->add_record() , visualization );
      }            
      
      return collection;            
    }
    
    void BlockManager::create_block_from_disk( const std::string& fileName )
    {
      //std::string fileName =  SamsonSetup::shared()->blockFileName( id );
      size_t block_id = SamsonSetup::shared()->blockIdFromFileName( fileName );
      
      if( block_id == 0 )
      {
        LM_W(("Not possible to get ids for file %s to recover block" , fileName.c_str() ));
        return;
      }
      
      FILE *file = fopen( fileName.c_str() , "r" );
      if(!file)
      {
        LM_W(("Not possible to open file %s to recover block" , fileName.c_str() ));
        return;
      }
      
      // Read header from the file...
      KVHeader header;
      
      int r = fread( &header , sizeof(KVHeader) , 1 , file );
      if( r != 1 )
      {
        fclose( file );
        LM_W(("Not possible to read header for file %s" , fileName.c_str() ));
        return;
      }
      
      size_t fileSize = au::sizeOfFile( fileName );
      
      // Check file-size
      if ( !header.check_size( fileSize ) )
      {
        LM_W(("Not correct size while recovering block_id %lu from file %s" 
              , block_id 
              , fileName.c_str() ));
        
        fclose( file );
        return;
      }
      
      fclose( file );
      
      // Create the block
      create_block(block_id , &header );
    }
    
    void BlockManager::recover_blocks_from_disks()
    {
      
      // Recover all the blocks in current blocks directory
      std::string blocks_dir = SamsonSetup::shared()->blocksDirectory();
      DIR *dp;
      struct dirent *dirp;
      if((dp  = opendir( blocks_dir.c_str() )) != NULL) 
      {
        while ((dirp = readdir(dp)) != NULL) 
        {
          std::string fileName = dirp->d_name;
          
          // Full path of the file
          std::string path = au::path_from_directory( blocks_dir , dirp->d_name );
          
          struct ::stat info;
          stat(path.c_str(), &info);
          
          if( S_ISREG(info.st_mode) )
          {
            // Trying to recover block from disk....
            create_block_from_disk( path );
          }
        }                                
        closedir(dp);                
      }
      
    }
    
    void BlockManager::schedule_remove_operation( BlockPointer block )
    {
      au::SharedPointer< engine::DiskOperation> operation( engine::DiskOperation::newRemoveOperation( block->file_name() , getEngineId() ) );
      operation->environment.set("block_id", block->get_block_id());
      
      engine::DiskManager::shared()->Add( operation );
    }
    
    void BlockManager::schedule_read_operation( BlockPointer block )
    {
      // Only make sense if block is only on disk
      if( block->state() != Block::on_disk )
      {
        LM_W(("Called schedule_read_operation for a block (%lu) that is in another state %s" 
              , block->get_block_id() , block->str_state().c_str() ));
      }
      
      if ( block->buffer() != NULL )
      {
        // No problem since previous buffer is auytomatically released in buffer_container
        LM_W(("There is an unused buffer of data in a block with state = on_disk"));
      }
      
      // Allocate a buffer ( it is retained since we are the creators )
      std::string buffer_title = au::str("read block %lu" , block->get_block_id() );
      size_t block_id = block->get_block_id();
      size_t size = block->getSize();
      
      // Alloc the buffer for the read operation
      block->buffer_ = engine::Buffer::create( buffer_title, "block", size );
      block->buffer_->setSize( size );
      
      // Read operation over this buffer
      std::string fileName = block->file_name();
      
      
      engine::DiskOperation* o =engine::DiskOperation::newReadOperation( fileName
                                                                        , 0
                                                                        , size
                                                                        , block->buffer()->getSimpleBuffer()
                                                                        , getEngineId() );
      au::SharedPointer< engine::DiskOperation> operation( o );
                                                          
      operation->environment.set("block_id" , block_id );
      operation->environment.set("operation_size" , size );
      engine::DiskManager::shared()->Add( operation );
      
      scheduled_read_size += size;
      
      // Change the state to reading
      block->state_ = Block::reading;
    }
    
    void BlockManager::schedule_write_operation( BlockPointer block )
    {
      
      // Only make sense if block is only on memory
      if( block->state() != Block::on_memory )
      {
        LM_W(("Called schedule_read_operation for a block (%lu) that is in another state %s" 
              , block->get_block_id() , block->str_state().c_str() ));
      }
      
      // Operation for writing
      engine::BufferPointer buffer = block->buffer();
      size_t block_id = block->get_block_id();
      size_t size = block->getSize();
      
      if( block->getHeader().info.size + sizeof(KVHeader)  != size )
        LM_X(1, ("Internal error: Not valid block"));
      
      std::string fileName = block->file_name();
      if( buffer == NULL )
        LM_X(1,("Internal error"));
      
      engine::DiskOperation* o = engine::DiskOperation::newWriteOperation( buffer
                                                                          , fileName
                                                                          , getEngineId() );
      au::SharedPointer<engine::DiskOperation> operation( o );
      operation->environment.set("block_id" , block_id );
      operation->environment.set("operation_size" , size );
      
      engine::DiskManager::shared()->Add( operation );
      
      scheduled_write_size += size;
      
      // Change state to writing
      block->state_ = Block::writing;
      
    }
    
    void BlockManager::resetBlockManager( )
    {
      // Clear dictionary and list of ids
      block_ids_.clear();
      blocks_.clear();
      
      LM_TODO(("Remove all files at BlockManager"));
    }

    
    
  }
}
