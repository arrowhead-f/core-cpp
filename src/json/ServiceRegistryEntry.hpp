#pragma once

#include "common.h"

using namespace std;

class ServiceRegistryEntry
{
public:
    json_object *mainObject;
    json_object *jResponse;

    ServiceRegistryEntry(){}

    ServiceRegistryEntry(const char *_sJson, bool &_brSuccess)
    {
        mainObject = json_tokener_parse(_sJson);
        if(mainObject == NULL)
            _brSuccess = false;
        else
            _brSuccess = true;
    }

//Input
    string sServiceDefinition;
    string sProviderSystem_SystemName;
    string sProviderSystem_Address;
    string sProviderSystem_Port;
    string sProviderSystem_AuthInfo;
    string sServiceUri;
    string sEndOfValidity;
    string sSecure;
    string sMetaData;
    //map<string, string> vMetaData;
    string sVersion;
    vector<string> vInterfaces;

//Output
    servQueryData sQData;

    void parseMeta(json_object *jobj);
    //bool containsKey(string _sKey);
    //bool getValue(json_object *_pjsonObj, string _sKey, string &_srValue);
    bool validRegistryEntry();
    bool parseRegistryEntry();

    void fillJsonResponse();
    string createRegistryEntry();
};

class ServiceRegistryEntryList
{
public:
    json_object *mainObject;
    std::vector<ServiceRegistryEntry> vServiceRegistryEntry;
    uint uCount;

    string createRegistryEntryList();
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
