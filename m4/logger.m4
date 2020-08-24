dnl check for a --with-module configure option and set up
dnl the tool to create the given module
dnl
dnl CONFIGURE_MODULE()
dnl
AC_DEFUN([CONFIGURE_LOGGER], [
  AC_MSG_CHECKING(for logger)

  AC_ARG_WITH(module,
    [AS_HELP_STRING([--with-logger], [the name of the logger])],
    [
      case "${withval}" in
          syslog) AC_MSG_RESULT([yes])
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
