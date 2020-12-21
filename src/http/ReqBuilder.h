#ifndef _HTTP_REQBUILDER_H_
#define _HTTP_REQBUILDER_H_


#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "http/StatusCode.h"
#include "http/KeyProvider.h"


namespace http {


    class ReqBuilder {

        public:

            struct request_error : public std::runtime_error {
                using std::runtime_error::runtime_error;
            };

            class result_code {

                private:

                    int ec = static_cast<int>(http::status_code::OK);

                public:

                    constexpr result_code() noexcept = default;

                    constexpr explicit result_code(int ec) noexcept: ec{ ec } { }

                    constexpr explicit result_code(long ec) noexcept: ec{ static_cast<int>(ec) } { }

                    constexpr result_code(status_code ec) noexcept: ec{ static_cast<int>(ec) } { }

                    constexpr int code() const noexcept { return ec; }

                    constexpr explicit operator bool() const noexcept {
                        return ec == 200;
                    }

                    constexpr bool is_library_error() const noexcept {
                        return ec < 0;
                    }

                    constexpr bool is_status_code() const noexcept {
                        return ec > 99;
                    }

            };

            class result : private std::pair<result_code, std::string> {

                private:

                    using Parent = std::pair<result_code, std::string>;

                public:

                    using Parent::Parent;

                    bool operator==(result_code ec) const;
                    bool operator!=(result_code ec) const;
                    bool operator<(result_code ec) const;
                    bool operator>(result_code ec) const;
                    bool operator<=(result_code ec) const;
                    bool operator>=(result_code ec) const;

                    constexpr std::string &value() & {
                        return second;
                    }

                    constexpr const std::string &value() const & {
                        return second;
                    }

                    constexpr std::string &&value() && {
                        return std::move(second);
                    }

                    constexpr const std::string &&value() const && {
                        return std::move(second);
                    }

                    constexpr int result_code() const noexcept {
                        return first.code();
                    }

                    constexpr bool has_library_error() const noexcept {
                        return first.is_library_error();
                    }

                    constexpr bool has_status_code() const noexcept {
                        return first.is_status_code();
                    }

                    constexpr bool is_ok() const noexcept {
                        return static_cast<bool>(first);
                    }

            };

        protected:

            inline static result make_result(result_code sc, const std::string &str) {
                return result{ sc, str };
            }

            inline static result make_result(result_code sc, std::string &&str) {
                return result{ sc, std::move(str) };
            }

            inline static result make_result(int sc, const std::string &str) {
                return result{ sc, str };
            }

            inline static result make_result(int sc, std::string &&str) {
                return result{ sc, std::move(str) };
            }

        protected:

            const http::KeyProvider &keyProvider;   ///< The key provider

        public:

            ReqBuilder(const http::KeyProvider &keyProvider) : keyProvider{ keyProvider } {}

            virtual ~ReqBuilder() = default;

            /// Send a request to the given url with the given method and payload.
            /// \param method     the request method to be used
            /// \param url        the url of the request
            /// \param por        the remote end's port to use
            /// \param payload    the payload of the reqquest
            /// \return           the pair that consists of the http code and returned data
            virtual result send(const char *method, const std::string &url, long port, const std::string &payload) = 0;

            /// Send a request to the given url with the given method and payload.
            /// \param method     the request method to be used
            /// \param url        the url of the request
            /// \param payload    the payload of the reqquest
            /// \return           the pair that consists of the http code and returned data
            result send(const char *method, const std::string &url, const std::string &payload) {
                return send(method, url, -1, payload);
            }

            /// Send a request to the given url with the given method and payload.
            /// \param method     the request method to be used
            /// \param url        the url of the request
            /// \param port       the port to use
            /// \param payload    the payload of the reqquest
            /// \return           the pair that consists of the http code and returned data
            result send(const std::string &method, const std::string &url, long port, const std::string &payload) {
                return send(method.c_str(), url, port, payload);
            }

            struct Request {
                virtual ~Request() = default;
                virtual result send(const char *method, const std::string &url, long port, const std::string &payload) = 0;
                result send(const char *method, const std::string &url, const std::string &payload) { return send(method, url, -1, payload); }
                result send(const std::string &method, const std::string &url, long port, const std::string &payload) { return send(method.c_str(), url, port, payload); }
            };

            virtual std::unique_ptr<Request> req() = 0;
    };


    inline bool operator==(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() == rhs.code();
    }

    inline bool operator!=(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() != rhs.code();
    }

    inline bool operator<(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() < rhs.code();
    }

    inline bool operator>(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() > rhs.code();
    }

    inline bool operator<=(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() <= rhs.code();
    }

    inline bool operator>=(const ReqBuilder::result_code &lhs, const ReqBuilder::result_code &rhs) noexcept {
        return lhs.code() >= rhs.code();
    }

    inline bool ReqBuilder::result::operator==(ReqBuilder::result_code ec) const {
        return first == ec;
    }

    inline bool ReqBuilder::result::operator!=(ReqBuilder::result_code ec) const {
        return first != ec;
    }

    inline bool ReqBuilder::result::operator<(ReqBuilder::result_code ec) const {
        return first < ec;
    }

    inline bool ReqBuilder::result::operator>(ReqBuilder::result_code ec) const {
        return first > ec;
    }

    inline bool ReqBuilder::result::operator<=(ReqBuilder::result_code ec) const {
        return first <= ec;
    }

    inline bool ReqBuilder::result::operator>=(ReqBuilder::result_code ec) const {
        return first >= ec;
    }

}

#endif  /*_HTTP_REQBUILDER_H_*/
