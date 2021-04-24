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
#include "hlpr/JsonComp.h"


TEST_CASE("cert_authority: GET /ECHO", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/ECHO", "" });

    REQUIRE(resp == http::status_code::NotFound);
}


TEST_CASE("cert_authority: check different methods with /checkTrustedKey", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/checkTrustedKey", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PUT", "/checkTrustedKey", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PATCH", "/checkTrustedKey", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "DELETE", "/checkTrustedKey", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "OPTIONS", "/checkTrustedKey", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                       // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: POST\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }
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
        const auto resp = certAuthority.handle(Request{ "127.0.0.1", "POST", "/checkTrustedKey", payload });
    });

    // no exception was observed outside the dispatch mehod
    REQUIRE(num == 0);
}


TEST_CASE("cert_authority: check different methods with /checkCertificate", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/checkCertificate", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PUT", "/checkCertificate", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PATCH", "/checkCertificate", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "HEAD", "/checkCertificate", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "DELETE", "/checkCertificate", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "OPTIONS", "/checkCertificate", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: POST\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }

}


TEST_CASE("cert_authority: call /checkCertificate with empty payload", "[core] [cert_authority]") {
/*
    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "" });
    REQUIRE(resp == http::status_code::BadRequest);
*/
}


TEST_CASE("cert_authority: call /checkCertificate with empty param", "[core] [cert_authority]") {
/*
    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1}" });
        REQUIRE(resp == http::status_code::BadRequest);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1;certific:\"DododoDadada\"}" });
        REQUIRE(resp == http::status_code::BadRequest);
    }
*/
}


TEST_CASE("cert_authority: check different methods with /mgmt/certificates", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/certificates", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/certificates", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/certificates", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "HEAD", "/mgmt/certificates", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/certificates", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "OPTIONS", "/mgmt/certificates", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: GET\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }

}


TEST_CASE("cert_authority: check /mgmt/certificates with malformed parameters", "[core] [cert_authority]") {

    MockDBase mdb{ };
    mdb.table("ca_certificate", false, { "id", "common_name", "serial", "created_by", "created_at", "valid_after", "valid_before", "updated_at", "revoked_at" }, {
        {  7, "sysname-1", "0707070707", "test1", "1981-05-22 10:10:10", "1981-05-22 10:10:10", "5000-07-09 10:10:10", "1981-05-22 10:10:10", nullptr }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    const auto bad_payload_msg = R"json({"errorMessage": "Only both or none of page and size may be defined.", "errorCode": 400, "exceptionType": "BAD_PAYLOAD", "origin": "/mgmt/certificates"})json";

    SECTION("With null page but defined size") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?item_per_page=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("With null size but defined page") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?page=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("With unknown parameter") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?freddy=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }

    SECTION("With invalid direction") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?direction=top", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }

    SECTION("With invalid sort field") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?sort_field=mushroom", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("cert_authority: check /mgmt/certificates with database error", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates", "" });
    REQUIRE(resp == http::status_code::InternalServerError);
}


TEST_CASE("cert_authority: check different methods with /mgmt/keys", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/mgmt/keys", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PUT", "/mgmt/keys", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "PATCH", "/mgmt/keys", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "HEAD", "/mgmt/keys", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "DELETE", "/mgmt/keys", "" });
        REQUIRE(resp == http::status_code::MethodNotAllowed);
    }

    {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "OPTIONS", "/mgmt/keys", "" });

        #ifndef ARROWHEAD_FEAT_NO_HTTP_OPTIONS
          REQUIRE(resp == http::status_code::OK);
          REQUIRE(resp.value().empty() == true);                                      // it has no content
          REQUIRE(resp.to_string().find("\r\nAllow: GET\r\n") != std::string::npos);  // the 'allow' header was found
        #else
          REQUIRE(resp == http::status_code::MethodNotAllowed);
        #endif
    }

}


TEST_CASE("cert_authority: check /mgmt/keys with malformed parameters", "[core] [cert_authority]") {

    MockDBase mdb{ };
    mdb.table("ca_trusted_key", false, { "id", "public_key", "hash", "description", "valid_after", "valid_before", "created_at", " updated_at" }, {
        {  7, "publicKey7", "hash7", "description7", "1981-05-22 10:10:10", "5981-05-22 10:10:10", "1981-05-22 10:10:10", "1991-01-03 10:10:10" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    SECTION("With null page but defined size") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?item_per_page=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("With null size but defined page") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?page=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
        REQUIRE(JsonCompareErrorResponse(resp.value(), "BAD_PAYLOAD") == true);
    }

    SECTION("With unknown parameter") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?freddy=5", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }

    SECTION("With invalid direction") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?direction=top", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }

    SECTION("With invalid sort field") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?sort_field=mushroom", "" });

        REQUIRE(resp == http::status_code::BadRequest);
    }
}


TEST_CASE("cert_authority: check /mgmt/keys with database error", "[core] [cert_authority]") {

    db::DatabasePool<MockDBase> pool{ "127.0.0.1", "root", "root", "arrowhead" };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<db::DatabasePool<MockDBase>, MockCurl> certAuthority{ pool, reqBuilder };

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys", "" });
    REQUIRE(resp == http::status_code::InternalServerError);
}
