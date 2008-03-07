# Process this file with autoconf to produce a configure script.
AC_INIT(juman-utf8, 6.0, nl-resource@nlp.kuee.kyoto-u.ac.jp)
AC_PREREQ(2.53)
# AC_REVISION($Revision$)
AC_CONFIG_SRCDIR([juman/juman.c])
DATE=`LANG=C date +%Y%m%d`
AM_INIT_AUTOMAKE(${PACKAGE_NAME}, ${PACKAGE_VERSION}-${DATE})
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
# AC_PROG_RANLIB
AC_PROG_LIBTOOL

LIBJUMAN_VERSION_INFO="4:0:0"
AC_SUBST(LIBJUMAN_VERSION_INFO)

# Checks for libraries.
AC_CHECK_LIB(socket, socket, NET_LIBS="-lsocket $NET_LIBS")
AC_CHECK_LIB(nsl, gethostbyname, NET_LIBS="-lnsl $NET_LIBS")
AC_SUBST(NET_LIBS)

AC_CHECK_LIB(icuuc, ucnv_open_3_8, ICU_LIBS="-licuuc $ICU_LIBS", AC_MSG_ERROR([ICU library is not found. Please install ICU library (http://www.icu-project.org/).]))
AC_CHECK_LIB(icui18n, ucnv_fromUChars_3_8, ICU_LIBS="-licui18n $ICU_LIBS")
AC_SUBST(ICU_LIBS)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stdlib.h string.h sys/file.h sys/socket.h unistd.h windows.h winsock.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([getcwd gethostbyname memset socket strcasecmp strchr strdup])


# Set additional definitions.
AC_DEFINE_UNQUOTED(NUMERIC_P, 1, [Define numeric_p])
AC_DEFINE_UNQUOTED(THROUGH_P, 1, [Define through_p])

RC_DEFAULT="$sysconfdir/jumanrc"

# Set RC_DEFAULT in config.h.
if test "x$prefix" = xNONE; then
   prefix="${ac_default_prefix}"
fi
if test "x$exec_prefix" = xNONE; then
   exec_prefix='${prefix}'
fi

eval RC_DEFAULT=`eval echo "$RC_DEFAULT"`
AC_DEFINE_UNQUOTED(RC_DEFAULT, "$RC_DEFAULT", [Define to the location of "jumanrc" file.])


AC_CONFIG_FILES([Makefile lib/Makefile makemat/Makefile makeint/Makefile makepat/Makefile juman/Makefile dic/Makefile doc/Makefile])
AC_OUTPUT
