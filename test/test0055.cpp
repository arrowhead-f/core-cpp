////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      crypto, sha256
/// Date:      2020-11-29
/// Author(s): ng201
///
/// Description:
/// * Test sha256 generation
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "utils/crypto/sha256.h"


TEST_CASE("sha256: Create sha256", "[crypto] [sha256]") {

    {
        const std::string data = "";
        std::string res;
        const auto q = crypto::sha256(data, res);

        REQUIRE(q   == true);
        REQUIRE(res == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    }
    {
        const std::string data = "12345";
        std::string res;
        const auto q = crypto::sha256(data, res);

        REQUIRE(q   == true);
        REQUIRE(res == "5994471abb01112afcc18159f6cc74b4f511b99806da59b3caf5a9c173cacfc5");
    }
    {
        const std::string data = "abcXYZ";
        std::string res;
        const auto q = crypto::sha256(data, res);

        REQUIRE(q   == true);
        REQUIRE(res == "17bdad942c360e9f4323959e9095d92bc1a4bb7ab1ab95bcfa0eae347baaf12a");
    }
    {
        const std::string data = "qwirwoirjwofksdnflksdnfklsdnlksndlvgknsdlkvnslknvslkdnvlkn";
        std::string res;
        const auto q = crypto::sha256(data, res);

        REQUIRE(q   == true);
        REQUIRE(res == "0d14f307c21945a8a83cdd5f031fad81a572651fea7acf9ea605722fa8a057d4");
    }
    {
        const std::string data = R"(
            Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.

            Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
            Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.)";

        std::string res;
        const auto q = crypto::sha256(data, res);

        REQUIRE(q   == true);
        REQUIRE(res == "3aad9bbfe98cfe68c0a75882b6d143da3840b8f7fe8b26ba99fd654d38e1bdf1");
    }
}
