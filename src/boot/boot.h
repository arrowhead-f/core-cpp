#ifndef _BOOT_BOOT_H_
#define _BOOT_BOOT_H_


#include <string>

#include "utils/inifile.h"


/// Load the config file.
/// \param name                 the name of the default config file
/// \param config               the name of the config file provided by the user
/// \param dbconf               the extra config string for the database
INIFile boot(const char *name, const std::string &config, const std::string &dbconf = std::string{});

/// Extract the HTTP server related configs.
/// \param inif                 the INI file to use
/// \param port                 return the port here, if it was previously not set (i.e., *port != 0)
/// \param thrd                 return the number of threads here
void configureHTTP(INIFile &inif, unsigned *port, unsigned *thrd);


/// Creates the pool of databases.
/// \param                      the INI file used for config
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


/// Creates the KeyProvider.
/// \param                      the INI file used for config
KeyProvider createKeyProvider(INIFile &inif);

#endif  /* _BOOT_BOOT_H_ */
