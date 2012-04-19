
#ifndef _H_BUFFER_CONTAINER
#define _H_BUFFER_CONTAINER

#include "au/containers/list.h"

namespace engine {
    
    class Buffer;
    
    // Class holding a buffer generated by memory manager
    
    class BufferContainer
    {
        Buffer* buffer;
        
    public:

        // Constructor and destructors
        BufferContainer();
        ~BufferContainer();

        // Get and set the internal buffer ( retaining and releasing acordingly )
        void setBuffer( Buffer * b );
        Buffer* getBuffer();
        
        // Release internal buffer ( if any )
        void release();
        
        // Handy operation to point to the same buffer
        void operator=( BufferContainer& other );

        // Create a buffer and retain internally
        Buffer* create( std::string name , std::string type , size_t size );
        
    };
 
    
    class BufferListContainer
    {
        au::list<Buffer> buffers;      
        
    public:
        
        ~BufferListContainer()
        {
            clear(); // Clear vector releasing all included buffers
        }
        
        void push_back( Buffer* buffer )
        {
            buffer->retain();
            buffers.push_back(buffer);
        }

        void clear()
        {
            au::list<Buffer>::iterator it_buffers;      
            for( it_buffers = buffers.begin() ; it_buffers != buffers.end() ; it_buffers ++ )
                (*it_buffers)->release();
            buffers.clear();
        }
        
        Buffer* front()
        {
            return buffers.findFront();
        }
        
        void pop()
        {
            Buffer*buffer =  buffers.extractFront();
            if( buffer )
                buffer->release();
        }
        
        size_t getTotalSize()
        {
            size_t total = 0;
            au::list<Buffer>::iterator it; 
            for( it = buffers.begin() ; it != buffers.end() ; it++ )
                total += (*it)->getSize();
            return total;
        }

        


        
    };
    
}

#endif

