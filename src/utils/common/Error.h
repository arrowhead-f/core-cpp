#ifndef _UTILS_ERROR_H_
#define _UTILS_ERROR_H_

#include <string>
#include "CommonJsonBuilder.h"

#include "http/crate/Response.h"

/*
enum ExceptionType {
	ARROWHEAD,
	AUTH,
	BAD_PAYLOAD,
	INVALID_PARAMETER,
	DATA_NOT_FOUND,
	TIMEOUT,
	GENERIC,
	UNAVAILABLE
}
*/

class ErrorResp {

private:
    std::string errorMsg;
    int statusCode;
    std::string type; //ExceptionType
    std::string origin;

public:
    ErrorResp(std::string errorMsg, int statusCode = 400, std::string type = "BAD_PAYLOAD", std::string origin = "serviceregistry/register") :
        errorMsg{ std::move(errorMsg) },
        statusCode{ statusCode},
        type{ std::move(type) },
        origin{ std::move(origin) } {}

    Response getResp()
    {
        CommonJsonBuilder jResp;
        jResp.addStr("errorMessage", errorMsg);
        jResp.addInt("errorCode", statusCode);
        jResp.addStr("exceptionType", type);
        jResp.addStr("origin", origin);
        return Response{ http::status_code(statusCode), jResp.str() };
    }

};

#endif
