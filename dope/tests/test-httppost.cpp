/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
   \file tests/test-httppost.cpp
   \brief http post test
   \author Jens Thiele
*/
#include <iostream>
#include <list>
#include <signal.h>


#include <dope/minihttp.h>
#include "streamtraits.h"
#include "utils.h"
#include "typenames.h"

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



void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}

//! macro to prevent typing the membername twice
#define SIMPLE(d) simple(d,#d)

struct form
{
  std::string foo;
  std::string bar;

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(foo).SIMPLE(bar);
  }
};

template <typename Layer2>
inline void composite(Layer2 &layer2, form &f)
{
  f.composite(layer2);
}




int main(int argc,char *argv[])
{
  try {
    URI uri("http://schlumpf/dope/test.php3");
    HTTPStreamBuf layer0(uri);
    URLEncodeStream<HTTPStreamBuf> out(layer0);
    signal(SIGPIPE,sigPipeHandler);
    form f;
    f.foo="hey this is foo";
    f.bar="hey this is bar";
    out.simple(f,NULL);
    layer0.sync();
    char c;
    std::istream ist(&layer0);
    while (ist.get(c))
      std::cout.put(c);
    std::cout << std::endl;
    return 0;
  }catch (const std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<< std::endl;
  }catch (const char *error){
    std::cerr << error << std::endl;
  }catch(...){
    std::cerr << "Uncaught unknown exception\n";
  }
  return 1;
}
