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

#include <dope/network.h>

//!* uri class - not really general - focused on http scheme/ "generic URI"
/*! 
  todo: this class is in a bade shape 
  look at the relevant rfc's or perhaps use an existing library
  rfc 2396
*/

class URI
{
public:
  URI(const char *_uri)
  {
    assert(_uri);
    std::string uri(_uri);
    // extract scheme part scheme:
    std::string scheme;
    split(uri,scheme,uri,':');
    if (scheme=="http")
      {
	// skip "//"
	uri=uri.substr(2);
      }
    else
      assert(0);
    // extract userinfo@host:port
    // look for path seperator if not found use complete
    std::string hostpart;
    std::string path;
    split(uri,hostpart,path,'/');
    std::string user,hostport;
    if (!split(hostpart,user,hostport,'@'))
      hostport=user;
    std::string host,port;
    if (!split(hostport,host,port,':'))
      {
	host=hostport;
	std::cerr << "Using default port: "<<port<<std::endl;
	port="80";
      }
    unsigned pnum;
    stringToAny(port,pnum);
    adr=DOPE_SMARTPTR<InternetAddress>(new InternetAddress(HostAddress(host.c_str()),Port(pnum)));
    relpart=std::string("/")+path;
  }

  /*  URI(const InternetAddress &_adr,const char *_relpart) : adr(_adr)
      {
      assert(_relpart);
      relpart=_relpart;
      }*/

  const InternetAddress &getAddress() const
  {
    assert(adr.get());
    return *(adr.get());
  }
  const std::string &getPath() const
  {
    return relpart;
  }
  
protected:
  DOPE_SMARTPTR<InternetAddress> adr;
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
