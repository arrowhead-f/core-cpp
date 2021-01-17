#include "RequestParser.h"


#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <algorithm>

#ifdef HAVE_STRCASECMP
  #include <strings.h>
#else
  #include <cctype>

  static int strcasecmp(const char *s1, const char *s2) {
       const unsigned char *p1 = (const unsigned char*)s1;
       const unsigned char *p2 = (const unsigned char*)s2;

       if (p1 == p2)
           return 0;

       int result;
       while ((result = std::tolower(*p1) - std::tolower(*p2++)) == 0)
           if (*p1++ == '\0')
               break;

      return result;
  }
#endif


bool RequestParser::isSpecial(int c) {
    switch (c) {
        case '(':
        case ')':
        case '<':
        case '>':
        case '@':
        case ',':
        case ';':
        case ':':
        case '\\':
        case '"':
        case '/':
        case '[':
        case ']':
        case '?':
        case '=':
        case '{':
        case '}':
        case ' ':
        case '\t':
                    return true;
    }

    return false;
}


RequestParser::result_t RequestParser::consume(char input) {

    switch (state) {

        case RequestMethodStart:

                //if (!isChar(input) || isControl(input) || isSpecial(input)) {
                //    return result_t::failed;
                //}
                //else {
                //    state = RequestMethod;
                //    req.method.push_back(input);
                //}

                if (isMethodStart(input)) {
                    state = RequestMethod;
                   req.method.push_back(input);
                }
                else {
                    return result_t::failed;
                }

                break;

        case RequestMethod:

                if (input == ' ')
                    state = RequestUriStart;
                else if (!isChar(input) || isControl(input) || isSpecial(input))
                    return result_t::failed;
                else
                    req.method.push_back(input);
                break;

        case RequestUriStart:

                if (isControl(input)) {
                    return result_t::failed;
                }
                else {
                    state = RequestUri;
                    req.uri.push_back(input);
                }
                break;

        case RequestUri:

                if (input == ' ') {
                    state = RequestHttpVersion_h;
                }
                else if (input == '\r') {
                    req.http_version_major = 0;
                    req.http_version_minor = 9;

                    return result_t::completed;
                }
                else if (isControl(input)) {
                    return result_t::failed;
                }
                else {
                    req.uri.push_back(input);
                }
                break;

        case RequestHttpVersion_h:

                if (input == 'H') {
                    state = RequestHttpVersion_ht;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_ht:

                if( input == 'T' ) {
                    state = RequestHttpVersion_htt;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_htt:

                if( input == 'T' ) {
                    state = RequestHttpVersion_http;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_http:

                if (input == 'P') {
                    state = RequestHttpVersion_slash;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_slash:

                if( input == '/' ) {
                    req.http_version_major = 0;
                    req.http_version_minor = 0;
                    state = RequestHttpVersion_majorStart;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_majorStart:

                if (isDigit(input)) {
                    req.http_version_major = input - '0';
                    state = RequestHttpVersion_major;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_major:

                if (input == '.' ) {
                    state = RequestHttpVersion_minorStart;
                }
                else if (isDigit(input)) {
                    req.http_version_major = req.http_version_major * 10 + input - '0';
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_minorStart:

                if (isDigit(input)) {
                    req.http_version_minor = input - '0';
                    state = RequestHttpVersion_minor;
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_minor:

                if (input == '\r') {
                    state = RequestHttpVersion_newLine;
                }
                else if (isDigit(input)) {
                    req.http_version_minor = req.http_version_minor * 10 + input - '0';
                }
                else {
                    return result_t::failed;
                }
                break;

        case RequestHttpVersion_newLine:

                if (input == '\n') {
                    state = HeaderLineStart;
                }
                else {
                    return result_t::failed;
                }
                break;

        case HeaderLineStart:

                if (input == '\r') {
                    state = ExpectingNewline_3;
                }
                else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
                    state = HeaderLws;
                }
                else if (!isChar(input) || isControl(input) || isSpecial(input)) {
                    return result_t::failed;
                }
                else {
                    req.headers.push_back(RequestBuffer::Header{});
                    req.headers.back().name.reserve(16);
                    req.headers.back().value.reserve(16);
                    req.headers.back().name.push_back(input);
                    state = HeaderName;
                }
                break;

        case HeaderLws:

                if (input == '\r') {
                    state = ExpectingNewline_2;
                }
                else if (input == ' ' || input == '\t') {
                }
                else if (isControl(input)) {
                    return result_t::failed;
                }
                else {
                    state = HeaderValue;
                    req.headers.back().value.push_back(input);
                }
                break;

        case HeaderName:

                if (input == ':') {
                    state = SpaceBeforeHeaderValue;
                }
                else if (!isChar(input) || isControl(input) || isSpecial(input)) {
                    return result_t::failed;
                }
                else {
                    req.headers.back().name.push_back(input);
                }
                break;

        case SpaceBeforeHeaderValue:

                if (input == ' ') {
                    state = HeaderValue;
                }
                else {
                    return result_t::failed;
                }
                break;

        case HeaderValue:

                if (input == '\r') {
                    if (req.method == "POST" || req.method == "PUT" || req.method == "PATCH" || req.method == "DELETE") {
                        auto &h = req.headers.back();

                        if (strcasecmp(h.name.c_str(), "Content-Length") == 0) {
                            contentSize = atoi(h.value.c_str());
                            req.content.reserve(contentSize);
                        }
                        else if (strcasecmp(h.name.c_str(), "Transfer-Encoding") == 0) {
                            if (strcasecmp(h.value.c_str(), "chunked") == 0)
                                chunked = true;
                        }
                    }
                    state = ExpectingNewline_2;
                }
                else if (isControl(input)) {
                    return result_t::failed;
                }
                else {
                    req.headers.back().value.push_back(input);
                }
                break;

        case ExpectingNewline_2:

                if (input == '\n') {
                    state = HeaderLineStart;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ExpectingNewline_3: {

               const auto it = std::find_if(req.headers.cbegin(), req.headers.cend(), [](const auto item){ return strcasecmp(item.name.c_str(), "Connection") == 0; });

                if (it != req.headers.end()) {
                    if (strcasecmp(it->value.c_str(), "keep-alive") == 0) {
                        req.keepAlive = true;
                    }
                    else { // == Close
                        req.keepAlive = false;
                    }
                }
                else {
                    // keep alive for HTTP/1.1 is the default
                    if (req.http_version_major == 1 && req.http_version_minor == 1)
                        req.keepAlive = true;
                }

                if (chunked) {
                    state = ChunkSize;
                }
                else if (contentSize == 0) {
                    if (input == '\n')
                        return result_t::completed;
                    else
                        return result_t::failed;
                }
                else {
                    state = Post;
                }
                break;
            }

        case Post:

                --contentSize;
                req.content.push_back(input);

                if (contentSize == 0) {
                    return result_t::completed;
                }
                break;

        case ChunkSize:

                if (isalnum(input)) {
                    chunkSizeStr.push_back(input);
                }
                else if (input == ';') {
                    state = ChunkExtensionName;
                }
                else if (input == '\r') {
                    state = ChunkSizeNewLine;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkExtensionName:

                if (isalnum(input) || input == ' ') {
                    // skip
                }
                else if (input == '=') {
                    state = ChunkExtensionValue;
                }
                else if (input == '\r') {
                    state = ChunkSizeNewLine;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkExtensionValue:

                if (isalnum(input) || input == ' ') {
                    // skip
                }
                else if (input == '\r') {
                    state = ChunkSizeNewLine;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkSizeNewLine:

                if (input == '\n' ) {
                    chunkSize = std::strtol(chunkSizeStr.c_str(), nullptr, 16);
                    chunkSizeStr.clear();
                    req.content.reserve(req.content.size() + chunkSize);

                    if (chunkSize == 0) 
                        state = ChunkSizeNewLine_2;
                    else
                        state = ChunkData;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkSizeNewLine_2:

                if (input == '\r') {
                    state = ChunkSizeNewLine_3;
                }
                else if (isalpha(input)) {
                    state = ChunkTrailerName;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkSizeNewLine_3:

                if (input == '\n') {
                    return result_t::completed;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkTrailerName:

                if (isalnum(input)) {
                    // skip
                }
                else if (input == ':') {
                    state = ChunkTrailerValue;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkTrailerValue:

                if (isalnum(input) || input == ' ') {
                    // skip
                }
                else if (input == '\r') {
                    state = ChunkSizeNewLine;
                }
                else {
                    return result_t::failed;
                }
                break;

        case ChunkData:

                req.content.push_back(input);

                if (--chunkSize == 0) {
                    state = ChunkDataNewLine_1;
                }
                break;

        case ChunkDataNewLine_1:

                if (input == '\r')
                    state = ChunkDataNewLine_2;
                else
                    return result_t::failed;
                break;

        case ChunkDataNewLine_2:

                if (input == '\n')
                    state = ChunkSize;
                else
                    return result_t::failed;
                break;

        default:

                return result_t::failed;
    }

    return result_t::incomplete;

}
