// ---------------------------------------------------------------------------
// 'lib-netstream++' is a library that helps you to build networked 
//  applications with c++ or to be a base to build further abstract
//  coomunication libraries
//  it provides two classes: inetstream and onetstream that should
//  behave like any other standard istream/ostream
//
//  Mainly it is a wrapper around a subset of libc system calls related
//  to sockets (see also info libc sockets) 
//  TODO:
//  * only tested with linux
//  Current restrictions:
//  * only IP4
//  * no internationilzation support
//  * only SOCK_STREAM sockets are supported (no plans to change this)
//  * only internet namespace socket adresses are supported (no named sockets)
//
//  Copyright (C) Jens Thiele <karme@unforgettable.com>
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this library; if not, write to the Free
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// ---------------------------------------------------------------------------

#ifndef DOPE_NETWORK_H
#define DOPE_NETWORK_H

// define IPPORT_RESERVED and IPPORT_USERRESERVED
// and internet addresses
#include "dope.h"

// standard c++ includes
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <streambuf>
#include <list>
#include <algorithm>
#include <cerrno>
#include <ios>
#include <map>
#include <stdexcept>

// "unix" includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h> // timeval
#include <sys/types.h>
#include <unistd.h>

// sigc++
#include <sigc++/signal_system.h>

// other dope includes
#include "dopeexcept.h"
#include "timestamp.h"

// uncomment this one if you don't want to use exceptions
//#define LIB_NET_NO_EXCEPTIONS

/** class representing a port
   * @author Jens Thiele <karme@unforgettable.com>
   * May 31 2000 
   */
class Port{
public:
  static Port get_reserved(){return Port(IPPORT_RESERVED);}
  static Port get_user_reserved(){return Port(IPPORT_USERRESERVED);}

  Port(unsigned short int _port_no,bool host_byte_order=true)
  {
    if (host_byte_order)
      libc_portno=htons(_port_no);
  }
  //  Port(){}
  
  unsigned short int get_hportno() const 
  {return ntohs(libc_portno);}
  unsigned short int get_nportno() const 
  {return libc_portno;}
  bool operator=(const Port &op) const 
  {return get_nportno()==op.get_nportno();}
private:
  unsigned short int libc_portno;
};

/** class representing an internet host address
   * @author Jens Thiele <karme@unforgettable.com>
   * May 31 2000 
   */
class HostAddress{
public:
  /* get address of host named by name 
   * uses set_from_name
   * Attention read comments to set_from name (it may throw an exception!)
   */
  HostAddress(const char *name);
  // create pseudo address for listening on incoming connections
  HostAddress() 
  {set_accept_connections();}
  // just take network byte order address as unsigned long int
  HostAddress(unsigned long int _host_address) : host_address(_host_address)
  {}
  void set_accept_connections()
  {host_address=INADDR_ANY;}
  bool get_accept_connections()
  {return host_address==INADDR_ANY;}
  void set_loopback()
  {host_address=INADDR_LOOPBACK;}
  void set_broadcast()
  {host_address=INADDR_BROADCAST;}

  /**
     This method sets the Internet host address from the
     standard numbers-and-dots notation.
     If the address isn't valid it throws an exception / returns false
  */
  bool set_from_number_and_dot(const char *name);
  /**
     This method sets the Internet host address from it's
     domainname or standard numbers-and-dots notation.
     Attention:
     This may need a name lookup which blocks for a long time
     especially in case of name server or network problems 
     On any error: this method throws an exception / returns false
  */
  bool set_from_name(const char *name);
  in_addr get_host_address() const;
private:
  unsigned long int host_address;
};

/** class representing an internet address (specialized address)
   * @author Jens Thiele <karme@unforgettable.com>
   * May 31 2000 
   */
class InternetAddress {
public:
  InternetAddress(const HostAddress &_in_host_addr,const Port &_port);
  //  InternetAddress(){}
  
  sockaddr *get_socket_address();
  bool get_accept_connections() 
  {return in_host_addr.get_accept_connections();}
private:
  sockaddr_in sock_addr_in;
  HostAddress in_host_addr;
  Port port;
};

/** class representing a socket
   * @author Jens Thiele <karme@unforgettable.com>
   * May 31 2000 
   */
class Socket{
public:
  // create "client socket object"
  Socket(const InternetAddress &_internet_address, bool runInit=false);
  // create "server socket object"
  Socket(const Port &_port, bool runInit=false);
  // use ready system socket
  Socket(const InternetAddress &_internet_address,int _socket_handle, bool runInit=false);

  /** create system socket
      if it is a "client socket":
      connect to internet address
      if it is a "server socket":
      bind socket to address and listen
  */
  bool init();
  bool reuse_socket();
  bool bind_socket();
  bool listen_socket();
  bool connect_socket();
  int get_handle() const
  {
    return socket_handle;
  }
  bool get_close_on_delete()
  {
    return close_on_delete;
  }
  void set_close_on_delete(bool _c)
  {
    close_on_delete=_c;
  }
  ~Socket();

  //! set socket to blocking or non-blocking mode - default is blocking
  void setBlocking(bool block);
  
private:
  bool close_on_delete;
  int socket_handle;
  InternetAddress internet_address;
};

//! basic netstreambuffer class
/*! 
  todo:
  the constructors may throw exceptions which aren't correctly handled yet
  replace _M_* specific to gnu stdc++ lib? with corresponding functions
*/

template <typename _CharT, typename _Traits = std::char_traits<_CharT> >
class BasicNetStreamBuf : public std::basic_streambuf<_CharT, _Traits>
{
protected:
Socket sock;

public:
  typedef std::basic_streambuf<_CharT, _Traits>::char_type char_type;
  typedef std::basic_streambuf<_CharT, _Traits>::traits_type traits_type;
  typedef std::basic_streambuf<_CharT, _Traits>::int_type int_type;

  BasicNetStreamBuf(const InternetAddress &in_address) 
    : sock(in_address,true)
  {
    init();
  }
  
  BasicNetStreamBuf(const Socket &_sock) 
    : sock(_sock)
  {
    init();
  }

  //! test for input
  /*!
    \param timeout from man 2 select: upper bound on the amount of time elapsed
    before select returns. It may be zero, causing  select  to
    return  immediately.  If  timeout  is  NULL  (no timeout),
    select can block indefinitely.

    \return true if data is available otherwise false
  */
  bool select(const TimeStamp* timeout=NULL)
  {
    if (in_avail()>0)
      return true;
    bool ret=false;
    fd_set read_fd_set;
    FD_ZERO(&read_fd_set);
    int fd=get_handle();
    FD_SET(fd,&read_fd_set);
    timeval ctimeout;
    if (timeout) {
      ctimeout.tv_sec=timeout->getSec();
      ctimeout.tv_usec=timeout->getUSec();
    }
    int av=0;
    while ((av=::select (fd+1, &read_fd_set, NULL, NULL, (timeout) ? (&ctimeout) : NULL))<0) {
      if (errno!=EINTR) 
	DOPE_FATAL("select failed");
    }
    return av>0;
  }

  //! return corresponding file handle / descriptor
  int get_handle(){
    return sock.get_handle();
  }

  //! set socket to blocking or non-blocking mode - default is blocking
  void setBlocking(bool block)
  {
    sock.setBlocking(block);
  }
  
  ~BasicNetStreamBuf()
  {
    freeBuf();
    if (pbase())
      delete [] pbase();
  }

  
protected:
  int_type underflow()
  {
    freeBuf();
    int bsize=1024;
    char_type * newBuf=new char_type[bsize];
    setg(newBuf,newBuf,newBuf+bsize);
    ssize_t res;
    ssize_t toread=sizeof(char_type)*bsize;
    res=read(sock.get_handle(),(void *)gptr(),toread);
    if (res>0) {
      if (res!=toread) {
	DOPE_CHECK(toread>res);
	setg(eback(),gptr(),gptr()+res);
      }
      int_type r=traits_type::to_int_type(*gptr());
      setg(eback(),gptr()+1,egptr());
      return r;
    }
    // else
    freeBuf();
    return traits_type::eof();
  }

  /*
  std::streamsize 
  xsgetn(char_type* s, std::streamsize n)
  {
    std::streamsize __ret = 0;
    while (__ret < n)
      {
	size_t __buf_len = in_avail();
	if (__buf_len > 0)
	    {
	      // consume chars in buffer
	      size_t __remaining = n - __ret;
	      size_t __len = std::min(__buf_len, __remaining);
	      traits_type::copy(s, gptr(), __len);
	      __ret += __len;
	      s += __len;
	      setg(eback(),gptr()+__len,egptr());
	      _M_in_cur_move(__len);
	    }
	  if (__ret < n)
	    {
	      // read from net
	      size_t remaining = n - __ret;
	      int got=read(sock.get_handle(),(void *)s,remaining);
	      if (got>0) {
		return __ret+got;
	      }
	      return __ret;
	    }
	}
      return __ret;
      }*/
  
  //! 
  /*! 
    \todo - why did i do this ?
  */
  int_type uflow()
  {
    int_type c = underflow();
    return c;
  }
  
  //! write to socket 
  int_type overflow(int_type i = traits_type::eof())
  {
    // todo - eof should close ?
    DOPE_CHECK(pbase());
    ssize_t towrite=((char *)pptr())-((char *)pbase());
    ssize_t written=0;
    do {
      ssize_t res=write(sock.get_handle(),(void *)((char *)pbase()+written),towrite-written);
      if (res<0)
	return traits_type::eof();
      written+=res;
    }while(written<towrite);
    pbump(-towrite/sizeof(char_type));
    if (i!=traits_type::eof())
      sputc(i);
    return 0;
  }

  int_type sync()
  {
    return overflow();
  }
  
  //! put back one character
  /*!
    two possible cases:
    there was no space left to put back characters
    the character simply must be stored

    \todo what should I do if I am called with eof ?
  */
  int_type pbackfail(int_type c  = traits_type::eof())
  { 
    // inspired from gcc std c++ lib streambuf.tcc
    bool haveSpace = gptr() && eback() < gptr();
    if (!haveSpace)
      {
	// acquire new space - if current pos != end pos we have to copy the chars
        assert(egptr()-eback()>=0);
	int_type oldSize=egptr()-eback();
	int_type pbSize=1000;
        int_type newSize=oldSize+pbSize;
	char_type* newBuf=new char_type[newSize];
	assert(eback()==gptr());
	traits_type::copy(newBuf+pbSize,gptr(),oldSize);
	freeBuf();
	setg(newBuf,newBuf+pbSize,newBuf+newSize);
      }
    //    sputbackc(traits_type::to_char_type(c)); calls pbackfail ? - i thought it only calls pbackfail if no space is available ?
    // todo there must be a better way to do this
    setg(eback(),gptr()-1,egptr());
    *gptr()=traits_type::to_char_type(c);
    return c;
  }

  void init()
  {
    _M_mode = std::ios_base::in | std::ios_base::out; // sockets are always read/write ?
    _M_buf_unified = false; // we want to use different buffers for input/output
    setg(NULL,NULL,NULL);
    int bsize=1024;
    char_type *buf=new char_type[bsize];
    setp(buf,buf+bsize);
  }

  void freeBuf()
  {
    if (eback())
      {
	delete [] eback();
	setg(NULL,NULL,NULL);
      }
  }
};

typedef BasicNetStreamBuf<char> NetStreamBuf;

class NetStreamBufServer {
public:
  typedef int ID;
  typedef SigC::Signal2<void, ID, DOPE_SMARTPTR<NetStreamBuf> > NewConnectionSignal;
  typedef SigC::Signal2<void, ID, DOPE_SMARTPTR<NetStreamBuf> > DataAvailableSignal;
  typedef SigC::Signal2<void, ID, DOPE_SMARTPTR<NetStreamBuf> > ConnectionClosedSignal;

  NetStreamBufServer(const Port &_port) : server_socket(_port)
  {}

  bool init();
  //! test for input
  /*!
    \param timeout from man 2 select: upper bound on the amount of time elapsed
    before select returns. It may be zero, causing  select  to
    return  immediately.  If  timeout  is  NULL  (no timeout),
    select can block indefinitely.

    \return true if data is available otherwise false
  */
  bool select(const TimeStamp* timeout=NULL);

  NewConnectionSignal newConnection;
  DataAvailableSignal dataAvailable;
  ConnectionClosedSignal connectionClosed;


private:
  void closeStream(DOPE_SMARTPTR<NetStreamBuf> strPtr){
    std::map<ID, DOPE_SMARTPTR<NetStreamBuf> >::iterator it(client_sockets.begin());
    while (it!=client_sockets.end())
      {
	if (it->second.get()==strPtr.get())
	  {
	    closeStream(it->first);
	    return;
	  }
	++it;
      }
    if (it!=client_sockets.end())
      {
	FD_CLR (it->first, &active_fd_set);
	client_sockets.erase(it);
      }
  }
  void closeStream(ID id){
    std::map<ID, DOPE_SMARTPTR<NetStreamBuf> >::iterator it(client_sockets.find(id));
    if (it==client_sockets.end())
      throw std::invalid_argument(__PRETTY_FUNCTION__);
    FD_CLR (it->first, &active_fd_set);
    connectionClosed.emit(it->first,it->second);
    client_sockets.erase(it);
  }

  std::map<ID, DOPE_SMARTPTR<NetStreamBuf> > client_sockets;
  Socket server_socket;
  fd_set active_fd_set;
};

#endif
