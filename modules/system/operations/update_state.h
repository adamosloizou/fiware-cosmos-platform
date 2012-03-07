
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_system_update_state
#define _H_SAMSON_system_update_state


#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace system{


	class update_state : public samson::Reduce
	{
        samson::system::Value key;
        samson::system::Value value;

        samson::system::Value _value;
        
        std::string command;
        
	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.Value system.Value  
//  input: system.Value system.Value  
//  output: system.Value system.Value
//  
// helpLine: Update state operations like last_10 , last_1 , ...
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
            command =  environment->get( "command" ,  "last" ); // Default command last
            
            /*
             
             last --> last_value

             rate --> #/second in the last 5 mins
             
             */
            
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
            
            if( inputs[1].num_kvs > 0 )
            {
                key.parse( inputs[1].kvs[0]->key);
                value.parse( inputs[1].kvs[0]->value);
            }
            else if ( inputs[0].num_kvs > 0 )
            {
                key.parse( inputs[0].kvs[0]->key);
                value.set_as_void();
            }
            else
                return;

            if( command == "last" )
            {
            
                // Get the last value as state
                if( inputs[0].num_kvs > 0 )
                    value.parse( inputs[0].kvs[ inputs[0].num_kvs-1 ]->value );
                
                // Emit the new state
                writer->emit( 0 , &key , &value );
            }
            
            
            if( command == "sum" )
            {
                double total = value.getDouble();
                for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    value.parse( inputs[0].kvs[i]->value);
                    total += value.getDouble();
                }
                
                value = total;
                writer->emit( 0 , &key , &value );
            }

                
            
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace system
} // end of namespace samson

#endif