
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_reduce_update_last_position
#define _H_SAMSON_mob2_reduce_update_last_position


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/mob2/UserState.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


	class reduce_update_last_position : public samson::Reduce
	{

		//Inputs
		samson::system::UInt node;
		samson::cdr::mobCdr cdr;

		//State: Input & Output
		samson::mob2::UserState state;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.mobCdr  
input: system.UInt mob2.UserState  
output: system.UInt mob2.UserState

helpLine: Updates last position information for every user
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			if (inputs[0].num_kvs == 0)
			{
				// No new information about user
				return;
			}

			node.parse(inputs[0].kvs[0]->key);

			if (inputs[1].num_kvs > 1)
			{
				OLM_E(("Error, more than one state(%lu) per user:%lu", inputs[1].num_kvs, node.value));
				return;
			}

			if (inputs[1].num_kvs == 0)
			{
				// We had not yet information about this user
				OLM_T(LMT_User06, ("New user:%lu detected", node.value));
				cdr.parse(inputs[0].kvs[0]->value);

				state.imei.value = 0;
				state.position.cell.value = cdr.cellId.value;
				state.position.time = cdr.timeUnix;
			}
			else
			{
				OLM_T(LMT_User06, ("Existing user:%lu detected", node.value));
				state.parse(inputs[1].kvs[0]->value);
			}

			for (uint64_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				cdr.parse(inputs[0].kvs[i]->value);
				if (cdr.cellId.value != state.position.cell.value)
				{
					state.position.cell.value = cdr.cellId.value;
					state.position.time = cdr.timeUnix;
				}
			}
			writer->emit(0, &node, &state);
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
