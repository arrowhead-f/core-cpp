#include "Response.h"


#include <sstream>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif


namespace status_line {

    #if __cplusplus > 201402L
      constexpr
    #endif
    inline const char* to_string(http::status_code sc) {
        switch (static_cast<int>(sc)) {

            case 200: return "200 OK";
            case 201: return "201 Created";
            case 202: return "202 Accepted";
            case 400: return "400 Bad Request";
            case 401: return "401 Unauthorized";
            case 403: return "403 Forbidden";
            case 404: return "404 Not Found";
            case 405: return "405 Method Not Allowed";
            case 411: return "411 Length Required";
            case 500: return "500 Internal Server Error";
            case 501: return "501 Not Implemented";

            #ifndef STRIP_LITERALS
              case 100: return "100 Continue";
              case 101: return "101 Switching Protocols";
              case 203: return "203 Non-Authoritative Information";
              case 204: return "204 No Content";
              case 205: return "205 Reset Content";
              case 206: return "206 Partial Content";
              case 300: return "300 Multiple Choices";
              case 301: return "301 Moved Permanently";
              case 302: return "302 Found";
              case 303: return "303 See Other";
              case 304: return "304 Not Modified";
              case 305: return "305 Use Proxy";
              case 307: return "307 Temporary Redirect";
              case 402: return "402 Payment Required";
              case 406: return "406 Not Acceptable";
              case 407: return "407 Proxy Authentication Required";
              case 408: return "408 Request Time-out";
              case 409: return "409 Conflict";
              case 410: return "410 Gone";
              case 412: return "412 Precondition Failed";
              case 413: return "413 Request Entity Too Large";
              case 414: return "414 Request-URI Too Large";
              case 415: return "415 Unsupported Media Type";
              case 416: return "416 Requested range not satisfiable";
              case 417: return "417 Expectation Failed";
              case 502: return "502 Bad Gateway";
              case 503: return "503 Service Unavailable";
              case 504: return "504 Gateway Time-out";
              case 505: return "505 Version not supported";
            #else
            #endif
        }

        #ifdef __GNUG__
          return "501 Not Implemented";
        #else
          throw std::logic_error{ "Wrong status code given." };
        #endif
    }

}


std::string Response::to_string() const {

    std::ostringstream out;

    // build the string representation of the Response
    out << "HTTP/1.1 " << status_line::to_string(status_code)
        << "\r\n"
        << "Content-Length: " << std::to_string(content.size())
        << "\r\n"
        << "Content-Type: "   << mime_type
        << "\r\n"
        << "\r\n"
        << content;

    return out.str();

}


namespace stock_replies {

    #if __cplusplus > 201402L
      constexpr
    #endif
    inline const char* to_string(http::status_code sc) {

        switch(static_cast<int>(sc)) {

            case 200: return "<html><head><title>Arrowhead</title></head><body><h1>200 OK</h1></body></html>";
            case 201: return "<html><head><title>Arrowhead</title></head><body><h1>201 Created</h1></body></html>";
            case 202: return "<html><head><title>Arrowhead</title></head><body><h1>202 Accepted</h1></body></html>";
            case 400: return "<html><head><title>Arrowhead</title></head><body><h1>400 Bad Request</h1></body></html>";
            case 401: return "<html><head><title>Arrowhead</title></head><body><h1>401 Unauthorized</h1></body></html>";
            case 403: return "<html><head><title>Arrowhead</title></head><body><h1>403 Forbidden</h1></body></html>";
            case 404: return "<html><head><title>Arrowhead</title></head><body><h1>404 Not Found</h1></body></html>";
            case 405: return "<html><head><title>Arrowhead</title></head><body><h1>405 Method Not Allowed</h1></body></html>";
            case 411: return "<html><head><title>Arrowhead</title></head><body><h1>411 Length Required</h1></body></html>";
            case 500: return "<html><head><title>Arrowhead</title></head><body><h1>500 Internal Server Error</h1></body></html>";
            case 501: return "<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>";

            #ifndef STRIP_LITERALS
              case 100: return "<html><head><title>Arrowhead</title></head><body><h1>100 Continue</h1></body></html>";
              case 101: return "<html><head><title>Arrowhead</title></head><body><h1>101 Switching Protocols</h1></body></html>";
              case 203: return "<html><head><title>Arrowhead</title></head><body><h1>203 Non-Authoritative Information</h1></body></html>";
              case 204: return "<html><head><title>Arrowhead</title></head><body><h1>204 No Content</h1></body></html>";
              case 205: return "<html><head><title>Arrowhead</title></head><body><h1>205 Reset Content</h1></body></html>";
              case 206: return "<html><head><title>Arrowhead</title></head><body><h1>206 Partial Content</h1></body></html>";
              case 300: return "<html><head><title>Arrowhead</title></head><body><h1>300 Multiple Choices</h1></body></html>";
              case 301: return "<html><head><title>Arrowhead</title></head><body><h1>301 Moved Permanently</h1></body></html>";
              case 302: return "<html><head><title>Arrowhead</title></head><body><h1>302 Found</h1></body></html>";
              case 303: return "<html><head><title>Arrowhead</title></head><body><h1>303 See Other</h1></body></html>";
              case 304: return "<html><head><title>Arrowhead</title></head><body><h1>304 Not Modified</h1></body></html>";
              case 305: return "<html><head><title>Arrowhead</title></head><body><h1>305 Use Proxy</h1></body></html>";
              case 307: return "<html><head><title>Arrowhead</title></head><body><h1>307 Temporary Redirect</h1></body></html>";
              case 402: return "<html><head><title>Arrowhead</title></head><body><h1>402 Payment Required</h1></body></html>";
              case 406: return "<html><head><title>Arrowhead</title></head><body><h1>406 Not Acceptable</h1></body></html>";
              case 407: return "<html><head><title>Arrowhead</title></head><body><h1>407 Proxy Authentication Required</h1></body></html>";
              case 408: return "<html><head><title>Arrowhead</title></head><body><h1>408 Request Time-out</h1></body></html>";
              case 409: return "<html><head><title>Arrowhead</title></head><body><h1>409 Conflict</h1></body></html>";
              case 410: return "<html><head><title>Arrowhead</title></head><body><h1>410 Gone</h1></body></html>";
              case 412: return "<html><head><title>Arrowhead</title></head><body><h1>412 Precondition Failed</h1></body></html>";
              case 413: return "<html><head><title>Arrowhead</title></head><body><h1>413 Request Entity Too Large</h1></body></html>";
              case 414: return "<html><head><title>Arrowhead</title></head><body><h1>414 Request-URI Too Large</h1></body></html>";
              case 415: return "<html><head><title>Arrowhead</title></head><body><h1>415 Unsupported Media Type</h1></body></html>";
              case 416: return "<html><head><title>Arrowhead</title></head><body><h1>416 Requested range not satisfiable</h1></body></html>";
              case 417: return "<html><head><title>Arrowhead</title></head><body><h1>417 Expectation Failed</h1></body></html>";
              case 502: return "<html><head><title>Arrowhead</title></head><body><h1>502 Bad Gateway</h1></body></html>";
              case 503: return "<html><head><title>Arrowhead</title></head><body><h1>503 Service Unavailable</h1></body></html>";
              case 504: return "<html><head><title>Arrowhead</title></head><body><h1>504 Gateway Time-out</h1></body></html>";
              case 505: return "<html><head><title>Arrowhead</title></head><body><h1>505 Version not supported</h1></body></html>";
            #else
            #endif
        }

        #ifdef __GNUG__
          return "<html><head><title>Arrowhead</title></head><body><h1>501 Not Implemented</h1></body></html>";
        #else
          throw std::logic_error{ "Wrong status code given." };
        #endif
    }

}


Response Response::from_stock(http::status_code sc) {

    return { sc, stock_replies::to_string(sc), "text/html" };

}
