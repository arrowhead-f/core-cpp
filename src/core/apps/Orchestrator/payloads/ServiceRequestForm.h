#ifndef _PAYLOADS_SERVICEREQUESTFORM_H_
#define _PAYLOADS_SERVICEREQUESTFORM_H_

#include <map>
#include <string>
#include <vector>
#include "utils/common/CommonPayloads.h"
#include "gason/gason.h"
#include "utils/common/CommonJsonBuilder.h"
#include <ctype.h>

class ServiceRequestForm : CommonPayloads
{

private:

    gason::JsonAllocator jsonAllocator;
    gason::JsonValue     jsonRootValue;

public:
    std::string sServiceDefinitionReq;
    stServiceRequestForm stServReqForm;
    std::vector<gason::JsonValue> vPreferredProviderDataValidated;
    std::vector<std::string> vKnownOrchFlags;

    bool setJsonPayload(std::string &_sJsonPayload)
    {
        auto status = gason::jsonParse(_sJsonPayload.data(), jsonRootValue, jsonAllocator);

        if ( status != gason::JSON_PARSE_OK )
            return false;

        vKnownOrchFlags.push_back("onlyPreferred");
        vKnownOrchFlags.push_back("overrideStore");
        vKnownOrchFlags.push_back("externalServiceRequest");
        vKnownOrchFlags.push_back("enableInterCloud");
        vKnownOrchFlags.push_back("enableQoS");
        vKnownOrchFlags.push_back("matchmaking");
        vKnownOrchFlags.push_back("metadataSearch");
        vKnownOrchFlags.push_back("triggerInterCloud");
        vKnownOrchFlags.push_back("pingProviders");

        return true;
    }

    uint8_t parseRequestForm(bool bDynamicOrchestration)
    {
        if ( !jsonRootValue.child("requesterSystem") )
            return 1;

        stServReqForm.jRequesterSystem = jsonRootValue.child("requesterSystem");

        if( bDynamicOrchestration )
        {
            if ( !jsonRootValue.child("requestedService") )
                return 2;

            if (  jsonRootValue.child("requestedService")("serviceDefinitionRequirement").getTag() != gason::JSON_STRING ) /*JSON_NULL or JSON_FALSE*/
                return 3;

            bool b = true;

            sServiceDefinitionReq = std::string(jsonRootValue.child("requestedService")("serviceDefinitionRequirement").toString(&b));

            if(sServiceDefinitionReq.size() == 0)
                return 4;

            stServReqForm.jRequestedService = jsonRootValue.child("requestedService");
            stServReqForm.bRequestedServiceExists = true;
        }
        else
        {
            if ( jsonRootValue.child("requestedService") )
            {
                stServReqForm.jRequestedService = jsonRootValue.child("requestedService");
                stServReqForm.bRequestedServiceExists = true;
            }
            else
            {
                stServReqForm.bRequestedServiceExists = false;
            }
        }


        if ( jsonRootValue.child("preferredProviders") )
        {
            stServReqForm.jPreferredProviders = jsonRootValue.child("preferredProviders");
            stServReqForm.bPreferredProvidersExits = true;
        }
        else
        {
            stServReqForm.bPreferredProvidersExits = false;
        }

        if ( jsonRootValue.child("orchestrationFlags") )
        {
            for(int i=0; i < vKnownOrchFlags.size(); ++i)
            {
                if ( jsonRootValue.child("orchestrationFlags")(vKnownOrchFlags[i].c_str()).getTag() == gason::JSON_TRUE )
                    stServReqForm.mFlags.insert(std::pair<std::string, bool>(vKnownOrchFlags[i], true));
                else if ( jsonRootValue.child("orchestrationFlags")(vKnownOrchFlags[i].c_str()).getTag() == gason::JSON_FALSE )
                    stServReqForm.mFlags.insert(std::pair<std::string, bool>(vKnownOrchFlags[i], false));
            }
        }

        return 0;
    }

    bool getFlagValue(std::string sFlagName)
    {
        if( stServReqForm.mFlags.find(sFlagName) == stServReqForm.mFlags.end() )
            return false;
        else
            return stServReqForm.mFlags[sFlagName];
    }

};

#endif /* _PAYLOADS_SERVICEREQUESTFORM_H_ */
