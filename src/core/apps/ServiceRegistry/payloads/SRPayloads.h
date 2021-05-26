
#ifndef _PAYLOADS_SRPAYLOADS_H_
#define _PAYLOADS_SRPAYLOADS_H_

#include <string>
#include <vector>

class SRPayloads
{

public:

    struct serviceQueryData
    {
        std::string sId;
        std::string sServiceDefinition_id;
        std::string sServiceDefinition_serviceDefinition;
        std::string sServiceDefinition_createdAt;
        std::string sServiceDefinition_updatedAt;
        std::string sProvider_id;
        std::string sProvider_systemName;
        std::string sProvider_address;
        std::string sProvider_port;
        std::string sProvider_authenticationInfo;
        std::string sProvider_createdAt;
        std::string sProvider_updatedAt;
        std::string sServiceUri;
        std::string sEndOfValidity;
        std::string sSecure;
        std::string sMetadata;
        std::string sVersion;
        std::vector<std::string> vInterfaces_id;
        std::vector<std::string> vInterfaces_interfaceName;
        std::vector<std::string> vInterfaces_createdAt;
        std::vector<std::string> vInterfaces_updatedAt;
        std::string sCreatedAt;
        std::string sUpdatedAt;
    };

    void toLowerAndTrim(std::string &_s)
    {
        std::transform(_s.begin(), _s.end(), _s.begin(), ::tolower);

        const char *whitespace = " \n\r\t\f\v";
        const char *s2 = _s.c_str();
        int n = _s.size();
        char *tmp = (char *)malloc(n+1);
        int j = 0;
        for(int i = 0; i < n; ++i)
            if( strchr(whitespace, s2[i]) == NULL) tmp[j++] = s2[i];
        tmp[j] = '\0';
        _s = std::string(tmp);
        free(tmp);
    }

    bool validInterfaceName(std::string &_intf)
    {
        //protocol-SECURE-JSON or protocol-INSECURE-JSON
        char *s = strchr(_intf.c_str(), '-');
        if(s == NULL)  return false;

        std::string str = std::string(s+1);
        if( str.compare("secure-json") != 0 && str.compare("insecure-json") != 0) return false;

        return true;
    }

    bool validEndOfValidity(const char *c)
    {
        //YYYY-MM-DD hh:mm:ss
        if( !isdigit(c[0]) || !isdigit(c[1]) || !isdigit(c[2]) || !isdigit(c[3])) return false;
        if( c[4] != '-' || c[7] != '-')          return false;
        if( !isdigit(c[5]) || !isdigit(c[6]) || !isdigit(c[8]) || !isdigit(c[9])) return false;
        if( c[10] != ' ' || c[13] != ':' || c[16] != ':' )                        return false;
        if(!isdigit(c[11]) || !isdigit(c[12]) ) return false;
        if(!isdigit(c[14]) || !isdigit(c[15]) ) return false;
        if(!isdigit(c[17]) || !isdigit(c[18]) ) return false;

        return true;
    }

    uint8_t validSecurityType(std::string &_sSecure, std::string &_sProviderSystem_AuthInfo)
    {
        if( _sSecure.compare("not_secure")  != 0 &&
            _sSecure.compare("certificate") != 0 &&
            _sSecure.compare("token") != 0
          ) return 4;

        if(_sProviderSystem_AuthInfo.size() != 0)
            if( _sSecure.compare("not_secure") == 0)
                return 5;

        if(_sSecure.compare("certificate") == 0 || _sSecure.compare("token") == 0)
            if(_sProviderSystem_AuthInfo.size() == 0)
                return 5;

        return 0;
    }

};

#endif /* _PAYLOADS_SRPAYLOADS_H_ */
