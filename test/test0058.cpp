////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      urlparser
/// Date:      2021-04-06
/// Author(s): ng201
///
/// Description:
/// * Test UrlParser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "http/UrlParser.h"


TEST_CASE("urlparser: Check parsing URL without query parameter", "[urlparser]") {

    {
        const auto url = std::string{ "/echo"  };
        REQUIRE(http::UrlParser{ url }.getPath() == "/echo");
    }

    {
        const auto url = std::string{ "/e%2Bcho" };
        REQUIRE(http::UrlParser{ url }.getPath() == "/e+cho");
    }

    {
        const auto url = std::string{ "/echo"  };
        auto parser = http::UrlParser{ url };

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%2Bcho" };
        auto parser = http::UrlParser{ url };

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }
}


TEST_CASE("urlparser: Check parsing URL with one query parameter", "[urlparser]") {

    {
        const auto url = std::string{ "/echo?theKey=theValue"  };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == true);

        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey");
            REQUIRE(kv.second == "theValue");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%2Bcho?theKey=theValue" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e+cho");
        REQUIRE(static_cast<bool>(parser) == true);

        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey");
            REQUIRE(kv.second == "theValue");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%2Bcho?theKey=The%20Value" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e+cho");
        REQUIRE(static_cast<bool>(parser) == true);

        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey");
            REQUIRE(kv.second == "The Value");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%2Bcho?theKey=The+Value" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e+cho");
        REQUIRE(static_cast<bool>(parser) == true);

        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey");
            REQUIRE(kv.second == "The Value");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }


    {
        const auto url = std::string{ "/echo?theKey="  };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == true);

        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey");
            REQUIRE(kv.second == "");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/echo?theKey"  };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == false);
    }

    {
        const auto url = std::string{ "/echo?"  };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == false);
    }
}


TEST_CASE("urlparser: Check parsing URL with multiple valid query parameters", "[urlparser]") {

    {
        const auto url = std::string{ "/echo?theKey1=theValue1&theKey2=theValue2&theKey3=theValue3" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue1");
        }
        ++parser;

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey2");
            REQUIRE(kv.second == "theValue2");
        }
        ++parser;

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey3");
            REQUIRE(kv.second == "theValue3");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%40cho?theKey1=theValue1&theKey2=theValue2" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e@cho");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue1");
        }
        ++parser;

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey2");
            REQUIRE(kv.second == "theValue2");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto url = std::string{ "/e%40cho?theKey1=theValue%3F1&theKey2=The+Value%3F2" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e@cho");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue?1");
        }
        ++parser;

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey2");
            REQUIRE(kv.second == "The Value?2");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

}


TEST_CASE("urlparser: Check parsing URL with multiple malformed query parameters", "[urlparser]") {

    {
        const auto url = std::string{ "/echo?theKey1=theValue1&theKey2" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/echo");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue1");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == false);
    }

    {
        const auto url = std::string{ "/e%40cho?theKey1=theValue1&theKey2" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e@cho");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue1");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == false);
    }

    {
        const auto url = std::string{ "/e%40cho?theKey1=theValue%3F1&theKey2The+Value%3F2" };
        auto parser = http::UrlParser{ url };

        REQUIRE(parser.getPath() == "/e@cho");
        REQUIRE(static_cast<bool>(parser) == true);

        {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "theKey1");
            REQUIRE(kv.second == "theValue?1");
        }
        ++parser;

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == false);
    }
}
