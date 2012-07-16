
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob_map_place_cell
#define _H_SAMSON_mob_map_place_cell


#include <samson/module/samson.h>


namespace samson{
namespace mob{


class map_place_cell : public samson::Map
{

	// Input[0v] & Output[0v]
	Place place;
	// Output [0k]
	samson::system::UInt32 cellId;

public:

#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob.Place
	output: system.UInt32 mob.Place

	helpLine: Maps place results into a set of data where the key is the cell id.
	extendedHelp: 		Maps place results into a set of data where the key is the cell id.

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	/**
	 * Map place results into a set of
	 * data where the key is the cell id.
	 */
	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		for (size_t i = 0 ; i < inputs[0].num_kvs ; ++i)
		{
			//Parsing key and value
			place.parse( inputs[0].kvs[i]->value );
			cellId.value = place.cellId.value;
			writer->emit (0, &cellId, &place);
		}
	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace mob

#endif
