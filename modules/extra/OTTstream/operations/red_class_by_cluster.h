
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_OTTstream_red_class_by_cluster
#define _H_SAMSON_OTTstream_red_class_by_cluster


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/OTTstream/User.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace OTTstream{


	class red_class_by_cluster : public samson::Reduce
	{

	    samson::system::UInt userId;
	    samson::system::UInt clusterId;
	    samson::OTTstream::ServiceHit input_hit;
	    samson::OTTstream::UserHit output_hit;

	    samson::system::UInt serviceId;
	    samson::OTTstream::User user;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.UInt OTTstream.ServiceHit  
//  input: system.UInt system.UInt  
//  output: system.UInt OTTstream.UserHit
//  
// helpLine: Reduce with user classification to add clusterId information
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
	        if (inputs[0].num_kvs == 0)
	        {
	            return;
	        }
	        userId.parse(inputs[0].kvs[0]->key);

	        if( inputs[1].num_kvs == 0)
	         {
	            //LM_W(("No cluster info for userId:%lu", userId.value));

	            // No cluster info for userId. Assigned core cluster=0
	            clusterId.value = 0;
	         }
	        else
	        {
	            if  (inputs[1].num_kvs > 1)
	            {
	                LM_E(("More than one clusterId for userId:%lu", userId.value));
	            }
	            clusterId.parse(inputs[1].kvs[0]->value);
	        }
	        user.userId = userId;
	        user.clusterId = clusterId;
	        user.sensitive.value = 0;

	        //LM_M(("Processing userId:%lu with clusterId:%lu and inputs[0].num_kvs:%lu", userId.value, clusterId.value, inputs[0].num_kvs));
	        for ( uint64_t i = 0 ; i< inputs[0].num_kvs; i++)
	        {
	            input_hit.parse(inputs[0].kvs[i]->value );

	            serviceId = input_hit.serviceId;
	            output_hit.user = user;
	            output_hit.timestamp = input_hit.timestamp;

	            writer->emit(0, &serviceId, &output_hit);
	        }
	        return;
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace OTTstream
} // end of namespace samson

#endif
