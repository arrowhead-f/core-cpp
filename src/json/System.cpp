
#include "System.h"

bool System::validJSONPayload()
{
    if( !getValue(mainObject, "systemName", sSystemName) ) return false;
    if( !getValue(mainObject, "address",     sAddress   ) ) return false;
    if( !getValue(mainObject, "port",        sPort      ) ) return false;

    return true;
}

void System::fillJsonResponse()
{
    jResponse = json_object_new_object();

    addString(jResponse, "id", sId);
    addString(jResponse, "systemName", sSystemName);
    addString(jResponse, "address", sAddress);
    addString(jResponse, "port", sPort);
    addString(jResponse, "authenticationInfo", sAuthInfo);
    addString(jResponse, "createdAt", sCreatedAt);
    addString(jResponse, "updatedAt", sUpdatedAt);
}

std::string System::createSystem()
{
    fillJsonResponse();
    return std::string( json_object_to_json_string( jResponse ) );
}

std::string SystemList::createSystemList()
{
    mainObject = json_object_new_object();
    json_object *jData = json_object_new_array();

    for(uint i = 0; i < vSystem.size(); ++i)
    {
        vSystem[i].fillJsonResponse();
        json_object_array_add(jData, vSystem[i].jResponse);
    }

    json_object_object_add(mainObject, "data", jData);

    addInt(mainObject, "count", uCount);

    return std::string(json_object_to_json_string(mainObject));
}
