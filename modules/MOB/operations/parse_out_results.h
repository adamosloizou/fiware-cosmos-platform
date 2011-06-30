
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_MOB_parse_out_results
#define _H_SAMSON_MOB_parse_out_results


#include <samson/module/samson.h>

#define CONF_SEP                    '|'


namespace samson{
namespace MOB{


	class parse_out_results : public samson::ParserOut
	{
#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif

#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt64 MOB.Results  

helpLine: Write results file in the appropriate readable format.
extendedHelp: 		Write results file in the appropriate readable format.

#endif // de INFO_COMMENT

		void init(TXTWriter *writer )
		{
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			samson::system::UInt64 phone;
			Results results;

			  for (int i  =0 ;  i < inputs[0].num_kvs ; ++i)
			  {
			      phone.parse( inputs[0].kvs[i]->key );
			      results.parse( inputs[0].kvs[i]->value );

			      int _total = 0;

			      if( (results.mobilityDegree.value >= 0) ||
				      (results.homeCellId.value > 0) || (results.homeBtsId.value > 0) || (results.homeLacId.value > 0) || (results.homeStateId.value > 0) ||
				      (results.workCellId.value > 0) || (results.workBtsId.value > 0) || (results.workLacId.value > 0) || (results.workStateId.value > 0))
			      {
			    		 _total += snprintf( output, MAX_STR_LEN, "%lu%c", phone.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.mobilityDegree.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeCellId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeCellFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeBtsId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeBtsFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeLacId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeLacFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeStateId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.homeStateFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workCellId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workCellFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workBtsId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workBtsFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workLacId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workLacFreq.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workStateId.value, CONF_SEP );
			    		 _total += snprintf( output+_total, MAX_STR_LEN, "%lu%c", results.workStateFreq.value, '\n' );

			    		 writer->emit(output);
			      }



			  }
			  return;
		}

		void finish(TXTWriter *writer )
		{
			   int _total = 0;

			   _total += snprintf( output, MAX_STR_LEN, "phone%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "mobility_degree%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_cell%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_cell_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_bts%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_bts_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_lac%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_lac_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_state%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "home_state_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_cell%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_cell_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_bts%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_bts_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_lac%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_lac_freq%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_state%c", CONF_SEP );
			   _total += snprintf( output+_total, MAX_STR_LEN, "work_state_freq%c", '\n' );

			   writer->emit(output);
		}



	};


} // end of namespace samson
} // end of namespace MOB

#endif
