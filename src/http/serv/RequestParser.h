#ifndef _HTTP_REQUESTPARSER_H_
#define _HTTP_REQUESTPARSER_H_


#include <utility>

#include "RequestBuffer.h"


/// Parser for incoming requests.
class RequestParser {

    public:

        /// Result of parsing.
        enum result_t {
            completed = 0,   // the parsing was ok
            incomplete,      // still processing
            failed           // processing ended with error
        };

    private:

        /// The current state of the parser.
        enum state_t {

            RequestMethodStart,
            RequestMethod,
            RequestUriStart,
            RequestUri,
            RequestHttpVersion_h,
            RequestHttpVersion_ht,
            RequestHttpVersion_htt,
            RequestHttpVersion_http,
            RequestHttpVersion_slash,
            RequestHttpVersion_majorStart,
            RequestHttpVersion_major,
            RequestHttpVersion_minorStart,
            RequestHttpVersion_minor,
            RequestHttpVersion_newLine,

            HeaderLineStart,
            HeaderLws,
            HeaderName,
            SpaceBeforeHeaderValue,
            HeaderValue,
            ExpectingNewline_2,
            ExpectingNewline_3,

            Post,
            ChunkSize,
            ChunkExtensionName,
            ChunkExtensionValue,
            ChunkSizeNewLine,
            ChunkSizeNewLine_2,
            ChunkSizeNewLine_3,
            ChunkTrailerName,
            ChunkTrailerValue,

            ChunkDataNewLine_1,
            ChunkDataNewLine_2,
            ChunkData,

        } state = RequestMethodStart;   ///< The current internal state of the parser.


        RequestBuffer req;                ///< All the collected data.

        std::size_t contentSize = 0;      ///< The size of the content.

        bool chunked            = false;  ///< Whether the parsed request is chunked.
        std::size_t chunkSize   = 0;      ///< The size of the chunk.
        std::string chunkSizeStr;         ///< The chunk.

    public:

        /// Parses the string given by the begin and end iterators.
        /// \param begin            the parsing's starting position
        /// \param end              the parser should read the input till this iterator
        /// \return                 the current status and teh next position of the stream to be read
        template<typename InputIterator>
        std::pair<result_t, InputIterator> parse(InputIterator begin, InputIterator end) {
            while (begin != end) {

                const auto result = consume(*begin++);

                if (result == result_t::failed)
                    return { result_t::failed, begin };

                if (result == result_t::completed)
                    return { result_t::completed, begin };

            }

            return { result_t::incomplete, begin };
        }

        const auto& inspect() const {
            return req;
        }

        /// Returns the request parsed.
        auto request() {
            return std::move(req).to_request();
        }

        /// Resets the state of the parser.
        void reset() {
            state = RequestMethodStart;
            contentSize = 0;
            chunkSizeStr.clear();
            chunkSize   = 0;;
            chunked     = false;
        }

    private:

        /// Check if a byte is an HTTP character.
        /// \param c               the character to decide of
        /// \return                true, if the given character is a HTTP character
        static constexpr bool isChar(int c) {
            return c >= 0 && c <= 127;
        }

        /// Check whether the byte given is an HTTP control character.
        /// \param c               the character to decide of
        /// \return                true, if the given character is a control character
        static constexpr bool isControl(int c) {
            return (c >= 0 && c <= 31) || (c == 127);
        }

        /// Check if a byte is defined as an HTTP special character.
        /// \param c               the character to decide of
        /// \return                true, if the given character is a special character.
        static bool isSpecial(int c);

        /// Check if a byte is a digit.
        /// \param c               the character to decide of
        /// \return                true, if the given character is digit
        static constexpr bool isDigit(int c) {
            return c >= '0' && c <= '9';
        }

        /// Handles the next character of the input.
        /// \param input            the next character of the input
        /// \return                 the status of the parsing
        result_t consume(char input);

};


#endif  /* _HTTP_REQUESTPARSER_H_ */
