
#ifndef _ENDPOINTS_ORCHESTRATION_H_
#define _ENDPOINTS_ORCHESTRATION_H_

#include "http/crate/Request.h"
#include "http/crate/Response.h"

#include "http/KeyProvider.h"
#include "http/wget/WG_Curl.h"

#include "../payloads/ServiceRequestForm.h"
#include "../payloads/OrchestrationResponse.h"

#include "core/apps/ServiceRegistry/payloads/ServiceQueryForm.h"

#include "core/Core.h"
#include "utils/common/CommonPayloads.h"
#include "utils/common/DbWrapper.h"
#include "utils/common/Error.h"


#include <stdlib.h>
#include <time.h>

#define EMPTY_RESPONSE 1

template<typename DB, typename RB>
class Orchestration : CommonPayloads{

    private:
        DB &db;
        RB &rb;
        ServiceRequestForm oServiceRequestForm;

    public:

        Orchestration(DB &db, RB &rb) : db{ db }, rb { rb } {}

        Response processOrchestrationId(int consumerSystemId, std::string errURL)
        {
            return ErrorResp{"in progress", 400, "BAD_PAYLOAD", errURL}.getResp();
        }

//////////////////////////
//                      //
// processOrchestration //
//                      //
//////////////////////////

        Response processOrchestration(Request &&req, std::string errURL)
        {
            ServiceRequestForm oInputServiceRequestForm;

            if(!oInputServiceRequestForm.setJsonPayload(req.content))
                return ErrorResp{"Bad Json", 400, "BAD_PAYLOAD", errURL}.getResp();

            uint8_t status = oInputServiceRequestForm.parseRequestForm(true);

            switch(status)
            {
                case 1:
                    return ErrorResp{"System name is null or blank.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 2:
                case 3:
                case 4:
                    return ErrorResp{"Requested service definition requirement is null or blank.", 400, "BAD_PAYLOAD", errURL}.getResp();
            }

            return dynamicOrchestration(oInputServiceRequestForm, errURL);
        }

        Response dynamicOrchestration(ServiceRequestForm &oInputServiceRequestForm, std::string errURL)
        {
            uint8_t status = checkServiceRequestForm(oInputServiceRequestForm);

            switch(status)
            {
                case 1: return ErrorResp{"Requested service can not be null when \"overrideStore\" is TRUE", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 2: return ErrorResp{"Requested service can not be null when \"triggerInterCloud\" is TRUE", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 3: return ErrorResp{"There is no valid preferred provider, but \"onlyPreferred\" is set to true", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 4: return ErrorResp{"Service definition requirement is null or blank.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 5: return ErrorResp{"System name is null or blank.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 6: return ErrorResp{"System address is null or blank.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 7: return ErrorResp{"System port is null.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 8: return ErrorResp{"Port must be between 0 and 65535.", 400, "BAD_PAYLOAD", errURL}.getResp();
                case 9: return ErrorResp{"Cloud operator is null or blank.", 400, "INVALID_PARAMETER", errURL}.getResp();
                case 10: return ErrorResp{"Cloud name is null or blank.", 400, "INVALID_PARAMETER", errURL}.getResp();
            }

            std::string sSRQueryResult;
            if( !queryServiceRegistry(oInputServiceRequestForm, sSRQueryResult) )
                return Response{ "{\"response\":[]}" };

            gason::JsonAllocator   jsonAllocator;
            gason::JsonValue       jsonRootValue;
            gason::JsonParseStatus jStatus = gason::jsonParse(sSRQueryResult.data(), jsonRootValue, jsonAllocator);

            if( jStatus != gason::JSON_PARSE_OK )
                return Response{ "{\"response\":[]}" };

            std::vector<gason::JsonValue> queryData;
            for( gason::JsonIterator it = gason::begin(jsonRootValue.child("serviceQueryData")); it != gason::JsonIterator(nullptr); ++it )
                queryData.push_back(it->value);

            if(queryData.size() == 0)
                return Response{ "{\"response\":[]}" };

            std::map<int, std::vector<gason::JsonValue>> mAuthorizedProvIntfs;

            uint8_t authRes = queryAuthorization(oInputServiceRequestForm, queryData, mAuthorizedProvIntfs);
            if(!authRes || mAuthorizedProvIntfs.size() == 0)
                return Response{ "{\"response\":[]}" };

            std::vector<gason::JsonValue> preferredQueryData;
            bool bOnlyPreferred = oInputServiceRequestForm.getFlagValue("onlyPreferred");
            if(bOnlyPreferred)
            {
                selectPreferred(oInputServiceRequestForm, queryData, preferredQueryData);
                if(preferredQueryData.size() == 0)
                    return Response{ "{\"response\":[]}" };
            }

            std::string sRequester = convertGasonToCJB(oInputServiceRequestForm.stServReqForm.jRequesterSystem).str();

            if( oInputServiceRequestForm.getFlagValue("matchmaking") )
            {
                if(bOnlyPreferred)
                {
                    int max = preferredQueryData.size();
                    srand(time(NULL));
                    int index = rand() % max + 0; //between 0 and max
                    std::vector<gason::JsonValue> vResp;
                    vResp.push_back(preferredQueryData[index]);
                    return createOrchestrationResponse(vResp, mAuthorizedProvIntfs, sRequester);
                }
                else
                {
                    int max = mAuthorizedProvIntfs.size();
                    srand(time(NULL));
                    int index = rand() % max + 0; //between 0 and max
                    int randProvId = 0;
                    for (const auto& [key, value] : mAuthorizedProvIntfs)
                    {
                        if(index == randProvId++)
                        {
                            randProvId = key;
                            break;
                        }
                    }

                    for(int i=0; i<queryData.size(); ++i)
                    {
                        if(queryData[i].child("provider")("id").toInt() == randProvId)
                        {
                            std::vector<gason::JsonValue> vResp;
                            vResp.push_back(queryData[i]);
                            return createOrchestrationResponse(vResp, mAuthorizedProvIntfs, sRequester);
                        }
                    }

                }
            }

            if(bOnlyPreferred)
                return createOrchestrationResponse(preferredQueryData, mAuthorizedProvIntfs, sRequester);
            else
                return createOrchestrationResponse(queryData, mAuthorizedProvIntfs, sRequester);
        }

        std::string createTokenRule(std::string &sConsumer, gason::JsonValue &jSRData, std::vector<gason::JsonValue> &vAuthorizedInterfaces)
        {
            CommonJsonBuilder jTokenRule;
            jTokenRule.addObj("consumer", sConsumer);
            jTokenRule.addNULL("consumerCloud");
            jTokenRule.addInt("duration", -1);

            CommonJsonBuilder jArrayElement;
            CommonJsonBuilder jProvElement;
            jProvElement.addStr("address", jSRData.child("provider")("address").toString());
            jProvElement.addStr("port", std::to_string(jSRData.child("provider")("port").toInt()) );
            jProvElement.addStr("systemName", jSRData.child("provider")("systemName").toString());

            if(jSRData.child("provider")("authenticationInfo").getTag() == gason::JSON_STRING)
                jProvElement.addStr("authenticationInfo", jSRData.child("provider")("authenticationInfo").toString());

            jArrayElement.addObj("provider", jProvElement.str());

            std::vector<std::string> vIntfNames;

            for(int i = 0; i < vAuthorizedInterfaces.size();++i)
            {
                if(vAuthorizedInterfaces[i].child("interfaceName"))
                {
                    vIntfNames.push_back(std::string(vAuthorizedInterfaces[i].child("interfaceName").toString()));
                }
            }

            jArrayElement.to_arrayObj<std::vector<std::string>::iterator>("serviceInterfaces", vIntfNames.begin(), vIntfNames.end());
            std::vector<std::string> v;
            v.push_back(jArrayElement.str());

            jTokenRule.to_arrayObj<std::vector<std::string>::iterator>("providers", v.begin(), v.end());
//----
            jTokenRule.addStr( "service", std::string(jSRData.child("serviceDefinition")("serviceDefinition").toString()) );

            return jTokenRule.str();
        }


        Response createOrchestrationResponse(std::vector<gason::JsonValue> &vProviders, std::map<int, std::vector<gason::JsonValue>> &mAuthorizedProvIntfs, std::string sConsumer)
        {
            CommonJsonBuilder jOrchResp;
            std::vector<std::string> vOrchResp;

            for(int i=0; i<vProviders.size(); ++i)
            {
                CommonJsonBuilder respArrayElement;

                respArrayElement.addObj("provider",   convertGasonToCJB(vProviders[i].child("provider")).str() );
                respArrayElement.addObj("service",    convertGasonToCJB(vProviders[i].child("serviceDefinition")).str() );
                respArrayElement.addStr("serviceUri", std::string( vProviders[i].child("serviceUri").toString() ));
                respArrayElement.addStr("secure",     std::string( vProviders[i].child("secure").toString() ));
                respArrayElement.addObj("metadata",   convertGasonToCJB( vProviders[i].child("metadata")).str() );
                respArrayElement.addInt("version",    vProviders[i].child("version").toInt());

                //interfaces

                std::vector<std::string> vIntf;

                int provId = vProviders[i].child("provider")("id").toInt();

                if( mAuthorizedProvIntfs.find(provId) == mAuthorizedProvIntfs.end())
                    continue;

                for(int j=0; j < mAuthorizedProvIntfs[provId].size(); ++j)
                {
                    std::string authIntf  = convertGasonToCJB( mAuthorizedProvIntfs[provId][j] ).str();
                    vIntf.push_back( authIntf );
                }

                respArrayElement.to_arrayObj<std::vector<std::string>::iterator>("interfaces", vIntf.begin(), vIntf.end());

                //
                std::string s1 = std::string(vProviders[i].child("secure").toString());
                toLowerAndTrim(s1);

                if( s1.compare("token") == 0 )
                {
                    int provID = vProviders[i].child("provider")("id").toInt();
                    std::string sTokenRule = createTokenRule(sConsumer, vProviders[i], mAuthorizedProvIntfs[provID]);

                    auto queryAuthTokensRes = rb.send("POST", "https://127.0.0.1/authorization/token", 12345, sTokenRule);

                    if( queryAuthTokensRes != http::status_code::OK ) continue;

                    std::string sAuthTokenResult = queryAuthTokensRes.value();

                    gason::JsonAllocator   jsonAllocator;
                    gason::JsonValue       jAuthTokens;
                    gason::JsonParseStatus jStatus = gason::jsonParse(sAuthTokenResult.data(), jAuthTokens, jsonAllocator);

                    if( jStatus != gason::JSON_PARSE_OK )
                        continue;

                    if(!jAuthTokens.child("tokenData")[0])
                        continue;

                    if(!jAuthTokens.child("tokenData")[0].child("tokens"))
                        continue;

                    std::string sTokens = convertGasonToCJB( jAuthTokens.child("tokenData")[0].child("tokens") ).str();

                    respArrayElement.addObj("authorizationTokens", sTokens.c_str());
                }

                std::vector<std::string> vWarnings;
                std::string sWarning = calculateOrchestratorWarnings(vProviders[i]);
                if(sWarning.size())
                    vWarnings.push_back(sWarning);

                respArrayElement.to_arrayObj<std::vector<std::string>::iterator>("warnings", vWarnings.begin(), vWarnings.end());

                vOrchResp.push_back(respArrayElement.str());
            }

            jOrchResp.to_arrayObj<std::vector<std::string>::iterator>("response", vOrchResp.begin(), vOrchResp.end());

            return Response{ jOrchResp.str() };
        }


        std::string calculateOrchestratorWarnings(gason::JsonValue &jProv)
        {
//
//Possible values:
//TTL_UNKNOWN, TTL_EXPIRED, TTL_EXPIRING, FROM_OTHER_CLOUD
//
            uint64_t localTimeNow = convertStrTimeToUint64(getLocalTime());

            if(!jProv.child("endOfValidity"))
                return "\"TTL_UNKNOWN\"";

            std::string sEndOfValidity = std::string(jProv.child("endOfValidity").toString());
            uint64_t uEndOfValidity = convertStrTimeToUint64(sEndOfValidity);

            if(uEndOfValidity < localTimeNow)
                return "\"TTL_EXPIRED\"";

            uint64_t EXPIRING_TIME_IN_MINUTES = 2 * 60;

            if( (uEndOfValidity+EXPIRING_TIME_IN_MINUTES) < localTimeNow )
                return "\"TTL_EXPIRING\"";

            return "";
        }

        bool providerEquals(gason::JsonValue &jCurrentProv, gason::JsonValue &jCurrentPrefProv)
        {
            std::string currentSystemName = std::string(jCurrentProv.child("provider")("systemName").toString());
            std::string currentAddress = std::string(jCurrentProv.child("provider")("address").toString());
            int currentPort = jCurrentProv.child("provider")("port").toInt();

            std::string currentPrefSystemName = std::string(jCurrentPrefProv.child("providerSystem")("systemName").toString());
            std::string currentPrefAddress = std::string(jCurrentPrefProv.child("providerSystem")("address").toString());
            int currentPrefPort = jCurrentPrefProv.child("providerSystem")("port").toInt();

            toLowerAndTrim(currentSystemName);
            toLowerAndTrim(currentPrefSystemName);

            if(currentSystemName.compare(currentPrefSystemName) != 0) return false;
            if(currentAddress.compare(currentPrefAddress) != 0) return false;
            if(currentPort != currentPrefPort) return false;

            return true;
        }

        void selectPreferred(ServiceRequestForm &oInputServiceRequestForm, std::vector<gason::JsonValue> &queryData, std::vector<gason::JsonValue> &preferredQueryData)
        {
            if(!oInputServiceRequestForm.stServReqForm.bPreferredProvidersExits)
                return;

            for(std::vector<gason::JsonValue>::iterator it = queryData.begin(); it != queryData.end(); ++it)
            {
                for( gason::JsonIterator itPref = gason::begin(oInputServiceRequestForm.stServReqForm.jPreferredProviders); itPref != gason::JsonIterator(nullptr); ++itPref )
                {
                    if( !isLocal( itPref->value ) )
                        continue;

                    if(providerEquals(*it, itPref->value))
                    {
                        preferredQueryData.push_back(*it);
                        break;
                    }
                }
            }
        }

        bool queryServiceRegistry(ServiceRequestForm &oInputServiceRequestForm, std::string &sSRQueryResult)
        {
            CommonJsonBuilder jRequestedService = convertGasonToCJB(oInputServiceRequestForm.stServReqForm.jRequestedService);
            auto querySRResult = rb.send("POST", "https://127.0.0.1/serviceregistry/query", 12345, jRequestedService.str());

            if( querySRResult != http::status_code::OK ) return false;

            sSRQueryResult = querySRResult.value();
            return true;
        }

        uint8_t queryAuthorization(ServiceRequestForm &oInputServiceRequestForm, std::vector<gason::JsonValue> &queryData, std::map<int, std::vector<gason::JsonValue>> &mAuthorizedProvIntfs)
        {
            auto authorizationQueryResult = rb.send("POST", "https://127.0.0.1/authorization/intracloud/check", 12345, createIntraCloudRule(oInputServiceRequestForm, queryData));

            if( authorizationQueryResult != http::status_code::OK ) return 0;

            std::string sAuthRes = authorizationQueryResult.value();

            return filterProviderListUsingAuthorizationResult(queryData, sAuthRes, mAuthorizedProvIntfs);
        }

        std::string createIntraCloudRule(ServiceRequestForm &oInputServiceRequestForm, std::vector<gason::JsonValue> &queryData)
        {
            CommonJsonBuilder jIntraCloudRule;
            jIntraCloudRule.addObj("consumer", convertGasonToCJB(oInputServiceRequestForm.stServReqForm.jRequesterSystem).str());
            jIntraCloudRule.addInt("serviceDefinitionId", queryData[0]("serviceDefinition")("id").toInt());

            std::vector<std::string> vProvIdWithIntfIds;

            for(int i=0; i<queryData.size(); ++i)
            {
                CommonJsonBuilder jProvIdWithIntfIdArrayElement;

                jProvIdWithIntfIdArrayElement.addInt("id",queryData[i]("provider")("id").toInt());
                std::vector<int> vIntfIds;
                for(int j=0; ;++j)
                {
                    if(queryData[i]("interfaces")[j])
                        vIntfIds.push_back(queryData[i]("interfaces")[j]("id").toInt());
                    else
                        break;
                }

                jProvIdWithIntfIdArrayElement.to_arrayObj<std::vector<int>::iterator>("idList", vIntfIds.begin(), vIntfIds.end());
                vProvIdWithIntfIds.push_back(jProvIdWithIntfIdArrayElement.str());
            }

            jIntraCloudRule.to_arrayObj<std::vector<std::string>::iterator>("providerIdsWithInterfaceIds", vProvIdWithIntfIds.begin(), vProvIdWithIntfIds.end());
            std::string sIntraCloudRule = jIntraCloudRule.str();

            return sIntraCloudRule;
        }

        uint8_t filterProviderListUsingAuthorizationResult(std::vector<gason::JsonValue> &queryData, std::string sAuthRes, std::map<int, std::vector<gason::JsonValue>> &mAuthorizedProvIntfs)
        {
            gason::JsonAllocator   jsonAllocator;
            gason::JsonValue       authResultBody;
            gason::JsonParseStatus jStatus = gason::jsonParse(sAuthRes.data(), authResultBody, jsonAllocator);

            if( jStatus != gason::JSON_PARSE_OK ) return 0;

            for(std::vector<gason::JsonValue>::iterator it1 = queryData.begin(); it1 != queryData.end(); ++it1)
            {
                int iSRProvId = it1->child("provider")("id").toInt();

                for( gason::JsonIterator it2 = gason::begin(authResultBody.child("authorizedProviderIdsWithInterfaceIds")); it2 != gason::JsonIterator(nullptr); ++it2 )
                {
                    if(it2->value.child("id").toInt() == iSRProvId)
                    {
                        for(gason::JsonIterator it3 = gason::begin(it2->value.child("idList")); it3 != gason::JsonIterator(nullptr); ++it3)
                        {
                            for(gason::JsonIterator it4 = gason::begin(it1->child("interfaces")); it4 != gason::JsonIterator(nullptr); ++it4)
                            {
                                if(it4->value.child("id").toInt() == it3->value.toInt() /*iAuthIntfId*/)
                                {
                                    if(mAuthorizedProvIntfs.find(iSRProvId) != mAuthorizedProvIntfs.end())
                                    {
                                        mAuthorizedProvIntfs[iSRProvId].push_back(it4->value);
                                    }
                                    else
                                    {
                                        std::vector<gason::JsonValue> vtmp;
                                        vtmp.push_back(it4->value);
                                        mAuthorizedProvIntfs.insert(std::pair<int, std::vector<gason::JsonValue>>(iSRProvId, vtmp));
                                    }
                                }
                            }
                        }
                    }
                }
            }

            return 1;
        }

        bool isInterCloudOrchestrationPossible(ServiceRequestForm &oInputServiceRequestForm)
        {
//todo: make gateKeeperIsPresent as commonConstant value
            bool gateKeeperIsPresent = false;
            return gateKeeperIsPresent && oInputServiceRequestForm.getFlagValue("enableInterCloud");
        }

        uint8_t checkSystemRequest(gason::JsonValue jProviderSystem)
        {
            if( !jProviderSystem.child("systemName") )
                return 5;
            if( !jProviderSystem.child("address") )
                return 6;
            if( !jProviderSystem.child("port") )
                return 7;

            int port = jProviderSystem.child("port").toInt();

            if( ( port < 0 ) || ( port > 65535 ) )
                return 8;

            return 0;
        }

        uint8_t checkServiceRequestForm(ServiceRequestForm &oInputServiceRequestForm)
        {
            uint8_t status = validateCrossParameterConstraints(oInputServiceRequestForm);

            if(!status)
                return status;

            if( oInputServiceRequestForm.sServiceDefinitionReq.size() == 0 )
                return 4;

            for(int i=0; i<oInputServiceRequestForm.vPreferredProviderDataValidated.size(); ++i)
            {
                status = checkSystemRequest(oInputServiceRequestForm.vPreferredProviderDataValidated[i].child("providerSystem"));

                if(status != 0)
                    return status;

                if( isInterCloudOrchestrationPossible(oInputServiceRequestForm) )
                {
                    if(oInputServiceRequestForm.vPreferredProviderDataValidated[i].child("providerCloud"))
                    {
                        if( !oInputServiceRequestForm.vPreferredProviderDataValidated[i].child("providerCloud")("operator") )
                            return 9;
                        if( !oInputServiceRequestForm.vPreferredProviderDataValidated[i].child("providerCloud")("name") )
                            return 10;
                    }
                }
            }
            return 0;
        }

        uint8_t validateCrossParameterConstraints(ServiceRequestForm &oInputServiceRequestForm)
        {
            if( !oInputServiceRequestForm.stServReqForm.bRequestedServiceExists && oInputServiceRequestForm.getFlagValue("overrideStore"))
               return 1;

            if( !oInputServiceRequestForm.stServReqForm.bRequestedServiceExists && oInputServiceRequestForm.getFlagValue("triggerInterCloud"))
               return 2;

            if( oInputServiceRequestForm.getFlagValue("onlyPreferred") )
            {
                if(!oInputServiceRequestForm.stServReqForm.bPreferredProvidersExits)
                    return 3;

                for( int i = 0; ;++i )
                {
                    if( oInputServiceRequestForm.stServReqForm.jPreferredProviders[i] )
                    {
                        if ( oInputServiceRequestForm.getFlagValue("triggerInterCloud") && !preferredProviderIsValid(oInputServiceRequestForm.stServReqForm.jPreferredProviders[i]))
        					continue;
                        else if( !isLocal( oInputServiceRequestForm.stServReqForm.jPreferredProviders[i] ) )
        					continue;
                        else
                            oInputServiceRequestForm.vPreferredProviderDataValidated.push_back(oInputServiceRequestForm.stServReqForm.jPreferredProviders[i]);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            return 0;
        }

        bool isLocal(gason::JsonValue jProv)
        {
            if( !jProv.child("providerCloud") && jProv.child("providerSystem"))
                return true;
            return false;
        }

        bool isGlobal(gason::JsonValue jProv)
        {
            if( jProv.child("providerCloud") )
                return true;
            return false;
        }

        bool preferredProviderIsValid(gason::JsonValue jProv)
        {
            return isLocal(jProv) || isGlobal(jProv);
        }
};


#endif   /* _ENDPOINTS_ORCHESTRATION_H_ */
