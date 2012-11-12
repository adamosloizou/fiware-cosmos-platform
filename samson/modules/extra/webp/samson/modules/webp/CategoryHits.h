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
		  current.value++;
	   }

	   void update( size_t seconds )
	   {
		  // Apply a forgetting factor to "current" counter
		  double time_span = 300;
		  double factor = ( time_span - 1 ) / time_span;
		  current.value = current.value * pow( factor  , (int) seconds );
	   }
	   

	};


} // end of namespace samson
} // end of namespace webp

#endif
