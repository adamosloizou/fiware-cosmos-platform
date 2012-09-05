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

/*
 * FILE            SpreadMapProcess.h
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Declaration of SpreadMapProcess class to emit
 *  all the fields from a system.Value key, under the Process paradigm.
 *  The inclusion of a reference field ("timestamp" usually) is optional
 *
 */

#ifndef _H_SAMSON_system_SPREAD_MAP_PROCESS
#define _H_SAMSON_system_SPREAD_MAP_PROCESS

#include <string>

#include "samson_system/ProcessComponent.h"
#include "samson_system/Value.h"
#include "samson/module/KVWriter.h"

namespace samson {
namespace system {
class SpreadMapProcess : public ProcessComponent {

  public:

    static const std::string kNullField;
    static const std::string kNullDest;

    SpreadMapProcess()
      : ProcessComponent("default")
      , out_app_name_("def_out")
      , out_def_name_("def_def")
      , additional_field_(kNullField)
      , include_field_(false) {
    }

    SpreadMapProcess(const std::string& name = "default", const std::string& out_app = "def_out",
        const std::string& out_def = "def_def", const std::string& additional_field = kNullField)
      : ProcessComponent(name)
      , out_app_name_(out_app)
      , out_def_name_(out_def)
      , additional_field_(additional_field) {
      if (additional_field_ != kNullField)
        include_field_ = true;
    }

    ~SpreadMapProcess() {
    }

    std::string out_app_name() {
      return out_app_name_;
    }

    std::string out_def_name() {
      return out_def_name_;
    }

    std::string additional_field() {
      return additional_field_;
    }

    // Update this state based on input values ( return true if this state has been updated with this component )
    bool Update(Value *key, Value *state, Value **values, size_t num_values, samson::KVWriter* const writer);

  private:
    std::string out_app_name_;
    std::string out_def_name_;
    std::string additional_field_;
    bool include_field_;
};
}
} // End of namespace

#endif // ifndef _H_SAMSON_system_SPREAD_MAP_PROCESS
