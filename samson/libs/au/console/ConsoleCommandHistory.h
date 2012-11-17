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
#ifndef _AU_CONSOLE_COMMAND_HISTORY
#define _AU_CONSOLE_COMMAND_HISTORY

#include <sstream>
#include <string>
#include <vector>


namespace au {
namespace console {
class ConsoleCommand;

class ConsoleCommandHistory {
public:

  ConsoleCommandHistory();
  ~ConsoleCommandHistory();

  ConsoleCommand *current();


  void recover_history();
  void save_history();

  void move_up();
  void move_down();
  void new_command();


  std::string str_history(size_t limit) {
    std::ostringstream output;

    size_t pos = 0;

    if (limit > 0) {
      if (commands.size() > limit) {
        pos = commands.size() - limit;
      }
    }

    for (; pos < commands.size(); pos++) {
      output << commands[pos]->getCommand() << "\n";
    }

    return output.str();
  }

private:

  std::string file_name;
  size_t pos;
  std::vector<ConsoleCommand *> commands;
};
}
}
#endif  // ifndef _AU_CONSOLE_COMMAND_HISTORY
