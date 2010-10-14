#ifndef IOM_CONNECT
#define IOM_CONNECT

/* ****************************************************************************
*
* FILE                     iomConnect.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <vector>               // vector
#include "Endpoint.h"			// Endpoint



/* ****************************************************************************
*
* iomConnect -  connect to the controller
*/
extern int iomConnect(const char* ip, unsigned short port);

#endif
