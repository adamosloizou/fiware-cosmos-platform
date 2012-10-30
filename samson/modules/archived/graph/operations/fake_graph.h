/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_graph_fake_graph
#define _H_SAMSON_graph_fake_graph

#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include "samson/modules/graph/Node.h"

#define LENGTH(v) sizeof(v)/sizeof(v[0])

namespace samson{
namespace graph{

	class fake_graph : public samson::Generator
	{

	public:

		void run( samson::KVWriter *writer )
		{

			int num_example = environment->getInt( "graph.fake_graph" ,  1 );
			
			switch (num_example) {
				case 1:
				{
					/*
					 Nodes of telefonica 1,2,3,4
					 All strongly connected except 1-4 with no-connection
					 */
					
					int nodes_s[]	= {
						1,	2,3,		0,
						2,	1,3,4,		0,
						3,	1,2,4,		0,
						4,	2,3,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
				case 2:
				{
					
					/*
					 Nodes of telefonica 1,2,3,4
					 All strongly connected except 1-4 with soft-connection
					 */
					
					int nodes_s[]	= {
						1,	 2,3,-4,		0,
						2,	 1,3, 4,		0,
						3,	 1,2, 4,		0,
						4,	-1,2, 3,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
				case 3:
				{
					
					/*
					 Nodes of telefonica 2,3
					 Nodes extern		 1,4
					 All strongly connected except 1-4 with soft-connection
					 */
					
					int nodes_s[]	= {
						-1,	2,3,		0,
						2,	1,3,4,		0,
						3,	1,2,4,		0,
						-4,	2,3,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
					
				case 4:
				{
					int nodes_s[]	= {
						1,	 2, 3,-4,		0,
						2,	 1, 3,   		0,
						3,	 1, 2, 4, 5,	0,
						4,	-1, 3, 5,		0,
						5,	 3, 4,			0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
					
				case 5:
				{
					int nodes_s[]	= {
						1,	 2, 3,-4,-5,	0,
						2,	 1, 3,-4,  		0,
						3,	 1, 2, 4, 5,	0,
						4,	-1,-2, 3, 5,	0,
						5,  -1, 3, 4,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
					
					
				case 6:
				{
					int nodes_s[]	= {
						1,	 2,           	0,
						2,	 1, 3,-4,  		0,
						3,	 1, 2, 4, 5,	0,
						4,	-1,-2, 3, 5,	0,
						5,  -1, 3, 4,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
					
				case 7:
				{
					int nodes_s[]	= {
						1,	-2,-3,-4,	0,
						2,	-1,3,4,		0,
						3,	-1,2,4,		0,
						4,	-1,2,3,		0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					break;
				}
					
				case 8:
				{
					
					/*
					 Nodes of telefonica 1,2,3,4
					 Nodes 1,2,3 strongly connected to form a clique
					 Node 4 softly connected to all of them to be an associated
					 */
					int nodes_s[]	= {
						1,	2,3,-4,		0,
						2,	1,3,-4,		0,
						3,	1,2,-4,		0,
						4,	-1,-2,-3,	0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s), writer );
					
					break;
				}
					
				case 100:	//Special for "perfect nodes"
				{
					int nodes_s[]	= {
						1,	       3, 4, 5, 6, 7,	0,
						2,	       3, 4, 5, 6, 7,	0,
						3,	 1, 2,    4, 5, 6, 7,	0,
						4,	 1, 2, 3,    5, 6, 7,	0,
						5,	 1, 2, 3, 4,    6, 7,	0,
						6,	 1, 2, 3, 4, 5,    7,	0,
						7,	 1, 2, 3, 4, 5, 6,   	0,
					};
					generateNodes( nodes_s , LENGTH(nodes_s) , writer );
					break;
				}
					
					
				default:
					break;
			}			
		}
		
		
		void generateNodes(int *nodes_s , int length , samson::KVWriter *writer  )
		{
			int node = 0;
			bool extern_node = false;
			int links[1000];
			double weights[1000];
			int pos=-1;
			
			for (int i = 0 ; i < length ; i++)
			{
				//fprintf(stderr, "Element %d -> %d (pos %d)\n", i , nodes_s[i], pos );
				
				if (pos == -1)	//new node...
				{
					if ( nodes_s[i] > 0 )
					{
						node = nodes_s[i];
						extern_node = false;
					}
					else
					{
						node = -nodes_s[i];
						extern_node = true;
					}
					
					assert(node>0);
					pos++;
				} else if ( nodes_s[i] == 0)	//End of a node
				{
					//Emit node
					if (pos>0)
					{
						//fprintf(stderr, "Emiting %d len%d\n", node , pos);
						generateNode(node, extern_node,  links, weights, pos , writer );
					}
					pos=-1;
				} else
				{
					//Normal contact
					if ( nodes_s[i] < 0) 
					{
						//Soft link
						links[pos] = -nodes_s[i];
						weights[pos] = 0.5;
						pos++;
					}
					else
					{
						//Strong link
						links[pos] = nodes_s[i];
						weights[pos] = 1.0;
						pos++;
					}
				}
				
			}
		}
		
		void generateNode( int id , bool extern_node,  int* links, double* weight, int num , samson::KVWriter *writer )
		{
			samson::system::UInt node_id;
			samson::graph::Node node;
			
			node_id.value	= id; 
			node.id			= id;
			
			node.flags =0;
			
			if( extern_node )
				node.flags.value |= GRAPH_FLAG_EXTERN; 
			else
				node.flags.value &= ~GRAPH_FLAG_EXTERN; 
			
			node.linksSetLength(num);
			for (int i = 0 ; i < num ; i++)
			{
				node.links[i].id		= links[i];
				node.links[i].setWeight( weight[i] );
			}
			
			writer->emit(0,&node_id, &node);
			
		}
		
		


	};


} // end of namespace samson
} // end of namespace graph

#endif
