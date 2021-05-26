#ifndef _PAYLOADS_SERVICEQUERYLIST_H_
#define _PAYLOADS_SERVICEQUERYLIST_H_

#include <string>
#include <vector>
#include "../utils/SRJsonBuilder.h"

#include <ctype.h>

#include <iostream>
#include <sstream>
#include <fstream>

class ServiceQueryList : SRPayloads
{

private:

public:
    int hits;
    std::vector<serviceQueryData> vServQueryData;

    std::string createServiceQueryList()
    {
        SRJsonBuilder jResponse;
        std::vector<std::string> vArrayElement;

        for( int i = 0; i < vServQueryData.size(); ++i)
        {
            SRJsonBuilder jArrayElement;
            jArrayElement.addInt("id", vServQueryData[i].sId);

            //serviceDefinition
            SRJsonBuilder jServiceDefinition;
            jServiceDefinition.addInt("id", vServQueryData[i].sServiceDefinition_id);
            jServiceDefinition.addStr("serviceDefinition", vServQueryData[i].sServiceDefinition_serviceDefinition);
            jServiceDefinition.addStr("createdAt", vServQueryData[i].sServiceDefinition_createdAt);
            jServiceDefinition.addStr("updatedAt", vServQueryData[i].sServiceDefinition_updatedAt);
            jArrayElement.addObj("serviceDefinition", (const std::string)jServiceDefinition.str());

            //provider
            SRJsonBuilder jProvider;
            jProvider.addInt("id", vServQueryData[i].sProvider_id);
            jProvider.addStr("systemName", vServQueryData[i].sProvider_systemName);
            jProvider.addStr("address", vServQueryData[i].sProvider_address);
            jProvider.addInt("port", vServQueryData[i].sProvider_port);
            jProvider.addStr("authenticationInfo", vServQueryData[i].sProvider_authenticationInfo);
            jProvider.addStr("createdAt", vServQueryData[i].sProvider_createdAt);
            jProvider.addStr("updatedAt", vServQueryData[i].sProvider_updatedAt);
            jArrayElement.addObj("provider", (const std::string)jProvider.str());

            //serviceUri
            jArrayElement.addStr("serviceUri", vServQueryData[i].sServiceUri);

            //endOfValidity
            jArrayElement.addStr("endOfValidity", vServQueryData[i].sEndOfValidity);

            //secure
            jArrayElement.addStr("secure", vServQueryData[i].sSecure);

            //metadata
            if(vServQueryData[i].sMetadata.size() < 2)
                vServQueryData[i].sMetadata = "{}";

            jArrayElement.addMetaData("metadata", vServQueryData[i].sMetadata);

            //version - int
            jArrayElement.addInt("version", vServQueryData[i].sVersion);

            //interfaces
            std::vector<std::string> v;
            for( int j = 0; j < vServQueryData[i].vInterfaces_id.size(); ++j)
            {
                SRJsonBuilder jIntfArray;
                jIntfArray.addInt("id", vServQueryData[i].vInterfaces_id[j]);
                jIntfArray.addStr("interfaceName", vServQueryData[i].vInterfaces_interfaceName[j]);
                jIntfArray.addStr("createdAt", vServQueryData[i].vInterfaces_createdAt[j]);
                jIntfArray.addStr("updatedAt", vServQueryData[i].vInterfaces_updatedAt[j]);
                v.push_back(jIntfArray.str());
            }

            jArrayElement.to_arrayObj<std::vector<std::string>::iterator>("interfaces", v.begin(), v.end());

            //createdAt
            jArrayElement.addStr("createdAt", vServQueryData[i].sCreatedAt);

            //updatedAt
            jArrayElement.addStr("updatedAt", vServQueryData[i].sUpdatedAt);

            vArrayElement.push_back(jArrayElement.str());
        }

        jResponse.to_arrayObj<std::vector<std::string>::iterator>("serviceQueryData", vArrayElement.begin(), vArrayElement.end());

        jResponse.addInt("unfilteredHits", hits);

        return jResponse.str();
    }

};

#endif /* _PAYLOADS_SERVICEQUERYFORM_H_ */
