
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_metric_communities_out
#define _H_SAMSON_sna_metric_communities_out


#include <samson/module/samson.h>
#include <iostream>
#include "sna_environment_parameters.h"


namespace samson{
namespace sna{


	class metric_communities_out : public samson::ParserOut
	{
		std::string sep;
		std::ostringstream output;

	public:

		void init(samson::KVWriter *writer)
		{
		  sep = environment->get(SNA_PARAMETER_COMMUNITIES_SEP, SNA_PARAMETER_COMMUNITIES_SEP_DEFAULT);
		}


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			  Clique id;
			  Metric_Communities metric_data;


			  for (int i  =0 ;  i< inputs[0].num_kvs ; i++)
			  {

			    metric_data.parse( inputs[0].kvs[i]->value );

			    output << metric_data.id_comm.value << sep;
			    output << metric_data.count_nodes.value << sep;

			    output << metric_data.count_nodes_strong.value << sep;
			    output << metric_data.count_nodes_associated.value << sep;
			    output << metric_data.count_nodes_competitor.value << sep;
			    output << metric_data.count_nodes_telefonica.value << sep;

			    output << Link::intScaled_2_double(metric_data.percent_nodes_competitor.value) << sep;

			    output << metric_data.count_regular_existing_links.value << sep;
			    output << metric_data.count_regular_possible_links.value << sep;
			    output << metric_data.count_all_existing_links.value << sep;
			    output << metric_data.count_all_possible_links.value << sep;

			    output <<  Link::intScaled_2_double(metric_data.density_regular_members.value) << sep;
			    output <<  Link::intScaled_2_double(metric_data.density_all_members.value) << sep;

			    output <<  Link::intScaled_2_double(metric_data.cohesion_regular_members.value) << sep;
			    output <<  Link::intScaled_2_double(metric_data.cohesion_all_members.value) << sep;

			    output << metric_data.reach_one_step.value << sep;
			    output << metric_data.reach_one_step_competitors.value << sep;
			    output << metric_data.count_nodes.value << sep;
			    output << metric_data.reach_comm_on_step.value << sep;

			    output << Link::intScaled_2_double(metric_data.value_total.value) << sep;
			    output << Link::intScaled_2_double(metric_data.value_per_node.value) << sep;
			    output << Link::intScaled_2_double(metric_data.value_per_edge.value);

			    output << std::endl;

			  }

			  writer->emit(output.str());
			  return ;
		}

#ifdef RUNFINISH
		void runFinish(TXTWriter *writer ) {
		  output <<  "id_comm:" << sep;
		  output <<  "count_nodes:" << sep;
		  output <<  "count_nodes_strong:" << sep;
		  output <<  "count_nodes_associated:" << sep;
		  output <<  "count_nodes_competitor:" << sep;
		  output <<  "count_nodes_telefonica:" << sep;
		  output <<  "percent_nodes_competitor:" << sep;
		  output <<  "count_regular_existing_links:" << sep;
		  output <<  "count_regular_possible_links:" << sep;
		  output <<  "count_all_existing_links:" << sep;
		  output <<  "count_all_possible_links:" << sep;
		  output <<  "density_regular_members:" << sep;
		  output <<  "density_all_members:" << sep;
		  output <<  "cohesion_regular_members:" << sep;
		  output <<  "cohesion_all_members:" << sep;
		  output <<  "one_step_node_reach:" << sep;
		  output <<  "one_step_node_compet_reach:" << sep;
		  output <<  "comm_size:" << sep;
		  output <<  "one_step_com_reach:" << sep;

		  output <<  "value_total:" << sep;
		  output <<  "value_per_node:" << sep;
		  output <<  "value_per_edge:" << sep;

		  output <<  std::endl;

		  writer->emit(output.str());
		}
#endif /* de RUNFINISH */


	};


} // end of namespace samson
} // end of namespace sna

#endif
