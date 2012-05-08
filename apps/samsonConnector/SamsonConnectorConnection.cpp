
#include "au/ThreadManager.h"

#include "SamsonConnector.h"
#include "BufferProcessor.h"
#include "SamsonConnectorConnection.h" // Own interface


extern size_t buffer_size;
extern size_t input_buffer_size; // Size of the chunks to read

namespace samson {
    namespace connector {
        
        void* run_FileDescriptorConnection( void* p )
        {
            // Recover the correct pointer
            FileDescriptorConnection* connection = ( FileDescriptorConnection* ) p;
            
            // Main run
            connection->run();
            
            return NULL;
            
        }
        
        FileDescriptorConnection::FileDescriptorConnection( Item  * _item , ConnectionType _type , std::string _name
                                                           , au::FileDescriptor * _file_descriptor )
        : Connection( _item , _type , _name )
        {
            // Keep pointer to file descriptor
            file_descriptor = _file_descriptor;
            
            // Create the thread
            thread_running = true;
            pthread_t t;
            au::ThreadManager::shared()->addThread("SamsonConnectorConnection",&t, NULL, run_FileDescriptorConnection, this);
        }
        
        FileDescriptorConnection::~FileDescriptorConnection()
        {
            if( file_descriptor )
                delete file_descriptor;
        }
        
        void FileDescriptorConnection::run_as_output()
        {
            while( true )
            {
                // Container to keep a retained version of buffer
                engine::BufferContainer container;
                getNextBufferToSent(&container);
                
                engine::Buffer* buffer = container.getBuffer();
                if( buffer )
                {
                    au::Status s = file_descriptor->partWrite(buffer->getData(), buffer->getSize(), "samsonConnectorConnection");
                    
                    if( s != au::OK )
                        return; // Just quit
                }
                else
                {
                    // Sleep a little bit before checking again
                    usleep( 100000 );
                }
            }        
        }
        
        void FileDescriptorConnection::run_as_input()
        {
            // Read from stdin and push blocks to the samson_connector
            while( true )
            {
                //Get a buffer
                engine::Buffer * buffer = engine::MemoryManager::shared()->createBuffer("stdin", "connector", input_buffer_size );
                
                //LM_V(("%s: Reading buffer up to %s" , file_descriptor->getName().c_str() , au::str(buffer_size).c_str() ));
                
                // Read the entire buffer
                size_t read_size = 0;
                au::Status s = file_descriptor->partRead(buffer->getData()
                                                         , input_buffer_size
                                                         , "read connector connections"
                                                         , 300 
                                                         , &read_size );
                
                // If we have read something...
                if( read_size > 0 )
                {
                    // Push input buffer
                    buffer->setSize(read_size);
                    pushInputBuffer( buffer );
                }
                
                // Relase allocated buffer
                buffer->release();
                
                // If last read is not ok...
                if( s != au::OK )
                {
                    // Flush whatever we have..
                    flushInputBuffers();
                    
                    // Quit main threads
                    return;
                }
            }
        }
        
        void FileDescriptorConnection::run()
        {
            if( getType() == connection_input )
                run_as_input();
            else
                run_as_output();
            
            // Mark as non thread_running
            thread_running = false;
            
        }
    }
}