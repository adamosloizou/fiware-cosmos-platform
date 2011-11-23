
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_select_top_reduce
#define _H_SAMSON_url_select_top_reduce


#include <samson/module/samson.h>
#include <samson/modules/url/ServerHits.h>
#include "ServerHitsAccumulator.h"

namespace samson{
namespace url{


	class select_top_reduce : public samson::ParserOutReduce
	{
	  ServerHitsAccumulator serverHitsAccumulator;

	  samson::url::ServerHits serverHits;
#define MAX_STR_LEN 2048
		  char line[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{

		  size_t num_top_urls = environment->getSizeT( "url.num_top_urls" ,  10 );

		  serverHitsAccumulator.init( num_top_urls);

		    for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		    {
		      serverHits.parse( inputs[0].kvs[i]->value );
		      serverHitsAccumulator.add( serverHits.server.value , serverHits.hits.value ); 
		    }


		    // Emit the output as txt
		    for (int i = 0 ; i < serverHitsAccumulator.size ; i++)
		    {
		      if( serverHitsAccumulator.hits[i] > 0 )
		      {
			snprintf(line , MAX_STR_LEN, "%lu %s\n" , serverHitsAccumulator.hits[i] , serverHitsAccumulator.servers[i].c_str() );
			writer->emit( line );
		      }
		    }
		  

		}


	};


} // end of namespace samson
} // end of namespace url

#endif