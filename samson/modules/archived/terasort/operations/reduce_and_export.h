
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_terasort_reduce_and_export
#define _H_SAMSON_terasort_reduce_and_export


#include <samson/module/samson.h>


namespace samson{
namespace terasort{


	class reduce_and_export : public samson::ParserOutReduce
	{
		Record value;
#define MAX_STR_LEN 2048
		  char line[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
			  {
			    value.parse( inputs[0].kvs[i]->value );
#define CAD_SEP "  "
			    snprintf( line , MAX_STR_LEN, "%s%s%s%s%s\r\n", value.keyf.value.c_str(), CAD_SEP, value.num_rec.value.c_str(), CAD_SEP, value.valf.value.c_str());
			    writer->emit( line );
			  }

		}


	};


} // end of namespace samson
} // end of namespace terasort

#endif
