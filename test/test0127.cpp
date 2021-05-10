////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, onboarding
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Onboarding
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/apps/Onboarding/Onboarding.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"
#include "hlpr/JsonComp.h"


TEST_CASE("onboarding: GET /ECHO", "[core] [onboarding]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    Onboarding<db::DatabasePool<MockDBase>, MockCurl> onb{ pool, reqBuilder };

    const auto resp = onb.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}

