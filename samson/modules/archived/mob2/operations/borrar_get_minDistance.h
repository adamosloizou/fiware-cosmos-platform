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

#ifndef _H_SAMSON_mob2_borrar_get_minDistance
#define _H_SAMSON_mob2_borrar_get_minDistance


#include <samson/module/samson.h>
#include <samson/modules/mob2/ItinRange.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace mob2{


	class borrar_get_minDistance : public samson::Reduce
	{
		//Inputs
		samson::system::UInt r1, r2;
		//Outputs
		samson::system::UInt mindist;
		samson::system::Void mr_void;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: mob2.ItinRange system.UInt  
output: system.UInt system.Void

extendedHelp: 		Get min distance between itineraries by client with the same movement

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			int min = 13;
			int rg1, rg2;

			if(inputs[0].num_kvs == 1){	mindist.value = 0;}

			for(uint64_t i=0; i<inputs[0].num_kvs - 1; i++)
			{
				r1.parse(inputs[0].kvs[i]->value);
				rg1 = r1.value;
				for(uint64_t j=i+1; j<inputs[0].num_kvs; j++)
				{
					r2.parse(inputs[0].kvs[j]->value);
					rg2 = r2.value;
					if(abs(rg1 - rg2) < min)
					{
						min = abs(rg1 - rg2);
					}
				}
			}
			mindist.value = min;
			writer->emit(0, &mindist,&mr_void);
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
