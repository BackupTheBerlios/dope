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
   \file debug.h
   \brief DebugOutStream
   \author Jens Thiele
*/

#ifndef DOPE_DEBUG_H
#define DOPE_DEBUG_H

#include "streamtraits.h"
#include "utils.h"
#include "typenames.h"
#include <iterator>

#define TYPE_NAME(type) TypeNameTrait<type>::name()

//! Layer 2 output stream for debugging purposes
/*!
  produces output similar to a debugger

  \todo make constructor fit into layer2 concept
*/
class DebugOutStream
{
protected:
  std::ostream &o;

public:
  DebugOutStream(std::ostream &_o) : o(_o)
  {}

  typedef Layer2OutStreamTraits Traits;

#define DOPE_SIMPLE(T) \
DOPE_INLINE DebugOutStream &simple(T data, MemberName mname) \
  { \
    if (!mname) mname=""; \
    o << column << TYPE_NAME(boost::remove_reference<T>::type) << " " << mname << " = " << data << ";\n"; \
    return *this; \
  }

  DOPE_SIMPLE(bool);
  DOPE_SIMPLE(char);
  DOPE_SIMPLE(unsigned char);
  DOPE_SIMPLE(short);
  DOPE_SIMPLE(unsigned short);
  DOPE_SIMPLE(int);
  DOPE_SIMPLE(unsigned int);
  DOPE_SIMPLE(long);
  DOPE_SIMPLE(unsigned long);
  DOPE_SIMPLE(float);
  DOPE_SIMPLE(double);
  DOPE_SIMPLE(DOPE_CONSTOUT std::string &);
  DOPE_SIMPLE(const char*);

  /*
  DOPE_SIMPLE(char* &);
  */
#undef DOPE_SIMPLE

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE DebugOutStream &simple(DOPE_CONSTOUT T& data, MemberName mname) {
    if (!mname) mname="";
    o << column << TYPE_NAME(T) << " " << mname << " = {\n";
    indent();
    ::composite(*this, data);
    unIndent();
    o << column << "}\n";
    return *this;
  }

  // map methods not named simple

  template <typename T>
  DOPE_INLINE DebugOutStream &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }

  DOPE_INLINE DebugOutStream &dynCString(const char* data, MemberName mname) 
  {
    return simple(data,mname);
  }
  DOPE_INLINE DebugOutStream &fixedCString(const char* data, size_t max, MemberName mname) 
  {
    return simple(data,mname);
  }

  template <class T>
  DOPE_INLINE DebugOutStream& dynCVector(DOPE_CONSTOUT T data[], size_t &size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE DebugOutStream& fixedCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! types conforming to the STL container concept
  template <class C>
  DOPE_INLINE DebugOutStream& container(C &data)
  {
    return printContainer(data.begin(),data.end());
  }
protected:
  template <typename I>
  DebugOutStream &printCVector(I begin, I end, MemberName mname)
  {
    typedef typename std::iterator_traits<I>::value_type value_type;
    if (!mname) mname="";
    o << column << TYPE_NAME(value_type) << " " << mname << "[] = {\n";
    indent();
    printContainer(begin,end);
    unIndent();
    o << column << "}\n";
    return *this;
  }
  
  template <typename I>
  DebugOutStream& printContainer(I begin, I end)
  {
    for (I i=begin; i!=end;++i)
      {
	simple(*i,NULL);
      }
    return *this;

  }

  std::string column;
  void indent() 
  {
    column+="  ";
  }

  void unIndent()
  {
    column=column.substr(0,column.size()-2);
  }
};

#undef TYPE_NAME

#endif
