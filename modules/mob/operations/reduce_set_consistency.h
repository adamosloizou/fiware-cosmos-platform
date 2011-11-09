
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob_reduce_set_consistency
#define _H_SAMSON_mob_reduce_set_consistency


#include <samson/module/samson.h>


namespace samson{
namespace mob{


class reduce_set_consistency : public samson::Reduce
{

	// Input[01k]
	samson::system::UInt32 cellId;
	// Input[0v] & Output[0v]
	Place place;
	// Input[1v]
	samson::cdr::Cell cell;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt32 mob.Place
	input: system.UInt32 cdr.Cell
	output: system.UInt mob.Place

	helpLine: Make location results consistent.
	extendedHelp: 		Make location results consistent.

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	/**
	 * Make location results consistent.
	 */
	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		if( inputs[1].num_kvs > 0 )
		{
			if (inputs[0].num_kvs > 0)
			{
				cellId.parse(inputs[0].kvs[0]->key);
			}
			else
			{
				//cellId.parse(inputs[1].kvs[0]->key);
				//OLM_T(LMT_User06, ("Aborts  cellId:%lu with inputs[0].num_kvs:%lu, inputs[1].num_kvs:%lu", cellId.value, inputs[0].num_kvs, inputs[1].num_kvs));
				return;
			}

			cell.parse( inputs[1].kvs[0]->value );

			for( size_t i=0; i<inputs[0].num_kvs; i++ )
			{
				place.parse( inputs[0].kvs[i]->value );

				// check consistency and fix if necessary,
				// assuming cell ids are equal

				// bts level
				if( place.btsId != cell.btsId )
				{
					place.btsId = cell.btsId;
					place.btsDaysWithCalls = place.cellDaysWithCalls;
					//OLM_T(LMT_User06, ("For cellId: %lu, (place.btsId(%lu) != cell.btsId(%lu)), update place.btsDaysWithCalls:%lu", cellId.value, place.btsId.value, cell.btsId.value, place.cellDaysWithCalls.value));
				}
				else
				{
					if( place.btsDaysWithCalls < place.cellDaysWithCalls )
					{
						//OLM_T(LMT_User06, ("For cellId: %lu, (place.btsDaysWithCalls(%lu) < place.cellDaysWithCalls(%lu)), update place.btsDaysWithCalls:%lu", cellId.value, place.btsDaysWithCalls.value, place.cellDaysWithCalls.value, place.cellDaysWithCalls.value));
						place.btsDaysWithCalls = place.cellDaysWithCalls;
					}
				}

				// lac level
				if( place.lacId != cell.lacId )
				{
					place.lacId = cell.lacId;
					place.lacDaysWithCalls = place.btsDaysWithCalls;
				}
				else
				{
					if( place.lacDaysWithCalls < place.btsDaysWithCalls )
					{
						place.lacDaysWithCalls = place.btsDaysWithCalls;
					}
				}

				// state level
				if( place.stateId != cell.stateId )
				{
					place.stateId = cell.stateId;
					place.stateDaysWithCalls = place.lacDaysWithCalls;
				}
				else
				{
					if( place.stateDaysWithCalls < place.lacDaysWithCalls )
					{
						place.stateDaysWithCalls = place.lacDaysWithCalls;
					}
				}

				// emit
				writer->emit(0, &place.phone, &place );
			}
		}
		else
		{
			if (inputs[0].num_kvs != 0)
			{
				cellId.parse(inputs[0].kvs[0]->key);
				OLM_T(LMT_User06, ("Aborts with cellId: %lu, unknown cell: inputs[0].num_kvs:%lu, inputs[1].num_kvs:%lu", cellId.value, inputs[0].num_kvs, inputs[1].num_kvs));
			}
		}
	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace mob

#endif