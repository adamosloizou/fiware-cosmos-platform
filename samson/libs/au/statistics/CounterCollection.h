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

/* ****************************************************************************
*
* FILE            -
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      CounterCollection is a class used to keep an arbitrary number of counters by a particular "key"
*      For instance could be a string, so we keep a counter for each string.
*
* ****************************************************************************/

#ifndef _H_AU_COUNTER_COLLECTION
#define _H_AU_COUNTER_COLLECTION

#include "au/containers/map.h"       // au::map
#include <sstream>
#include <string>

namespace au {
class Counter {
  int c;
public:
  Counter();
  int get();
  void append(int v);
};

template <typename T>
class CounterCollection {
  au::map<T, Counter> counters;

public:

  ~CounterCollection() {
    counters.clearMap();
  }

  Counter *getCounterFor(T t) {
    Counter *c = counters.findInMap(t);

    if (!c) {
      c = new Counter();
      counters.insertInMap(t, c);
    }
    return c;
  }

  int appendAndGetCounterFor(T t) {
    Counter *c = getCounterFor(t);

    c->append(1);
    return c->get();
  }
};
}

#endif  // ifndef _H_AU_COUNTER_COLLECTION
