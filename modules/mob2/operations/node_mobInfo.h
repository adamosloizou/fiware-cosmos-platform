
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_node_mobInfo
#define _H_SAMSON_mob2_node_mobInfo


#include <samson/module/samson.h>
#include <samson/modules/mob2/Bts_Counter.h>
#include <samson/modules/mob2/NodeMx_Counter.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


	class node_mobInfo : public samson::Reduce
	{
        //Inputs
        samson::system::UInt nodeId;
        Bts_Counter btsCount;
        //Outputs
        NodeMx_Counter nodeCount;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt mob2.Bts_Counter  
output: system.UInt mob2.NodeMx_Counter

extendedHelp: 		Create the array of counters by node: num of comms by day and hour

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

////////
// Array of counters by node: num of comms by day and hour
////////

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
        nodeId.parse(inputs[0].kvs[0]->key);
        nodeCount.btsSetLength(inputs[0].num_kvs);
        for(uint64_t i=0; i<inputs[0].num_kvs; i++)
        {
                btsCount.parse(inputs[0].kvs[i]->value);
                nodeCount.bts[i] = btsCount;
        }
        writer->emit(0, &nodeId,&nodeCount);

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif