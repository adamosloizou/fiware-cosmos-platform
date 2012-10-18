#include "au/mutex/Token.h"       // Own interface



#include "LockDebugger.h"       // LockDebugger
#include "au/string/string.h"
#include "logMsg/logMsg.h"      // LM_M()
#include <assert.h>
#include <sys/syscall.h>
#include <sys/time.h>           // gettimeofday()
#include <sys/types.h>
#include <unistd.h>


namespace au {
std::string GetThreadId(pthread_t t) {
  std::ostringstream output;

  unsigned char *base = reinterpret_cast<unsigned char *>(&t);

  for (size_t i = 0; i < sizeof(t); ++i) {
    output << au::str("%02x", base[i]);
  }
  return output.str();
}

Token::Token(const std::string& name) : name_(name), locked_(false) {
  // Take the name for debugging

  int r_init = pthread_mutex_init(&lock_, 0);

  if (r_init != 0) {
    LM_LX(1, ("pthread_mutex_init for '%s' returned %d", name_.c_str(), r_init ));
  }

  int t_init_cond = pthread_cond_init(&block_, NULL);

  if (t_init_cond != 0) {
    LM_LX(1, ("pthread_cond_init for '%s' returned %d", name_.c_str(), r_init ));
  }
}

Token::~Token() {
  pthread_mutex_destroy(&lock_);
  pthread_cond_destroy(&block_);
}

void Token::Retain() {
  // LM_LM(("Thread [%s] trying to token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));

  if (IsRetainedByMe()) {
    ++counter_;
    // LM_LM(("token %s was retained by me ( thread [%s] ) . Just updating counter to %d " , name_.c_str() , GetThreadId( pthread_self() ).c_str() ,  counter_ ));
    return;
  }

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->AddMutexLock(this);
#endif
  // LOCK the mutex
  int ans = pthread_mutex_lock(&lock_);
  if (ans) {
    LM_LE(("Error %d getting mutex  (EINVAL:%d, EFAULT:%d, EDEADLK:%d", ans, EINVAL, EFAULT, EDEADLK));
    LM_LE(("Token (name:%p): pthread_mutex_lock returned error %d (%p)", name_.c_str(), ans, this));
  }
  pthread_t my_own_pthread_t = pthread_self();

  if (locked_) {
    LM_LX(1, ("Internal error: Thread [%s] has retained au::Token (%s) previously locked by thread [%s]",
              GetThreadId(my_own_pthread_t).c_str(), name_.c_str(), GetThreadId(token_owner_thread_t_).c_str()));
  }
  token_owner_thread_t_ = my_own_pthread_t;
  counter_ = 1;
  locked_ = true;

  // LM_LM(("Thread [%s] has retained token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));
}

void Token::Release() {
  // LM_LM(("Thread [%s] trying to releases token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));

  if (!locked_) {
    LM_LE(("Internal error: Releasing a non-locked au::Token:'%s'", name_.c_str()));
  }

  pthread_t my_own_pthread_t = pthread_self();

  if (!pthread_equal(my_own_pthread_t, token_owner_thread_t_)) {
    LM_LE(("Internal error: Releasing an au::Token '%s' not locked by me (%s), owner:%s", name_.c_str(),
           GetThreadId(my_own_pthread_t).c_str(), GetThreadId(token_owner_thread_t_).c_str()));
  }
  --counter_;
  if (counter_ > 0) {
    // LM_LM(("Token %s is still locked by thread [%s] with counter %d" , name_.c_str() , GetThreadId( pthread_self() ).c_str() , counter_ ));
    return;
  }

  // Flag this as unlocked
  locked_ = false;

#ifdef DEBUG_AU_TOKEN
  LockDebugger::shared()->RemoveMutexLock(this);
#endif
  // UNLOCK the mutex
  int ans = pthread_mutex_unlock(&lock_);
  if (ans) {
    LM_LE(("Error %d releasing mutex (EINVAL:%d, EFAULT:%d, EPERM:%d", ans, EINVAL, EFAULT, EPERM));
    // Goyo. The segmentation fault when quitting delilah seems to be related to a corruption in name (SAMSON-314)
    LM_LE(("Token %p: pthread_mutex_unlock returned error %d (%p)", name_.c_str(), ans, this));
  }

  // LM_LM(("Thread [%s] completely releases by token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));
}

void Token::WakeUpAll() {
  if (pthread_cond_broadcast(&block_) != 0) {
    LM_LX(1, ("Internal error at au::Token"));
  }
}

void Token::WakeUp() {
  if (pthread_cond_signal(&block_) != 0) {
    LM_LX(1, ("Internal error at au::Token"));
  }
}

void Token::Stop() {
  // LM_LM(("Thread [%s] is being stopped at token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));

  // You are supposed to be retaining this lock
  if (!locked_) {
    LM_LE(("Internal error: Releasing a non-locked au::Token:'%s'", name_.c_str()));
  }

  pthread_t my_own_pthread_t = pthread_self();

  if (!pthread_equal(my_own_pthread_t, token_owner_thread_t_)) {
    LM_LE(("Internal error: Stopping an au::Token '%s' not locked by me, owner:%s, me:%s",
           name_.c_str(), GetThreadId(token_owner_thread_t_).c_str(), GetThreadId(my_own_pthread_t).c_str()));
  }
  int tmp_counter = counter_;                                                     // Keep counter of retains
  locked_ = false;    // We are temporally releasing this token

  // This unlock the mutex and froze the process in the condition
  if (pthread_cond_wait(&block_, &lock_) != 0) {
    LM_LX(1, ("Internal error at au::TokenTaker"));    // LM_LM(("Thread [%s] is back from stopped at token %s..." , GetThreadId( pthread_self() ).c_str() ,  name_.c_str() ));
  }
  // Now you are retaining again
  locked_ = true;
  token_owner_thread_t_ = my_own_pthread_t;
  counter_ = tmp_counter;
}

bool Token::IsRetainedByMe() const {
  if (locked_ && ( pthread_equal(token_owner_thread_t_, pthread_self()))) {
    return true;
  } else {
    return false;
  }
}
}
