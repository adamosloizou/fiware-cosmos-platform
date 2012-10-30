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

#ifndef _H_SAMSON_cdr_parse_clients
#define _H_SAMSON_cdr_parse_clients


#include <samson/module/samson.h>

#include "cdr_parse_fields.h"


namespace samson{
namespace cdr{


class parse_clients : public samson::Parser
{

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: system.UInt cdr.User

	helpLine: Parse the clients sample.
	extendedHelp: 		Parse the clients sample.

#endif // de INFO_COMMENT


	///Function to parse a line and convert it into
	///a phone number; used to
	///parse the customers sample.
	///
	///@param line Pointer to the line to be parsed.
	///@param phone Pointer to the phone number.
	///
	///@return TRUE if successful parsing, FALSE in case
	///   of error.
	bool gstGetClientPhone( char *line, samson::system::UInt *phone, User *user ){
		unsigned int _pos = 0;
		unsigned int _pos_field = 0;

		try{
			// Client phone number
			cdrGetNextField( line, &_pos, &_pos_field );
			cdrPhoneToNumber( line+_pos_field, &(phone->value), CONF_MOB_PHONE_LENGTH );
			if( phone->value != 0L ){
				user->id = *phone;
			}else{
				throw false;
			}

			user->titularId.value = 0;
			user->type.value = 0;
			user->activationDate.value = 0;
			user->activationCode.value = 0;
			user->age.value = 0;
			user->province.value = 0;
			user->sex.value = 0;
			user->churnDate.value = 0;
			user->churnCode = 0;


		}catch( ... ){
			OLM_D(("Line rejected: '%s'\n", line));
			return false;
		}

		return true;
	}



	void init( samson::KVWriter *writer )
	{
	}

	void parseLines( char *line, samson::KVWriter *writer )
	{
		//Datas to emit
		samson::system::UInt phone;
		User user;

		if( gstGetClientPhone( line, &phone, &user ) == true ){
#define MAX_STR_LEN 1024
				//char output[MAX_STR_LEN];
				//ctime_r(&(cdr.time.value), output);
				//output[strlen(output)-1] = '\0';
				//OLM_T(LMT_User06, ("cdr: time: '%s', week_day: %d\n", output, int(cdr.week_day.value)));

				writer->emit(0, &phone, &user );

			}
			else
			{
				OLM_E(("Error parsing line:'%s' for client", line));
				;
			}
		}

		void run( char *data , size_t length , samson::KVWriter *writer )
		{
			size_t offset = 0;
			size_t line_begin = 0;

			while( offset < length )
			{

				if( data[offset] == '\n')
				{
					data[offset] = '\0';

					parseLines(data+line_begin , writer);

					line_begin = offset+1;
				}
				++offset;
			}
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace cdr

#endif
