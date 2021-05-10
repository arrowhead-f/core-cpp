////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, dev_registry
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test DevRegistry
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/DevRegistry/DevRegistry.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"
#include "hlpr/JsonComp.h"


TEST_CASE("dev_registry: GET /ECHO", "[core] [dev_registry]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<db::DatabasePool<MockDBase>, MockCurl> devreg{ pool, reqBuilder };

    const auto resp = devreg.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}


TEST_CASE("dev_registry: check different methods with /unregister", "[core] [dev_registry]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<db::DatabasePool<MockDBase>, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/unregister", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "POST", "/unregister", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/unregister", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/unregister", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "HEAD", "/unregister", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "OPTIONS", "/unregister", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: DELETE\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }
}


TEST_CASE("dev_registry: DELETE /unregister with wrong url parameters", "[core] [dev_registry]") {

    MockDBase mdb{ };
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    SECTION("Without any parameter") {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("Only with device name given") {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?device_name=device", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }


    SECTION("Only with mac address given") {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?mac_address=macaddress", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("With unknown parameters") {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/unregister?hacker=TheBigHacket&mac_addres=macAddress", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }
}


TEST_CASE("dev_registry: check different methods with /register", "[core] [dev_registry]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<db::DatabasePool<MockDBase>, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/register", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/register", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/register", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "HEAD", "/register", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/register", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "OPTIONS", "/register", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: POST\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }
}


TEST_CASE("dev_registry: check different methods with /query", "[core] [dev_registry]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<db::DatabasePool<MockDBase>, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/query", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PUT", "/query", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "PATCH", "/query", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "HEAD", "/query", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "DELETE", "/query", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "OPTIONS", "/query", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: POST\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }
}


TEST_CASE("dev_registry: call /mgmt/device with wrong params", "[core] [dev_registry]") {

    MockDBase mdb{ };
    mdb.table("device", true, { "id", "device_name", "address", "mac_address", "authentication_info", "created_at", "updated_at" }, {
        { 3, "sensor1.testcloud2.arrowhead.aitia.eu", "address3", "33:34:56:78:9a:bc", "ai3", "2000-02-02", "2021-05-05" },
        { 4, "sensor2.testcloud2.arrowhead.aitia.eu", "address4", "44:34:56:78:9a:bc", "ai4", "2000-02-02", "2021-05-05" },
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    DevRegistry<MockPool, MockCurl> devRegistry{ pool, reqBuilder };

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/device/12/13", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }

    {
        const auto resp = devRegistry.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/device/almafa", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
}

