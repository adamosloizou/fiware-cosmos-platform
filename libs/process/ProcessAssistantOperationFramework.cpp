#include <assert.h>									// assert(.)

#include "MemoryManager.h"							// ss::SharedMemoryItem
#include "ProcessWriter.h"							// ss::ProcessWriter
#include "ProcessAssistantOperationFramework.h"		// Own interface
#include "WorkerTaskManager.h"						// ss::WorkerTaskItemWithOutput


namespace ss {


	ProcessAssistantOperationFramework::ProcessAssistantOperationFramework(ProcessAssistant *_processAssistant , network::ProcessMessage m  ) : OperationFramework( m )
	{
		processAssistant = _processAssistant;
		assert( processAssistant );

		// Setup correclt the process writer
		if( pw )
			pw->setProcessAssistant( processAssistant );
		if( ptw )
			ptw->setProcessAssistant( processAssistant );
	}

	/**
	 Function used to add input files for each one of the inputs ( prior to setup call)
	 */
	void ProcessAssistantOperationFramework::addInputFiles()
	{
	}


	void ProcessAssistantOperationFramework::flushOutput( WorkerTaskItem *item )
	{
		// Flush the pw buffer to emit the key-values in the output buffer
		if( pw )
			pw->FlushBuffer(item);
		if( ptw )
			ptw->FlushBuffer(item);
	}
}

