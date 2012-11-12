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


#include "Log.h"
#include "LogFile.h" // Own interface

#include "logMsg/logMsg.h"                                              // LM_T
#include "logMsg/traceLevels.h"            // LmtOtherFileDescriptors, etc.


namespace au 
{
    au::Status LogFile::read( std::string file_name , LogFile** logFile )
    {
        int fd = open( file_name.c_str() , O_RDONLY );
        LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
        if( fd < 0 )
            return OpenError;
        
        // File descriptor to read logs
        FileDescriptor file_descriptor( "reading log file" , fd );
        
        // Create log file
        *logFile = new LogFile();
        
        while (true) 
        {
            Log* log = new Log();
            if( log->read(&file_descriptor) )
                (*logFile)->logs.push_back( log );
            else
                break;
        }
        
        
        if( (*logFile)->logs.size() == 0 )
        {
            delete *logFile;
            *logFile = NULL;
            return Error;
        }
        
        return OK;
    }}
