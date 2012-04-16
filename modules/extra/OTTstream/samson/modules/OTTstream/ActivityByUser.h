
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_OTTstream_ActivityByUser
#define _H_SAMSON_OTTstream_ActivityByUser


#include <samson/modules/OTTstream/ActivityByUser_base.h>


namespace samson{
namespace OTTstream{


	class ActivityByUser : public ActivityByUser_base
	{
	  public:
	        void init()
	        {
	            servActivitySetLength(0);
	        }

	        void addService(samson::system::UInt serviceId, samson::system::TimeUnix timestamp, int inCount)
	        {
	            samson::OTTstream::ServiceActivity *service = servActivityAdd();
	            service->serviceId = serviceId;
	            service->timestamp = timestamp;
	            service->inCount = inCount;
	        }
	};


} // end of namespace samson
} // end of namespace OTTstream

#endif
