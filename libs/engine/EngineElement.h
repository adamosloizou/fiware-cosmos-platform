
/* ****************************************************************************
 *
 * FILE            EngineElement
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Element defining a simple foreground task to be executed in the main thread
 * This is the basic elements of the Engine library and can be scheduled using engine::Engine::shared()->add( . )
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_ELEMENT
#define _H_ENGINE_ELEMENT

#include <sstream>                      // std::ostringstream 
#include <string>

#include "logMsg/logMsg.h"              // Lmt
#include "logMsg/traceLevels.h"         // LmtEngine

#include "au/string.h"                  // au::Format
#include "au/namespace.h"
#include "au/Cronometer.h"


NAMESPACE_BEGIN(engine)

class EngineElement 
{
    
    bool repeated;								// Flag to determine if it is necessary to repeated the process
    int period;									// Period of the execution
    int counter;                                // Number of times this element has been executed ( only in repeated )

    au::Cronometer cronometer;                  // Cronometer since creation of last execution
    
protected:
    
    std::string description;                    // String for easy debugging
    std::string shortDescription;               // Short description

public:
    
    virtual void run()=0;						// Run method to execute
    
    // Constructor for inmediate action or repeated actions
    EngineElement();
    EngineElement( int seconds );
    
    // Virtual destructor necessary to destory children-classes correctly
    virtual ~EngineElement(){};
    
    // Reschedule action once executed
    void Reschedule();
    
    // Check if it is a repeat tasks
    bool isRepeated();
    
    // Get a description string for debuggin
    std::string getDescription();

    // Get time to be executed ( in repeated task )
    double getTimeToTrigger();
    
    double getWaitingTime();
    
    // get xml information
    void getInfo( std::ostringstream& output);
    
};

// Class used only to test the automatic cancelation works

class EngineElementSleepTest : public EngineElement
{
    
public:
    
    EngineElementSleepTest() : EngineElement(10)
    {
        description = au::str("Sleep element just to sleep %d seconds", 10 );    
        shortDescription = description;
    }
    
    virtual void run()
    {
        LM_M(("Running EngineElementSleepTest for 60 seconds..."));
        sleep( 60 );    // Test the program exit
        LM_M(("End of Running EngineElementSleepTest for 60 seconds..."));
    }
    
};

NAMESPACE_END

#endif
