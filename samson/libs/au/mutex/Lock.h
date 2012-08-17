

/* ****************************************************************************
 *
 * FILE            Lock.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Wrapper of the mutex included in the pthreadlibrary with usefull methods to simplify development
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef SAMSON_LOCK_H
#define SAMSON_LOCK_H

#include <pthread.h>             /* pthread_mutex_t                          */
#include <string>                /* std::string                              */

#include "au/mutex/StopLock.h"            /* StopLock                                 */



namespace au {

class Lock 
{
    pthread_mutex_t _lock;
    
public:	
    std::string description;
    
    Lock();
    ~Lock();
    
    void lock();
    void unlock();
    
    void unlock_waiting_in_stopLock(StopLock* stopLock);
    void unlock_waiting_in_stopLock(StopLock* stopLock, int max_seconds);
    void wakeUpStopLock(StopLock* stopLock);
    void wakeUpAllStopLock(StopLock* stopLock);
};


// Class to hold the lock during the life-span
class Locker
{
    Lock* lock;
public:
    
    Locker( Lock* _lock )
    {
        lock = _lock;
        lock->lock();
    }
    
    ~Locker()
    {
        lock->unlock();
    }
};

}

#endif
