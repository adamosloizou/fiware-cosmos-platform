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

#ifndef _H_LOG_SERVER_CHANNEL
#define _H_LOG_SERVER_CHANNEL

#include <string>
#include <sys/stat.h> // mkdir
#include <fcntl.h>

#include "logMsg/logMsg.h"

#include "au/Descriptors.h"
#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/containers/map.h"
#include "au/containers/list.h"
#include "au/tables/Table.h"
#include "au/string/split.h"

#include "au/log/TableLogFormatter.h"
#include "au/log/LogFile.h"
#include "au/log/Log.h"

namespace au 
{
    
    class LogServer;
    class LogServerQuery;
    
    // On memory container of logs ( for fast queries )
    class LogContainer
    {
        au::list<Log> logs;
        
        size_t size; // Total accumulated size
        size_t num;  // Number of logs 
        
        size_t max_size;
        size_t max_num;
        
        au::Token token;

        friend class LogServerChannel;
        
    public:
        
        LogContainer() : token("LogContainer")
        {
            size = 0;
            num = 0;
            
            max_size = 1000000000;
            max_num  = 100000000;
        }
        
        void push( Log* log )
        {
            au::TokenTaker tt(&token);
            
            // Retain log...
            log->retain();
            
            // Total size and num
            num++;
            size += log->getTotalSerialitzationSize();

            // Put in the list
            logs.push_back(log);

            while( ( size > max_size ) || ( num > max_num ) )
            {
                Log* tmp_log = logs.extractFront();

                // Modify totals
                num--;
                size-=log->getTotalSerialitzationSize();

                // Release the log
                tmp_log->release();
            }
        }
        
        
        void clear()
        {
            au::TokenTaker tt(&token);
            
            size = 0;
            num = 0;

            au::list<Log>::iterator it;
            for( it = logs.begin() ; it != logs.end() ; it++ )
                (*it)->release();
            logs.clear(); // Not remove internal elements
        }
        
        size_t getSize()
        {
            au::TokenTaker tt(&token);
            return logs.size();
        }
        
        std::string getInfo()
        {
            au::Descriptors types;
            au::Descriptors channels;
            
            au::TokenTaker tt(&token);
            au::list<Log>::iterator it;
            for( it = logs.begin() ; it != logs.end() ; it++ )
            {
                Log* log = *it;
                types.add( log->get("TYPE" ) );
                channels.add( log->get("channel" ) );
            }
            
            
            au::tables::Table table("Concept|Value,left" );
            table.setTitle("Log info");
            
            // Number of logs
            {
                au::StringVector values;
                values.push( "Number of logs" );
                values.push( au::str(num) + " / " + au::str(max_num) );
                table.addRow( values );
            }
            
            // Size of logs
            {
                au::StringVector values;
                values.push( "Size" );
                values.push( au::str(size,"B") + " / " + au::str(max_size,"B") );
                table.addRow( values );
            }
            
            // Type of logs
            {
                au::StringVector values;
                values.push( "Types" );
                values.push( types.str() );
                table.addRow( values );
            }

            // Channels
            {
                au::StringVector values;
                values.push( "Channels" );
                size_t num_channels = channels.size();
                if( num_channels < 5 )
                    values.push( channels.str() );
                else
                    values.push( au::str("%lu channels", num_channels ) );
                table.addRow( values );
            }
            
            if( logs.size() > 0 )
            {
                table.addRow( au::StringVector( "From" , (*logs.begin())->get("timestamp") )  );
                table.addRow( au::StringVector( "To" , (*logs.rbegin())->get("timestamp") )  );
            }
            
            return table.str();

        }
        
        
    };
    
    class LogServerChannel : public network::Service
    {
        std::string directory;      // Directory to save data in
         
        au::Token token;            // Mutex protection ( multithread since we receive multiple connections )
        
        int file_counter;           // Used to write over a file
        size_t current_size;        // Current written size
        
        au::FileDescriptor *fd;     // Current file descriptor to save data
        
        LogContainer log_container; // Container of logs in memory ( fast query )
        
        
        friend class LogServer;
        
    public:

        au::rate::Rate rate;     // Estimated data rate for this channel

        // Constructor & destructor
        LogServerChannel( int port , std::string _directory );
        virtual ~LogServerChannel();
        
        // Init service 
        void initLogServerChannel( au::ErrorManager * error );

        // network::Service interface : main function for every active connection
        void run( au::SocketConnection * socket_connection , bool *quit );
        
        // Get some info bout logs
        std::string getInfo();
        
        // Generic function to get a table of logs ( based on disk files )
        std::string  getTable( au::CommandLine * cmdLine );

        // Generic function to get a table of channels ( log connections )
        std::string  getChannelsTable( au::CommandLine * cmdLine );
        
        // Add a new session mark ( used in future queries )
        void addNewSession();
        
    private:
        
        // Open a new file descriptor ( disk ) to save received logs 
        void openFileDescriptor( au::ErrorManager * error );

        // Add a log to the channel
        void add( Log*log );
        
        // Get name for the counter-th log file
        std::string getFileNameForLogFile( int counter );

        
    };
}
#endif