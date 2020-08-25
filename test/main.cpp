#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "Catch2/catch.hpp"


#include "net/HTTPHandler.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE("Catch2 init test", "[catch2]") {
    REQUIRE(Factorial(1)  == 1 );
    REQUIRE(Factorial(2)  == 2 );
    REQUIRE(Factorial(3)  == 6 );
    REQUIRE(Factorial(10) == 3628800 );
}


TEST_CASE("HTTPHandler: Options Parser I.", "[HTTPHandler]") {

    auto res = HTTPHandlerBase::parseOptions("key1=val1 key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}

TEST_CASE("HTTPHandler: Options Parser II.", "[HTTPHandler]") {

    auto res = HTTPHandlerBase::parseOptions("key1 =    val1     key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}

TEST_CASE("HTTPHandler: Options Parser III.", "[HTTPHandler]") {

    auto res = HTTPHandlerBase::parseOptions("     key1 =   val1     key2=val2 key3=val3");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == "val3");
}

TEST_CASE("HTTPHandler: Options Parser IV.", "[HTTPHandler]") {

    auto res = HTTPHandlerBase::parseOptions("key1 = 'val1  val1'    key2='val2' key3=' val3'");

    REQUIRE(res.size() == 3);

    REQUIRE(res["key1"] == "val1  val1");
    REQUIRE(res["key2"] == "val2");
    REQUIRE(res["key3"] == " val3");
}
