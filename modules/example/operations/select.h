
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_example_select
#define _H_SAMSON_example_select


#include <samson/module/samson.h>

#include <samson/modules/system/UInt.h> 

namespace samson{
namespace example{


	class select : public samson::ParserOut
	{

		std::ostringstream output;
		samson::system::UInt key;
		samson::system::UInt value;

		std::string separator;
		
	public:

		void init(TXTWriter *writer)
		{
			separator = environment->get( "example.separator" , " " );
		}
		

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				key.parse( inputs[0].kvs[i]->key );
				value.parse( inputs[0].kvs[i]->value );
				
				output.str("");	// Clear content
				
				output << key.str() << separator << value.str() << std::endl;
				
				writer->emit(output.str());
			}
			
		}


	};


} // end of namespace samson
} // end of namespace example

#endif
