
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_UInt16
#define _H_SAMSON_system_UInt16




namespace samson{
namespace system{


	class UInt16 : public FixedLengthDataInstance<unsigned short>
	{
	public:
		int hash(int max_num_partitions){
			return abs(value)%max_num_partitions;
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
                        if (strcmp(dataPathCharP, "UInt16") == 0)
                        {
                                *dataPathIntP = -1;
                                return (0);
                        }

                        if (strncmp(dataPathCharP, "UInt16.value", strlen("UInt16.value")) != 0)
                        {
                                *dataPathIntP = 0;
                                *(dataPathIntP+1) = -1;
                                return (0);
                        }
                        return -1;
                }
                static std::string getType(const int *dataPathIntP){
                        switch(*dataPathIntP)
                        {
                                case -1:
                                        return ("UInt16");
                                        break;
                                case 0:
                                        if ((*dataPathIntP+1) == -1)
                                        {
                                                return ("unsigned short");
                                        }
                                        else
                                        {
                                                return ("_Unkwown_");
                                        }
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
                                        return (this);
                                        break;
                                case 0:
                                        if ((*dataPathIntP+1) == -1)
                                        {
                                                return ((DataInstance *)&value);
                                        }
                                        else
                                        {
                                                return (NULL);
                                        }
                                default:
                                        return (NULL);
                                        break;
                        };
                }

	};


} // end of namespace samson
} // end of namespace system

#endif
