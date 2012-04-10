/* ****************************************************************************
*
* FILE            processManager_test.cpp
*
* AUTHOR          Javier Lois
*
* DATE            January 2012
*
* DESCRIPTION
*
* unit testing of the ProcessManager class in the engine library
*
*/

//#include <direct.h> // for getcwd
#include <stdlib.h>// for MAX_PATH

#include "gtest/gtest.h"

#include "engine/ProcessManager.h"
#include "engine/ProcessItem.h"
#include "engine/Notification.h"

#include "xmlmarkup/xmlmarkup.h"

#include "common_engine_test.h"


// Tests ProcessManager's instantiation
TEST(processManagerTest, instantiationTest) 
{
    init_engine_test();
    
    EXPECT_TRUE(engine::ProcessManager::shared() != static_cast<engine::ProcessManager*>(NULL)) << "ProcessManager instance should not be null after instantiation"; 
    
    close_engine_test();
}

//Test static int getNumCores();
TEST(processManagerTest, getNumCoresTest) 
{
    init_engine_test();
    
    EXPECT_EQ(engine::ProcessManager::shared()->getNumCores(), 4);

    close_engine_test();
}

