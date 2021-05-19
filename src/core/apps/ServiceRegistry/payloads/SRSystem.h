#ifndef _PAYLOADS_SRSYSTEM_H_
#define _PAYLOADS_SRSYSTEM_H_

#include <string>
#include <vector>
#include "commonPayloads.h"
#include "gason/gason.h"
#include "../utils/SRJsonBuilder.h"

#include <ctype.h>

class SRSystem
{
    private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

    SRJsonBuilder        jResponse;

    public:
        std::string sId;
        std::string sSystemName;
        std::string sAddress;
        std::string sPort;
        std::string sAuthInfo;
        std::string sCreatedAt;
        std::string sUpdatedAt;

        bool setJsonPayload(std::string &_sJsonPayload)
        {
            auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

            if ( status == gason::JSON_PARSE_OK )
                return true;

            return false;
        }

        uint8_t validSystem()
        {
            if ( !jsonRootValue.child("systemName")                                ) return 1;
            if (  jsonRootValue.child("systemName").getTag() != gason::JSON_STRING ) return 2;/*JSON_NULL or JSON_FALSE*/
            bool b = true;
            sSystemName = std::string(jsonRootValue.child("systemName").toString(&b));
            if(sSystemName.size() == 0) return 2;

            if ( !jsonRootValue.child("address")                                   ) return 3;
            if (  jsonRootValue.child("address").getTag() != gason::JSON_STRING    ) return 4;/*JSON_NULL or JSON_FALSE*/
            b = true;
            sAddress = std::string(jsonRootValue.child("address").toString(&b));
            if(sAddress.size() == 0) return 4;

            if ( !jsonRootValue.child("port")                                      ) return 5;

            int port = jsonRootValue.child("port").toInt();
            if( ( port < 0 ) || ( port > 65535 ) )
                return 6;

            sPort = std::to_string(port);

            return 0;
        }

        void fillJsonResponse()
        {
            jResponse.addInt("id", sId);
            jResponse.addStr("systemName", sSystemName);
            jResponse.addStr("address", sAddress);
            jResponse.addInt("port", sPort);
            jResponse.addStr("authenticationInfo", sAuthInfo);
            jResponse.addStr("createdAt", sCreatedAt);
            jResponse.addStr("updatedAt", sUpdatedAt);
        }

        std::string createSRSystem()
        {
            fillJsonResponse();
            return jResponse.str();
        }
};

#endif /* _PAYLOADS_SRSYSTEM_H_ */
