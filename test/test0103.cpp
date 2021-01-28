////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, cert_authority
/// Date:      2020-12-06
/// Author(s): ng201
///
/// Description:
/// * this will test Certificate Authority
/// * sad path, e.g.,
///   * user data was mailformed
///   * (mandatory) fields are missing from the json
///   * exception was thrown by the database wrapper
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/CertAuthority/CertAuthority.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"


TEST_CASE("cert_authority: GET /ECHO", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}


TEST_CASE("cert_authority: wrong method for /checkTrustedKey", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/checkTrustedKey", "" });

    REQUIRE(resp == http::status_code::NotFound);
}


TEST_CASE("cert_authority: malformed payload for /checkTrustedKey", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        // payload is empty
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", "" });
        REQUIRE(resp == http::status_code::BadRequest);
    }

    {
        // mandatory key not found in dictionary
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", "{purricKey:\"TheKey\"}" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
    {
        // key found, but with wrog type
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", "{publicKey: 123}" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("cert_authority: POST /checkTrustedKey (exceptions)" , "[core] [cert_authority]") {

    HelperDB mdb{ };
    mdb.table("ca_trusted_key", false, { "id", "created_at", "description", "hash" }, {
        {1, "2020-12-06", "St. Nicholas", "cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45"},
        {3, "2020-12-06", "Who Cares I.", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"},
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    // sha256
    // TheKey   ---> cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45

    const auto num = mdb.run_except([&mdb, &certAuthority]() {
        const std::string payload = R"json({ "publicKey": "TheKey" })json";
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", payload });
    });

    // no exception was observed outside the dispatch mehod
    REQUIRE(num == 0);
}
