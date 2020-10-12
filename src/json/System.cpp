
#include "System.h"

bool System::validJSONPayload()
{
    if( !getValue(mainObject, "system_name", sSystemName) ) return false;
    if( !getValue(mainObject, "address",     sAddress   ) ) return false;
    if( !getValue(mainObject, "port",        sPort      ) ) return false;

    return true;
}

std::string System::createSystemJSON()
{
    json_object *jResponse = json_object_new_object();

    addString(jResponse, "id", sId);
    addString(jResponse, "system_name", sSystemName);
    addString(jResponse, "address", sAddress);
    addString(jResponse, "port", sPort);
    addString(jResponse, "authentication_info", sAuthInfo);
    addString(jResponse, "created_at", sCreatedAt);
    addString(jResponse, "updated_at", sUpdatedAt);

    return std::string( json_object_to_json_string( jResponse ) );
}
