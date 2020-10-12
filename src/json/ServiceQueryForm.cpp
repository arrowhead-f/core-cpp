
#include "ServiceQueryForm.hpp"


bool ServiceQueryForm::validQueryForm()
{
    if( !containsKey("serviceDefinitionRequirement") ) return false;

    return true;
}

/*
bool ServiceQueryForm::containsKey(string _sKey)
{
    json_object *obj;
    if(!json_object_object_get_ex(obj, (char *)_sKey.c_str(), &obj))
        return false;
    return true;
}

bool ServiceQueryForm::getValue(string _sKey, string &_srValue)
{
    struct json_object *jObj;
    if(!json_object_object_get_ex(mainObject, (char *)_sKey.c_str(), &jObj))
        return false;

    _srValue = std::string(json_object_get_string(jObj));
    return true;
}
*/
//Source: https://linuxprograms.wordpress.com/category/json-c/
void ServiceQueryForm::parseMeta(json_object *jobj) {
    enum json_type type;
    json_object_object_foreach(jobj, key, val)
    {
        type = json_object_get_type(val);
        switch (type) {
            case json_type_boolean:
            case json_type_double:
            case json_type_int:
            case json_type_string:
                mMetadataRequirements.insert( pair<string,string>(string(key), string(json_object_get_string(val))) );
                break;
            case json_type_object:
                json_object_object_get_ex(jobj, key, &jobj);
                parseMeta(jobj);
                break;
        }
    }
}

bool ServiceQueryForm::parseQueryForm()
{
////
// serviceDefinitionRequirement
////
    if( !getValue(mainObject, "serviceDefinitionRequirement", sServiceDefinition) )
        return false;

////
// interfaceRequirements
////
    struct json_object *jArray;
    struct json_object *jArrayElement;
    if( json_object_object_get_ex(mainObject, "interfaceRequirements", &jArray))
    {
        for(int i = 0; ; ++i)
        {
            jArrayElement = json_object_array_get_idx(jArray, i);
            if(jArrayElement == NULL) break;
            vInterfaceRequirements.push_back( string( json_object_get_string(jArrayElement) ) );
        }
    }

////
// securityRequirements
////
    if( json_object_object_get_ex(mainObject, "securityRequirements", &jArray))
    {
        for(int i = 0; ; ++i)
        {
            jArrayElement = json_object_array_get_idx(jArray, i);
            if(jArrayElement == NULL) break;
            vSecurityRequirements.push_back( string( json_object_get_string(jArrayElement) ) );
        }
    }

////
// metadataRequirements
////
    struct json_object *jObj;
    if( json_object_object_get_ex(mainObject, "metadataRequirements", &jObj))
        parseMeta(jObj);

////
// versionRequirement
////
    if( json_object_object_get_ex(mainObject, "versionRequirement", &jObj))
        sVersionReq = string( json_object_get_string(jObj) );

////
// maxVersionRequirement
////
    if( json_object_object_get_ex(mainObject, "maxVersionRequirement", &jObj))
        sMaxVersionReq = string( json_object_get_string(jObj) );

////
// minVersionRequirement
////
    if( json_object_object_get_ex(mainObject, "minVersionRequirement", &jObj))
        sMinVersionReq = string( json_object_get_string(jObj) );

////
// pingProviders
////
    if( json_object_object_get_ex(mainObject, "pingProviders", &jObj))
    {
        sPingProviders = string( json_object_get_string(jObj) );
        bPingProviders = json_object_get_boolean(jObj);
    }

    return true;
}
