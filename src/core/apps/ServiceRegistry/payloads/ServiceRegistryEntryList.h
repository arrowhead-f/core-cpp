#ifndef _PAYLOADS_SERVICEREGISTRYENTRYLIST_H_
#define _PAYLOADS_SERVICEREGISTRYENTRYLIST_H_

#include "ServiceRegistryEntry.h"

class ServiceRegistryEntryList : CommonPayloads
{
public:

    std::vector<serviceQueryData> vServiceRegistryEntry;
    uint uCount;

    std::string createServiceRegistryEntryList()
    {
        std::string sResp = "{\"data\": [";

        for(uint i = 0; i < vServiceRegistryEntry.size(); ++i)
        {
            std::string tmp = createServiceRegistry(vServiceRegistryEntry[i]);
            sResp += tmp + std::string(",");
        }

        sResp.back() = ']';
        sResp += ",\"count\":" + std::to_string(uCount) + "}";

        return sResp;
    }

    void fillJsonResponse(CommonJsonBuilder &_jResponse, serviceQueryData &sQData)
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

    std::string createServiceDefinition(serviceQueryData &_stServQueryData)
    {
        CommonJsonBuilder jResponse;

        fillJsonResponse(jResponse, _stServQueryData);

        return jResponse.str();
    }

};

#endif /* _PAYLOADS_SERVICEREGISTRYENTRYLIST_H_ */
