
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_webp_emit_hits_for_urls
#define _H_SAMSON_webp_emit_hits_for_urls


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

#include "comscore/SamsonComscoreDictionary.h"


namespace samson{
namespace webp{


	class emit_hits_for_urls : public samson::Map
	{

        samson::comscore::SamsonComscoreDictionary samson_comscore_dictionary;
        
        samson::system::String key;
        samson::system::UInt value;

        samson::system::String output_key;
        
	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.String system.UInt  
//  output: system.String system.UInt
//  
// helpLine: Process urls at the input and Emit all strings to be tracked: url , domain , category,...
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
            samson_comscore_dictionary.read( "/var/comscore/samson_comscore_dictionary.bin" );
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
            for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
            {
                // Recover original key&value
                key.parse( inputs[0].kvs[i]->key );
                value.parse( inputs[0].kvs[i]->value );

                // Remove protocol information ( if any )
                size_t pos = key.value.find("://");
                if( pos != std::string::npos )
                    key.value = key.value.substr( pos+3 );
                
                // Emit an output to track popular urls
                output_key.value = au::str("url %s" , key.value.c_str() );
                writer->emit( 0 , &output_key , &value );

                // Extract domain to get the most popular domains
                {
                    size_t pos = key.value.find("/");
                    
                    if( pos == std::string::npos )
                        output_key.value = au::str("domain %s" , key.value.c_str() );
                    else
                        output_key.value = au::str("domain %s" , key.value.substr(0,pos).c_str() );
                    writer->emit( 0 , &output_key , &value );
                }
                
                // Get all categories for this url
                std::vector<uint> categories_ids = samson_comscore_dictionary.getCategories( key.value.c_str() );
                for ( size_t i = 0 ; i < categories_ids.size() ; i++ )
                {
                    std::string name = samson_comscore_dictionary.getCategoryName( categories_ids[i] );
                    
                    // Replace spaces by "_"s
                    for ( size_t s = 0 ; s < name.length() ; s++ )
                        if( name[s] == ' ')
                            name[s] = '_';
                    
                    // Emit an output for each category that has received a hit
                    output_key.value = au::str("category %s" , name.c_str() );
                    writer->emit( 0 , &output_key , &value );
                }
                
            }
            
            
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace webp
} // end of namespace samson

#endif
