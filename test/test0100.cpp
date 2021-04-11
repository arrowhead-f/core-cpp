////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      core, core_utils
/// Date:      2021-04-05
/// Author(s): ng201
///
/// Description:
/// * this will test CoreUtils
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <string>

#include "core/CoreUtils.h"

#include "hlpr/JsonComp.h"


TEST_CASE("ErrorResponse: check format", "[core] [core_utils]") {

    const auto resp = ErrorResponse::from_stock(http::status_code::BadRequest, ErrorResponse::ARROWHEAD, "An error message", "/origin");

    #ifndef ARROWHEAD_FEAT_NO_ERROR_MSG
      const auto json = R"json({"errorMessage": "An error message", "errorCode": 400, "exceptionType": "ARROWHEAD", "origin": "/origin"})json";
    #else
      const auto json = R"json({"errorMessage": "", "errorCode": 400, "exceptionType": "ARROWHEAD", "origin": "/origin"})json";
    #endif

    REQUIRE(resp == http::status_code::BadRequest);
    REQUIRE(JsonCompare(resp.value(), json) == true);
}


TEST_CASE("CoreException: check format", "[core] [core_utils]") {

    try {
        throw CoreException{ http::status_code::NotFound, ErrorResponse::INVALID_PARAMETER, "An error message", "/origin/2" };
    }
    catch(const CoreException &ex) {

        const auto resp = ex.toResponse();
        #ifndef ARROWHEAD_FEAT_NO_ERROR_MSG
          const auto json = R"json({"errorMessage": "An error message", "errorCode": 404, "exceptionType": "INVALID_PARAMETER", "origin": "/origin/2"})json";
        #else
          const auto json = R"json({"errorMessage": "", "errorCode": 404, "exceptionType": "INVALID_PARAMETER", "origin": "/origin/2"})json";
        #endif

        REQUIRE(resp == http::status_code::NotFound);
        REQUIRE(JsonCompare(resp.value(), json) == true);
    }
}
