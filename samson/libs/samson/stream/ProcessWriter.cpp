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
#include "samson/stream/ProcessWriter.h"          // Own interface

#include <string>

#include "engine/Buffer.h"                        // samson::Buffer
#include "engine/MemoryManager.h"                 // samson::MemoryManager

#include "samson/module/ModulesManager.h"         // samson::ModulesManager
#include "samson/network/NetworkInterface.h"      // samson::NetworkInterface
#include "samson/network/Packet.h"                // samson::Packet
#include "samson/stream/ProcessIsolated.h"        // samson::ProcessIsolated
#include "samson/stream/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "samson/stream/SharedMemoryManager.h"    // samson::SharedMemoryManager

namespace samson {
ProcessWriter::ProcessWriter(ProcessIsolated *_processIsolated) {
  // Pointer to the worker task item
  processIsolated = _processIsolated;

  // Get the assignated shared memory region
  item = engine::SharedMemoryManager::shared()->getSharedMemoryChild(processIsolated->get_shm_id());

  // General output buffer
  buffer = item->data;
  size = item->size;

  if (!buffer) {
    LM_X(1, ("Internal error: No buffer in a ProcessWriter"));
  }
  if (size == 0) {
    LM_X(1, ("Wrong size in a ProcessWriter operation"));   // Number of outputs
  }
  num_outputs = processIsolated->get_num_outputs();

  // Take the ranges to emit packets....
  // num_hg_divisions = processIsolated->num_hg_divisions;

  // Hash code for the outputs
  keyValueHash = new KeyValueHash[num_outputs];

  outputKeyDataInstance = reinterpret_cast<DataInstance **>(malloc(sizeof(*outputKeyDataInstance) * num_outputs));
  outputValueDataInstance = reinterpret_cast<DataInstance **>(malloc(sizeof(*outputValueDataInstance) * num_outputs));

  if (num_outputs != static_cast<int>(processIsolated->get_outputFormats().size())) {
    LM_E((
           "Not possible to get the hash-code of the data instances used at the output since output formats are not defined"));
    processIsolated->setUserError(
      "Not possible to get the hash-code of the data instances used at the output since output formats are not defined");
    return;
  } else {
    for (int i = 0; i < static_cast<int>(num_outputs); i++) {
      std::string key_data = processIsolated->get_outputFormats()[i].keyFormat;
      std::string value_data = processIsolated->get_outputFormats()[i].valueFormat;

      Data *keyData = au::Singleton<ModulesManager>::shared()->getData(key_data);
      Data *valueData = au::Singleton<ModulesManager>::shared()->getData(value_data);

      if (!keyData) {
        processIsolated->setUserError(au::str("Data %s not found", key_data.c_str()));
        return;
      }
      if (!valueData) {
        processIsolated->setUserError(au::str("Data %s not found", value_data.c_str()));
        return;
      }

      outputKeyDataInstance[i] = reinterpret_cast<DataInstance *>(keyData->getInstance());
      outputValueDataInstance[i] = reinterpret_cast<DataInstance *>(valueData->getInstance());

      keyValueHash[i].key_hash = outputKeyDataInstance[i]->getHashType();
      keyValueHash[i].value_hash = outputValueDataInstance[i]->getHashType();
    }
  }

  // Init the minibuffer
  miniBuffer = reinterpret_cast<char *>(malloc(KVFILE_MAX_KV_SIZE));
  miniBufferSize = 0;

  // Outputs structures placed at the beginning of the buffer
  channel = reinterpret_cast<OutputChannel *>(buffer);

  if (size < sizeof(OutputChannel) * num_outputs) {
    LM_X(1, ("Wrong size of shared-memory segment (%lu)", size));   // Buffer starts next
  }
  node = reinterpret_cast<NodeBuffer *>((buffer + sizeof(OutputChannel) * num_outputs ));
  num_nodes = (size - (sizeof(OutputChannel) * num_outputs )) / sizeof(NodeBuffer);

  // Clear this structure to receive new key-values
  clear();
}

ProcessWriter::~ProcessWriter() {
  // Free minibuffer used to serialize key-value here
  // Note: If there was an error in the constructor, it may be NULL
  if (miniBuffer) {
    free(miniBuffer);
  }
  if (item) {
    delete item;   // Delete key-value hash vector
  }
  // Note: If there was an error in the constructor, it may be NULL
  if (keyValueHash) {
    delete[] keyValueHash;
  }
  if (outputKeyDataInstance) {
    for (int i = 0; i < num_outputs; i++) {
      delete outputKeyDataInstance[i];
    }
    free(outputKeyDataInstance);
  }

  if (outputValueDataInstance) {
    for (int i = 0; i < num_outputs; i++) {
      delete outputValueDataInstance[i];
    }
    free(outputValueDataInstance);
  }
}

void ProcessWriter::internal_emit(int output, int hg, char *data, size_t data_size) {
  // Get a pointer to the current node
  OutputChannel *_channel = &channel[output ];   // Final Output channel ( output + server )
  HashGroupOutput *_hgOutput = &_channel->hg[hg];   // Current hash-group output

  size_t availableSpace = (num_nodes - new_node) * KV_NODE_SIZE;

  if (_hgOutput->last_node != KV_NODE_UNASIGNED) {
    availableSpace += node[_hgOutput->last_node].availableSpace();   // Check if it will fit
  }
  if (data_size >= availableSpace) {
    // Process the output buffer and clear to continue
    flushBuffer(false);
    clear();
  }

  // Update the info in the particular output and the concrete hash-group
  _channel->info.append(data_size, 1);
  _hgOutput->info.append(data_size, 1);

  // Possition inside the data vector
  uint32 pos = 0;

  // Get a pointer to the first node ( or create if not created before )
  NodeBuffer *_node;
  if (_hgOutput->last_node == KV_NODE_UNASIGNED) {
    if (new_node >= num_nodes) {
      LM_X(1, ("Internal error"));
    }
    node[new_node].init();   // Init the new node
    _hgOutput->first_node = new_node;   // Update the HasgGroup structure to point here
    _hgOutput->last_node = new_node;   // Update the HasgGroup structure to point here
    _node = &node[new_node];   // Point to this one to write
    new_node++;
  } else {
    if (_hgOutput->last_node >= num_nodes) {
      LM_X(1, ("Internal error"));
    }
    _node = &node[_hgOutput->last_node];   // Current write node
  }

  // Fill following nodes...
  while (pos < data_size) {
    // Write in the node
    pos += _node->write(data + pos, data_size - pos);

    if (_node->isFull()) {
      _node->setNext(new_node);   // Set the next in my last node
      node[new_node].init();   // Init the new node
      _hgOutput->last_node = new_node;   // Update the HasgGroup structure to point here
      if (new_node > num_nodes) {
        LM_X(1, ("Internal error"));
      }
      _node = &node[new_node];   // Point to this one to write
      new_node++;
    }
  }
}

void ProcessWriter::emit(int output, DataInstance *key, DataInstance *value) {
  // Spetial case for logging...
  if (output == -1) {
    output = num_outputs - 1;   // Last channel
  }
  // Check if DataInstances used for key and value are correct

  // output = num_outputs-1 is the trace queue
  if (output > num_outputs) {
    std::ostringstream error_message;
    error_message << "Output queue index (" << output << ") is larger than the number of defined outputs("
                  << num_outputs << ")  (last one is trace channel)";
    LM_E(("Error: %s", error_message.str().c_str()));

    processIsolated->setUserError(error_message.str());
    return;
  }

  if (output > num_outputs) {
    LM_X(1, ("Emiting key-value usign channel %d ( this operation has only %d outputs)", output, num_outputs));
  }
  if (key->getHashType() != keyValueHash[output].key_hash) {
    std::ostringstream error_message;
    error_message << "Different hash-type for key at output # " << output << " of num_outputs:" << num_outputs
                  << " (last one is trace channel)";
    error_message << ". Used " << key->getName() << " instead of ";
    error_message << outputKeyDataInstance[output]->getName() << ".";
    LM_E(("Error: %s", error_message.str().c_str()));

    processIsolated->setUserError(error_message.str());
    return;
  }

  if (value->getHashType() != keyValueHash[output].value_hash) {
    std::string data_name = processIsolated->get_outputFormats()[output].valueFormat;

    std::ostringstream error_message;
    error_message << "Wrong data at output # " << (output + 1) << "/" << num_outputs;
    error_message << ". Used " << value->getName() << " instead of ";
    error_message << outputValueDataInstance[output]->getName() << ".";
    LM_E(("Error: %s", error_message.str().c_str()));

    processIsolated->setUserError(error_message.str());
    return;
  }

  // LOG_SM(("PW emit: %s %s", key->str().c_str()  , value->str().c_str() ));

  // Serialize to the minibuffer
  int hg_previous = key->hash(KVFILE_NUM_HASHGROUPS);
  std::string original_key = key->str();

  size_t key_size = key->serialize(miniBuffer);
  size_t key_size_theoretical = key->parse(miniBuffer);

  if (key_size != key_size_theoretical) {
    LOG_SW(("Error serializing [%s] '%s'", key->getType(), key->str().c_str()));
    LOG_SW(("Error serializing data. Different key size serializing key %lu vs %lu", key_size, key_size_theoretical));
    LM_X(1, ("Non valid serialization key"));
  }

  size_t value_size = value->serialize(miniBuffer + key_size);
  size_t value_size_theoretical = value->parse(miniBuffer + key_size);

  if (value_size != value_size_theoretical) {
    LOG_SW(("Error serializing [%s] '%s'", value->getType(), value->str().c_str()));
    LOG_SW(("Error serializing data. Different value size serializing key %lu vs %lu", value_size,
            value_size_theoretical));
    LM_X(1, ("Non valid serialization value"));
  }

  // Total size including key & value
  miniBufferSize = key_size + value_size;

  // Emit the miniBuffer to the right place
  int hg = key->hash(KVFILE_NUM_HASHGROUPS);

  if (hg != hg_previous) {
    LM_E(("Error, different hash for original key:'%s' --> hash:%d and parsed key:'%s' --> hash:%d",
          original_key.c_str(), hg_previous, key->str().c_str(), hg));
    return;
  }

  // Emit the generated data
  internal_emit(output, hg, miniBuffer, miniBufferSize);
}

void ProcessWriter::flushBuffer(bool finish) {
  // Send code to the platform
  if (finish) {
    processIsolated->sendCode(WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH);
  } else {
    processIsolated->sendCode(WORKER_TASK_ITEM_CODE_FLUSH_BUFFER);   // Clear the buffer
  }
  clear();
}

void ProcessWriter::clear() {
  // Init all the outputs
  for (size_t c = 0; c < (size_t)(num_outputs); c++) {
    channel[c].init();
  }
  new_node = 0;
}

#pragma mark ProcessTXTWriter

ProcessTXTWriter::ProcessTXTWriter(ProcessIsolated *_workerTaskItem) {
  workerTaskItem = _workerTaskItem;

  // Get the assignated shared memory region
  item = engine::SharedMemoryManager::shared()->getSharedMemoryChild(workerTaskItem->get_shm_id());

  // Size if the firt position in the buffer
  size = reinterpret_cast<size_t *>(item->data);

  // Init the data buffer used here
  data = item->data + sizeof(size_t);
  max_size = item->size - sizeof(size_t);   // This is the available space int he buffer

  // Init the size of the output
  *size = 0;
}

ProcessTXTWriter::~ProcessTXTWriter() {
  // Remove the shared memory segment
  if (item) {
    delete item;
  }
}

void ProcessTXTWriter::flushBuffer(bool finish) {
  // Send code to be understoo
  if (finish) {
    workerTaskItem->sendCode(WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH);
  } else {
    workerTaskItem->sendCode(WORKER_TASK_ITEM_CODE_FLUSH_BUFFER);   // Note: It is not necessary to delete item since it has been done inside "freeSharedMemory"
  }
  // Clear the buffer
  *size = 0;
}

void ProcessTXTWriter::emit(const char *_data, size_t _size) {
  if (*size + _size > max_size) {
    flushBuffer(false);
  }
  memcpy(data + (*size), _data, _size);
  *size += _size;
}
}
