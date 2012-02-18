
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_webp_CategoryHits
#define _H_SAMSON_webp_CategoryHits


#include <samson/modules/webp/CategoryHits_base.h>


namespace samson{
namespace webp{


	class CategoryHits : public CategoryHits_base
	{

	public:
	   
	   void setFrom( Category* _category )
	   {
		  // Copy category description
		  category.copyFrom( _category );
		  
		  // Init coutners
		  hits.value = 0 ;
		  current.value = 0;
	   }

	   void hit()
	   {
		  // Add a hit
		  hits.value++;
	   }

	};


} // end of namespace samson
} // end of namespace webp

#endif
