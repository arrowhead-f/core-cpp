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


#ARROWHEAD_CXXFLAGS="-Wall -Wextra"
#AC_SUBST([ARROWHEAD_CXXFLAGS])

#ARROWHEAD_CPPFLAGS="-DUNIX"
#AC_SUBST([ARROWHEAD_CPPFLAGS])

AC_HEADER_STDC

dnl
dnl Configure the logger
dnl
CONFIGURE_LOGGING


dnl
dnl Configure command line parameter parser
dnl
AC_CHECK_HEADER([getopt.h])
AC_CHECK_FUNCS([getopt])

dnl
dnl Configure the http server
dnl
CONFIGURE_HTTPSERVER

AC_ARG_ENABLE([https],
    [AS_HELP_STRING([--enable-https],
                [enable HTTPS support (yes, no, auto)[auto]])],
    [enable_https=${enableval}])
AS_IF([test "x$enable_https" != "xno"], [
    AC_CHECK_LIB([ssl],    [SSL_new],        [], [AC_MSG_FAILURE([Could not find ssl])])
    AC_CHECK_LIB([crypto], [OPENSSL_config], [], [AC_MSG_FAILURE([Could not find crypto])])
    AM_CONDITIONAL([ENABLE_HTTPS],[true])
], [
    AM_CONDITIONAL([ENABLE_HTTPS],[false])
])



#AC_ARG_ENABLE([sharedhttp],
#    [AS_HELP_STRING([--enable-sharedhttp],
#                [use shared HTTP library (yes, no, auto)[auto]])],
#    [enable_sharedhttp=${enableval}])
#AS_IF([test "x$enable_sharedhttp" == "xyes"], [
#   LT_INIT
#    AM_CONDITIONAL([ENABLE_SHAREDHTTP],[true])
#    AC_DEFINE([USE_SHAREDHTTP], [], [Compile http library as so])
#], [
#    AM_CONDITIONAL([ENABLE_SHAREDHTTP],[false])
#])






#AC_CHECK_HEADER([unistd.h])
#AC_CHECK_FUNCS([pipe read write])

#AC_CHECK_HEADER([signal.h])
#AC_CHECK_FUNCS([sigaction])


dnl
dnl
dnl


#AC_CHECK_HEADERS([microhttpd.h])
#AC_CHECK_FUNCS([MHD_start_daemon MHD_stop_daemon])

AC_CHECK_HEADERS([curl/curl.h])
#AC_CHECK_FUNCS([MHD_start_daemon MHD_stop_daemon])


#WITH_MYSQL()
#MYSQL_NEED_FORK([mariadb])
#MYSQL_NEED_VERSION([5.5.34])
#MYSQL_USE_CLIENT_API()
#MYSQL_SUBST()

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT