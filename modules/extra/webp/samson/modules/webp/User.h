
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_webp_User
#define _H_SAMSON_webp_User


#include <samson/modules/webp/User_base.h>
#include <samson/modules/webp/Category.h>


namespace samson{
namespace webp{


	class User : public User_base
	{

	public:

	   void init( std::string _id )
	   {
		  id.value = _id;
		  category_hitsSetLength(0);
		  last_log.init();
		  last_update.value = 0;
	   }

	   void update( )
	   {
		  if( last_update.value != 0 )
		  {
			 size_t secs = last_update.secondsSinceThisTime();
			 update( secs );
		  }
		  last_update.setCurrentTime();
	   }

	   void update( size_t seconds )
	   {
          // Update all categories considered so far
          for( int i = 0 ; i < category_hits_length ; i++ )
			 category_hits[i].update( seconds );
		  

	   }

	   void add_hit( Category* category )
	   {
		  // Add a hit for this category
		  for( int i = 0 ; i < category_hits_length ; i++ )
		  {
			 if( category_hits[i].category.id.value == category->id.value )
			 {
				// Update the hit count
				category_hits[i].hit();
				return;
			 }
		  }

		  // Add a new category
		  CategoryHits* category_hits = category_hitsAdd();
		  category_hits->setFrom( category );
		  category_hits->hit();
	   }
	   
	};


} // end of namespace samson
} // end of namespace webp

#endif