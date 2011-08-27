
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob_map_stat_days
#define _H_SAMSON_mob_map_stat_days


#include <samson/module/samson.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace mob{


	class map_stat_days : public samson::Map
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: mob.PhoneLocCounter system.Void  
output: mob.PhoneCounter system.Void

helpLine: Removes location information and keeps only day information.
extendedHelp: 		Removes location information and keeps only day information.

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

/**
 * Removes location information and keeps
 * only day information in order to compute
 * the number of different days with calls
 * per phone number.
 * @return Void.
 */
		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			PhoneLocCounter phoneLocCounter;
			PhoneCounter phoneCounter;

			samson::system::Void void_data;

			for (int i = 0 ; i < inputs[0].num_kvs ; ++i)
			{
				//Parsing key and value
				phoneLocCounter.parse( inputs[0].kvs[i]->key );
				phoneCounter.phone.value = phoneLocCounter.phone.value;
				phoneCounter.count = phoneLocCounter.count;

				writer->emit(0, &phoneCounter, &void_data);
			}
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace mob

#endif