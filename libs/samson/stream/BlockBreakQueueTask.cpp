
#include "logMsg/logMsg.h"                          // LM_M , LM_X

#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList
#include "Queue.h"                                  // samson::stream::Queue
#include "StreamManager.h"                          // samson::stream::StreamManager

#include "BlockBreakQueueTask.h"                    // Own interface

namespace samson {
    namespace stream {
        
        
#pragma mark BlockBreakQueueTask        

        
        BlockBreakQueueTask::BlockBreakQueueTask( size_t _id , std::string _queue_name )
        : SystemQueueTask( _id , au::str("BlockBreak ( %lu )" , _id ) )
        {

            queue_name = _queue_name;
            setProcessItemOperationName( "system.BlockBreak" );
        }
        
        void BlockBreakQueueTask::run()
        {
            // Get the input list of blocks
            BlockList *list = getBlockList("input");
            
            // Get all pointers to correct datas ( using KVFile structure )
            au::list<Block>::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                files.push_back( KVFile( (*b)->buffer->getData() ) );

            
            // Compute the size for each hg
            FullKVInfo *info = (FullKVInfo*) malloc( KVFILE_NUM_HASHGROUPS * sizeof(FullKVInfo) );
            
            for( int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++ )
            {
                info[hg].clear();
                for( size_t f = 0 ; f < files.size() ; f++ )
                    info[hg].append( files[f].info[hg]  );
            }

            // Create 64Mb ouptut blocks
            size_t accumulated_size = info[0].size;
            int from_hg = 0;
            int to_hg = 1;
            
            while( to_hg < ( KVFILE_NUM_HASHGROUPS - 1 ) )
            {
                if( ( accumulated_size + info[to_hg].size ) > (64*1024*1024) )
                {
                    createBlock( from_hg, to_hg+1 );
                    from_hg = to_hg + 1;
                    to_hg = from_hg;
                    accumulated_size = 0;
                }
                else
                {
                    accumulated_size += info[to_hg].size;
                    to_hg++;
                }
            }

            // Create the last block
            if( to_hg > from_hg )
                createBlock( from_hg, KVFILE_NUM_HASHGROUPS );
            
            
            free(info);
            
        }
        
        void BlockBreakQueueTask::createBlock( int hg_begin , int hg_end )
        {

            // Compute size of the generated buffer
            KVInfo info;
            for( int hg = hg_begin ; hg < hg_end ; hg++ )
                for (size_t f = 0 ; f < files.size() ; f++ )
                    info.append( files[f].info[hg] );
            
            if ( info.size == 0 )
                return; // No block is generated
            
            // total size of the new block
            size_t size = sizeof( KVHeader ) + sizeof(KVInfo)*KVFILE_NUM_HASHGROUPS + info.size;
            
            // Alloc buffer
            engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "block_break", size , MemoryBlocks );
            buffer->setSize( size );

            // KVFile to work with this file
            KVFile file( buffer->getData() );
            
            // Header copying the header of the first one
            memcpy( file.header , files[0].header ,  sizeof( KVHeader ) );
            file.header->info = info;
            
            //KVInfo vector for this buffer
            clearKVInfoVector( file.info );
            
            size_t offset = 0;
            for (int hg = hg_begin ; hg < hg_end ; hg++)
            {
                for (size_t f = 0 ; f < files.size() ; f++ )
                {
                    memcpy(file.data + offset, files[f].data + files[f].offset(hg) , files[f].info[hg].size );
                    offset += files[f].info[hg].size;
                    
                    file.info[hg].append( files[f].info[hg] );
                }
            }
            
            file.header->range = KVRange(hg_begin , hg_end );
            
            // Collect the output buffer
            outputBuffers.push_back(buffer);
            
        }
        
        void BlockBreakQueueTask::finalize( StreamManager* streamManager )
        {
        	LM_T(LmtBlockManager,("BlockBreakQueueTask::starts finalize for (%lu) (%s) outputBuffers.size():%d, creating blocks", id, concept.c_str(), outputBuffers.size()));

            // Create the list with the outputs
            BlockList tmp("tmp_block_break_outputs");

            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                tmp.createBlock( outputBuffers[i] );

            // Get the queue and push generated blocks
            Queue*queue = streamManager->getQueue( queue_name );
            if ( queue )
                queue->push(&tmp);
            
            LM_T(LmtBlockManager,("BlockBreakQueueTask::ends finalize for (%lu) (%s) outputBuffers.size():%d, creating blocks", id, concept.c_str(), outputBuffers.size()));
        }
        
        
    }
    
}
