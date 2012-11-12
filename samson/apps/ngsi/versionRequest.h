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
#ifndef VERSION_REQUEST_H
#define VERSION_REQUEST_H

/* ****************************************************************************
*
* FILE                versionRequest.h - 
*
*
*
*
*/
#include <string>                           // std::string

#include "Format.h"                         // Format (XML, JSON)
#include "Verb.h"                           // Verb



/* ****************************************************************************
*
* versionRequest - 
*/
extern int versionRequest(int fd, Verb verb, Format format, int components, std::string* component);

#endif

