
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
 * FILE            ThreadManager
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Classes to controll current threads working for this app.
 */

#ifndef SAMSON_LIBS_AU_THREADMANAGER_H_
#define SAMSON_LIBS_AU_THREADMANAGER_H_

#include <pthread.h>
#include <unistd.h>

#include <list>         // std::list
#include <map>          // std::map
#include <set>          // std::set
#include <string>       //  std::string
#include <vector>       // std::vector

#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/singleton/Singleton.h"
#include "au/statistics/Cronometer.h"
#include "au/string/StringUtilities.h"

#include "logMsg/logMsg.h"                                       // LOG_SM()

#include "au/containers/StringVector.h"
#include "au/containers/map.h"

#define AU_MAX_NUM_THREADS 100

namespace au {
typedef void * (*thread_function)(void *p);
void *run_ThreadInfo(void *p);
void *run_NonDetachedThreadInfo(void *p);

class ThreadManager;

// Information about a thread
class ThreadInfo {
public:

  ThreadInfo(const std::string& name, thread_function f, void *__restrict p) {
    name_ = name;
    f_ = f;
    p_ = p;
  }

private:

  au::Cronometer cronometer_;        // Cronometer for thsi thread
  std::string name_;                 // Name of this thread
  pthread_t t_;                      // Id of this thread

  // Real thread information to call this thread
  thread_function f_;                // Function to be executed
  void *__restrict p_;               // Parameter to be passed

  // Function to run a thread info
  friend void *run_ThreadInfo(void *p);
  friend void *run_NonDetachedThreadInfo(void *p);
  friend class ThreadManager;
};

// ------------------------------------------------------------
// Manager to controls all running threads
// ------------------------------------------------------------

class ThreadManager {
  ThreadManager();
  friend class au::Singleton<au::ThreadManager>;

public:

  static void wait_all_threads(std::string title);

  // Add a thread to the manager
  int addThread(
    std::string thread_name,
    pthread_t *__restrict t,
    const pthread_attr_t *__restrict attr_t,
    thread_function f,
    void *__restrict p
    );

  // Add a thread to the manager
  int addNonDetachedThread(
    std::string thread_name,
    pthread_t *__restrict t,
    const pthread_attr_t *__restrict attr_t,
    thread_function f,
    void *__restrict p
    );


  // Get name of all running threads
  au::StringVector getThreadNames();

  // Wait for all threads to finish ( except the calling thread )
  void wait(std::string title);

  // Internal function used to notify that a particular threads has finished
  void notify_finish_thread(ThreadInfo *thread_info);

  int num_threads() {
    int total = 0;

    for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
      if (threads_[i] != NULL) {
        total++;
      }
    }
    return total;
  }

  std::string str();

  static ThreadManager *shared() {
    return au::Singleton<au::ThreadManager>::shared();
  }

private:

  // "Name" of the running threads
  ThreadInfo *threads_[AU_MAX_NUM_THREADS];

  void AddThreads(ThreadInfo *thread_info) {
    for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
      if (threads_[i] == NULL) {
        threads_[i] = thread_info;
        return;
      }
    }

    LM_X(1, ("No space for more threads"));
  }

  void RemoveThreads(ThreadInfo *thread_info) {
    for (int i = 0; i < AU_MAX_NUM_THREADS; i++) {
      if (threads_[i] == thread_info) {
        threads_[i] = NULL;
        return;
      }
    }

    LM_X(1, ("Thread not found"));
  }

  // Mutex protection
  au::Token token_;
};


// Class to encapsulate a thread
//
// The thread itself is implemented in the "run" method
// Method thread_should_quit() should be periodically checked inside run() to return from "run"


class Thread {
public:

  explicit Thread(const std::string& name) {
    name_ = name;
    stoping_ = false;
    pthread_running_ = false;
  }

  // Main functions to control this thread ( from outputside the thread )
  void StartThread();
  void StopThread();

  // Overload this method to implement whatever is necessary to unlock background thread
  virtual void UnlockThread() {
  };

  bool IsThreadQuiting() {
    return stoping_;
  }

  bool IsThreadRunning() {
    return pthread_running_;
  }

private:

  // Main function of the thread to be overloaded
  virtual void RunThread() = 0;

  std::string name_;        // Name of this thread for debugging
  pthread_t t_;             // Identifier of backgroud thread
  bool pthread_running_;    // Flag to indicate background thread is really running
  bool stoping_;            // Flag to indicate background thread to stop

  friend void *run_Thread(void *p);
};
}

#endif  // SAMSON_LIBS_AU_THREADMANAGER_H_
