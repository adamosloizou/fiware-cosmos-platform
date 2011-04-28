/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 08 2011
*
*/
#include <sys/select.h>         // select

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "NetworkInterface.h"	// ss:NetworkInterface 
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // EndpointManager
#include "Network2.h"           // Own interface



namespace ss
{



/* ****************************************************************************
*
* Constructor
*/
Network2::Network2(EndpointManager* _epMgr)
{
	epMgr     = _epMgr;
	epMgr->tmoSet(2, 0);
}



/* ****************************************************************************
*
* tmoSet - 
*/
void Network2::tmoSet(int secs, int usecs)
{
	epMgr->tmoSet(secs, usecs);
}



/* ****************************************************************************
*
* run
*/
void Network2::run(bool oneShot)
{
	epMgr->run(oneShot);
}



/* ****************************************************************************
*
* setPacketReceiver - 
*/
void Network2::setPacketReceiver(PacketReceiverInterface* receiver)
{
	epMgr->setPacketReceiver(receiver);
}

}
