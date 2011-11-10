/* ****************************************************************************
*
* FILE			paFrom.c - 
*
* AUTHOR		Ken Zangelin
*
* HISTORY
* $Log: paFrom.c,v $
*
*/
#include <stdio.h>              /* stderr, stderr, ...                       */
#include <string.h>             /* strncmp, strspn, ...                      */
#include <stdlib.h>             /* strtol, ...                               */

#include "baStd.h"              /* BA standard header file                   */
#include "logMsg/logMsg.h"             /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paPrivate.h"          /* PafDefault, ...                           */
#include "paIterate.h"          /* paIterateInit, paIterateNext              */
#include "paEnvVals.h"          /* paEnvName                                 */
#include "paFrom.h"             /* Own interface                             */



/* ****************************************************************************
*
* paFromName - 
*/
char* paFromName(PaArgument* aP, char* out)
{
	switch (aP->from)
	{
	case PafError:        strcpy(out, "error");                 return out;
	case PafUnchanged:    strcpy(out, "not altered");           return out;
	case PafDefault:      strcpy(out, "default value");         return out;
	case PafEnvVar:       strcpy(out, "environment variable");  return out;
	case PafRcFile:       strcpy(out, "RC file");               return out;
	case PafArgument:     strcpy(out, "command line argument"); return out;
	}

	strcpy(out, "Yes, from where?");
	return out;
}



/* ****************************************************************************
*
* paFrom - from where did the value come?
*/
char* paFrom(PaArgument* paList, const char* name)
{
	PaArgument* aP;
	int         ix;

	paIterateInit();
	ix = 0;
	while ((aP = paIterateNext(paList)) != NULL)
	{
		char envVarName[128];

		paEnvName(aP, envVarName, ix);
		++ix;

		if ((aP->option) && strcmp(aP->option, name) == 0)
			break;
		if ((aP->envName) && strcmp(envVarName, name) == 0)
			break;
	}

	if (aP->type == PaLastArg)
		return (char*) "unrecognized option";

	switch (aP->from)
	{
	case PafError:        return (char*) "error";
	case PafUnchanged:    return (char*) "not altered";
	case PafDefault:      return (char*) "default value";
	case PafEnvVar:       return (char*) "environment variable";
	case PafRcFile:       return (char*) "RC file";
	case PafArgument:     return (char*) "command line argument";
	}

	return (char*) "origin unknown";
}



