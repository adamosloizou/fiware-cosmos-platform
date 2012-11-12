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

/* ****************************************************************************
 *
 * FILE            map
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Specialized version of the std::map when "value" objects are pointers.
 *      In this case, we can return NULL if object is not found in the map
 *      It greatly simplifies development of objects managers in memory
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_MAP
#define _H_AU_MAP

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set
#include <string.h>
#include <iostream>

#include "logMsg/logMsg.h"					 // LM_M()

#include "au/namespace.h"

// Usefull define for full iteration over a map structure

NAMESPACE_BEGIN(au)

/**
 Class to use map structures of <typedef,class*> with addittional function for easy manitpulation
 */

template <typename K, typename V, typename _Compare = std::less<K> >
class map : public std::map<K,V*,_Compare>
{
public:
    
    // Iterator definition
    typename std::map<K, V* >::iterator iter;

    // Insert a pair of elements ( easy method )
    // If a previous element was inserted with the same key, it is automatically deleted
    void insertInMap( K& key , V* value)
    {
        V* tmp =  extractFromMap( key );
        if( tmp )
            delete tmp;
        
        std::map<K, V*,_Compare >::insert( std::pair<K,V*>( key, value) );
    }

    // Insert a pair of elements ( easy method )
    // No check over previous 
    void insertNewInMap( K& key , V* value)
    {
        insert( std::pair<K,V*>( key, value) );
    }
    
    
    /*
     Function to easyly get pointers in a std::map < value , Pointer* >
     NULL if not found
     */
    
    V* findInMap( K& key ) 
    {
        typename std::map<K, V*,_Compare >::iterator iter = std::map<K, V*,_Compare >::find(key);
        
        if( iter == std::map<K,V*,_Compare>::end() )
            return NULL;
        return iter->second;
    }
    
    
    /**
     Function to easily get value for a key creating if necessary ( only used with simple constructor )
     */
    
    V* findOrCreate( K& key )
    {
        V* tmp = findInMap(key);
        if( !tmp )
        {
            tmp = new V();
            insertInMap(key, tmp);
        }
        return tmp;			
    }
    
    template <typename T>
    V* findOrCreate( K& key , T a )
    {
        V* tmp = findInMap(key);
        if( !tmp )
        {
            tmp = new V( a );
            insertInMap(key, tmp);
        }
        return tmp;			
    }
    
    
    
    
    /** 
     Function to remove a particular entry if exist
     Return if it really existed
     */
    
    bool removeInMap( K key ) 
    {
        typename std::map<K, V*,_Compare >::iterator iter = std::map<K,V*,_Compare>::find(key);
        
        if( iter == std::map<K,V*,_Compare>::end() )
            return false;
        else
        {
            delete iter->second;
            std::map<K,V*,_Compare>::erase( iter );
            return true;
        }
    }
    
    void removeInMap( std::set<K> &keys )
    {
        typename std::set<K>::iterator iter;
        for ( iter = keys.begin() ; iter != keys.end() ; iter++ )
            removeInMap(*iter);
        
    }
    
    // Remove all elements that satify a bool function
    // Returns the number of removed elements
    
    int removeInMapIfFinished()
    {
        typename std::map<K, V*,_Compare >::iterator iter;
        
        std::set<K> keys_to_remove;    
        
        for ( iter = std::map<K, V*,_Compare >::begin() ; iter != std::map<K, V*,_Compare >::end() ; iter++ )
            if (iter->second->isFinished() )
                keys_to_remove.insert( iter->first );
        
        typename std::set<K>::iterator k;
        
        for ( k = keys_to_remove.begin() ; k != keys_to_remove.end() ; k++ )
            removeInMap(*k);
        
        return keys_to_remove.size();
    }
    
    V* extractFromMap(  K key )
    {
        typename std::map<K, V*,_Compare >::iterator iter = std::map<K,V*,_Compare>::find(key);
        
        if( iter == std::map<K,V*,_Compare>::end() )
            return NULL;
        else
        {
            V* v = iter->second;
            std::map<K,V*,_Compare>::erase(iter);
            return v;
        }
        
    }		
    
    void clearMap()
    {
        typename std::map<K, V*,_Compare >::iterator iter;
        
        for (iter = std::map<K,V*,_Compare>::begin() ; iter != std::map<K,V*,_Compare>::end() ; iter++)
        {
            delete iter->second;
        }
        std::map<K, V* ,_Compare>::clear();
    }
    
    
    /* Funtion to get the keys */
    
    void getKeys( std::set<K>& childrens )
    {
        typename std::map<K, V*,_Compare >::iterator iter;
        for (iter = std::map<K,V*,_Compare>::begin() ; iter != std::map<K,V*,_Compare>::end() ; iter++)
            childrens.insert( iter->first );
    }

    std::vector<K> getKeysVector( )
    {
        std::vector<K> v;
        
        typename std::map<K, V*,_Compare >::iterator iter;
        for (iter = std::map<K,V*,_Compare>::begin() ; iter != std::map<K,V*,_Compare>::end() ; iter++)
            v.push_back( iter->first );
        return v;
    }
    
    
};



NAMESPACE_END

#endif
