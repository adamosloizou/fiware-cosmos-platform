#include "engine/DiskOperation.h"       // Own interface
#include "logMsg/logMsg.h"              // LM_T
#include "logMsg/traceLevels.h"         // LmtDisk
#include "engine/Engine.h"              // engine::Engine
#include "engine/DiskManager.h"         // engine::DiskManager
#include "engine/ReadFile.h"            // engine::ReadFile

NAMESPACE_BEGIN(engine)


DiskOperation::DiskOperation( )
{
    // Some default values
    buffer = NULL;
    read_buffer = NULL;
    
}

DiskOperation::~DiskOperation()
{
}


DiskOperation* DiskOperation::newReadOperation( char *data , std::string fileName , size_t offset , size_t size , size_t _listenerId  )
{
    
    DiskOperation *o = new DiskOperation();
    
    o->fileName = fileName;
    o->type = DiskOperation::read;
    o->read_buffer = data;
    o->size = size;
    o->offset = offset;
    o->setDevice();
    o->addListener( _listenerId );
    
    o->environment.set("type","read");
    
    return o;
}

DiskOperation * DiskOperation::newReadOperation( std::string fileName , size_t offset , size_t size ,  SimpleBuffer simpleBuffer , size_t _listenerId)
{
    
    DiskOperation *o = new DiskOperation();
    
    o->fileName = fileName;
    o->type = DiskOperation::read;
    o->read_buffer = simpleBuffer.getData();
    o->size = size;
    o->offset = offset;
    o->setDevice();
    o->addListener( _listenerId );
    
    o->environment.set("type","read");
    
    return o;
}



DiskOperation* DiskOperation::newWriteOperation( Buffer* buffer ,  std::string fileName , size_t _listenerId )
{
    DiskOperation *o = new DiskOperation();
    
    o->fileName = fileName;
    o->type = DiskOperation::write;
    o->buffer = buffer;
    o->size = buffer->getSize();
    o->offset = 0;
    o->setDevice();
    o->addListener( _listenerId );
    
    o->environment.set("type","write");
    
    return o;
}


DiskOperation* DiskOperation::newAppendOperation( Buffer* buffer ,  std::string fileName, size_t _listenerId )
{
    DiskOperation *o = new DiskOperation();
    
    o->fileName = fileName;
    o->type = DiskOperation::append;
    o->buffer = buffer;
    o->size = buffer->getSize();
    o->offset = 0;
    o->setDevice();
    o->addListener( _listenerId );
    
    o->environment.set("type","append");
    
    return o;
}

DiskOperation* DiskOperation::newRemoveOperation( std::string fileName , size_t _listenerId )
{
    DiskOperation *o = new DiskOperation();
    
    o->fileName = fileName;
    o->type = DiskOperation::remove;
    o->setDevice();
    o->size = 0;
    o->addListener( _listenerId );
    
    o->environment.set("type","remove");
    
    return o;
}


std::string DiskOperation::directoryPath( std::string path )
{
    size_t pos = path.find_last_of("/");
    
    if ( pos == std::string::npos)
        return ".";
    else
        return path.substr( 0 , pos );
}

bool DiskOperation::setDevice( )
{
    // Make sure we only use one device
    st_dev = 0;
    return true;
    
    
    struct ::stat info;
    stat(fileName.c_str(), &info);
    
    if( S_ISREG(info.st_mode) )
    {
        st_dev = info.st_dev;   // Get the devide of the inode 
        return true;
    }
    
    // File currently does not exist ( it wil be writed )
    if( type != DiskOperation::write )
        return false;
    
    // Get the path of the directory
    std::string directory = directoryPath( fileName );
    stat(directory.c_str(), &info);
    
    if( S_ISDIR(info.st_mode) )
    {
        st_dev = info.st_dev;   // Get the devide of the inode 
        st_dev = 0;// Force all files to be process as the same inode
        return true;
    }
    
    // Something wrong...
    return false;
}


std::string DiskOperation::getDescription()
{
    std::ostringstream o;
    
    switch (type) {
        case write:
            o << "Write to file: '" << fileName << "' Size:" << au::str(size,"B");
            break;
        case append:
            o << "Append to file: '" << fileName << "' Size:" << au::str(size,"B");
            break;
        case read:
            o << "Read from file: '" << fileName << "' Size:" << au::str(size,"B") << " ["<< size << "B] Offset:" << offset;
            break;
        case remove:
            o << "Remove file: '" << fileName << "'";
            break;
    }
    
    
    return o.str();
}

std::string DiskOperation::getShortDescription()
{
    std::ostringstream o;
    
    switch (type) {
        case write:
            o << "W:" << au::str( size );
            break;
        case append:
            o << "A:" << au::str( size );
            break;
        case read:
            o << "R:" << au::str( size );
            break;
        case remove:
            o << "X";
            break;
    }
    
    
    return o.str();
}

void DiskOperation::setError( std::string message )
{
    
    std::ostringstream o;
    o << message << " ( " << getDescription() << " )";
    error.set( o.str() );
}


void DiskOperation::run(  )
{
    // Detect some slow disk access if rate is going bellow 10Mb/s in large operations
    //double alarm_time_secs = std::max(  (double) size / 10000000.0 , 5.0 );
    //au::ExecesiveTimeAlarm alarm( au::str("Disk Operation '%s;",getDescription().c_str() , alarm_time_secs ) );
    
    LM_T( LmtDisk , ("DiskManager: Running operation %s", getDescription().c_str() ));
    
    if( type == DiskOperation::write )
    {
        // Create a new file
        
        
        LM_T( LmtDisk , ("DiskManager: Opening file %s to write", fileName.c_str() ));
        FILE *file = fopen( fileName.c_str() , "w" );
        if ( !file )
        {
            LM_E(("Error opening file for writing, fileName:%s, errno:%d", fileName.c_str(), errno));
            setError("Error opening file");
        }
        else
        {
            if( size > 0 )
            {
                if( fwrite(buffer->getData(), size, 1 , file) == 1 )
                    fflush(file);
                else
                {
                    LM_E(("Error writing data to file, fileName:%s, errno:%d", fileName.c_str(), errno));
                    setError("Error writing data to the file");
                }
            }
            
            fclose(file);
        }
    }
    
    if( type == DiskOperation::append )
    {
        // Create a new file
        
        
        LM_T( LmtDisk , ("DiskManager: Opening file %s to append", fileName.c_str() ));
        FILE *file = fopen( fileName.c_str() , "a" );
        if ( !file )
            setError("Error opening file");
        else
        {
            if( size > 0 )
            {
                if( fwrite(buffer->getData(), size, 1 , file) == 1 )
                    fflush(file);
                else
                    setError("Error writing data to the file");
            }
            
            fclose(file);
        }
    }
    
    
    if( type == DiskOperation::read )
    {
        LM_T( LmtDisk , ("DiskManager: Opening file %s to read", fileName.c_str() ));
        
        // Get the Read file from the Manager
        ReadFile *rf = diskManager->fileManager.getReadFile( fileName );
        
        if( !rf->isValid() )
        {
            LM_E(("Internal error: Not valid read file %s" , fileName.c_str()));
            setError( "Internal error: Not valid read file" );
        }
        else
        {
            if( rf->seek( offset ) )
            {
                LM_E(("Error while seeking data from file %s" , fileName.c_str()));
                setError( au::str("Error while seeking data from file %s" , fileName.c_str()));
            }
            
            
            if( rf->read(read_buffer, size ) )
            {
                LM_E(("Error while reading data from file %s" , fileName.c_str()));
                setError( au::str("Error while reading data from file %s" , fileName.c_str()));
            }
            
        }
        
        // When to close rf ?
        rf->close();

#if 0
        LM_T( LmtDisk , ("DiskManager: Opening file %s to read", fileName.c_str() ));
         
        FILE *file = fopen(fileName.c_str() , "r" );
        if( !file )
            setError("Error opening file");
        else
        {
            if( fseek(file, offset, SEEK_SET) != 0)
                setError("Error in fseek operation");
            else
            {
                if(size > 0 )
                {
                    if ( fread(read_buffer, size, 1, file) == 1 )
                    {
                        gettimeofday(&stop, NULL);
                        LM_TODO(("Fix statistics using Engine"));
                        operation_time = DiskStatistics::timevaldiff( &start , &stop);
                    }
                    else
                        setError("Error while reading data from file");
                }
                else
                    operation_time = 0;
            }
            
            fclose(file);
        }
#endif
    }
    
    if( type == DiskOperation::remove)
    {
        
        LM_T( LmtDisk , ("DiskManager: Removing file %s", fileName.c_str() ));
        
        // Remove the file
        int c = ::remove( fileName.c_str() );
        if( c != 0 )
            setError("Error while removing file");
    }
    
    
    // If write operation, destroy the buffer ( only in write and append operations )
    if( environment.get( destroy_buffer_after_write , "yes") == "yes" )
        destroyBuffer();
    
    LM_T( LmtDisk , ("DiskManager: Finished with file %s, ready to finishDiskOperation", fileName.c_str() ));
    // Notify to the engine
    diskManager->finishDiskOperation( this );
}

void DiskOperation::destroyBuffer()
{
    if( buffer )
    {
        engine::MemoryManager::shared()->destroyBuffer(buffer);
        buffer = NULL;
    }
}

bool DiskOperation::compare( DiskOperation *operation )
{
    // Priority to append and write operations
    if( ( type == append ) || ( type == write ) )
        if( ( operation->type != append ) && ( operation->type != write ) )
            return true;
    
    return false;
}

void DiskOperation::getInfo( std::ostringstream& output)
{
    output << "<disk_operation>\n";
    
    output << "<type>";
    switch (type) {
        case read:
            output << "read";
            break;
        case write:
            output << "write";
            break;
        case append:
            output << "append";
            break;
        case remove:
            output << "remove";
            break;
            
        default:
            break;
    }
    output << "</type>\n";
    
    output << "<file_name>" << fileName << "</file_name>\n";
    
    output << "<size>" << size << "</size>\n";
    output << "<offset>" << offset << "</offset>\n";
    
    output << "</disk_operation>\n";
}

NAMESPACE_END
