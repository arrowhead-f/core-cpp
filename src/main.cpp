#include <cstring>
#include <iostream>
#include <cstdlib>

/* utility functions */
#include "utils/logger.h"
#include "utils/pgetopt.h"
#include "utils/parsers.h"
#include "utils/traits.h"

/* the database adapter */
#include "db/DB.h"
#include "db/MariaDB.h"

/* core system elements */
#include "core/ServiceRegistry/ServiceRegistry.h"
#include "core/Authorizer/Authorizer.h"
#include "core/CertAuthority/CertAuthority.h"

/* connections*/
#include "http/KeyProvider.h"
#include "http/ReqBuilder.h"
#include "http/req/RB_Curl.h"
#include "http/HTTPServerBuilder.h"

/* this should be the last header included */
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif


/// Print help message to the standard output.
void print_hlp(const char*, const char*);


int main(int argc, char *argv[]) try {

    // parse command line arguments
    int ch = 0;
    poptarg = nullptr;

    // error flag for command line parsing
    bool error = false;

    bool suppress = false;  // whether to suppers command line parsing errors
    int port      = 16223;  // the listening port

    std::string dbconf;    // the configuration string for the database
    std::string config;    // the location/name of the config file
    std::string certsd;    // the directory that stores the certificates
    std::string passwd;    // the passphare for the certificates


    while((ch = pgetopt (argc, argv, "hvsc:d:p:C:P:")) != -1) {
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
                cerstd = std::string{ poptarg };
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
    (info{ } << fmt("Params:{} -p {} -c {} -d -C -P") << (suppress ? " -s" : "") << port << cfile).log(SOURCE_LOCATION);

    if (error && !suppress) {
        (info{ } << fmt("Could not initialize the program. Double chek the command line argument.")).log(SOURCE_LOCATION);
    }

    // process config file here

    // create a pool of database connection
    // -d "host=localhost port=5432 dbname=mydb user=root password=root"
    //auto dconfmap = parser::parseOptions(dconf.c_str());
    //db::DatabasePool<db::MariaDB> pool{ dconfmap["host"].c_str(), dconfmap["user"].c_str(), dconfmap["password"].c_str(), dconfmap["dbname"].c_str() };

    db::DatabasePool<db::MariaDB> pool{ "127.0.0.1", "root", "root", "arrowhead" };

    //  create the key provider
    http::KeyProvider keyProvider{ "../keys/tempsensor.testcloud1.publicCert.pem",
                                   "PEM",
                                   "../keys/tempsensor.testcloud1.private.key",
                                   "PEM",
                                   "12345",
                                   "../keys/tempsensor.testcloud1.caCert.pem" };

    http::RB_Curl reqBuilder{ keyProvider };

    // create core system element
    auto coreElement = CoreElement::Type<db::DatabasePool<db::MariaDB>, http::RB_Curl>{ pool, reqBuilder };

    // create the server
    auto http = http::HTTPServerBuilder::create<CoreElement::DispatcherType<db::DatabasePool<db::MariaDB>, http::RB_Curl>>("127.0.0.1", static_cast<std::size_t>(port), coreElement, keyProvider, 4);

    http.run();

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
              << "  -C CERTDIR    Sets the certificae directory.\n"
              << "  -P PSSWD      Password used for the certificates.\n"
              << "\n";
}
