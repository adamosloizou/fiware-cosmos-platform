
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_select_clique_links_to_be_removed
#define _H_SAMSON_sna_select_clique_links_to_be_removed


#include <samson/module/samson.h>

#include "sna_environment_parameters.h"


namespace samson{
namespace sna{


	class select_clique_links_to_be_removed : public samson::Reduce
	{
		Clique clique ;
		Clique_Node node_clique ;
		int max_community_links ;

	public:

		void init(samson::KVWriter *writer)
		{
			max_community_links = environment->getInt(SNA_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK,
																				SNA_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK_DEFAULT);
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			if (inputs[0].num_kvs <= max_community_links)
			{
				OLM_M(("inputs[0].num_kvs(%d) <= max_community_links(%d); nothing to do!", inputs[0].num_kvs, max_community_links));
				return;	//Nothing to do!
			}


			//Get the clique of the "key"
			clique.parse( inputs[0].kvs[0]->key );

			// Acumulate all the connected cliques in the node_clique structure
			node_clique.linksSetLength(0);
			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
				node_clique.linksAdd()->parse( inputs[0].kvs[i]->value );

			//Sort by weight
			node_clique.sortByWeight();

			Clique tmp_clique;
			Clique_Link tmp_clique_link;

			int rank = 1;
			for (int i = 1 ; i < node_clique.links_length ; i++)
			{
				if( node_clique.links[i].weight.value < node_clique.links[i-1].weight.value )
					rank=(i+1);

				if( rank > max_community_links )
				{
					tmp_clique.copyFrom(&clique);
					tmp_clique_link.clique.copyFrom( &node_clique.links[i].clique );
					tmp_clique_link.weight = node_clique.links[i].weight;

					writer->emit(0, &tmp_clique , &tmp_clique_link );

					tmp_clique.copyFrom( &node_clique.links[i].clique);
					tmp_clique_link.clique.copyFrom( &clique );
					tmp_clique_link.weight = node_clique.links[i].weight;

					writer->emit(0, &tmp_clique , &tmp_clique_link );
				}

			}

		}


	};


} // end of namespace samson
} // end of namespace sna

#endif
