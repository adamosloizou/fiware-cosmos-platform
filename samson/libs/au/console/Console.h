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

#ifndef _AU_CONSOLE
#define _AU_CONSOLE

#include <termios.h>                // termios

#include <list>
#include <string>

#include "au/ThreadManager.h"
#include "au/console/ConsoleEntry.h"
#include "au/console/ConsoleEscapeSequence.h"

#include "au/ErrorManager.h"
#include "au/log/LogCentralPlugin.h"
#include "au/log/LogFormatter.h"
#include "au/mutex/Token.h"

namespace au {
namespace console {
class Console;
class ConsoleAutoComplete;
class ConsoleCommandHistory;

/**
 * \brief Full-featured console for easy interaction with user
 *
 * Subclasses should implement "evalCommand" to respond to a command introduced by user:
 * virtual void evalCommand(const std::string& command);
 *
 * Optional method to get custom prompts:
 * virtual std::string getPrompt();
 *
 * Optional method to autocomplete user command entry with "tab" key:
 * virtual void autoComplete(ConsoleAutoComplete *info);
 *
 * Optional, subclasses could implement following method for escape sequence handling:
 * virtual void process_escape_sequence(const std::string& sequence);
 *
 */

class Console : public au::Thread {
public:

  Console();
  virtual ~Console();

  // Customize console
  virtual std::string getPrompt();
  virtual void evalCommand(const std::string& command);
  virtual void autoComplete(ConsoleAutoComplete *info);
  virtual void process_escape_sequence(const std::string& sequence) {
  };


  // Methods to run and stop the console in background
  void StartConsole(bool block_thread = false);
  void StopConsole();

  /* Methods to write things on screen */
  void writeWarningOnConsole(const std::string& message);
  void writeErrorOnConsole(const std::string& message);
  void writeOnConsole(const std::string& message);

  /*
   * \brief General write function for all content inside a ErrorManager instance
   */

  void write(au::ErrorManager *error);
  void Write(au::ErrorManager& error);

  /*
   * \brief Add a escape sequence to be considered in this console
   */

  void AddEspaceSequence(const std::string& sequence);

  void Refresh();

  /*
   * \brief Wait showing a message on screen.... ( background message works )
   */

  int WaitWithMessage(const std::string& message, double sleep_time, ConsoleEntry *entry);

  // Make sure all messages are shown
  void Flush();

  // Append to current command
  void AppendToCommand(const std::string& txt);

  // Get the history string
  std::string str_history(int limit);

private:

  void RunThread();     // Main routine for background process

  void PrintCommand();
  bool IsInputReady() const;

  void ProcessAutoComplete(ConsoleAutoComplete *info);
  void ProcessInternalCommand(const std::string& sequence);
  void ProcessChar(char c);
  void ProcessEscapeSequenceInternal(const std::string& sequence);

  void ProcessBackgroundMessages();
  bool IsNormalChar(char c) const;

  void Write(const std::string& message);

  void GetEntry(ConsoleEntry *entry);        // Get the next entry from console

  // History information ( all commands introduced before )
  ConsoleCommandHistory *command_history_;

  // Pending messages to be displayed in background
  au::Token token_pending_messages_;
  std::list<std::string> pending_messages_;

  // Flag to block background messages
  bool block_background_messages_;

  // Detector of escape sequences
  ConsoleEscapeSequence escape_sequence_;

  // Flag to finish the background thread
  bool quit_console_;
};
}
}
#endif  // ifndef _AU_CONSOLE

