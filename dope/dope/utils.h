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
   \file utils.h
   \brief Utility functions
   \author Jens Thiele
*/

#ifndef DOPE_UTILS_H
#define DOPE_UTILS_H

#define DOPE_HAVE_SSTREAM 1

#ifdef DOPE_HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <string>
#include "typenames.h"
#include "dopeexcept.h"

//! convert any type to a string which has operator<<
template<typename X>
std::string anyToString(X z)
{
#ifdef DOPE_HAVE_SSTREAM
  std::ostringstream o;
#else
  std::ostrstream o;
#endif

  // todo do this for float/double only and think about precision 
  // i don't want to loose any information but it should look good anyway
  // seems like this is a problem
  // i once fixed it but can't remember -> i will have to grep through all the sources
  // i also have to take care about the locale
  o.setf(std::ios::fixed);
  o.precision(50);
  o.unsetf(std::ios::showpoint);
#ifdef DOPE_HAVE_SSTREAM
  o << z;
  return o.str();
#else  
  o << z <<std::ends;
  char *s=o.str();
  std::string erg=s;
  delete [] s;
  return erg;
#endif
};

//! specialization for string
inline std::string anyToString(std::string s)
{
  return s;
}
inline std::string anyToString(unsigned char ui8)
{
  return anyToString(static_cast<unsigned int>(ui8));
}
inline std::string anyToString(signed char i8)
{
  return anyToString(static_cast<int>(i8));
}

//! convert string to any type which has operator>>
template<typename X>
X &stringToAny(const std::string &s,X &x)
{
#ifdef DOPE_HAVE_SSTREAM
  std::stringbuf buf;
#else
  std::strstreambuf buf;
#endif
  std::istream ist(&buf);
  std::ostream ost(&buf);
  ost << s;
  if (!(ist >> x))  throw StringConversion(s,TypeNameTrait<X>::name());
  return x;
};

//! specialization for string
/*!
  seems stupid but is useful because the non-specialized version would be stupid
*/
inline std::string &stringToAny(const std::string &s,std::string &res)
{
  return (res=s);
}
inline unsigned char &stringToAny(const std::string &s,unsigned char &res)
{
  unsigned ui;
  stringToAny(s,ui);
  res=static_cast<unsigned char>(ui);
  return res;
}
inline signed char &stringToAny(const std::string &s,signed char &res)
{
  int ui;
  stringToAny(s,ui);
  res=static_cast<signed char>(ui);
  return res;
}

//! string s begins with p
inline bool begins(const std::string &s, const std::string &b)
{
  // this was return s.compare(b,0,b.size()); 
  // perhaps my book is wrong it seems it should be compare(0,b.size(),b) ?
  if (s.size()<b.size())
    return false;
  for (std::string::size_type i=0;i<b.size();++i)
    if (s[i]!=b[i]) return false;
  return true;
}

//! split string
inline bool split(const std::string &s, std::string &first, std::string &second, char c)
{
  std::string::size_type p=s.find_first_of(c);
  if (p!=std::string::npos)
    {
      first=s.substr(0,p);
      if (p+1<s.size())
	second=s.substr(p+1);
      else
	second.clear();
      return true;
    }
  //    else
  first=s;
  return false;
}

#endif
