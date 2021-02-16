////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, ServiceRegistry
/// Date:      2021-02-15
/// Author(s): tt
///
/// Description:
/// * This will test the operation of the ServiceRegistry module through bad URLs
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include <string>

#include "core/ServiceRegistry/ServiceRegistry.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"


TEST_CASE("ServiceRegistry: /ECHO", "[core] [ServiceRegistry]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    ServiceRegistry<MockPool, MockCurl> serviceRegistry{ pool, reqBuilder };

    const auto resp = serviceRegistry.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}
