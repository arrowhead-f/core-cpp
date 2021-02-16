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
