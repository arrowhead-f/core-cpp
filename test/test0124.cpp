////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, gateway
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Gateway
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/Gateway/Gateway.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"
#include "hlpr/JsonComp.h"


TEST_CASE("gateway: GET /ECHO", "[core] [gateway]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    Gateway<db::DatabasePool<MockDBase>, MockCurl> gw{ pool, reqBuilder };

    const auto resp = gw.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}

