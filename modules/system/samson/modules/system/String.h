
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_String
#define _H_SAMSON_system_String


#ifdef DEBUG_FILES
#include <iostream>
#include <fstream>
#endif /* de DEBUG_FILES */




namespace samson{
namespace system{


	class String : public samson::DataInstance{
		
	public:

		std::string value;
		
		String() : samson::DataInstance(){

		}
		
		~String() {

		}
		
		int parse(char *data){
			int pos = 0;
			while( data[pos] != '\0' )
				pos++;
			value = data;
			return pos+1;
		}
		
		int serialize(char *data){
			int  l = value.length(); 
			memcpy(data, value.c_str(), l);
			data[l] = '\0';
			return l+1;
		}
		
		int hash(int max_num_partitions)
		{
		    static const size_t InitialFNV = 2166136261U;
		    static const size_t FNVMultiple = 16777619;

		    size_t hash = InitialFNV;
		    for(size_t i = 0; i < value.length(); i++)
		      {
			hash = hash ^ (value[i]);       /* xor  the low 8 bits */
			hash = hash * FNVMultiple;      /* multiply by the magic number */
		      }
		    return hash%max_num_partitions;
		  }

		static int size(char *data){
			int pos = 0;
			while( data[pos] != '\0')
				pos++;
			return pos+1;
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 )
		{
			char *data1_orig = data1+*offset1;
			char *data2_orig = data2+*offset2;
			int pos1 = 0;

#ifdef DEBUG_FILES
			{
                        std::string filename = "/tmp/String_compare.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "Compare: (" << data1_orig << ", " << data2_orig << ") offset1: " << *offset1 << ", offset2: " << *offset2 << std::endl;
                        fs.close();
			}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES

			while( data1_orig[pos1] != '\0')
				pos1++;
			
			int pos2 = 0;
			while( data2_orig[pos2] != '\0')
				pos2++;
			
			*offset1 += pos1+1;
			*offset2 += pos2+1;

#ifdef DEBUG_FILES
             {
			std::string filename = "/tmp/String_compare.log";
                        std::ofstream fs(filename.c_str(), std::ios::app);
                        fs << "update offset1: " << *offset1 << ", offset2: " << *offset2 << std::endl;
                        fs.close();
			}
#endif /* de DEBUG_FILES */
#undef DEBUG_FILES
			//if( pos1 < pos2 )
				//return -1;
			//else if( pos1 > pos2 )
				//return 1;
			
			// Same length
			
			for (int i = 0 ; i < pos1 ; i++)
			{
				if ( data1_orig[i] < data2_orig[i] )
					return -1;
				else if ( data1_orig[i] > data2_orig[i] )
					return 1;
			}
			
			// Same content
			return 0;
		}
		
		inline static int compare( char* data1 , char* data2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( data1 , data2 , &offset_1 , &offset_2 );
		}
		

		int *getDataPath(const std::string &dataPathString){
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
			if (strcmp(dataPathCharP, "String") == 0)
			{
				*dataPathIntP = -1;
				return (0);
			}

			return -1;
		}

                std::string getTypeFromPath(const std::string &dataPathString){
                        const char *dataPathCharP = dataPathString.c_str();
                        return(getTypeFromPathStatic(dataPathCharP));
                }

                static std::string getTypeFromPathStatic(const char * dataPathCharP){
                        if (strcmp(dataPathCharP, "String") == 0)
                        {
                                return ("system.String");
                        }
                        return("_ERROR_");
                }

                std::string getTypeFromPath(const int *dataPathIntP){
                        return(getTypeFromPathStatic(dataPathIntP));
                }

		static std::string getTypeFromPathStatic(const int *dataPathIntP){
			switch(*dataPathIntP)
			{
				case -1:
					return ("system.String");
					break;
				default:
					return ("_ERROR_");
					break;
				break;
			};
		}

		DataInstance * getDataInstanceFromPath(const int *dataPathIntP){
			switch(*dataPathIntP)
			{
				case -1:
					return (this);
					break;
				default:
					return (NULL);
					break;
			};
		}

		void copyFrom( String *other ){
			value = other->value;
		};
		
		std::string str(){
			return value;
		}
		
		void operator= (std::string &_value) {
			value = _value;
		}			
		
	}; 	


} // end of namespace samson
} // end of namespace system

#endif
