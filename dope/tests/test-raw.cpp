#include "test-common.h"

// layer 2 to 1 adapter
#include <dope/adapter2to1.h>
// layer 1 raw stream
#include <dope/raw.h>

int main(int argc,char *argv[])
{
  typedef std::streambuf L0;
  Test<RawInStream<L0> , RawOutStream<L0> > test;
  return test.main(argc,argv);
}
