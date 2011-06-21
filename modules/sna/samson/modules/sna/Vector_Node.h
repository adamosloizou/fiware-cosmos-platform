
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_sna_Vector_Node
#define _H_SAMSON_sna_Vector_Node

#include <samson/modules/graph/Node.h>

#include <samson/modules/sna/Vector_Node_base.h>
#include <algorithm>


namespace samson{
namespace sna{


	class Vector_Node : public Vector_Node_base
	{

	public:
	        graph::Node *nodeWithId( uint id)
		{
			for (int n = 0 ;  n < items_length ; n++)
				if( items[n].id == id)
					return &items[n];

			return NULL;
		}

		double internalConnectionStrength(double penalization_missing_link , double penalization_missing_link_externals)
		{
			int num_combinations = 0;	// Total number of combinations
			double total=0;				// Total score

			for (int i = 0 ; i < items_length ; i++)
				for (int j = i+1 ; j < items_length ; j++)
				{
				        graph::Node *node1 = &items[i];
					graph::Node *node2 = &items[j];
					double weight  = node1->weightOfLinkWithId( node2->id.value );


					if( weight >= 1.0)
						total += 1.0;
					else if( weight > 0)
						total += weight;
					else
					{

						if ( node1->isExtern() && node2->isExtern())
							total -= penalization_missing_link_externals;
						else
							total -= penalization_missing_link;

					}

					num_combinations ++;
				}

			double result = total / (double) num_combinations ;
			return result;


		}


		double computeStrengthWithNode( uint nodeId )
		{
			double total = 0;

			for (int i = 0 ; i < items_length ; i++)
			{
				double w = items[i].weightOfLinkWithId( nodeId );
				total += std::min( 1.0 , w );
			}

			return total / (double)items_length ;
		}


		void computeAssociatedNodes( Clique *associated_nodes , double threshold )
		{
			// Note that weight should be strictly superior to the threshold!!!

			associated_nodes->nodesSetLength(0);

			// Get a list of candidates
			std::vector<uint> candidates;

			for (int i = 0 ; i < items_length ; i++)
				for (int j = 0 ; j  < items[i].links_length ; j++)
					if ( !nodeWithId( items[i].links[j].id.value ) )	//Not part of the
						candidates.push_back( items[i].links[j].id.value );

			// Sort and remove duplicated
			std::sort(candidates.begin(), candidates.end());
			candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());


			for( uint i = 0 ; i < candidates.size() ; i++)
			{
				uint nodeId = candidates[i];
				double weight = computeStrengthWithNode( nodeId );
				if (weight > threshold)
					associated_nodes->nodesAdd()->value = nodeId;

			}


		}
	};


} // end of namespace samson
} // end of namespace sna

#endif
