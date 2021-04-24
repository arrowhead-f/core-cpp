////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, choreographer
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Choreographer
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
#include <string>

#include "core/Choreographer/Choreographer.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("choreographer: GET /echo", "[core] [choreographer]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Choreographer<MockPool, MockCurl> ch{ pool, reqBuilder };

    const auto resp = ch.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}

