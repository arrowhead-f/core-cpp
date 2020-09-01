#include <cstring>
#include <iostream>
#include <cstdlib>

/* utility functions */
//#include "utils/log.h"
#include "utils/pgetopt.h"
#include "utils/parsers.h"
#include "utils/traits.h"

/* the database adapter */
#include "db/DB.h"
#include "db/MariaDB.h"

/* core system elements */
#include "core/ServiceRegistry/ServiceRegistry.h"

/* connections*/
#include "net/KeyProvider.h"
#include "net/ReqBuilder.h"
#include "net/mhttp/MHTTPHandler.h"

/* this should be the last header included */
#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#if defined UNIX && defined HAVE_SIGACTION
  // maybe add static asserts with proper erro messages

  #include <signal.h>
  #include <unistd.h>

  int pipefd[2];

  /// Sigaction signal handler
  static void signal_hdl(int, siginfo_t*, void*);
#endif

/// Print help message to the standard output.
void print_hlp();

int main(int argc, char *argv[]) {

    //(notice{} << "STARTED").log();

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

    db::DatabasePool<db::MariaDB> pool{ "127.0.0.1", "root", "root", "capi" };

    //  create the key provider
    KeyProvider keyProvider{ "../keys/tempsensor.testcloud1.publicCert.pem",
                             "PEM",
                             "../keys/tempsensor.testcloud1.private.key",
                             "PEM",
                             "12345",
                             "../keys/tempsensor.testcloud1.caCert.pem" };

    ReqBuilder reqBuilder{ keyProvider };

    // create core system element
    CoreElement<CoreElementType::COREELEMENT, db::DatabasePool<db::MariaDB>>::Type coreElement { pool, reqBuilder };

    // create networking
    auto http = MHTTPHandler<CoreElement<CoreElementType::COREELEMENT, db::DatabasePool<db::MariaDB>>::Type>{ static_cast<std::size_t>(port), coreElement };

    if(!http.start()) {
        // error starting the server
        return 1;
    }

    #if defined UNIX && defined HAVE_SIGACTION
    {
        // rationale:
        //   * a signal is caught
        //   * a message is sent through the pipe
        //   * the receive message forces the program to stop

        // create the pipe
        if(pipe(pipefd) == -1) {
            // error
            // stop the http
            http.stop();
            return 1;
        }

        // install the signal handlers
        struct sigaction act;
        std::memset(&act, 0, sizeof(act));
        act.sa_sigaction = signal_hdl;
        act.sa_flags = SA_SIGINFO;

        if(sigaction(SIGTERM, &act, nullptr) < 0) {
            // cannot install signal handler error
            return 1;
        }

        if(sigaction(SIGINT, &act, nullptr) < 0) {
            // cannot install signal handler error
            return 1;
        }

        char buf;         // buffer for the data read through the pipe
        ssize_t ret = 0;  // the return value of teh reading from the pipe

        // signals may interrupt this system call
        while ((ret = read(pipefd[0], &buf, 1)) == -1 && errno == EINTR)
            continue;

        // stop the server
        http.stop();

        // close pipe
        close(pipefd[0]);
        close(pipefd[1]);

        // emit error message and exit
        switch(ret) {
            case 1:
                    // normal termination
                    return 0;
            default:
                    // emit error message
                    return 1;
        }
    }
    #endif

    return 0;
}

void print_hlp() {
    std::cout << "dododo-dadada\n";
}

#if defined UNIX && defined HAVE_SIGACTION
  void signal_hdl(int sig, siginfo_t*, void*) {
      switch(sig) {
           case SIGTERM:
           case SIGINT:
               write(pipefd[1], "1", 1);
               break;
      }
  }
#endif
