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
#include <dope/streamtraits.h>
#include <dope/utils.h>
#include <dope/typenames.h>


void sigPipeHandler(int x){
  std::cerr << "WARNING: Received sig pipe signal - I ignore it"<<std::endl;
}

//! macro to prevent typing the membername twice
#define SIMPLE(d) simple(d,#d)

struct form
{
  std::string foo;
  std::string bar;
  unsigned short int i;

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(foo).SIMPLE(bar).SIMPLE(i);
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
    f.i=40700;
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
