#ifndef DOPE_SIGNAL_H
#define DOPE_SIGNAL_H

//! transmit signals via streams

// std exceptions
#include <stdexcept>

#include "dope.h"
#include "typenames.h"
#include "objectfactory.h"

#define TYPE_NAME(x) TypeNameTrait<x>::name()

//! stream wrapper giving a libsigc++ like interface
template <typename L2>
class SignalOutAdapter
{
public:
  typedef L2 Layer2;
  
  SignalOutAdapter(Layer2 &_layer2) : layer2(_layer2)
  {}
  
  template <typename X>
  void emit(DOPE_CONSTOUT X &x)
  {
    std::string tname(TYPE_NAME(X));
    layer2.simple(tname,NULL).simple(x,NULL).flush();
  }
protected:
  Layer2 &layer2;
};

//! stream wrapper producing objects from a stream emiting them as signal
template <typename L2>
class SignalInAdapter
{
public:
  typedef L2 Layer2;

  SignalInAdapter(Layer2 &_layer2) : layer2(_layer2), ignoreUnknown(false)
  {}
  
  //! process pending objects in stream and emit signals
  void read()
  {
    TypeNameType tname;
    layer2.simple(tname,NULL);
    typename Factories::iterator it(factories.find(tname));
    if (it==factories.end())
      {
	if (!ignoreUnknown)
	  {
	    std::string e("received unknown object type: \"");
	    e+=tname+"\"";
	    throw std::range_error(e);
	  }
	else
	  return;
      }
    it->second->read(layer2);
  }
  

  //! add a receiver for objects of type X
  template <typename X>
  typename ObjectFactoryTraits<X>::SignalT &addFactory(TypeNameTrait<X>)
  {
    std::string tname(TYPE_NAME(X));
    if (factories.find(tname)!=factories.end()) throw std::invalid_argument(std::string("Wanted to add factory for type \"")+tname+"\" twice");
    ObjectFactory<X,Layer2>* nr=new ObjectFactory<X,Layer2>;
    factories[tname]=FactoryPtr(static_cast<GenericFactory<Layer2> *>(nr));
    return nr->signal;
  }
  
  //! connect slot to signals of objects of type X
  /*!
    \note the new object is passed by a smartptr - per default dope uses boost::shared_ptr

    \todo throw a specific exception and document it
  */
  template <typename X>
  void connect(SigC::Slot1<void, DOPE_SMARTPTR<X> > s) //ObjectFactoryTraits<X>::SlotT s)
  {
    try {
      addFactory(TypeNameTrait<X>()).connect(s);
    } catch (std::string error) {
      DOPE_WARN(error);
      typedef ObjectFactory<X,Layer2> ObjectFactory_t;
      ObjectFactory_t* r = dynamic_cast<ObjectFactory_t *>(factories[TYPE_NAME(X)].get());
      if (!r) {
	throw std::string("Wanted to connect a slot for object of type \"")+std::string(TYPE_NAME(X))+"\" to signal of type \""+factories[TYPE_NAME(X)]->getObjectTypename()+"\"";
      }
      r->signal.connect(s);
    }
  }
  
  //! set if we should ignore unknown incoming objects
  /*!
    \note if you don't ignore unkown objects - an exception is thrown if an unkown object arrives
  */
  void setIgnoreUnknown(bool i) 
  {
    ignoreUnknown=i;
  }
  
protected:
  Layer2 &layer2;
  //! should I ignore unknown objects ?
  bool ignoreUnknown;

  typedef DOPE_SMARTPTR<GenericFactory<Layer2> > FactoryPtr;
  typedef std::map<TypeNameType, FactoryPtr> Factories;
  Factories factories;
};

#undef TYPE_NAME

#endif
