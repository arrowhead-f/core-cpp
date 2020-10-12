bin_PROGRAMS = test

AM_CPPFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src
AM_LDFLAGS = $(shell mariadb_config --libs) -lmicrohttpd -lcurl -ljson-c

#AM_LDFLAGS = $(shell mariadb_config --libs)


#AM_CXXFLAGS = $(INTI_CFLAGS)
#ArrowheadSrv_LDADD = $(INTI_LIBS)

test_SOURCES = main.cpp                  \
               test0052.cpp              \
               test0053.cpp              \
               test0125.cpp              \
               ../src/utils/parsers.cpp  \
               ../src/db/MariaDB.cpp                   \
               ../src/net/request/ReqBuilderWCurl.cpp  \
               ../src/json/ServiceQueryForm.cpp        \
               ../src/json/ServiceQueryList.cpp        \
               ../src/json/ServiceRegistryEntry.cpp    \
               ../src/json/System.cpp
