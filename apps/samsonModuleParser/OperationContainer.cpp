/* ****************************************************************************
 *
 * FILE                     OperationContainer.cpp
 *
 * DESCRIPTION				Information about an operation  (module definition)
 *
 * ***************************************************************************/

#include "DataContainer.h"
#include "DataCreator.h"
#include "OperationContainer.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"					// AUTockenizer

namespace ss {

	std::string OperationContainer::getCompareFunctionForData( std::string data )
	{
		std::vector<std::string> data_names = tockenizeWithDots( data );
		
		std::ostringstream output;
		output << "::ss::";
		for (size_t i = 0 ; i < data_names.size() ;i++)
			output << data_names[i] << "::";
		output << "compare";
		return output.str();
		
	}

	std::string OperationContainer::getIncludeForData( std::string data )
	{
		std::vector<std::string> data_names = tockenizeWithDots( data );
		
		std::ostringstream output;
		output << "#include <samson/modules/";
		for (size_t i = 0 ; i < ( data_names.size()-1) ;i++)
			output << data_names[i] << "/";
		output << data_names[data_names.size()-1] << ".h>\n";
		return output.str();
		
	}
	
	
	
	void OperationContainer::parse( AUTockenizer *module_creator ,  int begin ,int end )
	{
		int pos = begin;
		while( pos < end )
		{
			assert( !module_creator->isSpecial( pos ) );
			std::string mainCommand = module_creator->itemAtPos( pos++ );
			
			if( mainCommand == "file" )
				file = module_creator->getLiteral(&pos);
			else if( mainCommand == "code" )
				code = module_creator->getLiteral(&pos);
			else if( mainCommand == "helpLine" )
				helpLine = module_creator->getLiteral(&pos);
			else if( mainCommand == "setup" )
				setup = true;
			else if( mainCommand == "top" )
				top = true;
			else if( mainCommand == "destructor" )
				destructor = true;
			else if( mainCommand == "dynamic_input_formats" )
				dynamic_input_formats = true;
			else if( mainCommand == "extern" )
			{
				//Extern command...
				
				ostringstream o;
				while ( !module_creator->isSemiColom( pos ) )
					o << module_creator->itemAtPos( pos++ ) << " ";
				o << ";";
				pos++;
				
				functions.push_back( o.str() );
				
			}
			else if( mainCommand == "help" )
			{
				int begin,end;
				module_creator->getScopeLimits(&pos, &begin,&end);
				while( begin < end)
					help.push_back( module_creator->getLiteral(&begin) );
				
			}
			else if( mainCommand == "in" )
			{
				
				assert( !module_creator->isSpecial( pos ) );
				std::string keyFormat = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				std::string valueFormat = module_creator->itemAtPos( pos++ );		
				
				inputs.push_back( KVFormat::format(keyFormat, valueFormat) );
				
			}
			else if( mainCommand == "out" )
			{
				
				assert( !module_creator->isSpecial( pos ) );
				std::string keyFormat = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				std::string valueFormat = module_creator->itemAtPos( pos++ );		
				
				outputs.push_back( KVFormat::format(keyFormat, valueFormat) );
				
			}
			else
			{
				fprintf(stderr, "Error: Unknown command inside  opertion section (%s)\n", mainCommand.c_str());
				exit(0);
			}
			
			
		}
		
		assert( pos == (end+1));	//Make sure parsing is correct
		
	}
}
