#include "test-common.h"

// layer 2 network byte order (raw) stream
#include <dope/nbostream.h>

int main(int argc,char *argv[])
{
  typedef std::streambuf L0;
  Test<NBOInStream<L0> , NBOOutStream<L0> > test;
  return test.main(argc,argv);
}
