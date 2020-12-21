#include "core/Dispatcher.h"

#ifdef USE_IOPOOL
  template<>class http::server::iopool::HTTPServer<Dispatcher>;
  template<>class http::server::iopool::HTTPSServer<Dispatcher>;
#endif

#ifdef USE_THPOOL
  template<>class http::server::thpool::HTTPServer<Dispatcher>;
  template<>class http::server::thpool::HTTPSServer<Dispatcher>
#endif

#ifdef USE_SERVLET
  template<>class http::server::servlet::HTTPSServer<Dispatcher>
#endif
