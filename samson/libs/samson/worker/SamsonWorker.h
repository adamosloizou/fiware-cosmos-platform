#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
 *
 * FILE                     SamsonWorker.h
 *
 * DESCRIPTION			   Main class for the worker element
 *
 */

#include <iomanip>                              // setiosflags()
#include <iostream>                             // std::cout
#include <string>

#include "au/console/Console.h"
#include "au/containers/SharedPointer.h"
#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "zoo/Connection.h"

#include "engine/EngineElement.h"               // samson::EngineElement
#include "logMsg/logMsg.h"

#include "samson/common/KVHeader.h"
#include "samson/common/Macros.h"                       // exit(.)
#include "samson/common/NotificationMessages.h"
#include "samson/common/samson.pb.h"                    // samson::network::
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT
#include "samson/delilah/Delilah.h"
#include "samson/module/Operation.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/BlockManager.h"
#include "samson/stream/WorkerTaskManager.h"
#include "samson/stream/WorkerTaskManager.h"     // samson::stream::WorkerTaskManager
#include "samson/worker/SamsonWorkerRest.h"
#include "samson/worker/SamsonWorkerSamples.h"
#include "samson/worker/WorkerBlockManager.h"
#include "samson/worker/WorkerCommandManager.h"
#include "samson/worker/DataModel.h"
#include "samson/worker/SamsonWorkerController.h"

/*
 *
 * SamsonWorker
 *
 * Status
 *
 * unconnected    --> Non connected to any cluster
 * connected      --> Connected to ZK
 * included       --> Part of the cluster
 * ready          --> All blocks required for processing are here
 * cluster_ready  --> All elements in the cluster are ready
 */

namespace samson {
class NetworkInterface;
class Info;
class DistributionOperation;

namespace stream {
class WorkerTaskManager;
}

class PushOperation;
class SamsonWorkerRest;

class SamsonWorker : public engine::NotificationListener, public au::Console {
  public:

    SamsonWorker(std::string zoo_host, int port, int web_port);
    ~SamsonWorker() {}
    // All internal components are shared pointers

    // Interface to receive Packets
    void receive(const PacketPointer& packet);

    // Notification from the engine about finished tasks
    void notify(engine::Notification *notification);

    // au::Console ( debug mode with fg )
    void autoComplete(au::ConsoleAutoComplete *info);
    void evalCommand(std::string command);
    std::string getPrompt();

    // Function to get information about current status
    au::SharedPointer<gpb::Collection> GetWorkerCollection(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetWorkerLogStatus(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForDataModelStatus(const Visualization& visualization);
    au::SharedPointer<gpb::Collection> GetCollectionForDataModelCommits(const Visualization& visualization);

    bool IsReady();   // Method to access if worker is ready
    bool IsConnected();   // Method to access if worker is ready

    // Accessors to individual components of this worker
    au::SharedPointer<au::zoo::Connection> zoo_connection();
    au::SharedPointer<SamsonWorkerController> worker_controller();
    au::SharedPointer<DataModel> data_model();
    au::SharedPointer<WorkerNetwork> network();

    // Accessors to individual components of this worker
    au::SharedPointer<SamsonWorkerRest> samson_worker_rest();
    au::SharedPointer<WorkerBlockManager> worker_block_manager();
    au::SharedPointer<stream::WorkerTaskManager> task_manager();
    au::SharedPointer<WorkerCommandManager> workerCommandManager();

    // Reload modules
    void ReloadModulesIfNecessary();

  
  
  private:

    enum State {
      unconnected, connected, ready
    };

    State state_;   // Current state of this worker
    std::string state_message_;   // Message of the last review of the state

    // Main function to review samson worker and all its elements
    // This function is preiodically called from engine
    void Review();

    void ResetToUnconnected();   // Reset when come back to unconnected
    void ResetToConnected();   // Reset when come back to connected ( change cluster setup )

    // Connection values ( to reconnect if connections fails down )
    std::string zoo_host_;
    int port_;
    int web_port_;

    // Initial time stamp for this worker
    au::Cronometer cronometer_;

    // Main elements of the worker
    au::SharedPointer<au::zoo::Connection> zoo_connection_;   // Main connection with the zk
    au::SharedPointer<SamsonWorkerController> worker_controller_;   // Cluster setup controller
    au::SharedPointer<DataModel> data_model_;   // Data model
    au::SharedPointer<WorkerNetwork> network_;   // Network manager to manage connections
    au::SharedPointer<SamsonWorkerRest> samson_worker_rest_;   // REST Service
    au::SharedPointer<WorkerBlockManager> worker_block_manager_;   // Map of blocks recently created
    au::SharedPointer<stream::WorkerTaskManager> task_manager_;   // Manager for tasks
    au::SharedPointer<WorkerCommandManager> workerCommandManager_;   // Manager of the "Worker commands"

  
    bool modules_available_;        // Flag to determine if blocks for modules are available
    size_t last_modules_version_;   // Last version of the queue .modules observed so far

  // Visualitzation of current data model
  void fill( gpb::CollectionRecord *record , const std::string& name, gpb::Data* data, const Visualization& visualization );
  
  
    // State of this worker
    std::string str_state();
};
}

#endif  // ifndef SAMSON_WORKER_H
