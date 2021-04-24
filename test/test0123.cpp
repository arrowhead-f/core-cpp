////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, gateway
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Gateway
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
#include <string>

#include "core/Gateway/Gateway.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("gateway: GET /echo", "[core] [gateway]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Gateway<MockPool, MockCurl> gw{ pool, reqBuilder };

    const auto resp = gw.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}

