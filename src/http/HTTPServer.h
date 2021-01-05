#ifndef _HTTP_HTTPSERVER_H_
#define _HTTP_HTTPSERVER_H_


#include <stdexcept>
#include <string>

#include "http/crate/Request.h"
#include "http/crate/Response.h"


/// The base non-templated base class of the servers.
struct HTTPServer {

    /// Exception thrown by the server. It is nothing else but a runtime error.
    struct Error : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    /// Virtual destruction to make deletion work through pointers.
    virtual ~HTTPServer() = default;

    /// Runs the server.
    virtual void run() = 0;

    // HTTP callbacks
    virtual Response handle(Request &&req) = 0;

    // Handle HTTP message formar errors
    virtual void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) = 0;

};


template<typename T>
class HTTPServerBase : public HTTPServer {

    private:

        std::string address;         ///< The address of the server.
        std::size_t port;            ///< The port used.
        T &dispatcher;               ///< The core system element attached to the port.

    public:

        /// Constructs a HTTP(S) server.
        /// \param address        the address of the server
        /// \param port           the port used by the service
        /// \param coreElement    reference to the core element sitting on the port
        HTTPServerBase(const std::string &address, std::size_t port, T &dispatcher) : address{ address }, port{ port }, dispatcher{ dispatcher } {
        }

        /// Constructs a HTTP(S) server.
        /// \param address        the address of the server
        /// \param port           the port used by the service
        /// \param coreElement    reference to the core element sitting on the port
        HTTPServerBase(std::string &&address, std::size_t port, T &dispatcher) : address{ std::move(address) }, port{ port }, dispatcher{ dispatcher } {
        }

        /// Returns the address of the server.
        /// \return               the address of the server
        const std::string& getAddress() const {
            return address;
        }

        /// Returns the port used by the server.
        /// \return               the port used by the server
        std::size_t getPort() const {
            return port;
        }

        Response handle(Request &&req) final {
            return dispatcher.dispatch(std::move(req));
        }

        void denied(const std::string &from, const char *method, const std::string &uri, const char *reason) final {
            dispatcher.error(from, method, uri, reason);
        }

};


#endif  /* __HTTP_HTTPSERVER_H_ */
