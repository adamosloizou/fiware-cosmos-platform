#ifndef _H_POP_DELILAH_COMPONENT
#define _H_POP_DELILAH_COMPONENT

#include <fstream>// ifstream
#include <vector>// std::vector
#include <set>// std::vector
#include <iostream>// std::cout
#include <sstream>// std::ostringstream
#include <list>// std::list

#include "au/string.h"// au::Format
#include "au/ErrorManager.h"// au::ErrorManager
#include "au/Cronometer.h"      // au::Cronometer
#include "au/Descriptors.h"         // au::CounterCollection
#include "au/CounterCollection.h"   // au::CounterCollection

#include "engine/Engine.h"          // engine::Object
#include "engine/Object.h"          // engine::Object
#include "engine/DiskManager.h"     // engine::DiskManager

#include "samson/common/samson.pb.h"// samson::network

#include "samson/delilah/Delilah.h"// samson::Delilah
#include "DelilahComponent.h"// samson::DelilahComponent

#include "TXTFileSet.h"// samson::TXTFileSet


namespace samson {
  
  
  class Delilah;
  class Buffer;
  
  
  class PopDelilahComponentItem
  {
    
  public:

    PopDelilahComponentItem( size_t pop_id , size_t block_id , const gpb::KVRanges& ranges )
    {
      pop_id_ = pop_id;
      block_id_ = block_id;
      ranges_.CopyFrom(ranges);
    }
    
    void SetContent( engine::BufferPointer buffer )
    {
      if( buffer_ != NULL )
      {
        LM_W(("Ignoring data received at PopDelilahComponentItem. Already received"));
        return;
      }
      buffer_ = buffer;
    }

    size_t worker_id()
    {
      return worker_id_;
    }

    void set_worker_id( size_t worker_id )
    {
      worker_id_ = worker_id;
    }
    
    engine::BufferPointer buffer()
    {
      return buffer_;
    }
    
    size_t block_id()
    {
      return block_id_;
    }
    
    const gpb::KVRanges& ranges()
    {
      return ranges_;
    }
    
    const au::Cronometer& cronometer()
    {
      return cronometer_;
    }
    
    bool worker_confirmation()
    {
      return worker_confirmation_;
    }
    
    void SetWorkerConfirmation()
    {
      worker_confirmation_ = true;
    }
    
    void ResetRequest()
    {
      cronometer_.Reset();
      worker_confirmation_ = false;
    }
    
    size_t pop_id()
    {
      return pop_id_;
    }
    
  private:

    engine::BufferPointer buffer_;       // Buffer received for this item
    
    size_t pop_id_;                      // Identifier of this particular item
    size_t block_id_;                    // Identifier of the block
    gpb::KVRanges ranges_;               // Ranges we are interested in
    
    size_t worker_id_;                   // worker selected to retrieve this block
    
    au::Cronometer cronometer_;          // Cronometer since request was sent to the worker
    bool worker_confirmation_;           // Confirmation from worker

    PopDelilahComponent* pop_delilah_component_;
    
  };
  
  class PopDelilahComponent : public DelilahComponent , public engine::Object
  {
    
  public:
    
	  PopDelilahComponent( std::string _queue , std::string _fileName , bool force_flag , bool show_flag );
	  ~PopDelilahComponent();
    
	  // Methods of DelilahComponent
	  virtual void run();
		virtual void receive( const PacketPointer& packet );
		virtual void review( );
    
	  // Function to get the status
	  std::string getStatus();
    std::string getExtraStatus();
    
	  // Function to receive notifications
	  void notify( engine::Notification* notification );

    // Function to check what to do with finish items
	  void check();
    
  private:
    
    // Send pop request
    void send_main_request();
    
    // Send request for a particular item
    void send_request( PopDelilahComponentItem* item )
    {
      item->ResetRequest();          // Reset request
      
      // Select a worker
      size_t worker_id = delilah->network->getRandomWorkerId( item->worker_id() );
      item->set_worker_id( worker_id );
      
      // Build & sent the packet
      au::SharedPointer<Packet> packet( new Packet( Message::PopBlockRequest , NodeIdentifier( WorkerNode , worker_id ) ) );
      
      // Fill information about this request
      packet->message->set_delilah_component_id( id );
      packet->message->set_pop_id( item->pop_id() );
      packet->message->set_block_id(item->block_id());
      packet->message->mutable_ranges()->CopyFrom( item->ranges() );
      delilah->network->Send( packet );
    }
    
    // Initial information of this pop operation
    std::string queue_;             // Name of the queue we are recovering
    std::string file_name_;         // Name of the file to create
	  bool force_flag_;               // Flag to remove previos directory
	  bool show_flag_;
    
    // Main request information
    size_t worker_id_;   // worker selected to request information about my queue
    int commit_id_;      // last commit observed
    au::Cronometer cronometer_;
    
    // Number of pop_responses
    int num_pop_queue_responses_;
    
    // Total number of blocks downloaded
    int num_blocks_downloaded_;
    
    // Information about selected queue ( contained in pop request response message )
    gpb::Queue* gpb_queue_;

    // counter for items
    size_t item_id_;
    
    // List of items for this pop operation
    au::map< size_t, PopDelilahComponentItem > items_;

    // Pending write operations
    int num_pending_write_operations_;
    
  };
  
}

#endif
