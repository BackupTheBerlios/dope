/*
  this sample program demonstrates (not xmlrpc conform) xml rpc calls

  just take the xmlrpc spec and remove all the http stuff
  (including the content size)
  allow mulltiple xml documents in one stream => requires a bit more logic on the parser side
  but if you don't like it just close the stream and open a new one
  or look at test-xmlrpc for xmlrpc a conform sample
  
  why ?  why use http ?
  why require to count the bytes before sending them ?
*/

#include "test-common.h"
#include <dope/dopesignal.h>
#include <dope/network.h>
#include <signal.h>

void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}

#define PORT 10000

/*
// layer 2 to 1 adapter
#include <dope/adapter2to1.h>
// layer 1 raw stream
#include <dope/raw.h>

typedef RawOutStream<std::streambuf> OutProto;
typedef RawInStream<std::streambuf> InProto;
*/

// layer 2 mini xml output stream
#include <dope/minixml.h>
// layer 2 xml sax input
#include <dope/xmlsaxinstream.h>

typedef XMLOutStream<std::streambuf> OutProto;
typedef XMLSAXInStream<std::streambuf> InProto;

int msgs=0;

void writeFoo(foo &f, const char *fname)
{
  // write foo to file

  // use ofstream to create a filebuf for simplicity
  std::ofstream fo(fname);
  // we want to write raw data to the file
  OutProto l2(*fo.rdbuf());
  // write it
  l2.simple(f,"f");
}


template <typename P = InProto >
class StreamFactory
{
public:
  StreamFactory(std::streambuf &_l0) : p(_l0) , sigFactory(p)
  {
  }

  void read()
  {
    sigFactory.read();
  }
  
  template <typename X>
  void connect(const SigC::Slot1<void, DOPE_SMARTPTR<X> > &s)
  {
    sigFactory.connect(s);
  }
  
protected:
  P p;
  SignalInAdapter<P> sigFactory;
};

class Connection : public SigC::Object
{
public:
  Connection(DOPE_SMARTPTR<NetStreamBuf> _streamPtr) : streamPtr(_streamPtr), factory(*(streamPtr.get()))
  {
    factory.connect(SigC::slot(*this,&Connection::handleFoo));
  }
  
  void read()
  {
    factory.read();
  }
  
  void handleFoo(DOPE_SMARTPTR<foo> fooPtr)
  {
    assert(fooPtr.get());
    ++msgs;
    std::cout << "saved foo from network to foo.net\n";
    writeFoo(*fooPtr.get(),"foo.net");
  }

protected:
  DOPE_SMARTPTR<NetStreamBuf> streamPtr;
  StreamFactory<> factory;
};

class Server : public SigC::Object
{
public:
  Server() : streamServer(PORT)
  {
  }
  
  void handleNewConnection(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf> streamPtr)
  {
    std::cerr << "New connection ("<<id<<"\n";
    assert(streamPtr.get());
    connections[id]=DOPE_SMARTPTR<Connection>(new Connection(streamPtr));
  }

  void handleDataAvailable(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    assert(connections.find(id)!=connections.end());
    connections[id]->read();
  }

  void handleConnectionClosed(NetStreamBufServer::ID id, DOPE_SMARTPTR<NetStreamBuf>)
  {
    Connections::iterator it(connections.find(id));
    assert(it!=connections.end());
    std::cout << "Connection "<<id<<" closed\n";
    connections.erase(it);
  }
  
  int main(int argc, char** argv)
  {
    streamServer.init();
    streamServer.newConnection.connect(SigC::slot(*this,&Server::handleNewConnection));
    streamServer.dataAvailable.connect(SigC::slot(*this,&Server::handleDataAvailable));
    streamServer.connectionClosed.connect(SigC::slot(*this,&Server::handleConnectionClosed));
    while (msgs<5)
      streamServer.select();
    connections.clear();
    return 0;
  }

protected:
  NetStreamBufServer streamServer;
  typedef std::map<NetStreamBufServer::ID, DOPE_SMARTPTR<Connection> > Connections;
  Connections connections;
};


int main(int argc,char *argv[])
{
  try {
    signal(SIGPIPE,sigPipeHandler);

    NetStreamBufServer server(10000);
    try{
      Server server;
      server.main(argc,argv);
      return 0;
    }catch(const std::exception &error){
      std::cerr << "Uncaught std::exception: "<<error.what()<< std::endl;
    }catch (const char *error){
      std::cerr << error << std::endl;
    }
    std::cerr << "can't create server => create client"<< std::endl;
    InternetAddress adr(HostAddress("quark"),10000);
    NetStreamBuf layer0(adr);
    OutProto l2(layer0);
    SignalOutAdapter<OutProto> s(l2);
    // emit signal to stream
    foo f;
    f.setSome();
    s.emit(f);
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
