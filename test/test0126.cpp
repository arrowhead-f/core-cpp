////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, onboarding
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Onboarding
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
#include <string>

#include "core/apps/Onboarding/Onboarding.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("onboarding: GET /echo", "[core] [onboarding]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    Onboarding<MockPool, MockCurl> onb{ pool, reqBuilder };

    const auto resp = onb.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}

