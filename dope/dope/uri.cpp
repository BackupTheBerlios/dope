#include "uri.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

URI::URI(const char *_uri)
{
  DOPE_ASSERT(_uri);
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
    // todo
    DOPE_CHECK(0);
  // extract userinfo@host:port
  // look for path seperator if not found use complete
  std::string path;
  split(uri,hostpart,path,'/');
  std::string user,hostport;
  if (!split(hostpart,user,hostport,'@'))
    hostport=user;
  std::string host,port;
  if (!split(hostport,host,port,':'))
    {
      host=hostport;
      port="80";
      //      std::cerr << "Using default port: "<<port<<std::endl;
    }
  unsigned pnum;
  stringToAny(port,pnum);
  adr=DOPE_SMARTPTR<InternetAddress>(new InternetAddress(HostAddress(host.c_str()),Port(pnum)));
  relpart=std::string("/")+path;
}


char *
URI::curl_escape(const char *string, int length)
{
  int alloc = (length?length:(int)strlen(string))+1;  
  char *ns = (char *)malloc(alloc);
  DOPE_CHECK(ns);
  
  unsigned char in;
  int newlen = alloc;
  int index=0;

  length = alloc-1;
  while(length--) {
    in = *string;
    if(' ' == in)
      ns[index++] = '+';
    else if(!(in >= 'a' && in <= 'z') &&
            !(in >= 'A' && in <= 'Z') &&
            !(in >= '0' && in <= '9')) {
      /* encode it */
      newlen += 2; /* the size grows with two, since this'll become a %XX */
      if(newlen > alloc) {
        alloc *= 2;
        ns = (char *)realloc(ns, alloc);
        if(!ns)
          return NULL;
      }
      sprintf(&ns[index], "%%%02X", in);

      index+=3;
    }
    else {
      /* just copy this */
      ns[index++]=in;
    }
    string++;
  }
  ns[index]=0; /* terminate it */
  return ns;
}

char *
URI::curl_unescape(const char *cstring, int length)
{
  int alloc = (length?length:(int)strlen(cstring))+1;
  char *ns = (char *)malloc(alloc);
  DOPE_CHECK(ns);

  unsigned char in;
  int index=0;
  unsigned int hex;
  char querypart=0; /* everything to the right of a '?' letter is
                           the "query part" where '+' should become ' '.
                           RFC 2316, section 3.10 */
  
  while(--alloc > 0) {
    in = *cstring;
    if(querypart && ('+' == in))
      in = ' ';
    else if(!querypart && ('?' == in)) {
      /* we have "walked in" to the query part */
      querypart=1;
    }
    else if('%' == in) {
      /* encoded part */
      if(sscanf(cstring+1, "%02X", &hex)) {
        in = hex;
        cstring+=2;
        alloc-=2;
      }
    }
    
    ns[index++] = in;
    cstring++;
  }
  ns[index]=0; /* terminate it */
  return ns;
  
}

std::string 
URI::urlEncode(const std::string &in) 
{
  char *n=curl_escape(in.c_str(),in.size());
  std::string res(n);
  free(n);
  return res;
}
