#include <cstdlib>
#include <cstring>
#include <iostream>

/* utility functions */
#include "utils/logger.h"
#include "utils/pgetopt.h"
#include "utils/inifile.h"
#include "utils/traits.h"

/* the database adapter */
#include "db/DB.h"
#include "db/MariaDB.h"

/* core system elements */
#include "core/Authorizer/Authorizer.h"
#include "core/CertAuthority/CertAuthority.h"

/* connections*/
#include "http/KeyProvider.h"
#include "http/wget/WG_Curl.h"
#include "http/serv/HTTPSServer.h"

/* this should be the last header included */
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

/* configuration helpers */
#include "conf.h"


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

    std::string config{ conf::def::config };  // the location/name of the config file
    std::string dbconf;  // the directory that stores the certificates
    std::string certsd;  // the directory that stores the certificates
    std::string passwd;  // the passphare for the certificates

    while((ch = pgetopt (argc, argv, "hvsc:p:C:P:")) != -1) {
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
            case 'C':
                certsd = std::string{ poptarg };
                break;
            case 'P':
                passwd = std::string{ poptarg };
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
    (info{ } << fmt("Params:{} -p {} -c {} -d -C {} -P {}") << (suppress ? " -s" : "") << port << config << certsd << passwd).log(SOURCE_LOCATION);

    if (error && !suppress) {
        (info{ } << fmt("Could not initialize the program. Double chek the command line argument.")).log(SOURCE_LOCATION);
    }

    // load the INI file
    auto conf = INIFile{ config };
    error = /*!conf.parse(CoreElement::name) ||*/ !conf.parse("DB", true) || !conf.parse("CERT", true) || !conf.parse("HTTP", true);

    conf.prepend("DB", conf::def::dbconf); // -d "host=127.0.0.1 port=5432 dbname=arrowhead user=root passwd=root"
    conf.append("DB", dbconf);

    if (error) {
        throw std::runtime_error{ "Cannot parse the INI file." };
    }

    // the port and the numer of threads
    conf::getHTTPConfig(conf, &port, &thrd);

    // create a pool of database connection
    auto pool = conf::createDBPool<db::DatabasePool<db::MariaDB>>(conf);

    //  create the key provider
    auto keyProvider = conf::createKeyProvider(conf, certsd, passwd);

    // create the request builder
    CoreElement::WebGet reqBuilder{ keyProvider };

    // the core element
    auto coreElement = CoreElement::Type<db::DatabasePool<db::MariaDB>, CoreElement::WebGet>{ pool, reqBuilder };

    // the http(s) server
    auto http = HTTPSServer<CoreElement::DispatcherType<db::DatabasePool<db::MariaDB>, CoreElement::WebGet>>("127.0.0.1", static_cast<std::size_t>(port), coreElement, keyProvider /*, thrd*/);

    // we do not need the inner structures of the INIFile anymore; free the interna structures
    conf.close();

    // ------------------***--- RUN ---***------------------ //
    http.run();
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
              << "  -C CERTDIR    Sets the certificae directory.\n"
              << "  -P PSSWD      Password used for the certificates.\n"
              << "\n";
}