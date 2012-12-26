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
#ifndef QSI_INPUT_RETURN_FUNCTION_H
#define QSI_INPUT_RETURN_FUNCTION_H

/* ****************************************************************************
*
* FILE                     QsiInputReturnFunction.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 19 2011
*
*/



namespace Qsi
{



/* ****************************************************************************
*
* InputReturnFunction - 
*/
typedef void (*InputReturnFunction)(char* nameV[], char* inputV[]);

}

#endif
