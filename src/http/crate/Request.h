#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_


#include <string>

#include "X509Cert.h"


/// The request received from the client.
class Request {

    public:

        std::string remote_address;    ///< The remote address.

        std::string method;     ///< The request method, e.g., GET, POST, PATCH, ...
        std::string uri;        ///< The requested URI
        std::string content;    ///< The content.

    private:

        X509Cert cert;          ///< The remote ends certificate.

    public:

        Request(const std::string &remote_address, const std::string &method, const std::string &uri, const std::string &content)
            : remote_address{ remote_address }, method{ method }, uri{ uri }, content{ content } {
        }

        Request(std::string &&remote_address, std::string &&method, std::string &&uri, std::string &&content)
            : remote_address{ std::move(remote_address) }, method{ std::move(method) }, uri{ std::move(uri) }, content{ std::move(content) } {
        }

        /// Returns whether a certicifate is stored or not.
        /// \return                 true, if a certificate is stored
        bool hasCert() const { return cert; }

        /// Returns the stored certificate.
        /// \return                 the certificate; if no certificate is stored empty string is returned
        const auto& getCert() const { return cert; }

        /// Sets the certificate.
        /// \param c                the certificate
        void setCert(X509Cert c) {
            cert = std::move(c);
        }

};


#endif  /* _HTTP_REQUEST_H_ */
