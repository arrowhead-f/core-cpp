
#ifndef _PAYLOADS_COMMONPAYLOADS_H_
#define _PAYLOADS_COMMONPAYLOADS_H_

#include <string>
#include <vector>
#include <map>

#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <inttypes.h>

#include "CommonJsonBuilder.h"
#include "gason/gason.h"

class CommonPayloads
{

public:

    struct stServDef
    {
        std::string sId;
        std::string sServiceDefinition;
        std::string sCreatedAt;
        std::string sUpdatedAt;
    };

    struct stSystem
    {
        std::string sId;
        std::string sSystemName;
        std::string sAddress;
        std::string sPort;
        std::string sAuthInfo;
        std::string sCreatedAt;
        std::string sUpdatedAt;
    };

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

    struct stOrchestratorStore
    {
        std::string sId;
        std::string sConsumerSystemId;
        std::string sProviderSystemId;
        std::string sForeign;
        std::string sServiceId;
        std::string sServiceInterfaceId;
        std::string sPriority;
    };

    struct stOrchResponse
    {
        gason::JsonValue jProvider;
        gason::JsonValue jService;
        std::string sServiceUri;
        std::string sSecure;
        gason::JsonValue jMetadata;
        gason::JsonValue jInterfaces;
        std::string sVersion;
        gason::JsonValue jAuthorizationTokens;
        std::vector<std::string> sWarnings;
    };

    struct stServiceRequestForm
    {
        gason::JsonValue jRequesterSystem;
        bool bRequestedServiceExists;
        gason::JsonValue jRequestedService;
        bool bPreferredProvidersExits;
        gason::JsonValue jPreferredProviders;
        std::map<std::string, bool> mFlags;
    };

    CommonJsonBuilder convertGasonToCJB(gason::JsonValue jGasonRoot)
    {
        CommonJsonBuilder cjb;

        for(gason::JsonIterator it = gason::begin(jGasonRoot); it != gason::JsonIterator(nullptr); ++it)
        {
            if(it->value.getTag() == gason::JSON_STRING)
                cjb.addStr(std::string(it->key), std::string(it->value.toString()));

            if(it->value.getTag() == gason::JSON_NUMBER)
                cjb.addInt(std::string(it->key), std::to_string(it->value.toInt()));

            if(it->value.getTag() == gason::JSON_ARRAY)
            {
                std::vector<std::string> v;
                for(int i=0; ;++i)
                {
                    if( jGasonRoot.child(it->key)[i] )
                    {
                        v.push_back( "\""+std::string(jGasonRoot.child(it->key)[i].toString())+"\"" );
                    }
                    else
                    {
                        break;
                    }
                }
                cjb.to_arrayObj<std::vector<std::string>::iterator>(it->key, v.begin(), v.end());
            }

            if(it->value.getTag() == gason::JSON_NULL)
                cjb.addObj(std::string(it->key),"null");

            if(it->value.getTag() == gason::JSON_TRUE)
                cjb.addObj(std::string(it->key),"true");

            if(it->value.getTag() == gason::JSON_FALSE)
                if(std::string(it->key).compare("authenticationInfo") != 0) //todo: gason bug
                    cjb.addObj(std::string(it->key),"false");

            if(it->value.getTag() == gason::JSON_OBJECT)
            {
                CommonJsonBuilder obj = convertGasonToCJB(it->value);
                cjb.addObj(std::string(it->key), obj.str());
            }
        }

        return cjb;
    }

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

    std::string getLocalTime()
    {
        std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //uint64_t u64Time = (uint64_t)tt;
        //printf("%" PRIu64 "\n", u64Time);
        struct std::tm *ptm = std::localtime(&tt);
        std::stringstream trTime;
        trTime << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
        std::string sTime = trTime.str();
        return sTime;
    }

    uint64_t convertStrTimeToUint64(std::string sLocalTime)
    {
        struct std::tm tm;
        std::istringstream ss(sLocalTime);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::time_t time = mktime(&tm);
        uint64_t u64Time = (uint64_t) time;
        return u64Time;
    }

};

#endif /* _PAYLOADS_COMMONPAYLOADS_H_ */
