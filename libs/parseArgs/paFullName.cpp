#include <stdio.h>              /* sprintf                                   */

#include "parseArgs/parseArgs.h"          /* PaArgument                                */
#include "paFullName.h"         /* Own interface                             */



/* ****************************************************************************
*
* paFullName - fill in name and description in string string
*/
char* paFullName(char* string, PaArgument* aP)
{
	if ((aP->option == NULL) || (aP->option[0] == 0))
		sprintf(string, "variable %s", aP->description);
	else if (aP->type == PaBoolean)
		sprintf(string, "%s (%s)", aP->name, aP->description);
	else
		sprintf(string, "%s <%s>", aP->name, aP->description);

	return string;
}
