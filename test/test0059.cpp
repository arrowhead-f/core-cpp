////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Tags:      x509cert
/// Date:      2021-05-02
/// Author(s): ng201
///
/// Description:
/// * Test X509Cert
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <catch2/catch.hpp>


#include <fstream>

#include "http/crate/X509Cert.h"


TEST_CASE("x509cert: Check valid PEM certificate", "[x509cert]") {

    const std::string pem = "-----BEGIN CERTIFICATE----- "                                     "\n"
                            "MIIDMzCCAhugAwIBAgIEWms4hTANBgkqhkiG9w0BAQsFADBqMQswCQYDVQQGEwJI" "\n"
                            "VTEQMA4GA1UECAwHSHVuZ2FyeTERMA8GA1UEBwwIQnVkYXBlc3QxDjAMBgNVBAoM" "\n"
                            "BUFJVElBMSYwJAYDVQQDDB1UZXN0Q2xvdWQxLmFpdGlhLmFycm93aGVhZC5ldTAe" "\n"
                            "Fw0xODAxMjYxNDE3NDFaFw0yODAxMjYxNDE3NDFaME0xCzAJBgNVBAYTAkhVMT4w" "\n"
                            "PAYDVQQDDDVTZWN1cmVUZW1wZXJhdHVyZVNlbnNvci50ZXN0Y2xvdWQxLmFpdGlh" "\n"
                            "LmFycm93aGVhZC5ldTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMsw" "\n"
                            "0VPj+ofCcKfXohmKqpYKAJjy6OjPz02V/qZCq9jCIO1S3Z41Hl8yhZY8CSo2Bht5" "\n"
                            "7rOdxeERZ8pk+1aCEE/kppLi8vdCK+jt10Vog++F5TiJLYU/P36uRT/bCSLrGkXP" "\n"
                            "Y5Md2PIwt8gs9QTyQ2vtAPoXlmqzfiu1u/fOWgRYidcnLAe8iRWALe8FUXNn5IBq" "\n"
                            "ZqeJd+E2uCe0FcM2ATsecOYSBNvRM1zoZdBgenP5YnHJYYrDpzd9Tlos6FiVgHXs" "\n"
                            "Zo0fboB4BUr+HlG40N/0HNLTN5Y5m5zDJo+eHymR7aXrC16wouOGkPPHu1Dnf8aZ" "\n"
                            "QG57RD789evKInuCMxUCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAiTqd1Yixl36q" "\n"
                            "vY5jdGs4Y6fqEX0LnclxdKAESoEZZPfBPcGMOI02xnmpjBPPxdIflLwpN1sMdHmj" "\n"
                            "yZNNPXfPjm3/YmhsJnKYfOiS7WONfShyAbkGnfr6pJOEd6XilrfXtWn/9YSNJdb4" "\n"
                            "NVXWyv1JtYVfh8ZqKPnpjoBSxWRE1l3U5gRteHNBtZBfI9Yq6go0rok1lrsXJFPd" "\n"
                            "p+BZL24yBOFkWGv4nPqNZ0W4xv/Fcm4AoDpuzx92AR7gkJNoHzR1+aT4hCnWwP1X" "\n"
                            "uNzIcP+fbCY/oWtDMPy/pfY5/XCyeyYeywJNktaxEh/xGjN4v7w/citWgLDuQgzQ" "\n"
                            "jYoK6o8/Yg=="                                                     "\n"
                            "-----END CERTIFICATE-----";

    auto cert = X509Cert::decodePEM(pem);
    REQUIRE(cert == true);

    REQUIRE(cert.serial_number() == 1516976261);
    REQUIRE(cert.subject_name()  == "/C=HU/CN=SecureTemperatureSensor.testcloud1.aitia.arrowhead.eu");
    REQUIRE(cert.common_name()   == "SecureTemperatureSensor.testcloud1.aitia.arrowhead.eu");
    REQUIRE(cert.issuer(false)   == "/C=HU/ST=Hungary/L=Budapest/O=AITIA/CN=TestCloud1.aitia.arrowhead.eu");
    REQUIRE(cert.issuer()        == "TestCloud1.aitia.arrowhead.eu");

}


TEST_CASE("x509cert: Check invalid PEM certificate", "[x509cert]") {

    const std::string bad = "-----BEGIN CERTIFICATE----- "                                     "\n"
                            "MIIDMzCCAhugAwIBAgIEWms4hTANBgkqhkiG9w0BAQsFADBqMQswCQYDVQQGEwJI" "\n"
                            "VTEQMA4GA1UECAwHSHVuZ2FyeTERMA8GA1UEBwwIQnVkYXBlc3QxDjAMBgNVBAoM" "\n"
                            "Y5Md2PIwt8gs9QTyQ2vtAPoXlmqzfiu1u/fOWgRYidcnLAe8iRWALe8FUXNn5IBq" "\n"
                            "ZqeJd+E2uCe0FcM2ATsecOYSBNvRM1zoZdBgenP5YnHJYYrDpzd9Tlos6FiVgHXs" "\n"
                            "Zo0fboB4BUr+HlG40N/0HNLTN5Y5m5zDJo+eHymR7aXrC16wouOGkPPHu1Dnf8aZ" "\n"
                            "QG57RD789evKInuCMxUCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAiTqd1Yixl36q" "\n"
                            "vY5jdGs4Y6fqEX0LnclxdKAESoEZZPfBPcGMOI02xnmpjBPPxdIflLwpN1sMdHmj" "\n"
                            "yZNNPXfPjm3/YmhsJnKYfOiS7WONfShyAbkGnfr6pJOEd6XilrfXtWn/9YSNJdb4" "\n"
                            "NVXWyv1JtYVfh8ZqKPnpjoBSxWRE1l3U5gRteHNBtZBfI9Yq6go0rok1lrsXJFPd" "\n"
                            "p+BZL24yBOFkWGv4nPqNZ0W4xv/Fcm4AoDpuzx92AR7gkJNoHzR1+aT4hCnWwP1X" "\n"
                            "uNzIcP+fbCY/oWtDMPy/pfY5/XCyeyYeywJNktaxEh/xGjN4v7w/citWgLDuQgzQ" "\n"
                            "jYoK6o8/Yg=="                                                     "\n"
                            "-----END CERTIFICATE-----";

    auto cert = X509Cert::decodePEM(bad);
    REQUIRE(cert == false);
}


TEST_CASE("x509cert: Decode DER", "[x509cert]") {

    std::ifstream ifs{ "data/test0059/cert.der" };

    REQUIRE(ifs.is_open() == true);

    if (ifs) {
        const std::string der{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
        const auto cert = X509Cert::decodeDER(der);

        REQUIRE(cert.serial_number() == 1516976261);
        REQUIRE(cert.subject_name()  == "/C=HU/CN=SecureTemperatureSensor.testcloud1.aitia.arrowhead.eu");
        REQUIRE(cert.common_name()   == "SecureTemperatureSensor.testcloud1.aitia.arrowhead.eu");
        REQUIRE(cert.issuer(false)   == "/C=HU/ST=Hungary/L=Budapest/O=AITIA/CN=TestCloud1.aitia.arrowhead.eu");
        REQUIRE(cert.issuer()        == "TestCloud1.aitia.arrowhead.eu");
    }
}
