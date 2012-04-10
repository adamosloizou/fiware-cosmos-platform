/* ****************************************************************************
*
* FILE            diskManagerTest.cpp
*
* AUTHOR          Javier Lois
*
* DATE            December 2011
*
* DESCRIPTION
*
* unit testing of the engine library
*
*/

#include <sys/time.h>

#include "gtest/gtest.h"

#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"

#include "common_engine_test.h"

// Test DiskManager's instantiation
TEST(diskManagerTest, instantiationTest) 
{

    init_engine_test();
    
    //call init() and then shared(). Should return a valid one.
    ASSERT_TRUE(engine::DiskManager::shared() != static_cast<engine::DiskManager*>(NULL)) 
                << "DiskManager instance should not be null after instantiation"; 

    close_engine_test();
}

//test void add( DiskOperation *operation )
TEST(diskManagerTest, addTest) 
{

    init_engine_test();
    
    class A : public engine::Object 
    {
        au::Token token;
        
    public:

        A() : token("Test A")
        {
        }
        
        void test()
        {
            au::TokenTaker tt(&token);
            
            char buffer[1024*1024];
            engine::DiskOperation* operation = engine::DiskOperation::newReadOperation( buffer , "test_filename.txt" , 0 , 1, 0 );
            engine::DiskManager::shared()->add(operation);
            
            EXPECT_EQ(engine::DiskManager::shared()->getNumOperations(), 1) << "Wrong number of disk operations";
            
        }
        
        void notify( engine::Notification* notification )
        {
            au::TokenTaker tt(&token);
            
        }

        
    };
    
    // instantiate and test
    A a;
    a.test();
    
    close_engine_test();
}

    


 
