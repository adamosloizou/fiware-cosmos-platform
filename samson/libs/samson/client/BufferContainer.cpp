

#include "BufferContainer.h"  // Own interface

namespace  samson {
void BufferContainer::Push(const std::string& queue_name, engine::BufferPointer buffer) {
  au::TokenTaker tt(&token_);

  if (buffer == NULL) {
    return;
  }

  if (buffer->getSize() == 0) {
    return;
  }

  au::Queue<engine::Buffer> *queue = queues_.findOrCreate(queue_name);
  queue->Push(buffer);
}

engine::BufferPointer BufferContainer::Pop(const std::string& queue_name) {
  au::TokenTaker tt(&token_);

  au::Queue<engine::Buffer> *queue = queues_.findInMap(queue_name);

  if (!queue) {
    return engine::BufferPointer(NULL);
  } else {
    return queue->Pop();
  }
}
}