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
 * FILE            ProcessComponentsManager.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Definition of ProcessComponentsManager class methods to prepare data and call
 *  the Update method implemented in the different child classes
 *
 */

#include "samson_system/ProcessComponentManager.h"  // Own interface

#include <algorithm>

#include "samson_system/TopProcessComponent.h"
#include "samson_system/ValueContainer.h"
#include "samson_system/ValueVector.h"

namespace samson {
namespace system {
// ProcessComponentsManager

ProcessComponentsManager::ProcessComponentsManager() {
  // Setup components ( this will be based on a txt file in the future )
  Add(new TopProcessComponent());
}

void ProcessComponentsManager::Add(ProcessComponent *const component) {
  components_.push_back(component);
}

// Function used in process function
void ProcessComponentsManager::Process(samson::KVSetStruct *inputs, samson::KVWriter *const writer) {
  ValueContainer keyContainer;
  ValueContainer stateContainer;

  ValueVector value_vector;

  // Recover key&state
  // ---------------------------------------------------------
  if (inputs[1].num_kvs > 0) {
    keyContainer.value->parse(inputs[1].kvs[0]->key);
    stateContainer.value->parse(inputs[1].kvs[0]->value);
  } else if (inputs[0].num_kvs > 0) {
    keyContainer.value->parse(inputs[0].kvs[0]->key);
    stateContainer.value->SetAsVoid();
  } else {
    LM_E(("Error, nothing to do because no key"));
    return;   // Nothing to do if no key ( this should never happen )
  }
  // Recover values to be processed
  // ---------------------------------------------------------
  for (size_t i = 0; i < inputs[0].num_kvs; ++i) {
    value_vector.add(inputs[0].kvs[i]->value);
  }

  // Update state ( if possible )
  // LOG_SM(("Calling ProcessComponentsManager::Update"));
  Update(keyContainer.value, stateContainer.value, value_vector.values_, value_vector.num_values_, writer);
}

void ProcessComponentsManager::Update(Value *key, Value *state, Value **values, size_t num_values,
                                      samson::KVWriter *const writer) {
  // Debug
  EmitLog("debug", "----------------------------------------", writer);
  EmitLog("debug",
          au::str("Processing state %s - %s with %lu values", key->str().c_str(), state->str().c_str(), num_values),
          writer);
  for (size_t i = 0; i < num_values; i++) {
    EmitLog("debug", au::str("Input value %lu/%lu: %s ", i, num_values, values[i]->str().c_str()), writer);
  }

  // Look into components
  // LOG_SM(("Loop for components, with components_.size():%lu", components_.size()));
  for (size_t i = 0, l = components_.size(); i < l; ++i) {
    if (components_[i]->Update(key, state, values, num_values, writer)) {
      // LOG_SM(("Processed component[%d](%s)->Update()", i, components_[i]->name_.c_str()));
      EmitLog("debug", au::str("Processed this state with component %s", components_[i]->name_.c_str()), writer);
      ++components_[i]->use_counter_;

      // Check to swap positions in the vector
      while ((i > 0) && (components_[i]->use_counter_ > components_[i - 1]->use_counter_)) {
        // LOG_SM(("Swapping components i(%d, '%s') with (i-1)(%d, '%s')", i, components_[i]->name_.c_str(), i-1, components_[i-1]->name_.c_str()));
        // Swap positions
        std::swap(components_[i], components_[i - 1]);
        --i;
      }

      // Return since this key has been processed
      EmitLog("debug", "----------------------------------------", writer);
      return;
    }
  }

  EmitLog("debug", "No component for this state", writer);
  EmitLog("debug", "----------------------------------------", writer);

  // Not processed since no component
  // Emit debug trace?
}
}
}   // End of namespace samson.system
