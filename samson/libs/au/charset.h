



/* ****************************************************************************
*
* FILE            charser.h
*
* AUTHOR          Andreu Urruela & Gregorio Escalada
*
* PROJECT         au library
*
* DATE            April 2012
*
* DESCRIPTION
*
* Handy functions to work with different charsets
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_AU_CHARSET
#define _H_AU_CHARSET

#include <string>

namespace au {
// Get the number of screen characters of a UTF-8 string
size_t strlenUTF8(const char *cad_utf);

// Serialize and unserialize a utf8 string
int serialize_utf8_symbol(unsigned long s, char *str);
unsigned long get_utf8_symbol(const char *str, int *length);

bool iso_8859_is_letter(unsigned char c);
bool iso_8859_is_printable(unsigned char c);

unsigned char iso_8859_to_lower(unsigned char c);
unsigned char iso_8859_to_upper(unsigned char c);
}  // end namespace au

#endif