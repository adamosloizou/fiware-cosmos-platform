
#include "au/containers/StringVector.h"
#include "au/utils.h"
#include "au/tables/Table.h"

#include "samson/common/ports.h"
#include "samson/network/NetworkConnection.h"

#include "DelilahNetwork.h" // Own interface

namespace samson {

    DelilahNetwork::DelilahNetwork( std::string connection_type , size_t delilah_id , NetworkInterfaceReceiver * receiver )
    : CommonNetwork( NodeIdentifier( DelilahNode , delilah_id ) , receiver , NULL , (size_t) -1 )
    {
        LM_V(("DelilahNetwork %s" , au::code64_str(delilah_id).c_str() ));
        
        // Save connection type string  to be send in all hello messages
        connection_type_ = connection_type;
    }
    
    
    
}
