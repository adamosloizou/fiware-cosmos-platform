#include "samson/delilah/DelilahCommandCatalogue.h"

namespace samson {
DelilahCommandCatalogue::DelilahCommandCatalogue() {
  // SETUP
  // ------------------------------------------------------------------

  add("show_cluster_setup", "setup", "Show cluster setup",
      "Get information about workers currently connected to current ( number of cores, memory, etc... )\n");

  add("show_cluster_assignation", "setup", "Show cluster assignment",
      "Get information about what hash-groups are assigned to what workers\n");

  add("ls_connections", "management", "Show status of all connections in the cluster");
  add_uint64_option("ls_connections", "-w", 0, "Specify a particular worker");
  add_tag("ls_connections", "send_to_all_workers");

  add("ls_local_connections", "management",
      "Show status of all connections of this delilah ( typically to all workers of the SAMSON cluster )");

  // DELILAH
  // ------------------------------------------------------------------

  add("connect", "delilah", "Connect to a SAMSON system");
  add_mandatory_string_argument("connect", "host", "SAMSON node/s to connect (\"host:port host:port..\")");
  add_string_option("connect", "-digest", "", "Username and password used to connect to this SAMSON cluster");

  add("disconnect", "delilah", "Disconnect from a SAMSON system");

  add("alerts", "delilah", "Activate or deactivate showing alerts from SAMSON platform on console");
  add_string_options_argument("alerts", "action", "Options:on:off", "Command to activate or deactivate alerts");

  add("log", "delilah", "Setup log in this delilah ( see more details about log system)");
  add_mandatory_string_argument("log", "command", "Command for log setup");

  add("show_alerts", "delilah", "Show the last alerts received from current SAMSON cluster at this delilah");

  add("open_alerts_file", "delilah", "Open a local file to store all received alerts");
  add_mandatory_string_argument("open_alerts_file", "file", "Local file to store alerts");

  add("close_alerts_file", "delilah", "Close local file opened to store with command 'open_traces_file'");

  add("verbose", "delilah", "Activate or deactivate verbose mode",
      "In verbose mode, extra information is shown to the user");
  add_string_options_argument("verbose", "action", "Options::on:off");

  add("wlog", "delilah", "Show information about what logs are activated at nodes of current SAMSON cluster");

  add("wverbose", "delilah", "Activate or deactivate verbose mode for workers",
      "verbose <0-5>   set level of verbosity\n"
        "verbose off     disable verbose mode\n"
        "verbose get     get level of verbosity\n");

  add("wdebug", "delilah", "Activate or deactivate debug mode for workers", "debug on\n"
    "debug off\n"
    "debug get\n");

  add("wreads", "delilah", "Activate or deactivate reads mode for workers.", "reads on\n"
    "reads off\n"
    "reads get\n");

  add("wwrites", "delilah", "Activate or deactivate writes mode for workers", "writes <on> <off> <get>");

  add("wtrace", "delilah", "Activate or deactivate trace levels for workers.");
  add_string_options_argument("wtrace", "action", "Options::off:set:add:remove",
                              "Action to be performed over trace levels");
  add_string_argument("wtrace", "levels", "", "Levels selection. Example '1-5,7,76-99'");

  add("send_alert", "delilah", "Send an alert to all connected delilahs using a random worker as sender");
  add_string_option("send_alert", "-worker", "", "Select broadcaster worker");
  add_bool_option("send_alert", "-error", "Mark message as an error");
  add_bool_option("send_alert", "-warning", "Mark message as a warning");
  add_string_argument("send_alert", "message", "", "Content of the message");

  add("ps", "delilah", "Show information about delilah processes");
  add_bool_option("ps", "-clear", "Remove finished processes");
  add_string_argument("ps", "id", "", "Show more information for this process");

  add("set", "delilah", "Set environment variable in delilah client");
  add_string_argument("set", "name", "", "Name of the variable to set");
  add_string_argument("set", "value", "", "Value to assign");

  add("unset", "delilah", "Remove an environment variable in delilah client");
  add_string_argument("set", "name", "", "Name of the variable to set");

  add("set_mode", "delilah", "Select delilah working mode: normal, database, logs");
  add_string_options_argument("set_mode", "mode", "Options:normal:database:logs");

  add("ls_local", "local", "Show a list of current directory with relevant information about local queues");
  add_string_argument("ls_local", "file", "*", "File or pattern to be displayed");

  add("ls_local_queues", "local", "Show a list of local queues");
  add_string_argument("ls_local_queues", "file", "*", "File or pattern to be displayed");

  add("rm_local", "local", "Remove a local file or directory (and all its contents)");
  add_mandatory_string_argument("rm_local", "file", "Local file or directory to be removed");

  add("history", "local", "Show the history of the last commands");
  add_int_option("history", "limit", 0);

  add("quit", "delilah", "Quit delilah console");

  // DATA MANIPULATION
  // ------------------------------------------------------------------

  add("ls", "data", "Show a list of all data queues in the system");
  add_string_argument("ls", "pattern", "*", "Pattern of the queues to show");
  add_bool_option("ls", "-rates", "Show information about data rate in queues (data movement)");
  add_bool_option("ls", "-blocks", "Show information about blocks contained at each queue");
  add_bool_option("ls", "-properties", "Show information about properties at each queue");
  add_bool_option("ls", "-a", "Show hidden queues as well ( used internally by the platform )");
  add_string_option("ls", "-group", "", "Group results by a particular column");

  add("add", "data", "Create a new queue");
  add_mandatory_string_argument("add", "queue", "Name of the new queue");
  add_mandatory_string_argument("add", "key_format", "Format for the key's in new queue (txt is accepted)");
  add_string_argument("add", "value_format", "txt", "Format for the value's in new queue (txt is accepted)");

  add("remove_all_data", "data", "Remove all queues in the system");

  add("remove_all", "data", "Remove all queues and stream_operations in the current SAMSON cluster");

  add("rm", "data", "Remove a queue");
  add_mandatory_string_argument("rm", "queue", "Queue to be removed");

  add("set_queue_property", "data", "Specify the value of a property in a queue ");
  add_mandatory_string_argument("set_queue_property", "queue", "Queue to be updated");
  add_mandatory_string_argument("set_queue_property", "property", "Name of the property we want to update");
  add_mandatory_string_argument("set_queue_property", "value", "Value for the selected property");

  add("unset_queue_property", "data", "Remove a previously defined property for a queue");
  add_mandatory_string_argument("unset_queue_property", "queue", "Queue to be updated");
  add_mandatory_string_argument("unset_queue_property", "property", "Name of the property we want to update");

  // Debug
  // ------------------------------------------------------------------

  add("threads", "debug", "Show current threads in this delilah");

  add("ls_blocks", "debug", "Show a list of data blocks managed by SAMSON nodes");
  add_uint64_option("ls_blocks", "-w", (size_t) -1, "Specify a worker to request the list of blocks");
  add_tag("ls_blocks", "send_to_all_workers");

  add("ls_queue_blocks", "debug", "Show detailed list of the blocks included in every queue");
  add_bool_option("ls_queue_blocks", "-a", "Show hiden queues as well ( used internally by the platform )");

  add("ls_buffers", "debug", "Show the list of data buffers managed in a SAMSON cluster. This is a debug tool");
  add_tag("ls_buffers", "send_to_all_workers");

  add("ls_block_requests", "debug", "Show current block requests operations in SAMSON nodes");
  add_tag("ls_block_requests", "send_to_all_workers");

  add("ls_last_data_commits", "debug", "Show last 100 commits on data model.");
  add_tag("ls_last_data_commits", "send_to_all_workers");

  add("ls_last_tasks", "debug", "Show last 100 tasks scheduled in workers");
  add_bool_option("ls_last_tasks", "-times" , "Show times spent by tasks");
  add_tag("ls_last_tasks", "send_to_all_workers");

  add("ls_queue_ranges", "debug", "Show how much information of a queue is at every range");
  add_mandatory_string_argument("ls_queue_ranges", "name", "Name of the queue we would like to scan");

  add("data_model_status"  , "debug" , "Show frozen and current data model status");
  add("data_model_commits" , "debug" , "Show on-the-fly commits pending to be consolidated");
  add("data_model_freeze"  , "debug" , "Show frozen and current data model status");
  add("data_model_cancel_freeze"  , "debug" , "Cancel candidate data model");
  add("data_model_recover" , "debug" , "Show frozen and current data model status");
  
  // MODULES
  // ------------------------------------------------------------------


  add("ls_modules", "modules", "Show a list of modules installed in SAMSON node workers");
  add_string_argument("ls_modules", "pattern", "*", "Filter modules with this pattern (* system.* ... )");

  add("ls_local_modules", "modules", "Show a list of modules installed available at this delilah");

  add("ls_operations", "modules", "Shows a list of available operations");
  add_string_argument("ls_operations", "pattern", "*", "Filter operations with this pattern (* system.* ... )");
  add_bool_option("ls_operations", "-v", "Show more details about input/output parameters");

  add("ls_datas", "modules", "Shows a list of available data-types.");
  add_string_argument("ls_datas", "pattern", "*", "Filter data-types with this pattern (* system.* ... )");

  add("push_module", "modules", "Push a module to the cluster.");
  add_mandatory_string_argument("push_module", "file", "Local file or directory")->set_options_group("#file");

  add("clear_modules", "modules", "Remove all previously updated modules");

  // STREAM
  // ------------------------------------------------------------------


  add("ls_stream_operations", "stream", "Show a list of stream operations",
      "Stream operations are operations that are automatically executed in a SAMSON cluster\n"
        "They consist in applying a particular operation ( defined in a module ) to process data from\n"
        "one or multiple input queues and send result to one or multiple output queues\n"
        "Please, add new stream operations with add_stream_operations command");
  add_bool_option("ls_stream_operations", "-a",
                  "Show hidden stream operations as well ( used internally by the platform )");

  add("ps_stream_operations", "stream", "Show a list of stream operations with information about execution planning");
  add_string_argument("ps_stream_operations", "pattern", "*", "Name of the stream operation");
  add_bool_option("ps_stream_operations", "-state","Show state of this stream operations");
  add_tag("ps_stream_operations", "send_to_all_workers");

  add("ps_stream_operations_ranges", "stream","Show a detailed list of a stream operation (for each range)");
  add_mandatory_string_argument("ps_stream_operations_ranges", "pattern", "Name of the stream operation");
  add_bool_option("ps_stream_operations_ranges", "-state",
                  "Show planning state ( paused, error, ready for scheduling...)");
  add_bool_option("ps_stream_operations_ranges", "-tasks", "Show tasks associated to stream_operations");
  add_bool_option("ps_stream_operations_ranges", "-properties", "Information about properties of each stream opertion");
  add_bool_option("ps_stream_operations_ranges", "-data", "Show input and output data processed of each operation");
  add_bool_option("ps_stream_operations_ranges", "-rates", "Show input and output data rates of each operation");
  add_uint64_option("ps_stream_operations_ranges", "-w", (size_t) -1, "Selected worker");
  add_tag("ps_stream_operations_ranges", "send_to_all_workers");

  add("add_stream_operation", "stream", "Add a new stream operation to the current SAMSON cluster");
  add_mandatory_string_argument("add_stream_operation", "name", "Name of the new stream operation");
  add_mandatory_string_argument("add_stream_operation", "operation", "Name of the operation to run");
  add_bool_option("add_stream_operation", "-paused" , "Start with the operation paused");

  add_string_option("add_stream_operation", "-input", "", "Input queues to get data from");
  add_string_option("add_stream_operation", "-output", "", "Output queues to push data to");

  add_bool_option("add_stream_operation", "-forward", "Forward reduce operations are scheduled. ( No state )");
  add_bool_option("add_stream_operation", "-update_only", "Only update state for keys with new input values");

  add("remove_stream_operation", "stream", "Remove a previously defined operation with add_stream_operation");
  add_mandatory_string_argument("remove_stream_operation", "name", "Name of the stream operations to remove");

  add("remove_all_stream_operations", "stream", "Remove all stream operations");

  add("wait", "stream", "Wait for all the stream operations activity to be finished");

  add("cancel_stream_operation", "stream", "Cancel of  execution of a particular stream operation");
  add_string_option("cancel_stream_operation", "name", "", "Name of the stream operation to be canceled");

  add("set_stream_operation_property", "stream",
      "Set value of an environment property associated to a stream operation (see add_stream_operation)",
      "set_stream_operation_property <stream_operation_name> <variable_name> <value>");
  add_mandatory_string_argument("set_stream_operation_property", "operation_name",
                                "Name of the affected stream operation");
  add_mandatory_string_argument("set_stream_operation_property", "variable_name", "Name of the variable");
  add_mandatory_string_argument("set_stream_operation_property", "value", "Value of the variable");

  add("unset_stream_operation_property", "stream",
      "Remove an environment property associated to a stream operation (see add_stream_operation)",
      "unset_stream_operation_property <stream_operation_name> <variable_name>");

  add_mandatory_string_argument("unset_stream_operation_property", "operation_name",
                                "Name of the affected stream operation");
  add_mandatory_string_argument("unset_stream_operation_property", "variable_name", "Name of the variable");

  add("add_queue_connection", "stream", "Connect a queue to other queues to duplicate data automatically");

  add_mandatory_string_argument("add_queue_connection", "source_queue", "Source queue");
  add_mandatory_string_argument("add_queue_connection", "target_queue", "Target queue ( or queues )");

  add("rm_queue_connection", "stream", "Remove a connection between queues (see  add_queue_connection )");
  add_mandatory_string_argument("rm_queue_connection", "source_queue", "Source queue");
  add_mandatory_string_argument("rm_queue_connection", "target_queue", "Target queue ( or queues )");

  add("ls_queue_connections", "stream", "Show a list with all queue connections ( see add_queue_connection ) ");
  add_bool_option("ls_queue_connections", "-a", "Show hidden items as well ( used internally by the platform )");

  add("ps_tasks", "stream", "Get a list of current stream tasks currently running in all workers");
  add_uint64_option("ps_tasks", "-w", (size_t) -1, "Specify worker to request list of tasks");
  add_bool_option("ps_tasks", "-data", "Get detailed information of input/output data");
  add_bool_option("ps_tasks", "-blocks", "Get detailed information of input/output blocks");
  add_tag("ps_tasks", "send_to_all_workers");

  add("ls_workers", "stream", "Get a list of current workers");
  add_bool_option("ls_workers", "-engine", "Show details about underlying engines at worker nodes");
  add_bool_option("ls_workers", "-disk", "Show details about underlying engines at worker nodes");
  add_tag("ls_workers", "send_to_all_workers");

  add("init_stream", "stream", "Execute am initialization script to setup some automatic stream operations",
      "init_stream [prefix] <script_name>\n"
        "          [prefix]         It is used to name operations and queues\n"
        "          <script_name>    Name of the script (e.g. module.script)\n");

  add("ps_workers", "stream", "Show a list of commands being executed in each node of the cluster",
      "ps_workers <command pattern>\n");

  add("defrag", "stream", "Defrag content of a particular queue", "defrag <queue> <destination_queue>\n");

  // BATCH

  add("run", "batch", "Run a particular operation over queues in batch mode");
  add_mandatory_string_argument("run", "operation", "Name of the operation to run");
  add_string_option("run", "-input", "", "Input queues to get data from");
  add_string_option("run", "-output", "", "Output queues to push data to");

  add("ls_batch_operations", "batch", "Show a list of current batch operations scheduled");
  add_bool_option("ls_batch_operations", "-input", "Show real input pending to be process");
  add_bool_option("ls_batch_operations", "-output", "Show output produces batch operations");

  add("clear_batch_operations", "batch", "Clear finished batch operations");
  add_bool_option("clear_batch_operations", "-a", "Clear also unfinished tasks");

  // PUSH&POP
  // ------------------------------------------------------------------

  add("push", "push&pop", "Push content of a local file or a directory to a set of queues");
  add_mandatory_string_argument("push", "file", "Local file or directory")->set_options_group("#file");
  add_mandatory_string_argument("push", "queue", "queue or queues to push data (queue1 queue2 queue3 ...)");

  add("pop", "push&pop", "Pop content of a queue to a local directory");
  add_mandatory_string_argument("pop", "queue", "Name of the queue we want to pop");
  add_mandatory_string_argument("pop", "file_name", "Local directory to save downloaded blocks");
  add_bool_option("pop", "-force", "Delete local directory if it exists");
  add_bool_option("pop", "-show", "Show first lines of the content once downloaded");

  add("ls_local_push_operations", "push&pop", "Show list of pending push items");

  add("connect_to_queue", "push&pop", "Connect to a queue to receive live data from a SAMSON cluster");
  add_mandatory_string_argument("connect_to_queue", "queue", "Source queue ( see ls command )");

  add("disconnect_from_queue", "push&pop",
      "Disconnects from a particular queue to not receive live data from a SAMSON cluster");
  add_mandatory_string_argument("disconnect_from_queue", "queue", "Source queue ( see ls command )");

  add("ls_pop_connections", "push&pop", "Show a list of connections to receive live data from SAMSON.",
      "ls_pop_connections");

  add("show_local_queue", "push&pop", "Show contents of a local queue downloaded using pop",
      "Note: Modules should be installed locally");

  add_mandatory_string_argument("show_local_queue", "queue", "Local queue to be displayed");
  add_bool_option("show_local_queue", "-header", "Display only header information");
  add_bool_option("show_local_queue", "-show_hg", "Show hash-group for each key-value");
  add_uint64_option("show_local_queue", "-limit", 0, "Limit the number of records displayed");

  add("push_queue", "push&pop", "Push content of a queue to another queue/s");
  add_mandatory_string_argument("push_queue", "queue_from", "Source queue to get data from");
  add_mandatory_string_argument("push_queue", "queue_to", "Target queue to push data to");
}
}
