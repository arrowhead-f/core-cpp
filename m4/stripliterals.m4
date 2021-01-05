dnl check for a --with-logger configure option and
dnl the program with the requested logger
dnl
dnl CONFIGURE_STRIPLITERALS()
dnl
AC_DEFUN([CONFIGURE_STRIPLITERALS], [
  AC_MSG_CHECKING(strip extra string literals)

  AC_ARG_WITH([strip-literals],
    [AS_HELP_STRING([--with-strip-literals], [strip extra string literals])],
    [
        AC_MSG_RESULT([yes])
        AC_DEFINE([STRIP_LITERALS], [], [Strip extra string literals])
    ],
    [
      AC_MSG_RESULT([no])
    ])
])
