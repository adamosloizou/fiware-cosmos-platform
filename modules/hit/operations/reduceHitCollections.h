
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_hit_reduceHitCollections
#define _H_SAMSON_hit_reduceHitCollections


#include <samson/module/samson.h>
#include <samson/modules/hit/common.h>
#include <samson/modules/hit/Hit.h>
#include <samson/modules/hit/HitCollection.h>
#include <samson/modules/system/String.h>


#include "HitCollectionManager.h"

namespace samson{
namespace hit{


	class reduceHitCollections : public samson::Reduce
	{

	   samson::system::String concept;


	public:

	   reduceHitCollections()
	   {
	   }


	   ~reduceHitCollections()
	   {
	   }

//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.String hit.Hit  
//  input: system.String hit.HitCollection  
//  output: system.String hit.HitCollection
//  output: system.String hit.HitCollection
// 
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

		   HitCollectionManager* manager;
		   
		   // Read the state to start
		   samson::hit::HitCollection hit_collection;

		   if( inputs[1].num_kvs > 0 )
		   {
			  concept.parse( inputs[1].kvs[0]->key );
			  hit_collection.parse( inputs[1].kvs[0]->value );

			  manager = new HitCollectionManager( concept.value );

			  for (int i = 0 ; i < hit_collection.hits_length ; i++ )
				 manager->add( &hit_collection.hits[i] );

		   }
		   else
		   {
			  // take the concept from the first input
			  concept.parse( inputs[0].kvs[0]->key );
			  manager = new HitCollectionManager( concept.value );
		   }




		   // Parse all the inputs
		   samson::hit::Hit hit;
		   for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		   {
			  hit.parse( inputs[0].kvs[i]->value );
			  manager->add( &hit );
		   }


		   // Emit the top elements at the output
		   samson::hit::HitCollection output_hit_collection;
		   for (int i = 0 ; i < NUM_TOP_ITEMS ; i++ )
		   {
			  if( manager->top_hits[i]->count.value > 0 )
				 output_hit_collection.hitsAdd()->copyFrom( manager->top_hits[i] );
		   }

		   // Emitted at both channels 
		   writer->emit( 0 , &concept , &output_hit_collection );
		   writer->emit( 1 , &concept , &output_hit_collection );


		   delete manager;

		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace hit
} // end of namespace samson

#endif
