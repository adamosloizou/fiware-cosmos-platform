#ifndef _H_TXT_FILE_SET
#define _H_TXT_FILE_SET

#include "au/file.h"

namespace samson {
class Delilah;
class Buffer;



class DataSource {
public:

  virtual ~DataSource() {
  }

  virtual bool isFinish() = 0;
  virtual int fill(engine::BufferPointer b) = 0;
  virtual size_t getTotalSize() = 0;
  virtual std::string get_name() = 0;
};

class TXTFileSet : public DataSource {
  std::vector<std::string> fileNames;               // List of files to read
  std::ifstream inputStream;                        // Stream of the file we are reading

  int file;
  bool finish;

  char *previousBuffer;                             // Part of the previous buffer waiting for the next read
  size_t previousBufferSize;

  std::vector<std::string> failedFiles;             // List of files that could not be uploaded

public:

  TXTFileSet(std::vector<std::string> &_fileNames) {
    fileNames = _fileNames;

    previousBufferSize = 0;
    previousBuffer = NULL;

    finish = false;
    file = 0;


    openNextFile();
  }

  virtual std::string get_name() {
    if (fileNames.size() == 1) {
      return au::str("FILE/DIR %s", fileNames[0].c_str());
    } else {
      return au::str("%s FILEs/DIRs", fileNames.size());
    }
  }

  void openNextFile() {
    while (!inputStream.is_open()) {
      if (file >= (int)fileNames.size()) {
        finish = true;
        return;
      }

      inputStream.open(fileNames[file].c_str());
      if (!inputStream.is_open()) {
        failedFiles.push_back(fileNames[file]);                                         // Add to the list of failed files
      }
      file++;
    }
  }

  bool isFinish() {
    return finish;
  }

  // Read as much as possible breaking in lines
  int fill(engine::BufferPointer b);

  std::vector<std::string> getFailedFiles() {
    return failedFiles;
  }

  std::string getStatus() {
    std::ostringstream output;

    output << "TXTFile: ";
    if (finish) {
      output << " finished";
    } else {
      output << " running: " << fileNames.size() << " pending files and " << failedFiles.size() << " failed files";
    }
    return output.str();
  }

  size_t getTotalSize() {
    size_t totalSize = 0;

    // Compute the total size for all the files
    for (size_t i =  0; i < fileNames.size(); i++) {
      totalSize += au::sizeOfFile(fileNames[i]);
    }

    return totalSize;
  }
};
}

#endif  // ifndef _H_TXT_FILE_SET
