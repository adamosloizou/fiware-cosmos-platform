/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include <map>
#include <string>

#ifndef _H_AU_STRING_COLLECTION
#define _H_AU_STRING_COLLECTION

namespace au {
/*
 * Compressed version of a collection of strings.
 * Added strings are serialized to a common vector.
 * When a string is added, the position inside the global vector is returned
 * Duplicated strings returns the same position
 *
 *
 * This class has two working modes: generating & reading
 *
 * generating:
 *    - Add strings using add() call.
 *    - Keep the returned index fro future recoveries of this string
 *    - Use write() call to put everything into a file
 *
 * working:
 *    - Recover data with read call\
 *    - Use get() call to get all required strings
 *
 */

class StringCollection {

public:


  // Map of previously added strings to avoid duplications
  std::map<std::string, int> previous_strings;

  // Constructor
  StringCollection();

  // Destructor
  ~StringCollection();

  // Add string ( cause reallocation if required )
  int Add(const char *string);

  // Read and write from disk
  size_t Write(FILE *file);
  void Read(FILE *file, size_t _size);

  // Get used size on disk/memory
  size_t GetSize();

  // Recover string at this position ( returned when added )
  const char *Get(int pos);
  
private:
  
  // Global pointer with all serialized strings
  char *v_;
  // Current size of the vector
  size_t size_;
  // Current malloc size of the vector
  size_t max_size_;

  
};
}

#endif  // ifndef _H_AU_STRING_COLLECTION
