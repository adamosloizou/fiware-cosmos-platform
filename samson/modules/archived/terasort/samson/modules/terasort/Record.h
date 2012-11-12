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
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_terasort_Record
#define _H_SAMSON_terasort_Record


#include <samson/modules/terasort/Record_base.h>


namespace samson{
namespace terasort{


	class Record : public Record_base
	{
	public:

		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
			{ // comparing keyf
#define NUMBYTES_KEYF 10
				int tmp = strncmp(data1,data2,NUMBYTES_KEYF);
				//OLM_T(LMT_User01, ("For data1:'%s', and data2:'%s', returns:%d", data1, data2, tmp));
				return tmp;
			}

			return 0; //We've done
		}

		inline static int compare( char* data1 , char* data2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( data1 , data2 , &offset_1 , &offset_2 );
		}
	};


} // end of namespace samson
} // end of namespace terasort

#endif
