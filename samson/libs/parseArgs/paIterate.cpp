#include "baStd.h"               /* BA standard header file                   */
#include "logMsg/logMsg.h"       /* LM_ENTRY, LM_EXIT, ...                    */

#include "parseArgs/parseArgs.h" /* PaArgument                                */
#include "paLog.h"               /* PA_XXX                                    */
#include "paBuiltin.h"           /* paBuiltin, paBuiltinNoOf                  */
#include "paPrivate.h"           /* PawBuiltin                                */
#include "paIterate.h"           /* Own interface                             */



/* ****************************************************************************
*
* static variables
*/
static int  ix       = 0;
int         builtins = -1;



/* ****************************************************************************
*
* paIterateInit - 
*/
void paIterateInit(void)
{
	PA_M(("Preparing to iterate"));
	ix       = 0;
	builtins = paBuiltinNoOf();
}



/* ****************************************************************************
*
* paIterateNext - 
*/
PaiArgument* paIterateNext(PaiArgument* paList)
{
	PaiArgument* aP;

	if (builtins == -1)
		paIterateInit();

	PA_M(("builtins == %d (ix == %d)", builtins, ix));
	do
	{
		if (ix < builtins)
		{
			aP = &paBuiltin[ix];
			aP->what |= PawBuiltin;
			PA_M(("Found builtin '%s'", aP->option));
		}
		else if (paList != NULL)
			aP = &paList[ix - builtins];
		else
		  return NULL;

		++ix;
	} while (aP->removed == true);

	if (aP->type == PaLastArg)
		return NULL;

	return aP;
}
