#ifndef _HTTP_WEBGET_H_
#define _HTTP_WEBGET_H_


#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "http/StatusCode.h"
#include "http/KeyProvider.h"


class WebGet {

    public:

        /// Exception thrown by the wget implementations. It is nothing else but a runtime error.
        struct Error : public std::runtime_error {
            using std::runtime_error::runtime_error;
        };


        class result : private std::pair<int, std::string> {

            private:

                using Parent = std::pair<int, std::string>;

            public:

                using Parent::Parent;

                constexpr bool operator==(http::status_code sc) const noexcept {
                    return first == static_cast<int>(sc);
                }

                constexpr bool operator!=(http::status_code sc) const noexcept {
                    return first != static_cast<int>(sc);
                }

                constexpr std::string& value() & noexcept {
                    return second;
                }

                constexpr const std::string& value() const & noexcept {
                    return second;
                }

                constexpr std::string &&value() && noexcept {
                    return std::move(second);
                }

                constexpr const std::string &&value() const && noexcept {
                    return std::move(second);
                }

                constexpr int result_code() const noexcept {
                    return first;
                }

                constexpr bool has_library_error() const noexcept {
                    return first < 0;
                }

                constexpr bool has_status_code() const noexcept {
                    return first > 99;
                }

                constexpr bool is_ok() const noexcept {
                    return first == 200;
                }

        };

    protected:

        ~WebGet() = default;

        inline static result make_result(http::status_code sc, const std::string &str) {
            return result{ static_cast<int>(sc), str };
        }

        inline static result make_result(http::status_code sc, std::string &&str) {
            return result{ static_cast<int>(sc), std::move(str) };
        }

        inline static result make_result(int sc, const std::string &str) {
            return result{ sc, str };
        }

        inline static result make_result(int sc, std::string &&str) {
            return result{ sc, std::move(str) };
        }

    protected:

        const KeyProvider &keyProvider;   ///< The key provider

    public:

        WebGet(const KeyProvider &keyProvider) : keyProvider{ keyProvider } {}

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

        struct Req {
            virtual ~Req() = default;
            virtual result send(const char *method, const std::string &url, long port, const std::string &payload) = 0;
            result send(const char *method, const std::string &url, const std::string &payload) { return send(method, url, -1, payload); }
            result send(const std::string &method, const std::string &url, long port, const std::string &payload) { return send(method.c_str(), url, port, payload); }
        };

        virtual std::unique_ptr<Req> req() = 0;

};

#endif  /* _HTTP_WEBGET_H_ */
