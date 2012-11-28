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

#include "au/ThreadManager.h"

#include "engine/DiskManager.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"

#include "samson/common/SamsonSetup.h"
#include "samson/common/samsonDirectories.h"
#include "samson/common/samsonVars.h"

#include "samson/client/SamsonClient.h"

void init_samson_setup() {
  std::string samson_home    = SAMSON_HOME_DEFAULT;
  std::string samson_working = SAMSON_WORKING_DEFAULT;
  char *env_samson_working = getenv("SAMSON_WORKING");
  char *env_samson_home = getenv("SAMSON_HOME");

  if (env_samson_working) {
    samson_working = env_samson_working;
  }
  if (env_samson_home) {
    samson_home = env_samson_home;
  }

  // Load setup and create default directories
  samson::SamsonSetup *samson_setup = au::Singleton<samson::SamsonSetup>::shared();
  samson_setup->SetWorkerDirectories(samson_home, samson_working);
}

void init_engine_test() {
  engine::Engine::InitEngine(4, 1000000, 1);  // Init engine
}

void close_engine_test() {
  engine::Engine::StopEngine();                      // Destroy engine  // Close engine simultion
}

samson::SamsonClient *init_samson_client_test() {
  // General init of the SamsonClient library
  size_t total_memory = 64 * 1024 * 1024;  // Use 64Mb for this test

  samson::SamsonClient::general_init(total_memory);
  LOG_SM(("general_init() returned"));

  // Create client connection
  LOG_SM(("creating samson_client"));
  samson::SamsonClient *samson_client = new samson::SamsonClient("SamsonClientTest");
  LOG_SM(("samson_client created"));

  if (samson_client->connect("localhost")) {
    LOG_SW(("Not possible to samson_client to  localhost"));  // SamsonClient to play with
  }
  return samson_client;
}

void close_samson_client_test(samson::SamsonClient *samson_client) {
  // Wait until all activity is finished
  samson_client->waitUntilFinish();

  LOG_SM(("waitUntilFinish finished"));

  // Disconnect from worker ( if previously connected )
  // samson_client->disconnect();

  LOG_SM(("client disconnected"));

  engine::Engine::StopEngine();

  LOG_SM(("engine  stopped"));

  // Remove the samson client instance
  delete samson_client;

  LOG_SM(("calling general_close()"));
  // General close of the SamsonClient library
  samson::SamsonClient::general_close();

  LOG_SM(("general_close() returned"));

  // Make sure no threads are pending to be finish
  au::ThreadManager::wait_all_threads("SamsonClientTest");    // Wait all threads to finish
}

