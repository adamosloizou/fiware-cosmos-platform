/* ****************************************************************************
*
* FILE                     main_samsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdio.h>                 // printf, ...
#include <sys/stat.h>              // struct stat
#include <unistd.h>                // stat
#include <fcntl.h>                 // open, O_RDWR, O_CREAT, O_TRUNC, ...
#include <stdlib.h>                // free

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "engine/MemoryManager.h"
#include "engine/Engine.h"

#include "samson/common/samsonVersion.h"
#include "samson/common/status.h"
#include "samson/common/SamsonSetup.h"
#include "samson/network/Endpoint2.h"
#include "SamsonStarter.h"



/* ****************************************************************************
*
* Option variables
*/
char           controllerHost[256];
int            workers;
const char*    ips[100];
bool           reset;
bool           pList;
bool           local;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",   controllerHost,  "CONTROLLER", PaString,  PaOpt,  PaND,   PaNL,  PaNL,  "Controller host"               },
	{ "-workers",     &workers,         "WORKERS",    PaInt,     PaOpt,     0,     0,   100,   "number of workers"             },
	{ "-ips",          ips,             "IP_LIST",    PaSList,   PaOpt,  PaND,   PaNL,  PaNL,  "list of worker IPs"            },
	{ "-local",       &local,           "LOCAL",      PaBool,    PaOpt,  false, false,  true,  "'cluster' only in localhost"   },
	{ "-reset",       &reset,           "RESET",      PaBool,    PaOpt,  false, false,  true,  "reset platform"                },
	{ "-plist",       &pList,           "P_LIST",     PaBool,    PaOpt,  false, false,  true,  "process list of platform"      },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int              logFd        = -1;
SamsonStarter*   samsonStarter  = NULL;
int              startTime;



/* ****************************************************************************
*
* plist - 
*/
static void plist(void)
{
	samson::Status s;

	if ((s = samsonStarter->processList()) != samson::OK)
		LM_X(1, ("Error sending Process List Message to spawners"));

	LM_M(("Got the list - I'm done"));
	exit(0);
}



/* ****************************************************************************
*
* resetAndStart - 
*/
static void resetAndStart(void)
{
	samson::Status s;

	if ((s = samsonStarter->reset()) != samson::OK)
	   LM_X(1, ("Error sending RESET to all spawners: %s", status(s)));

	if (reset)
	{
		LM_M(("Resetted platform - I'm done"));
		exit(0);
	}

	if ((s = samsonStarter->procVecSend()) != samson::OK)
		LM_X(1, ("Error sending Process Vector to all spawners: %s", status(s)));

	LM_M(("Started platform - I'm done"));
	exit(0);
}



/* ****************************************************************************
*
* readyCheck - 
*/
void readyCheck(void* callbackData, void* userParam)
{
	int unhelloed = 0;
	int now;
	int helloed = 0;

	callbackData  = NULL;
	userParam     = NULL;

	for (int ix = 0; ix < samsonStarter->networkP->epMgr->endpointCapacity(); ix++)
	{
		samson::Endpoint2* ep;

		ep = samsonStarter->networkP->epMgr->get(ix);
		if (ep == NULL)
			continue;   // Could do break here, really ...

		if (ep->typeGet() == samson::Endpoint2::Spawner)
		{
			if (ep->stateGet() != samson::Endpoint2::Ready)
				++unhelloed;
			else
				++helloed;

			LM_M(("Spawner endpoint in '%s'. helloed: %d, unhelloed: %d", ep->hostGet()->name, helloed, unhelloed));
		}			

		if (ep->typeGet() == samson::Endpoint2::Unhelloed)
			LM_W(("Endpoint %02d is Unhelloed - could it be a spawner-to-be ... ?", ix));
	}

	samsonStarter->networkP->epMgr->show("readyCheck");

	LM_M(("helloed: %d, unhelloed: %d", helloed, unhelloed));
	if (unhelloed == 0)
	{
		if (helloed != samsonStarter->spawners)
			LM_X(1, ("Helloed: %d, Spawners: %d - what has happened?", helloed, samsonStarter->spawners));

		LM_M(("All Helloes interchanged - READY TO ROLL!"));
		if (pList)
			plist();
		else
			resetAndStart();		
	}
	else
	{
		now = time(NULL);
		if ((now - startTime) > 60)
			LM_X(1, ("A minute elapsed since connecting to spawners and still not all of them have helloed - I die"));
	}
}



/* ****************************************************************************
*
* man texts -
*/
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsonSetup is a tool to configure/start/stop a samson cluster ...\n\n"
   "Complete list of options:\n";
static const char* manDescription      =
"samsonSetup is a tool to setup a samson cluster where the samson platform is to run.\n"
"A samson cluster consists of a set of one or more computers, also called nodes.\n"
"One node is special in that the samson controller runs there.\n"
"To tell samsonSetup in which node the controller is supposed to run,\n"
"the option '-controller' is used, followed by the name/IP of that node.\n"
"Typically, a samson worker would also run in the controller node, as the processing\n"
"load of the samson controller is quite low.\n"
"To inform samsonSetup of the other nodes of the cluster (apart form the controller node),\n"
"the option '-ips' is used, followed by the list of the nodes, as one string.\n"
"\n"
"Example 1 - Start a samson cluster on nodes m1, m2, m3 and m4, where m3 is the controller node:\n"
"\n"
"  % samsonSetup -controller m3 -workers 4 -ips 'm1 m2 m3 m4'\n"
"\n"
"\n"
"Before running samsonSetup, one must make sure that samsonSpawer is running in all nodes\n"
"that are to be part of the cluster. samsonSetup connects to the spawner process in each\n"
"of the nodes of the cluster, telling the spawner to stop any existing samson processes\n"
"and then to start the necessary samson processes for the new cluster.\n"
"In the controller node, a samsonController process and a samsonWorker will be spawner\n"
"by the spawner, and in all other nodes, just the samsonWorker will be spawned.\n"
"\n"
"samsonSetup can also be used to shutdown a running samson cluster. To do that, the\n"
"option 'reset' is used.\n"
"\n"
"Example 2 - Shutdown a running samson cluster\n"
"\n"
"  % samsonSetup -controller m3 -reset\n"
"\n"
"\n"
"If you want to use only one node for your samson platform you can use a simplefying option\n"
"for samsonSetup - '-local'. If this option is given, samsonSetup will prepare a minimal\n"
"samson cluster on the nodes where samsonSetup is executed, with just a controller and a worker.\n"
"\n"
"Example 3 - Start a minimal samson cluster on the local node\n"
"  % samsonSetup -local\n"
"\n"
"\n"
"samsonSetup prints traces on the console while it executes.\n"
"If you need more details, there are a number of options for this:\n"
"\n"
"  -v           Verbose output\n"
"  -d           Debug output\n"
"  -t <levels>  Trace levels. There are 256 trace levels, from 0 to 255 and the\n"
"               levels are parsed as a comma separated list of ranges. No spaces allowed.\n"
"  -r           Show all read messages\n"
"  -w           Show all written messages\n"
"\n"
"\n"
"Example 4 - Start a minimal cluster with plenty of console traces.\n"
"\n"
"  % samsonSetup -local -v -d -r -w -t0-10,40-55,121-240\n"
"\n"
"  NOTE: The '0-10,40-55,121-240' as trace levels is just an example on how the trace levels\n"
"        are parsed, it is not an interesting real life example.\n"
"\n"
"\n"
"Finally, the usage of samsonSetup is obtained using the '-u' option, but also if samsonSetup is\n"
"started with any unknown options.\n"
"\n"
"Example 5 - samsonSetup usage\n"
"\n"
"  % samsonSetup -u\n\n";


static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-bug-report@tid.es\nSamson home page: <http://www.tid.es/products/samson>";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	memset(controllerHost, 0, sizeof(controllerHost));

	paConfig("prefix",                        (void*) "SSP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT (FUNC)");
	paConfig("log to file",                   (void*) true);

    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (local == true)
	{
		sprintf(controllerHost, "localhost");
		ips[0]  = (char*) 1;
		ips[1]  = "localhost";
		ips[2]  = NULL;
		workers = 1;
	}
	
	if ((long) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (long) ips[0]));

	samson::SamsonSetup::init();

    // Init the engine
    engine::Engine::init();
    
	engine::MemoryManager::init(samson::SamsonSetup::getUInt64("general.memory"));

	samsonStarter = new SamsonStarter();
	samsonStarter->procVecCreate(controllerHost, workers, ips);

	startTime = time(NULL);
	if (samsonStarter->connect() != samson::OK)
		LM_X(1, ("Error connecting to all spawners"));

	LM_M(("Connected to all spawners"));

	samsonStarter->networkP->epMgr->callbackSet(samson::EndpointManager::Timeout,  readyCheck, NULL);
	samsonStarter->networkP->epMgr->callbackSet(samson::EndpointManager::Periodic, readyCheck, NULL);

	samsonStarter->networkP->epMgr->show("Before calling run");
	samsonStarter->run();

	return 0;
}