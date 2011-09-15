

#include "QueuesManager.h"  // Own interface
#include <string>           // std::string
#include <sstream>          // std::stringstream
#include "Queue.h"          // samson::stream::Queue
#include "Block.h"          // samson::stream::Block

#include "engine/Engine.h"              // engine::Engine
#include "engine/MemoryManager.h"
#include "engine/Notification.h"

#include "samson/common/Info.h"     // samson::Info

#include "samson/module/ModulesManager.h"   // ModulesManager

#include "samson/stream/PopQueue.h" // stream::PopQueue

#include "samson/worker/SamsonWorker.h"

#include "samson/stream/QueueTask.h"

#include "samson/stream/Queue.h"
#include "samson/stream/QueueItem.h"

#include "samson/stream/ParserQueueTask.h"            // samson::stream::ParserQueueTask


#include "BlockList.h"          // BlockList

#include "samson/stream/Queue.h"

namespace samson {
    namespace stream{
    
        QueuesManager::QueuesManager(::samson::SamsonWorker* _worker) : queueTaskManager( this )
        {
            worker = _worker;
            operation_list = NULL;
        }

        void QueuesManager::addBlocks( std::string queue_name ,  BlockList *bl )
        {
            // Get or create the queue
            Queue *queue = getQueue( queue_name );
            
            // Add the blocks to the queue ( no information about any particular task at the moment )
            queue->push( bl );

            // review all the automatic operations ( maybe we can only review affected operations in the future... )
            reviewStreamOperations();            
        }
     
        void QueuesManager::setOperationList( network::StreamOperationList *list )
        {
            if( operation_list) 
                delete operation_list;
            
            operation_list = new network::StreamOperationList();
            operation_list->CopyFrom( *list );
            
            
            // review all the operations...
            reviewStreamOperations();
        }
        
        
        void QueuesManager::remove_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
            {
                delete queue;
            }

        }
        
        void QueuesManager::pause_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
                queue->paused = true;
            
        }
        
        void QueuesManager::play_queue( std::string queue_name )
        {
            Queue *queue = queues.extractFromMap(queue_name);
            
            if( queue )
                queue->paused = false;
        }
        
        
        Queue* QueuesManager::getQueue( std::string queue_name )
        {
            Queue *queue = queues.findInMap( queue_name );
            if (! queue )
            {
                queue = new Queue( queue_name , this , 1 );
                queues.insertInMap( queue_name, queue );
            }
            
            return queue;
            
        }
        
        void QueuesManager::notifyFinishTask( QueueTask *task )
        {
            
        }
        
        void QueuesManager::notifyFinishTask( PopQueueTask *task )
        {
            // Recover identifiers
            size_t pop_queue_id = task->pop_queue_id;
            size_t task_id      = task->id;
            
            popQueueManager.notifyFinishTask( pop_queue_id , task_id , &task->error );
            
        }

        void QueuesManager::addPopQueue(const network::PopQueue& pq , size_t delilahId, int fromId )
        {
            // Create a new pop queue


            PopQueue *popQueue = new PopQueue( pq , delilahId, fromId );
            popQueueManager.add( popQueue );

            if( pq.has_queue() )
            {
                
                std::string queue_name = pq.queue();
                Queue* q = getQueue( queue_name );

                // Iterate thougth all the queue-items
                au::list< QueueItem >::iterator item;
                for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                {
                    
                    // Create a pop operation for each element in the blocks
                    au::list< Block >::iterator b;
                    for ( b = (*item)->list->blocks.begin() ; b != (*item)->list->blocks.end() ; b++ )
                    {
                        size_t id = queueTaskManager.getNewId();
                        
                        popQueue->addTask( id );
                        
                        PopQueueTask *tmp = new PopQueueTask( id , popQueue , (*item)->range ); 
                        
                        tmp->addBlock( *b );
                        
                        // Schedule tmp task into QueueTaskManager
                        queueTaskManager.add( tmp );
                    }
                }
            }
            popQueue->check();
        }
        

        // Get information for monitorization
        void QueuesManager::getInfo( std::ostringstream& output)
        {
            output << "<stream_manager>\n";
            
            output << "<queues>\n";

            au::map< std::string , Queue >::iterator q;
            for ( q = queues.begin() ; q != queues.end() ; q++ )
                q->second->getInfo(output);

            output << "</queues>\n";
            
            // Tasks
            output << "<queue_tasks>\n";
            queueTaskManager.getInfo( output );
            output << "</queue_tasks>\n";

            output << "</stream_manager>\n";
            
        }
        
        void QueuesManager::reviewStreamOperations()
        {
            // Review all the stream operations to see if a new squedule is necessary
            if( operation_list )
            {
                for ( int i = 0 ; i < operation_list->operation_size() ; i++)
                {
                    const network::StreamOperation& operation = operation_list->operation(i);
                    reviewStreamOperation( operation );
                }
            }
        }
        
        void QueuesManager::reviewStreamOperation(const network::StreamOperation& operation)
        {
            // Get the operation
            Operation* op = samson::ModulesManager::shared()->getOperation( operation.operation() );
            
            // No valid operation
            if( !op )
            {
                LM_W(("StreamOperation %s failed since operation %s is not valid",operation.name().c_str(), operation.operation().c_str()));
                return;
            }

            /*
            // Get the limit information for this operation    
            samson::Environment environment;
            copyEnviroment(operation.environment(), &environment);
            
            // Get limits to consider
            size_t max_time             = environment.getSizeT("max_time", 0);
            size_t max_size             = environment.getSizeT("max_size", 0);
            */
            
            switch (op->getType()) {
                    
                case Operation::parser:
                {
                    
                    //LM_W(("Reviwing StreamOperation parser %s " , operation.name().c_str()));
                    
                    if( operation.input_queues_size() != 1 )
                    {
                        //LM_W(("StreamOperation %s failed since has no input queue", operation.name().c_str()));
                        return;
                    }
                    
                    // Get the input queue
                    std::string input_queue_name = operation.input_queues(0);
                    Queue *q = getQueue( input_queue_name );
                    
                    if( q->items.size() != 1 )
                        LM_W(("Queue to be processed by a parser has got more that one items. This is a strange behavious"));
                    
                    // Iterate thougth all the queue-items
                    au::list< QueueItem >::iterator item;
                    for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                    {
                        QueueItem *queueItem = *item;
                        
                        while( !queueItem->list->isEmpty() )
                        {
                            
                            // Create a parser queue tasks
                            ParserQueueTask *tmp = new ParserQueueTask( queueTaskManager.getNewId() , operation ); 
                            tmp->setOutputFormats( &op->outputFormats );
                            
                            // Extract the rigth blocks from queue
                            tmp->list->extractFrom( queueItem->list , 1000000000 );
                            
                            // Schedule tmp task into QueueTaskManager
                            queueTaskManager.add( tmp );
                            
                        }
                    }
                    
                }
                    break;

                case Operation::parserOut:
                {
                    
                    //LM_W(("Reviwing StreamOperation parser %s " , operation.name().c_str()));
                    
                    if( operation.input_queues_size() != 1 )
                    {
                        //LM_W(("StreamOperation %s failed since has no input queue", operation.name().c_str()));
                        return;
                    }
                    
                    // Get the input queue
                    std::string input_queue_name = operation.input_queues(0);
                    Queue *q = getQueue( input_queue_name );
                    
                    // Iterate thougth all the queue-items
                    au::list< QueueItem >::iterator item;
                    for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                    {
                        
                        QueueItem *queueItem = *item;
                        
                        while( !queueItem->list->isEmpty() )
                        {
                            //Create the parserout operation
                            ParserOutQueueTask *tmp = new ParserOutQueueTask( queueTaskManager.getNewId() , operation , (*item)->range ); 
                            
                            // Extract the rigth blocks from queue
                            tmp->list->extractFrom( queueItem->list , 1000000000 );
                            
                            // Schedule tmp task into QueueTaskManager
                            queueTaskManager.add( tmp );
                            
                        }
                    }
                    
                }
                    break;
                    
                case Operation::map:
                {
                    
                    if( operation.input_queues_size() != 1 )
                    {
                        //LM_W(("StreamOperation %s failed since has no input queue", operation.name().c_str()));
                        return;
                    }
                    
                    // Get the input queue
                    std::string input_queue_name = operation.input_queues(0);
                    Queue *q = getQueue( input_queue_name );
                    
                    // Iterate thougth all the queue-items
                    au::list< QueueItem >::iterator item;
                    for ( item = q->items.begin() ; item != q->items.end() ; item++ )
                    {
                        
                        QueueItem *queueItem = *item;
                        
                        while( !queueItem->list->isEmpty() )
                        {
                            // Create the map operation
                            MapQueueTask *tmp = new MapQueueTask( queueTaskManager.getNewId() , operation , (*item)->range ); 
                            tmp->setOutputFormats( &op->outputFormats );
                            
                            // Extract the rigth blocks from queue
                            tmp->list->extractFrom( queueItem->list , 1000000000 );
                            
                            // Schedule tmp task into QueueTaskManager
                            queueTaskManager.add( tmp );
                            
                        }
                    }
                    
                }
                    break;
                case Operation::reduce:
                {

                    if( operation.input_queues_size() != 2 )
                    {
                        LM_W(("StreamOperation %s failed since reduce has not 2 inputs", operation.name().c_str()));
                        return;
                    }
                    
                    
                    // Get "pre input queue" and input queue
                    Queue *inputQueue = getQueue( operation.input_queues(0) );
                    
                    // Get state state associated with the second input
                    Queue *stateQueue = getQueue( operation.input_queues(1) );
  
                    if( stateQueue->paused )
                    {
                        // no new tasks since state queue is paused
                        return;
                    }

                    // distribute the input queue in the same way state queue ( same items )
                    inputQueue->distributeItemsAs( stateQueue );

                    

                    au::list< QueueItem >::iterator input_item = inputQueue->items.begin();
                    au::list< QueueItem >::iterator state_item = stateQueue->items.begin();
                    
                    for (  ; state_item != stateQueue->items.end() ; input_item++ , state_item++  )
                    {
                        QueueItem * stateItem = *state_item;
                        QueueItem * inputItem = *input_item;
                        
                        if( ( !stateItem->isWorking() )  && ( !inputItem->list->isEmpty() ) )
                        {
                            // Create a reduce task with all possible input
                            ReduceQueueTask *tmp = new ReduceQueueTask( queueTaskManager.getNewId() , operation , stateItem , stateItem->range ); 
                            
                            // Set the format of the outputs
                            tmp->setOutputFormats( &op->outputFormats );
                            
                            // Setup content of this task
                            tmp->getBlocks( inputItem->list , stateItem->list );
                            
                            // Set this item to "running mode"
                            stateItem->setRunning( tmp );
                            
                            // Schedule tmp task into QueueTaskManager
                            queueTaskManager.add( tmp );
                            
                        }
                        
                    }
                    
/*
                    
                    size_t max_state_item_size = SamsonSetup::shared()->getUInt64("stream.max_state_item_size");
                    
                    // Check all the stateItems here to run new operations
                    // Check all the elements in the list
                    au::list< StateItem >::iterator item;
                    for (item = state->items.begin() ; item != state->items.end() ; item++ )
                    {
                        StateItem *stateItem = (*item);
                        
                        int hg_begin = stateItem->hg_begin;
                        int hg_end = stateItem->hg_end;
                        int hg_mid = ( hg_begin + hg_end ) / 2;
                        
                        if( stateItem->isReadyToRun() )
                        {
                            
                            if( stateItem->state->getSize() > max_state_item_size )
                            {
                                ReduceQueueTask *tmp = new ReduceQueueTask( queueTaskManager.getNewId() , operation , stateItem , hg_begin , hg_mid ); 
                                ReduceQueueTask *tmp2 = new ReduceQueueTask( queueTaskManager.getNewId() , operation , stateItem , hg_mid , hg_end ); 
                                
                                // Set the format of the outputs
                                tmp->setOutputFormats( &op->outputFormats );
                                tmp2->setOutputFormats( &op->outputFormats );
                                
                                // Setup content of this task ( not that input is common for both operations )
                                BlockList tmp_list("tmp input for state");
                                BlockList tmp_list2("tmp input for state 2");
                                tmp_list.extractFrom ( stateItem->input , 100000000 );
                                tmp_list2.copyFrom ( &tmp_list );

                                tmp->getBlocks( &tmp_list  , stateItem->state );
                                tmp2->getBlocks( &tmp_list2  , stateItem->state );
                                
                                // Set this item to "running mode"
                                stateItem->setRunning( tmp , tmp2 );
                                
                                // Schedule tmp task into QueueTaskManager
                                queueTaskManager.add( tmp );
                                queueTaskManager.add( tmp2 );
                                
                            }
                            else
                            {
                            
                                ReduceQueueTask *tmp = new ReduceQueueTask( queueTaskManager.getNewId() , operation , stateItem , hg_begin , hg_end ); 
                                
                                // Set the format of the outputs
                                tmp->setOutputFormats( &op->outputFormats );
                                
                                // Setup content of this task
                                tmp->getBlocks( stateItem->input , stateItem->state );
                                
                                // Set this item to "running mode"
                                stateItem->setRunning( tmp );
                                
                                // Schedule tmp task into QueueTaskManager
                                queueTaskManager.add( tmp );
                            }
                            
                        }
                    }
*/
                    
                    
                }
                    break;
                  
                    
                default:
                    // Not supported operation at the moment
                    break;
            }
            
        }
    }
}
