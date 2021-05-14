#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>

/* utility functions */
#include "utils/logger.h"
#include "utils/pgetopt.h"
#include "utils/inifile.h"

/* the database adapter */
#include "db/DB.h"
#include "db/MariaDB.h"

/* connections*/
#include "http/KeyProvider.h"
#include "http/wget/WG_Curl.h"
#include "http/serv/HTTPSServer.h"

/* this should be the last header included */
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

/* configuration helpers */
#include "boot/boot.h"


/// Print help message to the standard output.
/// \param prog             The name of the program.
/// \param name             The name of the core element.
void print_hlp(const char *prog, const char *name);


int main(int argc, char *argv[]) try {

    // parse command line arguments
    int ch = 0;
    poptarg = nullptr;

    // error flag for command line parsing
    bool error = false;

    bool suppress = false;  // whether to suppers command line parsing errors

    unsigned port = 0;   // the listening port
    unsigned thrd = 0;   // the number of server threads

    std::string config;  // the location/name of the config file
    std::string dbconf;  // extra database config

    while((ch = pgetopt (argc, argv, "hvsc:d:p:")) != -1) {
        switch (ch) {
            case 'h':
                // argv[0] is always there
                print_hlp(argv[0], CoreElement::name);
                std::exit(0);
                break;
            case 's':
                suppress = true;
                break;
            case 'v':
                #ifdef PACKAGE_VERSION
                    std::cout << CoreElement::name << " version " << PACKAGE_VERSION << "\n";
                #else
                    std::cout << CoreElement::name << " version " << "1.0" << "\n";
                #endif
                std::exit(0);
                break;
            case 'c':
                config = std::string{ poptarg };
                break;
            case 'd':
                dbconf = std::string{ poptarg };
                break;
            case 'p':
                try {
                    port = std::stoi( std::string{ poptarg });
                }
                catch(...) {
                    error = true;
                }
                break;
            case '?':
                /* some error messaging */
                error = true;
                break;
            default:
                return 2;
        }
    }

    // initialize looger
    logger::init(LOG_DEBUG, CoreElement::name, argv[0]);

    (info{ } << fmt("Started...")).log(SOURCE_LOCATION);
    (info{ } << fmt("Params:{} -p {} -c '{}'") << (suppress ? " -s" : "") << port << config).log(SOURCE_LOCATION);

    if (error && !suppress) {
        (info{ } << fmt("Could not initialize the program. Double chek the command line argument.")).log(SOURCE_LOCATION);
    }

    // boot the up, load the INI file
    auto conf = boot(/*"arrowhead.ini"*/ CoreElement::ini, config, dbconf);

    // get the port and the numer of threads
    configureHTTP(conf, &port, &thrd);

    // create a pool of database connection
    auto pool = createDBPool<db::DatabasePool<db::MariaDB>>(conf);

    //  create the key provider
    auto keyProvider = createKeyProvider(conf);
    if (!keyProvider.check()) {
        throw std::runtime_error{ "Cannot parse certificate." };
    }

    // create the request builder
    CoreElement::WebGet reqBuilder{ keyProvider };

    // the core element
    auto coreElement = CoreElement::Type<db::DatabasePool<db::MariaDB>, CoreElement::WebGet>{ pool, reqBuilder };

    // the http(s) server
    auto http = HTTPSServer<CoreElement::DispatcherType<db::DatabasePool<db::MariaDB>, CoreElement::WebGet>>("127.0.0.1", static_cast<std::size_t>(port), coreElement, keyProvider);

    // we do not need the inner structures of the INIFile anymore; free the interna structures
    conf.close();

    // ------------------***--- RUN ---***------------------ //
    http.run(thrd);
    // ------------------***--- RUN ---***------------------ //

    return 0;

}
catch(const std::exception &e) {
    (error{ } << fmt("Execution terminated with exception. {}") << e.what()).log(SOURCE_LOCATION);
}
catch(...) {
    (error{ } << fmt("Execution terminated with unexpected error.")).log(SOURCE_LOCATION);
}


/*----------------------------------------------------------------------------------------------------*/


void print_hlp(const char *prog, const char *name) {
    std::cout << "\n" << name << ".\n\n";
    std::cout << "Usage: " << prog << " [-p <port>] [-c <config-file>] [-C <cert-directory>] [-P <passwd>]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h            Show this screen.\n"
              << "  -v            Show version.\n"
              << "  -p PORT       Set the listener PORT [default: 16223].\n"
              << "  -c CONFFILE   Set the configuration file.\n"
              << "  -d DBCONF     Config for the database.\n"
              << "\n";
}
