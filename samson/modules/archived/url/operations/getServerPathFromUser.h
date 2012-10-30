/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_getServerPathFromUser
#define _H_SAMSON_url_getServerPathFromUser


#include <samson/module/samson.h>


/***************************************************************
reduce getServerPathFromUser
{
	in system.UInt		url.ServerPath   	    # User - ServerPath
	out system.UInt		url.ServerPathVector	# User - ServerPathVector

	helpLine "Aggregate ServerPath per User, to form a ServerPathVector"
}
****************************************************************/

namespace samson{
namespace url{


	class getServerPathFromUser : public samson::Reduce
	{
		samson::system::UInt userId;

		ServerPath serverPath;
		ServerPath serverPathPrev;
		ServerPathCount serverPathCount;


	public:

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			bool first = true;
			size_t n_items = 0;
			size_t serverId;
			ServerPathVector paths;

			//OLM_T(LMT_User06,("For inputs[0].num_kvs:%d", inputs[0].num_kvs));

			if (inputs[0].num_kvs == 0)
			{
				return;
			}

			userId.parse(inputs[0].kvs[0]->key);
			//OLM_T(LMT_User06,("For userId:%d  inputs[0].num_kvs:%d", userId.value, inputs[0].num_kvs));


			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				serverPath.parse(inputs[0].kvs[i]->value);
				serverId = serverPath.server.value;
				//OLM_T(LMT_User06,("For userId:%d  serverId:%d", userId.value, serverId));

				if (first || ((serverPath.server == serverPathPrev.server) && (!serverPath.path.value.compare(serverPathPrev.path.value))))
				{
					n_items++;
					//OLM_T(LMT_User06,("userId(%d) serverId(%d) path:%s. Increment count to %d", userId.value, serverId, serverPath.path.value.c_str(), n_items));

					first = false;
				}
				else
				{
					//OLM_T(LMT_User06,("Add userId(%d) serverId(%d) path:%s count: %d", userId.value, serverId, serverPath.path.value.c_str(), n_items));

					serverPathCount.server = serverPath.server;
					serverPathCount.path = serverPath.path;
					serverPathCount.count = n_items;
					paths.serverPathAdd()->copyFrom(&serverPathCount);
					n_items = 1;
				}
				serverPathPrev = serverPath;
			}

			//OLM_T(LMT_User06,("Add last userId(%d) serverId(%d) path:%s count: %d", userId.value, serverId, serverPath.path.value.c_str(), n_items));

			serverPathCount.server = serverPath.server;
			serverPathCount.path = serverPath.path;
			serverPathCount.count = n_items;
			paths.serverPathAdd()->copyFrom(&serverPathCount);

			writer->emit(0, &userId, &paths);
		}
	};


} // end of namespace samson
} // end of namespace url

#endif
