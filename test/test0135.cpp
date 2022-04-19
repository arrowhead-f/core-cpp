////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, authorizer
/// Date:      2022-04-05
/// Author(s): ng201
///
/// Description:
/// * this will test Authorization
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
#include <string>

#include "core/apps/Authorization/Authorization.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("authorization: GET /echo", "[core] [authorization]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}


TEST_CASE("authorization: GET /mgmt/intracloud/{id}", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intracloud/1", "" });

    REQUIRE(resp == http::status_code::OK);

    const std::string result = R"json(
    {
        "id":1,
        "createdAt":"2020-12-06",
        "updatedAt":"2022-04-03",
        "consumerSystem":{
            "id":7,
            "systemName":"consumer-07",
            "address":"192.168.7.7",
            "port":172,
            "authenticationInfo":"auth-consumer-07",
            "createdAt":"2020-12-16",
            "updatedAt":"2022-04-04"
        },
        "providerSystem":{
            "id":12,
            "systemName":"provider-12",
            "address":"192.168.12.12",
            "port":212,
            "authenticationInfo":"auth-provider-12",
            "createdAt":"2020-12-16",
            "updatedAt":"2022-04-04"
        },
        "serviceDefinition":{
            "id":100,
            "serviceDefinition":"serv-definition-100",
            "createdAt":"2020-12-15",
            "updatedAt":"2022-04-03"
        },
        "interfaces":[
            {
                "id":52,
                "interfaceName":"iface-12",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            {
                "id":53,
                "interfaceName":"iface-13",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
            }
        ]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));
}


TEST_CASE("authorization: GET /mgmt/intracloud without query string", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intracloud", "" });

    REQUIRE(resp == http::status_code::OK);

    const std::string result = R"json(
    {
        "count":2,
        "data":[
        {
            "id":1,
            "createdAt":"2020-12-06",
            "updatedAt":"2022-04-03",
            "consumerSystem":{
                "id":7,
                "systemName":"consumer-07",
                "address":"192.168.7.7",
                "port":172,
                "authenticationInfo":"auth-consumer-07",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "providerSystem":{
                "id":12,
                "systemName":"provider-12",
                "address":"192.168.12.12",
                "port":212,
                "authenticationInfo":"auth-provider-12",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "serviceDefinition":{
                "id":100,
                "serviceDefinition":"serv-definition-100",
                "createdAt":"2020-12-15",
                "updatedAt":"2022-04-03"
            },
            "interfaces":[
            {
               "id":52,
               "interfaceName":"iface-12",
               "createdAt":"2020-12-16",
               "updatedAt":"2022-04-04"
            },
            {
               "id":53,
               "interfaceName":"iface-13",
               "createdAt":"2020-12-17",
               "updatedAt":"2022-04-05"
            }]
        },
        {
            "id":3,
            "createdAt":"2020-12-06",
            "updatedAt":"2022-04-03",
            "consumerSystem":{
                "id":8,
                "systemName":"consumer-08",
                "address":"192.168.8.8",
                "port":182,
                "authenticationInfo":"auth-consumer-08",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
            },
            "providerSystem":{
                "id":14,
                "systemName":"provider-14",
                "address":"192.168.14.14",
                "port":214,
                "authenticationInfo":"auth-provider-14",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
             },
             "serviceDefinition":{
                "id":111,
                "serviceDefinition":"serv-definition-111",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "interfaces":[
            {
               "id":51,
               "interfaceName":"iface-11",
               "createdAt":"2020-12-15",
               "updatedAt":"2022-04-03"
            }]
        }]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));
}


TEST_CASE("authorization: GET /mgmt/intracloud with query string I.", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intracloud?page=0&item_per_page=1", "" });

        REQUIRE(resp == http::status_code::OK);
        const std::string result = R"json(
        {
            "count":2,
            "data":[
            {
                "id":1,
                "createdAt":"2020-12-06",
                "updatedAt":"2022-04-03",
                "consumerSystem":{
                    "id":7,
                    "systemName":"consumer-07",
                    "address":"192.168.7.7",
                    "port":172,
                    "authenticationInfo":"auth-consumer-07",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "providerSystem":{
                    "id":12,
                    "systemName":"provider-12",
                    "address":"192.168.12.12",
                    "port":212,
                    "authenticationInfo":"auth-provider-12",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "serviceDefinition":{
                    "id":100,
                    "serviceDefinition":"serv-definition-100",
                    "createdAt":"2020-12-15",
                    "updatedAt":"2022-04-03"
                },
                "interfaces":[
                {
                    "id":52,
                    "interfaceName":"iface-12",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                {
                    "id":53,
                    "interfaceName":"iface-13",
                    "createdAt":"2020-12-17",
                    "updatedAt":"2022-04-05"
                }]
            }]
        })json";

        REQUIRE(JsonCompare(resp.value(), result));
    }

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intracloud?page=0&item_per_page=1&direction=DESC", "" });

        REQUIRE(resp == http::status_code::OK);

        const std::string result = R"json(
        {
            "count":2,
            "data":[
            {
                "id":3,
                "createdAt":"2020-12-06",
                "updatedAt":"2022-04-03",
                "consumerSystem":{
                    "id":8,
                    "systemName":"consumer-08",
                    "address":"192.168.8.8",
                    "port":182,
                    "authenticationInfo":"auth-consumer-08",
                    "createdAt":"2020-12-17",
                    "updatedAt":"2022-04-05"
                },
                "providerSystem":{
                    "id":14,
                    "systemName":"provider-14",
                    "address":"192.168.14.14",
                    "port":214,
                    "authenticationInfo":"auth-provider-14",
                    "createdAt":"2020-12-17",
                    "updatedAt":"2022-04-05"
                },
                "serviceDefinition":{
                    "id":111,
                    "serviceDefinition":"serv-definition-111",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "interfaces":[
                {
                    "id":51,
                    "interfaceName":"iface-11",
                    "createdAt":"2020-12-15",
                    "updatedAt":"2022-04-03"
                }]
            }]
        })json";

        REQUIRE(JsonCompare(resp.value(), result));
    }
}


TEST_CASE("authorization: DELETE /mgmt/intracloud/{id}", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/intracloud/1", "" });
        REQUIRE(resp == http::status_code::OK);
    }

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intracloud/1", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("authorization: POST /mgmt/intracloud", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", true, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", true, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/intracloud", R"json({"consumerId": 7, "providerIds": [12], "interfaceIds": [52, 53], "serviceDefinitionIds": [100]})json" });
    REQUIRE(resp == http::status_code::OK);

    REQUIRE(resp.value().find("consumer-07")         != std::string::npos);
    REQUIRE(resp.value().find("provider-12")         != std::string::npos);
    REQUIRE(resp.value().find("serv-definition-100") != std::string::npos);
    REQUIRE(resp.value().find("iface-12")            != std::string::npos);
    REQUIRE(resp.value().find("iface-13")            != std::string::npos);

}


TEST_CASE("authorization: POST /intracloud/check", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const char *payload = R"json(
    {
        "consumer": {
            "address": "192.168.7.7",
            "authenticationInfo": "auth-consumer-07",
            "port": 172,
            "systemName": "consumer-07"
        },
        "providerIdsWithInterfaceIds": [{ "id": 14, "idList": [ 52, 53, 77 ]}, { "id": 12, "idList": [ 51, 52, 53 ]}, { "id": 22, "idList": [ 51, 52, 53, 60]}],
        "serviceDefinitionId": 100
    })json";

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/intracloud/check", payload });
    REQUIRE(resp == http::status_code::OK);

    const char *result = R"json(
    {
        "serviceDefinitionId":100,
        "consumer":{
            "id":7,
            "address":"192.168.7.7",
            "port":172,
            "systemName":"consumer-07",
            "authenticationInfo":"auth-consumer-07",
            "createdAt":"2020-12-16",
            "updatedAt":"2022-04-04"
        },
        "authorizedProviderIdsWithInterfaceIds":[{ "id":12, "idList":[ 52,53 ]}]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));

}


TEST_CASE("authorization: POST /intracloud/check (multiple providers match)", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_intra_cloud", false, { "id", "created_at", "updated_at", "consumer_system_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 7, 14, 100 }
    });
    mdb.table("authorization_intra_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_intra_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const char *payload = R"json(
    {
        "consumer": {
            "address": "192.168.7.7",
            "authenticationInfo": "auth-consumer-07",
            "port": 172,
            "systemName": "consumer-07"
        },
        "providerIdsWithInterfaceIds": [{ "id": 14, "idList": [ 52,53,77,51 ]}, {"id": 12, "idList": [ 51,52,53 ]}, {"id": 22, "idList": [ 51,52,53,60 ]}],
        "serviceDefinitionId": 100
    })json";

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/intracloud/check", payload });
    REQUIRE(resp == http::status_code::OK);

    const char *result = R"json(
    {
        "serviceDefinitionId":100,
        "consumer":{
            "id":7,
            "address":"192.168.7.7",
            "port":172,
            "systemName":"consumer-07",
            "authenticationInfo":"auth-consumer-07",
            "createdAt":"2020-12-16",
            "updatedAt":"2022-04-04"
        },
        "authorizedProviderIdsWithInterfaceIds":[{"id":12, "idList":[ 52,53 ]}, {"id":14, "idList":[ 51 ]}]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));

}


namespace {

    void createInterCloudDatabase(MockDBase &mdb) {
        mdb.table("authorization_inter_cloud", false, { "id", "created_at", "updated_at", "consumer_cloud_id", "provider_system_id", "service_id" }, {
            { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
            { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
        });
        mdb.table("authorization_inter_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_inter_cloud_id", "interface_id" }, {
            { 11, "2020-12-16", "2022-04-04", 3, 51 },
            { 12, "2020-12-16", "2022-04-04", 1, 52 },
            { 13, "2020-12-17", "2022-04-05", 1, 53 }
        });
        mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
            {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
            {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
            {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
            { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
            { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
        });
        mdb.table("cloud", false, { "id", "created_at", "updated_at", "name", "operator", "secure", "neighbor", "own_cloud", "authentication_info" }, {
            {  3, "2020-12-16", "2022-04-04", "unused--", "op-003", 1, 1, 1, "auth-unused--" },
            {  7, "2020-12-16", "2022-04-04", "cloud-07", "op-007", 1, 1, 0, "auth-cloud-07" },
            {  8, "2020-12-17", "2022-04-05", "cloud-08", "op-008", 1, 1, 1, "auth-cloud-08" },
            { 12, "2020-12-16", "2022-04-04", "cloud-12", "op-012", 0, 1, 0, "auth-cloud-12" },
            { 25, "2020-12-17", "2022-04-05", "cloud-14", "op-025", 1, 1, 1, "auth-cloud-25" }
        });
        mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
            { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
            { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
            { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
        });
        mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
            { 51, "2020-12-15", "2022-04-03", "iface-11" },
            { 52, "2020-12-16", "2022-04-04", "iface-12" },
            { 53, "2020-12-17", "2022-04-05", "iface-13" }
        });
    }

}



TEST_CASE("authorization: GET /mgmt/intercloud/{id}", "[core] [authorization]") {

    MockDBase mdb{ };

    createInterCloudDatabase(mdb);

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intercloud/1", "" });

    REQUIRE(resp == http::status_code::OK);

    const std::string result = R"json(
    {
        "id":1, "createdAt":"2020-12-06", "updatedAt":"2022-04-03",
        "cloud":{ "id":7, "name":"cloud-07", "operator":"op-007", "secure":true, "neighbor":true, "ownCloud":false, "authenticationInfo":"auth-cloud-07", "createdAt":"2020-12-16", "updatedAt":"2022-04-04" },
        "providerSystem":{ "id":12, "systemName":"provider-12", "address":"192.168.12.12", "port":212, "authenticationInfo":"auth-provider-12", "createdAt":"2020-12-16", "updatedAt":"2022-04-04" },
        "serviceDefinition":{ "id":100, "serviceDefinition":"serv-definition-100", "createdAt":"2020-12-15", "updatedAt":"2022-04-03" },
        "interfaces":[{ "id":52, "interfaceName":"iface-12", "createdAt":"2020-12-16", "updatedAt":"2022-04-04" }, { "id":53, "interfaceName":"iface-13", "createdAt":"2020-12-17", "updatedAt":"2022-04-05" }]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));
}


TEST_CASE("authorization: GET /mgmt/intercloud without query string", "[core] [authorization]") {

    MockDBase mdb{ };

    createInterCloudDatabase(mdb);

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intercloud", "" });

    REQUIRE(resp == http::status_code::OK);

    const std::string result = R"json(
    {
        "count":2,
        "data":[
        {
            "id":1, "createdAt":"2020-12-06", "updatedAt":"2022-04-03",
            "cloud":{
                "id":7,
                "name":"cloud-07",
                "operator":"op-007",
                "secure":true,
                "neighbor":true,
                "ownCloud":false,
                "authenticationInfo":"auth-cloud-07",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "providerSystem":{
                "id":12,
                "systemName":"provider-12",
                "address":"192.168.12.12",
                "port":212,
                "authenticationInfo":"auth-provider-12",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "serviceDefinition":{
                "id":100,
                "serviceDefinition":"serv-definition-100",
                "createdAt":"2020-12-15",
                "updatedAt":"2022-04-03"
            },
            "interfaces":[
            {
                "id":52,
                "interfaceName":"iface-12",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            {
                "id":53,
                "interfaceName":"iface-13",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
            }
        ]},
        {
            "id":3,
            "createdAt":"2020-12-06",
            "updatedAt":"2022-04-03",
            "cloud":{
                "id":8,
                "name":"cloud-08",
                "operator":"op-008",
                "secure":true,
                "neighbor":true,
                "ownCloud":true,
                "authenticationInfo":"auth-cloud-08",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
            },
            "providerSystem":{
                "id":14,
                "systemName":"provider-14",
                "address":"192.168.14.14",
                "port":214,
                "authenticationInfo":"auth-provider-14",
                "createdAt":"2020-12-17",
                "updatedAt":"2022-04-05"
            },
            "serviceDefinition":{
                "id":111,
                "serviceDefinition":"serv-definition-111",
                "createdAt":"2020-12-16",
                "updatedAt":"2022-04-04"
            },
            "interfaces":[
            {
                "id":51,
                "interfaceName":"iface-11",
                "createdAt":"2020-12-15",
                "updatedAt":"2022-04-03"
            }]
        }]
    })json";

    REQUIRE(JsonCompare(resp.value(), result));
}


TEST_CASE("authorization: GET /mgmt/intercloud with query string ", "[core] [authorization]") {

    MockDBase mdb{ };

    createInterCloudDatabase(mdb);

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intercloud?page=0&item_per_page=1", "" });

        REQUIRE(resp == http::status_code::OK);

        const std::string result = R"json(
        {
            "count":2,
            "data":[
            {
                "id":1,
                "createdAt":"2020-12-06",
                "updatedAt":"2022-04-03",
                "cloud": {
                    "id":7,
                    "name":"cloud-07",
                    "operator":"op-007",
                    "secure":true,
                    "neighbor":true,
                    "ownCloud":false,
                    "authenticationInfo":"auth-cloud-07",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "providerSystem":{
                    "id":12,
                    "systemName":"provider-12",
                    "address":"192.168.12.12",
                    "port":212,
                    "authenticationInfo":"auth-provider-12",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "serviceDefinition":{
                    "id":100,
                    "serviceDefinition":"serv-definition-100",
                    "createdAt":"2020-12-15",
                    "updatedAt":"2022-04-03"
                },
                "interfaces":[{ "id":52, "interfaceName":"iface-12", "createdAt":"2020-12-16", "updatedAt":"2022-04-04" },
                              { "id":53, "interfaceName":"iface-13", "createdAt":"2020-12-17", "updatedAt":"2022-04-05" }]
            }]
        })json";

        REQUIRE(JsonCompare(resp.value(), result));
    }

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intercloud?page=0&item_per_page=1&direction=DESC", "" });

        REQUIRE(resp == http::status_code::OK);

        const std::string result = R"json(
        {
            "count":2,
            "data":[
            {
                "id":3, "createdAt":"2020-12-06", "updatedAt":"2022-04-03",
                "cloud":{
                    "id":8,
                    "name":"cloud-08",
                    "operator":"op-008",
                    "secure":true,
                    "neighbor":true,
                    "ownCloud":true,
                    "authenticationInfo":"auth-cloud-08",
                    "createdAt":"2020-12-17",
                    "updatedAt":"2022-04-05"
                },
                "providerSystem":{
                    "id":14,
                    "systemName":"provider-14",
                    "address":"192.168.14.14",
                    "port":214,
                    "authenticationInfo":"auth-provider-14",
                    "createdAt":"2020-12-17",
                    "updatedAt":"2022-04-05"
                },
                "serviceDefinition":{
                    "id":111,
                    "serviceDefinition":"serv-definition-111",
                    "createdAt":"2020-12-16",
                    "updatedAt":"2022-04-04"
                },
                "interfaces":[{ "id":51, "interfaceName":"iface-11", "createdAt":"2020-12-15", "updatedAt":"2022-04-03" }]
            }]
        })json";

        REQUIRE(JsonCompare(resp.value(), result));
    }
}


TEST_CASE("authorization: DELETE /mgmt/intercloud/{id}", "[core] [authorization]") {

    MockDBase mdb{ };

    createInterCloudDatabase(mdb);

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/intercloud/1", "" });
        REQUIRE(resp == http::status_code::OK);
    }

    {
        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/intercloud/1", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("authorization: POST /mgmt/intercloud", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_inter_cloud", true, { "id", "created_at", "updated_at", "consumer_cloud_id", "provider_system_id", "service_id" }, {
        { 3, "2020-12-06", "2022-04-03", 8, 14, 111 }
    });
    mdb.table("authorization_inter_cloud_interface_connection", true, { "id", "created_at", "updated_at", "authorization_inter_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("cloud", false, { "id", "created_at", "updated_at", "name", "operator", "secure", "neighbor", "own_cloud", "authentication_info" }, {
        {  3, "2020-12-16", "2022-04-04", "unused--", "op-003", 1, 1, 1, "auth-unused--" },
        {  7, "2020-12-16", "2022-04-04", "cloud-07", "op-007", 1, 1, 0, "auth-cloud-07" },
        {  8, "2020-12-17", "2022-04-05", "cloud-08", "op-008", 1, 1, 1, "auth-cloud-08" },
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 111, "2020-12-16", "2022-04-04", "serv-definition-111" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });


    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/intercloud", R"json({"cloudId": 7, "providerIds": [12], "interfaceIds": [52, 53], "serviceDefinitionIds": [100]})json" });
    REQUIRE(resp == http::status_code::OK);

    REQUIRE(resp.value().find("cloud-07")            != std::string::npos);
    REQUIRE(resp.value().find("provider-12")         != std::string::npos);
    REQUIRE(resp.value().find("serv-definition-100") != std::string::npos);
    REQUIRE(resp.value().find("iface-12")            != std::string::npos);
    REQUIRE(resp.value().find("iface-13")            != std::string::npos);

}


TEST_CASE("authorization: POST /intercloud/check", "[core] [authorization]") {

    MockDBase mdb{ };

    createInterCloudDatabase(mdb);
    mdb.table("cloud_gatekeeper_relay", false, { "id", "created_at", "updated_at", "cloud_id", "relay_id" }, {
        { 1111, "2020-12-16", "2022-04-04", 7, 151 },
        { 1112, "2020-12-16", "2022-04-04", 8, 152 }
    });
    mdb.table("cloud_gateway_relay", false, { "id", "created_at", "updated_at", "cloud_id", "relay_id" }, {
        { 2226, "2020-12-16", "2022-04-04", 7, 244 },
        { 2227, "2020-12-16", "2022-04-04", 8, 245 }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const char *payload = R"json(
    {
        "cloud": { "name": "cloud-07", "authenticationInfo": "auth-cloud-07", "operator": "op-007", "secure": true, "neighbor": true, "gatekeeperRelayIds": [151], "gatewayRelayIds": [244]},
        "providerIdsWithInterfaceIds": [{ "id": 14, "idList": [ 52, 53, 77 ]}, { "id": 12, "idList": [ 51, 52, 53 ]}, { "id": 22, "idList": [ 51, 52, 53, 60]}],
        "serviceDefinitionId": 100
    })json";

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/intercloud/check", payload });
    REQUIRE(resp == http::status_code::OK);

    const char *result = R"json({"serviceDefinitionId": 100,"cloud": {"id": 7,"name": "cloud-07","operator": "op-007","secure": true,"neighbor": true,"ownCloud": false,"authenticationInfo": "auth-cloud-07","createdAt": "2020-12-16","updatedAt": "2022-04-04"},"authorizedProviderIdsWithInterfaceIds": [{"id": 12,"idList": [52,53]}]})json";
    REQUIRE(JsonCompare(resp.value(), result));
}


TEST_CASE("authorization: POST /intercloud/check (multiple providers match)", "[core] [authorization]") {

    MockDBase mdb{ };
    mdb.table("authorization_inter_cloud", false, { "id", "created_at", "updated_at", "consumer_cloud_id", "provider_system_id", "service_id" }, {
        { 1, "2020-12-06", "2022-04-03", 7, 12, 100 },
        { 3, "2020-12-06", "2022-04-03", 7, 14, 100 }
    });
    mdb.table("authorization_inter_cloud_interface_connection", false, { "id", "created_at", "updated_at", "authorization_inter_cloud_id", "interface_id" }, {
        { 11, "2020-12-16", "2022-04-04", 3, 51 },
        { 12, "2020-12-16", "2022-04-04", 1, 52 },
        { 13, "2020-12-17", "2022-04-05", 1, 53 }
    });
    mdb.table("system_", false, { "id", "created_at", "updated_at", "system_name", "address", "port", "authentication_info" }, {
        {  5, "2020-12-16", "2022-04-04", "unused",      "192.168.5.5",   152, "auth-for-unused" },
        {  7, "2020-12-16", "2022-04-04", "consumer-07", "192.168.7.7",   172, "auth-consumer-07" },
        {  8, "2020-12-17", "2022-04-05", "consumer-08", "192.168.8.8",   182, "auth-consumer-08" },
        { 12, "2020-12-16", "2022-04-04", "provider-12", "192.168.12.12", 212, "auth-provider-12" },
        { 14, "2020-12-17", "2022-04-05", "provider-14", "192.168.14.14", 214, "auth-provider-14" }
    });
    mdb.table("cloud", false, { "id", "created_at", "updated_at", "name", "operator", "secure", "neighbor", "own_cloud", "authentication_info" }, {
        {  3, "2020-12-16", "2022-04-04", "unused--", "op-003", 1, 1, 1, "auth-unused--" },
        {  7, "2020-12-16", "2022-04-04", "cloud-07", "op-007", 1, 1, 0, "auth-cloud-07" },
        {  8, "2020-12-17", "2022-04-05", "cloud-08", "op-008", 1, 1, 1, "auth-cloud-08" },
        { 12, "2020-12-16", "2022-04-04", "cloud-12", "op-012", 0, 1, 0, "auth-cloud-12" },
        { 25, "2020-12-17", "2022-04-05", "cloud-14", "op-025", 1, 1, 1, "auth-cloud-25" }
    });
    mdb.table("cloud_gatekeeper_relay", false, { "id", "created_at", "updated_at", "cloud_id", "relay_id" }, {
        { 1111, "2020-12-16", "2022-04-04", 7, 151 },
        { 1112, "2020-12-16", "2022-04-04", 8, 152 }
    });
    mdb.table("cloud_gateway_relay", false, { "id", "created_at", "updated_at", "cloud_id", "relay_id" }, {
        { 2226, "2020-12-16", "2022-04-04", 7, 244 },
        { 2227, "2020-12-16", "2022-04-04", 8, 245 }
    });
    mdb.table("service_definition", false, { "id", "created_at", "updated_at", "service_definition" }, {
        { 100, "2020-12-15", "2022-04-03", "serv-definition-100" },
        { 113, "2020-12-17", "2022-04-05", "serv-definition-113" }
    });
    mdb.table("service_interface", false, { "id", "created_at", "updated_at", "interface_name" }, {
        { 51, "2020-12-15", "2022-04-03", "iface-11" },
        { 52, "2020-12-16", "2022-04-04", "iface-12" },
        { 53, "2020-12-17", "2022-04-05", "iface-13" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    {
        const char *payload = R"json(
        {
            "cloud": { "name": "cloud-07", "authenticationInfo": "auth-cloud-07", "operator": "op-007", "secure": true, "neighbor": true, "gatekeeperRelayIds": [151], "gatewayRelayIds": [244]},
            "providerIdsWithInterfaceIds": [{ "id": 14, "idList": [ 52,53,51,77 ]}, { "id": 12, "idList": [ 51,44,52,11,53 ]}, { "id": 22, "idList": [ 51,52,53,60 ]}],
            "serviceDefinitionId": 100
        })json";

        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/intercloud/check", payload });
        REQUIRE(resp == http::status_code::OK);

        const char *result = R"json(
        {
            "serviceDefinitionId":100,
            "cloud":{ "id":7, "name":"cloud-07", "operator":"op-007", "secure":true, "neighbor":true, "ownCloud":false, "authenticationInfo":"auth-cloud-07", "createdAt":"2020-12-16", "updatedAt":"2022-04-04" },
            "authorizedProviderIdsWithInterfaceIds":[{ "id":12, "idList":[ 52,53 ]}, { "id":14, "idList":[ 51 ]}]
        })json";

        REQUIRE(JsonCompare(resp.value(), result));
    }

    {
        const char *payload = R"json(
        {
            "cloud": { "name": "cloud-07", "authenticationInfo": "auth-cloud-07", "operator": "op-007", "secure": true, "neighbor": true, "gatekeeperRelayIds": [88], "gatewayRelayIds": [244]},
            "providerIdsWithInterfaceIds": [{ "id": 14, "idList": [ 52,53,51,77 ]}, { "id": 12, "idList": [ 51,44,52,11,53 ]}, { "id": 22, "idList": [ 51,52,53,60 ]}],
            "serviceDefinitionId": 100
        })json";

        const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/intercloud/check", payload });
        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), "{}"));
    }

}


TEST_CASE("authorization: POST /token", "[core] [authorization]") {

    MockDBase mdb{ };
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Authorization<MockPool, MockCurl> authorizer{ pool, reqBuilder };

    const char *req = R"json(
        {
            "consumer": {
                "systemName": "ClientSystemName",
                "address": "127.0.0.1",
                "port": 8000
            },
            "consumerCloud": null,
            "duration": -1,
            "providers": [
                {
                    "provider": {
                        "address":"127.0.0.1",
                        "port": 5002,
                        "systemName":"testsystemname2",
                        "authenticationInfo":"fdsa"
                    },
                    "serviceInterfaces": [ "http-secure-json", "http-insecure-json" ]
                }
            ],
            "service":"testservice_1"
        })json";

    const auto resp = authorizer.dispatch(Request{ "127.0.0.1", "POST", "/token", req });
    REQUIRE(resp == http::status_code::OK);

    //{"tokenData": [{"providerAddress": "127.0.0.1","providerName": "testsystemname2","providerPort": 5002,"tokens": {"http-secure-json": "token0,"http-insecure-json": "token1}}]}
}
