////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, cert_authority
/// Date:      2020-12-06
/// Author(s): ng201
///
/// Description:
/// * this will test Certificate Authority
/// * happier paths
///   * the format of the json found in the 
///     request is ok
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <ctime>
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

    const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/echo", "" });

    REQUIRE(resp == http::status_code::OK);
    REQUIRE(resp.value()  == "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>");
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
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", payload });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), R"json({"id": 1,"createdAt": "2020-12-06","description": "St. Nicholas"})json"));
    }

    SECTION("No database entry found") {
        const std::string payload = R"json({ "publicKey": "WrongKey" })json";
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkTrustedKey", payload });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(resp.value().empty());
    }
}

/*
TEST_CASE("cert_authority: POST /checkCertificate", "[core] [cert_authority]") {

    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer[80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer,80,"%Y-%m-%d %H:%M:%S",timeinfo);


    MockDBase mdb{ };
    mdb.table("ca_certificate", false, { "id", "common_name", "serial", "created_by", "valid_after", "valid_before", "created_at", "updated_at", "revoked_at" }, {
        {  7, "sysname-7777", "7777777777", "test", "1989-09-09 10:10:10", "9999-09-09 11:11:11", "1989-09-09 11:11:11", "1989-10-10 11:11:11", nullptr },
        { 11, "sysname-good", "1111111111", "test", "1989-09-09 10:10:10", "5555-05-05 11:11:11", "1989-09-09 11:11:11", "1989-10-10 11:11:11", nullptr },
        { 12, "sysname-revo", "2222222222", "test", "1989-09-09 10:10:10", "5555-05-05 11:11:11", "1989-09-09 11:11:11", "1989-10-10 11:11:11", buffer  },  // this one is revoked
        { 13, "sysname-exp1", "3333333333", "test", "9999-09-09 10:10:10", "9999-10-10 11:11:11", "9999-09-09 09:09:09", "9999-10-10 11:11:11", nullptr },  // not valid yet
        { 14, "sysname-exp2", "3333333333", "test", "1989-09-09 10:10:10", buffer,                "1989-09-09 09:09:09", "1989-10-10 11:11:11", nullptr },  // va;idity edned already
        { 15, "sysname-unkn", "4444444444", "test", "1989-09-09 10:10:10", "5555-05-05 11:11:11", "1989-09-09 11:11:11", "1989-10-10 11:11:11", nullptr }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    SECTION("Database entry not found") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-0\"}" });

        REQUIRE(resp == http::status_code::OK);
    }

    SECTION("Database entry found - good") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-good\"}" });

        REQUIRE(resp == http::status_code::OK);
    }

    SECTION("Database entry found - revoked") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-revo\"}" });

        REQUIRE(resp == http::status_code::OK);
    }

    SECTION("Database entry found - expired") {
        {
            // validity period does not started yet
            const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-exp1\"}" });
            REQUIRE(resp == http::status_code::OK);
        }

        {
            // validity period already ended
            const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-exp2\"}" });
            REQUIRE(resp == http::status_code::OK);
        }
    }

    SECTION("Database entry found - unknown") {
        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "POST", "/checkCertificate", "{version:1,certificate:\"sysname-unkn\"}" });

            REQUIRE(resp == http::status_code::OK);
    //    assertEquals(request.getVersion(), responseBody.getVersion());
    }
}
*/


/*
TEST_CASE("cert_authority: POST /sign", "[core] [cert_authority]") {

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

}
*/


TEST_CASE("cert_authority: GET /mgmt/certificates", "[core] [cert_authority]") {

    MockDBase mdb{ };
    mdb.table("ca_certificate", false, { "id", "common_name", "serial", "created_by", "created_at", "valid_after", "valid_before", "updated_at", "revoked_at" }, {
        { 77, "sysname-1", "0707070707", "test1", "1981-05-22 10:10:10", "1981-05-22 10:10:10", "5000-07-09 10:10:10", "1981-05-22 10:10:10", nullptr },
        { 11, "sysname-2", "1111111111", "test2", "1991-01-03 10:10:10", "1991-01-03 10:10:10", "2003-09-09 11:11:11", "2001-01-03 09:09:09", "2001-01-03 09:09:09"  },
        { 12, "sysname-3", "1212121212", "test3", "1981-02-09 10:10:10", "2001-03-09 10:10:10", "5028-09-09 11:11:11", "2002-03-03 10:11:12", "2003-03-03 10:11:12"  },
        { 13, "sysname-4", "1313131313", "test4", "1991-03-09 10:10:10", "1992-03-09 10:10:10", "5027-09-09 09:09:09", "1992-03-09 10:10:10", nullptr },
        { 14, "sysname-9", "1414141414", "test5", "2020-12-09 10:10:10", "3020-12-09 10:10:10", "6028-09-09 09:09:09", "2020-12-09 10:10:10", nullptr },
        { 25, "sysname-6", "2525252525", "test6", "1981-09-09 10:10:10", "2016-10-05 11:11:11", "2018-09-09 11:11:11", "2016-09-09 10:10:10", nullptr }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    SECTION("Should return all data sorted ASC by id") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 11, "commonName": "sysname-2", "serialNumber": "1111111111", "createdAt": "1991-01-03 10:10:10", "createdBy": "test2", "validFrom": "1991-01-03 10:10:10", "validUntil": "2003-09-09 11:11:11", "revokedAt": "2001-01-03 09:09:09", "status": "revoked"},
                {"id": 12, "commonName": "sysname-3", "serialNumber": "1212121212", "createdAt": "1981-02-09 10:10:10", "createdBy": "test3", "validFrom": "2001-03-09 10:10:10", "validUntil": "5028-09-09 11:11:11", "revokedAt": "2003-03-03 10:11:12", "status": "revoked"},
                {"id": 13, "commonName": "sysname-4", "serialNumber": "1313131313", "createdAt": "1991-03-09 10:10:10", "createdBy": "test4", "validFrom": "1992-03-09 10:10:10", "validUntil": "5027-09-09 09:09:09", "revokedAt": null, "status": "good"},
                {"id": 14, "commonName": "sysname-9", "serialNumber": "1414141414", "createdAt": "2020-12-09 10:10:10", "createdBy": "test5", "validFrom": "3020-12-09 10:10:10", "validUntil": "6028-09-09 09:09:09", "revokedAt": null, "status": "expired"},
                {"id": 25, "commonName": "sysname-6", "serialNumber": "2525252525", "createdAt": "1981-09-09 10:10:10", "createdBy": "test6", "validFrom": "2016-10-05 11:11:11", "validUntil": "2018-09-09 11:11:11", "revokedAt": null, "status": "expired"},
                {"id": 77, "commonName": "sysname-1", "serialNumber": "0707070707", "createdAt": "1981-05-22 10:10:10", "createdBy": "test1", "validFrom": "1981-05-22 10:10:10", "validUntil": "5000-07-09 10:10:10", "revokedAt": null, "status": "good"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return all data sorted DESC by id") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 77, "commonName": "sysname-1", "serialNumber": "0707070707", "createdAt": "1981-05-22 10:10:10", "createdBy": "test1", "validFrom": "1981-05-22 10:10:10", "validUntil": "5000-07-09 10:10:10", "revokedAt": null, "status": "good"},
                {"id": 25, "commonName": "sysname-6", "serialNumber": "2525252525", "createdAt": "1981-09-09 10:10:10", "createdBy": "test6", "validFrom": "2016-10-05 11:11:11", "validUntil": "2018-09-09 11:11:11", "revokedAt": null, "status": "expired"},
                {"id": 14, "commonName": "sysname-9", "serialNumber": "1414141414", "createdAt": "2020-12-09 10:10:10", "createdBy": "test5", "validFrom": "3020-12-09 10:10:10", "validUntil": "6028-09-09 09:09:09", "revokedAt": null, "status": "expired"},
                {"id": 13, "commonName": "sysname-4", "serialNumber": "1313131313", "createdAt": "1991-03-09 10:10:10", "createdBy": "test4", "validFrom": "1992-03-09 10:10:10", "validUntil": "5027-09-09 09:09:09", "revokedAt": null, "status": "good"},
                {"id": 12, "commonName": "sysname-3", "serialNumber": "1212121212", "createdAt": "1981-02-09 10:10:10", "createdBy": "test3", "validFrom": "2001-03-09 10:10:10", "validUntil": "5028-09-09 11:11:11", "revokedAt": "2003-03-03 10:11:12", "status": "revoked"},
                {"id": 11, "commonName": "sysname-2", "serialNumber": "1111111111", "createdAt": "1991-01-03 10:10:10", "createdBy": "test2", "validFrom": "1991-01-03 10:10:10", "validUntil": "2003-09-09 11:11:11", "revokedAt": "2001-01-03 09:09:09", "status": "revoked"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?direction=DESC", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 0th page of 2 items") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 11, "commonName": "sysname-2", "serialNumber": "1111111111", "createdAt": "1991-01-03 10:10:10", "createdBy": "test2", "validFrom": "1991-01-03 10:10:10", "validUntil": "2003-09-09 11:11:11", "revokedAt": "2001-01-03 09:09:09", "status": "revoked"},
                {"id": 12, "commonName": "sysname-3", "serialNumber": "1212121212", "createdAt": "1981-02-09 10:10:10", "createdBy": "test3", "validFrom": "2001-03-09 10:10:10", "validUntil": "5028-09-09 11:11:11", "revokedAt": "2003-03-03 10:11:12", "status": "revoked"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?page=0&item_per_page=2", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 1th page of 4 items") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 25, "commonName": "sysname-6", "serialNumber": "2525252525", "createdAt": "1981-09-09 10:10:10", "createdBy": "test6", "validFrom": "2016-10-05 11:11:11", "validUntil": "2018-09-09 11:11:11", "revokedAt": null, "status": "expired"},
                {"id": 77, "commonName": "sysname-1", "serialNumber": "0707070707", "createdAt": "1981-05-22 10:10:10", "createdBy": "test1", "validFrom": "1981-05-22 10:10:10", "validUntil": "5000-07-09 10:10:10", "revokedAt": null, "status": "good"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?page=1&item_per_page=4", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return all data sorted ASC by commonName") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 77, "commonName": "sysname-1", "serialNumber": "0707070707", "createdAt": "1981-05-22 10:10:10", "createdBy": "test1", "validFrom": "1981-05-22 10:10:10", "validUntil": "5000-07-09 10:10:10", "revokedAt": null, "status": "good"},
                {"id": 11, "commonName": "sysname-2", "serialNumber": "1111111111", "createdAt": "1991-01-03 10:10:10", "createdBy": "test2", "validFrom": "1991-01-03 10:10:10", "validUntil": "2003-09-09 11:11:11", "revokedAt": "2001-01-03 09:09:09", "status": "revoked"},
                {"id": 12, "commonName": "sysname-3", "serialNumber": "1212121212", "createdAt": "1981-02-09 10:10:10", "createdBy": "test3", "validFrom": "2001-03-09 10:10:10", "validUntil": "5028-09-09 11:11:11", "revokedAt": "2003-03-03 10:11:12", "status": "revoked"},
                {"id": 13, "commonName": "sysname-4", "serialNumber": "1313131313", "createdAt": "1991-03-09 10:10:10", "createdBy": "test4", "validFrom": "1992-03-09 10:10:10", "validUntil": "5027-09-09 09:09:09", "revokedAt": null, "status": "good"},
                {"id": 25, "commonName": "sysname-6", "serialNumber": "2525252525", "createdAt": "1981-09-09 10:10:10", "createdBy": "test6", "validFrom": "2016-10-05 11:11:11", "validUntil": "2018-09-09 11:11:11", "revokedAt": null, "status": "expired"},
                {"id": 14, "commonName": "sysname-9", "serialNumber": "1414141414", "createdAt": "2020-12-09 10:10:10", "createdBy": "test5", "validFrom": "3020-12-09 10:10:10", "validUntil": "6028-09-09 09:09:09", "revokedAt": null, "status": "expired"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?sort_field=commonName", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 1th page of 4 items ordered by serialName DESC") {
        const auto data = R"json({
            "count": 6,
            "issuedCertificates": [
                {"id": 11, "commonName": "sysname-2", "serialNumber": "1111111111", "createdAt": "1991-01-03 10:10:10", "createdBy": "test2", "validFrom": "1991-01-03 10:10:10", "validUntil": "2003-09-09 11:11:11", "revokedAt": "2001-01-03 09:09:09", "status": "revoked"},
                {"id": 77, "commonName": "sysname-1", "serialNumber": "0707070707", "createdAt": "1981-05-22 10:10:10", "createdBy": "test1", "validFrom": "1981-05-22 10:10:10", "validUntil": "5000-07-09 10:10:10", "revokedAt": null, "status": "good"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/certificates?page=1&item_per_page=4&direction=DESC&sort_field=serial", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

}


TEST_CASE("cert_authority: DELETE /mgmt/certificates/{id}", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: PUT /key", "[core] [cert_authority]") {
}


TEST_CASE("cert_authority: GET /mgmt/keys", "[core] [cert_authority]") {

    MockDBase mdb{ };
    mdb.table("ca_trusted_key", false, { "id", "public_key", "hash", "description", "valid_after", "valid_before", "created_at", " updated_at" }, {
        { 27, "publicKey-27", "hash-27", "description001", "1981-05-22 10:10:10", "5981-05-22 10:10:10", "1981-05-22 10:10:10", "1991-01-03 10:10:10" },
        { 52, "publicKey-52", "hash-52", "description002", "1981-06-22 10:10:10", "5981-06-22 10:10:10", "1981-06-22 10:10:10", "1991-02-03 10:10:10" },
        { 38, "publicKey-38", "hash-38", "description003", "1981-07-22 10:10:10", "5981-07-22 10:10:10", "1981-07-22 10:10:10", "1991-03-03 10:10:10" }
    });

    MockPool pool{ mdb };
    MockCurl reqBuilder;

    // create core system element
    CertAuthority<MockPool, MockCurl> certAuthority{ pool, reqBuilder };

    SECTION("Should return all data sorted ASC by id") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": [
                {"id": 27, "createdAt": "1981-05-22 10:10:10", "description": "description001"},
                {"id": 38, "createdAt": "1981-07-22 10:10:10", "description": "description003"},
                {"id": 52, "createdAt": "1981-06-22 10:10:10", "description": "description002"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return all data sorted DESC by id") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": [
                {"id": 52, "createdAt": "1981-06-22 10:10:10", "description": "description002"},
                {"id": 38, "createdAt": "1981-07-22 10:10:10", "description": "description003"},
                {"id": 27, "createdAt": "1981-05-22 10:10:10", "description": "description001"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?direction=DESC", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 0th page of 2 items") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": [
                {"id": 27, "createdAt": "1981-05-22 10:10:10", "description": "description001"},
                {"id": 38, "createdAt": "1981-07-22 10:10:10", "description": "description003"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?page=0&item_per_page=2", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 1th page of 4 items") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": []})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?page=1&item_per_page=4", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return all data sorted ASC by createdAt") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": [
                {"id": 27, "createdAt": "1981-05-22 10:10:10", "description": "description001"},
                {"id": 52, "createdAt": "1981-06-22 10:10:10", "description": "description002"},
                {"id": 38, "createdAt": "1981-07-22 10:10:10", "description": "description003"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?sort_field=createdAt", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

    SECTION("Should return 1th page of 2 items ordered by serialName DESC") {
        const auto data = R"json({
            "count": 3,
            "trustedKeys": [
                {"id": 27, "createdAt": "1981-05-22 10:10:10", "description": "description001"}]})json";

        const auto resp = certAuthority.dispatch(Request{ "127.0.0.1", "GET", "/mgmt/keys?page=1&item_per_page=2&direction=DESC&sort_field=createdAt", "" });

        REQUIRE(resp == http::status_code::OK);
        REQUIRE(JsonCompare(resp.value(), data) == true);
    }

}


TEST_CASE("cert_authority: DELETE /mgmt/keys/{id}", "[core] [cert_authority]") {
}
