#include "NetworkCenter.h"       // NetworkFakeCenter
#include "NetworkFake.h"             // Own interface



namespace samson {

	NetworkFake::NetworkFake( int _worker_id , NetworkFakeCenter *_center )	// -1 controller -2 dalila
	{
		worker_id = _worker_id;
		center = _center;
        receiver = NULL;
	}
	
	void NetworkFake::initAsSamsonController(void)
	{
		// Nothing to do
	}
	
	// Set the receiver element
	void NetworkFake::setPacketReceiver(PacketReceiverInterface* _receiver)
	{
		receiver = _receiver;
	}
	
	bool NetworkFake::ready()
	{
		return true;
	}
	
	// Get identifiers of known elements
	int NetworkFake::controllerGetIdentifier()
	{
		return -1;
	}

	int NetworkFake::workerGetIdentifier(int i)
	{
		return i;
	}

	int NetworkFake::getMyidentifier()
	{
		return worker_id;
	}
	
	int NetworkFake::getWorkerFromIdentifier( int identifier )
	{
		return identifier;
	}
	
	
	int NetworkFake::getNumWorkers()
	{
		return center->num_workers;
	}

	// Send a packet (return a unique id to inform the notifier later)
	size_t NetworkFake::send(PacketSenderInterface* sender, int endpointId, Packet* packetP)
	{
        samson::Message::MessageCode code = packetP->msgCode;
        
		// Add packet in the list of the center
        center->addPacket( new NetworkFakeCenterPacket(code, packetP , getMyidentifier(), endpointId, sender) );

		return 0;
	}
    
    void NetworkFake::delilahSend(PacketSenderInterface* packetSender, Packet* packetP)
    {
        // send to the only delilah we have here
        send( packetSender , -2 , packetP );
    }

	 void NetworkFake::run()
	{
		// Nothing to do here
	}
	
	 void NetworkFake::quit()
	{
		// Nothing to do here
	}

}
