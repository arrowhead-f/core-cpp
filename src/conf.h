#ifndef _CONF_H_
#define _CONF_H_


#include <map>
#include <string>
#include <utility>

#include "http/KeyProvider.h"


namespace conf {

    // default configs
    namespace def {

        static constexpr unsigned port = 16223;
        static constexpr unsigned thrd = 4;
        static constexpr const char* const config = "arrowhead.ini";
        static constexpr const char* const dbconf = "host=127.0.0.1 port=0 dbname=arrowhead user=root passwd=root";
        static constexpr const char* const certsd = "../certs";
        static constexpr const char* const passwd = "12345";

    }

    inline void getHTTPConfig(INIFile &inif, unsigned *port, unsigned *thrd) {
        auto &&dbe = inif.options("HTTP");

        if (!*port) {
            const auto it = dbe.find("port");
            if (it != dbe.cend()) {
                try {
                    *port = std::stoi(it->second);
                }
                catch(...) {
                    throw std::runtime_error{ "Cannot parse the server port from the INI file." };
                }
            }
            else
                *port = conf::def::port;
        }

        if (!*thrd) {
            const auto it = dbe.find("threads");
            if (it != dbe.cend()) {
                try {
                    *thrd = std::stoi(it->second);
                }
                catch(...) {
                    throw std::runtime_error{ "Cannot parse the number of requested threads from the INI file." };
                }
            }
            else
                *thrd = conf::def::thrd;
        }
    }


    template<typename DBPool>auto createDBPool(INIFile &inif) {

        auto &&dbc = inif.options("DB");

        unsigned port = 0;
        const auto it = dbc.find("port");
        if (it != dbc.cend()) {
            try {
                port = std::stoi(it->second);
            }
            catch(...) {
                throw std::runtime_error{ "Cannot parse db port set in INI file." };
            }
        }
        return DBPool{ dbc["host"].c_str(), dbc["user"].c_str(), dbc["passwd"].c_str(), dbc["dbname"].c_str(), port, inif.to_string("DB").c_str() };
    }


    inline auto createKeyProvider(INIFile &inif, const std::string &certsd, const std::string &passwd) {

        auto &&kpc = inif.options("CERT");

        if (certsd.empty()) {
            const auto ssl_cert   = kpc["ssl_cert"];
            const auto ssl_cert_t = kpc["ssl_cert_t"];
            const auto ssl_key    = kpc["ssl_key"];
            const auto ssl_key_t  = kpc["ssl_key_t"];
            const auto cainfo     = kpc["cainfo"];
            const auto keypasswd  = passwd.empty() ? kpc["keypasswd"] : passwd;

            return http::KeyProvider{ ssl_cert, ssl_cert_t, ssl_key, ssl_key_t, keypasswd, cainfo };
        }
        else {
            const auto ssl_cert   = certsd + "/ssl-cert.pem";
            const auto ssl_cert_t = "PEM";
            const auto ssl_key    = certsd + "/ssl_key.pem";
            const auto ssl_key_t  = "PEM";
            const auto cainfo     = certsd + "/cainfo.pem";

            if (passwd.empty())
                throw std::runtime_error{ "A keypasswd should be specified together with the certificate directory." };

            return http::KeyProvider{ ssl_cert, ssl_cert_t, ssl_key, ssl_key_t, passwd, cainfo };
        }
    }

}  // namespace conf

#endif  /* _CONF_H_ */
