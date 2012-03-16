#ifndef _H_BLOCK_MANAGER
#define _H_BLOCK_MANAGER

#include "Block.h"                      // samson::stream::Block
#include "au/list.h"

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/Object.h"  // engien::EngineListener
#include "engine/Object.h"              // engine::Object

#include <vector>

namespace samson {
    namespace stream {
        
        class BlockList;
        
        /**
         Manager of all the blocks running on the system
         */
        
        bool compare_blocks( Block*b1 , Block *b2 );

        
        
        class BlockManager : public engine::Object 
        {
            
            au::list<Block> blocks;         // List of blocks in the system ( ordered by priority / creation time )
            
            BlockManager();                 // Private constructor for singleton implementation
            ~BlockManager();

            size_t worker_id;               // Identifier of the worker in the cluster
            size_t id;                      // Next id to give to a block

            int num_writing_operations;     // Number of writing operations ( low priority blocks )
            int num_reading_operations;     // Number of reading operations ( high priority blocks )
            
            size_t memory;                  // Total amount of memory used by all blocks
            size_t max_memory;              // Maximum amount of memory to always kept
            
        public:

            // Singleton 
            static void init();
            static BlockManager* shared(); 
            static void destroy();
            
        public:

            // Auxiliar function to rise the next block identifier to not colide with previous blocks
            void setMinimumNextId( size_t min_id );
            
            // Update the worker id
            void resetBlockManager( size_t _worker_id )
            {
                worker_id = _worker_id;
                
                // Remover all blocks
                blocks.clearList();
                
                // Remove all files...
                LM_TODO(("Remove all files at BlockManager"));
            }
            
            // Function to get a new id for a block
            size_t getNextBlockId();
            size_t getWorkerId();
            
            // Add a block to the block manager
            // It is assumed block is NOT inside the list "blocks"
            
            void insert( Block* b );

            // Function to review pending read / free / write operations
            void review();
            
        public:
            
            // Get a particular block ( only for debugging )
            Block* getBlock( size_t _id );

        public:
            
            virtual void notify( engine::Notification* notification );

        private:
            
            void _freeMemoryWithLowerPriorityBLocks( Block *b );

            
        public:
            
            void update( BlockInfo &block_info );
            void getInfo( std::ostringstream& output);
            
            network::Collection* getCollectionOfBlocks( Visualization* visualization );
            
        };
    }
}

#endif
