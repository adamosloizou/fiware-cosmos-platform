

#include "State.h"      // samson::stream::State

#include "BlockList.h"  // samson::stream::BlockList

#include "StateItem.h"  // Own interface



namespace samson {

namespace stream
    {
        
        StateItem::StateItem( State *_myState , int _hg_begin , int _hg_end )
        {

            myState = _myState;
            
            hg_begin = _hg_begin;
            hg_end  = _hg_end;
            
            input = new BlockList( _myState->name + "_list" );
            state = new BlockList( _myState->name + "_state" );;      
            
            future_state = new BlockList( _myState->name + "_future_state" );
            future_state2 = new BlockList( _myState->name + "_future_state" );
            
            // Initial state to ready 
            mode = ready;
            
            task = NULL;
            task2 = NULL;
            
        }
        
        StateItem::~StateItem()
        {
            delete input;
            delete state;
            delete future_state;
            
        }
     
        void StateItem::push( BlockList *_list )
        {
            //LM_M(("Pushing block list to state_item"));

            input->copyFrom(_list);
        }

        void StateItem::getInfo( std::ostringstream& output)
        {
            output << "<state_item>\n";
            output << "<hg_begin>" << hg_begin << "</hg_begin>";
            output << "<hg_end>" << hg_end << "</hg_end>";

            switch (mode) {
                case ready:   
                    output << "<mode>running</mode>\n"; 
                    break;
                case running:   
                    output << "<mode>running</mode>\n"; 
                    break;
                case running_dividing:   
                    output << "<mode>running_dividing</mode>\n"; 
                    break;
            }
            
            output << "<list_input>";
            input->getInfo(output);
            output << "</list_input>\n";

            output << "<list_state>";
            state->getInfo(output);
            output << "</list_state>\n";

            output << "<list_future_state>";
            future_state->getInfo(output);
            output << "</list_future_state>\n";
            
            output << "</state_item>\n";
        }

        FullKVInfo StateItem::getFullKVInfo()
        {
            return input->getFullKVInfo();
        }
        
        bool StateItem::isReadyToRun()
        {
            if (mode != ready)
                return false;
            if( input->getSize() <= 0)
                return false;
            
            return true;
            
        }
        
        void StateItem::setRunning( ReduceQueueTask* _task )
        {
            // Set this as the task that is currently running ( or at least scheduled )
            task = _task;
            
            // Schedule the task?
            mode = running;
        }

        void StateItem::setRunning( ReduceQueueTask* _task , ReduceQueueTask* _task2 )
        {
            
            // Set this as the task that is currently running ( or at least scheduled )
            task = _task;
            task2 = _task2;
            
            // Schedule the task?
            mode = running_dividing;
        }
        
        void StateItem::notifyFinishOperation( ReduceQueueTask* _task )
        {
            
            if( mode == ready )
            {
                LM_W(("Major error receiving a finish operation notification in a stream-state that is not running"));
                return;
            }
            
            if( mode == running )
            {
                if( task != _task )
                {
                    LM_W(("Internal error"));
                    return;
                }
                
                task = NULL;
                
                // Free the current state
                state->clearBlockList();
                state->copyFrom(future_state);
                future_state->clearBlockList();
                
                // Go back to ready
                mode = ready;
                
            }
            
            if( mode == running_dividing )
            {
                
                if( _task == task )
                    task = NULL;
                if( _task == task2 )
                    task2 = NULL;
                
                if( !task && !task2 )
                {
                    
                    int hg_mid = ( hg_begin + hg_end ) / 2;
                    StateItem *item1 = new StateItem( myState , hg_begin , hg_mid );
                    StateItem *item2 = new StateItem( myState , hg_mid , hg_end );
                    
                    item1->state->copyFrom(future_state);
                    item2->state->copyFrom(future_state2);
                    future_state->clearBlockList();
                    
                    // Push pending blocks to be processed
                    item1->push(input);
                    item2->push(input);

                    
                    // Divide this item in two
                    myState->divide( this , item1 , item2 );

                    
                    return;
                }
            }
            
            
        }
        
        void StateItem::addStateBuffer( ReduceQueueTask* _task , engine::Buffer* buffer)
        {
            
            switch (mode) {
                case running:
                    
                    if ( _task == task )
                        future_state->createBlock(buffer, false);
                    else
                    {
                        LM_W(("Adding a buffer to a state that is not related with this operation ( mode = running)"));
                        LM_W(("Current tasks [%p %p] reported task %p", task , task2 , _task ));
                    }
                    
                    break;
                    
                case running_dividing:

                    if ( _task == task )
                        future_state->createBlock(buffer, false);
                    else if( _task == task2 )
                        future_state2->createBlock(buffer, false);
                    else
                    {
                        LM_W(("Adding a buffer to a state that is not related with this operation ( mode = running_divide)"));
                        LM_W(("Current tasks [%p %p] reported task %p", task , task2 , _task ));
                        
                    }
                    
                    break;
                    
                case ready:
                    LM_W(("Received a state buffer while er are in ready mode. This is a serious error..."));
                    break;
            }
            
                
        }
        
        bool StateItem::isWorking()
        {
            return ( (mode == running) || ( mode == running_dividing ) );
        }

        
        
    }
}