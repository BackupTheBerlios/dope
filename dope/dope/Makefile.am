INCLUDES = @DOPE_CFLAGS@ -I$(top_srcdir)

noinstheaders = 

libdopeincludedir = $(includedir)/@PACKAGE@-@VERSION@/@PACKAGE@

libdopeinclude_HEADERS = \
	adapter2to1.h layer1.h \
	uri.h uri.cpp \
	layer2.h utils.h \
	manip.h xml.h \
	debug.h minihttp.h xmldominstream.h \
	minixml.h xmldomoutstream.h \
	dom.h namemanip.h \
	dope.h \
	objectfactory.h \
	dopeexcept.h raw.h xmlsaxinstream.h \
	dopestl.h saxwrapper.h xslstream.h \
	dopesignal.h \
	typenames.h streamtraits.h \
	argvparser.h \
	timestamp.h \
	network.h \
	socket.h \
	bswap.h	\
	nbostream.h \
	smartptr.h

libdopearchincludedir = $(libdir)/@PACKAGE@-@VERSION@/@PACKAGE@
libdopearchinclude_HEADERS = \
	arch.h

lib_LTLIBRARIES = libdope.la
libdope_la_SOURCES = $(libdopeinclude_HEADERS) $(noinstheaders) \
	dom.cpp network.cpp socket.cpp dope.cpp timestamp.cpp

libdope_la_LDFLAGS = -version-info @DOPE_LTVERSION@ @DOPE_LDADD@

