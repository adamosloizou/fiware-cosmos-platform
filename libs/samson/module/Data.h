#ifndef SAMSON_DATA_H
#define SAMSON_DATA_H


#include <math.h>						/*                                          */
#include <stdlib.h>						/* malloc                                   */
#include <string.h>						/* memcpy                                   */
#include <string>						/* std::string                              */

#include <samson/module/KV.h>                  /* KV                                       */
#include <samson/module/KVVector.h>            /* KVVector                                 */
#include <samson/module/DataInstance.h>        /* DataInstance                             */



namespace samson {
	
	// Static functions necessary for a Data
	typedef void* (*DataCreationFunction)();

	// Function used to get the size of data from the stream of bytes
	typedef int(* DataSizeFunction)(char *data);
	
	// Templatd function to create an instance of DataInstance
	template <class T>
	void* getDataIntace()
	{
		return new T();
	}
	
	/** 
	 Definition of a new type of data
	 */
	
	class Data
	{		
		
	public:

		std::string _name;									// Name of this data ( inside the module defined )
		std::string _helpMessage;							// Help message shown on screen

	public:
		
		/**
		 Inform about the type of operation it is
		 */
		
		Data( std::string name , std::string help_message = "Help coming soon" )
		{
			_name = name;
			_helpMessage = help_message;
		}

        virtual ~Data(){};
		
        void getInfo( std::ostringstream& output)
        {
            output << "<data>\n";
            output << "<name>" << _name << "</name>\n";
            output << "<help>" << _helpMessage << "</help>\n";
            output << "</data>\n";
        }
        
		std::string getName()
		{
			return _name;
		}

		// Virtual function to create an instance of data
		virtual DataInstance* getInstance()=0;

        // Virtual function to duplicate this Data
		virtual Data* getDuplicate()=0;
		
		std::string help()
		{
			return _helpMessage;
		};									
		
	};
		
    
    template< class DI>
    class DataImpl : public Data
    {
        
    public:
        
        DataImpl(  std::string name , std::string help_message = "Help coming soon"  ) : Data(  name , help_message )
        {
            
        }

		DataInstance* getInstance()
        {
            return new DI();
        }
        
		virtual Data* getDuplicate()
        {
            return new DataImpl<DI>( _name ,_helpMessage );
        }
        
    };
	
	
	
} // ss namespace

#endif
