/*
  this example is a bit stupid because i pressed it into my testing framework

  what happens ?

  i wrapped the signaloutadapter to give it a stream interface - which is quite stupid
  because the signalloutadapter is a wrapper around the stream interface to give it a 
  sigc++ like interface

  what happens is quite simple - the object is written raw to a streambuffer

  the read side is more interesting
  the signalinadapter creates objects from a stream by emitting it as signal
  now the fakedinstream creates such an adapter on read request and connects the signal
  to its handleFoo slot which simply stores the smart ptr and returns

  back in the read request (method named simple) the object is copied into x
  (only if in the meantime an object of type foo arrived)
*/

#include "test-common.h"
#include <dope/dopesignal.h>

// layer 2 to 1 adapter
#include <dope/adapter2to1.h>
// layer 1 raw stream
#include <dope/raw.h>

template <typename L0>
class FakedInStream : public SigC::Object
{
protected:
  L0 &l0;
  DOPE_SMARTPTR<foo> fptr;
public:
  FakedInStream(L0 &_l0) : l0(_l0)
  {
  }
  template <typename X>
  FakedInStream &simple(X &x,MemberName)
  {
    {
      RawInStream<std::streambuf> l2(l0);
      SignalInAdapter<RawInStream<std::streambuf> > sigFactory(l2);
      sigFactory.connect(SigC::slot(*this,&FakedInStream<L0>::handleFoo));
      sigFactory.read();
      if (!fptr.get())
	exit(1);
    }
    {
      // x=*fptr.get(); hmm operator= is private => we use dope to do this
      // you can use this as example of how to implement an operator= with dope ;-)
      // but for the testing framework this means if test-raw fails test-signal will fail too
      // but this would be the case anyway because we used the rawstream anyway
      std::stringbuf buf;
      RawOutStream<std::streambuf> l2o(buf);
      l2o.simple(*fptr.get(),NULL);
      RawInStream<std::streambuf> l2i(buf);
      l2i.simple(x,NULL);
    }
    return *this;
  }
  void handleFoo(DOPE_SMARTPTR<foo> fooPtr)
  {
    assert(fooPtr.get());
    fptr=fooPtr;
  }
};


template <typename L0>
class FakedOutStream 
{
protected:
  L0 &l0;
public:
  FakedOutStream(L0 &_l0) : l0(_l0)
  {
  }

  template <typename X>
  FakedOutStream &simple(X &x, MemberName)
  {
    RawOutStream<L0> l2(l0);
    SignalOutAdapter<RawOutStream<L0> > o(l2);
    o.emit(x);
    return *this;
  }
};

int main(int argc,char *argv[])
{
  typedef std::streambuf L0;
  Test<FakedInStream<L0>, FakedOutStream<L0> > test;
  return test.main(argc,argv);
}
