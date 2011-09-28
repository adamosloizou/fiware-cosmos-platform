
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_shop_export_to_txt
#define _H_SAMSON_shop_export_to_txt


#include <samson/module/samson.h>


namespace samson{
namespace shop{


	class export_to_txt : public samson::ParserOut
	{

                std::ostringstream output;
                samson::system::UInt key;
                samson::shop::VectorProducts value;


	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
                        std::string separator = environment->get( "shop.separator" , ":" );

                        for (size_t i = 0; (i < inputs[0].num_kvs); i++)
                        {
                                key.parse( inputs[0].kvs[i]->key );
                                value.parse( inputs[0].kvs[i]->value );

                                output.str(""); // Clear content

                                output << key.str() << separator;
                                for (int j = 0; (j < value.products_length);j++)
                                {
                                        output << value.products[j].str() << ",";
                                }
                                output << std::endl;

                                writer->emit(output.str());
                        }
		}


	};


} // end of namespace samson
} // end of namespace shop

#endif
