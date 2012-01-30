#ifndef CONTROLLER_ENDPOINT_H
#define CONTROLLER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     ControllerEndpoint.h
*
* DESCRIPTION              Class for controller endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 02 2011
*
*/
#include "samson/network/Endpoint2.h"          // Endpoint2



namespace samson
{



/* ****************************************************************************
*
* ControllerEndpoint - 
*/
class ControllerEndpoint : public Endpoint2
{
public:
	ControllerEndpoint
	(
		EndpointManager* _epMgr,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~ControllerEndpoint();

	Status msgTreat2(Packet* packetP);
	Status msgTreat2(void);
};

}
#endif