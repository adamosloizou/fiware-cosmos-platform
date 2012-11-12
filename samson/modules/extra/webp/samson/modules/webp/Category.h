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

#ifndef _H_SAMSON_webp_Category
#define _H_SAMSON_webp_Category

#include "au/string.h"

#include <samson/modules/webp/Category_base.h>


namespace samson{
namespace webp{


	class Category : public Category_base
	{

	public:

	   std::string get_name()
	   {
		  std::string _name = au::str("%lu_%s" , id.value , name.value.c_str() );
		  // Replace spaces
		  for( size_t i = 0 ; i < _name.length() ; i++ )
			 if( _name[i] == ' ' )
				_name[i] = '_';
		  return _name;
	   }
	};


} // end of namespace samson
} // end of namespace webp

#endif
