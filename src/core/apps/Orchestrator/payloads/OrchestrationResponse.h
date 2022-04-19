#ifndef _PAYLOADS_ORCHESTRATIONRESPONSE_H_
#define _PAYLOADS_ORCHESTRATIONRESPONSE_H_

#include <map>
#include <string>
#include <vector>
#include "utils/common/CommonPayloads.h"
//#include "gason/gason.h"
#include "utils/common/CommonJsonBuilder.h"
#include <ctype.h>

class OrchestrationResponse : CommonPayloads
{

private:

    CommonJsonBuilder    jResponse;

public:

    std::vector<stOrchResponse> vOrchResponse;

    std::string createOrchestrationResponse()
    {
        std::vector<std::string> v;

        for(int i=0; i<vOrchResponse.size(); ++i)
        {
            CommonJsonBuilder jArrayElement;
            jArrayElement.addObj("provider", vOrchResponse[i].jProvider.toString());
            jArrayElement.addObj("service", vOrchResponse[i].jService.toString());
            jArrayElement.addStr("serviceUri", vOrchResponse[i].sServiceUri);
            jArrayElement.addStr("secure", vOrchResponse[i].sSecure);
            jArrayElement.addObj("metadata", vOrchResponse[i].jMetadata.toString());
            jArrayElement.addObj("interfaces", vOrchResponse[i].jInterfaces.toString());
            jArrayElement.addInt("version", vOrchResponse[i].sVersion);
            jArrayElement.addObj("authorizationTokens", vOrchResponse[i].jAuthorizationTokens.toString());
            jArrayElement.to_array("warnings", std::cbegin(vOrchResponse[i].sWarnings),  std::cend(vOrchResponse[i].sWarnings) );
            v.push_back(jArrayElement.str());
        }

        jResponse.to_arrayObj<std::vector<std::string>::iterator>("response", v.begin(), v.end());

        return jResponse.str();
    }

};

/*
{
  "response": [
    {
      "provider": {
        "id": 0,
        "systemName": "string",
        "address": "string",
        "port": 0,
        "authenticationInfo": "string",
        "createdAt": "string",
        "updatedAt": "string"
      },
      "service": {
        "id": 0,
        "serviceDefinition": "string",
        "createdAt": "string",
        "updatedAt": "string"
      },
      "serviceUri": "string",
      "secure": "TOKEN",
      "metadata": {
        "additionalProp1": "string",
        "additionalProp2": "string",
        "additionalProp3": "string"
      },
      "interfaces": [
        {
          "id": 0,
          "createdAt": "string",
          "interfaceName": "string",
          "updatedAt": "string"
        }
      ],
      "version": 0,
      "authorizationTokens": {
        "interfaceName1": "token1",
        "interfaceName2": "token2"
      },
      "warnings": [
        "FROM_OTHER_CLOUD", "TTL_UNKNOWN"
      ]
    }
  ]
}
*/

#endif /* _PAYLOADS_ORCHESTRATIONRESPONSE_H_ */
