
#ifndef _H_ENVIRONMENT_OPERATIONS
#define _H_ENVIRONMENT_OPERATIONS


#include "samson/module/Environment.h"			// samson::Environment
#include "samson/common/samson.pb.h"					// samson::network::...
#include "samson/common/coding.h"                     // KVInfo

namespace samson {

	void copyEnviroment( Environment* from , network::Environment * to );
	
	void copyEnviroment( const network::Environment & from , Environment* to  );
    
    void copy( KVInfo * from , network::KVInfo* to);
    
    void copy( FullKVInfo * from , network::KVInfo* to);
 
 
    std::string getStatus( network::StreamOperation *streamOperation );
    
    // Get a debug string for generic messages
    std::string strMessage( network::Message *message );
    
}

#endif
