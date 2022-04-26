////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, orchestrator
/// Date:      2021-04-24
/// Author(s): ng201, tt
///
/// Description:
/// * this will test Orchestrator
/// * happier paths
///   * the format of the json found in the
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <ctime>
#include <string>

#include "core/apps/Orchestrator/Orchestrator.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("orchestrator: GET /echo", "[core] [orchestrator]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, reqBuilder };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "Got it!");
}


/////////////////////////////////
// Orchestrator - /orchestration - matchmaking
////////////////////////////////

const char* szOrchestatorInputPayload =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000,"
    "\"authenticationInfo\": \"Public_Key\""
  "},"
  "\"requestedService\": {"
    "\"serviceDefinitionRequirement\": \"TestService_1\","
    "\"interfaceRequirements\": ["
      "\"HTTP-INSECURE-JSON\""
    "],"
    "\"securityRequirements\": ["
      "\"NOT_SECURE\", \"CERTIFICATE\", \"TOKEN\""
    "],"
    "\"metadataRequirements\": {"
      "\"meta1\": \"test1\""
    "},"
    "\"versionRequirement\": 3,"
    "\"maxVersionRequirement\": 5,"
    "\"minVersionRequirement\": 0"
  "},"
  "\"preferredProviders\": ["
    "{"
      "\"providerCloud\": {"
        "\"operator\": \"AITIA\","
        "\"name\": \"AITIA\""
      "},"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName1\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "},"
    "{"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName2\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "}"
  "],"
  "\"orchestrationFlags\": {"
    "\"onlyPreferred\" : false,"
    "\"overrideStore\" : false,"
    "\"externalServiceRequest\" : false,"
    "\"enableInterCloud\" : false,"
    "\"enableQoS\" : false,"
    "\"matchmaking\" : true,"
    "\"metadataSearch\" : false,"
    "\"triggerInterCloud\" : false,"
    "\"pingProviders\" : false"
  "}"
"}";

    const char *serviceRegistryResp =
    "{"
        "\"serviceQueryData\": ["
         "{"
            "\"id\": 4,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 1,\"systemName\": \"testsystemname1\",\"address\": \"127.0.0.1\",\"port\": 1234,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2021-09-11 10:39:08\","
            "\"secure\": \"not_secure\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
         "{"
            "\"id\": 5,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 2,\"systemName\": \"testsystemname2\",\"address\": \"127.0.0.1\",\"port\": 5002,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
            "\"secure\": \"token\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": ["
                "{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"},"
                "{\"id\": 3,\"interfaceName\": \"http-insecure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"}"
                "],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "}"
        "],"
        "\"unfilteredHits\": 2"
    "}";

    const char *authorizerResp =
    "{"
          "\"authorizedProviderIdsWithInterfaceIds\": ["
            "{"
             "\"id\":1,"
             "\"idList\":["
                "2"
             "]"
            "},"
            "{"
             "\"id\":2,"
             "\"idList\":["
                "2,"
                "3"
             "]"
            "}"
          "],"
          "\"consumer\": {"
            "\"address\": \"127.0.0.1\","
            "\"authenticationInfo\": \"Public_Key\","
            "\"createdAt\": \"2021-10-11 10:39:08\","
            "\"id\": 1,"
            "\"port\": 8000,"
            "\"systemName\": \"ClientSystemName\","
            "\"updatedAt\": \"2022-03-11 10:39:08\""
          "},"
          "\"serviceDefinitionId\": 1"
    "}";

    const char *authorizerTokenResp =
    "{"
      "\"tokenData\": ["
        "{"
          "\"providerAddress\": \"127.0.0.1\","
          "\"providerName\": \"testsystemname2\","
          "\"providerPort\": 5002,"
          "\"tokens\": {"
            "\"http-secure-json\": \"token1\","
            "\"http-insecure-json\": \"token2\""
          "}"
        "}"
      "]"
    "}";


TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_matchmaking", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{
                        { "POST:https://127.0.0.1/query", { 200, serviceRegistryResp} },
                        { "POST:https://127.0.0.1/intracloud/check", { 200, authorizerResp} },
                        { "POST:https://127.0.0.1/token", { 200, authorizerTokenResp} }
                      };

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload) });

    REQUIRE(resp == http::status_code::OK);

    if(resp.value().size() == 813)
        REQUIRE(JsonCompare(resp.value(), R"json({"response":[{"provider": {"id": 2,"systemName": "testsystemname2","address": "127.0.0.1","port": 5002,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri2","secure": "token","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"},{"id": 3,"interfaceName": "http-insecure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"}],"authorizationTokens": {"http-secure-json": "token1","http-insecure-json": "token2"},"warnings": []}]})json"));
    else if(resp.value().size() == 628)
        REQUIRE(JsonCompare(resp.value(), R"json({"response": [{"provider": {"id": 1,"systemName": "testsystemname1","address": "127.0.0.1","port": 1234,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri1","secure": "not_secure","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"}],"warnings": ["TTL_EXPIRED"]}]})json"));
    else
        REQUIRE(JsonCompare(resp.value(), R"json({"response": []})json"));
}


/////////////////////////////////
// Orchestrator - /orchestration - onlyPreferred
////////////////////////////////

const char* szOrchestatorInputPayload2 =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000,"
    "\"authenticationInfo\": \"Public_Key\""
  "},"
  "\"requestedService\": {"
    "\"serviceDefinitionRequirement\": \"TestService_1\","
    "\"interfaceRequirements\": ["
      "\"HTTP-INSECURE-JSON\""
    "],"
    "\"securityRequirements\": ["
      "\"NOT_SECURE\", \"CERTIFICATE\", \"TOKEN\""
    "],"
    "\"metadataRequirements\": {"
      "\"meta1\": \"test1\""
    "},"
    "\"versionRequirement\": 3,"
    "\"maxVersionRequirement\": 5,"
    "\"minVersionRequirement\": 0"
  "},"
  "\"preferredProviders\": ["
    "{"
      "\"providerCloud\": {"
        "\"operator\": \"AITIA\","
        "\"name\": \"AITIA\""
      "},"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName1\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "},"
    "{"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName2\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "}"
  "],"
  "\"orchestrationFlags\": {"
    "\"onlyPreferred\" : true,"
    "\"overrideStore\" : false,"
    "\"externalServiceRequest\" : false,"
    "\"enableInterCloud\" : false,"
    "\"enableQoS\" : false,"
    "\"matchmaking\" : false,"
    "\"metadataSearch\" : false,"
    "\"triggerInterCloud\" : false,"
    "\"pingProviders\" : false"
  "}"
"}";

    const char *serviceRegistryResp2 =
    "{"
        "\"serviceQueryData\": ["
         "{"
            "\"id\": 4,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 1,\"systemName\": \"testsystemname1\",\"address\": \"127.0.0.1\",\"port\": 1234,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2021-09-11 10:39:08\","
            "\"secure\": \"not_secure\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
         "{"
            "\"id\": 5,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 2,\"systemName\": \"testsystemname2\",\"address\": \"127.0.0.1\",\"port\": 5002,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
            "\"secure\": \"token\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": ["
                "{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"},"
                "{\"id\": 3,\"interfaceName\": \"http-insecure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"}"
                "],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "}"
        "],"
        "\"unfilteredHits\": 2"
    "}";

    const char *authorizerResp2 =
    "{"
          "\"authorizedProviderIdsWithInterfaceIds\": ["
            "{"
             "\"id\":1,"
             "\"idList\":["
                "2"
             "]"
            "},"
            "{"
             "\"id\":2,"
             "\"idList\":["
                "2,"
                "3"
             "]"
            "}"
          "],"
          "\"consumer\": {"
            "\"address\": \"127.0.0.1\","
            "\"authenticationInfo\": \"Public_Key\","
            "\"createdAt\": \"2021-10-11 10:39:08\","
            "\"id\": 1,"
            "\"port\": 8000,"
            "\"systemName\": \"ClientSystemName\","
            "\"updatedAt\": \"2022-03-11 10:39:08\""
          "},"
          "\"serviceDefinitionId\": 1"
    "}";

    const char *authorizerTokenResp2 =
    "{"
      "\"tokenData\": ["
        "{"
          "\"providerAddress\": \"127.0.0.1\","
          "\"providerName\": \"testsystemname2\","
          "\"providerPort\": 5002,"
          "\"tokens\": {"
            "\"http-secure-json\": \"token1\","
            "\"http-insecure-json\": \"token2\""
          "}"
        "}"
      "]"
    "}";


TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_onlyPreferred", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{
                        { "POST:https://127.0.0.1/query", { 200, serviceRegistryResp2} },
                        { "POST:https://127.0.0.1/intracloud/check", { 200, authorizerResp2} },
                        { "POST:https://127.0.0.1/token", { 200, authorizerTokenResp2} }
                      };

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload2) });

    REQUIRE(resp == http::status_code::OK);

    REQUIRE(JsonCompare(resp.value(), R"json({"response":[{"provider": {"id": 2,"systemName": "testsystemname2","address": "127.0.0.1","port": 5002,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri2","secure": "token","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"},{"id": 3,"interfaceName": "http-insecure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"}],"authorizationTokens": {"http-secure-json": "token1","http-insecure-json": "token2"},"warnings": []}]})json"));
}



/////////////////////////////////
// Orchestrator - /orchestration - onlyPreferred - matchmaking
////////////////////////////////

const char* szOrchestatorInputPayload3 =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000,"
    "\"authenticationInfo\": \"Public_Key\""
  "},"
  "\"requestedService\": {"
    "\"serviceDefinitionRequirement\": \"TestService_1\","
    "\"interfaceRequirements\": ["
      "\"HTTP-INSECURE-JSON\""
    "],"
    "\"securityRequirements\": ["
      "\"NOT_SECURE\", \"CERTIFICATE\", \"TOKEN\""
    "],"
    "\"metadataRequirements\": {"
      "\"meta1\": \"test1\""
    "},"
    "\"versionRequirement\": 3,"
    "\"maxVersionRequirement\": 5,"
    "\"minVersionRequirement\": 0"
  "},"
  "\"preferredProviders\": ["
    "{"
      "\"providerCloud\": {"
        "\"operator\": \"AITIA\","
        "\"name\": \"AITIA\""
      "},"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName1\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "},"
    "{"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName2\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "}"
  "],"
  "\"orchestrationFlags\": {"
    "\"onlyPreferred\" : true,"
    "\"overrideStore\" : false,"
    "\"externalServiceRequest\" : false,"
    "\"enableInterCloud\" : false,"
    "\"enableQoS\" : false,"
    "\"matchmaking\" : true,"
    "\"metadataSearch\" : false,"
    "\"triggerInterCloud\" : false,"
    "\"pingProviders\" : false"
  "}"
"}";

    const char *serviceRegistryResp3 =
    "{"
        "\"serviceQueryData\": ["
         "{"
            "\"id\": 4,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 1,\"systemName\": \"testsystemname1\",\"address\": \"127.0.0.1\",\"port\": 1234,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2021-09-11 10:39:08\","
            "\"secure\": \"not_secure\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
         "{"
            "\"id\": 5,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 2,\"systemName\": \"testsystemname2\",\"address\": \"127.0.0.1\",\"port\": 5002,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
            "\"secure\": \"token\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": ["
                "{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"},"
                "{\"id\": 3,\"interfaceName\": \"http-insecure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"}"
                "],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "}"
        "],"
        "\"unfilteredHits\": 2"
    "}";

    const char *authorizerResp3 =
    "{"
          "\"authorizedProviderIdsWithInterfaceIds\": ["
            "{"
             "\"id\":1,"
             "\"idList\":["
                "2"
             "]"
            "},"
            "{"
             "\"id\":2,"
             "\"idList\":["
                "2,"
                "3"
             "]"
            "}"
          "],"
          "\"consumer\": {"
            "\"address\": \"127.0.0.1\","
            "\"authenticationInfo\": \"Public_Key\","
            "\"createdAt\": \"2021-10-11 10:39:08\","
            "\"id\": 1,"
            "\"port\": 8000,"
            "\"systemName\": \"ClientSystemName\","
            "\"updatedAt\": \"2022-03-11 10:39:08\""
          "},"
          "\"serviceDefinitionId\": 1"
    "}";

    const char *authorizerTokenResp3 =
    "{"
      "\"tokenData\": ["
        "{"
          "\"providerAddress\": \"127.0.0.1\","
          "\"providerName\": \"testsystemname2\","
          "\"providerPort\": 5002,"
          "\"tokens\": {"
            "\"http-secure-json\": \"token1\","
            "\"http-insecure-json\": \"token2\""
          "}"
        "}"
      "]"
    "}";


TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_onlyPreferred_matchmaking", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{
                        { "POST:https://127.0.0.1/query", { 200, serviceRegistryResp3} },
                        { "POST:https://127.0.0.1/intracloud/check", { 200, authorizerResp3} },
                        { "POST:https://127.0.0.1/token", { 200, authorizerTokenResp3} }
                      };

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload3) });

    REQUIRE(resp == http::status_code::OK);

    REQUIRE(JsonCompare(resp.value(), R"json({"response":[{"provider": {"id": 2,"systemName": "testsystemname2","address": "127.0.0.1","port": 5002,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri2","secure": "token","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"},{"id": 3,"interfaceName": "http-insecure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"}],"authorizationTokens": {"http-secure-json": "token1","http-insecure-json": "token2"},"warnings": []}]})json"));
}


/////////////////////////////////
// Orchestrator - /orchestration dynamic
////////////////////////////////

const char* szOrchestatorInputPayload4 =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000,"
    "\"authenticationInfo\": \"Public_Key\""
  "},"
  "\"requestedService\": {"
    "\"serviceDefinitionRequirement\": \"TestService_1\","
    "\"interfaceRequirements\": ["
      "\"HTTP-INSECURE-JSON\""
    "],"
    "\"securityRequirements\": ["
      "\"NOT_SECURE\", \"CERTIFICATE\", \"TOKEN\""
    "],"
    "\"metadataRequirements\": {"
      "\"meta1\": \"test1\""
    "},"
    "\"versionRequirement\": 3,"
    "\"maxVersionRequirement\": 5,"
    "\"minVersionRequirement\": 0"
  "},"
  "\"preferredProviders\": ["
    "{"
      "\"providerCloud\": {"
        "\"operator\": \"AITIA\","
        "\"name\": \"AITIA\""
      "},"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName1\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "},"
    "{"
      "\"providerSystem\": {"
        "\"systemName\": \"TestSystemName2\","
        "\"address\": \"127.0.0.1\","
        "\"port\": 5002"
      "}"
    "}"
  "],"
  "\"orchestrationFlags\": {"
    "\"onlyPreferred\" : false,"
    "\"overrideStore\" : false,"
    "\"externalServiceRequest\" : false,"
    "\"enableInterCloud\" : false,"
    "\"enableQoS\" : false,"
    "\"matchmaking\" : false,"
    "\"metadataSearch\" : false,"
    "\"triggerInterCloud\" : false,"
    "\"pingProviders\" : false"
  "}"
"}";

    const char *serviceRegistryResp4 =
    "{"
        "\"serviceQueryData\": ["
         "{"
            "\"id\": 4,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 1,\"systemName\": \"testsystemname1\",\"address\": \"127.0.0.1\",\"port\": 1234,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2021-09-11 10:39:08\","
            "\"secure\": \"not_secure\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
         "{"
            "\"id\": 5,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 2,\"systemName\": \"testsystemname2\",\"address\": \"127.0.0.1\",\"port\": 5002,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
            "\"secure\": \"token\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
            "\"version\": 17,"
            "\"interfaces\": ["
                "{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"},"
                "{\"id\": 3,\"interfaceName\": \"http-insecure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"}"
                "],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "}"
        "],"
        "\"unfilteredHits\": 2"
    "}";

    const char *authorizerResp4 =
    "{"
          "\"authorizedProviderIdsWithInterfaceIds\": ["
            "{"
             "\"id\":1,"
             "\"idList\":["
                "2"
             "]"
            "},"
            "{"
             "\"id\":2,"
             "\"idList\":["
                "2,"
                "3"
             "]"
            "}"
          "],"
          "\"consumer\": {"
            "\"address\": \"127.0.0.1\","
            "\"authenticationInfo\": \"Public_Key\","
            "\"createdAt\": \"2021-10-11 10:39:08\","
            "\"id\": 1,"
            "\"port\": 8000,"
            "\"systemName\": \"ClientSystemName\","
            "\"updatedAt\": \"2022-03-11 10:39:08\""
          "},"
          "\"serviceDefinitionId\": 1"
    "}";

    const char *authorizerTokenResp4 =
    "{"
      "\"tokenData\": ["
        "{"
          "\"providerAddress\": \"127.0.0.1\","
          "\"providerName\": \"testsystemname2\","
          "\"providerPort\": 5002,"
          "\"tokens\": {"
            "\"http-secure-json\": \"token1\","
            "\"http-insecure-json\": \"token2\""
          "}"
        "}"
      "]"
    "}";


TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{
                        { "POST:https://127.0.0.1/query", { 200, serviceRegistryResp4} },
                        { "POST:https://127.0.0.1/intracloud/check", { 200, authorizerResp4} },
                        { "POST:https://127.0.0.1/token", { 200, authorizerTokenResp4} }
                      };

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload4) });

    REQUIRE(resp == http::status_code::OK);

    REQUIRE(JsonCompare(resp.value(), R"json({"response": [{"provider": {"id": 1,"systemName": "testsystemname1","address": "127.0.0.1","port": 1234,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri1","secure": "not_secure","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"}],"warnings": ["TTL_EXPIRED"]},{"provider": {"id": 2,"systemName": "testsystemname2","address": "127.0.0.1","port": 5002,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri2","secure": "token","metadata": {"meta1": "m","meta2": "2","meta3": "test","meta4": "true"},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"},{"id": 3,"interfaceName": "http-insecure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"}],"authorizationTokens": {"http-secure-json": "token1","http-insecure-json": "token2"},"warnings": []}]})json"));
}

/////////////////////////////////
// Orchestrator - /orchestration dynamic - test missing requesterSystem
////////////////////////////////

const char* szOrchestatorInputPayload_missing1 =
"{"
"}";

TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_missing_requesterSystem", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{ { "POST:https://127.0.0.1/query", { 200, ""} }};

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload_missing1) });

    REQUIRE(resp == http::status_code(400));
}

/////////////////////////////////
// Orchestrator - /orchestration dynamic - test missing serviceDefinitionRequirement
////////////////////////////////

const char* szOrchestatorInputPayload_missing2 =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000,"
    "\"authenticationInfo\": \"Public_Key\""
  "}"
"}";

TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_missing_servDefReq", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{ { "POST:https://127.0.0.1/query", { 200, ""} }};

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload_missing2) });

    REQUIRE(resp == http::status_code(400));
}

/////////////////////////////////
// Orchestrator - /orchestration dynamic - empty_SR_response
////////////////////////////////

const char* szOrchestatorInputPayload_empty_SR_response =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000"
  "}"
"\"requestedService\": {"
  "\"serviceDefinitionRequirement\": \"TestService_1\""
"}"
"}";

TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_empty_SR_response", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    auto mc = MockCurl{ { "POST:https://127.0.0.1/query", { 200, ""} }};

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload_empty_SR_response) });


    REQUIRE(resp == http::status_code(200));
}

/////////////////////////////////
// Orchestrator - /orchestration dynamic - minimal
////////////////////////////////

const char* szOrchestatorInputPayload_min =
"{"
  "\"requesterSystem\": {"
    "\"systemName\": \"ClientSystemName\","
    "\"address\": \"127.0.0.1\","
    "\"port\": 8000"
  "},"
  "\"requestedService\": {"
    "\"serviceDefinitionRequirement\": \"TestService_1\""
  "}"
"}";

    const char *serviceRegistryResp_min =
    "{"
        "\"serviceQueryData\": ["
         "{"
            "\"id\": 4,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 1,\"systemName\": \"testsystemname1\",\"address\": \"127.0.0.1\",\"port\": 1234,\"authenticationInfo\": null,"
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2021-09-11 10:39:08\","
            "\"secure\": \"not_secure\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":2,\"meta3\":\"test\",\"meta4\":true},"
            "\"version\": 17,"
            "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
         "{"
            "\"id\": 5,"
            "\"serviceDefinition\":"
                "{\"id\": 1,\"serviceDefinition\": \"testservice_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"provider\":"
                "{\"id\": 2,\"systemName\": \"testsystemname2\",\"address\": \"127.0.0.1\",\"port\": 5002,\"authenticationInfo\": \"fdsa\","
                "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
            "\"secure\": \"token\","
            "\"metadata\": {\"meta1\":\"m\",\"meta2\":2,\"meta3\":\"test\",\"meta4\":false},"
            "\"version\": 17,"
            "\"interfaces\": ["
                "{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"},"
                "{\"id\": 3,\"interfaceName\": \"http-insecure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2021-09-11 10:39:40\"}"
                "],"
            "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\""
          "}"
        "],"
        "\"unfilteredHits\": 2"
    "}";

    const char *authorizerResp_min =
    "{"
          "\"authorizedProviderIdsWithInterfaceIds\": ["
            "{"
             "\"id\":1,"
             "\"idList\":["
                "2"
             "]"
            "},"
            "{"
             "\"id\":2,"
             "\"idList\":["
                "2,"
                "3"
             "]"
            "}"
          "],"
          "\"consumer\": {"
            "\"address\": \"127.0.0.1\","
            "\"authenticationInfo\": \"Public_Key\","
            "\"createdAt\": \"2021-10-11 10:39:08\","
            "\"id\": 1,"
            "\"port\": 8000,"
            "\"systemName\": \"ClientSystemName\","
            "\"updatedAt\": \"2022-03-11 10:39:08\""
          "},"
          "\"serviceDefinitionId\": 1"
    "}";

    const char *authorizerTokenResp_min =
    "{"
      "\"tokenData\": ["
        "{"
          "\"providerAddress\": \"127.0.0.1\","
          "\"providerName\": \"testsystemname2\","
          "\"providerPort\": 5002,"
          "\"tokens\": {"
            "\"http-secure-json\": \"token1\","
            "\"http-insecure-json\": \"token2\""
          "}"
        "}"
      "]"
    "}";

TEST_CASE("Orchestrator: POST /orchestration_intracloud_dynamic_minimal", "[core] [Orchestrator]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

        auto mc = MockCurl{
                            { "POST:https://127.0.0.1/query", { 200, serviceRegistryResp_min} },
                            { "POST:https://127.0.0.1/intracloud/check", { 200, authorizerResp_min} },
                            { "POST:https://127.0.0.1/token", { 200, authorizerTokenResp_min} }
                          };

    // create core system element
    Orchestrator<MockPool, MockCurl> orch{ pool, mc };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "POST", "/orchestration", std::string(szOrchestatorInputPayload_min) });

    REQUIRE(resp == http::status_code(200));

    REQUIRE(JsonCompare(resp.value(), R"json({"response": [{"provider": {"id": 1,"systemName": "testsystemname1","address": "127.0.0.1","port": 1234,"createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri1","secure": "not_secure","metadata": {"meta1": "m","meta2": 2,"meta3": "test","meta4": true},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"}],"warnings": ["TTL_EXPIRED"]},{"provider": {"id": 2,"systemName": "testsystemname2","address": "127.0.0.1","port": 5002,"authenticationInfo": "fdsa","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"service": {"id": 1,"serviceDefinition": "testservice_1","createdAt": "2020-09-11 10:39:08","updatedAt": "2020-09-11 10:39:40"},"serviceUri": "mockuri2","secure": "token","metadata": {"meta1": "m","meta2": 2,"meta3": "test","meta4": false},"version": 17,"interfaces": [{"id": 2,"interfaceName": "http-secure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"},{"id": 3,"interfaceName": "http-insecure-json","createdAt": "2020-09-11 10:39:08","updatedAt": "2021-09-11 10:39:40"}],"authorizationTokens": {"http-secure-json": "token1","http-insecure-json": "token2"},"warnings": []}]})json"));
}

/////////////////////////////////
// Orchestrator - /orchestration/{id}
////////////////////////////////
//todo
