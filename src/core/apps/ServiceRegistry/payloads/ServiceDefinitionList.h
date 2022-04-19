#ifndef _PAYLOADS_SERVICEDEFINITIONLIST_H_
#define _PAYLOADS_SERVICEDEFINITIONLIST_H_

#include "utils/common/CommonPayloads.h"
#include "utils/common/CommonJsonBuilder.h"

class ServiceDefinitionList : CommonPayloads
{
    public:

    std::vector<stServDef> vServDef;
    uint uCount;

    std::string createServiceDefinitionList()
    {
        std::string sResp = "{\"data\": [";

        for(uint i = 0; i < vServDef.size(); ++i)
        {
            std::string tmp = createServiceDefinition(vServDef[i]);
            sResp += tmp + std::string(",");
        }

        sResp.back() = ']';
        sResp += ",\"count\":" + std::to_string(uCount) + "}";

        return sResp;
    }

    void fillJsonResponse(CommonJsonBuilder &_jResponse, stServDef &_stServDefData)
    {
        _jResponse.addInt("id", _stServDefData.sId);
        _jResponse.addStr("systemName", _stServDefData.sServiceDefinition);
        _jResponse.addStr("createdAt", _stServDefData.sCreatedAt);
        _jResponse.addStr("updatedAt", _stServDefData.sUpdatedAt);
    }

    std::string createServiceDefinition(stServDef &_stServDefData)
    {
        CommonJsonBuilder jResponse;
        fillJsonResponse(jResponse, _stServDefData);
        return jResponse.str();
    }

};

#endif /* _PAYLOADS_SERVICEDEFINITIONLIST_H_ */
