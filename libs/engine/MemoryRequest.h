/* ****************************************************************************
 *
 * FILE            MemoryRequest
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Request to be delivered to MemoryManager.
 * It is used where we want to allocate a buffer of memory when available.
 *
 * ****************************************************************************/

#ifndef _H_MEMORY_REQUEST
#define _H_MEMORY_REQUEST


#include <cstring>
#include "engine/Object.h"      // engine::Object

namespace engine {

    /**
     Information about a particular request
     */

    class Buffer;
    
    class MemoryRequest : public Object
    {
        
    public:
        
        // Size of the memory requets ( extracted from environment )
        size_t size;
        
        // Listner to notify
        size_t listner_id;

        // Buffer alocated for this
        Buffer *buffer;

        // Basic constructor
        MemoryRequest( size_t _size , size_t _listener_id );

        // Get information in xml mode
        void getInfo( std::ostringstream& output);

        
    };    
    
	
}

#endif
