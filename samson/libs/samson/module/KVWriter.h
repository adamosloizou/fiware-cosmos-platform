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
#ifndef KV_WRITER_H
#define KV_WRITER_H

#include "samson/module/DataInstance.h"        /* DataInstance                             */
#include <string>

namespace samson {
    

	class KVWriter
	{
        
        
	public:

		// Emit key values to a particular output
		virtual void emit( int output , DataInstance *key , DataInstance *value )=0;
		
		//virtual void emit(DataInstance* key, DataInstance* value) = 0;
		//virtual void close()=0;
		virtual ~KVWriter(){}
	};
	
	
	class TXTWriter
	{
	public:
        
		virtual ~TXTWriter(){}
        
        
		virtual void emit( const char * data , size_t length)=0;
		void emit( std::string data )
		{
			emit(data.c_str(), data.length());
		}
	};
	
}

#endif
