dnl process this file with autoconf to produce a configure script
AC_PREREQ(2.53)
AC_INIT
AC_CONFIG_SRCDIR([dope/dope.cpp])

dnl
dnl Source packaging numbers
DOPE_MAJOR_VERSION=0
DOPE_MINOR_VERSION=1
DOPE_MICRO_VERSION=1
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
dnl the configuration file - this is architecture dependant - does not get installed
dnl and must not be included from files getting installed
dnl the architecture dependant header is <dope/arch.h> (generated from arch.h.in)
dnl and gets installed in /usr/lib and not in /usr/include 
dnl (why? the fhs does not say this - hmm I always thought
dnl /usr/include should only contain architecture independent headers)
AM_CONFIG_HEADER(dope/dopeconfig.h)
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CPP

dnl byte order - todo: we need this in the header bswap.h (problem see above)
DOPE_BYTE_ORDER=DOPE_LITTLE_ENDIAN
AC_C_BIGENDIAN(
	[
		DOPE_BYTE_ORDER=DOPE_BIG_ENDIAN
	])
AC_SUBST(DOPE_BYTE_ORDER)

dnl this sets GXX to yes if we use g++
AC_PROG_CXX
AC_PROG_CXXCPP

dnl test for old gcc (<3) - we need >=3
if test "x$GXX" = "xyes"; then
	AC_LANG([C++])
	AC_MSG_CHECKING([for gcc version >=3.x])
	AC_TRY_COMPILE(, [
		#if defined (__GNUC__)
			#if (__GNUC__ == 2)
				#error "Compiler too old"
			#else
				#if (__GNUC__ == 3)
					// bene
				#else
					#error "Unknown gcc compiler version"
				#endif
			#endif
		#else
			// no gcc
		#endif
	], [AC_MSG_RESULT([yes])], [AC_MSG_ERROR([gcc is too old - we need gcc >=3])])
	AC_LANG([C])
fi


dnl build options
AC_ARG_ENABLE([ilibdep],
	AC_HELP_STRING([--enable-ilibdep],
		[enable libtool inter-library dependencies @<:@default=no@:>@]),
		[],
		[enable_ilibdep=no])

AC_ARG_ENABLE([karmehack],
	AC_HELP_STRING([--enable-karmehack],
		[enable libtool problem workaround @<:@default=no@:>@]),
		[],
		[enable_karmehack=no])

dnl checks for libraries

dnl added for SunOS
dnl unfortunately we cannot use this to detect wsock32
AC_SEARCH_LIBS(socket, [socket bsd], [found_socket=1])
AC_SEARCH_LIBS(inet_pton, nsl)

dnl if we have libwsock32 use it - this is not as nice as AC_SEARCH_LIBS
dnl but it seems to be the only solution in the moment
if test "x$found_socket" = "x"; then
	AC_MSG_CHECKING([for socket in -lwsock32])
	AC_LANG([C++])
	LIBS_BACKUP="$LIBS"
	LIBS="$LIBS -lwsock32"
	AC_TRY_LINK([
		#if defined(__WIN32__) || defined(WIN32)
			#define __USE_W32_SOCKETS
			#include <windows.h>
		#endif
		],
		[
			#ifndef WIN32
				thisfunctiondoesnotexist();
			#endif
			int  handle = socket (PF_INET, SOCK_STREAM, 0);
		],
		[AC_MSG_RESULT([yes])],
		[
			AC_MSG_RESULT([no])
		        LIBS="$LIBS_BACKUP"
		]
	)
fi
dnl check if we have libwinmm - and use it (see above why this is bad)
AC_MSG_CHECKING([for timeGetTime in -lwinmm])
AC_LANG([C++])
LIBS_BACKUP="$LIBS"
LIBS="$LIBS -lwinmm"
AC_TRY_LINK([
	#include <windows.h>
	],
	[
		#ifndef WIN32
			thisfunctiondoesnotexist();
		#endif
		unsigned long n=timeGetTime();
	],
	[AC_MSG_RESULT([yes])],
	[
		AC_MSG_RESULT([no])
	        LIBS="$LIBS_BACKUP"
	]
)


AC_LANG([C])
AM_PATH_XML2(2.4.0, , [AC_MSG_ERROR([libxml required])])
AM_PATH_SIGC(1.0.0, , [AC_MSG_ERROR([libsigc++ required])])


dnl check for c headers
AC_CHECK_HEADERS([execinfo.h])

dnl checks for c++ header files
dnl AC_LANG([C++])

dnl the needed parts of boost are now included in dope
dnl DOPE_USE_BOOST=0
dnl AC_CHECK_HEADER(boost/smart_ptr.hpp, DOPE_USE_BOOST=1 , [AC_MSG_ERROR([boost::shared_ptr required - either install boost or write a replacement])])
dnl AC_SUBST(DOPE_USE_BOOST)

dnl checks for library functions
AC_LANG([C])
dnl todo: abort if any of them is missing
AC_CHECK_FUNCS(gettimeofday inet_pton select strerror)
AC_FUNC_SELECT_ARGTYPES

dnl variable substitutions
dnl note order is important because of libtool
DOPE_LDADD="$SIGC_LIBS $XML_LIBS $LIBS"
TEST_LDADD=$DOPE_LDADD
DOPE_LIBS=$DOPE_LDADD

if test "x$enable_karmehack" = "xno"; then
	dnl libtool libraries if possible
	DOPE_LTLIBS=$DOPE_LDADD
else
	echo use hack to get the right sigc lib
	lib=`$SIGC_CONFIG --libs-dirs|sed 's/-L//'`
    	if test x$lib != x; then
		SIGC_LALIB=$lib/lib`$SIGC_CONFIG $sigc_config_args --libs-names|sed -n 's/-l\(sig\w*\).*/\1/p'`.la
    	else
    		SIGC_LALIB=$SIGC_LIBS
    	fi
    	echo SIGC_LALIB=$SIGC_LALIB
	DOPE_LTLIBS="$SIGC_LALIB $XML_LIBS"
	TEST_LDADD=$DOPE_LTLIBS
fi

echo "libdope depends upon: $DOPE_LDADD"
if test "x$enable_ilibdep" = "xno"; then
     echo "Inter-library dependencies tracking via libtool is disabled -> use dope-config instead"
     DOPE_LDADD=""
else
     TEST_LDADD=""
fi

DOPE_CFLAGS="$SIGC_CFLAGS $XML_CPPFLAGS"
echo DOPE_CFLAGS = $DOPE_CFLAGS

AC_SUBST(DOPE_CFLAGS)
AC_SUBST(DOPE_LDADD)
AC_SUBST(DOPE_LIBS)
AC_SUBST(DOPE_LTLIBS)
AC_SUBST(TEST_LDADD)

dnl output files
AC_CONFIG_FILES([Makefile doc/Makefile doc/Doxyfile config/Makefile config/m4/Makefile boost/Makefile boost/type_traits/Makefile scripts/Makefile scripts/dope-config dope/Makefile dope/dope.h dope/arch.h tests/Makefile])
AC_OUTPUT

