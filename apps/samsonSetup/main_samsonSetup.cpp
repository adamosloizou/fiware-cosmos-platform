/* ****************************************************************************
*
* FILE                     main_samsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdio.h>              // printf, ...
#include <sys/stat.h>           // struct stat
#include <unistd.h>             // stat
#include <fcntl.h>              // open, O_RDWR, O_CREAT, O_TRUNC, ...
#include <stdlib.h>             // free

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "samsonDirectories.h"  // SAMSON_IMAGES
#include "ports.h"              // WORKER_PORT
#include "Endpoint.h"           // ss::Endpoint
#include "Message.h"            // ss::Message
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgAwait.h"        // iomMsgAwait
#include "iomMsgRead.h"         // iomMsgRead
#include "platformProcesses.h"  // ss::platformProcessesGet, ss::platformProcessesSave
#include "Process.h"            // Process



/* ****************************************************************************
*
* Option variables
*/
int    workers;
char*  ip[100];
char   controllerHost[80];



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-ips",          ip,          "IP_LIST",    PaSList,   PaOpt,  PaND,   PaNL,  PaNL,  "listen port"         },
	{ "-controller",   controllerHost,  "CONTROLLER", PaString,  PaOpt,  PaND,   PaNL,  PaNL,  "Controller host"     },
	{ "-workers",     &workers,     "WORKERS",    PaInt,     PaOpt,     0,     0,   100,   "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int                 logFd                  = -1;
const char*         EtcDirPath             = SAMSON_ETC;
const char*         PlatformProcessesPath  = SAMSON_PLATFORM_PROCESSES;
const char*         ppFile                 = PlatformProcessesPath;
char*               spawnerIp              = NULL;
ss::ProcessVector*  procVec                = NULL;



/* ****************************************************************************
*
* accessCheck - 
*
* CASES
* 0. directory doesn't exist
* 1. No write permissions on directory
* 2. file doesn't exist and we have write permissions on directory
* 3. file exists 
*/
static int accessCheck(void)
{
	struct stat statBuf;

	if (stat(EtcDirPath, &statBuf) == -1)
	{
		if (errno == ENOENT)
		{
			printf("Samson Platform Setup Error.\n"
				   "The Samson Platform Directory '%s' doesn't exist - please create it and try again.\n", EtcDirPath);
			return 3;
		}
	}

	if (access(EtcDirPath, W_OK) == -1)
	{
		printf("Samson Platform Setup Error.\n"
			   "No permissions to create files under the samson platform directory '%s'.\n"
			   "Please fix the problem and try again.\n",
			   EtcDirPath);

		return 4;
	}

	if (access(PlatformProcessesPath, W_OK) == -1)
	{
		if (errno != ENOENT)
		{
			printf("Samson Platform Setup Error.\n"
				   "Cannot access the platform process file '%s' with write permissions: %s\n"
				   "Please fix the problem and try again.\n",
				   PlatformProcessesPath, strerror(errno));

			return 5;
		}
	}

	if (access(PlatformProcessesPath, R_OK) == 0)
	{
		char answer[32];

		printf("Samson Platform Setup Error.\n"
			   "The platform process file '%s' already exists.\n"
			   "Do you wish to overwrite it (Y/N)?> ", PlatformProcessesPath);
		fflush(stdout);
		scanf("%s", answer);
		if (((answer[0] == 'N') || (answer[0] == 'n')) && (answer[1] == 0))
			return 6;
		else if (((answer[0] == 'Y') || (answer[0] == 'y')) && (answer[1] == 0))
		{
			if (unlink(PlatformProcessesPath) == -1)
			{
				printf("Samson Platform Setup Error.\n"
					   "Unable to remove the Samson Platform Setup file '%s': %s\n",
					   PlatformProcessesPath, strerror(errno));

				return 7;
			}
		}
		else
		{
			printf("Samson Platform Setup Error.\n"
				   "Incorrect answer (%s).\n", answer);
			return 8;
		}
	}

	return 0;
}



/* ****************************************************************************
*
* platformFileCreate - 
*/
static int platformFileCreate(int workers, char* ip[])
{
	int                 s;
	int                 size;



	//
	// Checking access to the platform file
	//
	if ((s = accessCheck()) != 0)
		return s;



	//
	// Initializing variables for the worker vector
	//
	size     = sizeof(ss::ProcessVector) + (1 + workers) * sizeof(ss::Process);
	procVec  = (ss::ProcessVector*) malloc(size);

	memset(procVec, 0, size);

	procVec->processes = workers + 1;



	//
	// Filling the process vector buffer to be written to file
	//

	// 1. Controller
	strncpy(procVec->processV[0].name,   "samsonController",  sizeof(procVec->processV[0].name));
	strncpy(procVec->processV[0].host,   controllerHost,      sizeof(procVec->processV[0].host));
	strncpy(procVec->processV[0].alias,  "Controller",        sizeof(procVec->processV[0].alias));

	procVec->processV[0].port = CONTROLLER_PORT;
	
	// 2. Workers
	for (int ix = 1; ix < (long) ip[0] + 1; ix++)
	{
		strncpy(procVec->processV[ix].name,  "samsonWorker", sizeof(procVec->processV[ix].name));
		strncpy(procVec->processV[ix].host, ip[ix], sizeof(procVec->processV[ix].host));

		snprintf(procVec->processV[ix].alias, sizeof(procVec->processV[ix].alias), "Worker%02d", ix - 1);

		procVec->processV[ix].port = WORKER_PORT;
	}


	//
	// Saving the file to disk
	//
	ss::platformProcessesSave(procVec);

	return 0;
}



/* ****************************************************************************
*
* spawnerConnect - 
*/
static int spawnerConnect(int* errP)
{
	int fd;

	fd = iomConnect(controllerHost, SPAWNER_PORT);
	if (fd == -1)
	{
		printf("Samson Platform Setup Error.\n"
			   "Samson platform process 'spawner' in host '%s' doesn't respond.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

		*errP = 21;
	}

	return fd;
}



/* ****************************************************************************
*
* hello - 
*/
static int hello(ss::Endpoint* me, ss::Endpoint* spawner, int* errP)
{
	int                             s;
	ss::Message::Header             header;
	ss::Message::MessageCode        msgCode;
	ss::Message::MessageType        msgType;
	char                            data[1];
	void*                           dataP   = data;
	int                             dataLen = sizeof(data);
	ss::Message::HelloData          hello;

	*errP = 0;

	s = iomMsgAwait(spawner->rFd, 5, 0);
	if (s != 1)
	{
		printf("Samson Platform Setup Error.\n"
			   "Timeout awaiting response from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

		*errP = 31;
		return -1;
	}

	s = iomMsgPartRead(spawner, "hello header", (char*) &header, sizeof(header));
	if (s != sizeof(header))
	{
		LM_M(("iomMsgPartRead error %d", s));
		printf("Samson Platform Setup Error.\n"
			   "Error reading response header from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.\n",
			   controllerHost, controllerHost);

		*errP = 32;
		return -1;
	}

	s = iomMsgRead(spawner, &header, &msgCode, &msgType, &dataP, &dataLen);
	if (s != 0)
	{
		printf("Samson Platform Setup Error.\n"
			   "Error reading response from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.\n",
			   controllerHost, controllerHost);

		*errP = 33;
		return -1;
	}


	memset(&hello, 0, sizeof(hello));

	strncpy(hello.name,   "samsonSetup",   sizeof(hello.name));
	strncpy(hello.ip,     "myip",          sizeof(hello.ip));
	strncpy(hello.alias,  "samsonSetup",   sizeof(hello.alias));

	hello.type = ss::Endpoint::Setup;

	s = iomMsgSend(spawner, me, ss::Message::Hello, ss::Message::Ack, &hello, sizeof(hello));
	if (s != 0)
	{
		printf("Samson Platform Setup Error.\n"
			   "Error sending message to platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

		*errP = 34;
		return -1;
	}

	return 0;
}



/* ****************************************************************************
*
* procVecSend - 
*/
static int procVecSend(ss::Endpoint* me, ss::Endpoint* spawner, int* errP)
{
	int                         s;
	ss::Message::Header         header;
	ss::Message::MessageCode    msgCode;
	ss::Message::MessageType    msgType;
	int                         procVecSize = sizeof(ss::ProcessVector) + procVec->processes * sizeof(ss::Process);
	char                        data[1];
	void*                       dataP       = data;
	int                         dataLen     = sizeof(data);

	*errP = 0;

	s = iomMsgSend(spawner, me, ss::Message::ProcessVector, ss::Message::Msg, procVec, procVecSize);
	if (s != 0)
	{
		printf("Samson Platform Setup Error.\n"
			   "Error sending Samson Platform Process List to platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

		*errP = 41;
		return -1;
	}

	s = iomMsgAwait(spawner->rFd, 5, 0);
	if (s != 1)
	{
		printf("Samson Platform Setup Error.\n"
			   "Timeout awaiting response from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

 		*errP = 42;
		return -1;
	}

	LM_M(("Calling iomMsgPartRead (reading header)"));
	s = iomMsgPartRead(spawner, "header", (char*) &header, sizeof(header));
	LM_M(("iomMsgPartRead returned %d", s));
	if (s != sizeof(header))
	{
		LM_M(("iomMsgPartRead error %d", s));
		printf("Samson Platform Setup Error.\n"
			   "Error reading response header from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.\n",
			   controllerHost, controllerHost);

		*errP = 43;
		return -1;
	}


	s = iomMsgRead(spawner, &header, &msgCode, &msgType, &dataP, &dataLen);
	LM_M(("iomMsgRead returned %d", s));
	if (s != 0)
	{
		printf("Samson Platform Setup Error.\n"
			   "Error reading response from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.\n",
			   controllerHost, controllerHost);

		*errP = 44;
		return -1;
	}

	if ((msgCode != ss::Message::ProcessVector) || (msgType != ss::Message::Ack))
	{
		if (msgCode != ss::Message::ProcessVector)
			LM_E(("Bad message code: %d", msgCode));
		if (msgType != ss::Message::Ack)
			LM_E(("Bad message type: %d", msgType));

		printf("Samson Platform Setup Error.\n"
			   "Internal error flagged in response from Samson platform process 'spawner' in host '%s'.\n"
			   "Please check that the host '%s' is in order before you try again.",
			   controllerHost, controllerHost);

		*errP = 45;
		return -1;
	}

	return 0;
}



/* ****************************************************************************
*
* distribute - 
*/
static int distribute(void)
{
	int                         fd;
	ss::Endpoint                me;
	ss::Endpoint                spawner;
	int                         err      = 0;

	if ((fd = spawnerConnect(&err)) == -1)
		return err;

	me.name      = progName;
	spawner.name = "Spawner";
	spawner.rFd  = fd;
	spawner.wFd  = fd;

	
	if (hello(&me, &spawner, &err) == -1)
		return err;
	
	if (procVecSend(&me, &spawner, &err) == -1)
        return err;

	return 0;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	int err = 0;

	memset(controllerHost, 0, sizeof(controllerHost));

	paConfig("prefix",                        (void*) "SSP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (controllerHost[0] == 0)
	{
		printf("In what host will the controller run?> ");
		fflush(stdout);
		scanf("%s", controllerHost);
	}

	if (workers == 0)
	{
		printf("Number of workers> ");
		fflush(stdout);
		scanf("%d", &workers);

		ip[0] = (char*) workers;

		for (int ix = 0; ix < workers; ix++)
		{
			char ipaddress[64];

			printf("Please enter the IP address (or host name) for host number %d> ", ix + 1);
			fflush(stdout);
			scanf("%s", ipaddress);

			ip[ix + 1] = strdup(ipaddress);
		}
	}
	else
	{
		if (workers > (long) ip[0])
		{
			printf("Samson Platform Setup Error.\n"
				   "You specified %d workers with the '-workers' option, but gave only %d IP:s in the '-ip' option.\n"
				   "Please correct this error and try again.\n",
				   workers, (int) (long) ip[0]);
			err = 1;
		}
		else if (workers < (long) ip[0])
		{
			printf("Samson Platform Setup Error.\n"
				   "You specified %d workers with the '-workers' option, but gave %d IP:s in the '-ip' option.\n"
				   "Please correct this error and try again.\n",
				   workers, (int) (long) ip[0]);
			err = 2;
		}
	}

	
	if (err == 0)
	{
		err = platformFileCreate(workers, ip);

		if (err != 0)
			printf("Error creating platform file.\n");
	}

	if (err == 0)
	{
		err = distribute();

		if (err != 0)
			printf("Error distributing platform file.\n");
	}

	if (err != 0)
		printf("Operation terminated with failure %d.\n", err);

	return 0;
}
