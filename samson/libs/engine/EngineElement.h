/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Element defining a simple foreground task to be executed in the main thread
 * This is the basic elements of the Engine library and can be scheduled using engine::Engine::shared()->add( . )
 */

#ifndef SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_
#define SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_

#include <unistd.h>             // sleep

#include <sstream>              // std::ostringstream
#include <string>

#include "logMsg/logMsg.h"      // Lmt
#include "logMsg/traceLevels.h"  // LmtEngine

#include "au/Cronometer.h"
#include "au/namespace.h"
#include "au/string.h"          // au::Format


NAMESPACE_BEGIN(engine)
  class EngineElement {
    typedef enum {
      normal,                   // Executed once
      repeated,                 // Repeated periodically
      extra,                    // Executed when nothing else is necessary to be executed before
    } Type;


    // Name of the element ( to do some statistics over it )
    std::string name_;

    // Type of element
    Type type;

    // Fields in repeated type
    // ------------------------------------------------
    int counter;                                // Number of times this element has been executed ( only in repeated )
    int period;                                 // Period of the execution

    au::Cronometer cronometer;                  // Cronometer since creation or last execution

  protected:
    std::string description;                    // String for easy debugging
    std::string shortDescription;               // Short description

  public:
    virtual void run() = 0;                     // Run method to execute

    // Constructor for inmediate action or repeated actions
    explicit EngineElement(std::string name);
    EngineElement(std::string name, int seconds);


    // Set as an extra element ( to be executed when nothing else has to be executed )
    void set_as_extra() {
      type = extra;
    }

    // Virtual destructor necessary to destory children-classes correctly
    virtual ~EngineElement() {
    };

    // Reschedule action once executed ( for repeated )
    void Reschedule();

    // Check type of element
    bool isRepeated();
    bool isExtra();
    bool isNormal();

    // Recover name
    std::string getName();

    // Get a description string for debuggin
    std::string getDescription();
    std::string str();

    // Get time to be executed ( in repeated task )
    double getTimeToTrigger();
    double getWaitingTime();
    double getPeriod();

    // get xml information
    void getInfo(std::ostringstream& output);
  };

  // Class used only to test the automatic cancelation works
  class EngineElementSleepTest : public EngineElement {
  public:
    EngineElementSleepTest() : EngineElement("sleep", 10) {
      description = au::str("Sleep element just to sleep %d seconds", 10);
      shortDescription = description;
    }

    virtual void run() {
      LM_M(("Running EngineElementSleepTest for 60 seconds..."));
      sleep(60);        // Test the program exit
      LM_M(("End of Running EngineElementSleepTest for 60 seconds..."));
    }
  };
NAMESPACE_END

#endif  // SAMSON_LIBS_ENGINE_ENGINEELEMENT_H_
