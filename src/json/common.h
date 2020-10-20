#pragma once

#include <string>
#include <vector>
#include <map>
#include <json-c/json.h>

typedef struct servQueryData
{
    std::string sId;
    std::string sServiceDefinition_id;
    std::string sServiceDefinition_serviceDefinition;
    std::string sServiceDefinition_createdAt;
    std::string sServiceDefinition_updatedAt;
    std::string sProvider_id;
    std::string sProvider_systemName;
    std::string sProvider_address;
    std::string sProvider_port;
    std::string sProvider_authenticationInfo;
    std::string sProvider_createdAt;
    std::string sProvider_updatedAt;
    std::string sServiceUri;
    std::string sEndOfValidity;
    std::string sSecure;
    std::map<std::string,std::string> mMetadata; //todo:
    std::string sMetadata;
    std::string sVersion;
    std::vector<std::string> vInterfaces_id;
    std::vector<std::string> vInterfaces_interfaceName;
    std::vector<std::string> vInterfaces_createdAt;
    std::vector<std::string> vInterfaces_updatedAt;
    std::string sCreatedAt;
    std::string sUpdatedAt;
} servQueryData;

inline void addString(json_object *_jObj, std::string _key, std::string _value)
{
    json_object *jstring = json_object_new_string(_value.c_str());
    json_object_object_add(_jObj, _key.c_str(), jstring);
}

inline void addInt(json_object *_jObj, std::string _key, std::string _value)
{
    json_object *jint = json_object_new_int( atoi(_value.c_str()) );
    json_object_object_add(_jObj, _key.c_str(), jint);
}

inline void addInt(json_object *_jObj, std::string _key, uint _value)
{
    json_object *jint = json_object_new_int( (int) _value );
    json_object_object_add(_jObj, _key.c_str(), jint);
}

inline void addDouble(json_object *_jObj, std::string _key, std::string _value)
{
    std::string::size_type sz;
    double d = std::stod(_value, &sz);

    json_object *jDouble = json_object_new_double(d);
    json_object_object_add(_jObj, _key.c_str(), jDouble);
}

inline bool containsKey(std::string _sKey)
{
    json_object *obj;
    if(!json_object_object_get_ex(obj, (char *)_sKey.c_str(), &obj))
        return false;
    return true;
}

inline bool getValue(json_object *_pjsonObj, std::string _sKey, std::string &_srValue)
{
    struct json_object *jObj;
    if(!json_object_object_get_ex(_pjsonObj, (char *)_sKey.c_str(), &jObj))
    {
        _srValue = "";
        return false;
    }

    _srValue = std::string(json_object_get_string(jObj));
    return true;
}
