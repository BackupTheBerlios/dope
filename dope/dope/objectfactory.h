/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file objectfactory.h
   \brief Object factories
   \author Jens Thiele
*/

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
