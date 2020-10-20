#pragma once

#include "common.h"

class ServiceDefinition
{
public:
    json_object *mainObject;

    std::string sId;
    std::string sServiceDefinition;
    std::string sCreatedAt;
    std::string sUpdatedAt;

    ServiceDefinition(){}

    ServiceDefinition(const char *_sJson, bool &_brSuccess)
    {
        mainObject = json_tokener_parse(_sJson);
        if(mainObject == NULL)
            _brSuccess = false;
        else
            _brSuccess = true;
    }

    bool validJSONPayload();

    void fillJsonResponse();
    std::string createServiceDefinition();
};

class ServiceDefinitionList
{
public:
    json_object *mainObject;
    std::vector<ServiceDefinition> vServiceDefinition;
    uint uCount;

    std::string createServiceDefinitionList();
};
