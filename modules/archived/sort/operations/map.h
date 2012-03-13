
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sort_map
#define _H_SAMSON_sort_map


#include <samson/module/samson.h>
#include <samson/modules/system/UInt64.h>
#include <samson/modules/sort/Code.h>

namespace samson{
namespace sort{


	class map : public samson::Parser
	{

	public:


		void run( char *data , size_t length , samson::KVWriter *writer )
		{
		  //Datas to emit
		  samson::sort::Code key;
		  samson::system::UInt64 value;

		  //OLM_M(("Parsing %lu bytes", length  ));  

		  size_t pos_begin = 0;
		  for ( size_t i = 0 ; i < length ; i++ )
		  {
		    if( data[i] == '\n' )
		    {
		      // Emit cumulated key
		      data[i] = '\0';
		      value.value = strtoull( &data[pos_begin] , NULL , 10 );
		      pos_begin = i+1; // Begin of the next number
		      
		      // Emit key value
		      key.set( value.value );
		      writer->emit( 0, &key, &value );

		    }

		  }

		}
	};


} // end of namespace samson
} // end of namespace sort

#endif