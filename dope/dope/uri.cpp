#include "uri.h"
#include "utils.h"

URI::URI(const char *_uri)
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

void
URI::urlEncode(const std::string ins, std::string &ns)
{
  /* this is the c++ version of curl_escape http://curl.sourceforge.net */
  int alloc = ins.size()+1;
  ns.resize(alloc);
  unsigned char in;
  int newlen = alloc;
  int index=0;
  int co=0;
  int length = alloc-1;
  while(length--) {
    in = ins[co];
    if(' ' == in)
      ns[index++] = '+';
    else if(!(in >= 'a' && in <= 'z') &&
            !(in >= 'A' && in <= 'Z') &&
            !(in >= '0' && in <= '9')) {
      /* encode it */
      newlen += 2; /* the size grows with two, since this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        ns.resize(alloc);
      }
      sprintf(&ns[index], "%%%02X", in);
      index+=3;
    }
    else {
      /* just copy this */
      ns[index++]=in;
    }
    ++co;
  }
  ns.resize(index);
}

void
URI::urlDecode(const std::string in, std::string &out)
{
  /* this is the c++ version of curl_unescape http://curl.sourceforge.net */
  assert(0);
}
