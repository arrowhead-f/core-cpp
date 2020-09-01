
#include "ServiceQueryList.hpp"

void ServiceQueryList::fillContent()
{
    servQueryData sQData;
//filling with fake content
//todo: update from database response

    sQData.sId = "2";
    sQData.sServiceDefinition_id = "6";
    sQData.sServiceDefinition_serviceDefinition = "exampleServDefinition";
    sQData.sServiceDefinition_createdAt = "2020-07-02T09-50";
    sQData.sServiceDefinition_updatedAt = "2020-07-02T09-50";
    sQData.sProvider_id = "5";
    sQData.sProvider_systemName = "exampleProviderSystem";
    sQData.sProvider_address = "10.10.10.20";
    sQData.sProvider_port = "1234";
    sQData.sProvider_authenticationInfo = "";
    sQData.sProvider_createdAt = "2020-07-02T09-50";
    sQData.sProvider_updatedAt = "2020-07-02T09-50";
    sQData.sServiceUri = "/example/service";
    sQData.sEndOfValidity = "2022-07-02T09-50";
    sQData.sSecure = "INSECURE";
    sQData.mMetadata.insert( pair<string,string>( "meta1", "meta2"));
    sQData.mMetadata.insert( pair<string,string>( "meta3", "meta4"));
    sQData.sVersion = "1.2";
    sQData.vInterfaces_id.push_back("23");
    sQData.vInterfaces_interfaceName.push_back("example_interface");
    sQData.vInterfaces_createdAt.push_back("2020-07-02T09-50");
    sQData.vInterfaces_updatedAt.push_back("2020-07-02T09-51");
    sQData.sCreatedAt = "2020-07-02T09-50";
    sQData.sUpdatedAt = "2020-07-02T09-51";

    vServQueryData.push_back(sQData);

    unfilteredHits = 12;
}

string ServiceQueryList::createServiceQueryList()
{
    json_object *jResponse = json_object_new_object();
    json_object *jServiceQueryData = json_object_new_array();

    for( int i = 0; i < vServQueryData.size(); ++i)
    {
        json_object *jArrayElement = json_object_new_object();

        addInt(jArrayElement, "id", vServQueryData[i].sId);
        //serviceDefinition
        json_object *jServiceDefinition = json_object_new_object();
        addInt(   jServiceDefinition,    "id",                vServQueryData[i].sServiceDefinition_id);
        addString(jServiceDefinition,    "serviceDefinition", vServQueryData[i].sServiceDefinition_serviceDefinition);
        addString(jServiceDefinition,    "createdAt",         vServQueryData[i].sServiceDefinition_createdAt);
        addString(jServiceDefinition,    "updatedAt",         vServQueryData[i].sServiceDefinition_updatedAt);
        json_object_object_add(jArrayElement,"serviceDefinition", jServiceDefinition);
        //provider
        json_object *jProvider = json_object_new_object();
        addInt(   jProvider, "id",                 vServQueryData[i].sProvider_id);
        addString(jProvider, "systemName",         vServQueryData[i].sProvider_systemName);
        addString(jProvider, "address",            vServQueryData[i].sProvider_address);
        addInt(   jProvider, "port",               vServQueryData[i].sProvider_port);
        addString(jProvider, "authenticationInfo", vServQueryData[i].sProvider_authenticationInfo);
        addString(jProvider, "createdAt",          vServQueryData[i].sProvider_createdAt);
        addString(jProvider, "updatedAt",          vServQueryData[i].sProvider_updatedAt);
        json_object_object_add(jArrayElement, "provider", jProvider);
        //serviceUri
        addString(jArrayElement, "serviceUri", vServQueryData[i].sServiceUri);
        //endOfValidity
        addString(jArrayElement, "endOfValidity", vServQueryData[i].sEndOfValidity);
        //secure
        addString(jArrayElement, "secure", vServQueryData[i].sSecure);

        //metadata
        json_object *jMetadata = json_object_new_object();
        for (auto it = vServQueryData[i].mMetadata.begin(); it != vServQueryData[i].mMetadata.end(); ++it)
            addString(jMetadata, it->first, it->second);
        json_object_object_add(jArrayElement,"metadata", jMetadata);

        //version
        addDouble(jArrayElement, "version", vServQueryData[i].sVersion);
        //interfaces
        json_object *jInterfacesArray = json_object_new_array();

        for( int i = 0; i < vServQueryData[i].vInterfaces_id.size(); ++i)
        {
            json_object *jArrayElement2 = json_object_new_object();
            addInt(   jArrayElement2, "id",            vServQueryData[i].vInterfaces_id[i]);
            addString(jArrayElement2, "interfaceName", vServQueryData[i].vInterfaces_interfaceName[i]);
            addString(jArrayElement2, "createdAt",     vServQueryData[i].vInterfaces_createdAt[i]);
            addString(jArrayElement2, "updatedAt",     vServQueryData[i].vInterfaces_updatedAt[i]);
            json_object_array_add(jInterfacesArray, jArrayElement2);
        }

        json_object_object_add(jArrayElement, "interfaces", jInterfacesArray);
        //createdAt
        addString(jArrayElement, "createdAt", vServQueryData[i].sCreatedAt);
        //updatedAt
        addString(jArrayElement, "updatedAt", vServQueryData[i].sUpdatedAt);

        json_object_array_add(jServiceQueryData, jArrayElement);
    }

    json_object_object_add(jResponse, "serviceQueryData", jServiceQueryData);
    addInt(jResponse, "unfilteredHits", to_string(unfilteredHits));

    return string(json_object_to_json_string(jResponse));
}
