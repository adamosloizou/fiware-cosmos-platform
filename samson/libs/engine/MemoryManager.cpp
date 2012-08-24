/* ****************************************************************************
 *
 * FILE                     MemoryManager.cpp
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include "logMsg/logMsg.h"         // LM_*
#include "logMsg/traceLevels.h"

#include "au/mutex/TokenTaker.h"   // au::TokenTaker
#include "au/xml.h"                // au::xml...

#include "engine/Buffer.h"         // samson::Buffer

#include <sstream>                 // std::stringstream

#include "engine/DiskManager.h"    // enigne::DiskManager
#include "engine/Engine.h"         // engine::Engine
#include "engine/Notification.h"   // engine::Notification
#include "engine/ProcessManager.h"  // engine::ProcessManager

#include "engine/MemoryManager.h"  // Own interface

namespace engine {
MemoryManager::MemoryManager(size_t memory) : token_("Memory Manager") {
  memory_ = memory;      // Total available memory
  used_memory_ = 0;      // No memory used so far
}

MemoryManager::~MemoryManager() {
}

void MemoryManager::Add(Buffer *buffer) {
  buffers_.insert(buffer);
  used_memory_ += buffer->max_size();      // Increse the internal counter of memory
  if (used_memory_ > 3 * memory_) {
    LM_X(1, ("Too much memory allocated"));
  }
}

void MemoryManager::Remove(Buffer *buffer) {
  buffers_.erase(buffer);
  used_memory_ -= buffer->max_size();
}

size_t MemoryManager::memory() {
  return memory_;
}

size_t MemoryManager::used_memory() {
  return used_memory_;
}

int MemoryManager::num_buffers() {
  au::TokenTaker tk(&token_);

  return buffers_.size();
}

double MemoryManager::memory_usage() {
  return (double)used_memory_ / (double)memory_;
}

au::tables::Table MemoryManager::getTableOfBuffers() {
  au::tables::Table table("Type,left,different|Name,left,different|Size,f=uint64,sum");

  au::TokenTaker tt(&token_);

  std::set<Buffer *>::iterator it_buffers;
  for (it_buffers = buffers_.begin(); it_buffers != buffers_.end(); it_buffers++) {
    Buffer *buffer = *it_buffers;
    au::StringVector values;

    values.push_back(buffer->type());
    values.push_back(buffer->name());
    values.push_back(au::str("%lu", buffer->size()));

    table.addRow(values);
  }

  return table;
}
}

