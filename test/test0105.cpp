////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, ServiceRegistry
/// Date:      2021-02-15
/// Author(s): tt
///
/// Description:
/// * This will test the operation of the Service Registry module through URLs
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <string>

#include "core/apps/ServiceRegistry/ServiceRegistry.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"

#include "utils/json.h"

///////////////////////
// Client - Echo
//////////////////////

TEST_CASE("ServiceRegistry: GET /echo", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "Got it!");
}

///////////////////////
// Client - Query
//////////////////////

TEST_CASE("ServiceRegistry: POST /query blank Service Definition", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinitionRequirement\": \" \""
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Service definition requirement is null or blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/query\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /query missing Service Definition", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
        "\"key\" : \"value\""
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Service definition requirement is null or blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/query\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /query", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
        {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, {
        {1, 1, 1, "mockuri1", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 12, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, 1, 1, "mockuri2", "2022-09-11 10:39:08", "certificate", "meta1=m,meta2=2", 13, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {3, 1, 1, "mockuri2", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 14, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {4, 1, 1, "mockuri2", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 16, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {5, 1, 1, "mockuri2", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2,meta3=test,meta4=true", 17, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {6, 1, 1, "mockuri2", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 18, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, {
        {1, "testintf_1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, "http-secure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, {
        {1, 1, 1, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, 2, 1, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {3, 2, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {4, 3, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {5, 4, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {6, 5, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {7, 6, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinitionRequirement\": \"TestServ ic e\","
      "\"interfaceRequirements\": ["
            "\"HtTp-secure-json\""
        "],"
     "\"securityRequirements\": ["
       "\"NoT_SeCURE\""
        "]"
     "\"versionRequirement\": 17,"
     "\"minVersionRequirement\" : 17,"
     "\"maxVersionRequirement\" : 18,"

     "\"metadataRequirements \": {"
        "\"meta1\": \"m\","
        "\"meta2\": 2,"
        "\"meta3\": \"test\","
        "\"meta4\": true"
     "},"

     "\"pingProviders\": false"

     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", std::string(payload) });

    const char *expResp =
    "{"
        "\"serviceQueryData\": ["
        "{\"id\": 5,\"serviceDefinition\":"
        "{\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\",\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\","
        "\"secure\": \"not_secure\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\",\"meta4\":\"true\"},"
        "\"version\": 17,\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],"
        "\"unfilteredHits\": 6"
    "}";

    REQUIRE(resp == http::status_code::OK);
    //printf("resp: %s\n", resp.value().c_str());

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

///////////////////////
// Client - Register
//////////////////////

TEST_CASE("ServiceRegistry: POST /register", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1000, "test", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, { {1000, "test", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, { {1000, "test", "intf", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1000, 2000, 3000, "mockuri", "2022-09-11 10:39:08", "NOT_SECURE", "{\"meta1\":\"m\"}", 1.0, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, { {1000, 2000, 3000, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestServ ic e\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"Certificate\","
      "\"metadata\":"
      "{"
        "\"additionalProp1\": \"meta1\","
        "\"additionalProp2\": \"meta2\","
        "\"additionalProp3\": \"meta3\""
      "},"
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"HTTP-SECURE-JSON\","
        "\"HTTP-INSECUrE-jSon\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    const char *expResp =
    "{"
        "\"id\" : 1001,"
        "\"serviceDefinition\": {"
            "\"id\": 1001,"
            "\"serviceDefinition\": \"testservice\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"provider\": {"
            "\"id\": 1001,"
            "\"systemName\": \"testprovidersystemname18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"serviceUri\": \"/akarmi/url18\","
        "\"endOfValidity\": \"2022-08-05 12:00:00\","
        "\"secure\": \"certificate\","
        "\"metadata\": {"
            "\"additionalProp1\": \"meta1\","
            "\"additionalProp2\": \"meta2\","
            "\"additionalProp3\": \"meta3\"},"
        "\"version\": 2,"
        "\"interfaces\": ["
            "{\"id\": 1,"
                "\"interfaceName\": \"http-secure-json\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"},"
            "{\"id\": 2,"
                "\"interfaceName\": \"http-insecure-json\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"}],"
        "\"createdAt\": \"\","
        "\"updatedAt\": \"\""
    "}";

    REQUIRE(resp == http::status_code::OK);

    //printf("resp: %s\n", resp.value().c_str());

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /register (missing metadata)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1000, "test", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, { {1000, "test", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, { {1000, "test", "intf", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1000, 2000, 3000, "mockuri", "2022-09-11 10:39:08", "NOT_SECURE", "{\"meta1\":\"m\"}", 1.0, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, { {1000, 2000, 3000, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"CERTIFICATE\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"http-secure-json\","
        "\"http-insecure-json\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    const char *expResp =
    "{"
        "\"id\" : 1001,"
        "\"serviceDefinition\": {"
            "\"id\": 1001,"
            "\"serviceDefinition\": \"testservice\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"provider\": {"
            "\"id\": 1001,"
            "\"systemName\": \"testprovidersystemname18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"serviceUri\": \"/akarmi/url18\","
        "\"endOfValidity\": \"2022-08-05 12:00:00\","
        "\"secure\": \"certificate\","
        "\"metadata\": {},"
        "\"version\": 2,"
        "\"interfaces\": ["
            "{\"id\": 1,"
                "\"interfaceName\": \"http-secure-json\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"},"
            "{\"id\": 2,"
                "\"interfaceName\": \"http-insecure-json\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"}],"
        "\"createdAt\": \"\","
        "\"updatedAt\": \"\""
    "}";

    REQUIRE(resp == http::status_code::OK);

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (bad Json contents)", "[core] [ServiceRegistryUUT]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
    "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Bad Json\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /register (Service definition is NULL)", "[core] [ServiceRegistryUUT]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": null,"
        "\"k\" : \"v\""
    "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Service definition is null or blank.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /register (Service definition is empty)", "[core] [ServiceRegistryUUT]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"\""
    "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Service definition is null or blank.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /register (Invalid EndOfValidity)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"end of validity\","
      "\"secure\": \"CERTIFICATE\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"http-secure-json\","
        "\"http-insecure-json\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"End of validity is specified in the wrong format. Please provide UTC time using YYYY-MM-DD hh:mm:ss pattern.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (Invalid SecurityType)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"invalid\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"http-secure-json\","
        "\"http-insecure-json\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Security type is not valid.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (Invalid ServiceInterface)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"certificate\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"hTTp-securfde-json\","
        "\"http-inv-json\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Specified interface name is not valid: hTTp-securfde-json\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (Interfaces list empty)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"certificate\","
      "\"version\": 2,"
      "\"interfaces\": ["
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Interfaces list is null or empty.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (Interfaces list null)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"certificate\","
      "\"version\": 2"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Interfaces list is not specified.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (Secure withouth auth info)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"CERTIFICATE\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"http-secure-json\","
        "\"http-insecure-json\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    //printf("resp: %s\n", resp.value().c_str());

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Security type is in conflict with the availability of the authentication info.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /register (Insecure withouth auth info)", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1000, "test", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, { {1000, "test", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, { {1000, "test", "intf", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1000, 2000, 3000, "mockuri", "2022-09-11 10:39:08", "NOT_SECURE", "{\"meta1\":\"m\"}", 1.0, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, { {1000, 2000, 3000, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
      "\"serviceDefinition\": \"TestService\","
      "\"providerSystem\":"
        "{"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"\""
        "},"
      "\"serviceUri\": \"/akarmi/url18\","
      "\"endOfValidity\": \"2022-08-05 12:00:00\","
      "\"secure\": \"not_secure\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"http-secure-json\","
        "\"http-insecure-json\""
      "]"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    REQUIRE(resp == http::status_code(200));

    //const char *expResp = "{\"errorMessage\": \"Security type is in conflict with the availability of the authentication info.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    //const std::string sExpResp(expResp);
    //REQUIRE(JsonCompare(resp.value(), sExpResp));
}


///////////////////////
// Client - Unregister
//////////////////////

TEST_CASE("ServiceRegistry: DELETE /unregister", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1000, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, { {1000, "sys", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, { {1000, "test", "intf", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1000, 1000, 1000, "mockuri", "2022-09-11 10:39:08", "NOT_SECURE", "{\"meta1\":\"m\"}", 1.0, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, { {1000, 1000, 1000, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=127.0.0.2&port=1234", "" });

    REQUIRE(resp == http::status_code(200));

    //const char *expResp = "{\"errorMessage\": \"Security type is in conflict with the availability of the authentication info.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/register\"}";

    //const std::string sExpResp(expResp);
    //REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister missing serviceDefinition parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?system_name=sys&address=127.0.0.2&port=1234", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: DELETE /unregister missing systemName parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&address=127.0.0.2&port=1234", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: DELETE /unregister missing address parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&port=1234", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: DELETE /unregister missing port parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=127.0.0.2", "" });

    REQUIRE(resp == http::status_code(400));
}


TEST_CASE("ServiceRegistry: DELETE /unregister empty serviceDefinition parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=&system_name=sys&address=127.0.0.2&port=1234", "" });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Service definition is blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister empty systemName parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=&address=127.0.0.2&port=1234", "" });

    REQUIRE(resp == http::status_code(400));
    const char *expResp = "{\"errorMessage\": \"Name of the provider system is blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister empty address parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=&port=1234", "" });

    REQUIRE(resp == http::status_code(400));
    const char *expResp = "{\"errorMessage\": \"Address of the provider system is blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister empty port parameter", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=127.0.0.2&port=", "" });

    REQUIRE(resp == http::status_code(400));
    const char *expResp = "{\"errorMessage\": \"Port of the provider system is blank\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister bad port (-1)", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=127.0.0.2&port=-1", "" });

    REQUIRE(resp == http::status_code(400));
    const char *expResp = "{\"errorMessage\": \"Port must be between 0 and 65535.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: DELETE /unregister bad port (6600)", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?service_definition=testservice&system_name=sys&address=127.0.0.2&port=66000", "" });

    REQUIRE(resp == http::status_code(400));
    const char *expResp = "{\"errorMessage\": \"Port must be between 0 and 65535.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/unregister\"}";
    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}
