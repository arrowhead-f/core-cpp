bin_PROGRAMS = service_registry authorizer cert_authority

noinst_LIBRARIES = libgason.a libjson11.a libutils.a
libgason_a_SOURCES = ../inc/gason/gason.cpp
libjson11_a_SOURCES = ../inc/json11/json11.cpp
libutils_a_SOURCES = utils/pgetopt.cpp utils/inifile.cpp

lib_LTLIBRARIES = libiopool.la libthpool.la
libiopool_la_SOURCES = http/srv/Reply.cpp http/srv/ReplyBuffer.cpp http/srv/RequestHandler.cpp http/srv/RequestParser.cpp http/srv/iopool/Connection.cpp
libthpool_la_SOURCES = http/srv/Reply.cpp http/srv/ReplyBuffer.cpp http/srv/RequestHandler.cpp http/srv/RequestParser.cpp http/srv/thpool/Connection.cpp
libiopool_la_LDFLAGS = -lpthread
libthpool_la_LDFLAGS = -lpthread

#lib_LTLIBRARIES = libarrowheadhttp.la
#libarrowheadhttp_la_LDFLAGS = -lpthread

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/inc -I$(top_srcdir)/inc/asio -I$(top_srcdir)/src -DASIO_HAS_STD_THREAD
AM_LDFLAGS = $(shell mariadb_config --libs) -lcurl -ljson-c

# libs handled automatically by the configure script: -lpthread -lssl -lcrypto -lmicrohttpd

sources = main.cpp                         \
          http/req/RB_Curl.cpp             \
          json/ServiceQueryForm.cpp        \
          json/ServiceQueryList.cpp        \
          json/ServiceRegistryEntry.cpp

libraries = libgason.a libjson11.a libutils.a

if ENABLE_IOPOOL
libraries += libiopool.la
endif

if ENABLE_THPOOL
libraries += libthpool.la
endif

if ENABLE_SRVLET
sources += http/srv/Reply.cpp http/srv/RequestHandler.cpp http/srv/RequestParser.cpp
endif

################################################################################################
################################################################################################
################################################################################################

service_registry_CPPFLAGS = -include utils/traits/ServiceRegistry.h $(AM_CPPFLAGS)
service_registry_SOURCES = $(sources)
service_registry_LDADD = $(libraries)

authorizer_CPPFLAGS = -include utils/traits/Authorizer.h $(AM_CPPFLAGS)
authorizer_SOURCES = $(sources)
authorizer_LDADD = $(libraries)

cert_authority_CPPFLAGS = -include utils/traits/CertAuthority.h $(AM_CPPFLAGS)
cert_authority_SOURCES = $(sources)
cert_authority_LDADD = $(libraries)
