
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_gg_select_cdrs_for_price
#define _H_SAMSON_gg_select_cdrs_for_price


#include <samson/module/samson.h>

#include "samson/modules/gg/CDR.h"       // gg::CDR                                                                                     
#include "samson/modules/system/UInt.h"  // system::UInt   

namespace samson{
namespace gg{


	class select_cdrs_for_price : public samson::Map
	{

	public:

	  double min_price;
	  double max_price;


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt gg.CDR  
output: system.UInt gg.CDR

helpLine: Select a set of cdrs acording to the price. Use gg.min_price and gg.max_price to select range of valid prices
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

                  min_price = environment->getDouble( "gg.min_price" ,  0 );
                  max_price = environment->getDouble( "gg.max_price" ,  1000 );

		  system::UInt key;
		  gg::CDR value;

		  for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		  {
		    key.parse( inputs[0].kvs[i]->key );
		    value.parse( inputs[0].kvs[i]->value );
		    
		    if( ( value.price.value >= min_price ) && ( value.price.value <= max_price ) )
		      writer->emit( 0 , &key , &value );

	          }

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace gg
} // end of namespace samson

#endif