
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_hit_Hit
#define _H_SAMSON_hit_Hit


#include <samson/modules/hit/Hit_base.h>


namespace samson{
namespace hit{


	class Hit : public Hit_base
	{

	public:

	   std::string extractCategoryConcept()
	   {
		  size_t pos = concept.value.find('_');
		  
		  if( pos == std::string::npos )
			 return "top";
		  
		  if( pos == 0 )
		  {
			 // Word starts with "_"
			 
			 std::string name = concept.value.substr( pos+1 , std::string::npos );
			 concept.value = name;
			 
			 return "top";
		  }
		  
		  
		  std::string category_concept = concept.value.substr( 0 , pos );
		  std::string name = concept.value.substr( pos+1 , std::string::npos );
		  
		  concept.value = name;
		  return category_concept;
	   }
	};


} // end of namespace samson
} // end of namespace hit

#endif
