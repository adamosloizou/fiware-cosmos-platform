/* ****************************************************************************
 *
 * FILE                     DiskOperation.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

/*
 
 Note:
 
 DiskOperation is the class that contains all the information for a particular operation on disk
 
 It should contain the name of the file, the size, the offset, etc. ( basically all the information needed for the operation )
 It shouls also contain the buffer where data is writed or readed from
 It finally contains a engine id of the object that should be notified when finished
 
 */


#ifndef _H_DISK_OPERATION
#define _H_DISK_OPERATION

#include <stdio.h>			// fopen , fclose, ...
#include <string>			// std::string
#include <map>				// std::map
#include <set>				// std::set
#include <list>				// std::list
#include <pthread.h>        /* pthread_mutex_init, ...                  */
#include <sys/stat.h>		// stat(.)
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <time.h>			// clock(.)
#include <set>           // std::set

#include "au/string.h"			// au::Format
#include "au/ErrorManager.h"			// samson::Error
#include "au/Environment.h"     // au::Environment
#include "au/namespace.h"

#include "engine/MemoryManager.h"       // engine::BufferContainer
#include "engine/Buffer.h"              // engine::Buffer
#include "engine/BufferContainer.h"
#include "au/Object.h" 


NAMESPACE_BEGIN(engine)

class DiskManager;

class DiskOperation : public au::Object
{
    
public:
    
    au::Environment environment;    // Environment properties
    
    typedef enum
    {
        read,
        write,
        append,
        remove
    } DiskOperationType;		
    
private:
    
    DiskOperationType type;				// Type of operation ( read, write , remove , etc.. )
    std::string fileName;				// FileName to open
    
    engine::BufferContainer buffer_container;    // Auto-retained pointer to a buffer
    
    char *read_buffer;					// Buffer used when reading from disk	
    size_t size;						// Size to read/write
    size_t offset;						// Offset inside the file ( only for read operations )
    
    std::set<size_t> listeners;      // Collection of Ids of the listener to notify when operation is completed
    
    friend class DiskManagerNotification;
    friend class DiskManager;
	
    pthread_t t;						// Background thread to run the operation
    
    DiskOperation( );
    
public:
    
    DiskManager *diskManager;           // Pointer to the disk manager to notify
    
    ~DiskOperation();
    
    //int tag;                          // General tag to identify the operation
    
    au::ErrorManager error;				// Management of the error during this operation
    
    // Constructors used to create Disk Operations ( to be submitted to Engine )
    
    static DiskOperation* newReadOperation( char *data , std::string fileName , size_t offset , size_t size , size_t _listenerId  );
    static DiskOperation* newWriteOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
    static DiskOperation* newAppendOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId  );
    static DiskOperation* newRemoveOperation( std::string fileName, size_t _listenerId );
    
    static DiskOperation * newReadOperation( std::string _fileName , size_t _offset , size_t _size ,  SimpleBuffer simpleBuffer , size_t _listenerId );
    
    
    void setError( std::string message );
    
    std::string getDescription();
    std::string getShortDescription();
    
    DiskOperationType getType()
    {
        return type;
    }
    
    size_t getSize()
    {
        return size;
    }
    
public:
    
    // Run the operation
    void run();			
    
private:
    
    friend class DiskOperationGroup;
    
    size_t id;
    void setId( size_t _id)
    {
        id = _id;
    }
    
    size_t getId()
    {
        return id;
    }
    
public:
    
    bool compare( DiskOperation *operation );
    
    void addListener( size_t id )
    {
        listeners.insert( id );
    }
    
public:
    
    void getInfo( std::ostringstream& output);
    
    
};

NAMESPACE_END

#endif
