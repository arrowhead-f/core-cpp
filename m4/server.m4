dnl check for a --with-logger configure option and
dnl the program with the requested logger
dnl
dnl CONFIGURE_HTTPSERVER()
dnl
AC_DEFUN([CONFIGURE_HTTPSERVER], [
  AC_MSG_CHECKING(http server's presence)

  AC_ARG_WITH(server,
    [AS_HELP_STRING([--with-server@<:@=ARG@:>@], [use ARG as http server @<:@ARG=iopool@:>@])],

    [
        case "${withval}" in
          iopool)
                    AC_MSG_RESULT([yes])
                    AC_MSG_CHECKING([IOPool])
                    AC_MSG_RESULT([yes])
                    AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_FAILURE([pthread is needed for use threading])])
                    AC_DEFINE([USE_IOPOOL], [], [Use iopool based https server])
                    ;;
          thpool)
                    AC_MSG_RESULT([yes])
                    AC_MSG_CHECKING([THPool])
                    AC_MSG_RESULT([yes])
                    AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_FAILURE([pthread is needed for use threading])])
                    AC_DEFINE([USE_THPOOL], [], [Use iopool based https server])
                    ;;
          srvlet)
                    AC_MSG_RESULT([yes])
                    #AC_CHECK_HEADER([microhttpd.h], [], [AC_MSG_FAILURE([mhttpd was selected but could not found])])

                    AC_CHECK_HEADER([unistd.h], [], [AC_MSG_FAILURE([Could not find mandatory unistd.h.])])
                    AC_CHECK_FUNCS([pipe read write])

                    AC_CHECK_HEADER([sys/signalfd.h])
                    AC_CHECK_FUNCS([signalfd])

                    #AC_CHECK_LIB([microhttpd], [MHD_start_daemon], [], [AC_MSG_FAILURE([Could not find microhttpd library])])
                    #AC_CHECK_LIB([microhttpd], [MHD_stop_daemon], [], [AC_MSG_FAILURE([Could not find microhttpd library])])

                    AC_MSG_CHECKING([SRVLet])
                    AC_MSG_RESULT([yes])

                    AC_DEFINE([USE_SRVLET], [], [Use srvlet based https server])

                    ;;
          *)
                    AC_MSG_RESULT([no])
                    AC_MSG_ERROR([Unknown http server.])
                    ;;
        esac

        AM_CONDITIONAL([ENABLE_IOPOOL],[test "x${withval}" = "xiopool"])
        AM_CONDITIONAL([ENABLE_THPOOL],[test "x${withval}" = "xthpool"])
        AM_CONDITIONAL([ENABLE_SRVLET],[test "x${withval}" = "xsrvlet"])

    ],
    [
        AC_MSG_RESULT([yes])
        AM_CONDITIONAL([ENABLE_IOPOOL],[true])
        AM_CONDITIONAL([ENABLE_THPOOL],[false])
        AM_CONDITIONAL([ENABLE_SRVLET],[false])

        AC_MSG_CHECKING([IOPool])
        AC_MSG_RESULT([yes])
        AC_CHECK_LIB([pthread], [pthread_create], [], [AC_MSG_FAILURE([pthread is needed for use threading])])
        AC_DEFINE([USE_IOPOOL], [], [Use iopool based https server])
    ])
])
