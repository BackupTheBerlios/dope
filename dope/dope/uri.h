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
