/* ****************************************************************************
 *
 * FILE           SharedMemoryManager_test.cpp
 *
 * AUTHOR         Grant Croker
 *
 * DATE           May 2012
 *
 * DESCRIPTION
 *
 * Unit tests for the engine::SharedMemoryManager class
 *
 */

#include "gtest/gtest.h"

#include "au/ErrorManager.h"
#include "au/ThreadManager.h"
#include "samson/client/SamsonClient.h"
#include "samson/common/ports.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "samson/stream/SharedMemoryManager.h"

#include "unitTest/common_engine_test.h"


TEST(samson_stream_SharedMemoryManager , init) {
  
  engine::SharedMemoryManager::init(1, 64000000);        // Allocate a single 64MB buffer
  engine::SharedMemoryManager *sharedMemoryManager = engine::SharedMemoryManager::shared();
  EXPECT_TRUE(sharedMemoryManager != NULL) << "Error initializing the shared memory segment";
  engine::SharedMemoryManager::destroy();
  
}
