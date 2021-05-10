////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      uri, uriparser
/// Date:      2021-04-06
/// Author(s): ng201
///
/// Description:
/// * Test Uri
/// * Test Uri::Parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "http/crate/Uri.h"


TEST_CASE("uri: Uri::compare with const char*", "[uri]") {

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.compare("/meatloaf/badattitude") == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/badattitude" };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977" };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == false);
        CHECK(u.empty() == false);
    }

}


TEST_CASE("uri: Uri::compare with std::string", "[uri]") {

    using namespace std::literals;


    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.compare("/meatloaf/batoutofhell"s) == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.compare("/meatloaf/badattitude"s) == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/badattitude" };

        REQUIRE(u.compare("/meatloaf/batoutofhell"s) == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977" };

        REQUIRE(u.compare("/meatloaf/batoutofhell"s) == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977"s };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == false);
        CHECK(u.empty() == false);
    }

}



TEST_CASE("uri: Uri::consume with const char*", "[uri]") {

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.consume("/meatloaf/badattitude") == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/badattitude" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        std::string param;
        REQUIRE(u.pathParam(param) == true);
        REQUIRE(param == "date/1977");
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        unsigned long id;
        REQUIRE(u.pathId(id) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell/date") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == true);
        REQUIRE(id == 1977);
    }

}


TEST_CASE("uri: Uri::consume with std::string", "[uri]") {

    using namespace std::literals;

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.consume("/meatloaf/batoutofhell"s) == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        REQUIRE(u.consume("/meatloaf/badattitude"s) == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/badattitude" };

        REQUIRE(u.consume("/meatloaf/batoutofhell"s) == false);
        CHECK(u.empty() == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell"s) == true);
        CHECK(u.empty() == true);

        std::string param;
        REQUIRE(u.pathParam(param) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell"s) == true);
        CHECK(u.empty() == false);

        std::string param;
        REQUIRE(u.pathParam(param) == true);
        REQUIRE(param == "date/1977");
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell/date"s) == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == true);
        REQUIRE(id == 1977);
    }

}


TEST_CASE("uri: Check multiple compares/consumes after each other", "[uri]") {

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977/song/Paradise%20By%20The%20Dashboard%20Light" };

        REQUIRE(u.compare("/meatloaf/batoutofhell") == false);
        CHECK(u.empty() == false);

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        REQUIRE(u.compare("/date/1977") == false);
        CHECK(u.empty() == false);

        REQUIRE(u.consume("/date/1977") == true);
        CHECK(u.empty() == false);

        REQUIRE(u.compare("/song/Paradise%20By%20The%20Dashboard%20Light") == true);
        CHECK(u.empty() == true);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977/song/Paradise%20By%20The%20Dashboard%20Light" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        REQUIRE(u.consume("/date/1977") == true);
        CHECK(u.empty() == false);

        REQUIRE(u.consume("/song/Paradise%20By%20The%20Dashboard%20Light") == true);
        CHECK(u.empty() == true);
    }

}


TEST_CASE("uri: Test path id", "[uri]") {

    {
        auto u = Uri{ "/meatloaf/batoutofhell/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == true);
        REQUIRE(id == 1977);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/9.56" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/date/1977" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/1977/10" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == false);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell/05" };

        REQUIRE(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == false);

        unsigned long id;
        REQUIRE(u.pathId(id) == true);
    }

}


TEST_CASE("uri: Use / as uri", "[uri]") {

    {
        auto u = Uri{ "/" };

        REQUIRE(u.compare("") == false);
        REQUIRE(u.empty() == false);

        REQUIRE(u.compare("/") == true);
        REQUIRE(u.empty() == true);
    }

    {
        auto u = Uri{ "/" };

        REQUIRE(u.consume("") == true);
        REQUIRE(u.empty() == false);

        REQUIRE(u.consume("/") == true);
        REQUIRE(u.empty() == true);
    }
}



TEST_CASE("uri: Test compare empty uri", "[uri]") {

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        CHECK(u.compare("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        REQUIRE(u.compare("/") == false);
        REQUIRE(u.compare("/date") == false);
        REQUIRE(u.compare("?") == false);
        REQUIRE(u.compare("") == true);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977&song=Paradise%20By%20The%20Dashboard%20Light" };

        CHECK(u.compare("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        REQUIRE(u.compare("/") == false);
        REQUIRE(u.compare("/date") == false);
        REQUIRE(u.compare("?") == false);
        REQUIRE(u.compare("") == true);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell" };

        CHECK(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        REQUIRE(u.consume("/") == false);
        REQUIRE(u.consume("/date") == false);
        REQUIRE(u.consume("?") == false);
        REQUIRE(u.consume("") == true);
    }

    {
        auto u = Uri{ "/meatloaf/batoutofhell?date=1977&song=Paradise%20By%20The%20Dashboard%20Light" };

        CHECK(u.consume("/meatloaf/batoutofhell") == true);
        CHECK(u.empty() == true);

        REQUIRE(u.consume("/") == false);
        REQUIRE(u.consume("/date") == false);
        REQUIRE(u.consume("?") == false);
        REQUIRE(u.consume("") == true);
    }

}


TEST_CASE("uriparser: Check parsing URL without query parameter", "[uriparser]") {

    {
        const auto uri = Uri{ "/meatloaf"  };
        const auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

}


TEST_CASE("uriparser: Check parsing URL with one query parameter", "[uriparser]") {

    {
        const auto uri = Uri{ "/meatloaf?album=Deadringer"  };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == true);
        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "album");
            REQUIRE(kv.second == "Deadringer");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto uri = Uri{ "/Meat%20Loaf?album=Deadringer" };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == true);
        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "album");
            REQUIRE(kv.second == "Deadringer");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto uri = Uri{ "/Meat%20Loaf?album=Bat%20out%20of%20Hell" };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == true);
        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "album");
            REQUIRE(kv.second == "Bat out of Hell");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto uri = Uri{ "/Meat%20Loaf?album=Bat+out+of+Hell" };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == true);
        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "album");
            REQUIRE(kv.second == "Bat out of Hell");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }


    {
        const auto uri = Uri{ "/meatloaf?worstSong="  };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == true);
        while(parser) {
            auto &&kv = *parser;
            REQUIRE(kv.first  == "worstSong");
            REQUIRE(kv.second == "");
            ++parser;
        }

        REQUIRE(static_cast<bool>(parser) == false);
        REQUIRE(parser.check() == true);
    }

    {
        const auto uri = Uri{ "/meatloaf?worstSong"  };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == false);
    }

    {
        const auto uri = Uri{ "/meatloaf?"  };
        auto parser = Uri::Parser{ uri };

        REQUIRE(static_cast<bool>(parser) == false);
    }
}


TEST_CASE("uriparser: Check parsing URL with multiple valid query parameters", "[uriparser]") {

    {
        const auto url = Uri{ "/echo?theKey1=theValue1&theKey2=theValue2&theKey3=theValue3" };
        auto parser = Uri::Parser{ url };

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
        const auto url = Uri{ "/e%40cho?theKey1=theValue1&theKey2=theValue2" };
        auto parser = Uri::Parser{ url };

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
        const auto url = Uri{ "/e%40cho?theKey1=theValue%3F1&theKey2=The+Value%3F2" };
        auto parser = Uri::Parser{ url };

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


TEST_CASE("uriparser: Check parsing URL with multiple malformed query parameters", "[uriparser]") {

    {
        const auto url = Uri{ "/echo?theKey1=theValue1&theKey2" };
        auto parser = Uri::Parser{ url };

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
        const auto url = Uri{ "/e%40cho?theKey1=theValue1&theKey2" };
        auto parser = Uri::Parser{ url };

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
        const auto url = Uri{ "/e%40cho?theKey1=theValue%3F1&theKey2The+Value%3F2" };
        auto parser = Uri::Parser{ url };

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

