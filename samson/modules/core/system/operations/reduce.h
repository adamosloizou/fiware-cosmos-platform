
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_system_reduce
#define _H_SAMSON_system_reduce


#include <samson/module/samson.h>

// samson_system library
#include "samson_system/Value.h"
#include "samson_system/ValueReduce.h"

namespace samson{
namespace system{

    
    // -----------------------------------------------------------------------------------------------
    // reduce operation ( one input, out output )
    //
    // -----------------------------------------------------------------------------------------------

	class reduce : public samson::Reduce
	{
        
        std::string command;
        ValueReduce * operation;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.Value system.Value  
//  output: system.Value system.Value
//  
// helpLine: Reduce all the values for every particular key. Possible commands  sum_double (default) , average_double , max_double  
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
            // Setup the process chain...
            command =  environment->get( "command" ,  "" );
            
			if( command == "" )
			{
                tracer->setUserError( "Environment variable command not specified. Please specify with env:command sum,average,most_popular..." );
                return;
			}
            
            ValueReduceManager manager("reduce");
            operation = manager.getInstance( command );
            
            if( !operation )
            {
                tracer->setUserError( au::str("Non valid command '%s' Available commands: %s" , command.c_str() , manager.getListOfCommands().c_str() ) );
                return;
            }
            
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
            // Run selected operation
            operation->run( inputs , writer );
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace system
} // end of namespace samson

#endif
