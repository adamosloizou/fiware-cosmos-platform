
/* ****************************************************************************
 *
 * FILE            xml.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull functions to generate xml tags
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_XML
#define _H_AU_XML

#include <string>
#include <sstream>

#include "au/map.h"

namespace au {

    
    // Simple xml tags
    void xml_open( std::ostringstream& output , std::string name );
    void xml_close( std::ostringstream& output , std::string name );

    
    template< typename T>
    void xml_simple( std::ostringstream& output , std::string name , T value )
    {
        output << "<" << name << ">";
        output << value;
        output << "</" << name << ">";
    }
    
    template< typename T>
    void xml_single_element( std::ostringstream& output , std::string name , T* obj )
    {
        xml_open( output , name );
        obj->getInfo( output );
        xml_close( output , name );
    }
    
    
    template< typename T>
    void xml_iterate_list( std::ostringstream& output , std::string name , T& vector )
    {
        au::xml_open(output, name);
        
        //typename std::map<K, V* >::iterator iter;
        typename T::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            (*iter)->getInfo( output );
        
        au::xml_close(output, name);
        
    }

    template< typename T>
    void xml_reverse_iterate_list( std::ostringstream& output , std::string name , T& vector )
    {
        au::xml_open(output, name);
        
        //typename std::map<K, V* >::iterator iter;
        typename T::r_iterator iter;
        
        for (iter = vector.r_begin() ; iter != vector.r_end() ; iter++)
            (*iter)->getInfo( output );
        
        au::xml_close(output, name);
        
    }
    
    
    template< typename T>
    void xml_iterate_list_object( std::ostringstream& output , std::string name , T& vector )
    {
        au::xml_open(output, name);
        
        //typename std::map<K, V* >::iterator iter;
        typename T::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            (iter)->getInfo( output );
        
        au::xml_close(output, name);
        
    }
    
    template< typename K, typename V>
    void xml_iterate_map( std::ostringstream& output , std::string name , au::map< K , V >& vector )
    {
        au::xml_open(output, name);
        
        //typename std::map<K, V* >::iterator iter;
        typename au::map< K , V >::iterator iter;
        
        for (iter = vector.begin() ; iter != vector.end() ; iter++)
            iter->second->getInfo( output );
        
        au::xml_close(output, name);
        
    }
    
}

#endif