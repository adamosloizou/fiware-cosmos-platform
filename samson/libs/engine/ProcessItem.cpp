
#include "au/ErrorManager.h"                // au::ErrorManager
#include "au/ExecesiveTimeAlarm.h"
#include "au/ThreadManager.h"
#include "au/mutex/TokenTaker.h"            // au::TokenTake
#include "au/xml.h"                         // au::xml...

#include "engine/Engine.h"                  // engine::Engine
#include "engine/ProcessItem.h"             // Own interface
#include "engine/ProcessManager.h"          // engine::ProcessManager

namespace engine {
#pragma mark ----

ProcessItem::ProcessItem(int _priority) {
  // Default values
  priority_ = _priority;
  progress_ = 0;                // Initial progress to "0"
  running_ = false;
  process_item_description_ = "unknown";
  process_item_current_task_description_ = "";
}

ProcessItem::~ProcessItem() {
}

std::string ProcessItem::getStatus() {
  int p = progress_ * 100.0;
  std::ostringstream o;

  if (p > 0) {
    o << "[" << au::str_percentage(p) << "]";
  }
  if (running_) {
    o << "Running (" << cronometer_ << ")";
  } else {
    o << "Queued";
  } o << " : " << priority_;

  o << std::string(" : ") << process_item_current_task_description_;

  return o.str();
}

std::string ProcessItem::getDescription() {
  return au::str("Process Item '%s' Status: %s"
                 , process_item_description_.c_str()
                 , getStatus().c_str()
                 );
}

void ProcessItem::addListenerId(size_t _listenerId) {
  listeners_.insert(_listenerId);
}

bool ProcessItem::running() {
  return running_;
}

void ProcessItem::StartCronometer() {
  running_ = false;
  cronometer_.Start();
}

void ProcessItem::StopCronometer() {
  running_ = true;
  cronometer_.Stop();
}

std::set<size_t> ProcessItem::listeners() {
  return listeners_;
}

std::string ProcessItem::process_item_description() {
  return process_item_description_;
}

std::string ProcessItem::process_item_current_task_description() {
  return process_item_current_task_description_;
}

// Interface to monitor operations performance
void ProcessItem::set_process_item_description(const std::string& message) {
  process_item_description_ = message;
}

void ProcessItem::set_process_item_current_task_description(const std::string& message) {
  process_item_current_task_description_ = message;
}

au::Environment& ProcessItem::environment() {
  return environment_;
}

au::ErrorManager& ProcessItem::error() {
  return error_;
}

void ProcessItem::set_progress(double p) {
  progress_ = p;
}

double ProcessItem::progress() {
  return progress_;
}

const au::CronometerSystem& ProcessItem::cronometer() {
  return cronometer_;
}
}
