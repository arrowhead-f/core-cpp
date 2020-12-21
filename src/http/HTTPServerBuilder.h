#ifndef _ARROWHEAD_HTTP_HTTPSERVERBUILDER_H_
#define _ARROWHEAD_HTTP_HTTPSERVERBUILDER_H_

#include "config.h"

#include <string>

#include "KeyProvider.h"
#ifdef USE_IOPOOL
  #include "http/srv/iopool/Server.h"
#endif
#ifdef USE_THPOOL
  #include "http/srv/thpool/Server.h"
#endif
#ifdef USE_SRVLET
  #include "http/srv/srvlet/Server.h"
#endif


namespace http {

    namespace HTTPServerBuilder {

        // make decision based on the settings...
        template<typename T>
        auto create(const std::string &address, std::size_t port, T &coreElement, std::size_t threads) {
            #ifdef USE_IOPOOL
              return http::server::iopool::HTTPServer<T>{ address, port, coreElement, threads };
            #endif
            #ifdef USE_THPOOL
              return http::server::thpool::HTTPServer<T>{ address, port, coreElement, threads };
            #endif

        }

        template<typename T>
        auto create(const std::string &address, std::size_t port, T &coreElement, KeyProvider &keyProvider, std::size_t threads) {
            #ifdef USE_IOPOOL
              return http::server::iopool::HTTPSServer<T>{ address, port, coreElement, keyProvider, threads };
            #endif
            #ifdef USE_THPOOL
              return http::server::thpool::HTTPSServer<T>{ address, port, coreElement, keyProvider, threads };
            #endif
            #ifdef USE_SRVLET
              return http::server::srvlet::HTTPSServer<T>{ address, port, coreElement, keyProvider };
            #endif
        }

    }  // namespace HTTPServerBuilder

}  // namespace http

#endif  /* _ARROWHEAD_HTTP_HTTPSERVERBUILDER_H_ */

