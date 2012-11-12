/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_testcomparefunctions_gen_users
#define _H_SAMSON_testcomparefunctions_gen_users


#include <samson/module/samson.h>

#include <sstream>


namespace samson{
namespace testcomparefunctions{


	class gen_users : public samson::Generator
	{

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

output: system.UInt testcomparefunctions.User

helpLine: Generates user information userId-surname-company-age-expense
#endif // de INFO_COMMENT

    		void init (samson::KVWriter *writer)
		{
			OLM_T(LMT_User06, ("class gen_users : public samson::Generator: init()\n"));
		}

		void run( samson::KVWriter *writer )
		{
			User user;
			for (int i = 0; (i < 1000); i++)
			{
				user.unassignAllOptionals();
				user.id.value = i;
				std::ostringstream o;
				o << "User_" << i;
				user.surname.value = o.str();
				if (i%4)
				{
					o.str("");
					o << "Street_" << i%10;
					user.address.street.value = o.str();
					user.address.number.value = i%100;
					o.str("");
					o << "City_" << i%10;
					user.address.city.value = o.str();
					user.address.ZIP.value = 8000 + i%50;
					user.address_SetAssigned(true);
				}
				if (i%7)
				{
					user.age.value = i%100;
					user.age_SetAssigned(true);
				}
				user.companyId.value = i%5;
				writer->emit( 0, &user.id , &user );
			}
		}

    		void finish(samson::KVWriter *writer)
		{
			OLM_T(LMT_User06, ("class gen_users : public samson::Generator: finish()\n"));
		}


	};


} // end of namespace samson
} // end of namespace testcomparefunctions

#endif
