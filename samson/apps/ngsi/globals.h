#ifndef GLOBALS_H
#define GLOBALS_H

/* ****************************************************************************
*
* FILE                  globals.h -  
*
*
*
*/
#include <string>



/* ****************************************************************************
*
* MAX - 
*/
#define MAX(a, b) (((a) > (b))? (a) : (b))


/* ****************************************************************************
*
* TF - true or false
*/
#define TF(b) ((b == true)? "true" : "false")



/* ****************************************************************************
*
* version - 
*/
extern std::string version;

#endif