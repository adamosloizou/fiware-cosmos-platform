
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_print_user_statistics
#define _H_SAMSON_mob2_print_user_statistics


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


	class print_user_statistics : public samson::ParserOutReduce
	{
		//Inputs
		samson::system::UInt node;
		samson::system::TimeUnix firstTimeStamp;
		samson::system::TimeUnix lastTimeStamp;

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif
#define MAX_STR_LEN 1024
	char output[MAX_STR_LEN];

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt system.TimeUnix

extendedHelp: 		For every user, prints number of CDRs with cellId, and total period in seconds

#endif // de INFO_COMMENT

		void init(TXTWriter *writer )
		{
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			if (inputs[0].num_kvs == 0)
			{
				return;
			}
			node.parse(inputs[0].kvs[0]->key);
			firstTimeStamp.parse(inputs[0].kvs[0]->value);
			lastTimeStamp.parse(inputs[0].kvs[(inputs[0].num_kvs-1)]->value);

			snprintf(output, MAX_STR_LEN, "%lu|%lu|%s|%s\n", node.value, inputs[0].num_kvs, firstTimeStamp.str().c_str(), lastTimeStamp.str().c_str());
			writer->emit(output);
		}

		void finish(TXTWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
