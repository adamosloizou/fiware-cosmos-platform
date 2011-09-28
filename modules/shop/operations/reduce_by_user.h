
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_shop_reduce_by_user
#define _H_SAMSON_shop_reduce_by_user


#include <samson/module/samson.h>


namespace samson{
namespace shop{


	class reduce_by_user : public samson::Reduce
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
                        // Variables used to read content from the input
                        samson::system::UInt input_key;
                        samson::system::UInt input_value;

                        // Variables used to store content for the output
                        samson::shop::VectorProducts output_values;

                        for (size_t i = 0; (i < inputs[0].num_kvs); i++)
                        {
                                input_key.parse(inputs[0].kvs[i]->key);
                                input_value.parse(inputs[0].kvs[i]->value);

                                if ((output_values.products_length == 0) || (output_values.products[output_values.products_length-1] != input_value.value))
                                {
                                        output_values.productsAdd()->copyFrom(&input_value);
                                }
                        }

                        writer->emit( 0 , &input_key, &output_values);

		}


	};


} // end of namespace samson
} // end of namespace shop

#endif
