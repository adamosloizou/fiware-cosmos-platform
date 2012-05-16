#ifndef VERB_H
#define VERB_H

/* ****************************************************************************
*
* FILE                  Verb.h 
*
*
*
*
*/



/* ****************************************************************************
*
* Verb - 
*/ 
typedef enum Verb
{
    GET = 1,
    POST,
    PUT,
    DELETE
} Verb;



/* ****************************************************************************
*
* verbName - 
*/
extern const char* verbName(Verb verb);

#endif