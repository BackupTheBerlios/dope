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
   \file xmldomoutstream.h
   \brief xml layer2 output stream using the dom
   \author Jens Thiele
*/

#ifndef DOPE_XMLDOMINSTREAM_H
#define DOPE_XMLDOMINSTREAM_H

#include <stack>
#include "dope.h"
#include "dom.h"
// for xmlString
#include "xml.h"
#include "streamtraits.h"
#include <boost/type_traits.hpp> // boost::remove_const
#include <boost/smart_ptr.hpp>

#define TYPE_NAME(TYPE) TypeNameTrait<typename boost::remove_const<TYPE>::type >::name()

//! Layer 2 output stream writing xml via the DOM tree
template <typename Layer0>
class XMLDOMOutStream
{
protected:
  DOPE_SMARTPTR<DOM> domPtr;
  Layer0 &l0;
  std::ostream o;

  typedef std::string xmlString;
public:
  XMLDOMOutStream(Layer0 &_l0) : l0(_l0), o(&l0), encodeAttributes(false), format(false)
  {
  }
  ~XMLDOMOutStream()
  {
    flush();
  }
  
  void flush()
  {
    o << std::flush;
    l0.pubsync();
  }

  typedef Layer2OutStreamTraits Traits;

  template <typename X>
  XMLDOMOutStream &simpleHelper(X data, MemberName mname)
  {
    // simple values we try to encode as xml attributes
    // of course we need an element => if we don't have an element we can't encode as attribute
    if ((encodeStack.empty())||(!mname))
      {
	if (encodeAttributes)
	  return *this;
	typedef typename boost::remove_reference<X>::type T;
	xmlString tname(XML::xmlName((TYPE_NAME(T)).c_str()));
	xmlEmptyElement(tname,mname,XML::xmlName("value"),anyToString(data).c_str());
      }
    else
      {
	if (!encodeAttributes)
	  return *this;
	xmlAttribute(XML::xmlName(mname),anyToString(data).c_str());
      }
    return *this;
  }

#define DOPE_SIMPLE(type) \
DOPE_INLINE XMLDOMOutStream &simple(type data, MemberName mname) \
  { \
    return simpleHelper(data,mname); \
  }

  DOPE_SIMPLE(bool);
  DOPE_SIMPLE(char);
  DOPE_SIMPLE(signed char);
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

#undef DOPE_SIMPLE

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE XMLDOMOutStream &simple(DOPE_CONSTOUT T& data, MemberName mname) {
    if (encodeAttributes)
      return *this;
    xmlString tname(XML::xmlName(TYPE_NAME(T).c_str()));

    xmlStartElement(tname,mname);

    ::composite(*this, data);

    encodeAttributes=false;
    ::composite(*this, data);    

    xmlEndElement(tname);
    
    return *this;
  }

  // map methods not named simple
  template <typename T>
  XMLDOMOutStream &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
    
  }


  DOPE_INLINE XMLDOMOutStream &dynCString(const char* data, MemberName mname) 
  {
    return simple(data,mname);
  }
  DOPE_INLINE XMLDOMOutStream &fixedCString(const char* data, size_t max, MemberName mname) 
  {
    return simple(data,mname);
  }

  template <class T>
  DOPE_INLINE XMLDOMOutStream& dynCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE XMLDOMOutStream& fixedCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! types conforming to the STL container concept
  template <class C>
  DOPE_INLINE XMLDOMOutStream& container(C &data)
  {
    return printContainer(data.begin(),data.end());
  }
protected:
  /*
  template <typename C>
  DOPE_INLINE XMLDOMOutStream& containerHelper(C &data, OutStreamTag)
  {
    return printContainer(data.begin(),data.end());
    }*/

  template <typename I>
  XMLDOMOutStream &printCVector(I begin, I end, MemberName mname)
  {
    if (encodeAttributes)
      return *this;
    typedef typename std::iterator_traits<I>::value_type value_type;
    std::string s(TYPE_NAME(value_type[]));
    xmlString tname(XML::xmlName(s.c_str()));

    xmlStartElement(tname,mname);
    // we know that no attributes will follow
    
    // this is a bit stupid because we know at compile time 
    // that everything is encoded as element
    printContainer(begin,end);
    encodeAttributes=false;
    printContainer(begin,end);

    xmlEndElement(tname);
    
    return *this;
  }
  
  template <typename I>
  XMLDOMOutStream& printContainer(I begin, I end)
  {
    size_t j=0;
    for (I i=begin; i!=end;++i,++j)
      {
	simple(*i,NULL);
      }
    return *this;

  }
  

  //! start new xml element
  /*! side effects:
    pushs encodeAttributes on stack
    sets encodeAttributes to true
  */
  void xmlStartElement(const xmlString &tname, const char* mname, const char* nextLine="\n", bool indentit=true)
  {
    encodeStack.push(encodeAttributes);
    encodeAttributes=true;
    if (!domPtr.get())
      domPtr=DOPE_SMARTPTR<DOM>(new DOM(NULL,NULL));
    domPtr->NewChild(tname.c_str(),mname);
  }
  
  //! end xml element
  /*! side efffects:
    pop encodeAtrributes from stack
  */
  void xmlEndElement(const xmlString &tname, bool indentit=true)
  {
    assert(domPtr.get());
    domPtr->nodeUp();
    assert(!encodeStack.empty());
    encodeAttributes=encodeStack.top();
    encodeStack.pop();
    if (encodeStack.empty())
      {
	// this was the last element
	o << *(domPtr.get());
	// create new dom
	domPtr=DOPE_SMARTPTR<DOM>(new DOM(NULL,NULL));
      }
  }

  //! element with no characters and no sub elements - only attributes
  void xmlEmptyElement(const xmlString &tname, const char* mname, const xmlString &key, const char* value)
  {
    xmlStartElement(tname,mname," ",false);
    xmlAttribute(key,value);
    assert(domPtr.get());
    domPtr->nodeUp();
    assert(!encodeStack.empty());
    encodeAttributes=encodeStack.top();
    encodeStack.pop();
  }
  

  void xmlAttribute(const xmlString &key, const char * value, const char* nextLine=NULL,bool indentit=true)
  {
    assert(domPtr.get());
    domPtr->setProp(key.c_str(),value);
  }
  
  
  //! only encode attributes
  bool encodeAttributes;

  //! add whitespaces to make xml readable
  bool format;
  
  //! stack of encode mode
  std::stack<bool> encodeStack;
  
};

#undef TYPE_NAME

#endif
