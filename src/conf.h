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


    inline auto createKeyProvider(INIFile &inif) {

        auto &&kpc = inif.options("SSL");
        auto kp = KeyProvider{};

        if (kpc["key-store-type"] != "PEM" || kpc["trust-store-type"] != "PEM")
            throw std::runtime_error{ "Invalid store types." };

        kp.loadKeyStore(kpc["key-store"], kpc["key-alias"], kpc["key-password"])
          .loadTrustStore(kpc["trust-store"], kpc["trsut-store-password"], (kpc["client-auth"] == "need"));

        return kp;
    }

}  // namespace conf

#endif  /* _CONF_H_ */
