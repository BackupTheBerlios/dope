#include "test-common.h"

// layer 2 xml dom output stream
#include <dope/xmldomoutstream.h>

// layer 2 xml sax input - todo write xmldominput and then replace it
#include <dope/xmlsaxinstream.h>

int main(int argc,char *argv[])
{
  typedef std::streambuf L0;
  Test<XMLSAXInStream<L0> , XMLDOMOutStream<L0> > test;
  return test.main(argc,argv);
}
