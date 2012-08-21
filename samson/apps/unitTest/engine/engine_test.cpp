/* ****************************************************************************
 *
 * FILE            engineTest.cpp
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

#include "gtest/gtest.h"

#include "au/ThreadManager.h"

#include "engine/Engine.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "engine/Object.h"

#include "au/ProcessStats.h"

#include "unitTest/common_engine_test.h"
#include "xmlparser/xmlParser.h"

// Tests engine's instantiation
TEST(engineTest, instantiationTest) {
  // Init test
  init_engine_test();

  EXPECT_TRUE(engine::Engine::shared() !=
              static_cast<engine::Engine *>(NULL)) << "engine instance should not be null after instantiation";

  close_engine_test();
}

// Object* getObjectByName( const char *name );
TEST(engineTest, getObjectByNameTest) {
  // Init test
  init_engine_test();

  // engine::Engine::init();
  engine::Object *testObject = new engine::Object("test_object");

  // Now the object should be registered in engine
  EXPECT_EQ(engine::Engine::shared()->objectByName("test_object"), testObject);

  // Remove test object
  delete testObject;

  close_engine_test();
}


