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
   \file minihttp.h
   \brief http protocol stuff
   \author Jens Thiele
*/

#ifndef DOPE_MINIHTTP_H
#define DOPE_MINIHTTP_H

#include <iostream>
#include "uri.h"
#include "streamtraits.h"

//! mini http stream - designed for xmlrpc
/*! 
  output uses method post
  since http (at least xmlrpc) requires to specify the Content-length
  output is buffered and counted and sent with a sync
  => afterwards the stream can't send any data
*/
template <typename _Layer0>
class HTTProtocol : public std::basic_streambuf<typename _Layer0::char_type, typename _Layer0::traits_type>
{
protected:
  typedef _Layer0 Layer0;
  Layer0 &layer0;
  URI &uri;
  bool flushed;
  bool gotheader;
  unsigned contentLength;
public:
  typedef typename Layer0::char_type char_type;
  typedef typename Layer0::traits_type traits_type;
  typedef typename Layer0::int_type int_type;

  HTTProtocol(Layer0 &_layer0, URI &_uri) 
    : layer0(_layer0), uri(_uri), flushed(false), gotheader(false), contentLength(0)
  {
    std::ostream o(&layer0);
    //    std::cerr << "POST "<<uri<<" HTTP/1.0\n";
    o << "POST "<< uri.getPath().c_str() <<" HTTP/1.0\n";
    o << "User-Agent: DOPE\n";
    o << "Host: " << uri.getHost() << "\n";
    o << "Content-Type: application/x-www-form-urlencoded\n";
    o << "Content-Length: ";
  }

  int sync()
  {
    std::ostream o(&layer0);
    // Content-Length ()
    o << (pptr()-pbase()) << "\n\n";
    flushed=true;
    if (!pbase()) return 0;
    std::streamsize towrite=pptr()-pbase();
    std::streamsize written=layer0.sputn(pbase(),towrite);
    int_type ret=(towrite==written) ? 0 : traits_type::eof();
    delete [] pbase();
    setp(NULL,NULL);
    o.flush();
    return ret;
  }

  std::streamsize showmanyc()
  {
    // we do not have any buffer => return how many characters are avvailable in our underlying stream
    return layer0.in_avail();
  }

  int_type underflow()
  {
    if (!gotheader)
      readheader();
    if (!contentLength)
      return traits_type::eof();
    --contentLength;
    // was sgetc but according to a book sgetc does not modify the gptr()
    return layer0.sbumpc();
  }

  int_type uflow()
  {
    return underflow();
  }

  int_type pbackfail(int_type c = traits_type::eof())
  {
    int_type r=layer0.sputbackc(c);
    if (r==traits_type::eof())
      return r;
    ++contentLength;
    return r;
  }
  
  //! we are called if the buffer is full
  /*! allocates a new bigger buffer */
  int_type overflow(int_type i = traits_type::eof())
  {
    if (flushed)
      return traits_type::eof();
    int_type oldSize=epptr()-pbase();
    int_type add=1000;
    int_type newSize=oldSize+add;
    char_type* newBuf=new char_type[newSize];
    traits_type::copy(newBuf,pbase(),oldSize);
    if (pbase())
      delete [] pbase();
    setp(newBuf,newBuf+newSize);
    // pptr()=newBuf+oldSize;
    pbump(oldSize);
    *pptr()=traits_type::to_char_type(i);
    pbump(1);
    return i;
  }
protected:
  void readheader()
  {
    // we are only interested in finding the start of the document - which is marked by 3 empty lines
    std::istream i(&layer0);
    std::string l;
    while (getline(i,l)) {
      std::cerr << l.size() << ": ->" << l << "<-" << std::endl;
      std::string h("Content-Length: ");
      if ((l.size()>h.size())&&begins(l,h)) {
	stringToAny(l.substr(h.size()),contentLength);
	std::cerr << "Got Content-Length: "<<contentLength << std::endl;
      }
      if (l.empty()) {
	std::cerr << "Found empty line\n";
	break;
      }
      if (l=="\r") {
	std::cerr << "Found CR\n";
	break;
      }
    }
    gotheader=true;
  }
};


//! the idea is to wrap the stateless http - and hide that each request opens and closes the connection
template <typename _CharT, typename _Traits = std::char_traits<_CharT> >
class BasicHTTPStreamBuf : public std::basic_streambuf<_CharT, _Traits>
{
public:
  typedef _CharT 					char_type;
  typedef _Traits 					traits_type;
  typedef typename traits_type::int_type 		int_type;
  
  BasicHTTPStreamBuf(const URI &_uri) : uri(_uri)
  {
  }  

  int sync()
  {
    if (!isConnected())
      return traits_type::eof();
    return pptr->pubsync();
  }

  std::streamsize showmanyc()
  {
    if (!isConnected())
      return 0;
    return pptr->in_avail();
  }


  int_type underflow()
  {
    if (!isConnected())
      return traits_type::eof();
    return pptr->sbumpc();
  }

  //! it seems we have to overwrite uflow if we don't use a buffer
  int_type uflow()
  {
    return underflow();
  }

  //! put back one character
  /*!
    \todo what should I do if I am called with eof ?
  */
  int_type pbackfail(int_type c  = traits_type::eof())
  { 
    if (!isConnected())
      return traits_type::eof();
    return pptr->sputbackc(c);
  }

  int_type overflow(int_type i = traits_type::eof())
  {
    if (i==traits_type::eof()) {
      disconnect();
      return i;
    }
    char_type c(traits_type::to_char_type(i));
    connect()->sputc(c);
    return i;
  }
protected:
  typedef HTTProtocol<BasicNetStreamBuf<char_type, traits_type> > P;
  typedef DOPE_SMARTPTR<P> PPtr;
  typedef BasicNetStreamBuf<char_type,traits_type> N;
  typedef DOPE_SMARTPTR<N> NPtr;

  URI uri;
  NPtr nptr;
  PPtr pptr;
  
  PPtr &connect()
  {
    if (isConnected())
      return pptr;
    nptr=NPtr(new N(uri.getAddress()));
    pptr=PPtr(new P(*(nptr.get()),uri));
    return pptr;
  }
  void disconnect()
  {
    pptr=PPtr();
    nptr=NPtr();
  }
  bool isConnected() {
    if (pptr.get()) {
      assert(nptr.get());
      return true;
    }
    return false;
  }
};

typedef BasicHTTPStreamBuf<char> HTTPStreamBuf;


template <typename Layer0>
class URLEncodeStream
{
protected:
  Layer0 &l0;
  std::list<std::string> scope;
  bool first;
  
  template <typename X>
  URLEncodeStream &simpleHelper(X data, MemberName mname)
  {
    assert(mname);
    std::string l;
    if (!first) l="&";
    first=false;
    std::list<std::string>::iterator it(scope.begin());
    while (it!=scope.end()) {
      l+=(*it)+'.';
      ++it;
    }
    l+=(URI::urlEncode(mname)+"="+URI::urlEncode(anyToString(data)));
    l0.sputn(l.c_str(),l.size());
    return *this;
  }
  
public:
  URLEncodeStream(Layer0 &_l0) : l0(_l0), first(true)
  {}

  typedef Layer2OutStreamTraits Traits;

  void flush()
  {
    l0.pubsync();
  }

#define DOPE_SIMPLE(type) \
DOPE_INLINE URLEncodeStream &simple(type data, MemberName mname) \
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
  DOPE_INLINE URLEncodeStream &simple(DOPE_CONSTOUT T& data, MemberName mname) {
    if (mname) scope.push_back(URI::urlEncode(mname));
    ::composite(*this, data);
    if (mname) scope.pop_back();
    return *this;
  }


  // map methods not named simple

  //! extension
  template <typename T>
  URLEncodeStream &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }

  //! dynamic C string
  DOPE_INLINE URLEncodeStream &dynCString(const char* data, MemberName mname) 
  {
    return simple(data,mname);
  }
  //! fixed C string
  DOPE_INLINE URLEncodeStream &fixedCString(const char* data, size_t max, MemberName mname) 
  {
    return simple(data,mname);
  }
  //! dynamic C vector (array)
  template <class T>
  DOPE_INLINE URLEncodeStream& dynCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! C vector of fixed size
  template <class T>
  DOPE_INLINE URLEncodeStream& fixedCVector(DOPE_CONSTOUT T data[], size_t size, MemberName mname)
  {
    return printCVector(data,data+size,mname);
  }
  
  //! types conforming to the STL container concept
  template <class C>
  DOPE_INLINE URLEncodeStream& container(C &data)
  {
    return printContainer(data.begin(),data.end());
  }
protected:
  template <typename I>
  URLEncodeStream &printCVector(I begin, I end, MemberName mname)
  {
    printContainer(begin,end);
    return *this;
  }
  
  template <typename I>
  URLEncodeStream& printContainer(I begin, I end)
  {
    size_t j=0;
    for (I i=begin; i!=end;++i,++j)
      {
	simple(*i,NULL);
      }
    return *this;

  }
};



#endif
