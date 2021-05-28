#ifndef _PAYLOADS_SRSYSTEMLIST_H_
#define _PAYLOADS_SRSYSTEMLIST_H_

#include "SRPayloads.h"
#include "../utils/SRJsonBuilder.h"

class SRSystemList : SRPayloads
{
    public:

    std::vector<stSystem> vSRSystem;
    uint uCount;

    std::string createSystemList()
    {
        std::string sResp = "{\"data\": [";

        for(uint i = 0; i < vSRSystem.size(); ++i)
        {
            std::string tmp = createSRSystem(vSRSystem[i]);
            sResp += tmp + std::string(",");
        }

        sResp.back() = ']';
        sResp += ",\"count\":" + std::to_string(uCount) + "}";

        return sResp;
    }

    void fillJsonResponse(SRJsonBuilder &_jResponse, stSystem &_stSystemData)
    {
        _jResponse.addInt("id", _stSystemData.sId);
        _jResponse.addStr("systemName", _stSystemData.sSystemName);
        _jResponse.addStr("address", _stSystemData.sAddress);
        _jResponse.addInt("port", _stSystemData.sPort);
        _jResponse.addStr("authenticationInfo", _stSystemData.sAuthInfo);
        _jResponse.addStr("createdAt", _stSystemData.sCreatedAt);
        _jResponse.addStr("updatedAt", _stSystemData.sUpdatedAt);
    }

    std::string createSRSystem(stSystem &_stSystemData)
    {
        SRJsonBuilder jResponse;
        fillJsonResponse(jResponse, _stSystemData);
        return jResponse.str();
    }

};

#endif /* _PAYLOADS_SRSYSTEMLIST_H_ */
