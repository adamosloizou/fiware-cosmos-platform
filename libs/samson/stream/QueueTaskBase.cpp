
#include "samson/common/coding.h"

#include "BlockManager.h"       // samson::stream::BlockManager
#include "Block.h"              // samson::Stream::Block

#include "QueueTaskBase.h"      // Own interface

namespace samson {
    namespace stream{
        
        
        QueueTaskBase::QueueTaskBase( size_t _id )
        {
            // Set the id of this task
            id = _id;
            
            // Create the lock block list to put all inputs when ready...
            lockBlockList  = new BlockList( au::str("Task %lu input lock" , id ) , id , true );
            
            ready_flag = false;
            
            
            queue_task_state = "Init";
        }
        
        QueueTaskBase::~QueueTaskBase()
        {
            blockLists.clearMap();
            delete lockBlockList;
        }
        
        bool QueueTaskBase::ready()
        {
            if( ready_flag )
                return true;
            
            au::map<std::string, BlockList >::iterator b;
            for ( b = blockLists.begin() ; b != blockLists.end() ; b++ )
                if( !b->second->isContentOnMemory() )
                    return false;
            
            
            // Put all the input blocks in the lock list to make sure it is in memoryt while running...
            for ( b = blockLists.begin() ; b != blockLists.end() ; b++ )
                lockBlockList->copyFrom( b->second );
            
            // Set the flag to true
            ready_flag =  true;
            
            return true;
        }  
        
        BlockList* QueueTaskBase::getBlockList( std::string name )
        {
            BlockList* blockList = blockLists.findInMap( name );
            if( !blockList )
            {
                blockList  = new BlockList( au::str("QueueTaskBase %lu input %s" , id , name.c_str() ) , id , false );
                blockLists.insertInMap(name, blockList );
            }
            
            return blockList;
        }
        
        
        void QueueTaskBase::getInfo( std::ostringstream& output)
        {
            au::xml_open(output, "inputs");
            au::map<std::string, BlockList >::iterator b;
            for ( b = blockLists.begin() ; b != blockLists.end() ; b++ )
            {
                au::xml_open(output, "input" );
                
                au::xml_simple( output , "name" , b->first );

                b->second->getInfo( output );

                au::xml_close(output, "input" );
            
            }
            au::xml_close(output, "inputs");
        }        
        
        void QueueTaskBase::update( BlockInfo &block_info)
        {
            au::map<std::string, BlockList >::iterator b;
            for ( b = blockLists.begin() ; b != blockLists.end() ; b++ )
                b->second->update( block_info );
            
        }
        
        size_t QueueTaskBase::getId()
        {
            return id;
        }

    }
}
