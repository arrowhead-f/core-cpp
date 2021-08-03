#ifndef _PAYLOADS_SRSYSTEM_H_
#define _PAYLOADS_SRSYSTEM_H_

#include <string>
#include <vector>
#include "SRPayloads.h"
#include "gason/gason.h"
#include "../utils/SRJsonBuilder.h"
#include <ctype.h>

class SRSystem : SRPayloads
{
    private:

        gason::JsonAllocator jsonAllocator;
        gason::JsonValue     jsonRootValue;

        SRJsonBuilder        jResponse;

    public:
        stSystem stSystemData;

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
            stSystemData.sSystemName = std::string(jsonRootValue.child("systemName").toString(&b));
            if(stSystemData.sSystemName.size() == 0) return 2;

            toLowerAndTrim(stSystemData.sSystemName);
            if(stSystemData.sSystemName.size() == 0) return 2;

            if ( !jsonRootValue.child("address")                                   ) return 3;
            if (  jsonRootValue.child("address").getTag() != gason::JSON_STRING    ) return 4;/*JSON_NULL or JSON_FALSE*/
            b = true;
            stSystemData.sAddress = std::string(jsonRootValue.child("address").toString(&b));
            if(stSystemData.sAddress.size() == 0) return 4;

            if ( !jsonRootValue.child("port")                                      ) return 5;

            int port = jsonRootValue.child("port").toInt();
            if( ( port < 0 ) || ( port > 65535 ) )
                return 6;

            stSystemData.sPort = std::to_string(port);

            b = true;
            if ( jsonRootValue.child("authenticationInfo") )
                if ( jsonRootValue.child("authenticationInfo").getTag() == gason::JSON_STRING )
                    stSystemData.sAuthInfo = std::string(jsonRootValue.child("authenticationInfo").toString(&b));

            return 0;
        }

        void fillJsonResponse()
        {
            jResponse.addInt("id", stSystemData.sId);
            jResponse.addStr("systemName", stSystemData.sSystemName);
            jResponse.addStr("address", stSystemData.sAddress);
            jResponse.addInt("port", stSystemData.sPort);
            jResponse.addStr("authenticationInfo", stSystemData.sAuthInfo);
            jResponse.addStr("createdAt", stSystemData.sCreatedAt);
            jResponse.addStr("updatedAt", stSystemData.sUpdatedAt);
        }

        std::string createSRSystem()
        {
            fillJsonResponse();
            return jResponse.str();
        }

};

#endif /* _PAYLOADS_SRSYSTEM_H_ */
