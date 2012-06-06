
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_OTTstream_red_update_alerts
#define _H_SAMSON_OTTstream_red_update_alerts


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/UserActivity.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace OTTstream{


	class red_update_alerts : public samson::Reduce
	{

	public:

	    samson::OTTstream::ActivityByService alerts_history;
	    samson::system::UInt alertId;
	    samson::OTTstream::UserActivity alert;

//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.UInt OTTstream.UserActivity  
//  input: system.UInt OTTstream.ActivityByService
//  output: system.UInt OTTstream.ActivityByService
// 
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		    if ( inputs[1].num_kvs == 0 )
		    {
		      // New alerts state
		        alerts_history.Init();
		        alertId.parse( inputs[0].kvs[0]->key );
		    }
		    else
		    {
		      // Previous state
		        alerts_history.parse( inputs[1].kvs[0]->value ); // We only consider one state
		        alertId.parse( inputs[1].kvs[0]->key );
		      if (inputs[1].num_kvs > 1)
		      {
		        LM_W(("Multiple alerts_states(%lu) for alertId:%lu", inputs[1].num_kvs, alertId.value));
		      }
		    }

		    LM_M(("For alertId:%lu, detected:%lu alerts", alertId.value,  inputs[0].num_kvs ));
		    for (uint64_t i = 0; (i <  inputs[0].num_kvs) ; i++ )
		    {
		        alert.parse(inputs[0].kvs[i]->value);
		      bool user_found = false;

		      // First inefficient try with vectors (we would like having lists or maps)
		      // As we insert new users at the end, will look first at the end of the vector

		      for (int j= (alerts_history.user_activity_length - 1); ((j >= 0) && (user_found == false)); j--)
		      {
		        if (alert.user.userId.value == alerts_history.user_activity[j].user.userId.value)
		        {
		          if (alerts_history.user_activity[j].last_timestamp.value < alert.last_timestamp.value)
		          {
		              alerts_history.user_activity[j].last_timestamp.value = alert.last_timestamp.value;
		          }
		          alerts_history.user_activity[j].count.value++;
		          user_found = true;
		        }


		      }

		      if (user_found == false)
		      {
		        //LM_M(("For alertId:%d new userId:%lu at time:%lu", alertId.value, alert.user.userId.value, static_cast<unsigned long>(alert.timestamp.value)));
		          alerts_history.AddUser(alert.user, alert.last_timestamp, 1);
		      }
		    }
		    //LM_M(("Update alertId:%lu state with %d users", alertId.value, alerts_history.user_activity_length));
		    writer->emit(0, &alertId, &alerts_history);
		    //LM_M(("Update alertId:%lu updated", alertId.value));
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace OTTstream
} // end of namespace samson

#endif