////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, dev_registry
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test DevRegistry
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <iostream>
#include <ctime>
#include <string>

#include "core/apps/DevRegistry/DevRegistry.h"
#include "http/crate/X509Cert.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("dev_registry: GET /echo", "[core] [dev_registry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devreg{ pool, reqBuilder };

    const auto resp = devreg.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}


TEST_CASE("dev_registry: GET /mgmt/device", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", true, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor1", "address3", "33:34:56:78:9a:bc", "ai3", "2000-02-02", "2021-05-05" },
        { 4, "sensor2", "address4", "44:34:56:78:9a:bc", "ai4", "2000-02-02", "2021-05-05" },
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    // query existing device
    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/device/3", "" });
        REQUIRE(resp == http::status_code::OK);

        const auto r = R"json({
            "id": 3,
            "createdAt": "2000-02-02",
            "updatedAt": "2021-05-05",
            "deviceName": "sensor1",
            "address": "address3",
            "macAddress": "33:34:56:78:9a:bc",
            "authenticationInfo": "ai3"
        })json";
        REQUIRE(JsonCompare(resp.value(), r));
    }

    // query nonexisting device
    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/device/12", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("dev_registry: CRUD /mgmt/devices", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", true, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor1", "address3", "33:34:56:78:9a:bc", "ai3", "2000-02-02", "2021-05-05" },
        { 4, "sensor2", "address4", "44:34:56:78:9a:bc", "ai4", "2000-02-02", "2021-05-05" },
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/devices", "" });
        REQUIRE(resp == http::status_code::OK);

        const auto r = R"json({
            "count": 2,
            "data": [
                {"id": 3, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor1","address": "address3","macAddress": "33:34:56:78:9a:bc","authenticationInfo": "ai3"},
                {"id": 4, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor2","address": "address4","macAddress": "44:34:56:78:9a:bc","authenticationInfo": "ai4"}
            ]})json";

        REQUIRE(JsonCompare(resp.value(), r) == true);
    }

    {
        const auto p = R"json({
            "deviceName": "sensor1",
            "macAddress": "55:34:56:78:9a:bc"
            "address": "address5",
        })json";

        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/devices/3", p });
        REQUIRE(resp == http::status_code::OK);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/devices", "" });
        REQUIRE(resp == http::status_code::OK);
        const auto r = R"json({
            "count": 2,
            "data": [
                {"id": 3, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor1","address": "address5","macAddress": "55:34:56:78:9a:bc","authenticationInfo": "ai3"},
                {"id": 4, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor2","address": "address4","macAddress": "44:34:56:78:9a:bc","authenticationInfo": "ai4"}
            ]})json";

        REQUIRE(JsonCompare(resp.value(), r) == true);
    }

    {
        const auto p = R"json({
            "deviceName": "sensor6",
            "macAddress": "66:34:56:78:9a:bc"
            "address": "address6",
        })json";

        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/devices/3", p });

        REQUIRE(resp == http::status_code::OK);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/devices", "" });
        REQUIRE(resp == http::status_code::OK);
        const auto r = R"json({
            "count": 2,
            "data": [
                {"id": 3, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor6","address": "address6","macAddress": "66:34:56:78:9a:bc","authenticationInfo": null},
                {"id": 4, "createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor2","address": "address4","macAddress": "44:34:56:78:9a:bc","authenticationInfo": "ai4"}
            ]})json";

        REQUIRE(JsonCompare(resp.value(), r) == true);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/devices/4", "" });
        REQUIRE(resp == http::status_code::OK);

        // one entry was deleted
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device", db);
        REQUIRE(db == 1);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/devices/1", "" });
        REQUIRE(resp == http::status_code::OK);

        // no entry was deleted
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device", db);
        REQUIRE(db == 1);
    }

    {
        const auto p = R"json({
            "deviceName": "sensor7",
            "macAddress": "77:34:56:78:9a:bc"
            "address": "address7",
            "authenticationInfo": "ai7"
        })json";

        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/devices", p });
        REQUIRE(resp == http::status_code::OK);

        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device", db);
        REQUIRE(db == 2);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/devices", "" });
        REQUIRE(resp == http::status_code::OK);
        const auto r = R"json({
            "count": 2,
            "data": [
                {"id": 3,"createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor6","address": "address6","macAddress": "66:34:56:78:9a:bc","authenticationInfo": null},
                {"id": 4,"createdAt": "", "updatedAt": "", "deviceName": "sensor7", "address": "address7", "macAddress": "77:34:56:78:9a:bc","authenticationInfo": "ai7"}
            ]})json";

        REQUIRE(JsonCompare(resp.value(), r) == true);
    }

}


TEST_CASE("dev_registry: DELETE /unregister", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", false, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor.testcloud2.arrowhead.aitia.eu", "address3", "ma3", "ai3", "2000-02-02", "2021-05-05" },
        { 5, "magic1.testcloud2.arrowhead.aitia.eu", "address5", "ma5", "ai5", "2000-02-02", "2021-05-05" },
        { 6, "gadget.testcloud2.arrowhead.aitia.eu", "address6", "ma6", "ai6", "2000-02-02", "2021-05-05" }
    });

    mdb.table("device_registry", false, { "id", "device_id", "end_of_validity", "metadata", "version", "created_at", "updated_at" }, {
        { 11, 3, "2555-09-11", "metadata3", "12", "2000-02-02", "2021-05-05" },
        { 15, 6, "2555-09-11", "metadata6", "16", "2000-02-02", "2021-05-05" }
    });


    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    SECTION("Without any client certificate") {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?device_name=sensor.testcloud2.arrowhead.aitia.eu&mac_address=ma3", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "AUTH") == true);

        // and the content of the table did not change
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device_registry", db);
        REQUIRE(db == 2);
    }

    SECTION("With wrong client certificate") {
        auto req = Request{ "127.0.0.1", "DELETE", "/unregister?device_name=gadget.testcloud2.arrowhead.aitia.eu&mac_address=ma6", "" };
        req.setCert(X509Cert::load("data/test0117/sensor.pem"));
        const auto resp = devRegistry.dispatch(std::move(req));

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "AUTH") == true);

        // and the content of the table did not change
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device_registry", db);
        REQUIRE(db == 2);
    }

    SECTION("Device exists") {
        auto req = Request{ "127.0.0.1", "DELETE", "/unregister?device_name=sensor.testcloud2.arrowhead.aitia.eu&mac_address=ma3", "" };
        req.setCert(X509Cert::load("data/test0117/sensor.pem"));
        const auto resp = devRegistry.dispatch(std::move(req));

        REQUIRE(resp == http::status_code::OK);

        // one entry was deleted
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device_registry", db);
        REQUIRE(db == 1);
    }

    SECTION("Device does not exist") {
        auto req = Request{ "127.0.0.1", "DELETE", "/unregister?device_name=sensor.testcloud2.arrowhead.aitia.eu&mac_address=ma6", "" };
        req.setCert(X509Cert::load("data/test0117/sensor.pem"));
        const auto resp = devRegistry.dispatch(std::move(req));

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "INVALID_PARAMETER") == true);

        // and the content of the table did not change
        long db = 0;
        mdb.fetch("SELECT COUNT(*) FROM device_registry", db);
        REQUIRE(db == 2);
    }
}


TEST_CASE("dev_registry: POST /query (without further filtering)", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", false, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor.testcloud2.arrowhead.aitia.eu", "address3", "12:34:56:78:9a:bc", "ai3", "2000-02-02", "2021-05-05" },
        { 5, "magic1.testcloud2.arrowhead.aitia.eu", "address5", "11:11:11:11:11:11", "ai5", "2000-02-02", "2021-05-05" },
        { 6, "gadget.testcloud2.arrowhead.aitia.eu", "address6", "aa:aa:aa:aa:aa:aa", "ai6", "2000-02-02", "2021-05-05" }
    });

    mdb.table("device_registry", false, { "id", "device_id", "end_of_validity", "metadata", "version", "created_at", "updated_at" }, {
        { 11, 3, "2555-09-11", "metadata3", "12", "2000-02-02", "2021-05-05" },
        { 15, 6, "2555-09-11", "metadata6", "16", "2000-02-02", "2021-05-05" }
    });


    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto q = R"json({
            "deviceNameRequirement": "sensor.testcloud2.arrowhead.aitia.eu"
        })json";
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", q });
        REQUIRE(resp == http::status_code::OK);
        REQUIRE(resp.value() == R"json({"unfilteredHits": 1,"deviceQueryData": [{"id": 11,"version": 12,"createdAt": "2000-02-02","updatedAt": "2021-05-05","endOfValidity": "2555-09-11","device": {"id": 3,"createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor.testcloud2.arrowhead.aitia.eu","macAddress": "12:34:56:78:9a:bc","address": "address3","authenticationInfo": "ai3"}}]})json");
    }

    {
        const auto q = R"json({
            "deviceNameRequirement": "sensor.testcloud2.arrowhead.aitia.eu"
            "macAddressRequirement": "12:34:56:78:9a:bc"
        })json";
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", q });
        REQUIRE(resp == http::status_code::OK);
        REQUIRE(resp.value() == R"json({"unfilteredHits": 1,"deviceQueryData": [{"id": 11,"version": 12,"createdAt": "2000-02-02","updatedAt": "2021-05-05","endOfValidity": "2555-09-11","device": {"id": 3,"createdAt": "2000-02-02","updatedAt": "2021-05-05","deviceName": "sensor.testcloud2.arrowhead.aitia.eu","macAddress": "12:34:56:78:9a:bc","address": "address3","authenticationInfo": "ai3"}}]})json");
    }

    {
        const auto q = R"json({
            "deviceNameRequirement": "rubber.testcloud2.arrowhead.aitia.eu"
        })json";
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", q });
        REQUIRE(resp == http::status_code::OK);
        REQUIRE(resp.value() == R"json({"unfilteredHits": 0,"deviceQueryData": []})json");
    }

    {
        const auto q = R"json({
            "deviceNameRequirement": "sensor.testcloud2.arrowhead.aitia.eu"
            "macAddressRequirement": "11:11:11:11:11:11"
        })json";
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/query", q });
        REQUIRE(resp == http::status_code::OK);
        REQUIRE(resp.value() == R"json({"unfilteredHits": 0,"deviceQueryData": []})json");
    }
}


TEST_CASE("dev_registry: POST /register", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", true, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor1.testcloud2.arrowhead.aitia.eu", "address3", "33:34:56:78:9a:bc", "ai3", "2000-02-02", "2021-05-05" },
        { 4, "sensor2.testcloud2.arrowhead.aitia.eu", "address4", "44:34:56:78:9a:bc", "ai4", "2000-02-02", "2021-05-05" },
    });
    mdb.date("device", "id", "created_at");
    mdb.date("device", "id", "updated_at", true);

    mdb.table("device_registry", true, { "id", "device_id", "end_of_validity", "metadata", "version", "created_at", "updated_at" }, {
        { 11, 3, "2555-09-11", nullptr, 12, "2000-02-02", "2021-05-05" },
    });
    mdb.date("device_registry", "id", "created_at");
    mdb.date("device_registry", "id", "updated_at", true);


    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto q = R"json({
            "device": {
                "deviceName": "sensor1.testcloud2.arrowhead.aitia.eu",
                "macAddress": "33:34:56:78:9a:bc",
                "address": "addressNEW",
                "authenticationInfo": "aiNEW"
            },
            "version": 7
        })json";

        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/register", q });

        REQUIRE(resp == http::status_code::OK);
    }

}
