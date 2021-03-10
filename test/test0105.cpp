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

#include "core/ServiceRegistry/ServiceRegistry.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"

#include "utils/json.h"

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

TEST_CASE("ServiceRegistry: POST /register", "[core] [ServiceRegistry]") {

    printf("\n###TEST###\n");
    printf("ServiceRegistry: POST /register\n");
    printf("##########\n\n");

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
      "\"secure\": \"NOT_SECURE\","
      "\"metadata\":"
      "{"
        "\"additionalProp1\": \"meta1\","
        "\"additionalProp2\": \"meta2\","
        "\"additionalProp3\": \"meta3\""
      "},"
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"interface1\","
        "\"interface2\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    const char *expResp =
    "{"
        "\"id\" : 1001,"
        "\"serviceDefinition\": {"
            "\"id\": 1001,"
            "\"serviceDefinition\": \"TestService\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"provider\": {"
            "\"id\": 1001,"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"serviceUri\": \"/akarmi/url18\","
        "\"endOfValidity\": \"2022-08-05 12:00:00\","
        "\"secure\": \"NOT_SECURE\","
        "\"metadata\": {"
            "\"additionalProp1\": \"meta1\","
            "\"additionalProp2\": \"meta2\","
            "\"additionalProp3\": \"meta3\"},"
        "\"version\": 2,"
        "\"interfaces\": ["
            "{\"id\": 1,"
                "\"interfaceName\": \"interface1\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"},"
            "{\"id\": 2,"
                "\"interfaceName\": \"interface2\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"}],"
        "\"createdAt\": \"\","
        "\"updatedAt\": \"\""
    "}";

    REQUIRE(resp == http::status_code::OK);

    printf("resp: %s\n", resp.value().c_str());

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: POST /register (missing metadata)", "[core] [ServiceRegistry]") {

    printf("\n###TEST###\n");
    printf("ServiceRegistry: POST /register (missing metadata)\n");
    printf("##########\n\n");

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
      "\"secure\": \"NOT_SECURE\","
      "\"version\": 2,"
      "\"interfaces\": ["
        "\"interface1\","
        "\"interface2\""
      "]"
     "}";

    //printf("%s\n", payload);

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    const char *expResp =
    "{"
        "\"id\" : 1001,"
        "\"serviceDefinition\": {"
            "\"id\": 1001,"
            "\"serviceDefinition\": \"TestService\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"provider\": {"
            "\"id\": 1001,"
            "\"systemName\": \"TestProviderSystemName18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 2345,"
            "\"authenticationInfo\": \"authInfo\","
            "\"createdAt\": \"\","
            "\"updatedAt\": \"\"},"
        "\"serviceUri\": \"/akarmi/url18\","
        "\"endOfValidity\": \"2022-08-05 12:00:00\","
        "\"secure\": \"NOT_SECURE\","
        "\"metadata\": {},"
        "\"version\": 2,"
        "\"interfaces\": ["
            "{\"id\": 1,"
                "\"interfaceName\": \"interface1\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"},"
            "{\"id\": 2,"
                "\"interfaceName\": \"interface2\","
                "\"createdAt\": \"\","
                "\"updatedAt\": \"\"}],"
        "\"createdAt\": \"\","
        "\"updatedAt\": \"\""
    "}";

    REQUIRE(resp == http::status_code::OK);

    printf("resp: %s\n", resp.value().c_str());

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistryUUT: POST /register (missing Json contents)", "[core] [ServiceRegistryUUT]") {

    printf("\n###TEST###\n");
    printf("ServiceRegistry: POST /register (missing Json contents)\n");
    printf("##########\n\n");

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
            "\"authenticationInfo\": \"authInfo\""
        "},"
    "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", std::string(payload) });

    REQUIRE(resp == http::status_code::NotAcceptable);
}


TEST_CASE("ServiceRegistryUUT: POST /register (bad Json contents)", "[core] [ServiceRegistryUUT]") {

    printf("\n###TEST###\n");
    printf("ServiceRegistry: POST /register (bad Json contents)\n");
    printf("##########\n\n");

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

    REQUIRE(resp == http::status_code::NotAcceptable);
}
