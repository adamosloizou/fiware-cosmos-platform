#ifndef WORKER_ENDPOINT_H
#define WORKER_ENDPOINT_H

/* ****************************************************************************
*
* FILE                     WorkerEndpoint.h
*
* DESCRIPTION              Class for worker endpoints
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
* WorkerEndpoint - 
*/
class WorkerEndpoint : public Endpoint2
{
public:
	WorkerEndpoint
	(
		EndpointManager* _epMgr,
		int              _id,
		Host*            _host,
		int              _rFd   = -1,
		int              _wFd   = -1
	);
	~WorkerEndpoint();

	Status msgTreat2(Packet* packetP);
};

}
#endif
