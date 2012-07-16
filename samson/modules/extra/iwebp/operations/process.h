
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_iwebp_process
#define _H_SAMSON_iwebp_process


#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>

#include <samson_system/ProcessComponentManager.h>
#include <samson_system/HubMapTimeProcess.h>
#include <samson_system/HitCountProcess.h>


namespace samson{
namespace iwebp{


	class process : public samson::Reduce
	{

	    samson::system::ProcessComponentsManager process_components_manager_;
	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.Value system.Value  
//  input: system.Value system.Value  
//  output: system.Value system.Value
//  output: system.Value system.Value
//  output: system.Value system.Value
//  
//  helpLine: Bulk process operation for instant individual web profiling. Instances added at init()
//  extendedHelp: 		Bulk process operation for instant individual web profiling. Instances added at init()
//  		As all the system.Value process(), one of the outputs is fedback to the stream input
// 
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		    process_components_manager_.add(new samson::system::HubMapTimeProcess("agregatedKey", "globalTops_short", "globalTopsCategory" ));
            process_components_manager_.add(new samson::system::HitCountProcess("globalTops_short", 300, 100, "globalTops_short_count", "globalTops_medium" ));
            process_components_manager_.add(new samson::system::HitCountProcess("globalTops_medium", 3600, 100, "globalTops_medium_count", "globalTops_long" ));
            process_components_manager_.add(new samson::system::HitCountProcess("globalTops_long", 24*3600, 100, "globalTops_long_count", "globalTopsCategory" ));

		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		    process_components_manager_.process(inputs, writer);
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace iwebp
} // end of namespace samson

#endif
