
#include "ServiceDefinition.h"

bool ServiceDefinition::validJSONPayload()
{
    if( !getValue(mainObject, "serviceDefinition", sServiceDefinition) ) return false;

    return true;
}

void ServiceDefinition::fillJsonResponse()
{
    mainObject = json_object_new_object();
    addInt   (mainObject, "id"               , sId);
    addString(mainObject, "serviceDefinition", sServiceDefinition);
    addString(mainObject, "createdAt"        , sCreatedAt);
    addString(mainObject, "updatedAt"        , sUpdatedAt);
}


std::string ServiceDefinition::createServiceDefinition()
{
    fillJsonResponse();
    return std::string(json_object_to_json_string(mainObject));
}

std::string ServiceDefinitionList::createServiceDefinitionList()
{
    mainObject = json_object_new_object();
    json_object *jData = json_object_new_array();

    for(uint i = 0; i < vServiceDefinition.size(); ++i)
    {
        vServiceDefinition[i].fillJsonResponse();
        json_object_array_add(jData, vServiceDefinition[i].mainObject);
    }

    json_object_object_add(mainObject, "data", jData);

    addInt(mainObject, "count", uCount);

    return std::string(json_object_to_json_string(mainObject));
}
