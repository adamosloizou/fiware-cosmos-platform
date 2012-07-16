
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_nodes_info
#define _H_SAMSON_sna_nodes_info


#include <samson/module/samson.h>
#include "AUIntStatistics.h"



namespace samson{
namespace sna{


	class nodes_info : public samson::ParserOut
	{

		Node node ;
		AUIntStatistics numConnectionsStatistics ;
		AUIntStatistics numStrongConnectionsStatistics ;
		AUIntStatistics telefonicaNumbers ;

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  size_t total = 0;

			  for (size_t i  =0 ;  i< inputs[0].num_kvs  ; i++)
			  {

			    node.parse( inputs[0].kvs[i]->key);


			    numConnectionsStatistics.add(node.links_length );
			    numStrongConnectionsStatistics.add( node.numberOfLinksWithWeightEqualOrHigher(1.0) );


			    if(!( node.flags.value & Node::NODE_FLAG_EXTERN ))
			      telefonicaNumbers.add(1);

			  }
			  return;
		}

#ifdef RUNFINISH

		void runFinish(TXTWriter *writer)
		{
			  //Print results...
			  int total = 0;

			  total += fprintf(file, "Distribution of number of connections:\n");
			  total += fprintf(file, "--------------------------------------\n");
			  total += numConnectionsStatistics.printDistribution(file);
			  total += fprintf(file, "\n--------------------------------------\n");

			  total += fprintf(file, "Distribution of number of STRONG connections:\n");
			  total += fprintf(file, "---------------------------------------------\n");
			  total += numStrongConnectionsStatistics.printDistribution(file);
			  total += fprintf(file, "\n--------------------------------------\n");


			  total += fprintf(file, "\nGeneral information:\n");
			  total += fprintf(file, "--------------------------------------\n");

			  total += fprintf(file, "Total nodes: %lu\n", numConnectionsStatistics.count);
			  total += fprintf(file, "Num connections: mean %f std-dev %f\n" , numConnectionsStatistics.mean , sqrt(numConnectionsStatistics.var ) );
			  total += fprintf(file, "Number of Telefonica: %02f%% (%lu)\n" , 100.0*(double) telefonicaNumbers.count / (double) numConnectionsStatistics.count  , telefonicaNumbers.count );

			  return total;
			  output.str(""); //Clear content
			output << "\n\nDistribution of sizes of the cliques:\n";
			output << "-----------------------------------------\n";
			sizeClique.printDistribution(output);
			output << "-----------------------------------------\n";

			output << "General information:\n";
			output << "-------------------------\n";
			output << "Size of the clique: mean " << sizeClique.mean << " std-dev:" << sqrt( sizeClique.var ) << "\n";
			double average_connections = (double) ( total.count - sizeClique.count) / (double) sizeClique.count;
			output << "Average number of connections: " << average_connections << std::endl;

			writer->emit(output.str());
		}
#endif /* de RUNFINISH */




	};


} // end of namespace samson
} // end of namespace sna

#endif
