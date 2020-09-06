////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      catch2
/// Date:      2020-08-20
/// Author(s): ng201
///
/// Description:
/// * [catch2] - Showcase of catch2 tests. Nothing special.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"


unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}


TEST_CASE("Check factorial's result", "[catch2]") {

    REQUIRE(Factorial(1)  == 1 );
    REQUIRE(Factorial(2)  == 2 );
    REQUIRE(Factorial(3)  == 6 );
    REQUIRE(Factorial(10) == 3628800 );

}


TEST_CASE("Factorial should not throw", "[catch2]") {

    CHECK_NOTHROW(Factorial(2));

}
