////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      inifile
/// Date:      2020-12-23
/// Author(s): ng201
///
/// Description:
/// * This one checks the INIFile reader/parser
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include "utils/inifile.h"


TEST_CASE("inifile: Test parsing of INI files without sections", "[inifile]") {

    {
        auto parser = INIFile{ "data/test0053/c001.ini" };

        REQUIRE(parser.parse() == true);
        REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel'");
    }

    {
        auto parser = INIFile{ "data/test0053/c003.ini" };

        REQUIRE(parser.parse() == true);
        REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel'");
    }

    {
        auto parser = INIFile{ "data/test0053/c002.ini" };

        REQUIRE(parser.parse() == true);

        auto &&res = parser.options("");

        REQUIRE(res.size() == 5);

        REQUIRE(res["keyboards"] == "Tony Banks");
        REQUIRE(res["bass"]      == "Mike Rutherford");
        REQUIRE(res["drums"]     == "Phil Collins");
        REQUIRE(res["guitars"]   == "Steve Hacket");
        REQUIRE(res["vocals"]    == "Peter Gabriel");
    }

}


TEST_CASE("inifile: Test opening nonexisting INI file", "[inifile]") {

    CHECK_THROWS([]() {
        auto parser = INIFile{ "data/test0053/c000000001.ini" };
    }());
}


TEST_CASE("inifile: Test loading/resetting", "[inifile]") {

    {
        auto parser = INIFile{ "data/test0053/c002.ini" };

        CHECK_THROWS([&parser]() {
            parser.load("data/test0053/c000000001.ini");
        }());
    }

    {
        auto parser = INIFile{ "data/test0053/c003.ini" };

        REQUIRE(parser.parse() == true);
        REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel'");

        parser.reset("data/test0053/c002.ini");

        REQUIRE(parser.parse() == true);
        REQUIRE(parser.to_string("") == "keyboards = 'Tony Banks' guitars   = 'Steve Hacket' drums     = 'Phil Collins' bass      = 'Mike Rutherford' vocals    = 'Peter Gabriel'");
    }
}



TEST_CASE("inifile: Test clear", "[inifile]") {

    auto parser = INIFile{ "data/test0053/c001.ini" };

    REQUIRE(parser.parse() == true);
    REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel'");

    parser.clear();

    REQUIRE(parser.to_string("") == "");
}


TEST_CASE("inifile: Test append/prepend custom config", "[inifile]") {

    auto parser = INIFile{ "data/test0053/c001.ini" };

    REQUIRE(parser.parse() == true);
    REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel'");

    {
        auto &&res = parser.options("");
        REQUIRE(res.size() == 5);

        REQUIRE(res["keyboards"] == "Tony Banks");
        REQUIRE(res["bass"]      == "Mike Rutherford");
        REQUIRE(res["drums"]     == "Phil Collins");
        REQUIRE(res["guitars"]   == "Steve Hacket");
        REQUIRE(res["vocals"]    == "Peter Gabriel");
    }

    parser.append("", "vocals='Phil Collins' drums='Bill Bruford'");
    REQUIRE(parser.to_string("") == "keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel' vocals='Phil Collins' drums='Bill Bruford'");

    {
        auto &&res = parser.options("");
        REQUIRE(res.size() == 5);

        REQUIRE(res["keyboards"] == "Tony Banks");
        REQUIRE(res["bass"]      == "Mike Rutherford");
        REQUIRE(res["drums"]     == "Bill Bruford");
        REQUIRE(res["guitars"]   == "Steve Hacket");
        REQUIRE(res["vocals"]    == "Phil Collins");
    }

    parser.prepend("", "guitars='Daryl Stuermer' drums='Chester Thompson'");
    REQUIRE(parser.to_string("") == "guitars='Daryl Stuermer' drums='Chester Thompson' keyboards='Tony Banks' guitars='Steve Hacket' drums='Phil Collins' bass='Mike Rutherford' vocals='Peter Gabriel' vocals='Phil Collins' drums='Bill Bruford'");

    {
        auto &&res = parser.options("");
        REQUIRE(res.size() == 5);

        REQUIRE(res["keyboards"] == "Tony Banks");
        REQUIRE(res["bass"]      == "Mike Rutherford");
        REQUIRE(res["drums"]     == "Bill Bruford");
        REQUIRE(res["guitars"]   == "Steve Hacket");
        REQUIRE(res["vocals"]    == "Phil Collins");
    }
}


TEST_CASE("inifile: Test parsing of INI files with sections", "[inifile]") {

    {
        auto parser = INIFile{ "data/test0053/c004.ini" };

        REQUIRE(parser.parse("Members") == true);

        REQUIRE(!parser.to_string("").empty());
        REQUIRE(!parser.to_string("Members").empty());
        REQUIRE(parser.to_string("Studio albums").empty());
    }

    {
        auto parser = INIFile{ "data/test0053/c004.ini" };

        REQUIRE(parser.parse("Members", true) == true);

        REQUIRE(parser.to_string("").empty());
        REQUIRE(!parser.to_string("Members").empty());
        REQUIRE(parser.to_string("Studio albums").empty());
    }

    {
        auto parser = INIFile{ "data/test0053/c004.ini" };

        REQUIRE(parser.parse() == true);

        REQUIRE(!parser.to_string("").empty());
        REQUIRE(!parser.to_string("Members").empty());
        REQUIRE(!parser.to_string("Studio albums").empty());
        REQUIRE(!parser.to_string("Box sets").empty());
    }

    {
        auto parser = INIFile{ "data/test0053/c004.ini" };

        REQUIRE(parser.parse("Members", true) == true);
        REQUIRE(parser.parse("Studio albums", true) == true);

        REQUIRE(parser.to_string("").empty());
        REQUIRE(!parser.to_string("Members").empty());
        REQUIRE(!parser.to_string("Studio albums").empty());
        REQUIRE(parser.to_string("Box sets").empty());
    }
}
