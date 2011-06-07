
/**
 File autogenerated with samsonModuleParser, please do not edit
*/

#ifndef _H_SAMSON_system_UInt2_BASE
#define _H_SAMSON_system_UInt2_BASE


#include <samson/modules/system/UInt.h>


namespace samson{
namespace system{


	class UInt2_base : public samson::DataInstance{

	public:
	::samson::system::UInt value_1;
	::samson::system::UInt value_2;

	UInt2_base() : samson::DataInstance(){
	}

	~UInt2_base() {
	}

	int parse(char *data){
		int offset=0;
		{ //Parsing value_1
			offset += value_1.parse(data+offset);
		}

		{ //Parsing value_2
			offset += value_2.parse(data+offset);
		}

		return offset;
	}

	int serialize(char *data){
		int offset=0;
		{ //Serializing value_1
			offset += value_1.serialize(data+offset);
		}

		{ //Serializing value_2
			offset += value_2.serialize(data+offset);
		}

		return offset;
	}

	static inline int size(char *data){
		int offset=0;
		{ //Sizing value_1
			offset += ::samson::system::UInt::size(data+offset);
		}

		{ //Sizing value_2
			offset += ::samson::system::UInt::size(data+offset);
		}

		return offset;
	}

	int hash(int max_num_partitions){
		{ //Partitioning value_1
			return value_1.hash(max_num_partitions);
		}

	}

	inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
		{ // comparing value_1
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  value_1 compared 
		{ // comparing value_2
			int tmp = ::samson::system::UInt::compare(data1,data2,offset1 , offset2);
			if( tmp != 0) return tmp;
		}   //  value_2 compared 
		return 0; //If everything is equal
	}

	inline static int compare( char* data1 , char* data2 )
	{
		size_t offset_1=0;
		size_t offset_2=0;
		return compare( data1 , data2 , &offset_1 , &offset_2 );
	}




	static int *getDataPath(const std::string &dataPathString){
		const char *dataPathCharP = dataPathString.c_str();
		int nlevels = 1;
		int *dataPathIntP;

		const char *p_sep = dataPathCharP;
		while ((p_sep = strchr(p_sep, '.')) != NULL)
		{
			nlevels++;
			p_sep++;
		}

		if ((dataPathIntP = (int *)malloc((nlevels + 1)*sizeof(int))) == NULL)
		{
			return ((int *)NULL);
		}

		int retError = getDataPath(dataPathCharP, dataPathIntP);

		if (retError)
		{
			free(dataPathIntP);
			dataPathIntP = NULL;
		}

		return  (dataPathIntP);
	}

	static int getDataPath(const char * dataPathCharP, int *dataPathIntP){
		if (strcmp(dataPathCharP, "UInt2") == 0)
		{
			*dataPathIntP = -1;
			return (0);
		}

		if (strncmp(dataPathCharP, "UInt2.", strlen("UInt2.")) == 0)
		{
			return (getDataPath(dataPathCharP+strlen("UInt2."), dataPathIntP));
		}
		{ //Scanning value_1, terminal and non-terminal
			if (strcmp(dataPathCharP, "value_1") == 0)
			{
				*dataPathIntP = 0;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "value_1.", strlen("value_1.")) == 0)
			{
				*dataPathIntP = 0;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		{ //Scanning value_2, terminal and non-terminal
			if (strcmp(dataPathCharP, "value_2") == 0)
			{
				*dataPathIntP = 1;
				*(dataPathIntP+1) = -1;
				return (0);
			}
			if (strncmp(dataPathCharP, "value_2.", strlen("value_2.")) == 0)
			{
				*dataPathIntP = 1;
				*(dataPathIntP+1) = -1;
				return (0);
			}
		}
		return -1;
	}

	static std::string getType(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
			case -1:
				return ("UInt2");
				break;
			case 0:
				return(system::UInt::getType(dataPathIntP+1));
				break;
			case 1:
				return(system::UInt::getType(dataPathIntP+1));
				break;
			default:
				return ("_Unknown_");
				break;
		};
	}

	DataInstance * getInstance(const int *dataPathIntP){
		switch(*dataPathIntP)
		{
			case -1:
				return(this);
				break;
			case 0:
				return(value_1.getInstance(dataPathIntP+1));
				break;
			case 1:
				return(value_2.getInstance(dataPathIntP+1));
				break;
			default:
				return (NULL);
				break;
		};
	}

	void copyFrom( UInt2_base *other ){
		{ //Copying value_1
			value_1.copyFrom(&other->value_1);
		}

		{ //Copying value_2
			value_2.copyFrom(&other->value_2);
		}

	};

	std::string str(){
		std::ostringstream o;
				{ //Texting value_1
			o << value_1.str();
		}

		o<<" ";
				{ //Texting value_2
			o << value_2.str();
		}

		o<<" ";
		return o.str();
	}

	}; //class UInt2_base

} // end of namespace samson
} // end of namespace system

#endif
