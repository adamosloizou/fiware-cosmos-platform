
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob_reduce_stat_count_different_days
#define _H_SAMSON_mob_reduce_stat_count_different_days


#include <samson/module/samson.h>


namespace samson{
namespace mob{


class reduce_stat_count_different_days : public samson::Reduce
{

	// Input[0k] & Output[0k]
	samson::system::UInt phone;
	// Output[0v]
	samson::system::UInt number_days;

public:

#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt system.Void
	output: system.UInt system.UInt

	helpLine: Count different days with calls per phone.
	extendedHelp: 		Count different days with calls per phone.

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	/**
	 * Count different days with calls per phone.
	 */
	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		if( inputs[0].num_kvs > 0 )
		{
			phone.parse( inputs[0].kvs[0]->key );
			number_days.value = inputs[0].num_kvs;
			writer->emit(0, &phone, &number_days );
			//OLM_T(LMT_User06, ("For phone:%lu, number_days:%lu", phone.value, number_days.value));
		}
	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace samson
} // end of namespace mob

#endif
