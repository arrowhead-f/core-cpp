#ifndef _PAYLOADS_SERVICEREGISTRYENTRY_H_
#define _PAYLOADS_SERVICEREGISTRYENTRY_H_

#include <string>
#include <vector>
#include "utils/common/CommonPayloads.h"
#include "gason/gason.h"
#include "utils/common/CommonJsonBuilder.h"
#include <ctype.h>

class ServiceRegistryEntry : CommonPayloads
{

private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

    CommonJsonBuilder    jResponse;

public:

//Input
    std::string sServiceDefinition;
    std::string sProviderSystem_SystemName;
    std::string sProviderSystem_Address;
    std::string sProviderSystem_Port;
    std::string sProviderSystem_AuthInfo;
    std::string sServiceUri;
    std::string sEndOfValidity;
    std::string sSecure;
    std::string sMetaData;
    std::string sVersion;
    std::vector<std::string> vInterfaces;

//Output
    serviceQueryData sQData;

    bool setJsonPayload(std::string &_sJsonPayload)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status == gason::JSON_PARSE_OK )
            return true;

        return false;
    }

    uint8_t validRegistryEntry()
    {
        try
        {
            if ( !jsonRootValue.child("serviceDefinition")                              ) return 1;
            if (  jsonRootValue.child("serviceDefinition").getTag() != gason::JSON_STRING ) return 2;/*JSON_NULL or JSON_FALSE*/
            bool b = true;
            sServiceDefinition = std::string(jsonRootValue.child("serviceDefinition").toString(&b));
            if(sServiceDefinition.size() == 0) return 2;

            if ( !jsonRootValue.child("providerSystem")                                 ) return 3;
            if ( !jsonRootValue.child("providerSystem")("systemName")                   ) return 4;
            if ( !jsonRootValue.child("providerSystem")("address")                      ) return 5;
            if ( !jsonRootValue.child("providerSystem")("port")                         ) return 6;
            // ( !jsonRootValue.child("serviceUri")                                     ) return 7; //?
            if ( !jsonRootValue.child("interfaces")                                     ) return 8;
            if ( !jsonRootValue.child("interfaces")[0]                                  ) return 9;
        }
        catch(...)
        {
            return 10;
        }

        return 0;
    }

    uint8_t parseRegistryEntry(std::string &_errResp)
    {
        sServiceDefinition = std::string(jsonRootValue.child("serviceDefinition").toString());
        toLowerAndTrim(sServiceDefinition);

        sProviderSystem_SystemName = std::string(jsonRootValue.child("providerSystem")("systemName").toString());
        if( strchr(sProviderSystem_SystemName.c_str(), '.') != NULL ) return 1;
        toLowerAndTrim(sProviderSystem_SystemName);

        sProviderSystem_Address = std::string(jsonRootValue.child("providerSystem")("address").toString());

        int port = jsonRootValue.child("providerSystem")("port").toInt();
        if( ( port < 0 ) || ( port > 65535 ) )
            return 2;

        sProviderSystem_Port = std::to_string(port);

        if( jsonRootValue.child("providerSystem")("authenticationInfo") )
        {
            if ( jsonRootValue.child("providerSystem")("authenticationInfo").getTag() == gason::JSON_STRING )
            {
                bool b = true;
                sProviderSystem_AuthInfo = std::string(jsonRootValue.child("providerSystem")("authenticationInfo").toString(&b));
            }
        }

        sServiceUri = std::string(jsonRootValue.child("serviceUri").toString());

        sEndOfValidity = std::string(jsonRootValue.child("endOfValidity").toString());

        if( !validEndOfValidity(sEndOfValidity.c_str()) )
            return 3;

        sSecure = std::string(jsonRootValue.child("secure").toString());
        toLowerAndTrim(sSecure);

        uint8_t status = validSecurityType(sSecure, sProviderSystem_AuthInfo); // 4 or 5

        if( status )
            return status;

        sVersion = std::to_string(jsonRootValue.child("version").toInt());

        if(jsonRootValue.child("metadata"))
        {
            gason::JsonValue metaObj = jsonRootValue.child("metadata");
            sMetaData = "";

            for(gason::JsonIterator it = gason::begin(metaObj); it != gason::end(metaObj); ++it)
            {
                if(it->value.isNumber())
                    sMetaData += std::string(it->key) + std::string("=") + std::to_string(it->value.toInt()) + std::string(",");
                else if(it->value.isString())
                    sMetaData += std::string(it->key) + std::string("=") + it->value.toString() + std::string(",");
                else if(it->value.isDouble())
                    sMetaData += std::string(it->key) + std::string("=") + std::to_string(it->value.toNumber()) + std::string(",");
                else if(it->value.isBoolean())
                    sMetaData += std::string(it->key) + std::string("=") + (it->value.getTag() == 4 ? std::string("true,") : std::string("false,"));
            }

            sMetaData.erase( sMetaData.end() - 1 );
            toLowerAndTrim(sMetaData);
        }

        for( int i = 0; ;++i )
        {
            if(jsonRootValue.child("interfaces")[i])
            {
                std::string sintf = std::string(jsonRootValue.child("interfaces")[i].toString());
                std::transform(sintf.begin(), sintf.end(), sintf.begin(), ::tolower);
                if( !validInterfaceName(sintf) )
                {
                    _errResp = std::string(jsonRootValue.child("interfaces")[i].toString());
                    return 7;
                }

                vInterfaces.push_back(sintf);
            }
            else
            {
                break;
            }
        }

        return 0;
    }

    void fillJsonResponse()
    {
        jResponse.addInt("id", sQData.sId);

//serviceDefinition
        CommonJsonBuilder jServiceDefinition;
        jServiceDefinition.addInt("id", sQData.sServiceDefinition_id);
        jServiceDefinition.addStr("serviceDefinition", sQData.sServiceDefinition_serviceDefinition);
        jServiceDefinition.addStr("createdAt", sQData.sServiceDefinition_createdAt);
        jServiceDefinition.addStr("updatedAt", sQData.sServiceDefinition_updatedAt);
        jResponse.addObj("serviceDefinition", (const std::string)jServiceDefinition.str());

//provider
        CommonJsonBuilder jProvider;
        jProvider.addInt("id", sQData.sProvider_id);
        jProvider.addStr("systemName", sQData.sProvider_systemName);
        jProvider.addStr("address", sQData.sProvider_address);
        jProvider.addInt("port", sQData.sProvider_port);
        jProvider.addStr("authenticationInfo", sQData.sProvider_authenticationInfo);
        jProvider.addStr("createdAt", sQData.sProvider_createdAt);
        jProvider.addStr("updatedAt", sQData.sProvider_updatedAt);
        jResponse.addObj("provider", (const std::string)jProvider.str());

//serviceUri
        jResponse.addStr("serviceUri", sQData.sServiceUri);

//endOfValidity
        jResponse.addStr("endOfValidity", sQData.sEndOfValidity);

//secure
        jResponse.addStr("secure", sQData.sSecure);

//metadata
        if(sQData.sMetadata.size() < 2)
            sQData.sMetadata = "{}";

        jResponse.addMetaData("metadata", sQData.sMetadata);

//version - double? int?
        jResponse.addDbl("version", sQData.sVersion);

//interfaces
        std::vector<std::string> v;
        for( int i = 0; i < sQData.vInterfaces_id.size(); ++i)
        {
            CommonJsonBuilder jArrayElement;
            jArrayElement.addInt("id", sQData.vInterfaces_id[i]);
            jArrayElement.addStr("interfaceName", sQData.vInterfaces_interfaceName[i]);
            jArrayElement.addStr("createdAt", sQData.vInterfaces_createdAt[i]);
            jArrayElement.addStr("updatedAt", sQData.vInterfaces_updatedAt[i]);
            v.push_back(jArrayElement.str());
        }

        jResponse.to_arrayObj<std::vector<std::string>::iterator>("interfaces", v.begin(), v.end());

//createdAt
        jResponse.addStr("createdAt", sQData.sCreatedAt);

//updatedAt
        jResponse.addStr("updatedAt", sQData.sUpdatedAt);
    }

    std::string createRegistryEntry()
    {
        fillJsonResponse();
        return jResponse.str();
    }

};

//Input
/*
{
  "serviceDefinition": "string",
  "providerSystem": {
    "systemName": "string",
    "address": "string",
    "port": 0,
    "authenticationInfo": "string"
  },
  "serviceUri": "string",
  "endOfValidity": "string",
  "secure": "NOT_SECURE",
  "metadata": {
    "additionalProp1": "string",
    "additionalProp2": "string",
    "additionalProp3": "string"
  },
  "version": 0,
  "interfaces": [
    "string"
  ]
}
*/

//Output
/*
{
  "id": 0,
  "serviceDefinition": {
    "id": 0,
    "serviceDefinition": "string",
    "createdAt": "string",
    "updatedAt": "string"
  },
  "provider": {
    "id": 0,
    "systemName": "string",
    "address": "string",
    "port": 0,
    "authenticationInfo": "string",
    "createdAt": "string",
    "updatedAt": "string"
  },
  "serviceUri": "string",
  "endOfValidity": "string",
  "secure": "NOT_SECURE",
  "metadata": {
    "additionalProp1": "string",
    "additionalProp2": "string",
    "additionalProp3": "string"
  },
  "version": 0,
  "interfaces": [
    {
      "id": 0,
      "interfaceName": "string",
      "createdAt": "string",
      "updatedAt": "string"
 }
 ],
 "createdAt": "string",
 "updatedAt": "string"
}
*/

#endif /* _PAYLOADS_SERVICEREGISTRYENTRY_H_ */
