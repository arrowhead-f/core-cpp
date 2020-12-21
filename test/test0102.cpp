////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, cert_authority
/// Date:      2020-12-06
/// Author(s): ng201
///
/// Description:
/// * This will test Certificate Authority; happy path
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/CertAuthority/CertAuthority.h"

#include "hlpr/JsonComp.h"
#include "hlpr/MockCurl.h"
#include "hlpr/MockDBase.h"


TEST_CASE("cert_authority: GET /echo", "[core] [cert_authority]") {

    MockDBase mdb;
    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    std::string response;
    const auto ret = certAuthority.dispatch("127.0.0.1", "GET", "/echo", response, "");

    REQUIRE(ret == 0);
    REQUIRE(response.empty());
}


TEST_CASE("cert_authority: POST /checkTrustedKey", "[core] [cert_authority]") {

    MockDBase mdb{ };
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
    // WrongKey ---> b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52

    SECTION("Database entry found") {
        const std::string payload = R"json({ "publicKey": "TheKey" })json";
        std::string response;
        const auto ret = certAuthority.dispatch("127.0.0.1", "POST", "/checkTrustedKey", response, payload);

        REQUIRE(ret == 0);
        REQUIRE(JsonCompare(response, R"json({"id": 1,"createdAt": "2020-12-06","description": "St. Nicholas"})json"));
    }

    SECTION("No database entry found") {
        const std::string payload = R"json({ "publicKey": "WrongKey" })json";
        std::string response;
        const auto ret = certAuthority.dispatch("127.0.0.1", "POST", "/checkTrustedKey", response, payload);

        REQUIRE(ret == 0);
        REQUIRE(response.empty());
    }

}


TEST_CASE("cert_authority: POST /sign", "[core] [cert_authority]") {



/*
{
  "encodedCSR": "string",
  "validAfter": "string",
  "validBefore": "string"
}

encodedCSR	PKCS #10 Certificate Signing Request	mandatory	Base64 encoded CSR
validAfter	Beginning of Certificate validity	optional	ISO 8601 date/time string
validBefore	End of Certificate validity	optional	ISO 8601 date/time string


{
  "id": "integer",
  "certificateChain": [
    "<generated client certificate>",
    "<cloud certificate>",
    "<root certificate>"
  ]
}

id	ID of the newly generated Certificate in the database
certificateChain	The whole certificate chain in an array of Base64 encoded DER strings (PEM without headers)
*/
}


TEST_CASE("cert_authority: POST /checkCertificate", "[core] [cert_authority]") {

//    MockFetch mf;
//    mf.addResponse("SELECT id, created_at, description FROM ca_trusted_key WHERE hash = 'cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45'", { MF::LINE(1, "2020-12-06", "St. Nicholas") });    mf.addResponse("SELECT id, created_at, description FROM ca_trusted_key WHERE hash = 'b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52'");

//    MockPool pool{ mf };
//    MockCurl reqBuilder;

    // create core system element
//    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    // sha256
    // TheKey   ---> cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45
    // WrongKey ---> b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52

//    SECTION("Database entry found") {
//        const std::string payload = R"json({"version": 1, "certificate": "TheKey" })json";
//        std::string response;
//        const auto ret = certAuthority.dispatch("127.0.0.1", "POST", "/checkTrustedKey", response, payload);

/*
{"version": 1,
 "producedAt": "string",
 "endOfValidity": "string",
 "commonName": "string",
 "serialNumber": "string",
 "status": "string" // good, revoked, expired, unknown
}
*/
//        REQUIRE(ret == 0);
//        REQUIRE(JsonCompare(response, R"json({"id": 1,"createdAt": "2020-12-06","description": "St. Nicholas"})json"));
//    }

//    SECTION("No database entry found") {
//        const std::string payload = R"json({"version": 1, "certificate": "WrongKey" })json";
//        std::string response;
//        const auto ret = certAuthority.dispatch("127.0.0.1", "POST", "/checkTrustedKey", response, payload);
//
//        REQUIRE(ret == 0);
//        REQUIRE(response.empty());
//    }

}


TEST_CASE("cert_authority: GET /mgmt/certificates", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: DELETE /mgmt/certificates/{id}", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: GET /mgmt/keys", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: PUT /mgmt/keys", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: DELETE /mgmt/keys/{id}", "[core] [cert_authority]") {
}
