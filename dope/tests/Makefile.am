INCLUDES = -I$(top_srcdir) @DOPE_CFLAGS@
DOPELIB = $(top_builddir)/dope/libdope.la
LDADD = $(DOPELIB) @TEST_LDADD@

# (echo -n "noinst_PROGRAMS= " && for A in *.cpp *.c; do echo -e -n '\\\n' "     " ${A%%.c*} ; done&& echo && echo && for A in *.cpp; do echo ${A%%.cpp}_SOURCES = $A ; done) >> Makefile.am

automatictests = \
	test-dom \
	test-raw \
	test-signal \
	test-xml \
	test-streambuf

manualtests = \
	test-debug \
	test-rpc \
	test-saxwrapper \
	test-xmlrpc \
	test-argvparser \
	test-httppost

check_PROGRAMS= \
	$(automatictests) \
	$(manualtests)

common = test-common.h

test_debug_SOURCES = test-debug.cpp $(common)
test_dom_SOURCES = test-dom.cpp $(common)
test_raw_SOURCES = test-raw.cpp $(common)
test_rpc_SOURCES = test-rpc.cpp $(common)
test_saxwrapper_SOURCES = test-saxwrapper.cpp $(common)
test_signal_SOURCES = test-signal.cpp $(common)
test_xml_SOURCES = test-xml.cpp $(common)
test_xmlrpc_SOURCES = test-xmlrpc.cpp $(common)
test_argvparser_SOURCES = test-argvparser.cpp $(common)
test_httppost_SOURCES = test-httppost.cpp
test_streambuf_SOURCES = test-streambuf.cpp


# todo make check on cross compiles to mingw has problems
# the check programs are build fine with .exe
# then the tests should be run (which will not work anyway) but because they depend
# on the programs (without .exe) they are tried to be build a second time

TESTS = $(automatictests)


