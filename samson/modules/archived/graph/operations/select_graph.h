
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_graph_select_nodes
#define _H_SAMSON_graph_select_nodes


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include "samson/modules/graph/Node.h"
#include <iostream>

namespace samson{
namespace graph{


	class select_graph : public samson::ParserOut
	{
		samson::system::UInt key;
		samson::graph::Node value;

		std::ostringstream output;
		
	public:

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				key.parse( inputs[0].kvs[i]->key );
				value.parse( inputs[0].kvs[i]->value );
				
				output.str(""); // Clear content                                                                                   
				
				output << key.str() << " " << value.str() << std::endl;
				
				writer->emit(output.str());
			}
		}


	};


} // end of namespace samson
} // end of namespace graph

#endif
