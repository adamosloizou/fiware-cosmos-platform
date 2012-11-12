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

#ifndef _H_SAMSON_mob2_borrar_get_bts_vector
#define _H_SAMSON_mob2_borrar_get_bts_vector


#include <samson/module/samson.h>
#include <samson/modules/mob2/ClusterVector.h>
#include <samson/modules/mob2/Node_Bts.h>
#include "macros_parse_fields_mob2.h"
#include "mob2_environment_parameters.h"



namespace samson{
namespace mob2{


class borrar_get_bts_vector : public samson::Parser
{
	// Environment variable
	int coordsLength;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	output: mob2.Node_Bts mob2.ClusterVector

	extendedHelp: 		Parse a txt file to extract bts vectors from Roc�o

#endif // de INFO_COMMENT

	void init( samson::KVWriter *writer )
	{
		coordsLength = environment->getInt(MOB2_PARAMETER_COORDS_LENGTH, MOB2_PARAMETER_COORDS_LENGTH_DEFAULT);
	}

	/******* Extraer bts Rocío ******/
	void parseLines( char *line, samson::KVWriter *writer )
	{
		// Input: txt
		// Outputs
		Node_Bts nodbts;
		ClusterVector btsCoord;

		samson::system::Double coord;
		int id;

		btsCoord.comsSetLength(0);
		MR_PARSER_INIT(line);
		GET_CLUST_INIT_MX
		GET_CLUST_NEXT_FIELD
		GET_BTS_ID( id )
		nodbts.bts.value = id;
		GET_CLUST_NEXT_FIELD
		GET_CLUST_NEXT_FIELD
		nodbts.phone.value = nodbts.wday.value = nodbts.range.value = 0;
		for(int j=0; j<coordsLength; j++)
		{
			GET_CLUST_NEXT_FIELD
			GET_CLUST_GET_COORD(coord.value)
			btsCoord.comsAdd()->copyFrom(&coord);
		}
		writer->emit(0, &nodbts,&btsCoord);
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


} // end of namespace mob2
} // end of namespace samson

#endif
