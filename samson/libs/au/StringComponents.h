
/* ****************************************************************************
*
* FILE            StringComponent
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            2012
*
* DESCRIPTION
*
*  Class used to parse a string into components in a non-intrusive way
*  Original string is never modified
*
* COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/


#ifndef _H_AU_STRING_COMPONENT
#define _H_AU_STRING_COMPONENT

#include <cstring>
#include <list>
#include <math.h>
#include <string>    // std::String
#include <time.h>

#include "au/Cronometer.h"
#include "au/string.h"
#include "au/xml.h"




namespace au {
class StringComponents {
  char *internal_line;
  size_t max_size;

public:

  // Components of the last split
  std::vector<char *> components;

  StringComponents();
  ~StringComponents();

  size_t process_line(const char *line, size_t max_length, char separator);
};
}

#endif  // ifndef _H_AU_STRING_COMPONENT