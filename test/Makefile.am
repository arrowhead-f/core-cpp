bin_PROGRAMS = test

noinst_LIBRARIES = libsqlite3.a
libsqlite3_a_SOURCES = ../inc/sqlite3/sqlite3.c

AM_CPPFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src -include boot/features.h -DTESTRUNNER
AM_LDFLAGS = $(shell mariadb_config --libs) -lcurl -ldl

test_LDADD = libsqlite3.a

test_SOURCES = main.cpp                  \
               test0001.cpp              \
               test0002.cpp              \
               test0051.cpp              \
               test0052.cpp              \
               test0053.cpp              \
               test0054.cpp              \
               test0055.cpp              \
               test0056.cpp              \
               test0058.cpp              \
               test0059.cpp              \
               test0093.cpp              \
               test0100.cpp              \
               test0102.cpp              \
               test0103.cpp              \
               test0104.cpp              \
               test0105.cpp              \
               test0106.cpp              \
               test0108.cpp              \
               test0109.cpp              \
               test0111.cpp              \
               test0112.cpp              \
               test0114.cpp              \
               test0115.cpp              \
               test0117.cpp              \
               test0118.cpp              \
               test0120.cpp              \
               test0121.cpp              \
               test0123.cpp              \
               test0124.cpp              \
               test0126.cpp              \
               test0127.cpp              \
               test0129.cpp              \
               test0130.cpp              \
               test0132.cpp              \
               test0133.cpp              \
               test0501.cpp              \
               test0502.cpp              \
               test0503.cpp              \
               test1000.cpp              \
               test1001.cpp              \
               test1002.cpp              \
               test1003.cpp              \
               test1004.cpp              \
               \
               \
               \
               ../inc/json11/json11.cpp            \
               ../inc/gason/gason.cpp              \
               \
               ../src/utils/inifile.cpp            \
               ../src/http/wget/WG_Curl.cpp        \
               ../src/http/crate/Response.cpp      \
               ../src/http/crate/Uri.cpp           \
               ../src/http/crate/X509Cert.cpp      \
               ../src/http/serv/RequestParser.cpp
