
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_fake_users
#define _H_SAMSON_sna_fake_users


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class fake_users : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
			samson::system::UInt id;
			User user;
			int num_nodes = 1000;

			for (int i = 0 ; i < num_nodes ; i++)
			{
				id.value = i+1;
				user.id = i+1;

				user.age = rand()%100;

				if( rand()%100 > 10 )
					writer->emit(0, &id, &user);
			}
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif