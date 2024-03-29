dnl check for a --with-logger configure option and
dnl the program with the requested logger
dnl
dnl CONFIGURE_LOGGER()
dnl
AC_DEFUN([CONFIGURE_LOGGING], [
  AC_MSG_CHECKING(logging method's presence)

  AC_ARG_ENABLE(log,
    [AS_HELP_STRING([--enable-log@<:@=ARG@:>@], [use the logging method give @<:@ARG=syslog@:>@])],
    [
      case "${enable_log}" in
          syslog)
                    AC_MSG_RESULT([yes])
                    AC_CHECK_HEADER([syslog.h], [AC_CHECK_FUNCS([openlog syslog closelog], [], [use_syslog=no])], [use_syslog=no])
                    AS_IF([test "x$use_syslog" = "xno"],
                        [AC_MSG_ERROR([syslog requested but not found])],
                        [AC_DEFINE([USE_SYSLOG], [], [Use syslog for logging])]
                    )
                    ;;
          spdlog)
                    AC_MSG_RESULT([yes])
                    AC_DEFINE([USE_SPDLOG], [], [Use spdlog for logging])
                    ;;
          scrlog)
                    AC_MSG_RESULT([yes])
                    AC_DEFINE([USE_SCRLOG], [], [Use scrlog for logging])
                    ;;
          nillog)
                    AC_MSG_RESULT([yes])
                    AC_DEFINE([USE_NILLOG], [], [Use nillog for logging])
                    ;;
          *)
                    AC_MSG_RESULT([no])
                    AC_MSG_ERROR([Unknown logger.])
                    ;;
      esac
    ],
    [
      AC_MSG_RESULT([yes])
      AC_DEFINE([USE_SCRLOG], [], [Use scrlog for logging])
    ])
])
