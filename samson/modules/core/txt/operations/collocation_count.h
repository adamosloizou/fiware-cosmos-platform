
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_txt_collocation_count
#define _H_SAMSON_txt_collocation_count


#include <samson/module/samson.h>
#include <iostream>
#include <fstream>
#include <string>



namespace samson{
namespace txt{


class collocation_count : public samson::Reduce
{

public:

	samson::txt::Collocation key;
	samson::txt::Collocation keyPrev;
	samson::system::UInt count;
	samson::system::UInt total;

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{


		if( inputs[0].num_kvs == 0 )
		{
			return;
		}

		total.value = 0;
		//LM_M(("Reducing %d pairs", inputs[0].num_kvs));
		key.parse(inputs[0].kvs[0]->key);



		for (size_t i = 0; i < inputs[0].num_kvs ; i++)
		{
			key.parse(inputs[0].kvs[i]->key);


			count.parse( inputs[0].kvs[i]->value );

			total.value += count.value;
		}

		if (total.value > 0)
		{
			writer->emit( 0, &key , &total );
		}
	}
};


} // end of namespace samson
} // end of namespace txt

#endif
