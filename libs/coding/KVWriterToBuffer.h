#ifndef _H_KVWriterToBuffer
#define _H_KVWriterToBuffer

/* ****************************************************************************
*
* FILE                 KVWriterToBuffer.h - 
*
*/
#include <iostream>					// std::cout
#include <vector>					// std::vector
#include <set>						// std::set

#include "coding.h"					// ss_hg, ss_hg_size, etc..
#include "samson/KVWriter.h"		// KVWriter
#include "Lock.h"					// au::Lock
#include "BufferSinkInterface.h"	// ss::BufferSinkInterface
#include "MemoryManager.h"			// ss::MemoryManager

/*

namespace ss {

	
	
	typedef struct
	{
		size_t offset;		// Position in the buffer where this key-value is stored
		unsigned int size;	// Size of the key-value in bytes
		ss_hg hashGroup;	// HashGroup of the key value
	} KVHashGroup;

	bool KVHashGroupSorter(KVHashGroup *a, KVHashGroup *b);

	class KVWriterGeneral;
	class KVWriterToBuffer : public KVWriter
	{
		KVWriterGeneral* generalWriter;		// Pointer to the general writing
		int server;							// Identifier of the server that corresponds to me
		
		// Internal buffer to store key-values
		char * _data;
		size_t _size;
		
		// Vector of generated key-values
		KVHashGroup* _kvs;
		size_t _num_kvs;
		
		// Vector of pointers of the structures
		KVHashGroup** __kvs;
		
		char *_tmp_data;
		
		
	public:
		
		KVWriterToBuffer( KVWriterGeneral *_generalWriter  , int _server )
		{
			generalWriter = _generalWriter;
			server = _server;
			
			// Init the buffer data
			_size = 0;
			_data = (char*) malloc(KV_BUFFER_SIZE);
			
			// Init the kvs buffer
			_kvs = (KVHashGroup*) malloc(KV_BUFFER_MAX_NUM_KVS *sizeof(KVHashGroup) );
			_num_kvs = 0;
			
			// Init the vector of pointsd
			__kvs = (KVHashGroup**) malloc(KV_BUFFER_MAX_NUM_KVS * sizeof(KVHashGroup*) );
			
			// Init the tmp buffer to serialize key-values
			_tmp_data = (char*) malloc( KV_MAX_SIZE );
		}
		
		~KVWriterToBuffer()
		{
			free( _data );
			free( _tmp_data );
		}
		
		virtual void emit(DataInstance* key, DataInstance* value)
		{
			size_t key_size = key->serialize(_tmp_data);
			size_t value_size = value->serialize(_tmp_data+key_size);
			size_t total_size = key_size + value_size;
			
			if( (_size + total_size) > KV_BUFFER_SIZE )
				processBuffer();
			
			// Store the new key-value
			_kvs[_num_kvs].offset = _size;
			_kvs[_num_kvs].size = total_size;
			_kvs[_num_kvs].hashGroup = key->hash(KV_NUM_HASHGROUPS);
			__kvs[_num_kvs] = &_kvs[_num_kvs];
			_num_kvs++;
			
			// Copy the new key-value to the buffer
			memcpy(_data+_size, _tmp_data, total_size);
			_size += total_size;
			
			
			if( _num_kvs >= KV_BUFFER_MAX_NUM_KVS)
				processBuffer();
			
		}
		
		void processBuffer();
		
		virtual void close()
		{
			// Process the last piece of data ( if any )
			if( _size > 0 )
				processBuffer();
		}
		
	};
	
	

	
	class KVWriterGeneral : public KVWriter
	{
		int num_servers;
		std::vector<KVWriterToBuffer*> buffers;		// Real buffers

		// Buffers of data generated pendign to be removed
		std::set<Buffer*> generatedBuffers;	
		
		au::Lock lock;

		
		BufferSinkInterface* _interface;
		
	public:

		KVWriterGeneral( int _num_servers , BufferSinkInterface* interface )
		{
			_interface = interface;
			
			num_servers = _num_servers;
			for (int i = 0 ; i < num_servers ; i++)
				buffers.push_back( new KVWriterToBuffer( this , i ) );
		}

		
		virtual void emit(DataInstance* key, DataInstance* value)
		{
			int server = key->hash( num_servers );
			buffers[server]->emit( key , value );
		}
		
		virtual void close()
		{
			for (int i = 0 ; i < num_servers ; i++)
				buffers[i]->close();
		}
		
		
		
		void processBuffer( Buffer *b )
		{
			std::cout << "Global writer receive a packet for server " << b->server << " with " << b->getSize() << " bytes\n";

			if( _interface )
			{
				lock.lock();
				_interface->newBuffer( b );				
				generatedBuffers.insert( b );
				lock.unlock();
			}
		}
		
		
		void removeGeneratedBuffer( Buffer *b )
		{
			lock.lock();
			
			std::set<Buffer*>::iterator i = generatedBuffers.find(b);
			assert( i != generatedBuffers.end() );
			generatedBuffers.erase( i );
			MemoryManager::shared()->destroy( b );
			
			lock.unlock();
			
			
		}
		
		
	};
	
}
*/
#endif
