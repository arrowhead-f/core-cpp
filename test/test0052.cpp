////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      KeyProvider
/// Date:      2020-08-27
/// Author(s): ng201
///
/// Description:
/// * [KeyProvider] - Testing all the functionality of the KeyProvder.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>

#include "http/KeyProvider.h"


//TEST_CASE("KeyProvider: Check opening notexising file", "[KeyProvider]") {
//
//    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.tqt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.txt" });
//    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.tqt", "PEM", "12345", "data/test0052/tempsensor.txt" });
//    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.tqt" });
//
//}


//TEST_CASE("KeyProvider: Check openning existing file", "[KeyProvider]") {
//
//    CHECK_NOTHROW(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.txt" });
//
//}


TEST_CASE("KeyProvider: Is valid", "[KeyProvider]") {

    {
        auto kp = KeyProvider{};
        REQUIRE(kp == false);
    }

    {
        auto kp = KeyProvider{};
        kp.loadKeyStore("data/test0052", "tempsensor", "passwd");
        REQUIRE(kp == true);
    }

}


TEST_CASE("KeyProvider: Check paths stored", "[KeyProvider]") {

    auto kp = KeyProvider{};
    kp.loadKeyStore("data/test0052", "tempsensor", "passwd");

    REQUIRE(kp.keyStore.getCert() == "data/test0052/tempsensor.pem");
    REQUIRE(kp.keyStore.getKey()  == "data/test0052/tempsensor.key");
    REQUIRE(kp.keyStore.password  == "passwd");

    kp.loadTrustStore("data/test0052", "12345", true);

    REQUIRE(kp.trustStore.getCert() == "data/test0052/ca.pem");
    REQUIRE(kp.trustStore.password  == "12345");

}
