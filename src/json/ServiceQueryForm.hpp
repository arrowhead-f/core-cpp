#pragma once

#include "common.h"

class ServiceQueryForm
{
private:
    json_object *mainObject;

public:
    std::string sServiceDefinition;
    std::vector<std::string> vInterfaceRequirements;
    std::vector<std::string> vSecurityRequirements;
    std::map<std::string, std::string> mMetadataRequirements;
    std::string sVersionReq;
    std::string sMinVersionReq;
    std::string sMaxVersionReq;
    std::string sPingProviders;
    bool bPingProviders;

    ServiceQueryForm(const char *_sJson, bool &_brSuccess)
    {
        mainObject = json_tokener_parse(_sJson);
        if(mainObject == NULL)
            _brSuccess = false;
        else
            _brSuccess = true;

        bPingProviders = false;
    }

    bool validQueryForm();
    //bool containsKey(string _sKey);
    //bool getValue(string _sKey, string &_srValue);
    void parseMeta(json_object *jobj);
    bool parseQueryForm();
};

//ServiceQueryForm:
/*
{
 "serviceDefinitionRequirement": "string",   -- Mandatory!
 "interfaceRequirements": [
   "string"
 ],
 "securityRequirements": [
   "NOT_SECURE"
 ],
 "metadataRequirements": {
   "additionalProp1": "string",
   "additionalProp2": "string",
   "additionalProp3": "string"
 },
 "versionRequirement": 0,
 "maxVersionRequirement": 0,
 "minVersionRequirement": 0,
 "pingProviders": true
}
*/

//
// Note: Valid interfaceRequirements name pattern: protocol-SECURE or INSECURE format. (e.g.: HTTPS-SECURE-JSON)
//Note: Possible values for securityRequirements are:
//- NOT_SECURE
//- CERTIFICATE
//- TOKEN
//- not defined, if you don't want to filter on security type


//Example:
/*
{
 "serviceDefinitionRequirement": "exampleService",
 "interfaceRequirements": [
   "HTTPS-SECURE-JSON",
   "HTTP-INSECURE-JSON",
   "TCP-INSECURE-CUSTOM"
 ],
 "securityRequirements": [
   "NOT_SECURE",
   "CERTIFICATE",
   "TOKEN"
 ],
 "metadataRequirements": {
   "additionalProp1": "meta1",
   "additionalProp2": "meta2",
   "additionalProp3": "meta3"
 },
 "versionRequirement": 0,
 "maxVersionRequirement": 0,
 "minVersionRequirement": 0,
 "pingProviders": true
}
*/
