#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 * Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <algorithm>
#include <cstdlib>                  // atexit
#include <sstream>                  // std::ostringstream
#include <string>

#include "au/console/CommandCatalogue.h"
#include "au/console/Console.h"     // au::console::Console
#include "au/console/ConsoleAutoComplete.h"
#include "au/log/LogProbe.h"
#include "au/mutex/TokenTaker.h"    // au::TokenTake
#include "au/network/ConsoleService.h"
#include "au/tables/Select.h"
#include "au/tables/Table.h"

#include "logMsg/logMsg.h"

#include "samson/delilah/AlertCollection.h"
#include "samson/delilah/Delilah.h"               // samson::Delilah
#include "samson/delilah/DelilahCommandCatalogue.h"

namespace samson {
/**
 *     Main class for the DelilahConsole program
 */

class DelilahConsole : public au::console::Console, public Delilah {
public:

  explicit DelilahConsole(size_t delilah_id = 1);
  ~DelilahConsole();

  void setSimpleOutput() {
    simple_output_ = true;
  }

  void setNoOutput() {
    no_output_ = true;
  }

  // Console related methods
  virtual std::string getPrompt();
  virtual void evalCommand(const std::string& command);
  virtual void autoComplete(au::console::ConsoleAutoComplete *info);
  void autoCompleteOperations(au::console::ConsoleAutoComplete *info);
  void autoCompleteOperations(au::console::ConsoleAutoComplete *info, std::string type);
  void autoCompleteQueueForOperation(au::console::ConsoleAutoComplete *info, std::string operation_name,
                                     int argument_pos);
  void autoCompleteQueueWithFormat(au::console::ConsoleAutoComplete *info, std::string key_format,
                                   std::string value_format);
  void autoCompleteQueues(au::console::ConsoleAutoComplete *info);
  virtual void process_escape_sequence(const std::string& sequence) {
    if (sequence == "samson") {
      writeWarningOnConsole("SAMSON's cool ;)");
    }
  }

  // Run asynch command and returns the internal operation in delilah
  size_t runAsyncCommand(std::string command);
  size_t runAsyncCommand(au::console::CommandInstance *command_instance);
  void runAsyncCommandAndWait(std::string command);      // Run asynch command and wait to finish ( only used in tests )

  // Functions overloaded from Delilah
  // --------------------------------------------------------
  void delilahComponentFinishNotification(DelilahComponent *component);
  void delilahComponentStartNotification(DelilahComponent *component);

  // Function to process messages from network elements not handled by Delila class ( with DelilahComponenets )
  int _receive(const PacketPointer& packet);

  // Process buffers of data received in streaming from SAMSON
  void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer);

  // Notify that an operation hash finish
  virtual void notifyFinishOperation(size_t id) {
    std::ostringstream output;

    output << "Finished local delilah process with : " << id;
    showWarningMessage(output.str());
  }

  void Show(au::ErrorManager& error, const std::string& prefix_message) {
    for (size_t i = 0; i < error.errors().size(); i++) {
      switch (error.errors()[i]->type()) {
        case au::ErrorMessage::item_message:
          showMessage(prefix_message + " " + error.errors()[i]->GetMessage());
          break;
        case au::ErrorMessage::item_warning:
          showWarningMessage(prefix_message + " " + error.errors()[i]->GetMessage());
          break;
        case au::ErrorMessage::item_error:
          showErrorMessage(prefix_message + " " + error.errors()[i]->GetMessage());
          break;
      }
    }
  }

  // Show a message on screen
  void showMessage(std::string message) {
    if (no_output_) {
      LM_V(("%s", message.c_str()));
      return;
    }
    if (simple_output_) {
      std::cout << message;
      return;
    }
    writeOnConsole(au::StringInConsole(message));
  }

  void showWarningMessage(std::string message) {
    if (no_output_) {
      LOG_SV(("%s", au::str(au::BoldMagenta, "%s", message.c_str()).c_str()));
      return;
    }
    if (simple_output_) {
      std::cout << au::str(au::BoldMagenta, "%s", message.c_str());
      return;
    }
    writeWarningOnConsole(au::StringInConsole(message));
  }

  void showErrorMessage(std::string message) {
    if (no_output_) {
      LOG_SV(("%s", au::str(au::BoldRed, "%s", message.c_str()).c_str()));
      return;
    }
    if (simple_output_) {
      std::cout << au::str(au::BoldRed, "%s", message.c_str());
      return;
    }
    writeErrorOnConsole(au::StringInConsole(message));
  }

  virtual void showTrace(std::string message) {
    if (show_alerts_) {
      writeWarningOnConsole(message);
    }
  }

private:

  DelilahCommandCatalogue delilah_command_catalogue_;

  AlertCollection trace_colleciton_;   // Collection of traces for monitoring
  std::string trace_file_name_;        // Name of the traces file ( if any )
  FILE *trace_file_;                   // FILE to store traces if defined with save_traces

  // Counter for the received stream buffers
  au::CounterCollection<std::string> stream_out_queue_counters_;

  // Flag to indicate if we are shoing traces
  bool show_alerts_;
  bool show_server_logs_;

  bool verbose_;        // Flag to show on screen certain messages
  bool simple_output_;  // Flag to just visualize content on screen ( delilah -command  or -f XX )
  bool no_output_;      // Flag to avoid any message visualization


  // LogClient used when working in log_client mode
  au::SharedPointer<au::LogProbe> log_probe;
};
}

#endif  // ifndef _H_DelilahConsole
