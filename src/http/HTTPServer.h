#ifndef _ARROWHEAD_HTTPSERVER_H_
#define _ARROWHEAD_HTTPSERVER_H_

#include <exception>
#include <string>

namespace http {

    enum errc {
        SUCCESS  =   0,  // not a real error code
        //
        SSLCTX   =   1,
        SIGNAL   =   2,
        TCPPORT  =   3,
        //
        UNDEF    = 127
    };


    class ServerException : public std::exception {

        private:

            int err_code;            ///< The error's code.

            #ifndef VERYSLIM
              const std::string msg;   ///< The error's description.
            #endif

        public:

            #ifndef VERYSLIM
              ServerException(int err_code, std::string msg) : err_code{ err_code }, msg{ std::move(msg) } {}
            #else
              ServerException(int err_code, std::string) : err_code{ err_code } {}
            #endif

            int code() const noexcept {
                return err_code;
            }

            const char* what() const noexcept final {
                #ifndef VERYSLIM
                  return msg.c_str();
                #else
                  return "N/A";
                #endif
            }

    };  // class HTTPServerException

    struct HTTPServerBase {

        /// Virtual destruction to make deletion work through pointers.
        virtual ~HTTPServerBase() = default;

        /// Runs the server.
        virtual void run() = 0;

        // HTTP callbacks
        virtual int handle(const std::string &from_address, const char *method, const std::string &uri, std::string &response, const std::string &payload) = 0;

        // Handle HTTP message formar errors
        virtual void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) = 0;

    };  // struct HTTHServerBase

    template<typename T>
    class HTTPServer : public HTTPServerBase {

        private:

            std::string address;         ///< The address of the server.
            std::size_t port;            ///< The port used.
            T &dispatcher;               ///< The core system element attached to the port.

        public:

            /// Constructs a HTTP(S) server.
            /// \param address        the address of the server
            /// \param port           the port used by the service
            /// \param coreElement    reference to the core element sitting on the port
            HTTPServer(const std::string &address, std::size_t port, T &dispatcher) : address{ address }, port{ port }, dispatcher{ dispatcher } {
            }

            /// Constructs a HTTP(S) server.
            /// \param address        the address of the server
            /// \param port           the port used by the service
            /// \param coreElement    reference to the core element sitting on the port
            HTTPServer(std::string &&address, std::size_t port, T &dispatcher) : address{ std::move(address) }, port{ port }, dispatcher{ dispatcher } {
            }

            /// Returns the address of the server.
            /// \return    the address of the server
            std::string getAddress() const {
                return address;
            }

            /// Returns the port used by the server.
            /// \return    the port used by the server
            std::size_t getPort() const {
                return port;
            }

            int handle(const std::string &from, const char *method, const std::string &uri, std::string &response, const std::string &payload) override {
                return dispatcher.dispatch(from, method, uri, response, payload);
            }

            void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) override {
                dispatcher.error(from, method, uri);
            }

    };  // class HTTPServer


}  // namespace http

#endif  /*_ARROWHEAD_HTTPSERVER_H_*/
