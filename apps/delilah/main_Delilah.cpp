#include "parseArgs.h"          // parseArgs
#include "DelilahConsole.h"		// ss::DelilahConsole
#include "SamsonSetup.h"		// ss::SamsonSetup


/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
int              workers;
char             controller[80];



#define LOC "localhost:1234"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-controller",  controller,  "CONTROLLER",  PaString,  PaOpt, _i LOC,   PaNL,   PaNL,  "controller IP:port"  },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,     80,      3,    100,  "number of endpoints" },
	{ "-workers",    &workers,     "WORKERS",     PaInt,     PaOpt,      1,      1,    100,  "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	paConfig("prefix",                        (void*) "SSW_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) false);
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);
	lmAux((char*) "father");
	logFd = lmFirstDiskFileDescriptor();
	
	
	ss::SamsonSetup::load();
	
	ss::Network  network(endpoints,workers);
	std::cout << "Waiting for network connection ...";
	
	network.init(ss::Endpoint::Delilah, "delilah", 0, controller);
	network.runInBackground();
	
	while ( !network.ready() )
		sleep(1);
	std::cout << "OK\n";
	
	
	// Create a Delilah object one network is ready for the first time
	ss::Delilah delilah( &network );

	// Create a DelilahControler once Delilah is ready
	ss::DelilahConsole console( &delilah );
	console.run();
}
