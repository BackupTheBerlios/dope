#ifndef DOPE_OBJECTFACTORY_H
#define DOPE_OBJECTFACTORY_H

#include "dope.h"
#include <sigc++/signal_system.h>

#define TYPE_NAME(x) TypeNameTrait<x>::name()

template <class X>
class ObjectFactoryTraits
{
public:
  typedef typename SigC::Signal1<void, DOPE_SMARTPTR<X> > SignalT;
  typedef typename SigC::Slot1<void, DOPE_SMARTPTR<X> > SlotT;
};

template <class Layer3, class TypeID=TypeNameType >
class GenericFactory
{
public:
  virtual TypeID getObjectTypename()=0;
  virtual void read(Layer3 &p)=0;
  virtual ~GenericFactory(){}
};

template <class X, class Layer3, class TypeID=TypeNameType >
class ObjectFactory : public GenericFactory<Layer3, TypeID>
{
public:
  TypeID getObjectTypename() 
  {
    return TYPE_NAME(X);
  }
  void read(Layer3 &layer3)
  {
    DOPE_SMARTPTR<X> xptr(new X());
    layer3.simple(*xptr,NULL);
    signal.emit(xptr);
  }
  typename ObjectFactoryTraits<X>::SignalT signal;
};

#undef TYPE_NAME


#endif
