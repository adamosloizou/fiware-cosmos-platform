
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_graph_add_extern_mark_to_graph
#define _H_SAMSON_graph_add_extern_mark_to_graph


#include <samson/module/samson.h>

#include <samson/modules/system/UInt.h>
#include "samson/modules/graph/Node.h"

namespace samson{
namespace graph{

	class add_extern_mark_to_graph : public samson::Reduce
	{

		samson::system::UInt id;
		samson::graph::Node node;
		
	public:

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			
			if( inputs[0].num_kvs == 1 )
			{
				id.parse( inputs[0].kvs[0]->key );
				node.parse( inputs[0].kvs[0]->value );
				
				if( inputs[1].num_kvs == 0)	// If we do not have this element, it is considered extern
					node.flags.value |= GRAPH_FLAG_EXTERN;	//Activate the extern flag
				else
					node.flags.value &= ~GRAPH_FLAG_EXTERN;	// Deactivate the extern flag
				writer->emit(0 , &id, &node);
			}			
			
		}


	};


} // end of namespace samson
} // end of namespace graph

#endif
