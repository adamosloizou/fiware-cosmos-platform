
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_export_UInt_mobcdrs
#define _H_SAMSON_cdr_export_UInt_mobcdrs


#include <samson/module/samson.h>


namespace samson{
namespace cdr{


	class export_UInt_mobcdrs : public samson::ParserOut
	{
		samson::system::UInt key;
		samson::cdr::mobCdr value;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.mobCdr  

helpLine: Export mobility cdrs in txt format
#endif // de INFO_COMMENT

		void init(TXTWriter *writer )
		{
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			  {
				  key.parse( inputs[0].kvs[i]->key );
				  value.parse( inputs[0].kvs[i]->value );

				  if (i%1000 == 0)
				  {
					  OLM_T(LMT_User06, ("ParsingOut cdr:%lu, key:%lu, value:'%s' with timeUnix:%lu\n", i, key.value, value.str().c_str(), value.timeUnix.value));
				  }

				  snprintf(output, MAX_STR_LEN, "%lu %s\n", key.value, value.str().c_str());

				  writer->emit(output);
			  }

		}

		void finish(TXTWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace cdr

#endif
