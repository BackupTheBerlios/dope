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
   \file uri.h
   \brief URI handling
   \author Jens Thiele
*/

#ifndef DOPE_URI_H
#define DOPE_URI_H

#include "network.h"

//!* uri class - not really general - focused on http scheme/ "generic URI"
/*! 
  todo: this class is in a bade shape 
  look at the relevant rfc's or perhaps use an existing library
  rfc 2396
*/

class URI
{
public:
  URI(const char *_uri);

  const InternetAddress &getAddress() const
  {
    assert(adr.get());
    return *(adr.get());
  }
  const std::string &getPath() const
  {
    return relpart;
  }
  const std::string &getHost() const
  {
    return hostpart;
  }
  
  static std::string urlEncode(const std::string in) 
  {
    std::string res;
    urlEncode(in,res);
    return res;
  }
  static void urlEncode(const std::string in, std::string &out);
  static void urlDecode(const std::string in, std::string &out);
  
protected:
  DOPE_SMARTPTR<InternetAddress> adr;
  std::string hostpart;
  std::string relpart;

  bool split(const std::string &s, std::string &first, std::string &second, char c)
  {
    std::string::size_type p=s.find_first_of(c);
    if (p!=std::string::npos)
      {
	first=s.substr(0,p);
	assert(p+1<s.size());
	second=s.substr(p+1);
	return true;
      }
    //    else
    first=s;
    return false;
  }
  
};

#endif
