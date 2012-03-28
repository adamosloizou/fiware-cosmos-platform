#include <string>
#include <vector>


class OTTService
{

    std::vector <std::string> httpPatterns;
    std::vector <std::string> dnsPatterns;

public:
    
    std::string name;
    int serviceId;
    bool category;
    
    OTTService( std::string _name, int _serviceId, bool _category )
    {
        name = _name;
        serviceId = _serviceId;
        category = _category;
    }
    
    ~OTTService()
    {
        while (httpPatterns.size() > 0)
        {
            httpPatterns.pop_back();
        }

        while (dnsPatterns.size() > 0)
        {
            dnsPatterns.pop_back();
        }
    }

    void addHTTPPattern(std::string _httpPattern)
    {
        httpPatterns.push_back(_httpPattern);
    }

    void addDNSPattern(std::string _dnsPattern)
    {
        dnsPatterns.push_back(_dnsPattern);
    }
    
    bool checkHTTP(const char *url, const char *domain);

    bool checkDNS(const char *dns);
    
    
};

