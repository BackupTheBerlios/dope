CXX=g++-3.0
DEBUG=-g
OPTIM=-O0
CXXFLAGS = $(DEBUG) $(OPTIM) -ftemplate-depth-40 -I. -Wall -ansi -pedantic `xml2-config --cflags` `sigc-config --cflags`
LFLAGS = $(DEBUG) $(OOPTIM) `xml2-config --libs` `sigc-config --libs`

PROGRAMS=test-xml test-raw test-signal test-rpc test-xmlrpc test-debug test-dom

all: $(PROGRAMS)

test-xml: test-xml.o
	$(CXX) $< $(LFLAGS) -o $@

test-xml.o: test-xml.cpp test-common.h
	$(CXX) -c test-xml.cpp $(CXXFLAGS)

test-dom: test-dom.o dope/dom.o
	$(CXX) $< dope/dom.o $(LFLAGS) -o $@

test-dom.o: test-dom.cpp test-common.h dope/xmldomoutstream.h
	$(CXX) -c test-dom.cpp $(CXXFLAGS)

test-raw: test-raw.o
	$(CXX) $< $(LFLAGS) -o $@

test-raw.o: test-raw.cpp test-common.h
	$(CXX) -c test-raw.cpp $(CXXFLAGS)

test-signal: test-signal.o
	$(CXX) $< $(LFLAGS) -o $@

test-signal.o: test-signal.cpp test-common.h
	$(CXX) -c test-signal.cpp $(CXXFLAGS)

dope/network.o: dope/network.cpp dope/network.h
	$(CXX) -c dope/network.cpp $(CXXFLAGS) -o $@

test-rpc: test-rpc.o dope/network.o
	$(CXX) $< dope/network.o $(LFLAGS) -o $@

test-rpc.o: test-rpc.cpp test-common.h
	$(CXX) -c test-rpc.cpp $(CXXFLAGS)

test-xmlrpc: test-xmlrpc.o dope/network.o
	$(CXX) $< dope/network.o $(LFLAGS) -o $@

test-xmlrpc.o: test-xmlrpc.cpp test-common.h dope/minihttp.h
	$(CXX) -c test-xmlrpc.cpp $(CXXFLAGS)

clean:
	rm *.o $(PROGRAMS) dope/network.o test-dom dope/dom.o

test: all
	@echo test xml ------------ && time -p ./test-xml && cmp foo.xml foo2.xml && echo Test passed || echo Test failed
	@echo test raw ------------ && time -p ./test-raw && cmp foo.raw foo2.raw && echo Test passed || echo Test failed
	@echo test signal ------------ && time -p ./test-signal && cmp foo.sig foo2.sig && echo Test passed || echo Test failed

	@echo test dom ------------ && time -p ./test-dom && cmp foo.xml foo2.xml && echo Test passed || echo Test failed


