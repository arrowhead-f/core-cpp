AC_INIT([arrowheads], 1.0)
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CC
AC_LANG([C++])
AC_PROG_CXX([clang++ g++])
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

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
dnl Check signals and pipes to handle termination
dnl

AC_CHECK_HEADER([unistd.h])
AC_CHECK_FUNCS([pipe read write])

AC_CHECK_HEADER([signal.h])
AC_CHECK_FUNCS([sigaction])


dnl
dnl
dnl


AC_CHECK_HEADERS([microhttpd.h])
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