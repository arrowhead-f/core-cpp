#ifndef _ARROWHEAD_HTTPHANDLER_H_
#define _ARROWHEAD_HTTPHANDLER_H_

#include <string>
#include <map>

struct HTTPHandlerBase {

    /// Virtual destruction to make deletion work through pointers.
    virtual ~HTTPHandlerBase() = default;

    /// Called to start the server.
    virtual bool start() {
        // implement base logging
        return true;
    }

    /// Called to gracefully stop the server.
    virtual bool stop() {
        // implement base logging
        return true;
    }

    //virtual void setOptions(const std::string &opt) = 0;
    //virtual void setOptions(std::string &&opt)      = 0;

    // HTTP callbacks
    virtual int GETCallback   (const char *url, std::string &response, const char *page, const char *itemPerPage, const char *sortField, const char *direction) = 0;
    virtual int DELETECallback(const char *url, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname) = 0;
    virtual int POSTCallback  (const char *url, std::string &response, const char *payload) = 0;
    virtual int PUTCallback   (const char *url, std::string &response, const char *payload) = 0;
    virtual int PATCHCallback (const char *url, std::string &response, const char *payload) = 0;

};  // struct HTTHandlerBase


template<typename T>class HTTPHandler : public HTTPHandlerBase {

    private:

        std::size_t port;            ///< The port used.
        T           &coreElement;    ///< The core system element attached to the port.

    public:

        /// Constructs a HTTP request handler.
        /// \param port           the port used by the service
        /// \param coreElement    reference to the core element sitting on the port
        HTTPHandler(std::size_t port, T &coreElement) : port{ port }, coreElement{ coreElement } {
        }

        /// Returns the port used by the server.
        /// \return    the port used by the server
        std::size_t getPort() const {
            return port;
        }

        int GETCallback(const char *url, std::string &response, const char *page, const char *itemPerPage, const char *sortField, const char *direction) final {
            return coreElement.GETCallback(url, response, page, itemPerPage, sortField, direction);
        }

        int DELETECallback(const char *url, std::string &response, const char *addr, const char *port, const char *servdef, const char *sysname) final {
            return coreElement.DELETECallback(url, response, addr, port, servdef, sysname);
        }

        int POSTCallback(const char *url, std::string &response, const char *payload) final {
            return coreElement.POSTCallback(url, response, payload);
        }

        int PUTCallback(const char *url, std::string &response, const char *payload) final {
            return coreElement.PUTCallback(url, response, payload);
        }

        int PATCHCallback(const char *url, std::string &response, const char *payload) final {
            return coreElement.PATCHCallback(url, response, payload);
        }

};  // class HttpHandler

#endif  /*_ARROWHEAD_HTTPHANDLER_H_*/
