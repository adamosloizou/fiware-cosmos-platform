
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_get_itineraries
#define _H_SAMSON_mob2_get_itineraries


#include <samson/module/samson.h>
#include <samson/modules/mob2/ItinRange.h>
#include <samson/modules/mob2/Itinerary.h>
#include <samson/modules/mob2/TwoInt.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace mob2{


class get_itineraries : public samson::Reduce
{
	//Inputs
	ItinRange moves;
	TwoInt wday_count;
	//Outputs
	samson::system::UInt node;
	Itinerary itinerary;

	//Intermediate
	samson::system::UInt wday;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: mob2.ItinRange mob2.TwoInt
	output: system.UInt mob2.Itinerary

	extendedHelp: 		Calculate itineraries for a client

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{

		itinerary.num_moves = 0;
		itinerary.wdaysSetLength(0);

		moves.parse(inputs[0].kvs[0]->key);
		node.value = moves.node.value;
		itinerary.source.value = moves.poiSrc.value;
		itinerary.target.value = moves.poiTgt.value;
		itinerary.range.value = moves.range.value;

		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			wday_count.parse(inputs[0].kvs[i]->value);
			itinerary.num_moves.value += wday_count.num2.value;
			wday.value = wday_count.num1.value;
			itinerary.wdaysAdd()->copyFrom(&wday);
		}

		if(itinerary.num_moves.value > 1) //Filter itineraries over by number of movements
		{
			writer->emit(0, &node,&itinerary);
		}


	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
