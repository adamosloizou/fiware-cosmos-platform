
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob_reduce_compute_reference_location
#define _H_SAMSON_mob_reduce_compute_reference_location


#include <samson/module/samson.h>


namespace samson{
namespace mob{



class reduce_compute_reference_location : public samson::Reduce
{

	// Input[0k] & Output[0k]
	samson::system::UInt phone;
	// Input[0v]
	LocCounter locCounter;
	// Output[0v]
	LocCounter refLocCounter;
public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob.LocCounter
	output: system.UInt mob.LocCounter

	helpLine: Get the location with the highest number of calls or days with calls.
	extendedHelp: 		Get the location with the highest number of calls or days with calls.

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	/**
	 * Compute the location (i.e. cell, BTS, LAC or state)
	 * where the highest number of items (calls or
	 * days with calls) have taken place.
	 */
	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{

		refLocCounter.loc.value = 0;
		refLocCounter.count = 0;

		//for( size_t i=0; i<inputs[0].num_kvs; i++ )
		//{
		//	locCounter.parse( inputs[0].kvs[i]->value );
		//	if( locCounter.count >= refLocCounter.count )
		//	{
		//		refLocCounter.loc = locCounter.loc;
		//		refLocCounter.count = locCounter.count;
		//	}
		//}
		//locCounter.parse( inputs[0].kvs[0]->value);
		//OLM_T(LMT_User06, ("locCounter[0]: loc:%lu, count:%lu\n", locCounter.loc.value, locCounter.count.value));
		refLocCounter.parse( inputs[0].kvs[inputs[0].num_kvs-1]->value);
		//OLM_T(LMT_User06, ("locCounter[%lu]: loc:%lu, count:%lu\n", inputs[0].num_kvs-1,locCounter.loc.value, locCounter.count.value));

		// parse phone number
		phone.parse( inputs[0].kvs[0]->key );

		// emit the location with the highest number of calls
		writer->emit(0, &phone, &refLocCounter );
	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace mob

#endif