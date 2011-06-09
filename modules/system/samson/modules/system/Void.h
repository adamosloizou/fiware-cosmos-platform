
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Void
#define _H_SAMSON_system_Void


namespace samson{
namespace system{


	class Void : public samson::DataInstance{
		
		
	public:
		Void() : samson::DataInstance(){
		}
		
		~Void() {
		}
		
		int parse(char *data){
			return 0;
		}
		
		int serialize(char *data){
			return 0;
		}
		
		int hash(int max_num_partitions){
			return rand()%max_num_partitions;	// This has never to be used unless a random distribution is desired
		}
		
		static int size(char *data){
			return 0;
		}
		
		inline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){
			return 0; // No unnecessary movement for sorting
		}
		
		inline static int compare( KV* kv1 , KV*kv2 )
		{
			size_t offset_1=0;
			size_t offset_2=0;
			return compare( kv1->key , kv2->key , &offset_1 , &offset_2 );
		}
		
		inline static int compare( char* data1 , char* data2 )
		{
			// No necessary to compare
			return 0;
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
                        if (strcmp(dataPathCharP, "Void") == 0)
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
                        if (strcmp(dataPathCharP, "Void") == 0)
                        {
                                return ("system.Void");
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
                                        return ("system.Void");
                                        break;
                                default:
                                        return ("_ERROR_");
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

		
		void copyFrom( Void *other ){
			// Nothing to do here
		};
		
		std::string str(){
			return "";
		}
		
	}; 	

} // end of namespace samson
} // end of namespace system

#endif
