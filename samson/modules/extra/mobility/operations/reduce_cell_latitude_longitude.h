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

#ifndef _H_SAMSON_mobility_reduce_cell_latitude_longitude
#define _H_SAMSON_mobility_reduce_cell_latitude_longitude


#include <samson/module/samson.h>
#include <samson/modules/mobility/Cell.h>
#include <samson/modules/mobility/CellRecord.h>
#include <samson/modules/mobility/Record.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt32.h>


namespace samson{
namespace mobility{


	class reduce_cell_latitude_longitude : public samson::Reduce
	{

	   samson::mobility::CellRecord cell_record;  // Input record
	   samson::mobility::Record record;           // Output record
	   samson::mobility::Cell cell;               // Information about cell

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt32 mobility.CellRecord  
input: system.UInt32 mobility.Cell  
output: system.UInt mobility.Record
output: system.UInt32 mobility.CellRecord

helpLine: Add lat-lon information to Records. Note that key at the input is cellid,key at the output user-id
#endif // de INFO_COMMENT

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		   if( inputs[1].num_kvs == 0)
		   {
			  // Non existing cell... forward input to output 1
			  for ( size_t i = 0 ; i< inputs[0].num_kvs ; i++)
			  {
				 cell_record.parse( inputs[0].kvs[i]->value );
				 writer->emit( 1 , &cell_record.cellId , &cell_record );
			  }
			  return;
		   }


		   cell.parse( inputs[1].kvs[0]->value ); // Only consider the first one...

		   for ( size_t i = 0 ; i< inputs[0].num_kvs; i++)
		   {
			  cell_record.parse( inputs[0].kvs[i]->value );

			  // Complete the complete record
			  record.userId.value = cell_record.userId.value;
			  record.timestamp.value = cell_record.timestamp.value;
			  record.cellId.value = cell_record.cellId.value;

			  record.position.copyFrom( &cell.position );

			  writer->emit( 0 , &record.userId , &record );
		   } 
		   return;


		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mobility
} // end of namespace samson

#endif
