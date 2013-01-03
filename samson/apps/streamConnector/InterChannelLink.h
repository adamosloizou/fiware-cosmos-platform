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
#ifndef _H_STREAM_CONNECTOR_INTERCHANNEL_LINK
#define _H_STREAM_CONNECTOR_INTERCHANNEL_LINK

#include "au/network/SocketConnection.h"

#include "engine/ProcessItem.h"

#include "Adaptor.h"
#include "Connection.h"
#include "InterChannelPacket.h"
#include "PacketReaderWriter.h"
#include "common.h"
#include "common.h"
#include "message.pb.h"

namespace stream_connector {
// ------------------------------------------------------------------
//
// InterChannelLink
//
// Endpoint to write and receive InterChannelPacket
//
// ------------------------------------------------------------------


class InterChannelLink {
  // Keep a pointer to the socket connection
  au::SocketConnection *socket_connection_;

  // Two threads to read and write over this fd
  au::network::PacketReaderWriter<InterChannelPacket> *packet_reader_writer;

public:

  InterChannelLink(std::string name
                   , au::SocketConnection *socket_connection
                   , au::network::PacketReaderInteface<InterChannelPacket> *interface
                   );
  ~InterChannelLink();

  // Push a packet
  void push(au::SharedPointer<InterChannelPacket> packet);
  void push(const au::Queue<InterChannelPacket>& packets);

  void close_socket_connection();
  void close_and_stop();

  // Return If this can be removed looking at threads
  bool isRunning();
  bool isConnected();

  std::string host_and_port();
  size_t bufferedSize();

  void extract_pending_packets(au::Queue<InterChannelPacket>& packets);
};
}

#endif  // ifndef _H_STREAM_CONNECTOR_INTERCHANNEL_LINK
