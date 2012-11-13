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

#include "au/log/LogMain.h"
#include "PacketQueue.h"  // Own interface

namespace samson {
void MultiPacketQueue::Clear() {
  // Just clear the map ( now it is a map of shared pointers )
  au::TokenTaker tt(&token_packet_queues);

  packet_queues.clear();
}

// Push a packet to be sent
void MultiPacketQueue::Push(const au::SharedPointer<Packet>& packet) {
  // Robust agains NULL Packets...
  if (packet == NULL) {
    return;
  }

  // Get target node
  const NodeIdentifier& node_identifier = packet->to;

  // Select the correct queue and push the new packet
  std::string name = node_identifier.getCodeName();
  packet_queues.findOrCreate(name)->Push(packet);
}

au::SharedPointer<Packet> MultiPacketQueue::Front(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues);
  std::string name = node_identifier.getCodeName();
  PacketQueue *paquet_queue = packet_queues.findOrCreate(name);

  // Get next packet
  return paquet_queue->Front();
}

void MultiPacketQueue::Pop(const NodeIdentifier& node_identifier) {
  au::TokenTaker tt(&token_packet_queues);
  std::string name = node_identifier.getCodeName();
  PacketQueue *paquet_queue = packet_queues.findInMap(name);

  // Pop packet
  paquet_queue->Pop();
}

au::tables::Table *MultiPacketQueue::getPendingPacketsTable() {
  au::tables::Table *table = new au::tables::Table(au::StringVector("Connection", "#Packets", "Size"));


  au::map<std::string, PacketQueue>::iterator it;

  for (it = packet_queues.begin(); it != packet_queues.end(); it++) {
    au::StringVector values;

    values.push_back(it->first);     // Name of the connection

    PacketQueue *packet_queue = it->second;


    values.push_back(au::str(packet_queue->size()));
    values.push_back(au::str(packet_queue->byte_size()));

    table->addRow(values);
  }

  table->setTitle("Pending packets");

  return table;
}

void MultiPacketQueue::RemoveOldConnections(const std::set<std::string> current_connections) {
  au::TokenTaker tt(&token_packet_queues);

  au::map<std::string, PacketQueue>::iterator it_packet_queues;
  for (it_packet_queues = packet_queues.begin(); it_packet_queues != packet_queues.end(); ) {
    std::string name = it_packet_queues->first;

    if (current_connections.find(name) != current_connections.end()) {
      // It is still connected
      it_packet_queues->second->ResetInactivityCronometer();
      ++it_packet_queues;
      continue;
    }

    if (it_packet_queues->second->inactivity_time() > 60) {
      LOG_SW(("Removing  pending packets for %s since it has been disconnected mote than 60 secs", name.c_str()));
      packet_queues.erase(it_packet_queues++);
    } else {
      ++it_packet_queues;
    }
  }
}
}