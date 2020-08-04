bin_PROGRAMS = arrowheads

AM_CPPFLAGS = -I$(top_srcdir)/inc
AM_LDFLAGS = $(shell mariadb_config --libs)
AM_LDFLAGS += -lmicrohttpd
AM_LDFLAGS += -lcurl
AM_LDFLAGS += -ljson-c


#AM_CXXFLAGS = $(INTI_CFLAGS)
#ArrowheadSrv_LDADD = $(INTI_LIBS)

arrowheads_SOURCES = main.cpp db/MariaDB.cpp http/MhttpdHandler.cpp json/ServiceQueryForm.cpp json/ServiceQueryList.cpp json/ServiceRegistryEntry.cpp modules/ServiceRegistry/ServiceRegistry.cpp
