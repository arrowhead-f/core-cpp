#ifndef _HTTP_X509CERT_H_
#define _HTTP_X509CERT_H_


#include <string>
#include <utility>

#include <openssl/x509.h>


class X509Cert {

    private:

        X509 *cert;  ///< The wrapped X509 certificate.

    public:

        /// Constructor.
        X509Cert() : cert{ nullptr } {}

        /// Constructor.
        X509Cert(X509 *cert) : cert{ cert } {}

        X509Cert(const X509Cert&) = delete;

        X509Cert& operator=(const X509Cert&) = delete;

        X509Cert(X509Cert &&other) noexcept : cert{ std::exchange(other.cert, nullptr) } {}

        X509Cert& operator=(X509Cert &&other) noexcept {
            std::swap(cert, other.cert);
            return *this;
        }

        /// Destructor. Frees the internal X509 structure.
        ~X509Cert() {
            if (cert)
                X509_free(cert);
        }

        /// Loadsa PEM certificate from the given file.
        /// \param fname            the name of the file
        /// \return                 a new X509Cert object
        static X509Cert load(const std::string &fname);

        /// Creates an object from the certificate given as PEM format string.
        /// \param fname            the der format string
        /// \return                 a new X509Cert object
        static X509Cert decodePEM(const std::string &pem);

        /// Creates an object from the certificate given as DER format string.
        /// \param fname            the der format string
        /// \return                 a new X509Cert object
        static X509Cert decodeDER(const std::string &der);

        /// Checks whether the stored certificate is valid.
        /// \return                 true if the certificate is valid
        operator bool() const {
            return cert;
        }

        /// Returns the serial number of the certificate.
        /// \return                 the serial number
        long serial_number() const;

        /// Returns the subject's name.
        /// \return                 the name of the subject
        std::string subject_name() const;

        /// Returns the common name of the certificate.
        /// \return                 the common name.
        std::string common_name() const;

        /// Returns the issuer's data.
        /// \param CN               if true, only the common name of the issuer is returned
        /// \return                 the name of the issuer
        std::string issuer(bool CN = true) const;

};

#endif  /* _HTTP_X509CERT_H_ */
