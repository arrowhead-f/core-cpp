#pragma once

#include <string>
#include <vector>
//#include <map>

typedef struct serviceQueryData
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
    //std::map<std::string,std::string> mMetadata;
    std::string sMetadata;
    std::string sVersion;
    std::vector<std::string> vInterfaces_id;
    std::vector<std::string> vInterfaces_interfaceName;
    std::vector<std::string> vInterfaces_createdAt;
    std::vector<std::string> vInterfaces_updatedAt;
    std::string sCreatedAt;
    std::string sUpdatedAt;
} servQueryData;
