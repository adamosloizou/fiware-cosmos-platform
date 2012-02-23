#ifndef _H_SAMSON_SETUP
#define _H_SAMSON_SETUP

#include <map>				// std::map
#include <string>			// std::string
#include <iostream>			// std::cout
#include <stdlib.h>         // atoll
#include <sys/stat.h>		// mkdir

#include "logMsg/logMsg.h"

#include "au/map.h"         // au::map
#include "au/StringVector.h"
#include "au/Environment.h" // au::Environment


NAMESPACE_BEGIN(au)
    class ErrorManager;
}

namespace samson 
{
    
    typedef enum 
    {
        SetupItem_uint64,
        SetupItem_string
    } SamsonItemType;
	
    
    class SetupItem
    {
        
        
        std::string name;
        std::string default_value;

        std::string value;
        bool value_available;

        std::string description;
        SamsonItemType type;
        
    public:
        
        SetupItem( std::string _name , std::string _default_value , std::string _description , SamsonItemType _type )
        {
            name = _name;
            default_value = _default_value;
            value_available =  false;
            
            description = _description;
            type = _type;
            
            if( !check_valid( _default_value ) )
                LM_W(("Default value %s not valid for setup item %s" , _default_value.c_str() , _name.c_str() ));
            
        }

        bool setValue( std::string _value )
        {
            if( ! check_valid(value) )
                return false;
            
            value = _value;
            value_available = true;
            return true;
        }
        
        bool check_valid( std::string _value )
        {
            if( type == SetupItem_string )
                return true;
            
            if ( type == SetupItem_uint64 )
            {
                if( _value.find_first_not_of("0123456789") == std::string::npos )
                    return true;
                else
                    return false;
            }

            // Unknown type
            return false;
        }
        
        std::string getValue()
        {
            if( value_available )
                return value;
            else
                return default_value;
        }

        std::string getDefaultValue()
        {
            return default_value;
        }
        
        std::string getSetValue()
        {
            if( value_available )
                return value;
            else
                return "";
        }
        
        std::string getDescription()
        {
            return description;
        }
        
        std::string getConcept()
        {
            return name.substr(0, name.find(".",0 ) );
        }
        
        void resetToDefaultValue()
        {
            value_available =  false;
        }
        
        void clearCustumValue()
        {
            value = default_value;
            value_available = false;
        }
        
    };
    
    
    class SetupItemCollection
    {
    protected:
        
        au::map< std::string , SetupItem > items;
        
        // Add a new parameter to consider
        void  add( std::string _name , std::string _default_value , std::string _description , SamsonItemType type );
        
    public:
        
        ~SetupItemCollection();

        // Load a particular file to include all setup parameters
        void load( std::string file );
        
        // get the value of a parameter
        std::string getValueForParameter( std::string name );
    
        // Set manualy a particular parameter ( special case )
        bool setValueForParameter( std::string name ,std::string value );
    
        // Check if a particular property if defined
        bool isParameterDefined( std::string name );
        
        // Come back to default parameters
        void resetToDefaultValues();
        
        std::string str();
        
    };
    
    
	class SamsonSetup : public SetupItemCollection
	{
		SamsonSetup( std::string samson_home , std::string samson_working );
        
        std::string _samson_home;           // Home directory for SAMSON system
        std::string _samson_working;        // Working directory for SAMSON system
		
	public:

		static void init( std::string samson_home , std::string samson_working );
		static void destroy();
		static SamsonSetup *shared();
		
        
        // Get access to parameters
        std::string get( std::string name );
        size_t getUInt64( std::string name );
        int getInt( std::string name );

        std::string get_default( std::string name );
        
        
        // Get names fo files
        std::string setupFileName();                         // Get the Steup file
        std::string sharedMemoryLogFileName();
        std::string dataDirectory(  );     
        std::string dataFile( std::string filename );        // Get the name of a particular data file ( batch processing )
        std::string modulesDirectory();
        std::string blocksDirectory();
        
        std::string blockFileName( size_t worker_id , size_t id );
        bool blockIdFromFileName( std::string fileName , size_t* worker_id , size_t *id );

        std::string streamManagerLogFileName();
        std::string streamManagerAuxiliarLogFileName();

        
        std::string clusterInformationFileName();
        
        std::vector<std::string> getItemNames();
        
        
        // Create working directories
        void createWorkingDirectories();
		
        // Clear values specified manually
        void clearCustumValues();
        
        void edit();
        int save();
        
	};

}

#endif
