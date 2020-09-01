bin_PROGRAMS = service_registry

#bin_PROGRAMS = dummy_registry service_registry

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/inc
#${ARROWHEAD_CPPFLAGS}
#AM_CXXFLAGS = ${ARROWHEAD_CXXFLAGS}
AM_LDFLAGS = $(shell mariadb_config --libs) -lmicrohttpd -lcurl -ljson-c

#AM_CXXFLAGS = $(INTI_CFLAGS)
#ArrowheadSrv_LDADD = $(INTI_LIBS)

sources = main.cpp                         \
          db/MariaDB.cpp                   \
          utils/pgetopt.cpp                \
          utils/parsers.cpp                \
          net/mhttp/MHTTPHandler.cpp       \
          net/request/ReqBuilderWCurl.cpp  \
          json/ServiceQueryForm.cpp        \
          json/ServiceQueryList.cpp        \
          json/ServiceRegistryEntry.cpp

service_registry_CPPFLAGS = -DCOREELEMENT=ServiceRegistry
service_registry_SOURCES = $(sources)

#dummy_registry_CPPFLAGS = -DCOREELEMENT=DummyRegistry
#dummy_registry_SOURCES = $(sources)
