
#ifndef _H_SS_TRACER
#define _H_SS_TRACER


#include <stdarg.h>             /* va_start, va_arg, va_end                  */
#include <stdio.h>
#include "samson/module/Log.h"

namespace samson
{

	// Interface to send traces to the platform
	
   class Tracer
   {
   public:

	   
	   /**
		Unique function to send traces to the parent process
		*/
	   virtual void trace( LogLineData *logData )=0;

	   /** 
		Report an error and exit the function
		*/
	   virtual void setUserError( std::string message )=0; 
	   
	   /**
		Handy function used by macros to get the message string
		*/
	   static char* print( const char* format, ...)
	   {
		   va_list        args;
		   char           vmsg[1024];
		   char*          allocedString;
		   char*          nl;
		   
		   /* "Parse" the varible arguments */
		   va_start(args, format);
		   
		   /* Print message to variable */
		   vsnprintf(vmsg, sizeof(vmsg), format, args);
		   vmsg[1023] = 0;
		   va_end(args);
		   
		   if ((nl = strchr(vmsg, '\n')) != NULL)
			   *nl = 0;
		   
		   allocedString = (char*) strdup(vmsg);
		   return allocedString;
		   
		  // trace(channel, allocedString );
		  // free( allocedString );
		   
	   }
	   
	   
   };
	
	
	
}

#endif
