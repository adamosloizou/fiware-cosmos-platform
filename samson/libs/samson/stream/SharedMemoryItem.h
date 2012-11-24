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
/* ****************************************************************************
 *
 * FILE                     SharedMemoryItem.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_SHARED_MEMORY_ITEM
#define _H_SHARED_MEMORY_ITEM

#include <iostream>                          // std::cout
#include <list>                              // std::list
#include <map>                               // std::map
#include <set>                               // std::set
#include <stdio.h>                           // perror
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>


#include "au/containers/map.h"               // au::map
#include "au/mutex/Token.h"                  // au::Token
#include "au/string/StringUtilities.h"       // au::Format

#include "engine/Buffer.h"                   // samson::Buffer

// network::..
#include "samson/common/samsonDirectories.h"  // SAMSON_SETUP_FILE


namespace engine {
/*
 * SharedMemoryItem is a class that contains information about a region of memory shared between different processes
 * Memory manager singleton provides pointers to these objects
 */

class SharedMemoryItem {
public:

  int id;                                               /* Identifier of the shared memory area 0 .. N-1 */
  int shmid;                                            /* return value from shmget() */
  char *data;                                           /* Shared memory data */
  size_t size;                                          /* Information about the size of this shared memory item */

  SharedMemoryItem(int _id, int _shmid, size_t _size) {
    id = _id;
    shmid = _shmid;
    size = _size;

    // Attach to local-space memory
    data = (char *)shmat(shmid, 0, 0);
    if (data == (char *)-1) {
      if (errno == EMFILE) {
        LM_X(1,
             ("Error with shared memory while attaching to local memory, "
              "the number of shared memory segments has reached the system-wide limit.\n"
              "Use 'ipcs' to check the allocated shared memory segments."
              "(shmat error %d:%s)(shared memory id %d shmid %d size %lu)\n",
              errno, strerror(errno), id, shmid, size));
      } else {
        LM_X(1,
             ("Error with shared memory while attaching to local memory "
              "(shmat error %d:%s)(shared memory id %d shmid %d size %lu)\n",
              errno, strerror(errno), id, shmid, size));
      }
    }
  }

  ~SharedMemoryItem() {
    // Detach data if it was previously attached
    if (data) {
      if (shmdt(data) == -1) {
        LM_X(1, ("Error (%s) calling shmdt in destructor", strerror(errno)));
      }
    }
  }

  // --------------------------------------------------------------------------------
  // Interfaces to get SimpleBuffer elements in order to read or write to them
  // --------------------------------------------------------------------------------

  SimpleBuffer getSimpleBuffer() {
    return SimpleBuffer(data, size);
  }

  SimpleBuffer getSimpleBufferAtOffset(size_t offset) {
    return SimpleBuffer(data + offset, size - offset);
  }

  SimpleBuffer getSimpleBufferAtOffsetWithMaxSize(size_t offset, size_t _size) {
    if (_size > ( size - offset )) {
      LM_X(1, ("Error cheking size of a simple Buffer"));
    }
    return SimpleBuffer(data + offset, _size);
  }
};
};

#endif  // ifndef _H_SHARED_MEMORY_ITEM
