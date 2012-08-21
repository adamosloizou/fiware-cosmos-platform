
/* ****************************************************************************
*
* FILE            SamsonClient.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         Samson
*
* DATE            7/14/11
*
* DESCRIPTION
*
*
* COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
*
* ****************************************************************************/

#ifndef _H_Samson_SamsonClient
#define _H_Samson_SamsonClient

// We need to undef the macro "emit" because Qt library was setting it to empty string
#undef emit

#include <pthread.h>                /* pthread*/
#include <string>       // std::string
#include <vector>       // std::vector

#include "samson/common/ports.h"

#include "au/Cronometer.h"          // au::Cro
#include "au/Rate.h"                // au::rate::Rate
#include "au/containers/list.h"     // au::list
#include "au/containers/map.h"      // au::map
#include "au/mutex/Token.h"         // au::Token
#include "au/mutex/TokenTaker.h"    // au::TokenTaker

#include "engine/Buffer.h"          // engine::Buffer
#include "engine/MemoryManager.h"   // engine::MemoryManager
#include "engine/Object.h"          // engine::Object

#include "samson/common/coding.h"
#include "samson/delilah/Delilah.h"  // samson::Delilah
#include "samson/delilah/DelilahComponent.h"    // samson::DelilahComponent
#include "samson/network/DelilahNetwork.h"

#include "samson/module/ModulesManager.h"

#include "samson/client/BufferContainer.h"
#include "samson/client/SamsonClientBlock.h"
#include "samson/client/SamsonClientBlockInterface.h"


/*
 * Main class to connect to a samson cluster
 */

namespace  samson {
class SamsonClient : public DelilahLiveDataReceiverInterface {
  std::string connection_type_;                 // String to describe connection with SAMSON (pop, push, console, ...)

  samson::Delilah *delilah_;                    // Delilah client

  BufferContainer buffer_container_;            // Blocks of data received so far ( live data )

public:

  au::rate::Rate push_rate;                     // Statistics about rate
  au::rate::Rate pop_rate;                      // Statistics about rate

  // General static init ( Init engine )
  static void general_init(size_t memory = 1000000000, size_t load_buffer_size = 64000000);
  static void general_close();

  // Constructor and destructor
  SamsonClient(std::string connection_type);
  virtual ~SamsonClient();

  // Connect with a cluter
  bool connect(const std::vector<std::string>& hosts);
  bool connect(const std::string& host);

  // Check if connection is ready
  bool connection_ready();


  // Push content to a particular queue ( returns a push_id )
  size_t push(engine::BufferPointer buffer, const std::string& queue);
  size_t push(engine::BufferPointer buffer, const std::vector<std::string>& queues);
  size_t getNumPendingPushItems();              // Get number of operations we are waiting for...
  bool isFinishedPushingData();                 // Check if all operations are finished
  void waitFinishPushingData();                 // Wait until all operations are finished

  // Live data connection
  void connect_to_queue(std::string queue, bool flag_new, bool flag_remove);
  SamsonClientBlockInterface *getNextBlock(std::string queue);

  // Change the interface to receive live data from SAMSON
  void set_receiver_interface(DelilahLiveDataReceiverInterface *interface);

  // DelilahLiveDataReceiverInterface
  void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer);

  // Wait until everything is finished
  void waitUntilFinish();

private:

  void init(std::string connection_type, const std::vector<std::string>& hosts);
};
}
#endif  // ifndef _H_Samson_SamsonClient
