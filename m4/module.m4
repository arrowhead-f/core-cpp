dnl check for a --with-module configure option and set up
dnl the tool to create the given module
dnl
dnl CONFIGURE_MODULE()
dnl
AC_DEFUN([CONFIGURE_MODULE], [
  AC_MSG_CHECKING(for module)

  AC_ARG_WITH(module,
    [AS_HELP_STRING([--with-module], [the name of the module])],
    [
      case "${withval}" in
          ServiceRegistry) AC_DEFINE(MODULE, ServiceRegistry)
                           AC_MSG_RESULT([yes])
                           ;;
          *)
                           AC_MSG_RESULT([no])
                           AC_MSG_ERROR([Unknown module.])
                           ;;
      esac
    ],
    [
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([Configure a module with the --with-module flag.])
    ])
])
