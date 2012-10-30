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

#ifndef _H_SAMSON_mob2_itin_join_cell_bts
#define _H_SAMSON_mob2_itin_join_cell_bts


#include <samson/module/samson.h>
#include <samson/modules/cdr/CellPos.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/mob2/ItinTime.h>
#include <samson/modules/mob2/TwoInt.h>
#include <samson/modules/system/UInt32.h>
#include <samson/modules/system/DateComplete.h>
#include <samson/modules/system/Time.h>




namespace samson{
namespace mob2{


class itin_join_cell_bts : public samson::Reduce
{
	// Inputs
	samson::cdr::mobCdr cdr;
	samson::cdr::CellPos cell_info;
	// Outputs
	TwoInt node_bts; // num1: node   num2: bts
	ItinTime it_time;
	// Local variables to handle time
	samson::system::DateComplete lDate;
	samson::system::Time lTime;


public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt32 cdr.mobCdr
	input: system.UInt32 cdr.CellPos
	output: mob2.TwoInt mob2.ItinTime

	extendedHelp: 		Change cell of cdrs by bts for itineraries
	Input 1: Cdrs with cell info
	Input 2: Cells catalogue

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		// Cell catalogue
		for(uint64_t i=0; i<inputs[1].num_kvs; i++)
		{
			cell_info.parse(inputs[1].kvs[i]->value);

			for(uint64_t j=0; j<inputs[0].num_kvs; j++)
			{
				cdr.parse(inputs[0].kvs[j]->value);
				node_bts.num1.value = cdr.phone.value;
				node_bts.num2.value = cell_info.bts.value;
				cdr.timeUnix.getDateTimeFromTimeUTC(&lDate, &lTime);
				it_time.date.day.value = lDate.day.value;
				it_time.date.month.value = lDate.month.value;
				it_time.date.year.value = lDate.year.value;
				it_time.date.days_2000.value = lDate.days_2000.value;
				it_time.date.week_day.value = 0;
				it_time.time.hour.value = lTime.hour.value;
				it_time.time.minute.value = lTime.minute.value;
				it_time.time.seconds.value = lTime.seconds.value;
				it_time.bts.value = cell_info.bts.value;
				writer->emit(0, &node_bts,&it_time);
			}
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif
