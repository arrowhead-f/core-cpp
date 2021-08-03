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

    bool setJsonPayload(std::string &_sJsonPayload)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status == gason::JSON_PARSE_OK )
            return true;

        return false;
    }

    uint8_t validJSONPayload()
    {
        try{
            if ( !jsonRootValue.child("serviceDefinition")                              ) return 1;
            if (  jsonRootValue.child("serviceDefinition").getTag() != gason::JSON_STRING ) return 2;/*JSON_NULL or JSON_FALSE*/
            bool b = true;
            stServDefData.sServiceDefinition = std::string(jsonRootValue.child("serviceDefinition").toString(&b));
            if(stServDefData.sServiceDefinition.size() == 0) return 2;
            toLowerAndTrim(stServDefData.sServiceDefinition);
        }
        catch(...)
        {
            return 10;
        }

        return 0;
    }

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
