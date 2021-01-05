#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>


/// The request received from the client.
class Request {

    public:

        std::string remote_address;    ///< The remote address.

        std::string method;     ///< The request method, e.g., GET, POST, PATCH, ...
        std::string uri;        ///< The requested URI
        std::string content;    ///< The content.

    private:

        int has_cert = 0;       ///< Whether the request stores the remote ends certificate. 1 means that a cert is stored, 2 means that a hash of a cert is stored.
        std::string cert;       ///< The remote ends certificate.

    public:

        Request(const std::string &remote_address, const std::string &method, const std::string &uri, const std::string &content)
            : remote_address{ remote_address }, method{ method }, uri{ uri }, content{ content } {
        }

        Request(std::string &&remote_address, std::string &&method, std::string &&uri, std::string &&content)
            : remote_address{ std::move(remote_address) }, method{ std::move(method) }, uri{ std::move(uri) }, content{ std::move(content) } {
        }

        /// Returns whether a certicifate is stored or not.
        /// \return                 true, if a certificate is stored
        bool hasCert() const { return has_cert; }

        /// Returns whether the stored cert is a hash.
        /// \return                 true if a hash is stored instead of a certificate
        bool hasCertHash() const { return has_cert; }

        /// Returns the stored certificate.
        /// \return                 the certificate; if no certificate is stored empty string is returned
        const std::string& getCert() const { return cert; }

        /// Sets the certificate.
        /// \param c                the certificate
        /// \param hash             whether the certificate is a hash or not, default: true (i.e., hash)
        void setCert(std::string c, bool hash = true) {
            cert = std::move(c);
            has_cert = hash ? 2 : 1;
        }

};


#endif  /* _HTTP_REQUEST_H_ */
