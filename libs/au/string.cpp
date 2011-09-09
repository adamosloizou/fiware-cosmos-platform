
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <cstdarg>

namespace au {
    
    std::string tabs(int t)
    {
        std::ostringstream output;
        for ( int i = 0 ; i < t ; i ++)
            output << "\t";
        return output.str();
    }
    
    std::string percentage_string( double p )
	{
		char line[2000];
		sprintf(line, "%05.1f%%",p*100);
		return std::string(line);
	}
    
	std::string percentage_string( double value , double total )
	{
		if ( total == 0)
			return percentage_string( 0 );
		else
			return percentage_string( value / total);
	}
	
	
	std::string int_string(int value, int digits)
	{
		
		char line[100];
		char format[100];
		
		sprintf(format, "%%%dd",digits);
		sprintf(line, format,value ); 
		return std::string(line);
	}
	
	std::string time_string( int seconds )
	{
		int minutes = seconds/60;
		seconds -= minutes*60;
		
		int hours = minutes/60;
		minutes -= hours*60;
		
		char line[1000];
		sprintf(line, "%02d:%02d:%02d" , hours , minutes , seconds	);
		return std::string(line);
		
	}
    
    std::string progress_bar( double p , int len )
    {
        std::ostringstream output;
        
        
        if( p < 0 )
            p = 0;
        if( p > 1 )
            p = 1;
        
        int pos = len * p;
        
        output << " [ ";
        for (int s = 0 ; s < pos ; s++ )
            output << "*";
        for (int s = pos ; s < len ; s++ )
            output << ".";
        output << " ] ";
        
        return  output.str();
    }
    
    std::string progress_bar( double p , int len , char c , char c2 )
    {
        std::ostringstream output;
        
        
        if( p < 0 )
            p = 0;
        if( p > 1 )
            p = 1;
        
        int pos = len * p;
        
        output << " [ ";
        for (int s = 0 ; s < pos ; s++ )
            output << c;
        for (int s = pos ; s < len ; s++ )
            output << c2;
        output << " ] ";
        
        return  output.str();
    }    
    
    std::string double_progress_bar( double p1 , double p2 , char c1 ,char c2 , char c3, int len )
    {
        std::ostringstream output;
        
        
        if( p1 < 0 )
            p1 = 0;
        if( p1 > 1 )
            p1 = 1;
        
        if( p2 < 0 )
            p2 = 0;
        if( p2 > 1 )
            p2 = 1;
        
        if( p2 < p1 )
            p2 = p1;    // no sense
        
        
        int pos1 = (double)len * p1;
        int pos2 = (double)len * p2;
        
        output << " [ ";
        
        for (int s = 0 ; s < pos1 ; s++ )
            output << c1;
        
        for (int s = pos1 ; s < pos2 ; s++ )
            output << c2;
        
        for (int s = pos2 ; s < len ; s++ )
            output << c3;
        
        output << " ] ";
        
        return  output.str();
    }
    
    void find_and_replace( std::string &source, const std::string find, std::string replace ) 
    {
        size_t pos = 0;
        
        //LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
        pos = source.find( find , pos );
        //LM_M(("Position found %lu bytes" , find.length() ));
        
        while(pos != std::string::npos )
        {
            source.replace( pos, find.length(), replace );
            
            // Go forward in the input string
            pos += replace.length();
            
            //LM_M(("Finding string of %d bytes at position %lu of a string with length %lu" , find.length() , pos , source.length() ));
            pos = source.find( find , pos );
            //LM_M(("Position found %lu bytes" , find.length() ));
            
            
        }
    }
    
    std::string getRoot( std::string& path )
    {
        size_t pos = path.find( "." , 0 );
        
        if( pos == std::string::npos )
            return path;
        
        return path.substr( 0 , pos );
    }
    
    std::string getRest( std::string& path )
    {
        size_t pos = path.find( "." , 0 );
        
        if( pos == std::string::npos )
            return "";
        
        return path.substr( pos+1 , path.length() );
    }    
    
    
    std::string indent( std::string txt )
    {
        // Replace all "returns" by "return and tab"
        find_and_replace( txt , "\n" , "\n\t" );
        
        // Insert the first tab
        txt.insert(0, "\t");
        return txt;
    }
    
    std::string indent( std::string txt , int num_spaces )
    {
        std::string sep;
        for (int i = 0 ; i < num_spaces ; i++ )
            sep.insert(0, " ");
        
        // Replace all "returns" by "return and tab"
        find_and_replace( txt , "\n" , "\n" + sep );
        
        // Insert the first tab
        txt.insert(0, sep);
        return txt;
    }
    
    std::string str( double value, char  letter )
	{
		char line[2000];
		
		if ( value < 10 )
			sprintf(line, "%4.2f%c",value,letter);
		else if ( value < 100 )
			sprintf(line, "%4.1f%c",value,letter);
		else 
			sprintf(line, "%4.0f%c",value,letter);
		
		
		return std::string( line );
	}
	
	std::string str( size_t value )
	{
		
		if (value < 1000)
			return au::str( (double)value , ' ' );
		else if( value < 1000000)
			return au::str( (double)value/ 1000.0 , 'K');
		else if( value < 1000000000)
			return au::str( (double)value/ 1000000.0 , 'M');
		else if( value < 1000000000000)
			return au::str( (double)value/ 1000000000.0 , 'G');
		else if( value < 1000000000000000)
			return au::str( (double)value/ 1000000000000.0 , 'T');
		else 
			return au::str( (double)value/ 1000000000000000.0 , 'P');
		
	}
	
	std::string str( size_t value , std::string postfix )
	{
		return str( value ) + postfix;
	}
    
    std::string str(const char* format, ...)
    {
        va_list        args;
        char           vmsg[2048];
        
        /* "Parse" the varible arguments */
        va_start(args, format);
        
        /* Print message to variable */
        vsnprintf(vmsg, sizeof(vmsg), format, args);
        //vmsg[2047] = 0;
        va_end(args);
        
        return std::string(vmsg);
    }        
    
    bool isOneOf( char c , std::string s )
	{
		for (size_t i = 0 ; i < s.size() ; i++)
			if( s[i] == c )
				return true;
		
		return false;
	}
    
    std::vector<std::string> simpleTockenize( std::string txt )
	{
		std::string tockens = " #\r\t\r\n{};\"";//All possible delimiters
		
		std::vector<std::string> items;
		
		// Simple parser
		size_t pos = 0;
		for (size_t i = 0 ; i < txt.size() ; i++)
		{
			if ( isOneOf( txt[i] , tockens ) )
			{
				if ( i > pos )
					items.push_back(txt.substr(pos, i - pos ));
				/*
                 //Emit the literal with one letter if that is the case
                 std::ostringstream o;
                 o << txt[i];
                 items.push_back( o.str() );
                 */
				pos = i+1;
			}
		}
        
        if ( txt.size() > pos )
            items.push_back(txt.substr(pos, txt.size() - pos ));
		
		
		return items;
	}
    
    
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
    {
        std::stringstream ss(s);
        std::string item;
        while(std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    
    std::vector<std::string> split(const std::string &s, char delim) 
    {
        std::vector<std::string> elems;
        return split(s, delim, elems);
    }  
    
    
}