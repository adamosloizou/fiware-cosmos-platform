/*
 *  CliquesSolver.cpp
 *  cdr_filter
 *
 *  Created by ANDREU URRUELA PLANAS on 10/21/09.
 *  Copyright 2009 TID. All rights reserved.
 *
 */

#include "CliquesSolver.h"

//#define DEBUG_CLIQUES_SOLVER

#pragma mark CliquesSolverLevelInfo

void CliquesSolverLevelInfo::removePerfectoNodes( )
{
	perfectNodes.clear();
	
	//Revome some nodes from nodes and put them into perfectNodes

	CliqueSolution normalNodes;
	
	for (int i = 0; i < nodes.num_nodes ; i++)
	{
		int node_i = nodes.nodes[i];
		bool perfect = true;

		for (int j = 0 ; j < nodes.num_nodes ; j++)
		{
			if ( j!= i )
			{
				int node_j = nodes.nodes[j];

				if( !solver->connections[node_i][node_j] )
				{
					perfect = false;
					break;
				}
			}
		}
		
		
		if ( perfect )
			perfectNodes.add( nodes.nodes[i] );
		else
			normalNodes.add( nodes.nodes[i] );
		
	}
	
	//Keep only normal nodes
	nodes.setFrom( &normalNodes );
	
}


#pragma mark CliquesSolver	


void CliquesSolver::solveCliques( )
{
	//printConnectionsMatrix();
	
	num_solutions = 0;
	
	if ( num_nodes == 0 )
		return;
/*	
	fprintf(stderr , "Solving cliques (%d nodes)\n", num_nodes);
	
	for (int i = 0 ; i < num_nodes ; i++)
	{
		for (int j = 0 ; j < num_nodes ; j++)
			fprintf(stderr, "%d ", connections[i][j]);
		fprintf(stderr, "\n");
	}
 */
		
	assert( num_nodes <= MAX_NODES );
	
	int level = 0;
	infoAtLevel[0].nodes.setDefaultSolution(num_nodes);	//Default solution for level "0"
	
	while( level >= 0)
	{

		//Element selected in the previous iteration is the minimum to consider... (to avoid duplicated results)
		int min_element_to_consider = -1;
		if (level>0)
			min_element_to_consider = infoAtLevel[level-1].nodes.getPreviousSelectedNode();
		
		
		
		if( infoAtLevel[level].nodes.pos == -1 )	//this menas that we are new in this level...
		{
			
			//Remove perfecto nodes from the list of nodes
			infoAtLevel[level].removePerfectoNodes();
			
			
			
#define DEBUG_CLIQUES_SOLVER 1
#ifdef DEBUG_CLIQUES_SOLVER			
			//Print status for debugging
			fprintf(stderr,"Considering (level %d)\n",level);
			fprintf(stderr,"\tPerfect nodes: %s\n", infoAtLevel[level].perfectNodes.toString().c_str());
			fprintf(stderr,"\tNodes: %s\n", infoAtLevel[level].nodes.toString().c_str());
			fprintf(stderr,"\tNodes considered thourght the tree:");
			for (int i  = 0 ;  i < level ; i++)
				fprintf(stderr, "%d ", infoAtLevel[i].nodes.getPreviousSelectedNode() );
			fprintf(stderr,"\n");
#endif			
			
			
			if( ( infoAtLevel[level].perfectNodes.num_nodes > 0) && (infoAtLevel[level].perfectNodes.nodes[0] < min_element_to_consider) )
			{
				//Not consider this possibility since the perfect nodes (allways present) are not in order with previously selected element
				// This means that all solutions found in this branch are duplicated in other place of the tree.
				level--;
				continue;
			}
			
			if ( infoAtLevel[level].nodes.num_nodes == 0 )	//All the elements are perfect-> new clique!
			{
				
				//Alloc space if necessary...
				if( num_solutions == max_solutions)
				{
					//fprintf(stderr, "Allocating space for %d solutions (%lu)\n",2*max_solutions, sizeof(CliqueSolution)*2*max_solutions );
					
					CliqueSolution* tmp = new CliqueSolution[2*max_solutions];
					memcpy(tmp, solutions, max_solutions * sizeof(CliqueSolution) );
					delete[] solutions;

					solutions = tmp;
					max_solutions = 2*max_solutions;	//new space
				}
				
				assert( num_solutions < max_solutions );
				solutions[num_solutions].clear();
				
				//Add node from previous levels...
				for (int i = 0 ; i < level ; i++)
				{
					solutions[num_solutions].addAll( &infoAtLevel[i].perfectNodes );
					solutions[num_solutions].add( infoAtLevel[i].nodes.getPreviousSelectedNode() );
				}

				//Add the perfect solutions at this level
				solutions[num_solutions].addAll( &infoAtLevel[level].perfectNodes );
				solutions[num_solutions].sort();

				if ( solutions[num_solutions].num_nodes >= min_nodes_in_solution)
				{
					/*
					bool previouslyFound =  false;
					for (int s = 0 ;  s < num_solutions ; s++ )
						if ( solutions[s].isEqual( &solutions[num_solutions] ) )
						{
							previouslyFound = true;
							break;						
						}
					
					if (!previouslyFound)
					 */
						num_solutions++;
					
				}
				//Continue with the next elements on the previous level
				level--;
				continue;	
			}
			else
				infoAtLevel[level].nodes.pos++;	// Go to the next positions (0 in this case)
			
		}


		// It is not a clique, so let's consider subgroups with each element
		
		if( infoAtLevel[level].nodes.pos >= infoAtLevel[level].nodes.num_nodes )	
		{
			level--; 			//No more elements to consider at this level
		}
		else
		{
			
			//Consider options with element at infoAtLevel[level].nodes.pos
			int *m = infoAtLevel[level].nodes.nodes;	//mapping vector of available nodes
			int n = infoAtLevel[level].nodes.pos;
			int node_to_consider = m[n];
			
			if( node_to_consider > min_element_to_consider )
			{
				
				//Take its connections
				infoAtLevel[level+1].nodes.clear();
				for (int nn = 0; nn < infoAtLevel[level].nodes.num_nodes ;nn++)
					if ( n!=nn )
						if (connections[ m[n] ][ m[nn] ] )
						{
							infoAtLevel[level+1].nodes.add( infoAtLevel[level].nodes.nodes[nn] );
							//infoAtLevel[level+1].nodes.nodes[num++] = infoAtLevel[level].nodes.nodes[nn];
						}
				//infoAtLevel[level+1].nodes.num_nodes = num;
				
				//Prepare for the next element
				infoAtLevel[level].nodes.pos++;			//Skip to the next one in this level
				infoAtLevel[level+1].nodes.pos = -1;	//Prepare the next level to check as global solution
				
				level++;
			}
			else
				infoAtLevel[level].nodes.pos++;	//Skip to the next one
				
			
		}			
	}
	
/*	*/
	fprintf(stderr , "Solving cliques: Solutions found:%d\n", num_solutions);
	
	for (int i = 0 ; i < num_solutions	; i++)
	{
		fprintf(stderr, "\tSolution %d: %s", i , solutions[i].toString().c_str());
	}
 /* */
	
	
}


	



