#include "test-common.h"

// layer 2 debug stream
#include <dope/debug.h>

int main(int argc,char *argv[])
{
  try{
    {
      foo f;
      f.setSome();
      DebugOutStream d(std::cerr);
      d.SIMPLE(f);
    }
    return 0;
  }
  catch (const std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<<std::endl;
  }
  return 1;
}
