#ifndef PLATFORM_PROCESSES_H
#define PLATFORM_PROCESSES_H

/* ****************************************************************************
*
* FILE                     platformProcesses.h - get/set platform process list
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Mar 02 2011
*
*/
#include "samson/common/Process.h"            // Process


namespace samson
{



/* ****************************************************************************
*
* platformProcessesSave - 
*/
extern void platformProcessesSave(ProcessVector* wvP);



/* ****************************************************************************
*
* platformProcessesGet -
*/
extern ProcessVector* platformProcessesGet(int* sizeP = NULL);

}

#endif