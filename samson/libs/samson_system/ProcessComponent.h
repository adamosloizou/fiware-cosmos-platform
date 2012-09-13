/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/*
 * FILE            ProcessComponent.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Declaration of ProcessComponent class to implement a flexible processing framework,
 *  under the Process paradigm
 *
 */
#ifndef _H_SAMSON_system_PROCESS_COMPONENT
#define _H_SAMSON_system_PROCESS_COMPONENT

#include <string>

#include "samson_system/Value.h"
#include "samson/module/KVWriter.h"

namespace samson {
namespace system {
class ProcessComponent {
  public:
    explicit ProcessComponent(std::string name) :
      name_(name), use_counter_(0) {}

    virtual ~ProcessComponent() {}

    std::string name() const {
      return name_;
    }

    // Update this state based on input values ( return true if this state has been updated with this component )
    virtual bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer) = 0;
  private:
    std::string name_;   // Debug name
    int use_counter_;    // Internal use-counter to reorder ProcessComponent's inside a ProcessComponentsManager

    friend class ProcessComponentsManager;
};

// Handy functions ( MACROs ? )
void EmitOutput(Value *key, Value *state, samson::KVWriter* const writer);
void EmitFeedback(Value *key, Value *state, samson::KVWriter* const writer);
void EmitState(Value *key, Value *state, samson::KVWriter* const writer);
void EmitLog(const std::string& key, const std::string& message, samson::KVWriter* const writer);
}
}   // End of namespace

#endif  // ifndef _H_SAMSON_system_PROCESS_COMPONENT
