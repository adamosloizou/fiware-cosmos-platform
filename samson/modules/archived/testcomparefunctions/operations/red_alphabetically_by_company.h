
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_testcomparefunctions_red_alphabetically_by_company
#define _H_SAMSON_testcomparefunctions_red_alphabetically_by_company


#include <samson/module/samson.h>


namespace samson{
namespace testcomparefunctions{


	class red_alphabetically_by_company : public samson::ParserOutReduce
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt testcomparefunctions.User  compareValueFunction=compare_by_surname

helpLine: For every company, prints consumers alphabetically
#endif // de INFO_COMMENT

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif
// Max line to be output
#define MAX_STR_LEN 2048
    char line[MAX_STR_LEN];

    		void init( TXTWriter *writer)
		{
			OLM_T(LMT_User06, ("class red_alphabetically_by_company : public samson::ParserOutReduce: init()\n"));
			writer->emit("Hello, world!\n");
		}

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			system::UInt companyId;
			User user;
			size_t expense;

			if (inputs[0].num_kvs < 1)
			{
				OLM_E(("No users for this companies group\n"));
				return;
			}
			companyId.parse(inputs[0].kvs[0]->key);
			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
			{
				user.parse(inputs[0].kvs[i]->value);
                                if (user.expense_GetAssigned())
                                {
                                        expense = user.expense.value;
                                }
                                else
                                {
                                        expense = 0;
                                }

				snprintf( line , MAX_STR_LEN, "companyId:%lu, user:'%s', expense:%lu\n", companyId.value, user.surname.value.c_str(), expense );
				writer->emit( line );
			}
		}

    		void finish(TXTWriter *writer)
		{
			OLM_T(LMT_User06, ("class red_alphabetically_by_company : public samson::ParserOutReduce: finish()\n"));
			writer->emit("That's all, folks\n");
		}
	};


} // end of namespace samson
} // end of namespace testcomparefunctions

#endif
