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


#ifndef _H_AU_LOG_PLUGIN_SERVER
#define _H_AU_LOG_PLUGIN_SERVER

#include <arpa/inet.h>          // inet_ntoa
#include <netdb.h>              // gethostbyname
#include <netinet/in.h>         // struct sockaddr_in
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <unistd.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "parseArgs/paBuiltin.h"
#include "parseArgs/paConfig.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/parseArgs.h"

#include "au/containers/SharedPointer.h"
#include "au/containers/set.h"
#include "au/log/Log.h"
#include "au/log/LogCommon.h"
#include "au/log/LogCentralPlugin.h"
#include "au/network/SocketConnection.h"
#include "au/string/StringUtilities.h"

namespace au {
/*
 * Plugin to connect to a log server
 */

class LogCentralPluginServer : public LogCentralPlugin {
public:

  LogCentralPluginServer() : LogCentralPlugin("Server") {
    set_activated(false);
  }

  LogCentralPluginServer(const std::string& host, int port, const std::string& local_file);
  virtual ~LogCentralPluginServer();

  void set_host(const std::string& host, int port, const std::string& local_file);

  // Change server host
  void SetLogServer(std::string host, int port);

  // Emit a log in this channel
  virtual void Emit(au::SharedPointer<Log> log);
  virtual std::string status();

  // Accesors
  std::string host() const;
  int port() const;
  std::string local_file() const;

  virtual std::string str_info() {
    return au::str("%s:%d", host_.c_str(), port_);
  }

private:

  void ReviewSocketConnection();   // Review socket connection

  // Connection information
  std::string host_;
  int port_;
  std::string local_file_;

  // Active connections to local file or network connection
  au::SharedPointer<FileDescriptor> local_file_descriptor_;      // Local file descriptor to write the log if not possible to connect
  au::SharedPointer<SocketConnection> socket_connection_;        // Socket connection with the logServer ( if any )

  au::Cronometer time_since_last_connection_;                    // Cronometer with the time since last connection
  size_t time_reconnect_;                                        // time for the next reconnection
};
}

#endif  // ifndef _H_AU_LOG_CENTRAL