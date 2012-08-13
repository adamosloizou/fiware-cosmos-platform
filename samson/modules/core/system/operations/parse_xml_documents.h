
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_system_parse_xml_documents
#define _H_SAMSON_system_parse_xml_documents


#include <samson/module/samson.h>
#include <samson/modules/system/Value.h>


namespace samson{
namespace system{


	class parse_xml_documents : public samson::Parser
	{

	   samson::system::ValueContainer keyContainer;
	   samson::system::ValueContainer valueContainer;

	   samson::system::String trace;
	   samson::system::Void value_void;

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  output: system.Value system.Value
//  
// helpLine: Parse xml documents and generate system.Values as maps
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		   valueContainer.value->set_double( 1 );
		}

        
		void run( char *data , size_t length , samson::KVWriter *writer )
		{

            // Limiter sequence
            const char* sequence = "<?xml";
            size_t sequence_len = strlen( sequence );
            
            size_t pos_begin = 0; // Position where we begin
            size_t pos = 0;
            
            while( pos < length )
            {
                
                if( memcmp( &data[pos] , sequence , sequence_len ) == 0 )
                {
                    // Sequence found
                    if( pos > pos_begin )
                    {
                        keyContainer.value->set_string( &data[pos_begin] , pos - pos_begin );
                        writer->emit( 0 , keyContainer.value , valueContainer.value );
                    }
                    
                    pos_begin = pos;
                    pos = pos_begin+1;
                        
                }
                else
                    pos++;
            }
            

            // emit the last one
            if( pos > pos_begin )
            {
                keyContainer.value->set_string( &data[pos_begin] , pos - pos_begin );
                writer->emit( 0 , keyContainer.value , valueContainer.value );
            }
            

		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace system
} // end of namespace samson

#endif