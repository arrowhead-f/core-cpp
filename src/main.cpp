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
void print_hlp();


int main(int argc, char *argv[]) try {

    // initialize looger
    #define _____xstr(s) _____str(s)
    #define _____str(s) #s
      logger::init(LOG_DEBUG, _____xstr(COREELEMENT), "logfile");
    #undef _____xstr
    #undef _____str

    (info{ } << fmt("started")).log(SOURCE_LOCATION);

    // parse command line arguments
    int ch = 0;
    optarg = nullptr;

    std::string dconf;    // the configuration string for the database
    std::string cfile;    // the name of the config file
    int port = 16223;     // the port used with the core element

    while((ch = pgetopt (argc, argv, "hc:p:d:")) != -1) {
        std::cout << (char)ch << "\n";
        switch (ch) {
            case 'h':
                print_hlp();
                break;
            case 'c':
                cfile = std::string{ optarg };
                break;
            case 'd':
                dconf = std::string{ optarg };
                break;
            case 'p':
                port = std::stoi(optarg);
                break;
            case '?':
                /* some error messaging */
                return 1;
            default:
                return 2;
        }
    }

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
    auto coreElement = CoreElement<db::DatabasePool<db::MariaDB>, http::RB_Curl>::Type{ pool, reqBuilder };

    auto http = http::HTTPServerBuilder::create<CoreElement<db::DatabasePool<db::MariaDB>, http::RB_Curl>::DispatcherType>("127.0.0.1", static_cast<std::size_t>(port), coreElement, keyProvider, 4);

    http.run();

    return 0;

}
catch(const std::exception &e) {
    (error{ } << fmt("Execution terminated with exception. Exception: {}") << e.what()).log(SOURCE_LOCATION);
}
catch(...) {
    (error{ } << fmt("Execution terminated with unexpected error.")).log(SOURCE_LOCATION);
}


/*----------------------------------------------------------------------------------------------------*/


void print_hlp() {
    std::cout << "dododo-dadada\n";
}
