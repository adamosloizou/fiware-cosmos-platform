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
 * FILE            MapCompareFunctor.h
 *
 * AUTHOR          Gregorio Escalada
 *
 * PROJECT         SAMSON samson_system library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  Definition of MapCompareAscendingFunctor and MapCompareDescendingFunctor
 *  to compare map type vector elements in system.Value, to allow sorting
 *  vector by different fields
 *
 */

#ifndef _H_SAMSON_system_MAP_COMPARE_FUNCTOR
#define _H_SAMSON_system_MAP_COMPARE_FUNCTOR

#include <string.h>

#include "samson_system/Value.h"

namespace samson {
namespace system {


struct MapCompareAscendingFunctor {
    std::string field_;

    MapCompareAscendingFunctor(const std::string& field) {
      field_ = field;
    }

    bool operator()(const system::Value* const left, const Value* const right) {
      if (left->GetValueType() == Value::value_map) {
        Value *p_left = const_cast<Value *>(left)->GetValueFromMap(field_);
        Value *p_right = const_cast<Value *>(right)->GetValueFromMap(field_);
        if ((p_left != NULL) && (p_right != NULL)) {
          return Value::Less(p_left, p_right);
        }
      }
      return Value::Less(left, right);
    }
};

struct MapCompareDescendingFunctor {
    std::string field_;

    MapCompareDescendingFunctor(const std::string& field): field_(field) {
    }

    bool operator()(const system::Value* const left, const Value* const right) {
      if (left->GetValueType() == Value::value_map) {
        Value *p_left = const_cast<Value *>(left)->GetValueFromMap(field_);
        Value *p_right = const_cast<Value *>(right)->GetValueFromMap(field_);
        if ((p_left != NULL) && (p_right != NULL)) {
          return Value::Greater(p_left, p_right);
        }
      }
      return Value::Greater(left, right);
    }
};

}
} // namespaces
#endif // _H_SAMSON_system_MAP_COMPARE_FUNCTOR
