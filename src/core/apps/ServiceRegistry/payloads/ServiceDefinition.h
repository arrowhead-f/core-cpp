#ifndef _PAYLOADS_SERVICEDEFINITION_H_
#define _PAYLOADS_SERVICEDEFINITION_H_

#include <map>
#include <string>
#include <vector>
#include "SRPayloads.h"
#include "gason/gason.h"
#include "../utils/SRJsonBuilder.h"
#include <ctype.h>

class ServiceDefinition : SRPayloads
{

private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

    SRJsonBuilder        jResponse;

public:
    stServDef stServDefData;

/*
    bool setJsonPayload(std::string &_sJsonPayload)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status == gason::JSON_PARSE_OK )
            return true;

        return false;
    }

    bool validJSONPayload()
    {
        if( !getValue(mainObject, "serviceDefinition", sServiceDefinition) ) return false;

        return true;
    }
*/

    void fillJsonResponse()
    {
        jResponse.addInt( "id", stServDefData.sId);
        jResponse.addStr( "serviceDefinition", stServDefData.sServiceDefinition);
        jResponse.addStr( "createdAt"        , stServDefData.sCreatedAt);
        jResponse.addStr( "updatedAt"        , stServDefData.sUpdatedAt);
    }

    std::string createServiceDefinition()
    {
        fillJsonResponse();
        return jResponse.str();
    }

};

#endif /* _PAYLOADS_SERVICEDEFINITION_H_ */
