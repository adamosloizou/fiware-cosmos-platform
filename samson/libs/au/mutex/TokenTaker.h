
/* ****************************************************************************
*
* FILE            TokenTaker
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            Septembre 2011
*
* DESCRIPTION
*
*      Automatic management of a mutex
*      Classical trick of retaining a mutex in the contructor and releasing in destructor
*      Used in collaboration with au::Token class
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_TOCKEN_TAKER
#define _H_AU_TOCKEN_TAKER

#include <pthread.h>             /* pthread_mutex_t                          */
#include <string>

namespace au {
class Token;

class TokenTaker {
public:

  TokenTaker(Token *_token, const std::string& name = "unknown");
  ~TokenTaker();

  void Stop();
  void WakeUp();
  void WakeUpAll();

private:
  std::string name_;
  Token *token_;
};
}

#endif  // ifndef _H_AU_TOCKEN_TAKER