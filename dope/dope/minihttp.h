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
#include <dope/uri.h>

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
  bool flushed;
  
public:
  typedef typename Layer0::char_type char_type;
  typedef typename Layer0::traits_type traits_type;
  typedef typename Layer0::int_type int_type;

  HTTProtocol(Layer0 &_layer0, const char *uri, const char* myhostname) : layer0(_layer0), flushed(false)
  {
    _M_mode=std::ios_base::in|std::ios_base::out;
    _M_buf_unified = false;
    
    std::ostream o(&layer0);
    o << "POST "<<uri<<" HTTP/1.0\n";
    o << "User-Agent: DOPE\n";
    o << "Content-Type: text/xml\n";
    o << "Content-Length: ";
    /*
    // we are only interested in finding the start of the document - which is marked by 3 empty lines
    std::istream i(&layer0);
    std::string l;
    unsigned empty=0;
    do {
      getline(i,l);
      if (l=="")
	++empty;
      else 
	empty=0;
    }while(empty<3);
    */
  }

  int_type underflow()
  {
    return layer0.sgetc();
  }

  int_type uflow()
  {
    int_type c = underflow();
    return c;
  }
  
  //! we are called if the buffer is full
  /*! allocates a new bigger buffer */
  int_type overflow(int_type i = traits_type::eof())
  {
    if (flushed)
      return traits_type::eof();
    int_type oldSize=_M_out_end-_M_out_beg;
    int_type add=1000;
    int_type newSize=oldSize+add;
    char_type* newBuf=new char_type[newSize];
    traits_type::copy(newBuf,_M_out_beg,oldSize);
    if (_M_out_beg)
      delete [] _M_out_beg;
    _M_out_beg=newBuf;
    _M_out_cur=newBuf+oldSize;
    _M_out_end=newBuf+newSize;
    *(_M_out_cur++)=traits_type::to_char_type(i);
    return i;
  }

  int sync()
  {
    assert(_M_out_beg); // todo
    std::ostream o(&layer0);
    o << (_M_out_cur-_M_out_beg) << "\n\n";
    flushed=true;
    // todo what should sync return ? - how to report errors ?
    int ret=layer0.sputn(_M_out_beg,_M_out_cur-_M_out_beg);
    delete [] _M_out_beg;
    _M_out_beg=_M_out_cur=_M_out_end=NULL;
    o.flush();
    return 0; // todo
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
  
  BasicHTTPStreamBuf(const URI &_uri, const std::string &_myhostname) : uri(_uri), myhostname(_myhostname)
  {
  }  

  int_type sputc(char_type c)
  {
    assert(0);
  }

  std::streamsize 
  sputn(const char_type* __s, std::streamsize __n)
  { 
    assert(0);
  }

  int_type overflow(int_type i = traits_type::eof())
  {
    // todo - eof should close ?
    char_type c(traits_type::to_char_type(i));
    connect()->sputc(c);
    return i;
  }

  int_type underflow()
  {
    int_type i=connect()->sgetc();
    if (i==traits_type::eof())
      disconnect();
    return i;
  }

  int_type uflow()
  {
    int_type c = underflow();
    return c;
  }

  int sync()
  {
    int r=connect()->pubsync();
    return r;
  }
  
protected:
  typedef HTTProtocol<BasicNetStreamBuf<char_type, traits_type> > P;
  typedef DOPE_SMARTPTR<P> PPtr;
  typedef BasicNetStreamBuf<char_type,traits_type> N;
  typedef DOPE_SMARTPTR<N> NPtr;
  
  PPtr &connect()
  {
    if (pptr.get())
      {
	assert(nptr.get());
	return pptr;
      }
    nptr=NPtr(new N(uri.getAddress()));
    pptr=PPtr(new P(*(nptr.get()),uri.getPath().c_str(),myhostname.c_str()));
    return pptr;
  }
  void disconnect()
  {
    pptr=PPtr(NULL);
    nptr=NPtr(NULL);
  }
  
  URI uri;
  std::string myhostname;
  NPtr nptr;
  PPtr pptr;
};

typedef BasicHTTPStreamBuf<char> HTTPStreamBuf;

#endif
