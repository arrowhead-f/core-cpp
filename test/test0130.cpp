////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, qos_monitor
/// Date:      2021-04-24
/// Author(s): ng201
///
/// Description:
/// * this will test QosMonitor
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/apps/QosMonitor/QosMonitor.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"
#include "hlpr/JsonComp.h"


TEST_CASE("qos_monitor: GET /ECHO", "[core] [qos_monitor]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    QosMonitor<db::DatabasePool<MockDBase>, MockCurl> qosm{ pool, reqBuilder };

    const auto resp = qosm.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}

