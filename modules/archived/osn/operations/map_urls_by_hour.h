
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_map_urls_by_hour
#define _H_SAMSON_osn_map_urls_by_hour


#include <samson/module/samson.h>
#include <string>


namespace samson{
namespace osn{


	class map_urls_by_hour : public samson::Map
	{
		samson::system::UInt64 msisdn;
		samson::osn::URLConnection  connect;
		samson::system::String key_out;
		samson::system::UInt value_out;
#define MAX_STR_LEN 1024
			char conv[MAX_STR_LEN];

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			if (inputs[0].num_kvs == 0)
			{
				return;
			}

			for (size_t i=0; (i < inputs[0].num_kvs); i++)
			{
				msisdn.parse(inputs[0].kvs[i]->key);
				connect.parse(inputs[0].kvs[i]->value);



			snprintf(conv, MAX_STR_LEN, "%d", int(connect.time.hour.value));

				key_out.value.assign(conv);
				value_out.value = 1;
				writer->emit(0, &key_out, &value_out);


			}
                        samson::system::UInt key_total;
                        samson::system::UInt val_total;
                        key_total.value = 1;
                        val_total.value = inputs[0].num_kvs;
                        writer->emit(1, &key_total, &val_total);
		}

	};


} // end of namespace samson
} // end of namespace osn

#endif