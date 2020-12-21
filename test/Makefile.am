bin_PROGRAMS = test

noinst_LIBRARIES = libsqlite3.a
libsqlite3_a_SOURCES = ../inc/sqlite3/sqlite3.c

AM_CPPFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src -DTESTRUNNER
AM_LDFLAGS = $(shell mariadb_config --libs) -lcurl -ldl

test_LDADD = libsqlite3.a

test_SOURCES = main.cpp                  \
               test0001.cpp              \
               test0002.cpp              \
               test0052.cpp              \
               test0053.cpp              \
               test0054.cpp              \
               test0055.cpp              \
               test0056.cpp              \
               test0093.cpp              \
               test0102.cpp              \
               test0103.cpp              \
               test0104.cpp              \
               test0125.cpp              \
               test1000.cpp              \
               test1001.cpp              \
               test1002.cpp              \
               test1003.cpp              \
               test1004.cpp              \
               \
               ../inc/json11/json11.cpp \
               \
               ../src/utils/parsers.cpp  \
               ../src/http/req/RB_Curl.cpp

