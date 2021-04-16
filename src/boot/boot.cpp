#include "boot.h"


// default configs
namespace details {

    static constexpr unsigned port = 16223;
    static constexpr unsigned thrd = 4;
    static constexpr const char* const dbconf = "host=127.0.0.1 port=0 dbname=arrowhead user=root passwd=root";

    #ifdef NDEBUG
      #define BOOT_SEQ {"etc","/etc"}
    #else
      #define BOOT_SEQ {"../etc", "etc", "/etc" }
    #endif

}


#include <iostream>

INIFile boot(const char *name, const std::string &config, const std::string &dbconf /* = std::string{} */) {

    auto conf = INIFile{};
    if (config.empty()) {
        bool f = false;
        for(const auto &c : BOOT_SEQ) {
            try {
                conf.reset(c + std::string{ "/" } + name);
            }
            catch(...) {
                continue;
            }
            f = true;
            break;
        }
        if (!f)
            throw std::runtime_error{ "Cannot find/open the INI file." };
    }
    else {
        conf.reset(config);
    }

    bool error = !conf.parse("DB", true) || !conf.parse("SSL", true) || !conf.parse("HTTP", true);

    if (error) {
        throw std::runtime_error{ "Cannot parse the INI file." };
    }

    conf.prepend("DB", details::dbconf);  // prepend default settings
    conf.append("DB", dbconf);            // append user's override

    return conf;
}


void configureHTTP(INIFile &inif, unsigned *port, unsigned *thrd) {

    auto &&dbe = inif.options("HTTP");  // get the right section from the file

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
            *port = details::port;
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
            *thrd = details::thrd;
    }
}


KeyProvider createKeyProvider(INIFile &inif) {
    auto &&kpc = inif.options("SSL");
    auto kp = KeyProvider{};

    if (kpc["key-store-type"] != "PEM" || kpc["trust-store-type"] != "PEM")
        throw std::runtime_error{ "Invalid store types." };

    kp.loadKeyStore(kpc["key-store"], kpc["key-alias"], kpc["key-password"])
      .loadTrustStore(kpc["trust-store"], kpc["trsut-store-password"], (kpc["client-auth"] == "need"));

    return kp;
}
