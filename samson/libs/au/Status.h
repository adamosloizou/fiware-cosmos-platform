
/* ****************************************************************************
 *
 * FILE            status.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Definition of all the returned values in au library
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_STATUS
#define _H_AU_STATUS

#include "au/namespace.h"

namespace au 
{
    
    typedef enum Status
    {
        OK,
        Error,    // Generic error
        
        NotImplemented,
        
        Timeout,
        
        OpenError, // Error in the open call
        
        ConnectError,
        AcceptError,
        SelectError,
        SocketError,
        BindError,
        ListenError,
        ReadError,
        WriteError,
        ConnectionClosed,
        GetHostByNameError,
        
        // Google Protocol Buffer errros
        
        GPB_Timeout,
        GPB_ClosedPipe,
        GPB_ReadError,
        GPB_CorruptedHeader,
        GPB_WrongReadSize,
        GPB_ReadErrorParsing,
        GPB_NotInitializedMessage,
        GPB_WriteErrorSerializing,
        GPB_WriteError,
        
    } Status;
    
    const char* status( Status code );
    
}

#endif
