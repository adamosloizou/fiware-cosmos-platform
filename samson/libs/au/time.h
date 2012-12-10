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
*      useful functions to work with time
*
* ****************************************************************************/

#ifndef _H_AU_TIME
#define _H_AU_TIME


#include "logMsg/logMsg.h"     // LOG_SW
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/containers/map.h"  // au::map




namespace au {
int ellapsedSeconds(struct timeval *init_time);

// String with today's timestamp
std::string todayString();


// conversions
double timeval_to_secs(timeval t);
timeval secs_to_timeval(double s);

size_t seconds_to_microseconds(double time);
size_t seconds_to_nanoseconds(double time);

std::string GetTimeStampString(time_t time);
}

#endif  // ifndef _H_AU_TIME
