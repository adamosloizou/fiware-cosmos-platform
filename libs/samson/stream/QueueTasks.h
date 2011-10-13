

#ifndef _H_PARSER_QUEUE_TASK
#define _H_PARSER_QUEUE_TASK

#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/StreamOperation.h" 
#include "samson/stream/QueueTask.h"                // parent class 


namespace samson
{
    
    namespace stream
    {
        class StateItem;
        
        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class ParserQueueTask : public stream::QueueTask 
        {
            bool clear_inputs;
            
        public:
            
            ParserQueueTask( size_t id  , StreamOperation* streamOperation  ) : stream::QueueTask(id , streamOperation )
            {
                setProcessItemOperationName( "stream:" + streamOperation->operation );
            }
            
            ~ParserQueueTask()
            {
            }

            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );

            void finalize( StreamManager* streamManager );         
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        // ParserOut task is used only in popQueue operations
        
        
        class ParserOutQueueTask : public stream::QueueTask 
        {
            KVRange range;
            
        public:
            
            ParserOutQueueTask( size_t id  , StreamOperation* streamOperation , KVRange _range  ) : stream::QueueTask(id , streamOperation )
            {
                // Set operation name for debugging
                setProcessItemOperationName( "stream:" + streamOperation->operation );

                // Set the limits
                range = _range;
                
                // Change to txt mode ( not key-value )
                setProcessBaseMode(ProcessIsolated::txt);

            }
                        
            // Function to generate output key-values
            void generateTXT( TXTWriter *writer );
            
            void finalize( StreamManager* streamManager );         
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };
        
        

        // Parser QueueTask ( at the same time is the ProcessItem in the engine library to be executed )
        
        class MapQueueTask : public stream::QueueTask 
        {
            KVRange range;
            
        public:
            
            MapQueueTask( size_t id , StreamOperation* streamOperation , KVRange _range  ) :stream::QueueTask(id , streamOperation )
            {
                // Set operation name for debugging
                setProcessItemOperationName( "stream:" + streamOperation->operation );
                
                // Set the limits
                range = _range;
                
            }
            
            ~MapQueueTask()
            {
            }
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );

            void finalize( StreamManager* streamManager );         
            
            // Get a string with the status of this task
            virtual std::string getStatus();
            
        };

        class ReduceQueueTask : public stream::QueueTask 
        {
            
            KVRange range;                                  // Range of hash-groups
            std::vector<engine::Buffer*> outputBuffers;     // Ouput buffers generated by this operation in "update_state" mode

            bool update_state_mode;     // Spetial flag to indicate update_status mode
            int division;               // Division, this operation is processing

        public:
            
            ReduceQueueTask( size_t id , StreamOperation* streamOperation , KVRange range  );

            
            // Set mode state_update
            void setUpdateStateDivision( int _division );
            
            // Function to generate output key-values
            void generateKeyValues( KVWriter *writer );
            
            // Get a string with the status of this task
            std::string getStatus();
            
            void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish );
            
            void finalize( StreamManager* streamManager );         
            
        };         

        
        
    }    
}

#endif
