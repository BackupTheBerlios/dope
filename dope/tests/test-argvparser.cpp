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
   \file tests/test-argvparser.cpp
   \brief Small example how to use the ArgvParser
   \author Jens Thiele

   This example uses the ArgvParser to parse command line arguments
   (or if -F|--file is given read a xml configuration file)
   then it prints the configuration to stdout
*/

#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>

// use stl types
#include <dope/dopestl.h>
// DOPE_CLASS macro
#include <dope/typenames.h>
// anyToString
#include <dope/utils.h>

//! macro to prevent typing the membername twice
#define SIMPLE(d) simple(d,#d)

// layer 2 argument vector parser
#include <dope/argvparser.h>

// layer 2 xml dom output stream
#include <dope/xmldomoutstream.h>

struct Arguments
{
  Arguments() : port(10000), foo("Hello World")
  {}
  
  int port;
  std::string foo;

  //! this method lists all memberes that should be pickled
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(port).SIMPLE(foo);
  }
};
DOPE_CLASS(Arguments);

template <typename Layer2>
inline void composite(Layer2 &layer2, Arguments &a)
{
  a.composite(layer2);
}

int main(int argc,char *argv[])
{
  try{
    ArgvParser parser(argc,argv);
    Arguments args;
    parser.SIMPLE(args);
    // exit if parser printed the help message
    if (parser.gotHelpArg()) return 1;
    // now write configuration to stdout
    XMLDOMOutStream<std::streambuf> d(*std::cout.rdbuf());
    d.simple(args,NULL);
    return 0;
  }
  catch (const std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<<std::endl;
  }
  return 1;
}
