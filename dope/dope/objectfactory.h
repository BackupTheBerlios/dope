#ifndef DOPE_OBJECTFACTORY_H
#define DOPE_OBJECTFACTORY_H

#include "dope.h"
#include <sigc++/signal_system.h>

#define TYPE_NAME(x) TypeNameTrait<x>::name()

//! traits of an objectfactory for type X
template <class X>
class ObjectFactoryTraits
{
public:
  typedef typename SigC::Signal1<void, DOPE_SMARTPTR<X> > SignalT;
  typedef typename SigC::Slot1<void, DOPE_SMARTPTR<X> > SlotT;
};

//! generic object factory - the base class for the specialized factories
template <class Layer2, class TypeID=TypeNameType >
class GenericFactory
{
public:
  //! return the TypeID of the class for which this factory produces objects
  virtual TypeID getObjectTypename()=0;
  //! read exactly one object and emit a signal
  virtual void read(Layer2 &p)=0;
  virtual ~GenericFactory(){}
};

//! specialized object factory for one class type
template <class X, class Layer2, class TypeID=TypeNameType >
class ObjectFactory : public GenericFactory<Layer2, TypeID>
{
public:
  TypeID getObjectTypename() 
  {
    return TYPE_NAME(X);
  }
  void read(Layer2 &layer2)
  {
    DOPE_SMARTPTR<X> xptr(new X());
    layer2.simple(*xptr,NULL);
    signal.emit(xptr);
  }
  typename ObjectFactoryTraits<X>::SignalT signal;
};

#undef TYPE_NAME


#endif
