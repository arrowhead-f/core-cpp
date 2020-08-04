bin_PROGRAMS = arrowheads

AM_CPPFLAGS = -I$(top_srcdir)/inc
AM_LDFLAGS = $(shell mariadb_config --libs)


#AM_CXXFLAGS = $(INTI_CFLAGS)
#ArrowheadSrv_LDADD = $(INTI_LIBS)

arrowheads_SOURCES = main.cpp db/MariaDB.cpp utils/pgetopt.cpp
