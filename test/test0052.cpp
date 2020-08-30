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

#include <Catch2/catch.hpp>

#include "net/KeyProvider.h"


TEST_CASE("KeyProvider: Check opening notexising file", "[KeyProvider]") {

    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.tqt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.txt" });
    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.tqt", "PEM", "12345", "data/test0052/tempsensor.txt" });
    CHECK_THROWS(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.tqt" });

}


TEST_CASE("KeyProvider: Check openning existing file", "[KeyProvider]") {

    CHECK_NOTHROW(KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.txt" });

}


TEST_CASE("KeyProvider: Is valid", "[KeyProvider]") {

    {
        auto kp = KeyProvider{};
        REQUIRE(kp == false);
    }

    {
        auto kp = KeyProvider{ "data/test0052/tempsensor.txt", "AAA", "data/test0052/tempsensor.txt", "BBB", "12345", "data/test0052/tempsensor.txt" };
        REQUIRE(kp == true);
    }

}


TEST_CASE("KeyProvider: Check paths stored", "[KeyProvider]") {

    auto kp = KeyProvider{ "data/test0052/tempsensor.txt", "AAA", "data/test0052/tempsensor.txt", "BBB", "12345", "data/test0052/tempsensor.txt" };

    REQUIRE(kp.sslCert == "data/test0052/tempsensor.txt");
    REQUIRE(kp.sslKey  == "data/test0052/tempsensor.txt");
    REQUIRE(kp.caInfo  == "data/test0052/tempsensor.txt");

    REQUIRE(kp.keyPasswd   == "12345");
    REQUIRE(kp.sslKeyType  == "BBB");
    REQUIRE(kp.sslCertType == "AAA");

}


TEST_CASE("KeyProvider: Check file content read", "[KeyProvider]") {

    auto kp = KeyProvider{ "data/test0052/tempsensor.txt", "PEM", "data/test0052/tempsensor.txt", "PEM", "12345", "data/test0052/tempsensor.txt" };

    REQUIRE(kp.sslCertData == "12345\n54327");
    REQUIRE(kp.sslKeyData  == "12345\n54327");
    REQUIRE(kp.caInfoData  == "12345\n54327");

}
