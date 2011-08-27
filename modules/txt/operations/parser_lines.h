
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_parser_lines
#define _H_SAMSON_txt_parser_lines


#include <samson/module/samson.h>
#include <samson/module/samson.h>
#include <samson/modules/system/SimpleParser.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <iostream>

namespace samson{
namespace txt{


	class parser_lines : public samson::Parser
	{

		samson::system::String key;
		samson::system::UInt value;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

output: system.String system.UInt

helpLine: Parse a txt file emiting every line as an independent string ( return character not included )
#endif // de INFO_COMMENT

		void init( samson::KVWriter *writer )
		{
			value.value = 1; // the counter used as value at the output is always 1
		}

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
		  
			int pos_begin=0;
			int pos_end;

			while( pos_begin < length )
			{

			  if( (pos_begin%100000) == 0 )
			  {
			        // Report progress
    			        operationController->reportProgress( (double) pos_begin / (double) length );
			        
			  }


			  pos_end = pos_begin; // Allow empty words if \n is found

			  while( (pos_end < (length-1) ) && ( data[pos_end] != '\n' ) && ( data[pos_end] != '\0') )
				pos_end++;

			  // Mark the end of the string 
			  data[pos_end]='\0';
			  key.value = &data[pos_begin];

			  // Emit the new key value
			  writer->emit( 0 , &key, &value);

			  // Put the pos_begin at the next character
			  pos_begin = pos_end+1;
			}

		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace txt
} // end of namespace samson

#endif