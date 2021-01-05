////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, cert_authority, query
/// Date:      2020-12-07
/// Author(s): ng201
///
/// Description:
/// * This will test CertAuthority's CAQueries class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "core/CertAuthority/CAQueries.h"

#include "hlpr/MockDBase.h"


TEST_CASE("caqueries: getTrustedKey", "[core] [cert_authority] [query]") {

    MockDBase mdb{ };

    mdb.table("ca_trusted_key", false, { "id", "created_at", "description", "hash" }, {
        {2,  "2020-12-01", "Dr. Evil II.", "1212121212121212121212121212121212121212121212121212121212121212"},
        {7,  "2020-12-06", "St. Nicholas", "cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45"},
        {13, "2020-12-06", "Who Cares I.", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"},
    });

    // create query object
    CAQueries<MockDBase> caq{ mdb };

    {
        auto row = caq.getTrustedKey("cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45");

        REQUIRE(row);

        int i;
        row->get(0, i);
        REQUIRE(i == 7);

        std::string d;
        row->get(1, d);
        REQUIRE(d == "2020-12-06");

        std::string s;
        row->get(2, s);
        REQUIRE(s == "St. Nicholas");

        REQUIRE(!row->next());
    }

    {
        auto row = caq.getTrustedKey("b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52");
        REQUIRE(!row);
    }
}


TEST_CASE("caqueries: getCertificate", "[core] [cert_authority] [query]") {
/*
    MockFetch mf;
    mf.addResponse("SELECT id, created_at, description FROM ca_trusted_key WHERE hash = 'cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45'", { MF::LINE(7, "2020-12-06", "St. Nicholas") });
    mf.addResponse("SELECT id, created_at, description FROM ca_trusted_key WHERE hash = 'b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52'");

    // create query object
    CAQueries<MockFetch> caq{ mf };

    {
        auto row = caq.getTrustedKey("cf8f9fb3da1c86745f616b8a7d6a47681179c042f4897741c3febe207ce50d45");

        REQUIRE(row);

        int i;
        row->get(0, i);
        REQUIRE(i == 7);

        std::string d;
        row->get(1, d);
        REQUIRE(d == "2020-12-06");

        std::string s;
        row->get(2, s);
        REQUIRE(s == "St. Nicholas");

        REQUIRE(!row->next());
    }

    {
        auto row = caq.getTrustedKey("b8033022e29297b50fd647b6a1c34e6595fff15cd1fe9f1bb8e18fbf5306fa52");
        REQUIRE(!row);
    }
*/
}
