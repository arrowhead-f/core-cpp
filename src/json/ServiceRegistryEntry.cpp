
#include "ServiceRegistryEntry.hpp"

bool ServiceRegistryEntry::validRegistryEntry()
{
    if( !containsKey("serviceDefinition") ) return false;
    if( !containsKey("providerSystem") ) return false;
    if( !containsKey("serviceUri") ) return false;
    if( !containsKey("interfaces") ) return false;

    return true;
}


//https://linuxprograms.wordpress.com/category/json-c/
/*
void ServiceRegistryEntry::parseMeta(json_object *jobj) {
    enum json_type type;
    json_object_object_foreach(jobj, key, val)
    {
        type = json_object_get_type(val);
        switch (type) {
            case json_type_boolean:
            case json_type_double:
            case json_type_int:
            case json_type_string:
                vMetaData.insert( pair<string,string>(string(key), string(json_object_get_string(val))) );
                break;
            case json_type_object:
                json_object_object_get_ex(jobj, key, &jobj);
                parseMeta(jobj);
                break;
        }
    }
}
*/

bool ServiceRegistryEntry::parseRegistryEntry()
{
    if( !getValue(mainObject, "serviceDefinition", sServiceDefinition) )
        return false;

    json_object *obj;
    if(!json_object_object_get_ex(mainObject, "providerSystem", &obj))
        return false;

    if( !getValue(obj, "systemName", sProviderSystem_SystemName) )
        return false;

    if( !getValue(obj, "address", sProviderSystem_Address) )
        return false;

    if( !getValue(obj, "port", sProviderSystem_Port) )
        return false;

    getValue(obj, "authenticationInfo", sProviderSystem_AuthInfo);

    if( !getValue(mainObject, "serviceUri", sServiceUri) )
        return false;

    getValue(mainObject, "endOfValidity", sEndOfValidity);
    getValue(mainObject, "secure", sSecure);
    getValue(mainObject, "version", sVersion);

    if( json_object_object_get_ex(mainObject, "metadata", &obj))
        sMetaData = string( json_object_get_string(obj) );
        //parseMeta(obj);

    struct json_object *jArrayElement;
    if( json_object_object_get_ex(mainObject, "interfaces", &obj))
    {
        for(int i = 0; ; ++i)
        {
            jArrayElement = json_object_array_get_idx(obj, i);
            if(jArrayElement == NULL) break;
            vInterfaces.push_back( string( json_object_get_string(jArrayElement) ) );
        }
    }

    return true;
}

void ServiceRegistryEntry::fillJsonResponse()
{
    jResponse = json_object_new_object();

    addInt(jResponse, "id", sQData.sId);
//serviceDefinition
    json_object *jServiceDefinition = json_object_new_object();
    addInt(   jServiceDefinition,    "id",                sQData.sServiceDefinition_id);
    addString(jServiceDefinition,    "serviceDefinition", sQData.sServiceDefinition_serviceDefinition);
    addString(jServiceDefinition,    "createdAt",         sQData.sServiceDefinition_createdAt);
    addString(jServiceDefinition,    "updatedAt",         sQData.sServiceDefinition_updatedAt);
    json_object_object_add(jResponse,"serviceDefinition", jServiceDefinition);
//provider
    json_object *jProvider = json_object_new_object();
    addInt(   jProvider, "id",                 sQData.sProvider_id);
    addString(jProvider, "systemName",         sQData.sProvider_systemName);
    addString(jProvider, "address",            sQData.sProvider_address);
    addInt(   jProvider, "port",               sQData.sProvider_port);
    addString(jProvider, "authenticationInfo", sQData.sProvider_authenticationInfo);
    addString(jProvider, "createdAt",          sQData.sProvider_createdAt);
    addString(jProvider, "updatedAt",          sQData.sProvider_updatedAt);
    json_object_object_add(jResponse,"provider", jProvider);
//serviceUri
    addString(jResponse, "serviceUri", sQData.sServiceUri);
//endOfValidity
    addString(jResponse, "endOfValidity", sQData.sEndOfValidity);
//secure
    addString(jResponse, "secure", sQData.sSecure);
//metadata
    //json_object *jMetadata = json_object_new_object();
    //for (auto it = sQData.mMetadata.begin(); it != sQData.mMetadata.end(); ++it)
    //    addString(jMetadata, it->first, it->second);
    //json_object_object_add(jResponse,"metadata", jMetadata);

    addString(jResponse, "metadata", sQData.sMetadata);

//version - double? int?
    addDouble(jResponse, "version", sQData.sVersion);
//interfaces
    json_object *jInterfacesArray = json_object_new_array();

    for( int i = 0; i < sQData.vInterfaces_id.size(); ++i)
    {
        json_object *jArrayElement = json_object_new_object();
        addInt(   jArrayElement, "id",            sQData.vInterfaces_id[i]);
        addString(jArrayElement, "interfaceName", sQData.vInterfaces_interfaceName[i]);
        addString(jArrayElement, "createdAt",     sQData.vInterfaces_createdAt[i]);
        addString(jArrayElement, "updatedAt",     sQData.vInterfaces_updatedAt[i]);
        json_object_array_add(jInterfacesArray, jArrayElement);
    }

    json_object_object_add(jResponse, "interfaces", jInterfacesArray);
//createdAt
    addString(jResponse, "createdAt", sQData.sCreatedAt);
//updatedAt
    addString(jResponse, "updatedAt", sQData.sUpdatedAt);
}

std::string ServiceRegistryEntry::createRegistryEntry()
{
    fillJsonResponse();

    return string(json_object_to_json_string(jResponse));
}

std::string ServiceRegistryEntryList::createRegistryEntryList()
{
    mainObject = json_object_new_object();
    json_object *jData = json_object_new_array();

    for(uint i = 0; i < vServiceRegistryEntry.size(); ++i)
    {
        vServiceRegistryEntry[i].fillJsonResponse();
        json_object_array_add(jData, vServiceRegistryEntry[i].jResponse);
    }

    json_object_object_add(mainObject, "data", jData);

    addInt(mainObject, "count", uCount);

    return string(json_object_to_json_string(mainObject));
}

/*Creating a json boolean*/
//json_object *jboolean = json_object_new_boolean(1);
/*Creating a json double*/
//json_object *jdouble = json_object_new_double(2.14);

/*
// Output
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
