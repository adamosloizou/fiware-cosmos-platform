
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_fakeURLs_fixed
#define _H_SAMSON_url_fakeURLs_fixed


#include <samson/module/samson.h>
#include "samson/modules/url/URL.h"
#include <samson/modules/system/Void.h>

/*
        out url.URL system.Void
        helpLine "Generate url.num_users_per_user URLs for url.num_users fixed, for testing"
*/

namespace samson{
namespace url{


	class fakeURLs_fixed : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
			URL url;
			samson::system::Void v;

			// Recover the number of users in this demo
			size_t num_users  = environment->getSizeT( "url.num_users" ,  1000000 );

			// Recover the number of urls per user
			size_t num_urls_per_user = environment->getSizeT( "url.num_urls_per_user" ,  100 );

			// Recover the number of paths per server
			size_t num_paths_per_server = environment->getSizeT( "url.num_paths_per_server" ,  10 );

			// Recover the number of servers from the environment
			size_t num_servers = environment->getSizeT( "url.num_servers" ,  1000000 );

			OLM_T(LMT_User06, ("Generating URLs per %lu users\n", num_users));
			for (size_t u = 0 ; u < num_users ; u++ )
			{
				if( !(u%1000) )
					operationController->reportProgress( (double) u / (double) num_users );

				OLM_T(LMT_User06, ("Generating URLs per %lu num_urls_per_user\n", num_urls_per_user));
				for ( size_t t = 0 ; t < num_urls_per_user ; t++ )
				{
					std::ostringstream o;
					o << "path_to_somewhere_" << ((u*num_urls_per_user + t)%num_paths_per_server);
					url.path.value = o.str();
					o.str(""); //Clear content;

					size_t server = (u*num_urls_per_user + t)%num_servers;
					o << "ServerName_" << (server%num_servers);
					url.server.value = o.str();

					url.type.value = (unsigned char) t%10;
					url.user.value = (t*u+u/2)%num_users;

					writer->emit( 0 , &url, &v);
				}
			}
		}


	};


} // end of namespace samson
} // end of namespace url

#endif