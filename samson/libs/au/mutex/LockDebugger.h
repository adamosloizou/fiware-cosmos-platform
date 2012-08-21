
/* ****************************************************************************
*
* FILE            LockDebugger.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*  Class used to debug the Locks and Tokens in au library.
*  It can detecs auto-locks and cross-lock conditions
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef SAMSON_LOCK_DEBUGGER_H
#define SAMSON_LOCK_DEBUGGER_H

#include <map>                   /* map                                      */
#include <pthread.h>             /* pthread_mutex_t                          */
#include <set>                   /* set                                      */
#include <string>                /* std::string                              */



namespace au {
class Lock;

class LockDebugger {
  pthread_mutex_t _lock;

  std::map< pthread_t, std::set< void * > * > locks;

  LockDebugger();
  ~LockDebugger();

public:

  pthread_key_t key_title;      // A title for each thread

  static void setThreadTitle(std::string);

  void add_lock(void *new_lock);
  void remove_lock(void *new_lock);

  static LockDebugger *shared();
  static void           destroy();

private:

  std::string _getTitle();
  std::set<void *> *_getLocksVector();
  bool _cross_blocking(void *new_lock);
};
}

#endif // ifndef SAMSON_LOCK_DEBUGGER_H
