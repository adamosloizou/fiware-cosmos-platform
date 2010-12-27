#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

/* ****************************************************************************
*
* FILE                 CommandLine.h
*
*  Created by ANDREU URRUELA PLANAS on 4/22/10.
*  Copyright 2010 TID. All rights reserved.
*
* CommandLine
* 
* Class used to parse a string containing a command with multiple flags and arguments
* The advantadge of this class is that is supports running-time definiton of possible flags.
*
* 
* Example: command arg1 -f input.ttx -g 2 arg2
* 
* Depending on how you define flags "f" and "g" this will behavie differently.
*
* You can define flags as "bool", "double", "int" "string" "uint64".
* The properties of each element are:
*   bool:       It is a true/false flag. It does not take any parameter
*   int:        Takes an interguer parameter with it.   Example: -n 12
*   double:     Takes a double parameter.               Example: -threshold 5.67
*   uint64:     Takes an unsigned parameter.            Example: -size 12G , -size 100
*   string:     Takes an atring parameter.              Example: -file andreu.txt
*
* The basic usage has three steps:
*   * Use command set_flag_X( ) methods to define flags and types
*   * Use one of the parse(.) methods to parse the incoming command line
*   * Use get_flag_X methods to access content
*/
#include <stdio.h>             // fprintf, stderr, ...
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>



namespace au {

	typedef struct CommandLineFlag
	{
		std::string type;
		std::string default_value;
		std::string value;
	} CommandLineFlag;

	class CommandLine 
	{

	public:
		
		std::string command;											//!<< Incoming General command
		std::map< std::string , CommandLineFlag > flags;				//!<< Map of flags
		std::vector<std::string> arguments;								//!<< Command line arguments separated in "words"
		
		
	public:
		
		/** 
		 Simple constructor
		 */
		
		CommandLine(){}
		
		/**
		 Constructor with the provided string or argument list
		 */
		
		CommandLine( std::string _command );
		CommandLine(int argc, const char *argv[]);
		
		
		/**
		 Parse a command line in the classical C-style way
		 */
		
		void parse(int args , const char *argv[] );
		
		/**
		 Defining flags
		 */
		
		void reset_flags();
		
		void set_flag_boolean( std::string name );
		void set_flag_int( std::string name , int default_value );
		void set_flag_string( std::string name , std::string default_value );
		void set_flag_uint64( std::string name , std::string default_value );
		void set_flag_uint64( std::string name , size_t default_value );
		
		/** 
		 Parse a commnad line. It extracts all "arguments"
		 */

		
		void parse( std::string _command );		
		void clear_values();
		void parse_tockens( std::vector<std::string> &tockens );
		
		/**
		 Acces information about arguments
		 */

		int get_num_arguments();
		std::string get_argument( int index );
		bool isArgumentValue( int index , std::string value , std::string value2);
		
		/** 
		 Access to flags
		 */
		 
		std::string getFlagValue( std::string flag_name );
		std::string getFlagType( std::string flag_name );
		
		/**
		 Specialed access to parametes 
		 */

		bool get_flag_bool( std::string flag_name );
		int get_flag_int( std::string flag_name );
		double get_flag_double( std::string flag_name );
		std::string get_flag_string( std::string flag_name );
		size_t get_flag_uint64( std::string flag_name );
		
		/**
		 Functions to transform values
		 */
		
		int getIntValue	( std::string value );
		size_t getUint64Value	( std::string value );
		double getDoubleValue	(  std::string value );
		bool getBoolValue( std::string value );

	};

}

#endif
