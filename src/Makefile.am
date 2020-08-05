bin_PROGRAMS = dummy_registry service_registry

AM_CPPFLAGS = -I$(top_srcdir)/inc
AM_LDFLAGS = $(shell mariadb_config --libs)


#AM_CXXFLAGS = $(INTI_CFLAGS)
#ArrowheadSrv_LDADD = $(INTI_LIBS)

sources = main.cpp db/MariaDB.cpp utils/pgetopt.cpp

service_registry_CPPFLAGS = -DMODULE=ServiceRegistry
service_registry_SOURCES = $(sources)

dummy_registry_CPPFLAGS = -DMODULE=DummyRegistry
dummy_registry_SOURCES = $(sources)
