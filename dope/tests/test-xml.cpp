#include "test-common.h"

// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>

int main(int argc,char *argv[])
{
  typedef std::streambuf L0;
  Test<XMLSAXInStream<L0> , XMLOutStream<L0> > test;
  return test.main(argc,argv);
}
