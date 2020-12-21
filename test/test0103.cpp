////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, cert_authority
/// Date:      2020-12-06
/// Author(s): ng201
///
/// Description:
/// * This will test Certificate Authority; sad path
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/CertAuthority/CertAuthority.h"

#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"
#include "hlpr/HelperDB.h"


TEST_CASE("cert_authority: /ECHO", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    std::string response;
    const auto ret = certAuthority.dispatch("127.0.0.1", "GET", "/ECHO", response, "");

    REQUIRE(ret != 0);
}


TEST_CASE("cert_authority: wrong method for /checkTrustedKey", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    std::string response;
    const auto ret = certAuthority.dispatch("127.0.0.1", "GET", "/checkTrustedKey", response, "");

    REQUIRE(ret == ErrCode::UNKNOWN_URI);
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
        std::string response;
        const auto ret = certAuthority.dispatch("127.0.0.1", "POST", "/checkTrustedKey", response, payload);
    });

    REQUIRE(num == 0);
}
