
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_spread_communities_to_links
#define _H_SAMSON_sna_spread_communities_to_links


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class spread_communities_to_links : public samson::Map
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

			Clique clique;
			samson::system::UInt pivotNode;
			Clique cliqueOut;

			for (size_t i=0; i < inputs[0].num_kvs; i++)
			{
				//Get the clique N
				clique.nodesSetLength(0);
				clique.parse(inputs[0].kvs[i]->key);

				//For each element of the clique, emit the elelement as key and the rest of nodes as the value
				for (int j=0; j < clique.nodes_length; j++)
				{
					pivotNode = clique.nodes[j];
					cliqueOut.nodesSetLength(0);
					//isn't there a nodesRemove function in a clique? Better with copyFrom and a kind of 'nodeRemove' method...
					for (int k=0; k < clique.nodes_length; k++)
					{
						if (clique.nodes[k] != pivotNode.value)
						{
							cliqueOut.nodesAdd()->value = clique.nodes[k].value;
						}
					}
					//Emit the node and the rest of the community
					writer->emit(0, &pivotNode, &cliqueOut);
				}

			}
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif