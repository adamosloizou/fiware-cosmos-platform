
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_parse_client_clusters
#define _H_SAMSON_mob2_parse_client_clusters


#include <samson/module/samson.h>
#include <samson/modules/mob2/Cluster.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


	class parse_client_clusters : public samson::Parser
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

output: system.UInt mob2.Cluster

extendedHelp: 		Parse a txt file to extract client clusters

#endif // de INFO_COMMENT

		void init( samson::KVWriter *writer )
		{
		}

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif