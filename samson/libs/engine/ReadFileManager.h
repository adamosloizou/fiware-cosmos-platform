

/* ****************************************************************************
*
* FILE            ReadFileManager
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* Collection of "ReadFile"s. All the already open files to be access in read mode.
* It is an internal component of the DiskManager
*
* ****************************************************************************/
#ifndef _H_ENGINE_READ_FILE_MANAGER
#define _H_ENGINE_READ_FILE_MANAGER


#include <string>           // std::string

#include "au/containers/ListMap.h"     // au::ListMap


namespace engine {
class ReadFile;

class ReadFileManager {

public:

  ReadFileManager();
  ~ReadFileManager();

  ReadFile *GetReadFile( const std::string& file_name );

private:
  
  au::ListMap<std::string, ReadFile> read_files_;
  int max_open_files_;

};
}

#endif  // ifndef _H_ENGINE_READ_FILE_MANAGER
