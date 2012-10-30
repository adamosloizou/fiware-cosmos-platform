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


#include "SamsonWorker.h"
#include "SamsonWorkerSamples.h"  // Own interface
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

namespace samson {
void SamsonWorkerSamples::take_samples() {
  int num_processes = engine::Engine::process_manager()->num_used_procesors();
  int max_processes = engine::Engine::process_manager()->max_num_procesors();

  size_t used_memory = engine::Engine::memory_manager()->used_memory();
  size_t max_memory = engine::Engine::memory_manager()->memory();

  size_t disk_read_rate = (size_t) engine::Engine::disk_manager()->rate_in();
  size_t disk_write_rate = (size_t) engine::Engine::disk_manager()->rate_out();

  size_t network_read_rate = 0;
  size_t network_write_rate = 0;

  if (samsonWorker_->network() != NULL) {
    network_read_rate = (size_t)samsonWorker_->network()->get_rate_in();
    network_write_rate = (size_t)samsonWorker_->network()->get_rate_out();
  }

  cpu.push(100.0 *  (double)num_processes / (double)max_processes);
  memory.push(100.0 * (double)used_memory / (double)max_memory);

  disk_in.push(disk_read_rate / 1000000);
  disk_out.push(disk_write_rate / 1000000);

  net_in.push(network_read_rate / 1000000);
  net_out.push(network_write_rate / 1000000);
}
}