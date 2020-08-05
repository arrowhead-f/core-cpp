AC_INIT([arrowheads], 1.0)
AC_CONFIG_MACRO_DIRS([m4])

#CONFIGURE_MODULE()

AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CXX([clang++ g++])
AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])

AC_DEFINE(UNIX)

AC_CHECK_HEADER([getopt.h])
AC_CHECK_FUNCS([getopt])

#WITH_MYSQL()
#MYSQL_NEED_FORK([mariadb])
#MYSQL_NEED_VERSION([5.5.34])
#MYSQL_USE_CLIENT_API()
#MYSQL_SUBST()

AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT