
/* ****************************************************************************
 *
 * FILE            SamsonClient .cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            7/14/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#include "au/Format.h"                          // au::Format

#include "engine/Engine.h"                      // engine::Engine
#include "engine/MemoryManager.h"               // engine::MemoryManager
#include "engine/DiskManager.h"                 // engine::DiskManager

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup

#include "samson/network/Network2.h"
#include "samson/network/EndpointManager.h"
#include "samson/network/Packet.h"

#include "samson/delilah/Delilah.h"             // samson::Delilah

#include "SamsonClient.h"                       // Own interface

namespace samson {
    
    samson::EndpointManager* epMgr     = NULL;
    samson::Network2*        networkP  = NULL;
    samson::Delilah* delilah = NULL;    
    
    SamsonClient::SamsonClient()
    {
        memory = 1024*1024*1024;
        
        load_buffer_size =  64*1024*1024;
       
    }
 
    void SamsonClient::setMemory ( size_t _memory )
    {
        memory = _memory;
    }
    
    bool SamsonClient::init( std::string controller )
    {
                
        // Init the setup system
        samson::SamsonSetup::init();    

        // Change the values for this parameters
        samson::SamsonSetup::shared()->setValueForParameter("general.memory", au::Format::string("%lu",memory) );
        samson::SamsonSetup::shared()->setValueForParameter("load.buffer_size",  au::Format::string("%lu",load_buffer_size) );
        
        // Init the engine sussytem
        engine::Engine::init();

        // Init the memory manager
        engine::MemoryManager::init(  samson::SamsonSetup::getUInt64("general.memory") );
        
        // Init the disk manager for the popQueue operations
        engine::DiskManager::init( 1 );
        
        
        // Initialize the network element for delilah
        epMgr     = new samson::EndpointManager(samson::Endpoint2::Delilah, controller.c_str() );
        networkP  = new samson::Network2(epMgr);
        
        // Init the network connection in background
        networkP->runInBackground();
        
        
        // Check connection is ok...
        for (int i = 0 ; i < 10 ; i++ )
        {
            if( networkP->ready() )
                break;
            else
                sleep(1);
        }

        if( !networkP->ready() )
        {
            error_message = au::Format::string( "Not possible to connect with controller '%s'", controller.c_str() );
            return false;
        }
        
        //
        // Ask the Controller for the platform process list
        samson::Packet*  packetP  = new samson::Packet(samson::Message::Msg, samson::Message::ProcessVector);
        
        LM_TODO(("I should probably go through NetworkInterface here ..."));
        epMgr->controller->send( packetP );
        
        // Create a DelilahControler once network is ready
        delilah = new Delilah( networkP, true );
       
        return true;
        
    }
    
}
