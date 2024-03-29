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

/////////////////////////////////
// Mgmt - POST /mgmt
////////////////////////////////

TEST_CASE("ServiceRegistry: POST /mgmt ok", "[core] [ServiceRegistry]") {
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

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/mgmt", std::string(payload) });

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
            "\"additionalprop1\":\"meta1\","
            "\"additionalprop2\":\"meta2\","
            "\"additionalprop3\":\"meta3\"},"
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

/////////////////////////////////
// Mgmt - PATCH /mgmt/systems/{id}
////////////////////////////////

TEST_CASE("ServiceRegistry: PATCH /mgmt/systems/{id} ok1", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"systemName\" : \"sys1\"}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"sys1\",\"address\": \"10.1.2.3\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PATCH /mgmt/systems/{id} ok2", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"address\": \"10.1.2.4\"}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"testprovidersystemname18\",\"address\": \"10.1.2.4\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PATCH /mgmt/systems/{id} ok3", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"port\": 5678}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"testprovidersystemname18\",\"address\": \"10.1.2.3\",\"port\": 5678,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PATCH /mgmt/systems/{id} ok4", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"authenticationInfo\" : \"qqq\"}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"testprovidersystemname18\",\"address\": \"10.1.2.3\",\"port\": 1234,\"authenticationInfo\": \"qqq\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

/////////////////////////////////
// Mgmt - PATCH /mgmt/services/{id}
////////////////////////////////

TEST_CASE("ServiceRegistry: PATCH /mgmt/services/{id} ok1", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/services/1", "{ \"serviceDefinition\" : \"newServDef\"}" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"serviceDefinition\": \"newservdef\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PATCH /mgmt/services/{id} ok2", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/services/1", "{ }" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PATCH /mgmt/services/{id} invalid", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/services/3", "{ }" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(400));
}

/////////////////////////////////
// Mgmt - PUT /mgmt/systems/{id}
////////////////////////////////

TEST_CASE("ServiceRegistry: PUT /mgmt/systems/{id} ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"systemName\" : \"sys1\", \"address\" : \"192.168.1.2\", \"port\" : 5678}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"sys1\",\"address\": \"192.168.1.2\",\"port\": 5678,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PUT /mgmt/systems/{id} authInfo", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"systemName\" : \"sys1\", \"address\" : \"192.168.1.2\", \"port\" : 5678, \"authenticationInfo\" : \"qqq\"}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/systems/1", payload });

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"systemName\": \"sys1\",\"address\": \"192.168.1.2\",\"port\": 5678,\"authenticationInfo\": \"qqq\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: PUT /mgmt/systems/{id} invalid", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"systemName\" : \"sys1\", \"address\" : \"192.168.1.2\", \"port\" : 5678}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/systems/4", payload });

    REQUIRE(resp == http::status_code(400));
}

/////////////////////////////////
// Mgmt - PUT /mgmt/services/{id}
////////////////////////////////

TEST_CASE("ServiceRegistry: PUT /mgmt/services/{id} ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/services/1", "{ \"serviceDefinition\" : \"newServDef\"}" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 1,\"serviceDefinition\": \"newservdef\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

/////////////////////////////////
// Mgmt - PUT /mgmt/services/{id}
////////////////////////////////

TEST_CASE("ServiceRegistry: PUT /mgmt/services/{id} invalid", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/services/3", "{ \"serviceDefinition\" : \"newServDef\"}" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(400));
}

/////////////////////////////////
// Mgmt - POST /mgmt/systems
////////////////////////////////

TEST_CASE("ServiceRegistry: POST /mgmt/systems ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    std::string payload = "{ \"systemName\" : \"sys1\", \"address\" : \"192.168.1.2\", \"port\" : 5678}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/systems", payload });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 2,\"systemName\": \"sys1\",\"address\": \"192.168.1.2\",\"port\": 5678,\"authenticationInfo\": \"\",\"createdAt\": \"\",\"updatedAt\": \"\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

/////////////////////////////////
// Mgmt - POST /mgmt/services
////////////////////////////////

TEST_CASE("ServiceRegistry: POST /mgmt/services ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
      {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/services", "{ \"serviceDefinition\" : \"thisIsTheServiceDefinition\"}" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp = "{\"id\": 2,\"serviceDefinition\": \"thisistheservicedefinition\",\"createdAt\": \"\",\"updatedAt\": \"\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

///////////////////////////
// Mgmt - GET mgmt
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, {
        {1, "http-secure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, "http-insecure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" },
    {
        {1, 1, 1, "/url/url18", "2022-09-11 10:39:08", "not_secure", "key1=value1,key2=value2", 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, {
        {1, 1, 1, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, 1, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt", "" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
  "\"data\": ["
    "{"
      "\"id\": 1,"
      "\"serviceDefinition\": {"
        "\"id\": 1,"
        "\"serviceDefinition\": \"testservice\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
      "},"
      "\"provider\": {"
        "\"id\": 1,"
        "\"systemName\": \"testprovidersystemname18\","
        "\"address\": \"10.1.2.3\","
        "\"port\": 1234,"
        "\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
      "},"
      "\"serviceUri\": \"/url/url18\","
      "\"endOfValidity\": \"2022-09-11 10:39:08\","
      "\"secure\": \"not_secure\","
      "\"metadata\": {"
        "\"key1\": \"value1\","
        "\"key2\": \"value2\""
      "},"
      "\"version\": 2,"
      "\"interfaces\": ["
        "{"
          "\"id\": 1,"
          "\"interfaceName\": \"http-secure-json\","
          "\"createdAt\": \"2020-09-11 10:39:08\","
          "\"updatedAt\": \"2020-09-11 10:39:40\""
        "},"
        "{"
          "\"id\": 2,"
          "\"interfaceName\": \"http-insecure-json\","
          "\"createdAt\": \"2020-09-11 10:39:08\","
          "\"updatedAt\": \"2020-09-11 10:39:40\""
        "}"
      "],"
      "\"createdAt\": \"2020-09-11 10:39:08\","
      "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}"
  "],"
  "\"count\": 1"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

///////////////////////////
// Mgmt - GET mgmt/grouped
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/grouped ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, {
        {1, "http-secure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, "http-insecure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1, 1, 1, "/url/url18", "2022-09-11 10:39:08", "not_secure", "key1=value1,key2=value2", 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, {
        {1, 1, 1, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, 1, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/grouped", "" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
  "\"autoCompleteData\": {"
    "\"interfaceList\": ["
      "{"
        "\"id\": 1,"
        "\"value\": \"http-secure-json\""
      "},"
      "{"
        "\"id\": 2,"
        "\"value\": \"http-insecure-json\""
      "}"
    "],"
    "\"serviceList\": ["
      "{"
        "\"id\": 1,"
        "\"value\": \"testservice\""
      "}"
    "],"
    "\"systemList\": ["
      "{"
        "\"id\": 1,"
        "\"systemName\": \"testprovidersystemname18\","
        "\"address\": \"10.1.2.3\","
        "\"port\": 1234,"
        "\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
      "}"
    "]"
  "},"
  "\"servicesGroupedByServiceDefinition\": ["
    "{"
      "\"serviceDefinitionId\": 1,"
      "\"serviceDefinition\": \"testservice\","
      "\"providerServices\": ["
        "{"
          "\"id\": 1,"
          "\"serviceDefinition\": {"
            "\"id\": 1,"
            "\"serviceDefinition\": \"testservice\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
          "\"provider\": {"
            "\"id\": 1,"
            "\"systemName\": \"testprovidersystemname18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 1234,"
            "\"authenticationInfo\": \"fdsa\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
          "\"serviceUri\": \"/url/url18\","
          "\"endOfValidity\": \"2022-09-11 10:39:08\","
          "\"secure\": \"not_secure\","
          "\"metadata\": {"
            "\"key1\": \"value1\","
            "\"key2\": \"value2\""
          "},"
          "\"version\": 2,"
          "\"interfaces\": ["
            "{"
              "\"id\": 1,"
              "\"interfaceName\": \"http-secure-json\","
              "\"createdAt\": \"2020-09-11 10:39:08\","
              "\"updatedAt\": \"2020-09-11 10:39:40\""
            "},"
            "{"
              "\"id\": 2,"
              "\"interfaceName\": \"http-insecure-json\","
              "\"createdAt\": \"2020-09-11 10:39:08\","
              "\"updatedAt\": \"2020-09-11 10:39:40\""
            "}"
          "],"
          "\"createdAt\": \"2020-09-11 10:39:08\","
          "\"updatedAt\": \"2020-09-11 10:39:40\""
        "}"
      "]"
    "}"
  "],"
  "\"servicesGroupedBySystems\": ["
    "{"
      "\"systemId\": 1,"
      "\"systemName\": \"testprovidersystemname18\","
      "\"address\": \"10.1.2.3\","
      "\"port\": 1234,"
      "\"services\": ["
        "{"
          "\"id\": 1,"
          "\"serviceDefinition\": {"
            "\"id\": 1,"
            "\"serviceDefinition\": \"testservice\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
          "\"provider\": {"
            "\"id\": 1,"
            "\"systemName\": \"testprovidersystemname18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 1234,"
            "\"authenticationInfo\": \"fdsa\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\""
          "},"
          "\"serviceUri\": \"/url/url18\","
          "\"endOfValidity\": \"2022-09-11 10:39:08\","
          "\"secure\": \"not_secure\","
          "\"metadata\": {"
            "\"key1\": \"value1\","
            "\"key2\": \"value2\""
          "},"
          "\"version\": 2,"
          "\"interfaces\": ["
            "{"
              "\"id\": 1,"
              "\"interfaceName\": \"http-secure-json\","
              "\"createdAt\": \"2020-09-11 10:39:08\","
              "\"updatedAt\": \"2020-09-11 10:39:40\""
            "},"
            "{"
              "\"id\": 2,"
              "\"interfaceName\": \"http-insecure-json\","
              "\"createdAt\": \"2020-09-11 10:39:08\","
              "\"updatedAt\": \"2020-09-11 10:39:40\""
            "}"
          "],"
          "\"createdAt\": \"2020-09-11 10:39:08\","
          "\"updatedAt\": \"2020-09-11 10:39:40\""
        "}"
      "]"
    "}"
  "]"
"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

///////////////////////////
// Mgmt - GET {id}
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" },
    { {1, 1, 1, "/url/url18", "2022-09-11 10:39:08", "not_secure", "key1=value1,key2=value2", 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/3", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: GET /mgmt/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" },
    { {1, 1, 1, "/url/url18", "2022-09-11 10:39:08", "not_secure", "key1=value1,key2=value2", 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/-33", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: GET /mgmt/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });
    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, { {1, "testprovidersystemname18", "10.1.2.3", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    mdb.table("service_interface", true, { "id", "interface_name", "created_at", "updated_at" }, {
        {1, "http-secure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, "http-insecure-json", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, { {1, 1, 1, "/url/url18", "2022-09-11 10:39:08", "not_secure", "key1=value1,key2=value2", 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    mdb.table("service_registry_interface_connection", true, { "id", "service_registry_id", "interface_id", "created_at", "updated_at" }, {
        {1, 1, 1, "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, 1, 2, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/1", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"id\" : 1,"
        "\"serviceDefinition\": {"
            "\"id\": 1,"
            "\"serviceDefinition\": \"testservice\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"provider\": {"
            "\"id\": 1,"
            "\"systemName\": \"testprovidersystemname18\","
            "\"address\": \"10.1.2.3\","
            "\"port\": 1234,"
            "\"authenticationInfo\": \"fdsa\","
            "\"createdAt\": \"2020-09-11 10:39:08\","
            "\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"serviceUri\": \"/url/url18\","
        "\"endOfValidity\": \"2022-09-11 10:39:08\","
        "\"secure\": \"not_secure\","
        "\"metadata\": {"
            "\"key1\":\"value1\","
            "\"key2\":\"value2\"},"
        "\"version\": 2,"
        "\"interfaces\": ["
            "{\"id\": 1,"
                "\"interfaceName\": \"http-secure-json\","
                "\"createdAt\": \"2020-09-11 10:39:08\","
                "\"updatedAt\": \"2020-09-11 10:39:40\"},"
            "{\"id\": 2,"
                "\"interfaceName\": \"http-insecure-json\","
                "\"createdAt\": \"2020-09-11 10:39:08\","
                "\"updatedAt\": \"2020-09-11 10:39:40\"}],"
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}


///////////////////////////
// Mgmt - GET serviceDef
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/serviceDef ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

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

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/servicedef/testService", "" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 1,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\",\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\"},\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"not_secure\","
        "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},\"version\": 12,\"interfaces\": [{\"id\": 1,\"interfaceName\": \"testintf_1\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\"},{\"id\": 2,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"certificate\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},"
        "\"version\": 13,\"interfaces\": [{\"id\": 1,\"interfaceName\": \"testintf_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"},{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},{\"id\": 3,"
        "\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\",\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\":"
        "\"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\":"
        "\"not_secure\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},\"version\": 14,\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"},{\"id\": 4,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\":"
        "\"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\",\"address\": \"127.0.0.2\","
        "\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"serviceUri\":"
        "\"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"not_secure\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},\"version\": 16,"
        "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},{\"id\": 5,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\":"
        "\"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"not_secure\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\",\"meta3\":\"test\","
        "\"meta4\":\"true\"},\"version\": 17,\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},{\"id\": 6,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\":"
        "\"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"not_secure\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},\"version\": 18,"
        "\"interfaces\": [{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],"
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":6"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/serviceDef invalid", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
        {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/servicedef/testService1234", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: GET /mgmt/serviceDef page limit", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

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

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/servicedef/testService?item_per_page=2", "" });

    //printf("resp:\n%s\n",resp.value().c_str());

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 1,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\",\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\"},\"serviceUri\": \"mockuri1\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"not_secure\","
        "\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},\"version\": 12,\"interfaces\": [{\"id\": 1,\"interfaceName\": \"testintf_1\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\"},{\"id\": 2,\"serviceDefinition\": {\"id\": 1,\"serviceDefinition\": \"testservice\","
        "\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},\"provider\": {\"id\": 1,\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\",\"port\": 1234,\"authenticationInfo\": \"fdsa\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "\"serviceUri\": \"mockuri2\",\"endOfValidity\": \"2022-09-11 10:39:08\",\"secure\": \"certificate\",\"metadata\": {\"meta1\":\"m\",\"meta2\":\"2\"},"
        "\"version\": 13,\"interfaces\": [{\"id\": 1,\"interfaceName\": \"testintf_1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"},{\"id\": 2,\"interfaceName\": \"http-secure-json\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\":"
        "\"2020-09-11 10:39:40\"}],\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":2"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

///////////////////////////
// Mgmt - GET services
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/services ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {1,  "testservice1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
         {2,  "testservice2", "2020-09-11 10:39:09", "2020-09-11 10:39:40"},
         {3,  "testservice3", "2020-09-11 10:39:10", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 1, \"systemName\": \"testservice1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 2,\"systemName\": \"testservice2\",\"createdAt\": \"2020-09-11 10:39:09\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 3,\"systemName\": \"testservice3\",\"createdAt\": \"2020-09-11 10:39:10\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":3"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/services page", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {1,  "testservice1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
         {2,  "testservice2", "2020-09-11 10:39:09", "2020-09-11 10:39:40"},
         {3,  "testservice3", "2020-09-11 10:39:10", "2020-09-11 10:39:40"},
         {4,  "testservice4", "2020-09-11 10:39:11", "2020-09-11 10:39:40"},
         {5,  "testservice5", "2020-09-11 10:39:12", "2020-09-11 10:39:40"},
         {6,  "testservice6", "2020-09-11 10:39:13", "2020-09-11 10:39:40"},
         {7,  "testservice7", "2020-09-11 10:39:20", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services?page=5", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 6,\"systemName\": \"testservice6\",\"createdAt\": \"2020-09-11 10:39:13\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 7,\"systemName\": \"testservice7\",\"createdAt\": \"2020-09-11 10:39:20\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":2"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/services item_per_page", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {1,  "testservice1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
         {2,  "testservice2", "2020-09-11 10:39:09", "2020-09-11 10:39:40"},
         {3,  "testservice3", "2020-09-11 10:39:10", "2020-09-11 10:39:40"},
         {4,  "testservice4", "2020-09-11 10:39:11", "2020-09-11 10:39:40"},
         {5,  "testservice5", "2020-09-11 10:39:12", "2020-09-11 10:39:40"},
         {6,  "testservice6", "2020-09-11 10:39:13", "2020-09-11 10:39:40"},
         {7,  "testservice7", "2020-09-11 10:39:20", "2020-09-11 10:39:40"},
         {8,  "testservice8", "2020-09-11 10:39:30", "2020-09-11 10:39:40"},
         {9,  "testservice9", "2020-09-11 10:39:21", "2020-09-11 10:39:40"},
         {10, "testservice10", "2020-09-11 10:39:22", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services?page=2&item_per_page=2", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 3,\"systemName\": \"testservice3\",\"createdAt\": \"2020-09-11 10:39:10\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 4,\"systemName\": \"testservice4\",\"createdAt\": \"2020-09-11 10:39:11\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":2"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));

}

TEST_CASE("ServiceRegistry: GET /mgmt/services sort_field", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {2,  "testservice2", "2020-09-11 10:39:09", "2020-09-11 10:39:40"},
         {3,  "testservice3", "2020-09-11 10:39:20", "2020-09-11 10:39:40"},
         {4,  "testservice4", "2020-09-11 10:39:11", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services?sort_field=createdAt", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 2,\"systemName\": \"testservice2\",\"createdAt\": \"2020-09-11 10:39:09\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 4,\"systemName\": \"testservice4\",\"createdAt\": \"2020-09-11 10:39:11\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 3,\"systemName\": \"testservice3\",\"createdAt\": \"2020-09-11 10:39:20\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":3"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/services sort_field invalid", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {1,  "testservice1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services?sort_field=createdAtInvalid", "" });

    REQUIRE(resp == http::status_code(400));
}

TEST_CASE("ServiceRegistry: GET /mgmt/services direction DESC", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
         {1,  "testservice1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
         {2,  "testservice2", "2020-09-11 10:39:09", "2020-09-11 10:39:40"},
         {3,  "testservice3", "2020-09-11 10:39:10", "2020-09-11 10:39:40"},
         {8,  "testservice8", "2020-09-11 10:39:30", "2020-09-11 10:39:40"},
         {9,  "testservice9", "2020-09-11 10:39:21", "2020-09-11 10:39:40"},
         {10, "testservice10", "2020-09-11 10:39:22", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services?sort_field=createdAt&direction=DESC", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
        "{\"id\": 8,\"systemName\": \"testservice8\",\"createdAt\": \"2020-09-11 10:39:30\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 10,\"systemName\": \"testservice10\",\"createdAt\": \"2020-09-11 10:39:22\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 9,\"systemName\": \"testservice9\",\"createdAt\": \"2020-09-11 10:39:21\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 3,\"systemName\": \"testservice3\",\"createdAt\": \"2020-09-11 10:39:10\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 2,\"systemName\": \"testservice2\",\"createdAt\": \"2020-09-11 10:39:09\",\"updatedAt\": \"2020-09-11 10:39:40\"},"
        "{\"id\": 1,\"systemName\": \"testservice1\",\"createdAt\": \"2020-09-11 10:39:08\",\"updatedAt\": \"2020-09-11 10:39:40\"}"
        "],\"count\":6"
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

///////////////////////////
// Mgmt - GET services/{id}
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/services/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services/1", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"id\": 1,"
        "\"serviceDefinition\": \"testservice\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/services/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, { {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"} });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/services/41", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Mgmt - GET systems
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/systems ok", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname1", "127.0.0.1", 1234, "fdsa1", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {2, "testsystemname2", "127.0.0.2", 1235, "fdsa2", "2020-09-11 10:39:08", "2020-09-11 10:39:40"},
        {3, "testsystemname3", "127.0.0.3", 1236, "fdsa3", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/systems", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"data\": ["
            "{"
                "\"id\": 1,"
                "\"systemName\": \"testsystemname1\","
                "\"address\": \"127.0.0.1\","
                "\"port\": 1234,"
                "\"authenticationInfo\": \"fdsa1\","
                "\"createdAt\": \"2020-09-11 10:39:08\","
                "\"updatedAt\": \"2020-09-11 10:39:40\""
            "},"
            "{"
                "\"id\": 2,"
                "\"systemName\": \"testsystemname2\","
                "\"address\": \"127.0.0.2\","
                "\"port\": 1235,"
                "\"authenticationInfo\": \"fdsa2\","
                "\"createdAt\": \"2020-09-11 10:39:08\","
                "\"updatedAt\": \"2020-09-11 10:39:40\""
            "},"
            "{"
                "\"id\": 3,"
                "\"systemName\": \"testsystemname3\","
                "\"address\": \"127.0.0.3\","
                "\"port\": 1236,"
                "\"authenticationInfo\": \"fdsa3\","
                "\"createdAt\": \"2020-09-11 10:39:08\","
                "\"updatedAt\": \"2020-09-11 10:39:40\""
            "}"
        "],"
        "\"count\": 3"
    "}";

    //printf("resp:\n%s\n",resp.value().c_str());

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/systems empty db", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {

    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/systems", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Mgmt - GET systems/{id}
//////////////////////////

TEST_CASE("ServiceRegistry: GET /mgmt/systems/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/systems/1", "" });

    REQUIRE(resp == http::status_code(200));

    const char *expResp =
    "{"
        "\"id\": 1,"
        "\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\","
        "\"port\": 1234,"
        "\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/systems/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/systems/23", "" });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"System with id 23 not found.\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/mgmt/systems/{id}\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /mgmt/systems/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/systems/-23", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Mgmt - DELETE systems/{id}
//////////////////////////

TEST_CASE("ServiceRegistry: DELETE /mgmt/systems/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/systems/2", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/systems/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/systems/1", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/systems/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/systems/-1", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Mgmt - DELETE services/{id}
//////////////////////////

TEST_CASE("ServiceRegistry: DELETE /mgmt/services/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
        {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/services/11", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/services/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
        {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/services/1", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/services/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_definition", true, { "id", "service_definition", "created_at", "updated_at" }, {
        {1, "testservice", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/services/-1", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Mgmt - DELETE {id}
//////////////////////////

TEST_CASE("ServiceRegistry: DELETE /mgmt/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, {
        {1, 1, 1, "mockuri1", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 12, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/3", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, {
        {1, 1, 1, "mockuri1", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 12, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/1", "" });

    REQUIRE(resp == http::status_code(200));
}

TEST_CASE("ServiceRegistry: DELETE /mgmt/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    mdb.table("service_registry", true, { "id", "service_id", "system_id", "service_uri", "end_of_validity", "secure", "metadata", "version", "created_at", "updated_at" }, {
        {1, 1, 1, "mockuri1", "2022-09-11 10:39:08", "not_secure", "meta1=m,meta2=2", 12, "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/-1", "" });

    REQUIRE(resp == http::status_code(400));
}

///////////////////////////
// Private - Query/System
//////////////////////////

TEST_CASE("ServiceRegistry: POST /query/system missing systemName", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"\","
          "\"address\": \"127.0.0.2\","
          "\"port\": 1234"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"parameter null or empty\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /query/system missing address", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"test\","
          "\"port\": 1234"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"parameter null or empty\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}


TEST_CASE("ServiceRegistry: POST /query/system missing port", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"test\","
          "\"address\": \"127.0.0.2\""
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"parameter null or empty\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}


TEST_CASE("ServiceRegistry: POST /query/system invalid port", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"test\","
          "\"address\": \"127.0.0.2\","
          "\"port\": 65536"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Port must be between 0 and 65535.\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /query/system missing system from db", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"test\","
          "\"address\": \"127.0.0.2\","
          "\"port\": 65533"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"No system with name: test, address: 127.0.0.2 and port: 65533\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: POST /query/system", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const char *payload =
     "{"
          "\"systemName\": \"tesT sy sTemName\","
          "\"address\": \"127.0.0.2\","
          "\"port\": 1234,"
     "}";

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query/system", std::string(payload) });

    REQUIRE(resp == http::status_code::OK);

    const char *expResp =
    "{"
        "\"id\": 1,"
        "\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\","
        "\"port\": 1234,"
        "\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}


///////////////////////////////
// Private - Query/System/Id
//////////////////////////////


TEST_CASE("ServiceRegistry: GET /query/system/{id} missing id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp1 = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query/system", "" });

    REQUIRE(resp1 == http::status_code::NotFound);

    const auto resp2 = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query/system/", "" });

    REQUIRE(resp1 == http::status_code::NotFound);
}

TEST_CASE("ServiceRegistry: GET /query/system/{id} invalid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query/system/12", "" });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"System with id 12 not found.\",\"errorCode\": 400,\"exceptionType\": \"INVALID_PARAMETER\",\"origin\": \"serviceregistry/query/system/{id}\"}";

    const std::string sExpResp(expResp);

    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /query/system/{id} negative id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query/system/-1", "" });

    REQUIRE(resp == http::status_code(400));

    const char *expResp = "{\"errorMessage\": \"Id must be greater than 0.\",\"errorCode\": 400,\"exceptionType\": \"BAD_PAYLOAD\",\"origin\": \"serviceregistry/query/system/{id}\"}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
}

TEST_CASE("ServiceRegistry: GET /query/system/{id} valid id", "[core] [ServiceRegistry]") {
    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    mdb.table("system_", true, { "id", "system_name", "address", "port", "authentication_info", "created_at", "updated_at" }, {
        {1, "testsystemname", "127.0.0.2", 1234, "fdsa", "2020-09-11 10:39:08", "2020-09-11 10:39:40"}
    });

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query/system/1", "" });

    REQUIRE(resp == http::status_code::OK);

    const char *expResp =
    "{"
        "\"id\": 1,"
        "\"systemName\": \"testsystemname\","
        "\"address\": \"127.0.0.2\","
        "\"port\": 1234,"
        "\"authenticationInfo\": \"fdsa\","
        "\"createdAt\": \"2020-09-11 10:39:08\","
        "\"updatedAt\": \"2020-09-11 10:39:40\""
    "}";

    const std::string sExpResp(expResp);
    REQUIRE(JsonCompare(resp.value(), sExpResp));
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
            "\"additionalprop1\":\"meta1\","
            "\"additionalprop2\":\"meta2\","
            "\"additionalprop3\":\"meta3\"},"
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
