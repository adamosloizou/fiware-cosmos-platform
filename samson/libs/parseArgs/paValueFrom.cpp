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
/* ****************************************************************************
*
* FILE			paArgs.c - source file for the parse arguments utility
*
* AUTHOR		Ken Zangelin
*
* HISTORY
* $Log: paArgs.c,v $
*
*/
#include <string.h>                   /* strcmp                              */

#include "parseArgs/parseArgs.h"      /* PaFrom, PaArgument                  */
#include "parseArgs/paPrivate.h"      /* paiList                             */
#include "parseArgs/paIterate.h"      /* paIterateInit, paIterateNext        */
#include "parseArgs/paEnvVals.h"      /* paEnvName                           */
#include "parseArgs/paValueFrom.h"    /* Own interface                       */



/* ****************************************************************************
*
* paValueFrom - 
*/
PaFrom paValueFrom(char* oName)
{
	PaiArgument* aP;

	paIterateInit();
	while ((aP = paIterateNext(paiList)) != NULL)
	{
		char envVarName[64];

		if (aP->option == NULL)
			continue;

		paEnvName(aP, envVarName);

		if (aP->option && (strcmp(oName, aP->option) == 0))
			return aP->from;
		else if (aP->envName && (strcmp(oName, envVarName) == 0))
			return aP->from;
	}  

	return PafError;
}
