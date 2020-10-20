#pragma once

#include "common.h"

class System
{
    public:
        json_object *mainObject;
        json_object *jResponse;

        std::string sId;
        std::string sSystemName;
        std::string sAddress;
        std::string sPort;
        std::string sAuthInfo;
        std::string sCreatedAt;
        std::string sUpdatedAt;

        System(){}

        System(const char *_sJson, bool &_brSuccess)
        {
            mainObject = json_tokener_parse(_sJson);
            if(mainObject == NULL)
                _brSuccess = false;
            else
                _brSuccess = true;
        }

        bool validJSONPayload();
        void fillJsonResponse();
        std::string createSystem();
};

class SystemList
{
public:
    json_object *mainObject;
    std::vector<System> vSystem;
    uint uCount;

    std::string createSystemList();
};
