

#include <sys/stat.h>

#include "samson/module/ModulesManager.h"           // samson::ModulesManager

#include "SamsonFile.h"                             // Own interface

namespace samson {
    
    SamsonFile::SamsonFile( std::string _fileName )
    {
        fileName = _fileName;
        file = fopen( fileName.c_str() , "r" );        
        
        if( !file )
        {
            error.set("Not possible to open file");
            return;
        }
        
        // Read header
        int nb = fread(&header, 1, sizeof(samson::KVHeader), file);
        
        if (nb != sizeof(samson::KVHeader))
            error.set(au::str("Getting header: read only %d bytes (wanted to read %ld)\n", nb, (long int) sizeof(samson::KVHeader)));
        
        if (!header.check())
        {
            error.set("Wrong magic number in header");
            return;
        }
        
        
        // Check file-size
        
        // Check the lenght of the file
        struct stat filestatus;
        stat( fileName.c_str() , &filestatus );
        
        if ( header.getKVFormat().isTxt() )
        {
            size_t expected_size =   (size_t)( sizeof(samson::KVHeader)  + header.info.size );
            
            if( (size_t)filestatus.st_size != expected_size)
            {
                std::ostringstream message;
                message << "Wrong file length\n";
                message << "Expected:";
                message << " Header: " << sizeof(samson::KVHeader);
                message << " + Data: " <<  header.info.size;
                message << " = " << expected_size << "\n";
                message << "File size: " << filestatus.st_size << " bytes\n";
                
                error.set( message.str() );
                
                std::cout << header.str() << "\n";
                
                return;
            }
        }
        else
        {
            size_t expected_size =   (size_t)( sizeof(samson::KVHeader) + header.info.size ) ;            
            
            if( (size_t)filestatus.st_size != expected_size)
            {
                std::ostringstream message;
                message << "Wrong file length\n";
                message << "Expected:";
                message << " Header: " << sizeof(samson::KVHeader);
                message << " + Data: " <<  header.info.size;
                message << " = " << expected_size << "\n";
                message << "File size: " << filestatus.st_size << " bytes\n";
                
                error.set( message.str() );
                return;
            }
        }
        
        // Get format
        format = header.getKVFormat(); 
    }
    
    SamsonFile::~SamsonFile()
    {
        if( file )
            fclose( file );
    }
    
    bool SamsonFile::hasError()
    {
        return ( error.isActivated() );
    }
    
    std::string SamsonFile::getErrorMessage()
    {
        return error.getMessage();
    }
    
    class SimpleBuffer
    {
        
    public:
        
        char* data;
        
        SimpleBuffer( std::string fileName )
        {
            // In case of error
            data = NULL;
            
            struct stat filestatus;
            if( stat( fileName.c_str() , &filestatus ) != 0 )
                return;            
            
            FILE *file = fopen( fileName.c_str() , "r" );    
            
            data = (char*) malloc(filestatus.st_size);
            
            fread(data, filestatus.st_size, 1, file);
            fclose(file);
            
        }
        
        SimpleBuffer( size_t size )
        {
            if( data )
                data = ( char* ) malloc( size );
        }
        
        ~SimpleBuffer()
        {
            free( data );
        }
        
    };

    
    std::string SamsonFile::getHashGroups()
    {
        SimpleBuffer simple_buffer( fileName );
        
        au::ErrorManager error;
        KVInfo* info = createKVInfoVector( simple_buffer.data , &error );

        if( !info)
            return au::str( "Error getting vector for hashgroups. (%s)" , error.getMessage().c_str() );;
        
        std::ostringstream output;
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
            output << i << " " << info[i].kvs << " " << info[i].size << "\n";

        free( info );
        
        return output.str();
    }
    
    
    
    size_t SamsonFile::printContent( size_t limit , std::ostream &output )
    {
        // Number of records displayed    
        size_t records = 0 ;
        
        if( error.isActivated() )
            return records;
        
        if ( header.getKVFormat().isTxt() )
        {
            // txt content
            char buffer[1025];
            
            while( fgets(buffer, 1024, file) )
            {
                output << buffer;
                records++;

                if( limit > 0 )
                    if ( records >= limit )
                        return records;
            }
            return records;
        }
        
        samson::ModulesManager* modulesManager = samson::ModulesManager::shared();
        samson::Data *keyData = modulesManager->getData(format.keyFormat);
        samson::Data *valueData = modulesManager->getData(format.valueFormat);
        
        if(!keyData )
        {
            output << "Data format " << format.keyFormat << " not supported\n";
            return records;
        }
        
        if(!valueData )
        {
            output << "Data format " << format.valueFormat << " not supported\n";
            return records;
        }
        
        samson::DataInstance *key = (samson::DataInstance *)keyData->getInstance();
        samson::DataInstance *value = (samson::DataInstance *)valueData->getInstance();
        
        
        // Load file in memory..
        SimpleBuffer file_buffer( fileName );
        char* data = file_buffer.data;
        
        size_t offset = sizeof(KVHeader);
        for ( int i = 0 ; i < (int)header.info.kvs ; i++ )
        {
            size_t key_size = key->parse(data+offset);
            offset += key_size;
            
            size_t value_size = value->parse(data+offset);
            offset += value_size;

            output << key->str() << " " << value->str() << std::endl;
            
            records++;
            if( limit > 0)
                if( records >= limit )
                    return records;
        }
        
        return records;
        
    }
    
}
