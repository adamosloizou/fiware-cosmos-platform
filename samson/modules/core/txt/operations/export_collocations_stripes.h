
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_txt_export_collocations_stripes
#define _H_SAMSON_txt_export_collocations_stripes


#include <samson/module/samson.h>


namespace samson{
namespace txt{


	class export_collocations_stripes : public samson::ParserOut
	{
		samson::system::String key;
		samson::txt::Stripe value;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
                        for (size_t i = 0; (i < inputs[0].num_kvs); i++)
                        {
                                key.parse( inputs[0].kvs[i]->key );
                                value.parse( inputs[0].kvs[i]->value );
				std::string keystr = key.value;
				OLM_T(LMT_User06,("Start key:'%s' with %d collocs", keystr.c_str(), value.colList_length));
				for (int j = 0; (j < value.colList_length); j++)
				{
					if (value.colList[j].count.value > 0)
					{
						//OLM_M(("key: '%s' value[%d]:'%s', count:%d", keystr.c_str(), j, value.colList[j].word.value.c_str(), value.colList[j].count.value));
						snprintf(output, MAX_STR_LEN, "%s, %s: %lu\n", keystr.c_str(), value.colList[j].word.value.c_str(), value.colList[j].count.value);

						writer->emit(output);
					}
				}
                        }
		}
	};


} // end of namespace samson
} // end of namespace txt

#endif
