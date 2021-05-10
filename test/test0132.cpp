////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, sys_registry
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test SysRegistry
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
#include <string>

#include "core/apps/SysRegistry/SysRegistry.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("sys_registry: GET /echo", "[core] [sys_registry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    SysRegistry<MockPool, MockCurl> sysreg{ pool, reqBuilder };

    const auto resp = sysreg.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
}

