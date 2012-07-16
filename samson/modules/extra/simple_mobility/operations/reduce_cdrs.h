
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_simple_mobility_reduce_cdrs
#define _H_SAMSON_simple_mobility_reduce_cdrs

#include "au/string.h"

#include <samson/module/samson.h>
#include <samson/modules/mobility/Position.h>
#include <samson/modules/mobility/Record.h>


#include <samson/modules/simple_mobility/User.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

#include <samson/modules/level/LevelInt32.h>


namespace samson{
namespace simple_mobility{


	class reduce_cdrs : public samson::Reduce
	{

	   samson::system::UInt key;              // User identifier
	   samson::simple_mobility::User user;    // State information

	   samson::mobility::Record  record;      // Input record

	   samson::system::String message;        // Message emited at the output
	   
	   samson::level::LevelInt32 *level_int_32;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt mobility.Record
input: system.UInt simple_mobility.User
output: system.UInt system.String
output: system.String system.Int32
output: system.UInt simple_mobility.User

helpLine: Update internal state
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		   level_int_32 = new samson::level::LevelInt32( writer , 1 );
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
           if ( inputs[1].num_kvs == 0 )
           {
              // New user state
              user.init();
              key.parse( inputs[0].kvs[0]->key );
           }
           else
           {
			  // Previous state
			  user.parse( inputs[1].kvs[0]->value ); // We only consider one state
			  key.parse( inputs[1].kvs[0]->key );
           }

		   if( inputs[0].num_kvs > 0)
		   {

			  // Update in the position
			  record.parse( inputs[0].kvs[ inputs[0].num_kvs - 1 ]->value );



/*
			  double distance = user.position.distance( &record.position );
			  message.value = au::str("Received a cdrs for user %lu moving %f %s --> %s" , key.value , distance,  user.position.str().c_str() , record.position.str().c_str() );
			  writer->emit( 0 , &key , &message );
*/

			  // Emit whatever is necessary
			  if( user.isTracking() )
			  {
/*
                 // All notification
				 message.value = au::str("Update position of user %lu to [%d,%d] %s", key.value , position.x.value , position.y.value , position.time.str().c_str() );
				 writer->emit( 0 , &key,  &message );				 
*/
			  }

			  for (int i = 0 ; i < user.areas_length ; i++)
			  {
				 bool in_previous = user.areas[i].isInside( &user.position );
				 bool in_now = user.areas[i].isInside( &record.position );


/*
				 double d1 = user.areas[i].center.distance( &user.position );
				 double d2 = user.areas[i].center.distance( &record.position );
				 message.value = au::str("Area %s at %s ( %f %f --> %d %d)" , user.areas[i].name.value.c_str() , user.areas[i].center.str().c_str() , d1 ,d2 , in_previous , in_now );					
				 writer->emit( 0 , &key,  &message );
*/
				 
				 if( !in_previous && in_now )
				 {
					level_int_32->push( user.areas[i].name.value , 1 );

					message.value = au::str("User enters area '%s' when moving from %s to %s" , user.areas[i].str().c_str() , user.position.str().c_str() , record.position.str().c_str() );
					writer->emit( 0 , &key,  &message );					

				 }
				 
				 if( in_previous && !in_now )
				 {
					level_int_32->push( user.areas[i].name.value , -1 );
					
			   	    message.value = au::str("User  leaves '%s' when moving from %s to %s" , user.areas[i].str().c_str() , user.position.str().c_str() , record.position.str().c_str() );
				    writer->emit( 0 , &key,  &message );
				 }
				 
			  }
			  
	            for (int i = 0 ; i < user.cells_length ; i++)
	              {
	                 bool in_previous = user.cells[i].cellId.value == user.cellId.value;
	                 bool in_now = user.cells[i].cellId.value == record.cellId.value;

	                 if( !in_previous && in_now )
	                 {
	                    level_int_32->push( user.cells[i].name.value , 1 );

	                    message.value = au::str("User enters cellIdLabelled '%s' when moving from %s to %s" , user.cells[i].str().c_str() , user.cellId.str().c_str() , record.cellId.str().c_str() );
	                    writer->emit( 0 , &key,  &message );

	                 }

	                 if( in_previous && !in_now )
	                 {
	                    level_int_32->push( user.cells[i].name.value , -1 );

	                    message.value = au::str("User leaves cellIdLabelled '%s' when moving from %s to %s" , user.cells[i].str().c_str() , user.cellId.str().c_str() , record.cellId.str().c_str() );
	                    writer->emit( 0 , &key,  &message );
	                 }

	              }

			  // update state
			  user.position.copyFrom( &record.position );
			  user.cellId.value =  record.cellId.value;
			  
		   }
		
		   
		// Emit the state at the output
		   writer->emit( 2 , &key , &user );
		   
		   
		}
		
		void finish(samson::KVWriter *writer )
		{
           level_int_32->flush();
           delete level_int_32;
           level_int_32 = NULL;
		}
		
		
		
	};
   

} // end of namespace simple_mobility
} // end of namespace samson

#endif
