
/* ****************************************************************************
 *
 * FILE            time.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull functions to work with time
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_TIME
#define _H_AU_TIME


#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map

namespace au {


    int ellapsedSeconds(struct timeval *init_time);    

    // String with today's timestamp
	std::string todayString();
    
}

#endif
