/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <sys/select.h>         // select
#include <string>               // string
#include <vector>				// ss::vector
#include <sys/types.h>          // pid_t
#include <unistd.h>             // fork, getpid
#include <sched.h>              // sched_setaffinity
#include <pthread.h>            // pthread_t

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*
#include "Alarm.h"              // ALARM

#include "Misc.h"               // ipGet 
#include "Endpoint.h"			// Endpoint
#include "Message.h"            // ss::Message::MessageCode
#include "Packet.h"				// Packet
#include "MemoryManager.h"      // MemoryManager
#include "iomInit.h"            // iomInit
#include "iomServerOpen.h"      // iomServerOpen
#include "iomConnect.h"         // iomConnect
#include "iomAccept.h"          // iomAccept
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead
#include "iomMsgAwait.h"        // iomMsgAwait
#include "workerStatus.h"       // workerStatus, WorkerStatusData
#include "Network.h"			// Own interface



/* ****************************************************************************
*
* KILO - 
* MEGA - 
*/
#define KILO(x)                      (x * 1024)
#define MEGA(x)                      (KILO(x) * 1024)



/* ****************************************************************************
*
* THREAD_BUF_SIZE_THRESHOLD
* SEND_SIZE_TO_USE_THREAD
*/
#define THREAD_BUF_SIZE_THRESHOLD    (MEGA(10))
#define SEND_SIZE_TO_USE_THREAD      (MEGA(10))



namespace ss
{



/* ****************************************************************************
*
* Constructor 
*/
Network::Network()
{
	iAmReady     = false;
	receiver     = NULL;
	me           = NULL;
	listener     = NULL;
	controller   = NULL;
	Workers      = WORKERS;
	Endpoints    = 3 + WORKERS + DELILAHS + CORE_WORKERS + TEMPORALS;
	tmoSecs      = 10;
	tmoUsecs     = 0;

	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));
}



/* ****************************************************************************
*
* Constructor
*/
Network::Network(int endpoints, int workers)
{
	iAmReady     = false;
	receiver     = NULL;
	me           = NULL;
	listener     = NULL;
	controller   = NULL;
	Workers      = workers;
	Endpoints    = endpoints;
	tmoSecs      = 10;
	tmoUsecs     = 0;

	endpoint = (Endpoint**) calloc(Endpoints, sizeof(Endpoint*));
	if (endpoint == NULL)
		LM_XP(1, ("calloc(%d, %d)", Endpoints, sizeof(Endpoint*)));
}



/* ****************************************************************************
*
* setPacketReceiverInterface - set the element to be notified when packages arrive
*/
void Network::setPacketReceiverInterface(PacketReceiverInterface* _receiver)
{
	LM_T(LMT_DELILAH, ("Setting receiver to %p", _receiver));
	receiver = _receiver;
}
	


/* ****************************************************************************
*
* init - create endpoints 0 and 1
*/
void Network::init(Endpoint::Type type, const char* alias, unsigned short port, const char* controllerName)
{
	endpoint[0] = new Endpoint(type, port);
	if (endpoint[0] == NULL)
		LM_XP(1, ("new Endpoint"));
	me = endpoint[0];

	if (me == NULL)
		LM_XP(1, ("unable to allocate room for Endpoint 'me'"));

	me->name     = progName;
	me->state    = Endpoint::Me;
	me->ip       = ipGet();
	me->alias    = alias;

	if (strncmp(&alias[1], "orker", 5) == 0)
		me->workerId = atoi(&alias[6]);
	else
		me->workerId = -2;

	if (port != 0)
	{
		endpoint[1] = new Endpoint(*me);
		if (endpoint[1] == NULL)
			LM_XP(1, ("new Endpoint"));
		listener = endpoint[1];

		listener->rFd      = iomServerOpen(listener->port);
		if (listener->rFd == -1)
			LM_XP(1, ("unable to open port %d for listening", listener->port));

		listener->state    = Endpoint::Listening;
		listener->type     = Endpoint::Listener;
		listener->ip       = me->ip;
		listener->name     = "Listener";
		listener->wFd      = listener->rFd;

		LM_T(LMT_FDS, ("opened fd %d to accept incoming connections", listener->rFd));
	}

	if ((type == Endpoint::Worker) || (type == Endpoint::Delilah))
	{
		endpoint[2] = new Endpoint(Endpoint::Controller, controllerName);
		if (endpoint[2] == NULL)
			LM_XP(1, ("new Endpoint"));
		controller = endpoint[2];

		controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
		if (controller->rFd == -1)
			LM_X(1, ("error connecting to controller at %s:%d", controller->ip.c_str(), controller->port));

		controller->wFd   = controller->rFd;
		controller->state = ss::Endpoint::Connected;
	}
	else
	{
		endpoint[2] = NULL;
		controller  = NULL;
	}

	LM_F(("I am a '%s', my name: '%s', ip: %s", me->typeName(), me->nam(), me->ip.c_str()));
}



/* ****************************************************************************
*
* helloSend
*/
int Network::helloSend(Endpoint* ep, Message::MessageType type)
{
	ss::Message::HelloData hello;

	strncpy(hello.name,   me->name.c_str(),   sizeof(hello.name));
	strncpy(hello.ip,     me->ip.c_str(),     sizeof(hello.ip));
	strncpy(hello.alias,  me->alias.c_str(),  sizeof(hello.alias));

	hello.type     = me->type;
	hello.workers  = me->workers;
	hello.port     = me->port;
	hello.coreNo   = me->coreNo;
	hello.workerId = me->workerId;

	LM_T(LMT_WRITE, ("sending hello %s to '%s' (name: '%s', type: '%s')", messageType(type), ep->name.c_str(), hello.name, me->typeName()));

	return iomMsgSend(ep, me, Message::Hello, type, &hello, sizeof(hello));
}



#define WEB_SERVICE_PORT (unsigned short) 9898
/* ****************************************************************************
*
* initAsSamsonController - 
*/
void Network::initAsSamsonController(int port, int workers)
{
	init(Endpoint::Controller, "Controller", port);

	int   ix;
	char  alias[16];
	char  name[32];

	Workers = workers;

	for (ix = 0; ix < Workers; ix++)
	{
		snprintf(alias, sizeof(alias), "Worker%02d",  ix);
		snprintf(name,  sizeof(name),  "Worker %02d", ix);

		endpoint[3 + ix] = new Endpoint();
		if (endpoint[3 + ix] == NULL)
			LM_XP(1, ("new Endpoint"));

		endpoint[3 + ix]->rFd     = -1;
		endpoint[3 + ix]->wFd     = -1;
		endpoint[3 + ix]->name    = name;
		endpoint[3 + ix]->alias   = alias;
		endpoint[3 + ix]->workers = 0;
		endpoint[3 + ix]->state   = Endpoint::FutureWorker;
		endpoint[3 + ix]->type    = Endpoint::Worker;
		endpoint[3 + ix]->ip      = "II.PP";
		endpoint[3 + ix]->port    = 0;
		endpoint[3 + ix]->coreNo  = -1;

		LM_T(LMT_ENDPOINT, ("Created endpoint %d, worker %d (%s)", 3 + ix, ix, endpoint[3 + ix]->alias.c_str()));
	}


	int fd = iomServerOpen(WEB_SERVICE_PORT);
	if (fd == -1)
		LM_XP(1, ("error opening web service listen socket"));

	endpointAdd(fd, fd, "Web Listener", "Weblistener", 0, Endpoint::WebListener, "localhost", WEB_SERVICE_PORT);
}



/* ****************************************************************************
*
* controllerGetIdentifier - 
*/
int Network::controllerGetIdentifier(void)
{
	LM_T(LMT_DELILAH, ("Asking for controller id"));
	return 2;
}



/* ****************************************************************************
*
* workerGetIdentifier - 
*
* worker 0 is at index 3 in the local endpoint vector
*/
int Network::workerGetIdentifier(int nthWorker)
{
	return nthWorker + 3;
}
	
	

/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*
* worker 0 is at index 3 in the local endpoint vector
*/
int Network::getWorkerFromIdentifier(int identifier)
{
	if (identifier >= Workers)
		LM_RE(-1, ("invalid worker identifier '%d'  (only have %d workers)", identifier, Workers));

	if (identifier == 0)
		return me->workerId;

	return identifier - 3;
}



/* ****************************************************************************
*
* getNumWorkers - 
*
* Return number of workers in the system
*/
int Network::getNumWorkers(void)
{
	return Workers;

#if 0
	int  ix;
	int  workers = 0;

	LM_T(LMT_DELILAH, ("IN"));

	for (ix = 3; ix < 3 + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->state == Endpoint::Connected)
			++workers;
	}

	return workers;
#endif
}
	
	

/* ****************************************************************************
*
* samsonWorkerEndpoints - 
*
* Return list of connected workers ?
*/
std::vector<Endpoint*> Network::samsonWorkerEndpoints()
{
	int                     ix;
	std::vector<Endpoint*>  v;

	LM_X(1, ("IN"));

	for (ix = 3; ix <  3 + Workers; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->state == Endpoint::Connected)
			v.push_back(endpoint[ix]);
	}

	return v;
}
	


/* ****************************************************************************
*
* senderThread - 
*/
static void* senderThread(void* vP)
{
	Endpoint*  ep     = (Endpoint*) vP;
	
	LM_T(LMT_FORWARD, ("Sender Thread for '%s' running - wFd: %d (reading from fd %d)", ep->name.c_str(), ep->wFd, ep->senderReadFd));

	while (1)
	{
		SendJob job;
		int     s;

		s = iomMsgAwait(ep->senderReadFd, -1);
		LM_T(LMT_FORWARD, ("Got something to read on fd %d", ep->senderReadFd));
		s = read(ep->senderReadFd, &job, sizeof(job));
		LM_T(LMT_FORWARD, ("read %d bytes from fd %d", s, ep->senderReadFd));
		if (s != sizeof(job))
		{
			LM_E(("bad size read (%d - expected %d)", s, sizeof(job)));
			if (s == -1)
				LM_P(("read"));
			else
				LM_E(("read error (%d returned)", s));
			continue;
		}

		LM_T(LMT_FORWARD, ("Received and read '%s' job from '%s' father (fd %d) - the job is forwarded to fd %d (packetP at %p)",
				   messageCode(job.msgCode),
				   ep->name.c_str(),
				   ep->senderReadFd,
				   ep->wFd,
				   job.packetP));
		LM_T(LMT_FORWARD, ("google protocol 'message': %p (packet at %p)", &job.packetP->message, job.packetP));
		LM_T(LMT_FORWARD, ("google protocol buffer data len: %d", job.packetP->message.ByteSize()));

		s = iomMsgSend(ep, NULL, job.msgCode, job.msgType, job.dataP, job.dataLen, job.packetP);

		LM_T(LMT_FORWARD, ("iomMsgSend returned %d", s));
		if (s != 0)
		{
			LM_E(("iomMsgSend error"));
			if (ep->packetSender)
				ep->packetSender->notificationSent(-1, false);
		}
		else
		{
			LM_T(LMT_FORWARD, ("iomMsgSend OK"));
			if (ep->packetSender)
				ep->packetSender->notificationSent(0, true);
		}

		LM_T(LMT_FORWARD, ("iomMsgSend ok"));
		if (job.dataP)
		{
			LM_T(LMT_FORWARD, ("before freeing job data pointer (%p - %p)", job.dataP, job.packetP));
			free(job.dataP);
			LM_T(LMT_FORWARD, ("after freeing job data pointer"));
		}

		if (job.packetP)
			delete job.packetP;
	}

	// Cannot really get here ... !!!

	LM_E(("******************* after while TRUE ..."));

	close(ep->senderReadFd);
	close(ep->senderWriteFd);
	ep->sender = false;

	return NULL;
}



/* ****************************************************************************
*
* send - 
*/
size_t Network::send(PacketSenderInterface* packetSender, int endpointId, ss::Message::MessageCode code, Packet* packetP)
{
	Endpoint* ep        = endpoint[endpointId];
	int       nb;

	
	if (packetP != NULL)
		LM_T(LMT_SEND, ("Request to send '%s' package with %d packet size (to endpoint '%s')", messageCode(code), packetP->message.ByteSize(), ep->name.c_str()));
	else
		LM_T(LMT_SEND, ("Request to send '%s' package without data (to endpoint '%s')", messageCode(code), ep->name.c_str()));

	if (ep == NULL)
		LM_X(1, ("No endpoint at index %d", endpointId));

	if (packetP != NULL)
	{
		if (packetP->message.ByteSize() == 0)
			LM_W(("packet not NULL but its data len is 0 ..."));
	}

	if (ep->state != Endpoint::Connected)
	{
		SendJob* jobP = new SendJob();

		if (ep->useSenderThread == false)
			LM_X(1, ("cannot send to an unconnected peer '%s' if not using sender threads, sorry ...", ep->name.c_str()));

		jobP->ep      = ep;
		jobP->me      = me;
		jobP->msgCode = code;
		jobP->msgType = Message::Msg;
		jobP->dataP   = NULL;
		jobP->dataLen = 0;
		jobP->packetP = packetP;

		LM_T(LMT_JOB, ("pushing a job for endpoint '%s'", ep->name.c_str()));
		ep->jobPush(jobP);

		return 0;
	}

	ep->packetSender  = packetSender;

	LM_T(LMT_DELILAH, ("sending a '%s' message to endpoint %d", messageCode(code), endpointId));
	if (ep->useSenderThread == true)
	{
		if (ep->sender == false)
		{
			LM_T(LMT_FORWARD, ("Creating a new sender thread for endpoint '%s'", ep->name.c_str()));

			int tunnelPipe[2];

			if (pipe(tunnelPipe) == -1)
				LM_X(1, ("pipe: %s", strerror(errno)));

			ep->senderWriteFd = tunnelPipe[1];  // father writes to this fd
			ep->senderReadFd  = tunnelPipe[0];  // child reads from this fd
			ep->sender        = true;

			LM_T(LMT_FORWARD, ("msgs for endpoint '%s' to fd %d instead of fd %d", ep->name.c_str(), ep->senderWriteFd, ep->wFd));
			LM_T(LMT_FORWARD, ("'%s' sender thread to read from fd %d and send to fd %d", ep->name.c_str(), ep->senderReadFd, ep->wFd));
			//
			// Create sender thread
			//
			pthread_create(&ep->senderTid, NULL, senderThread, ep);
			usleep(1000);


			//
			// Flush job queue on sender pipe
			//
			SendJob* jobP;

			LM_T(LMT_JOB, ("sender thread created - flushing job queue"));
			while ((jobP = ep->jobPop()) != NULL)
			{
				LM_T(LMT_JOB, ("sending a queued job to job-sender"));
				nb = write(ep->senderWriteFd, jobP, sizeof(SendJob));
				if (nb == -1)
					LM_P(("write(SendJob)"));
				else if (nb != sizeof(SendJob))
					LM_E(("error writing SendJob. Written %d bytes and not %d", nb, sizeof(SendJob)));
				
				free(jobP);
			}

			LM_T(LMT_JOB, ("sender thread created - job queue flushed"));
		}

		SendJob job;

		job.ep      = ep;
		job.me      = me;
		job.msgCode = code;
		job.msgType = Message::Msg;
		job.dataP   = NULL;
		job.dataLen = 0;
		job.packetP = packetP;

		LM_T(LMT_FORWARD, ("Sending '%s' job to '%s' sender (real destiny fd: %d) with %d packet size - the job is tunneled over fd %d (packet pointer: %p)",
						   messageCode(job.msgCode), ep->name.c_str(), ep->wFd, job.packetP->message.ByteSize(), ep->senderWriteFd, job.packetP));
		
		nb = write(ep->senderWriteFd, &job, sizeof(job));
		if (nb != (sizeof(job)))
		{
			LM_E(("write(written only %d bytes (of %d) to sender thread)", nb, sizeof(job)));
			return -1;
		}

		return 0;
	}

	LM_T(LMT_FORWARD, ("Sending message directly (%d bytes)", packetP->message.ByteSize()));
	nb = iomMsgSend(ep, me, code, Message::Msg, NULL, 0, packetP);
	if (packetP != NULL)
		delete packetP;

	return nb;
}



/* ****************************************************************************
*
* ready - 
*/
bool Network::ready()
{
	return iAmReady;
}



/* ****************************************************************************
*
* endpointAdd - add an endpoint to the vector
*
* The first three slots in this vector are:
*  - 0: me
*  - 1: listener
*  - 2: controller
*
* From slot 3, the worker endpoints are stored. These endpoints have a fix
* position in the vector and these positions are defined by the response
* of the WorkerVector from thge controller. The controller itself gets the
* positions from the workers in the configuration file 'setup.txt'.
* The worker positions in the endpoint vector may never change.
*
* After the workers, we have instances of delilah, which are considered
* temporal, i.e. they're removed and forgotten if a delilah is disconnected.
*
* The method 'endpointAdd' is called when a Hello is received, as we don't really
* know who is on the other side until we receive the Hello package, so at the
* end of this vector, temporal endpoints are stored, these endpoints have not
* yet received the Hello package. These temporal endpoints grow downwards in
* the vector.
*
*/
Endpoint* Network::endpointAdd
(
	int              rFd,
	int              wFd,
	const char*      name,
	const char*      alias,
	int              workers,
	Endpoint::Type   type,
	std::string      ip,
	unsigned short   port,
	int              coreNo,
	Endpoint*        inheritedFrom
)
{
	int ix;

	LM_T(LMT_ENDPOINT, ("Adding endpoint '%s' of type '%s' for fd %d", name, me->typeName(type), rFd));

	switch (type)
	{
	case Endpoint::Sender:
	case Endpoint::CoreWorker:
	case Endpoint::Unknown:
	case Endpoint::Listener:
		LM_X(1, ("bad type: %d", type));
		return NULL;

	case Endpoint::Controller:
		if (inheritedFrom != NULL)
		{
			endpoint[2]->msgsIn         = inheritedFrom->msgsIn;
			endpoint[2]->msgsOut        = inheritedFrom->msgsOut;
			endpoint[2]->msgsInErrors   = inheritedFrom->msgsInErrors;
			endpoint[2]->msgsOutErrors  = inheritedFrom->msgsOutErrors;
		}

		endpoint[2]->rFd      = rFd;
		endpoint[2]->wFd      = wFd;
		endpoint[2]->name     = std::string(name);
		endpoint[2]->alias    = (alias != NULL)? alias : "NO ALIAS" ;
		endpoint[2]->workers  = workers;
		endpoint[2]->type     = type;
		endpoint[2]->port     = port;
		endpoint[2]->coreNo   = coreNo;

		if (strcmp(ip.c_str(), "II.PP") != 0)
			endpoint[2]->ip       = ip;

		if (me->type == Endpoint::Delilah)
		{
			endpoint[2]->useSenderThread = true;
			LM_T(LMT_FORWARD, ("Delilah controller endpoint uses SenderThread"));
		}

		return controller;

	case Endpoint::Temporal:
		for (ix = Endpoints - 1; ix >= 3 + Workers; ix--)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating temporal Endpoint"));

				if (inheritedFrom != NULL)
				{
					endpoint[ix]->msgsIn         = inheritedFrom->msgsIn;
					endpoint[ix]->msgsOut        = inheritedFrom->msgsOut;
					endpoint[ix]->msgsInErrors   = inheritedFrom->msgsInErrors;
					endpoint[ix]->msgsOutErrors  = inheritedFrom->msgsOutErrors;
				}

				endpoint[ix]->name   = std::string(name);
				endpoint[ix]->rFd    = rFd;
				endpoint[ix]->wFd    = wFd;
				endpoint[ix]->type   = Endpoint::Temporal;
				endpoint[ix]->ip     = ip;
				endpoint[ix]->alias  = (alias != NULL)? alias : "NO ALIAS" ;
				endpoint[ix]->state  = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No temporal endpoint slots available - redefine and recompile!"));
		break;

	case Endpoint::Delilah:
	case Endpoint::WebListener:
	case Endpoint::WebWorker:
		for (ix = 3 + Workers; ix < (int) (Endpoints - 1); ix++)
		{
			if (endpoint[ix] == NULL)
			{
				endpoint[ix] = new Endpoint();
				if (endpoint[ix] == NULL)
					LM_XP(1, ("allocating Endpoint"));

				if (inheritedFrom != NULL)
				{
					endpoint[ix]->msgsIn         = inheritedFrom->msgsIn;
					endpoint[ix]->msgsOut        = inheritedFrom->msgsOut;
					endpoint[ix]->msgsInErrors   = inheritedFrom->msgsInErrors;
					endpoint[ix]->msgsOutErrors  = inheritedFrom->msgsOutErrors;
					endpoint[ix]->ip             = inheritedFrom->ip;
				}

				endpoint[ix]->name       = std::string(name);
				endpoint[ix]->alias      = (alias != NULL)? alias : "NO ALIAS" ;
				endpoint[ix]->rFd        = rFd;
				endpoint[ix]->wFd        = wFd;
				endpoint[ix]->state      = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;   /* XXX */
				endpoint[ix]->type       = type;
				endpoint[ix]->port       = port;
				endpoint[ix]->coreNo     = coreNo;
				endpoint[ix]->restarts   = 0;
				endpoint[ix]->jobsDone   = 0;
				endpoint[ix]->startTime  = time(NULL);

				if (endpoint[ix]->type == Endpoint::WebListener)
					endpoint[ix]->state = Endpoint::Listening;

				if (strcmp(ip.c_str(), "II.PP") != 0)
					endpoint[ix]->ip       = ip;

				return endpoint[ix];
			}
		}

		if (endpoint[ix] == NULL)
			LM_X(1, ("No endpoint slots available - redefine and recompile!"));
		LM_X(1, ("ERROR ?"));
		break;

	case Endpoint::Worker:
		if (me->type == Endpoint::CoreWorker)
		{
			if (controller == NULL)
				LM_X(1, ("controller == NULL"));

			controller->rFd    = rFd;
			controller->wFd    = wFd;
			controller->name   = std::string(name);
			controller->alias  = (alias != NULL)? alias : "NO ALIAS" ;
			
			return controller;
		}

		for (ix = 3; ix < 3 + Workers; ix++)
		{
			Endpoint* ep;

			if (endpoint[ix] == NULL)
				LM_X(1, ("NULL worker endpoint at slot %d", ix));

			if ((ep = endpointLookup((char*) alias)) != NULL)
			{
				if (ep->wFd != wFd)
					LM_E(("write file descriptors don't coincide for endpoint '%s' (%d vs %d)", alias, wFd, ep->wFd));

				LM_E(("Intent to connect a second Worker with alias '%s' - rejecting connection", alias));
				iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);

				close(rFd);
				if (wFd != rFd)
					close(wFd);
				return NULL;
			}

			if (me->type == Endpoint::Delilah)
			{
				endpoint[ix]->useSenderThread = true;
				LM_T(LMT_FORWARD, ("Delilah worker endpoint uses SenderThread"));
			}

			if (strcmp(endpoint[ix]->alias.c_str(), alias) == 0)
			{
				if (inheritedFrom != NULL)
				{
					endpoint[ix]->msgsIn         = inheritedFrom->msgsIn;
					endpoint[ix]->msgsOut        = inheritedFrom->msgsOut;
					endpoint[ix]->msgsInErrors   = inheritedFrom->msgsInErrors;
					endpoint[ix]->msgsOutErrors  = inheritedFrom->msgsOutErrors;
					endpoint[ix]->ip             = inheritedFrom->ip;
				}

				endpoint[ix]->rFd      = rFd;
				endpoint[ix]->wFd      = wFd;
				endpoint[ix]->name     = std::string(name);
				endpoint[ix]->alias    = std::string(alias);
				endpoint[ix]->workers  = workers;
				endpoint[ix]->type     = Endpoint::Worker;
				endpoint[ix]->port     = port;
				endpoint[ix]->state    = (rFd > 0)? Endpoint::Connected : Endpoint::Unconnected;   /* XXX */

				if (strcmp(ip.c_str(), "II.PP") != 0)
					endpoint[ix]->ip       = ip;

				LM_T(LMT_JOB, ("worker '%s' connected - any pending messages for him? (jobQueueHead at %p)", endpoint[ix]->alias.c_str(),  endpoint[ix]->jobQueueHead));
				
				return endpoint[ix];
			}
		}

		LM_E(("Worker '%s:%d' not found!", ip.c_str(), port));

		LM_E(("alias '%s' - rejecting connection", alias));
		iomMsgSend(wFd, name, progName, Message::Die, Message::Evt);

		return NULL;
	}

	LM_X(1, ("BUG"));
	return NULL;
}



/* ****************************************************************************
*
* endpointRemove
*/
void Network::endpointRemove(Endpoint* ep)
{
	int ix;

	LM_T(LMT_EP, ("Removing '%s' endpoint '%s'", ep->typeName(), ep->name.c_str()));

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix] == ep)
		{
			if (ep->type == Endpoint::Worker)
			{
				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->state = Endpoint::Disconnected;
				ep->name  = std::string("To be a worker");
			}
			else if (ep->type == Endpoint::Controller)
			{
				LM_W(("NOT removing Controller"));
			}
			else
			{
				delete ep;
				endpoint[ix] = NULL;
			}
		}
	}
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(int ix)
{
	return endpoint[ix];
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(int rFd, int* idP)
{
	int ix = 0;

	if (rFd < 0)
		return NULL;

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if (endpoint[ix]->rFd == rFd)
		{
			if (idP)
				*idP = ix;
			return endpoint[ix];
		}
	}

	LM_E(("endpoint (rFd:%d) not found", rFd));
	return NULL;
}



/* ****************************************************************************
*
* endpointLookup
*/
Endpoint* Network::endpointLookup(char* alias)
{
	int ix = 0;

	for (ix = 0; ix < Endpoints; ix++)
	{
		if (endpoint[ix] == NULL)
			continue;

		if ((strcmp(endpoint[ix]->alias.c_str(), alias) == 0) && (endpoint[ix]->state == Endpoint::Connected))
		{
			return endpoint[ix];
		}
	}

	LM_E(("endpoint (alias:%s) not found", alias));
	return NULL;
}



/* ****************************************************************************
*
* MsgTreatParams - 
*/
typedef struct MsgTreatParams
{
	Network*          diss;
	Endpoint*         ep;
	int               endpointId;
	Message::Header   header;
	Endpoint::State   state;
} MsgTreatParams;



/* ****************************************************************************
*
* msgTreatThreadFunction - 
*/
static void* msgTreatThreadFunction(void* vP)
{
	MsgTreatParams*  paramP = (MsgTreatParams*) vP;
	Endpoint*        ep     = paramP->ep;

	LM_M(("Calling threaded msgTreat"));
	paramP->diss->msgTreat(paramP);

	ep->state = paramP->state;

	LM_E(("back after msgTreat - set state for '%s' to %d", ep->name.c_str(), ep->state));

	free(vP);
	return NULL;
}



/* ****************************************************************************
*
* msgPreTreat - 
*/
void Network::msgPreTreat(Endpoint* ep, int endpointId)
{
	Message::Header header;
	int             nb;

	LM_M(("ep->type: %d", ep->type));

	//
	// Special case - incoming connection on WebListener interface
	//
	if (ep->type == Endpoint::WebListener)
	{
		int   fd;
		char  hostName[128];

		fd = iomAccept(ep->rFd, hostName, sizeof(hostName));
		if (me->type != Endpoint::Controller)
		{
			LM_E(("got incoming WebListener connection but I'm not the controller ..."));
			if (fd != -1)
				close(fd);
			return;
		}

		if (fd == -1)
		{
			LM_P(("iomAccept(%d)", ep->rFd));
			ep->msgsInErrors += 1;
		}
		else
		{
			endpointAdd(fd, fd, "Web Worker", "Webworker", 0, Endpoint::WebWorker, hostName, 0);
			ep->msgsIn += 1;
		}

		return;
	}



	//
	// Special case - controller reads from Web Service connection
	//
	if (ep->type == Endpoint::WebWorker)
	{
		char buf[1024];
		int  nb;

		if (me->type != Endpoint::Controller)
			LM_X(1, ("Got a request from a WebWorker and I'm not a controller !"));

		nb = read(ep->rFd, buf, sizeof(buf));
		if (nb == -1)
		{
			ep->msgsInErrors += 1;
			LM_E(("error reading web service request: %s", strerror(errno)));
		}
		else if (nb == 0)
		{
			ep->msgsInErrors += 1;
			LM_E(("read ZERO bytes of web service request"));
		}
		else
		{
			std::string command     = receiver->getJSONStatus(std::string(buf));
			int         commandLen  = command.size();

			ep->msgsIn  += 1;
			ep->msgsOut += 1;
			write(ep->wFd, command.c_str(), commandLen);
		}

		close(ep->wFd);
		endpointRemove(ep);
		return;
	}



	//
	// Reading header of the message
	//
	nb = read(ep->rFd, &header, sizeof(header));
	if (nb == 0) /* Connection closed */
	{
		LM_T(LMT_SELECT, ("Connection closed - ep at %p", ep));
		ep->msgsInErrors += 1;

		if (ep->type == Endpoint::Worker)
		{
			LM_W(("Worker %d just died !", ep->workerId));
			receiver->notifyWorkerDied(ep->workerId);

			if (me->type == Endpoint::Delilah)
			{
				LM_T(LMT_TIMEOUT, ("Lower select timeout to ONE second to poll restarting worker"));
				tmoSecs = 1;
			}
		}

		if (ep == controller)
		{
			if (me->type == Endpoint::CoreWorker)
				LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

			LM_W(("controller died ... trying to reconnect !"));

			controller->rFd    = -1;
			controller->state = ss::Endpoint::Disconnected;

			while (controller->rFd == -1)
			{
				controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
				sleep(1); // sleep one second before reintenting connection to controller
			}

			controller->state = ss::Endpoint::Connected;
			controller->wFd   = controller->rFd;
		}
		else if (ep != NULL)
		{
			if (ep->type == Endpoint::Worker)
			{
				--me->workers;

				close(ep->rFd);
				if (ep->wFd == ep->rFd)
					close(ep->wFd);

				ep->state = ss::Endpoint::Closed;
				ep->rFd   = -1;
				ep->wFd   = -1;
				ep->name  = "-----";
			}
			else if (ep->type == Endpoint::CoreWorker)
				LM_X(1, ("should get no messages from core worker ..."));
			else
				endpointRemove(ep);
		}

		return;
	}
	else if (nb == -1)
		LM_RVE(("iomMsgRead: error reading message from '%s': %s", ep->name.c_str(), strerror(errno)));
	else if (nb != sizeof(header))
		LM_RVE(("iomMsgRead: error reading header from '%s' (read %d, wanted %d bytes", ep->name.c_str(), nb, sizeof(header)));

	LM_T(LMT_MSGTREAT, ("Read header of '%s' message with dataLens %d, %d, %d", messageCode(header.code), header.dataLen, header.gbufLen, header.kvDataLen));

	//
	// calling msgTreat ...
	//
	if (header.dataLen + header.gbufLen + header.kvDataLen > THREAD_BUF_SIZE_THRESHOLD)
	{
		pthread_t        tid;
		MsgTreatParams*  paramsP = (MsgTreatParams*) malloc(sizeof(MsgTreatParams));

		paramsP->diss        = this;
		paramsP->ep          = ep;
		paramsP->endpointId  = endpointId;
		paramsP->header      = header;
		paramsP->state       = ep->state;

		ep->state = Endpoint::Threaded;
		LM_T(LMT_MSGTREAT, ("setting state of '%s' to Threaded (%d)  old state: %d", ep->name.c_str(), ep->state, paramsP->state));

		LM_T(LMT_MSGTREAT, ("calling msgTreatThreadFunction via pthread_create (params at %p) (dataLens: %d, %d, %d)", paramsP, header.dataLen, header.gbufLen, header.kvDataLen));
		pthread_create(&tid, NULL, msgTreatThreadFunction, (void*) paramsP);
		LM_T(LMT_MSGTREAT, ("after pthread_create of msgTreatThreadFunction"));
	}
	else
	{
		MsgTreatParams  params;

		params.diss        = this;
		params.ep          = ep;
		params.endpointId  = endpointId;
		params.header      = header;

		LM_M(("calling msgTreat (endpointId: %d, type: %d", endpointId, ep->type));
		msgTreat(&params);
	}
}



/* ****************************************************************************
*
* checkAllWorkersConnected - 
*/
void Network::checkAllWorkersConnected(void)
{
	int ix;

	for (ix = 0; ix < Workers; ix++)
	{
		if (endpoint[3 + ix] == NULL)
			return;
		if (endpoint[3 + ix]->state != Endpoint::Connected)
			return;
	}	

	//
	// All workers are connected!
	//

	LM_F(("All workers are connected!"));
	if (me->type == Endpoint::Delilah)
	{
		LM_T(LMT_TIMEOUT, ("All workers are connected! - select timeout set to 60 secs"));
		tmoSecs = 60;
	}
}



/* ****************************************************************************
*
* controllerMsgTreat - 
*/
void Network::controllerMsgTreat
(
	Endpoint*             ep,
	Message::MessageCode  msgCode,
	Message::MessageType  msgType,
	void*                 dataP,
	int                   dataLen,
	Packet*               packetP
)
{
	const char* name = ep->name.c_str();

	LM_T(LMT_TREAT, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::WorkerVector:
		if (msgType != Message::Msg)
			LM_X(1, ("Controller got an ACK for WorkerVector message"));

		Message::Worker*  workerV;
		int               ix;

		workerV = (Message::Worker*) calloc(Workers, sizeof(Message::Worker));
		if (workerV == NULL)
			LM_XP(1, ("calloc(%d, %d)", Workers, sizeof(Endpoint)));

		for (ix = 3; ix < 3 + Workers; ix++)
		{
			if (endpoint[ix] != NULL)
			{
				strncpy(workerV[ix - 3].name,  endpoint[ix]->name.c_str(),  sizeof(workerV[ix - 3].name));
				strncpy(workerV[ix - 3].alias, endpoint[ix]->alias.c_str(), sizeof(workerV[ix - 3].alias));
				strncpy(workerV[ix - 3].ip,    endpoint[ix]->ip.c_str(),    sizeof(workerV[ix - 3].ip));

				workerV[ix - 3].port   = endpoint[ix]->port;
				workerV[ix - 3].state  = endpoint[ix]->state;
			}
		}

		LM_T(LMT_WRITE, ("sending ack with entire worker vector to '%s'", name));
		iomMsgSend(ep, me, Message::WorkerVector, Message::Ack, workerV, Workers * sizeof(Message::Worker));
		free(workerV);
		break;

	default:
		LM_X(1, ("message code '%s' not treated in this method", messageCode(msgCode)));
	}
}



/* ****************************************************************************
*
* msgTreat - 
*/
void Network::msgTreat(void* vP)
{
	char                  data[1024];
	void*                 dataP        = data;
	int                   dataLen      = sizeof(data);

	MsgTreatParams*       paramsP      = (MsgTreatParams*) vP;
	Endpoint*             ep           = paramsP->ep;
	int                   endpointId   = paramsP->endpointId;
	Message::Header*      headerP      = &paramsP->header;
	char*                 name         = (char*) ep->name.c_str();

	Packet                packet;
	Packet                ack;
	Message::MessageCode  msgCode;
	Message::MessageType  msgType;
	int                   s;


	LM_M(("treating incoming message from '%s' (ep at %p) (dataLens: %d, %d, %d)", name, ep, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));
	s = iomMsgRead(ep, headerP, &msgCode, &msgType, &dataP, &dataLen, &packet, NULL, 0);
	LM_T(LMT_READ, ("iomMsgRead returned %d (dataLens: %d, %d, %d)", s, headerP->dataLen, headerP->gbufLen, headerP->kvDataLen));

	if (s != 0)
	{
		LM_T(LMT_SELECT, ("iomMsgRead returned %d", s));

		if (s == -2) /* Connection closed */
		{
			if (ep->type == Endpoint::Worker)
			{
				LM_W(("Worker %d just died !", ep->workerId));
				receiver->notifyWorkerDied(ep->workerId);

				if (me->type == Endpoint::Delilah)
				{
					LM_T(LMT_TIMEOUT, ("Lower select timeout to ONE second to poll restarting worker"));
					tmoSecs = 1;
				}
			}

			LM_T(LMT_SELECT, ("Connection closed - ep at %p", ep));
			if (ep == controller)
			{
				if (me->type == Endpoint::CoreWorker)
					LM_X(1, ("My Father (samsonWorker) died, I cannot exist without father process"));

				LM_W(("controller died ... trying to reconnect !"));

				controller->rFd    = -1;
				controller->state = ss::Endpoint::Disconnected;

				while (controller->rFd == -1)
				{
					controller->rFd = iomConnect((const char*) controller->ip.c_str(), (unsigned short) controller->port);
					sleep(1); // sleep one second before reintenting connection to controller
				}

				controller->state = ss::Endpoint::Connected;
				controller->wFd   = controller->rFd;
				return;
			}
			else if (ep != NULL)
			{
				if (ep->type == Endpoint::Worker)
				{
					--me->workers;

					close(ep->rFd);
					if (ep->wFd == ep->rFd)
						close(ep->wFd);

					ep->state = ss::Endpoint::Closed;
					ep->rFd   = -1;
					ep->wFd   = -1;
					ep->name  = "-----";
				}
				else if (ep->type == Endpoint::CoreWorker)
					LM_X(1, ("should get no messages from core worker ..."));
				else
					endpointRemove(ep);
			}
		}

		LM_RVE(("iomMsgRead: error reading message from '%s'", name));
	}


	LM_T(LMT_TREAT, ("Treating %s %s from %s", messageCode(msgCode), messageType(msgType), name));
	switch (msgCode)
	{
	case Message::Die:
		LM_X(1, ("Got a DIE message from '%s' - I die", name));
		break;

	case Message::Hello:
		Endpoint*            helloEp;
		Message::HelloData*  hello;

		hello   = (Message::HelloData*) dataP;
		LM_M(("Got a Hello from a type %d endpoint", hello->type));
		helloEp = endpointAdd(ep->rFd, ep->wFd, hello->name, hello->alias, hello->workers, (ss::Endpoint::Type) hello->type, hello->ip, hello->port, hello->coreNo, ep);
	
		if (helloEp == NULL)
		{
			endpointRemove(ep);
			return;
		}
		helloEp->workerId = hello->workerId;

		LM_T(LMT_HELLO, ("Got Hello %s from %s, type %s, %s:%d, workers: %d",
						 messageType(msgType), helloEp->name.c_str(), helloEp->typeName(), helloEp->ip.c_str(), helloEp->port, helloEp->workers));

		if (ep && ep->type == Endpoint::Temporal)
			endpointRemove(ep);

		if (msgType == Message::Msg)
		{
			LM_T(LMT_JOB, ("Acking HELLO"));
			helloSend(helloEp, Message::Ack);
		}
		else
			LM_T(LMT_JOB, ("HELLO was an ACK"));

		if (helloEp->jobQueueHead != NULL)
		{
			SendJob*  jobP;
			int       ix  = 1;

			LM_T(LMT_JOB, ("Sending pending JOBs to '%s' (job queue head at %p)", helloEp->name.c_str(), helloEp->jobQueueHead));
			while ((jobP = helloEp->jobPop()) != NULL)
			{
				int nb;

				LM_T(LMT_JOB, ("Sending pending JOB %d (job at %p)", ix, jobP));
				nb = write(helloEp->senderWriteFd, jobP, sizeof(SendJob));
				if (nb != sizeof(SendJob))
				{
					if (nb == -1)
						LM_P(("write"));
					else
						LM_E(("written only %d bytes, instead of %d", nb, sizeof(SendJob)));
				}
				++ix;
			}
		}

		if (ep == controller)
		{
			if ((me->type != Endpoint::CoreWorker) && (me->type != Endpoint::Controller))
				iomMsgSend(controller, me, Message::WorkerVector, Message::Msg, NULL, 0, NULL);
		}

		checkAllWorkersConnected();
		break;

	case Message::WorkerVector:
		if ((msgType == Message::Msg) && (me->type != Endpoint::Controller))
			LM_X(1, ("Got a WorkerVector request from '%s' but I'm not the controller ...", name));

		if (me->type == Endpoint::Controller)
			controllerMsgTreat(ep, msgCode, msgType, dataP, dataLen, &packet);
		else
		{
			if (msgType != Message::Ack)
				LM_X(1, ("Got a WorkerVector request, not being controller ..."));

			if (ep->type != Endpoint::Controller)
				LM_X(1, ("Got a WorkerVector ack NOT from Controller ... (endpoint type: %d)", ep->type));

			LM_T(LMT_ENDPOINT, ("Got the worker vector from the Controller - now connect to them all ..."));

			unsigned int        ix;
			Message::Worker*    workerV = (Message::Worker*) dataP;

			if ((unsigned int) Workers != dataLen / sizeof(Message::Worker))
				LM_X(1, ("Got %d workers from Controller - I thought there were %d workers", dataLen / sizeof(Message::Worker), Workers));

			for (ix = 0; ix < dataLen / sizeof(Message::Worker); ix++)
			{
				Endpoint* epP;

				if (endpoint[3 + ix] == NULL)
				{
					endpoint[3 + ix] = new Endpoint(Endpoint::Worker, workerV[ix].name, workerV[ix].ip, workerV[ix].port, -1, -1);
					endpoint[3 + ix]->state = Endpoint::Unconnected;
					endpoint[3 + ix]->alias = workerV[ix].alias;
				}

				epP = endpoint[3 + ix];

				if (strcmp(epP->alias.c_str(), me->alias.c_str()) == 0)
				{
					LM_T(LMT_WORKERS, ("NOT connecting to myself ..."));
					epP->name = std::string("me: ") + epP->ip;
					continue;
				}

				if ((epP->rFd == -1) && (epP->port != 0))
				{
					int workerFd;

					LM_T(LMT_WORKERS, ("Connect to worker %d: %s (host %s, port %d, alias '%s')",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port, epP->alias.c_str()));

					if ((workerFd = iomConnect(epP->ip.c_str(), epP->port)) == -1)
					{
						if (me->type == Endpoint::Delilah)
						{
							LM_X(1, ("Unable to connect to worker %d (%s) - delilah must connect to all workers", ix, workerV[ix].name));
						}

						LM_T(LMT_WORKERS, ("worker %d: %s (host %s, port %d) not there - no problem, he'll connect to me",
									   ix, epP->name.c_str(), epP->ip.c_str(), epP->port));
					}
					else
					{
						Endpoint* ep;

						ep = endpointAdd(workerFd, workerFd, (char*) "to be worker", NULL, 0, Endpoint::Temporal, epP->ip, epP->port);
						if (ep != NULL)
						{
							ep->state = Endpoint::Connected;
							LM_T(LMT_ENDPOINT, ("Added ep with state '%s'", ep->stateName()));
						}
						else
							LM_X(1, ("endpointAdd failed"));
					}
				}
				else if ((me->type == Endpoint::Delilah) && (epP->port == 0))
				{
					LM_X(1, ("Unable to connect to worker %d (%s) - delilah must connect to all workers", ix, workerV[ix].name));
				}
			}
		}
		break;

	case Message::WorkerStatus:
		if ((me->type != Endpoint::Controller) && (msgType == Message::Msg))
			LM_X(1, ("Non-controller got a WorkerStatus message"));

		if (ep->status == NULL)
		{
			ep->status = (Message::WorkerStatusData*) malloc(sizeof(Message::WorkerStatusData));
			if (ep->status == NULL)
				LM_XP(1, ("malloc(WorkerStatusData"));
			memcpy(ep->status, dataP, sizeof(Message::WorkerStatusData));
			LM_T(LMT_STAT, ("endpoint '%s' has %d cores", ep->name.c_str(), ep->status->cpuInfo.cores));
		}
		break;

	case Message::Alarm:
		if (me->type == Endpoint::Worker)
		{
			if (ep->type != Endpoint::CoreWorker)
				LM_E(("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
			else
			{
				// Forward Alarm to controller
				iomMsgSend(controller, me, Message::Alarm, Message::Evt, dataP, dataLen);
			}
		}
		else if (me->type == Endpoint::Controller)
		{
			Alarm::AlarmData* alarmP = (Alarm::AlarmData*) dataP;

			LM_F(("Alarm from '%s': '%s'", ep->name.c_str(), alarmP->message));
			alarmSave(ep, alarmP);
		}
		else
			LM_X(1, ("Got an alarm event from %s endpoint '%s' - not supposed to happen!", ep->typeName(), ep->name.c_str()));
		break;

	default:
		if (receiver == NULL)
			LM_X(1, ("no packet receiver and unknown message type: %d", msgType));
		
		// LM_T(LMT_FORWARD, ("forwarding '%s' %s from %s to Endpoint %d", messageCode(msgCode), messageType(msgType), ep->name.c_str(), endpointId));
		LM_T(LMT_FORWARD, ("calling receiver->receive for message code '%s'", messageCode(msgCode)));
		receiver->receive(endpointId, msgCode, &packet);
		break;
	}

	if (dataP != data)
		free(dataP);
}



#define PeriodForSendingWorkerStatusToController  10
/* ****************************************************************************
*
* run
*/
void Network::run()
{
	int             fds;
	fd_set          rFds;
	struct timeval  timeVal;
	time_t          now   = 0;
	time_t          then  = time(NULL);
	int             max;

	while (1)
	{
		int ix;

		do
		{
			if (me->type == Endpoint::Worker)
			{
				now = time(NULL);
				if (now - then > PeriodForSendingWorkerStatusToController)
				{
					// workerStatusToController();
					then = now;
				}
			}

			LM_T(LMT_TIMEOUT, ("using a timeout of %d seconds", this->tmoSecs));
			timeVal.tv_sec  =  this->tmoSecs;
			timeVal.tv_usec =  this->tmoUsecs;

			FD_ZERO(&rFds);
			max = 0;

			if (me->type == Endpoint::Delilah)  /* reconnect to dead workers */
			{
				for (ix = 3; ix < 3 + Workers; ix++)
				{
					int workerFd;

					if (endpoint[ix] == NULL)
						continue;

					if (endpoint[ix]->type != Endpoint::Worker)
						continue;

					if (endpoint[ix]->state == Endpoint::Closed)
					{
						LM_T(LMT_RECONNECT, ("delilah reconnecting to %s:%d (type: '%s', state: '%s')",
											 endpoint[ix]->ip.c_str(), endpoint[ix]->port, endpoint[ix]->typeName(), endpoint[ix]->stateName()));
						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							endpointAdd(workerFd, workerFd, (char*) "Reconnecting worker", NULL, 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
							endpoint[ix]->state = Endpoint::Reconnecting;
						}
					}
					else if (endpoint[ix]->state == Endpoint::Disconnected)
					{
						LM_T(LMT_ENDPOINT, ("Connect to %s:%d ?", endpoint[ix]->ip.c_str(), endpoint[ix]->port));

						workerFd = iomConnect(endpoint[ix]->ip.c_str(), endpoint[ix]->port);
						if (workerFd != -1)
						{
							Endpoint* ep;

							ep = endpointAdd(workerFd, workerFd, (char*) "New Worker", NULL, 0, Endpoint::Temporal, endpoint[ix]->ip.c_str(), endpoint[ix]->port);
							if (ep != NULL)
								LM_T(LMT_ENDPOINT, ("Added ep with state '%s'", ep->stateName()));
							else
								LM_X(1, ("endpointAdd failed"));

							endpoint[ix]->state = Endpoint::Connected;
						}
					}
				}
			}


			//
			// Adding fds to the read-set
			//
			LM_F((""));
			LM_F(("------------ %d secs timeout, %d endpoints -------------------------------------", tmoSecs, Endpoints));
			for (ix = 0; ix < Endpoints; ix++)
			{
				if (endpoint[ix] == NULL)
					continue;

				if ((endpoint[ix]->state == Endpoint::Connected || endpoint[ix]->state == Endpoint::Listening) && (endpoint[ix]->rFd >= 0))
				{
					FD_SET(endpoint[ix]->rFd, &rFds);
					max = MAX(max, endpoint[ix]->rFd);

					LM_F(("+ %02d: %-15s %-20s %-12s %15s:%05d %16s  fd: %02d  (in: %03d, out: %03d)",
						  ix,
						  endpoint[ix]->typeName(),
						  endpoint[ix]->name.c_str(),
						  endpoint[ix]->alias.c_str(),
						  endpoint[ix]->ip.c_str(),
						  endpoint[ix]->port,
						  endpoint[ix]->stateName(),
						  endpoint[ix]->rFd,
						  endpoint[ix]->msgsIn,
						  endpoint[ix]->msgsOut));
				}
				else
				{
					LM_F(("- %02d: %-15s %-20s %-12s %15s:%05d %16s  fd: %02d  (in: %03d, out: %03d)",
						  ix,
						  endpoint[ix]->typeName(),
						  endpoint[ix]->name.c_str(),
						  endpoint[ix]->alias.c_str(),
						  endpoint[ix]->ip.c_str(),
						  endpoint[ix]->port,
						  endpoint[ix]->stateName(),
						  endpoint[ix]->rFd,
						  endpoint[ix]->msgsIn,
						  endpoint[ix]->msgsOut));
						  
				}
			}

			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		LM_T(LMT_SELECT, ("select returned %d", fds));
		if (fds == -1)
		{
			if (errno != EINTR)
				LM_XP(1, ("select"));
		}
		else if (fds == 0)
		{
			LM_D(("Timeout in network event loop"));
		}
		else
		{
			int ix;

			if (listener && (listener->state == Endpoint::Listening) && FD_ISSET(listener->rFd, &rFds))
			{
				int  fd;
				char hostName[128];

				LM_T(LMT_SELECT, ("incoming message from my listener - I will accept ..."));
				--fds;
				fd = iomAccept(listener->rFd, hostName, sizeof(hostName));
				if (fd == -1)
				{
					LM_P(("iomAccept(%d)", listener->rFd));
					listener->msgsInErrors += 1;
				}
				else
				{
					std::string  s   = std::string("tmp:") + std::string(hostName);
					Endpoint*    ep  = endpointAdd(fd, fd, (char*) s.c_str(), NULL, 0, Endpoint::Temporal, (char*) "ip", 0);

					listener->msgsIn += 1;
					helloSend(ep, Message::Msg);
				}
			}

			if (fds > 0)
			{
				// Treat endpoint for endpoint vector - skipping the first two ... (me & listener)
				// For now, only temporal endpoints are in endpoint vector
				LM_T(LMT_SELECT, ("looping from %d to %d", 3, Endpoints));
				for (ix = 2; ix < Endpoints; ix++)
				{
					if ((endpoint[ix] == NULL) || (endpoint[ix]->rFd < 0))
						continue;

					if (FD_ISSET(endpoint[ix]->rFd, &rFds))
					{
						--fds;
						LM_T(LMT_SELECT, ("incoming message from '%s' endpoint %s (fd %d)", endpoint[ix]->typeName(), endpoint[ix]->name.c_str(), endpoint[ix]->rFd));
						msgPreTreat(endpoint[ix], ix);
						if (endpoint[ix])
							FD_CLR(endpoint[ix]->rFd, &rFds);
					}
				}
			}
		}
	}
}



/* ****************************************************************************
*
* quit - 
*/
void Network::quit()
{
}	

}
