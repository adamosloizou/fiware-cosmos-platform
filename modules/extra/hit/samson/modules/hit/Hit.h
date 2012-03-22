
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_hit_Hit
#define _H_SAMSON_hit_Hit

#include <time.h>
#include <limits.h>
#include <stdint.h>

#include <samson/modules/hit/Hit_base.h>


namespace samson{
namespace hit{


	class Hit : public Hit_base
	{

	public:

	   void init( )
	   {
		  init( ""  , 0 );
	   }

	   void init( std::string _concept , time_t ts )
	   {
		  concept.value = _concept;
		  count.value = 0;
		  time.value = ts;
	   }
/*
	   void setTime( time_t current_ts )
	   {
		  if( current_ts < time.value )
			 return;

          ::time_t diff =  current_ts - time.value;
          count.value = (( (double) count.value ) * pow( 0.99666666  , (double) diff ));
		  time.value = current_ts;
	   }
*/


	   void setTime( time_t current_ts , size_t time_span )
	   {
		  if( current_ts < time.value )
			 return;

          ::time_t diff =  current_ts - time.value;
		  double f = ((double)(time_span - 1)) / ((double) time_span); 

          count.value = (( (double) count.value ) * pow( f  , (double) diff ));
		  time.value = current_ts;
	   }

	   std::string getConcept()
	   {
		  size_t pos = concept.value.find(' ');

		  if( ( pos == std::string::npos ) || ( pos == 0 ))
			 return concept.value;

		  //std::string category_concept = concept.value.substr( 0 , pos );
		  std::string name = concept.value.substr( pos+1 , std::string::npos );
		  
		  return name;
	   }

	   std::string getCategory()
	   {
		  size_t pos = concept.value.find(' ');
		  
		  if( ( pos == std::string::npos ) || ( pos == 0 ))
			 return "top";
		  
		  std::string category_concept = concept.value.substr( 0 , pos );
		  //std::string name = concept.value.substr( pos+1 , std::string::npos );
		  
		  return category_concept;
	   }
	};


} // end of namespace samson
} // end of namespace hit

#endif