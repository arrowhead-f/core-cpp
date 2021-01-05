bin_PROGRAMS = cert_authority authorizer service_registry

noinst_LIBRARIES = libgason.a libjson11.a libutils.a
libgason_a_SOURCES = ../inc/gason/gason.cpp
libjson11_a_SOURCES = ../inc/json11/json11.cpp
libutils_a_SOURCES = utils/pgetopt.cpp utils/inifile.cpp

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/inc -I$(top_srcdir)/src
AM_LDFLAGS = $(shell mariadb_config --libs)

# libs handled automatically by the configure script: -lpthread -lssl -lcrypto -lcurl

sources = main.cpp                         \
          http/wget/WG_Curl.cpp

libraries = libgason.a libjson11.a libutils.a

sources += http/crate/Response.cpp         \
           http/serv/RequestParser.cpp

################################################################################################
################################################################################################
################################################################################################

cert_authority_CPPFLAGS = -include utils/traits/CertAuthority.h $(AM_CPPFLAGS)
cert_authority_SOURCES = $(sources) 
cert_authority_LDADD = $(libraries)

authorizer_CPPFLAGS = -include utils/traits/Authorizer.h $(AM_CPPFLAGS)
authorizer_SOURCES = $(sources)
authorizer_LDADD = $(libraries)

service_registry_CPPFLAGS = -include utils/traits/ServiceRegistry.h $(AM_CPPFLAGS)
service_registry_SOURCES = $(sources)
service_registry_LDADD = $(libraries)


