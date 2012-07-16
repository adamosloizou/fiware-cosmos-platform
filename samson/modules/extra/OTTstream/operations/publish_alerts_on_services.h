
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_OTTstream_publish_alerts_on_services
#define _H_SAMSON_OTTstream_publish_alerts_on_services


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/UserActivity.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace OTTstream{


	class publish_alerts_on_services : public samson::ParserOut
	{

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.UInt OTTstream.UserActivity  
//  
// helpLine: Offers a textual representation of alerts generated at red_commands_on_service_profile
//  END_INFO_MODULE

		void init( TXTWriter *writer )
		{
		}

		void run( KVSetStruct* inputs , TXTWriter *writer )
		{
		    samson::system::UInt serviceId;
		    samson::OTTstream::UserActivity user_activity;

		    char line[1024];

		    for (uint64_t i = 0; (i < inputs[0].num_kvs) ; i++ )
            {
		       serviceId.parse( inputs[0].kvs[i]->key );
		       user_activity.parse( inputs[0].kvs[i]->value );

               sprintf( line , "service=%lu userId=%lu cluster=%lu count=%lu last_access='%s'\n" , serviceId.value , user_activity.user.userId.value, user_activity.user.clusterId.value, user_activity.count.value, user_activity.last_timestamp.str().c_str());
               //LM_M(("Published: %s", line));

               writer->emit( line );
            }


		}

		void finish( TXTWriter *writer )
		{
		}



	};


} // end of namespace OTTstream
} // end of namespace samson

#endif
