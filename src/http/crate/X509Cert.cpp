#include "X509Cert.h"

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/pem.h>


X509Cert X509Cert::load(const std::string &fname) {
    return { nullptr };
}


X509Cert X509Cert::decodePEM(const std::string &pem) {
    BIO *certBio = BIO_new(BIO_s_mem());
    BIO_write(certBio, pem.c_str(), pem.length());
    X509 *cert = PEM_read_bio_X509(certBio, NULL, NULL, NULL);

    BIO_free(certBio);

    return { cert };
}


X509Cert X509Cert::decodeDER(const std::string &der) {
    const unsigned char *p = (const unsigned char*)der.data();
    X509 *cert = d2i_X509(nullptr, &p, der.length());

    return { cert };
}


long X509Cert::serial_number() const {
    ASN1_INTEGER *serial = X509_get_serialNumber(cert);
    return ASN1_INTEGER_get(serial);
}


std::string X509Cert::subject_name() const {
    char buf[256];
    X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);
    return buf;
}


std::string X509Cert::common_name() const {

    X509_NAME *sn = X509_get_subject_name(cert);

    if (!sn)
        return {};

    auto lastpos = X509_NAME_get_index_by_NID(sn, NID_commonName, -1);

    if(lastpos < 0)
        return {};

    X509_NAME_ENTRY *ne = X509_NAME_get_entry(sn, lastpos);
    if (!ne)
        return {};

    ASN1_STRING *asn1_str = X509_NAME_ENTRY_get_data(ne);
    if (!asn1_str)
        return {};

    return std::string{ (char*)ASN1_STRING_get0_data(asn1_str), static_cast<std::string::size_type>(ASN1_STRING_length(asn1_str)) };
}


std::string X509Cert::issuer(bool CN /* = true */) const {

    if (CN) {
        X509_NAME *in = X509_get_issuer_name(cert);

        if (!in)
            return {};

        auto lastpos = X509_NAME_get_index_by_NID(in, NID_commonName, -1);

        if(lastpos < 0)
            return {};

        X509_NAME_ENTRY *ne = X509_NAME_get_entry(in, lastpos);
        if (!ne)
            return {};

        ASN1_STRING *asn1_str = X509_NAME_ENTRY_get_data(ne);
        if (!asn1_str)
            return {};

        return std::string{ (char*)ASN1_STRING_get0_data(asn1_str), static_cast<std::string::size_type>(ASN1_STRING_length(asn1_str)) };
    }

    char buf[256]; buf[0] = 0;
    X509_NAME_oneline(X509_get_issuer_name(cert), buf, 256);
    return buf;
}
