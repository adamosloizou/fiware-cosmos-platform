

#include <sys/time.h>                   // gettimeofday()

#include "logMsg/logMsg.h"              // LM_M()

#include "LockDebugger.h"               // LockDebugger
#include "au/mutex/Token.h"                   // au::Token
#include "au/Cronometer.h"              // au::Cronometer
#include "au/ExecesiveTimeAlarm.h"
#include "TokenTaker.h"				    // Own interface

//#define DEBUG_AU_TOKEN

namespace au {

TokenTaker::TokenTaker( Token* _token  )
{
    au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");
    
    token = _token;
    name = "Unknown";
    
    //LM_M(("New TokenTaker %s for token %s", name ,  token->name));
    token->Retain();
}

TokenTaker::TokenTaker( Token* _token , const char* _name )
{
    au::ExecesiveTimeAlarm alarm("TokenTaker::TokenTaker");
    
    token = _token;
    name = _name;
    
    //LM_M(("New TokenTaker %s for token %s", name ,  token->name));
    token->Retain();
}

TokenTaker::~TokenTaker()
{
    //LM_M(("Destroy TokenTaker %s for token %s", name ,  token->name));
    token->Release();
}


void TokenTaker::stop(  )
{
    token->Stop();
}

void TokenTaker::wakeUp()
{
    // Wake up a thread that has been "stop"
    //LM_M(("Wake up for token %s", token->name ));
    
    if( pthread_cond_signal(&token->_block) != 0)
        LM_X(1, ("Internal error at au::TokenTaker"));
    
    
}

void TokenTaker::wakeUpAll()
{
    // Wake up all stopped threads
    //LM_M(("Wake up all for token %s", token->name ));
    
    if( pthread_cond_broadcast(&token->_block) != 0)
        LM_X(1, ("Internal error at au::TokenTaker"));
}

}
