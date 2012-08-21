

#include "engine/Buffer.h"

#include "InterChannelPacket.h"  // Own interface

namespace stream_connector {
InterChannelPacket::InterChannelPacket() {
  header_.magic_number = InterChannelPacketHeader_magic_number;
  message = new Message();
  header_.buffer_size = 0;
}

InterChannelPacket::~InterChannelPacket() {
  delete message;
}

// Get a pointer to the message to read or write
Message *InterChannelPacket::getMessage() {
  return message;
}

engine::BufferPointer InterChannelPacket::buffer() {
  return buffer_;
}

void InterChannelPacket::set_buffer(engine::BufferPointer buffer) {
  buffer_ = buffer;
}

au::Status InterChannelPacket::read(au::FileDescriptor *fd) {
  // Read a packet from file descriptor

  au::Status s = fd->partRead(&header_, sizeof(header_), "header", 0);

  if (s != au::OK)
    return s;

  // Reject excesive large packets
  if (header_.buffer_size > ( 256 * 1024 * 1024 ))
    return au::Error; if (header_.message_size > ( 256 * 1024 * 1024 ))
    return au::Error;

  // Read the GPB message
  if (header_.message_size != 0) {
    char *dataP = (char *)calloc(1, header_.message_size + 1);

    s = fd->partRead(dataP, header_.message_size, "Google Protocol Buffer", 300);
    if (s != au::OK) {
      free(dataP);
      return s;
    }

    // Decode the google protocol buffer message
    message->ParseFromArray(dataP, header_.message_size);

    if (message->IsInitialized() == false) {
      LM_E(("Error parsing Google Protocol Buffer of %lu bytes because is not initialized!",
            header_.message_size ));
      // Close connection ( We close here since it is not a io error, is a protocol error )
      free(dataP);
      fd->Close();
      return au::Error;           // Generic error
    }

    free(dataP);
  }

  // Create buffer and read it
  if (header_.buffer_size > 0) {
    buffer_ = engine::Buffer::create("InterChannelPacket", "streamConnector", header_.buffer_size);
    s = fd->partRead(buffer_->getData()
                     , header_.buffer_size
                     , "data"
                     , 0);

    // Set the correct size for this buffer
    buffer_->setSize(header_.buffer_size);
  }
  if (s != au::OK)
    return s;

  // Return OK
  return au::OK;
}

void InterChannelPacket::recompute_sizes_in_header() {
  header_.message_size = message->ByteSize();
  if (buffer_ != NULL)
    header_.buffer_size = buffer_->getSize(); else
    header_.buffer_size = 0;
}

au::Status InterChannelPacket::write(au::FileDescriptor *fd) {
  // Write a packet to a file descriptor

  if (!is_valid_packet())
    return au::Error;

  // Recompute size of the mssage in the header
  recompute_sizes_in_header();

  au::Status s = fd->partWrite(&header_, sizeof(header_), "header");
  if (s != au::OK)
    return s;

  //
  // Sending Google Protocol Buffer
  //
  if (header_.message_size > 0) {
    char *outputVec = (char *)malloc(header_.message_size + 2);
    if (outputVec == NULL)
      LM_XP(1, ("malloc(%d)", header_.message_size ));
    if (message->SerializeToArray(outputVec, header_.message_size) == false)
      LM_X(1, ("SerializeToArray failed")); s = fd->partWrite(outputVec, header_.message_size, "Google Protocol Buffer");
    free(outputVec);

    if (s != au::OK)
      LM_RE(s, ("partWrite:GoogleProtocolBuffer(): %s", status(s)));
  }

  if (buffer_ != NULL) {
    s = fd->partWrite(buffer_->getData(), buffer_->getSize(), "buffer");
    if (s != au::OK)
      return s;
  }

  return au::OK;
}

bool InterChannelPacket::is_valid_packet() {
  if (header_.magic_number != InterChannelPacketHeader_magic_number)
    return false;

  return true;
}

size_t InterChannelPacket::getSize() {
  // Recompute
  recompute_sizes_in_header();

  size_t total = 0;
  total += sizeof( header_ );
  total += header_.message_size;
  total += header_.buffer_size;

  return total;
}

std::string InterChannelPacket::str() {
  std::ostringstream output;

  output << "InterChannelPacket ( " << au::str(getSize(), "B") <<  " )";

  if (!is_valid_packet()) {
    output << "[ Non  valid ]";
    return output.str();
  }

  // Message information
  output << "[ ";

  if (message->has_target_channel())
    output << "target_channel=" << message->target_channel() << " "; if (message->has_ack())
    output << "ack=" << message->ack() << " "; output << " ]";

  // Buffer at the end of the message

  if (buffer_ != NULL)
    output << "[ Buffer " << au::str(buffer_->getSize(), "B") << " ]"; else
    output << "[ No buffer ]"; return output.str();
}
}