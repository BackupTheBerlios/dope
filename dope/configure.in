dnl process this file with autoconf to produce a configure script
AC_PREREQ(2.53)
AC_INIT
AC_CONFIG_SRCDIR([dope/dope.h])

dnl
dnl Source packaging numbers
DOPE_MAJOR_VERSION=0
DOPE_MINOR_VERSION=0
DOPE_MICRO_VERSION=4
DOPE_VERSION=$DOPE_MAJOR_VERSION.$DOPE_MINOR_VERSION.$DOPE_MICRO_VERSION
AC_DEFINE_UNQUOTED(DOPE_MAJOR_VERSION, $DOPE_MAJOR_VERSION, [major version])
AC_DEFINE_UNQUOTED(DOPE_MINOR_VERSION, $DOPE_MINOR_VERSION, [minor version])
AC_DEFINE_UNQUOTED(DOPE_MICRO_VERSION, $DOPE_MICRO_VERSION, [micro version])


dnl for automake
VERSION=$DOPE_VERSION
PACKAGE=dope
dnl set the libtool version see info libtool
DOPE_LTVERSION="0:0:0"
AC_SUBST(DOPE_LTVERSION)

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_CONFIG_HEADER(dope/dopeconfig.h)
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AM_PROG_LIBTOOL

dnl build options
AC_ARG_ENABLE(ilibdep, [  --enable-inter-libdep            enable inter-library dependencies [default=no]], , enable_interlibdep=no)

dnl checks for libraries

dnl added for SunOS
AC_SEARCH_LIBS(bind, socket)
AC_SEARCH_LIBS(inet_pton, nsl)

AM_PATH_XML2(2.4.0, , [AC_MSG_ERROR([libxml required])])
AM_PATH_SIGC(1.0.0, , [AC_MSG_ERROR([libsigc++ required])])

dnl checks for c++ header files
AC_LANG([C++])
DOPE_USE_BOOST=0
AC_CHECK_HEADER(boost/smart_ptr.hpp, DOPE_USE_BOOST=1 , [AC_MSG_ERROR([boost::shared_ptr required - either install boost or write a replacement])])
AC_SUBST(DOPE_USE_BOOST)

dnl checks for library functions
AC_LANG([C])
dnl todo: abort if any of them is missing
AC_CHECK_FUNCS(gettimeofday inet_pton select strerror)
AC_FUNC_SELECT_ARGTYPES

dnl variable substitutions
DOPE_LDADD="$XML_LIBS $SIGC_LIBS"
echo "libdope depends upon: $DOPE_LDADD"
if test "x$enable_interlibdep" = "xno"; then
     TEST_LDADD="$DOPE_LDADD"
     DOPE_LDADD=""
     echo "Inter-library dependencies tracking via libtool is disabled -> use dope-config instead"
fi

DOPE_CFLAGS="$SIGC_CFLAGS $XML_CFLAGS"
echo DOPE_CFLAGS = $DOPE_CFLAGS
AC_SUBST(DOPE_LDADD)
AC_SUBST(TEST_LDADD)
AC_SUBST(DOPE_CFLAGS)


dnl output files
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile config/Makefile config/m4/Makefile scripts/Makefile scripts/dope-config dope/Makefile dope/dope.h tests/Makefile])
AC_OUTPUT

