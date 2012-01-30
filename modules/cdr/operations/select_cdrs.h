
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_select_cdrs
#define _H_SAMSON_cdr_select_cdrs


#include <samson/module/samson.h>

#include <samson/modules/system/UInt.h>
#include "samson/modules/cdr/CDR.h"
#include <iostream>

namespace samson{
namespace cdr{


	class select_cdrs : public samson::ParserOut
	{
		samson::system::UInt key;
		samson::cdr::CDR value;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];
		
		
	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			
			  for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			  {
				  key.parse( inputs[0].kvs[i]->key );
				  value.parse( inputs[0].kvs[i]->value );

				  snprintf(output, MAX_STR_LEN, "%lu %s\n", key.value, value.str().c_str());

				  writer->emit(output);
			  }

		}


	};


} // end of namespace samson
} // end of namespace cdr

#endif