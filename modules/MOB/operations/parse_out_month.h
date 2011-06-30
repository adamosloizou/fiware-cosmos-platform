
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_MOB_parse_out_month
#define _H_SAMSON_MOB_parse_out_month


#include <samson/module/samson.h>
#include <samson/modules/system/Date.h>


namespace samson{
namespace MOB{


	class parse_out_month : public samson::ParserOut
	{

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif

#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.Date system.Void

helpLine: Write month into a readable file.
extendedHelp: 		Write month into a readable file.

#endif // de INFO_COMMENT

		void init(TXTWriter *writer )
		{
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			samson::system::Date date;


			  for (int i  =0 ;  i < inputs[0].num_kvs ; ++i)
			  {
				date.parse(inputs[0].kvs[i]->key);

			    snprintf(output, MAX_STR_LEN, "%4d%02d\n", date.year.value, date.month.value);

			    writer->emit(output);
			  }
			  return;
		}

		void finish(TXTWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace MOB

#endif
