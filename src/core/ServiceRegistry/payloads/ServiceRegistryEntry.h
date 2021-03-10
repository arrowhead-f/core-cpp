#pragma once

#include <string>
#include <vector>
#include "commonPayloads.h"
#include "gason/gason.h"
#include "../utils/SRJsonBuilder.h"

class ServiceRegistryEntry{

private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

    SRJsonBuilder        jResponse;

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

    void setJsonPayload(std::string &_sJsonPayload, bool &_brSuccess)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status == gason::JSON_PARSE_OK )
            _brSuccess = true;
        else
            _brSuccess = false;
    }

    bool validRegistryEntry()
    {
        try
        {
            if ( !jsonRootValue.child("serviceDefinition")            ) return false;
            if ( !jsonRootValue.child("providerSystem")("systemName") ) return false;
            if ( !jsonRootValue.child("providerSystem")("address")    ) return false;
            if ( !jsonRootValue.child("providerSystem")("port")       ) return false;
            if ( !jsonRootValue.child("serviceUri")                   ) return false;
            if ( !jsonRootValue.child("interfaces")[0]                ) return false;
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    void parseRegistryEntry()
    {
        sServiceDefinition = std::string(jsonRootValue.child("serviceDefinition").toString());
        sProviderSystem_SystemName = std::string(jsonRootValue.child("providerSystem")("systemName").toString());
        sProviderSystem_Address = std::string(jsonRootValue.child("providerSystem")("address").toString());
        sProviderSystem_Port = std::to_string(jsonRootValue.child("providerSystem")("port").toInt());

        char *authInfo = jsonRootValue.child("providerSystem")("authenticationInfo").toString();
        if(authInfo != NULL)
            sProviderSystem_AuthInfo = std::string(authInfo);

        sServiceUri = std::string(jsonRootValue.child("serviceUri").toString());
        sEndOfValidity = std::string(jsonRootValue.child("endOfValidity").toString());
        sSecure = std::string(jsonRootValue.child("secure").toString());
        sVersion = std::to_string(jsonRootValue.child("version").toInt());

        gason::JsonValue metaObj = jsonRootValue.child("metadata");
        SRJsonBuilder jMeta;
        for(gason::JsonIterator it = gason::begin(metaObj); it != gason::end(metaObj); ++it)
        {
            if(it->value.isNumber())
                jMeta.addInt(it->key, it->value.toInt());
            else if(it->value.isString())
                jMeta.addStr(it->key, it->value.toString());
            else if(it->value.isDouble())
                jMeta.addDbl(it->key, it->value.toNumber());
            else if(it->value.isBoolean())
                jMeta.addStr(it->key, it->value.toBool() ? "TRUE" : "FALSE");
        }

        sMetaData = jMeta.str();

        for( int i=0; ;++i ){
            if(jsonRootValue.child("interfaces")[i]){
                char *intf = jsonRootValue.child("interfaces")[i].toString();
                vInterfaces.push_back( std::string(intf));
            }
            else{
                break;
            }
        }
    }

    void fillJsonResponse()
    {
        jResponse.addInt("id", sQData.sId);

//serviceDefinition
        SRJsonBuilder jServiceDefinition;
        jServiceDefinition.addInt("id", sQData.sServiceDefinition_id);
        jServiceDefinition.addStr("serviceDefinition", sQData.sServiceDefinition_serviceDefinition);
        jServiceDefinition.addStr("createdAt", sQData.sServiceDefinition_createdAt);
        jServiceDefinition.addStr("updatedAt", sQData.sServiceDefinition_updatedAt);
        jResponse.addObj("serviceDefinition", (const std::string)jServiceDefinition.str());

//provider
        SRJsonBuilder jProvider;
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

        jResponse.addObj("metadata", sQData.sMetadata);

//version - double? int?
        jResponse.addDbl("version", sQData.sVersion);

//interfaces
        std::vector<std::string> v;
        for( int i = 0; i < sQData.vInterfaces_id.size(); ++i)
        {
            SRJsonBuilder jArrayElement;
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
