/* ****************************************************************************
 *
 * FILE            Tree.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull classes to get tree data-sets
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_TREE
#define _H_AU_TREE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W

#include "au/CommandLine.h"
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/map.h"                 // au::map
#include "au/string.h"              // au::str(...)

namespace au {
    
    
    
    // Conversion function
    double toDouble( std::string value );
    std::string toString( double value );
    
    
    class TreeItem
    {
        std::string value;
        std::vector<TreeItem*> items;
        
    public:
        
        TreeItem( std::string _value );

        std::string getValue();
        
        std::string getChildrenValue();
        
        std::string getFirstItemValue();
        
        bool isValue( std::string _value );
        
        TreeItem* getItem( std::string name , int position );
        
        std::string getProperty( std::string name );
        
        void add( TreeItem* item);
        
        size_t getNumItems();
        
        TreeItem* getItem( size_t pos );
        
        void str( std::ostringstream& output , int level );
        
        std::string str();
      
        TreeItem* getItemFromPath( std::string path );
        TreeItem* getItemFromFieldDescription( std::string field_description );
        
        
        size_t getUInt64( std::string path );
        std::string get( std::string path );
        
    };
}

#endif