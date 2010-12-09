/* ****************************************************************************
*
* FILE                      Endpoint.cpp
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string.h>             // strchr
#include <stdlib.h>             // atoi
#include <unistd.h>             // gethostname

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*, ...

#include "Endpoint.h"           // Own interface



namespace ss {



/* ****************************************************************************
*
* init - 
*/
void Endpoint::init(void)
{
	this->name             = "no name";
	this->ip               = "localhost";
	this->port             = 0xFFFF;
	this->rFd              = -1;
	this->wFd              = -1;
	this->state            = Unconnected;
	this->workers          = 0;
	this->type             = Unknown;
	this->status           = NULL;
	this->sender           = false;
	this->useSenderThread  = false;
	this->jobQueueHead     = NULL;
	this->msgsIn           = 0;
	this->msgsOut          = 0;
	this->msgsInErrors     = 0;
	this->msgsOutErrors    = 0;
	this->bytesIn          = 0;
	this->bytesOut         = 0;

	hostnameGet();
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(void)
{
	init();
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, unsigned short port)
{
	init();

	this->type             = type;
	this->port             = port;
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, char* alias)
{
	init();

	this->type             = type;
	this->alias            = alias;
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string ipAndPort)
{
	char* port = strchr((char*) ipAndPort.c_str(), ':');

	init();

	if (port == NULL)
	{
		this->name  = ipAndPort;
		this->port  = atoi(ipAndPort.c_str());
		this->ip    = "127.0.0.1";
	}
	else
	{
		this->name  = ipAndPort;

		*port = 0;
		++port;
		this->ip    = ipAndPort;
		this->port  = atoi(port);
	}
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(Type type, std::string name, std::string ip, unsigned short port, int rFd, int wFd)
{
	init();

	this->type             = type;
	this->name             = name;
	this->ip               = ip;
	this->port             = port;
	this->rFd              = rFd;
	this->wFd              = wFd;
	this->state            = (rFd == -1)? Unconnected : Connected;
}



/* ****************************************************************************
*
* hostnameGet - 
*/
void Endpoint::hostnameGet(void)
{
	char hn[128];

	if (gethostname(hn, sizeof(hn)) == 0)
	{
		hostname = std::string(hn);
		LM_T(LMT_CONFIG, ("hostname: '%s'", hostname.c_str()));
	}
	else
		LM_P(("gethostname"));
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Endpoint::typeName(Endpoint::Type type)
{
	switch (type)
	{
	case Endpoint::Unknown:         return "Unknown";
	case Endpoint::Temporal:        return "Temporal";
	case Endpoint::Listener:        return "Listener";
	case Endpoint::Controller:      return "Controller";
	case Endpoint::Worker:          return "Worker";
	case Endpoint::CoreWorker:      return "CoreWorker";
	case Endpoint::Delilah:         return "Delilah";
	case Endpoint::WebListener:     return "WebListener";
	case Endpoint::WebWorker:       return "WebWorker";
	case Endpoint::Sender:          return "Sender";
	case Endpoint::Spawner:         return "Spawner";
	case Endpoint::Supervisor:      return "Supervisor";
	}

	return "UnknownType";
}



/* ****************************************************************************
*
* typeName - 
*/
const char* Endpoint::typeName(void)
{
   return typeName(this->type);
}



/* ****************************************************************************
*
* nam - 
*/
const char* Endpoint::nam(void)
{
	return name.c_str();
}



/* ****************************************************************************
*
* stateName - 
*/
const char* Endpoint::stateName(void)
{
	switch (state)
	{
	case Free:            return (char*) "Free";
	case Me:              return (char*) "ME";
	case FutureWorker:    return (char*) "FutureWorker";
	case Listening:       return (char*) "Listening";
	case Unconnected:     return (char*) "Unconnected";
	case Connected:       return (char*) "Connected";
	case Closed:          return (char*) "Closed";
	case Disconnected:    return (char*) "Disconnected";
	case Reconnecting:    return (char*) "Reconnecting";
	case Dead:            return (char*) "Dead";
	case Threaded:        return (char*) "Threaded";
	}

	return (char*) "Unknown";
}



/* ****************************************************************************
*
* reset - 
*/
void Endpoint::reset(void)
{
	if (rFd != -1)
		close(rFd);

	if ((wFd != rFd) && (wFd != -1))
		close(wFd);

	init();
}



/* ****************************************************************************
*
* jobPush - 
*/
void Endpoint::jobPush(SendJob* jobP)
{
	SendJobQueue* qP   = jobQueueHead;

	LM_T(LMT_JOB, ("Pushing a job"));

	if (jobQueueHead == NULL)
	{
		LM_T(LMT_JOB, ("Pushing first job"));
		jobQueueHead = new SendJobQueue;
		jobQueueHead->job   = jobP;
		jobQueueHead->next  = NULL;
		return;
	}

	while (qP->next != NULL)
		qP = qP->next;

	qP->next = new SendJobQueue;
	qP->next->job   = jobP;
	qP->next->next  = NULL;
}



/* ****************************************************************************
*
* jobPop - return last job in queue
*/
SendJob* Endpoint::jobPop(void)
{
	SendJobQueue*  prev = NULL;
	SendJobQueue*  qP   = jobQueueHead;
	SendJob*       jobP;

	LM_T(LMT_JOB, ("Popping a job?"));

	if (qP == NULL)
		return NULL;

	LM_T(LMT_JOB, ("Popping a job (qP == %p)", qP));
	while (qP->next != NULL)
	{
		prev = qP;
		qP   = qP->next;
	}

	jobP = qP->job;

	if (prev == NULL)  // Only one job in queue
		jobQueueHead = NULL;
	else
		prev->next = NULL;

	free(qP);
	return jobP;
}
}
