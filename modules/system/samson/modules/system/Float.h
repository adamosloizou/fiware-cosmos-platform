
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
*/

#ifndef _H_SAMSON_system_Float
#define _H_SAMSON_system_Float

#include <samson/modules/system/FixedLengthDataInstance.h>

namespace samson{
namespace system{


	class Float : public FixedLengthDataInstance<float>
	{
	public:
		int hash(int max_num_partitions)
		{
			return abs((int) 1000*value) % max_num_partitions;
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
                        if (strcmp(dataPathCharP, "Float") == 0)
                        {
                                *dataPathIntP = -1;
                                return (0);
                        }

                        if (strncmp(dataPathCharP, "Float.value", strlen("Float.value")) != 0)
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
                                        return ("Float");
                                        break;
                                case 0:
                                        if ((*dataPathIntP+1) == -1)
                                        {
                                                return ("float");
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
