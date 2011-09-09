
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_vector_filt_clients
#define _H_SAMSON_mob2_vector_filt_clients


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


	class vector_filt_clients : public samson::Reduce
	{
        // Inputs/outputs
        samson::system::UInt node;
        samson::cdr::mobCdr cdr;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.mobCdr  
input: system.UInt system.samson::system::UInt  
output: system.UInt cdr.mobCdr

extendedHelp: 		Filter cdrs to clients filtered by total comms
		Input 0: Cdrs
		Input 1: List of clients filtered by total comms

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

////////////////////////////
// VECTOR COMMS IN A NODE //
////////////////////////////

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
	// Should we check only if inputs[1].num_kvs > 0?
        for(uint64_t i=0; i<inputs[1].num_kvs; i++)
        {
		//As this is a reduce operation, we have to parse the key only once. 
		node.parse(inputs[0].kvs[0]->key);
                for(uint64_t j=0; j<inputs[0].num_kvs; j++)
                {
                        cdr.parse(inputs[0].kvs[j]->value);
                        writer->emit(0, &node,&cdr);
                }
        }

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif