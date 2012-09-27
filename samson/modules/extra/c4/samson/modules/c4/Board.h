
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_c4_Board
#define _H_SAMSON_c4_Board


#include <samson/modules/c4/Board_base.h>
#include "extra/c4/Board.h"

namespace samson{
namespace c4{
	class Board : public Board_base
	{
    
  public:
    
    Board()
    {
      // Init board with initial position
      white.value = 0;
      black.value = 0;

	  // Turn is white
	  turn.value = C4_WHITE;
    }
    


	std::string str()
	{
	   std::ostringstream output;
	   au::c4::Board b( white.value , black.value );	   
	   output << b.str();
	   if( turn.value == C4_WHITE )
		  output << "Turn white\n";
	   else if( turn.value == C4_BLACK )
		  output << "Turn black\n";
	   else
		  output << "Unknown turn";
	   return output.str();
	}
    
    
	};


} // end of namespace samson
} // end of namespace c4

#endif
