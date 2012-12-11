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
* FILE            Queue
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Specialized version of the std::queue when included objects are shared pointers.
*      In this case, we can return NULL if no object should be returned
*      It greatly simplifies development of objects managers in memory
*
*
* COPYRIGHT       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef AU_CONTAINERS_QUEUE_H_
#define AU_CONTAINERS_QUEUE_H_

#include "au/containers/SharedPointer.h"
#include <list>            // std::map

#include "logMsg/logMsg.h"  // LOG_SM()

namespace au {
template <class V>
class Queue {
public:

  void Push(SharedPointer<V> v) {
    list_.push_back(v);
  }

  void Push(const Queue<V>& queue) {
    std::vector<SharedPointer<V> > items = queue.items();
    for (size_t i = 0; i < items.size(); i++) {
      list_.push_back(items[i]);
    }
  }

  void PushFront(const Queue<V>& queue) {
    std::vector<SharedPointer<V> > items = queue.items();
    for (int i = (int)items.size() - 1; i >= 0; i--) {
      list_.push_front(items[i]);
    }
  }

  // Handy function to push newly allocated objects
  void Push(V *v) {
    SharedPointer<V> tmp_v(v);
    Push(tmp_v);
  }

  SharedPointer<V> Pop() {
    if (list_.size() == 0) {
      return SharedPointer<V>(NULL);
    } else {
      SharedPointer<V> r = list_.front();
      list_.pop_front();
      return r;
    }
  }

  SharedPointer<V> Front() {
    if (list_.size() == 0) {
      return SharedPointer<V>(NULL);
    } else {
      return list_.front();
    }
  }

  SharedPointer<V> Back() {
    if (list_.size() == 0) {
      return NULL;
    } else {
      return list_.back();
    }
  }

  void Clear() {
    list_.clear();
  }

  // Extract an element from the queue
  bool Contains(SharedPointer<V> v) {
    typename std::list< SharedPointer<V> >::iterator iter;
    for (iter = list_.begin(); iter != list_.end(); ++iter) {
      if (*iter == v) {
        return true;
      }
    }
    return false;
  }

  void ExtractAll(SharedPointer<V> v) {
    typename std::list< SharedPointer<V> >::iterator iter;
    for (iter = list_.begin(); iter != list_.end(); ) {
      if (*iter == v) {
        iter = list_.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  void LimitToLastItems(size_t num) {
    while (list_.size() > num) {
      list_.pop_front();
    }
  }

  size_t size() const {
    return list_.size();
  }

  std::vector< SharedPointer<V> > items() const {
    std::vector< SharedPointer<V> > vector;
    typename std::list< SharedPointer<V> >::const_iterator iter;
    for (iter = list_.begin(); iter != list_.end(); ++iter) {
      vector.push_back(*iter);
    }
    return vector;
  }

private:

  std::list< SharedPointer<V> > list_;
};
}

#endif  // ifndef AU_CONTAINERS_QUEUE_H_
