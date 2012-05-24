
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this data
 */

#ifndef _H_SAMSON_system_Void
#define _H_SAMSON_system_Void

#include <iostream>
#include <sstream>


namespace samson{
namespace system{


class Void : public samson::DataInstance{


public:
    Void() : samson::DataInstance(){
    }

    ~Void() {
    }

    std::string getName()
    {
        return "system.Void";
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

    int serial_compare( char* data1 , char* data2 )
    {
        return compare( data1, data2 );
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

    void setFromString(const char *_data)
    {
        ;
    }



    int *getDataPath(const std::string &dataPathString){
        return(getDataPathStatic(dataPathString));
    }

    static int *getDataPathStatic(const std::string &dataPathString){
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
        if (*dataPathCharP == 0)
        {
            *dataPathIntP = -1;
            return (0);
        }

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
        if (*dataPathCharP == 0)
        {
            return ("system.Void");
        }

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
        return ("_ERROR_");
    }

    static const char *getTypeStatic()
    {
        return ("system.Void");
    }

    const char *getType()
    {
        return ("system.Void");
    }

    static bool checkTypeStatic(const char *type)
    {
        if (strcmp(type, "system.Void") == 0)
        {
            return true;
        }
        return false;
    }

    bool checkType(const char *type)
    {
        if (strcmp(type, "system.Void") == 0)
        {
            return true;
        }
        return false;
    }

    static size_t getHashTypeStatic(){
        return(12376010167428248488ULL);
    }

    size_t getHashType(){
        return(12376010167428248488ULL);
    }

    static bool checkHashTypeStatic(size_t valType){
        if (valType == 12376010167428248488ULL)
        {
            return true;
        }		return false;
    }

    bool checkHashType(size_t valType){
        if (valType == 12376010167428248488ULL)
        {
            return true;
        }		return false;
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
        return (NULL);
    }


    void copyFrom( Void *other ){
        // Nothing to do here
    };

    std::string str(){
        return "_void_";
    }

    std::string strJSON(){
        return str();
    }

    std::string strXML(){
        return str();
    }

    std::string strHTML(int level_html_heading){
        return str();
    }

    std::string strHTMLTable(std::string _varNameInternal){
        std::ostringstream o;
        o << "<table border=\"1\">\n";
         o << "<caption>" <<  _varNameInternal << "</caption>\n";
         o << "<tr>\n";
         o << "<th>" << _varNameInternal << "</th>\n";
         o << "</tr>\n";
         o << "<tr>\n";
         o << "<th>" << getName() << "</th>\n";
         o << "</tr>\n";
         o << "<td>" << str() << "</td>\n";
         o << "</tr>\n";
         o << "<table>\n";
        return o.str();
    }


    std::string paint_header(int init_col)
    {
        return "Term";
    }

    std::string paint_header_basic(int init_col)
    {
        return "Term";
    }

    std::string paint_value(int index_row)
    {
        std::ostringstream o;
        if (index_row >= 0)
        {
            o  << "<td>" << str() << "</td>";
        }
        else
        {
            o  << "<td></td>";
        }
        return o.str();
    }

    int num_fields()
    {
        return 1;
    }

    int num_basic_fields()
    {
        return 1;
    }

    int max_depth()
    {
        return 1;
    }

    int max_num_values()
    {
        return 1;
    }

    bool is_terminal()
    {
        return true;
    }


};

} // end of namespace samson
} // end of namespace system

#endif
