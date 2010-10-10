

#include "SamsonController.h"			// Own interface
#include "packet.h"						// ss::Packet

namespace ss {

	void SamsonController::run()
	{
		// Testing receiving packets and sending to all the workers
		
		while( ! network.ready() )
		{
			std::cout << "Waiting for the network interface to be ready" << std::endl;
			sleep(1);
		}
		
		while( true )
		{
			
			std::vector<EndPoint> workers = network.samsonWorkersPoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<EndPoint>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network.send(&p, *e, NULL);
			
			sleep(1);
		}
		
	}
	
	void SamsonController::receive( Packet *p , EndPoint fromEndPoint )
	{
		// Do something with it
		std::cout << "Packet received from " << fromEndPoint.str() << " -> " << p->str();
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}