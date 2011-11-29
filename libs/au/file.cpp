
#include <sys/stat.h>	// stat(.)
#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <string.h>             // strchr
#include <sstream>              // std::ostringstream
#include <dirent.h>
#include <string>

#include "au/string.h"	 

#include "au/file.h"        // Own interface

namespace au
{
    
	size_t sizeOfFile( std::string fileName )
	{
		struct ::stat info;
		if( stat(fileName.c_str(), &info) == 0)
			return info.st_size;
		else
		{
			return 0;
		}
	}		
	
    bool isDirectory( std::string fileName ) 
    {
        struct stat buf;
        stat( fileName.c_str() , &buf );
        return S_ISDIR(buf.st_mode);
    }
    
    bool isRegularFile( std::string fileName ) 
    {
        struct stat buf;
        stat( fileName.c_str() , &buf );
        return S_ISREG(buf.st_mode);
    }
    
    void removeDirectory( std::string fileName , au::ErrorManager & error ) 
    {        
        if( isRegularFile( fileName ) )
        {
            // Just remove
            int s = remove( fileName.c_str() );
            if (s != 0)
                error.set( "Not possible to remove file " + fileName  );
            return;
        }
        
        // Navigate in the directory
        if( !isDirectory( fileName ) )
        {
            error.set( au::str("%s is not a directory or a file", fileName.c_str() ) );
            return;
        }
        
        // first off, we need to create a pointer to a directory
        DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
        struct dirent *pent = NULL;
        if (pdir != NULL) // if pdir wasn't initialised correctly
        {
            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                if (pent != NULL)
                {                    
                    if( strcmp( "." , pent->d_name )==0 )
                        continue;
                    if( strcmp( ".." , pent->d_name )==0 )
                        continue;
                    
                    std::ostringstream localFileName;
                    localFileName << fileName << "/" << pent->d_name;
                    
                    removeDirectory( localFileName.str() , error );
                    
                }
            // finally, let's close the directory
            closedir (pdir);						
        }
        
        // Remove the directory properly
        int s = rmdir(fileName.c_str());
        if (s != 0)
            error.set( au::str("Not possible to remove directory %s" , fileName.c_str() ) );
        
        
    }
    
    bool check_string_ends_with( std::string txt , std::string end_txt )
    {
        
        if( txt.length() < end_txt.length() )
            return false;
        
        for ( size_t i=0 ; i < end_txt.length() ; i++ )
            if ( end_txt[ end_txt.length() - 1 - i] != txt[ txt.length() - 1 - i ] )
                return false;
        return true;
    }
    
    std::string path_remove_last_component( std::string path )
    {
        size_t pos = path.find_last_of( '/' );
        
        if( pos == std::string::npos )
        {
            return "";
        }
        else
        {
            if( pos == 0 )
                return "/";
            
            return path.substr( 0 , pos );
        }
        
    }
    
    std::string get_directory_from_path( std::string path )
    {
        if ( ( path == "" ) || ( path == "." ) || ( path == "./" ) )
            return "./";
        
        if ( path == "/" )
            return path;
        
        
        // Remove the last part of the path
        size_t pos = path.find_last_of( '/' );
        
        if ( path[0] == '/' )
        {
            if( pos != 0 )
                return path.substr( 0 , pos+1 );
            else
                return "/";
        }
        
        if ( path[0] == '.' )
        {
            if( pos == std::string::npos )
                return "./";
            else
                return path.substr( 0 , pos+1 );
        }
        
        if( pos == std::string::npos )
            return "./";
        else
            return "./" + path.substr( 0 , pos+1 );
        
    }

    
}
