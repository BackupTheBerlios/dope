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
   \file minixml.h
   \brief mini xml output stream
   \author Jens Thiele
*/

#ifndef DOPE_MINIXML_H
#define DOPE_MINIXML_H

#include <iterator>
#include <stack>

#include "streamtraits.h"
#include "utils.h"
#include "typenames.h"
#include "xml.h"

#define TYPE_NAME(TYPE) TypeNameTrait<typename boost::remove_const<TYPE>::type >::name()

//! Layer 2 "xml" output stream (mini "xml")
/*!
  we do not build a dom we write similar to how a sax parser reads
  \note this is by no means a fullfeatured xml output but it is a good choice if you
  have can't afford the memory to build a DOM first. (s.a. XMLDOMOutStream)
*/
template <typename Layer0>
class XMLOutStream
{
protected:
  Layer0 &l0;
  std::ostream o;
  
  typedef std::string xmlString;

  template <typename X>
  XMLOutStream &simpleHelper(X data, MemberName mname)
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
  
public:
  XMLOutStream(Layer0 &_l0) : l0(_l0), o(&l0), encodeAttributes(false), format(false)
  {
  }

  void flush()
  {
    l0.pubsync();
  }

  typedef Layer2OutStreamTraits Traits;


#define DOPE_SIMPLE(type) \
DOPE_INLINE XMLOutStream &simple(type data, MemberName mname) \
  { \
    return simpleHelper(data,mname); \
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

#undef DOPE_SIMPLE

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE XMLOutStream &simple(DOPE_CONSTOUT T& data, MemberName mname) {
    if (encodeAttributes)
      return *this;
    xmlString tname(XML::xmlName(TYPE_NAME(T).c_str()));

    xmlStartElement(tname,mname);

    ::composite(*this, data);

    unIndent();
    o << column << ">";
    if (format) o << "\n";
    indent();
    encodeAttributes=false;
    ::composite(*this, data);    

    xmlEndElement(tname);
    return *this;
  }


  // map methods not named simple

  //! extension
  template <typename T>
  XMLOutStream &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }

  //! dynamic C string
  DOPE_INLINE XMLOutStream &dynCString(const char* data, MemberName mname) 
  {
    return simple(data,mname);
  }
  //! fixed C string
  DOPE_INLINE XMLOutStream &fixedCString(const char* data, size_t max, MemberName mname) 
  {
    return simple(data,mname);
  }
  //! dynamic C vector (array)
  template <class T>
  DOPE_INLINE XMLOutStream& dynCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! C vector of fixed size
  template <class T>
  DOPE_INLINE XMLOutStream& fixedCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! types conforming to the STL container concept
  template <class C>
  DOPE_INLINE XMLOutStream& container(C &data)
  {
    return printContainer(data.begin(),data.end());
  }
protected:
  template <typename I>
  XMLOutStream &printCVector(I begin, I end, MemberName mname)
  {
    if (encodeAttributes)
      return *this;
    typedef typename std::iterator_traits<I>::value_type value_type;
    std::string s(TYPE_NAME(value_type[]));
    xmlString tname(XML::xmlName(s.c_str()));

    xmlStartElement(tname,mname);
    // we know that no attributes will follow
    unIndent();
    o << column << ">";
    if (format) o << "\n";

    indent();
    
    // this is a bit stupid because we know at compile time 
    // that everything is encoded as element
    printContainer(begin,end);
    encodeAttributes=false;
    printContainer(begin,end);

    xmlEndElement(tname);
    
    return *this;
  }
  
  template <typename I>
  XMLOutStream& printContainer(I begin, I end)
  {
    size_t j=0;
    for (I i=begin; i!=end;++i,++j)
      {
	simple(*i,NULL);
      }
    return *this;

  }
  

  std::string column;
  void indent() 
  {
    if (!format)
      return;
    column+="  ";
  }

  void unIndent()
  {
    if (!format)
      return;
    column=column.substr(0,column.size()-2);
  }


  //! start new xml element
  /*! side effects:
    pushs encodeAttributes on stack
    sets encodeAttributes to true
  */
  void xmlStartElement(const xmlString &tname, const char* mname, const char* nextLine="\n", bool indentit=true)
  {
    if (!format) {
      nextLine=" ";
      indentit=false;
    }
    
    encodeStack.push(encodeAttributes);
    encodeAttributes=true;
    o << column << '<' <<  tname << nextLine;
    if (indentit)
      indent();
    if (mname&&(!std::string(mname).empty())) {
      o << column;
      o << "this.name=" << XML::xmlAttValue(mname) << nextLine;
    }
  }
  
  //! end xml element
  /*! side efffects:
    pop encodeAtrributes from stack
  */
  void xmlEndElement(const xmlString &tname, bool indentit=true)
  {
    if (!format) {
      indentit=false;
    }

    if (indentit) {
      unIndent();
      o << column;
    }
    o << "</" << tname << ">\n";
    encodeAttributes=encodeStack.top();
    encodeStack.pop();
  }

  //! element with no characters and no sub elements - only attributes
  void xmlEmptyElement(const xmlString &tname, const char* mname, const xmlString &key, const char* value)
  {
    xmlStartElement(tname,mname," ",false);
    xmlAttribute(key,value,format ? "/>\n" : "/>",false);
    encodeAttributes=encodeStack.top();
    encodeStack.pop();
  }
  

  void xmlAttribute(const xmlString &key, const char * value, const char* nextLine=NULL,bool indentit=true)
  {
    if (!nextLine)
      nextLine = format ? "\n" : " ";
    if (!format) {
      indentit=false;
    }

    if (indentit) o << column;
    o << key << "=" << XML::xmlAttValue(value) << nextLine;
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
