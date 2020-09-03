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
          devlog)
                    AC_MSG_RESULT([yes])
                    AC_CHECK_HEADER([cstdio], [
                                                 AC_MSG_CHECKING(for std::printf...)
                                                 AC_CACHE_VAL(ac_cv_std_printf,
                                                     AC_TRY_RUN([
                                                         #include <cstdio>
                                                         int main() { int x = std::printf(""); return 0; }],
                                                     ac_cv_std_printf=yes,
                                                     ac_cv_std_printf=no,
                                                     ac_cv_std_printf=no))
                                                 AC_MSG_RESULT($ac_cv_std_printf)
                                                 if test "x$ac_cv_std_printf" = "xyes"; then
                                                     use_devlog=yes
                                                 else
                                                     use_devlog=no
                                                 fi
                                               ])
                    AS_IF([test "x$use_devlog" = "xno"],
                        [AC_MSG_ERROR([devlog requested but not found])],
                        [AC_DEFINE([USE_DEVLOG], [], [Log to screen])]
                    )
                    ;;
          *)
                    AC_MSG_RESULT([no])
                    AC_MSG_ERROR([Unknown logger.])
                    ;;
      esac
    ],
    [
      AC_MSG_RESULT([yes])
      # configure default logger
    ])
])
