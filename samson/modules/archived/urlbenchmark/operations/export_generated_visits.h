
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_urlbenchmark_export_generated_visits
#define _H_SAMSON_urlbenchmark_export_generated_visits


#include <samson/module/samson.h>

/****************************************************************
parserOut export_generated_visits
{
	in system.UInt system.String  # UserId - URL visited
	out txt txt

	helpLine "Dumps to text the generated visits"
}
*****************************************************************/

namespace samson{
namespace urlbenchmark{


	class export_generated_visits : public samson::ParserOut
	{
		samson::system::UInt user;
		samson::system::String url;

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif


#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				user.parse(inputs[0].kvs[i]->key);
				url.parse(inputs[0].kvs[i]->value);

				snprintf(output, MAX_STR_LEN, "%lu %s\n", user.value, url.value.c_str());

				writer->emit(output);
			}
		}


	};


} // end of namespace samson
} // end of namespace urlbenchmark

#endif
