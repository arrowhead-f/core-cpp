#ifndef _PAYLOADS_SERVICEQUERYFORM_H_
#define _PAYLOADS_SERVICEQUERYFORM_H_

#include <map>
#include <string>
#include <vector>
#include "commonPayloads.h"
#include "gason/gason.h"
#include "../utils/SRJsonBuilder.h"

#include <ctype.h>

class ServiceQueryForm{

private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

public:

    std::string sServiceDefinition;
    std::vector<std::string> vInterfaceRequirements;
    std::vector<std::string> vSecurityRequirements;
    std::map<std::string, std::string> mMetadataRequirements;
    bool bVersionReqExists;
    int iVersionReq;
    bool bMinVersionReqExists;
    int iMinVersionReq;
    bool bMaxVersionReqExists;
    int iMaxVersionReq;
    bool bPingProviders;

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

    bool setJsonPayload(std::string &_sJsonPayload)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status == gason::JSON_PARSE_OK )
            return true;

        return false;
    }

    uint8_t parseQueryForm(std::string &_errResp)
    {
        if ( !jsonRootValue.child("serviceDefinitionRequirement")                              ) return 1;
        if (  jsonRootValue.child("serviceDefinitionRequirement").getTag() != gason::JSON_STRING ) return 2;/*JSON_NULL or JSON_FALSE*/
        bool b = true;
        sServiceDefinition = std::string(jsonRootValue.child("serviceDefinitionRequirement").toString(&b));
        if(sServiceDefinition.size() == 0) return 2;

        toLowerAndTrim(sServiceDefinition);

        if(sServiceDefinition.size() == 0) return 2;
        // interfaceRequirements
        for( int i = 0; ;++i )
        {
            if( jsonRootValue.child("interfaceRequirements") && jsonRootValue.child("interfaceRequirements")[i] )
            {
                std::string sintf = std::string(jsonRootValue.child("interfaceRequirements")[i].toString());
                std::transform(sintf.begin(), sintf.end(), sintf.begin(), ::tolower);
                if( !validInterfaceName(sintf) )
                {
                    _errResp = std::string(jsonRootValue.child("interfaceRequirements")[i].toString());
                    return 3;
                }

                vInterfaceRequirements.push_back(sintf);
            }
            else
            {
                break;
            }
        }

        // securityRequirements
        for( int i = 0; ;++i )
        {
            if( jsonRootValue.child("securityRequirements") && jsonRootValue.child("securityRequirements")[i] )
            {
                std::string sSec= std::string(jsonRootValue.child("securityRequirements")[i].toString());
                toLowerAndTrim(sSec);

                if( sSec.compare("not_secure")  != 0 && sSec.compare("certificate") != 0 && sSec.compare("token") != 0)
                {
                    _errResp = std::string(jsonRootValue.child("securityRequirements")[i].toString());
                    return 4;
                }

                vSecurityRequirements.push_back(sSec);
            }
            else
            {
                break;
            }
        }

        // metadataRequirements
        gason::JsonValue metaObj = jsonRootValue.child("metadataRequirements");
        for(gason::JsonIterator it = gason::begin(metaObj); it != gason::end(metaObj); ++it)
        {
            if(it->value.isNumber())
                mMetadataRequirements.insert({it->key, std::to_string(it->value.toInt())});
            else if(it->value.isString())
            {
                std::string s = it->value.toString();
                toLowerAndTrim(s);
                mMetadataRequirements.insert({it->key, s});
            }
            else if(it->value.isDouble())
                mMetadataRequirements.insert({it->key, std::to_string(it->value.toNumber())});
            else if(it->value.isBoolean())
                mMetadataRequirements.insert({it->key, it->value.getTag() == 4 ? "true" : "false"});
        }

        // versionRequirement
        if ( jsonRootValue.child("versionRequirement") && jsonRootValue.child("versionRequirement").isNumber())
        {
            bVersionReqExists = true;
            iVersionReq = jsonRootValue.child("versionRequirement").toInt();
        }
        else
        {
            bVersionReqExists = false;
        }

        // maxVersionRequirement
        if ( jsonRootValue.child("maxVersionRequirement") && jsonRootValue.child("maxVersionRequirement").isNumber())
        {
            bMaxVersionReqExists = true;
            iMaxVersionReq = jsonRootValue.child("maxVersionRequirement").toInt();
        }
        else
        {
            bMaxVersionReqExists = false;
        }

        // minVersionRequirement
        if ( jsonRootValue.child("minVersionRequirement") && jsonRootValue.child("minVersionRequirement").isNumber())
        {
            bMinVersionReqExists = true;
            iMinVersionReq = jsonRootValue.child("minVersionRequirement").toInt();
        }
        else
        {
            bMinVersionReqExists = false;
        }

        // pingProviders
        if ( jsonRootValue.child("pingProviders") && jsonRootValue.child("pingProviders").isBoolean())
            bPingProviders = (int)jsonRootValue.child("pingProviders").getTag() == 4 ? true : false;
        else
            bPingProviders = false;

        return 0;
    }


};

#endif /* _PAYLOADS_SERVICEQUERYFORM_H_ */