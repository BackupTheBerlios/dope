/*
  (not yet) xmlrpc conform client
*/

#include "test-common.h"


#include <signal.h>
// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>

#include <dope/dopesignal.h>
#include <dope/minihttp.h>


void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}


#define PORT 8080

int main(int argc,char *argv[])
{
  try {
    signal(SIGPIPE,sigPipeHandler);
    foo f;
    f.setSome();
    HTTPStreamBuf layer0("http://schlumpf/dope/test.php3","localhost");
    XMLOutStream<HTTPStreamBuf> l3(layer0);
    SignalOutAdapter<XMLOutStream<HTTPStreamBuf> > o(l3);
    o.emit(f);
    std::string l;
    std::istream i(&layer0);
    char c;
    while (i.get(c))
      std::cout.put(c);
    return 0;
  }catch (const std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<< std::endl;
  }catch (const char *error){
    std::cerr << error << std::endl;
  }catch(...){
    std::cerr << "Uncaught unknown exception\n";
  }
  return 1;
}


