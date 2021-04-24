////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, orchestrator
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test Orchestrator
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/Orchestrator/Orchestrator.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"
#include "hlpr/JsonComp.h"


TEST_CASE("orchestrator: GET /ECHO", "[core] [orchestrator]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    Orchestrator<db::DatabasePool<MockDBase>, MockCurl> orch{ pool, reqBuilder };

    const auto resp = orch.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}

