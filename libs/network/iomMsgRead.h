#ifndef IOM_MSG_READ
#define IOM_MSG_READ

/* ****************************************************************************
*
* FILE                     iomMsgRead.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include "Packet.h"             // Packet
#include "Message.h"            // MessageType, Code, etc.
#include "Endpoint.h"           // Endpoint



/* ****************************************************************************
*
* iomMsgPartRead - read from a socket until completed or error
*/
ssize_t iomMsgPartRead(ss::Endpoint* ep, const char* what, char* buf, ssize_t bufLen);



/* ****************************************************************************
*
* iomMsgRead - read a message from an endpoint
*/
extern int iomMsgRead
(
	ss::Endpoint*              ep,
	ss::Message::Header*       headerP,
	ss::Message::MessageCode*  msgCodeP,
	ss::Message::MessageType*  msgTypeP,
	void**                     dataPP,
	int*                       dataLenP,
	ss::Packet*                packetP    = NULL,
	void*                      kvData     = NULL,
	int*                       kvDataLenP = NULL
);

#endif
