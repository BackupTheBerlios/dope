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
   \file dope/argvparser.h
   \brief command line argument parser
   \author Jens Thiele
*/

#ifndef ARGVPARSER_H
#define ARGVPARSER_H

#include "dope.h"
#include "streamtraits.h"
#include "utils.h"
#include "xmlsaxinstream.h"
#include <iomanip>
#include <fstream>

#define TYPE_NAME(type) TypeNameTrait<type>::name()


//! write out description of corresponding ArgvParser (see below)
template <typename L0=std::streambuf>
class ArgvParserDescription
{
protected:
  L0 &l0;
  std::ostream o;
public:
  typedef Layer2OutStreamTraits Traits;

  ArgvParserDescription(L0 &_l0) : l0(_l0), o(&l0)
  {
    o << "-h       --help          print help and exit\n"
      << "-FSTRING --file=STRING   read configuration from file\n";
  }
  ~ArgvParserDescription(){}

#define DOPE_SIMPLE(T) \
DOPE_INLINE ArgvParserDescription &simple(T data, MemberName mname) \
  { \
    DOPE_CHECK(mname); \
    std::string field(std::string("--")+mname+"="+upcase(removeNamespace(TYPE_NAME(boost::remove_reference<T>::type)))); \
    o << std::left << std::setw(25) << field << "(default="<< data << ")\n"; \
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
#undef DOPE_SIMPLE

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE ArgvParserDescription &simple(DOPE_CONSTOUT T& data, MemberName mname) {
    ::composite(*this, data);
    return *this;
  }

  // map methods not named simple

  template <typename T>
  DOPE_INLINE ArgvParserDescription &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }
protected:
  static std::string upcase(const std::string &s)
  {
    std::string r;
    for (std::string::size_type i=0;i<s.size();++i)
      r+=std::toupper(s[i],std::cout.getloc());
    return r;
  }
  static std::string removeNamespace(const std::string &s)
  {
    std::string::size_type pos=s.find_last_of("::");
    if (pos!=std::string::npos)
      {
	// todo: hmm why pos+1 is it because of find_last_of ? - search for pos+
	DOPE_CHECK(pos+1<s.size());
	return s.substr(pos+1);
      }
    return s;
  }
};

//! incomplete layer 2 stream to parse command line arguments
/*!
  \note this stream can't read all classes - because of the characteristics of command lines and because
  it tries to be usable and produce short paramters

  \todo 
  - complete at least the most important features
  - check for unparsed/wrong options
  - make comment stream extension which is a good demonstration for extensions
  - catch StringConversion exceptions and print help message on error
  - perhaps make composites as --composite.foo=10 ?
*/
template <typename L0=std::streambuf, typename L2=XMLSAXInStream<std::streambuf> >
class ArgvParserT
{
protected:
  L0 &l0;
  
  void init()
  {
    m_gotHelpArg=false;
    for (int a=1;a<m_argc;++a) {
      std::string carg(m_argv[a]);
      if ((carg=="-h")||(carg=="--help"))
	m_gotHelpArg=true;
      else if (begins(carg,"-F")) {
	if (carg.size()<3) m_gotHelpArg=true;
	else m_configFile=carg.substr(2);
      }else if (begins(carg,"--file=")) {
	if (carg.size()<8) m_gotHelpArg=true;
	else m_configFile=carg.substr(7);
      }
    }
  }
  
public:
  ArgvParserT(int argc, char** argv, L0 &_l0=*std::cerr.rdbuf()) 
    : l0(_l0), m_argc(argc), m_argv(argv),m_depth(0) 
  {
    init();
  }
  ~ArgvParserT(){}
  typedef Layer2InStreamTraits Traits;

#define DOPE_SIMPLE(T) \
DOPE_INLINE ArgvParserT &simple(T &data, MemberName mname) \
  { \
    DOPE_CHECK(mname); \
    std::string v; \
    if (getArg(mname,v,#T)) \
      stringToAny(v,data); \
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
  DOPE_SIMPLE(std::string);
#undef DOPE_SIMPLE 

  template <typename T>
  ArgvParserT &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE ArgvParserT &simple(T& data, MemberName mname) {
    if ((!m_depth)&&gotHelpArg())
      {
	std::ostream o(&l0);
	DOPE_CHECK(m_argv[0]);
	o << "Usage: "<<m_argv[0]<<std::endl;
	ArgvParserDescription<L0> d(l0);
	d.simple(data,mname);
      }
    else if (!m_configFile.empty())
      {
	// todo use a filebuf not a ifstream
	std::ifstream file(m_configFile.c_str());
	if (!file.good()) throw FileNotFound(m_configFile);
	DOPE_CHECK(file.good());
	L2 in(*file.rdbuf());
	in.simple(data,mname);
      }
    else
      {
	++m_depth;
	::composite(*this,data);
	--m_depth;
      }
    return *this;
  }

  bool gotHelpArg() const
  {
    return m_gotHelpArg;
  }
protected:
  //! get a parameter value pair from the command line
  bool getArg(const std::string name, std::string &res, const char *type) 
  {
    // todo this could be improved - but a commandline does not have a lot of args
    for (int a=1;a<m_argc;++a)
      {
	std::string carg(m_argv[a]);
	if (!isValidParam(carg)) continue;
	if (getParamName(carg)==name)
	  {
	    res=getParamValue(carg,type);
	    return true;
	  }
      }
    return false;
  }
  static bool isValidParam(const std::string &p)
  {
    return ((p.size()>=3)&&(p.substr(0,2)=="--"));
  }
  static std::string getParamName(const std::string &p)
  {
    DOPE_CHECK(isValidParam(p));
    std::string r(p.substr(2));
    std::string::size_type pos=r.find_last_of('=');
    if (pos!=std::string::npos)
      r=r.substr(0,pos);
    return r;
  }
  static std::string getParamValue(const std::string &p, const char *type)
  {
    DOPE_CHECK(isValidParam(p));
    std::string r(p.substr(2));
    std::string::size_type pos=r.find_first_of('=');
    if (pos!=std::string::npos) 
      {
	// '=' found
	if (pos+1<r.size())
	  // there is at least one character after =
	  return r.substr(pos+1);
	// no character after =
	return "";
      }
    // no '=' found
    if (std::string(type)=="bool")
      return "1";
    return "";
  }
  bool begins(const std::string &s, const std::string &b)
  {
    // this was return s.compare(b,0,b.size()); 
    // perhaps my book is wrong it seems it should be compare(0,b.size(),b) ?
    if (s.size()<b.size())
      return false;
    for (std::string::size_type i=0;i<b.size();++i)
      if (s[i]!=b[i]) return false;
    return true;
  }
  
  int m_argc;
  char **m_argv;
  int m_depth;
  bool m_gotHelpArg;
  std::string m_configFile;
};
typedef ArgvParserT<> ArgvParser;

#undef TYPE_NAME

#endif
