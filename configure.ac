AC_INIT([arrowheads], 1.0)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_LANG([C++])
AC_PROG_CXX([clang++ g++])
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

LT_INIT

AC_DEFINE([UNIX], [], [Unix/Linux])

AC_HEADER_STDC

dnl
dnl Configure the logger
dnl
CONFIGURE_LOGGING

dnl
dnl Configure veryslim
dnl
CONFIGURE_STRIPLITERALS

dnl
dnl Configure command line parameter parser
dnl
AC_CHECK_HEADER([getopt.h])
AC_CHECK_FUNCS([getopt])

dnl
dnl Check case insensitive comparison functions
dnl
AC_CHECK_HEADER([strings.h])
AC_CHECK_FUNCS([strcasecmp])

dnl
dnl Configure the server
dnl
AC_CHECK_HEADER([unistd.h], [], [AC_MSG_FAILURE([Could not find mandatory unistd.h.])])
AC_CHECK_FUNCS([pipe read write])

AC_CHECK_HEADER([sys/signalfd.h])
AC_CHECK_FUNCS([signalfd])

AC_CHECK_LIB([ssl],    [SSL_new],        [], [AC_MSG_FAILURE([Could not find ssl])])
AC_CHECK_LIB([crypto], [OPENSSL_config], [], [AC_MSG_FAILURE([Could not find crypto])])

dnl
dnl Configure WebGet
dnl
AC_CHECK_HEADERS([curl/curl.h])
AC_CHECK_LIB([curl], [curl_easy_perform], [], [AC_MSG_FAILURE([Could not find curl])])




#AM_CONDITIONAL([ENABLE_HTTPS],[true])


#WITH_MYSQL()
#MYSQL_NEED_FORK([mariadb])
#MYSQL_NEED_VERSION([5.5.34])
#MYSQL_USE_CLIENT_API()
#MYSQL_SUBST()

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT