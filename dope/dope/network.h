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

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <streambuf>
#include <list>
#include <algorithm>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ios>

#include <sigc++/signal_system.h>
#include <map>
#include <stdexcept>

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
  typedef _CharT char_type;
  typedef _Traits traits_type;
  typedef typename traits_type::int_type int_type;

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

  int get_handle(){
    return sock.get_handle();
  }

  ~BasicNetStreamBuf()
  {
    freeBuf();
  }
  
protected:
  int_type underflow()
  {
    if (_M_in_beg)
      {
	delete [] _M_in_beg;
	_M_in_beg=_M_in_cur=_M_in_end=NULL;
      }
    char_type c;
    ssize_t res=read(sock.get_handle(),(void *)&c,sizeof(c));
    if (res==sizeof(c))
      return traits_type::to_int_type(c);
    
    if (res>0)
      // todo - we got some chars but not all
      assert(0);
    ;
    if (res<0) {
      // man 2 read
      // todo
      return 0;
    }
    assert(res==0);
    return  traits_type::eof();
  }

  int_type uflow()
  {
    int_type c = underflow();
    return c;
  }

  
  
  int_type overflow(int_type i = traits_type::eof())
  {
    // todo - eof should close ?
    char_type c(traits_type::to_char_type(i));
    ssize_t res=write(sock.get_handle(),(void *)&c,sizeof(c));
    if (res==sizeof(c))
      return i; // todo ?
    if (res==0)
      assert(0); // ?
    // if (res<0)
    // error
    return traits_type::eof();
  }
  
  //! put back one character
  /*!
    two possible cases:
    there was no space left to put back characters
    the character simply must be stored
  */
  int_type pbackfail(int_type c  = traits_type::eof())
  { 
    // inspired from gcc std c++ lib streambuf.tcc
    bool haveSpace = _M_in_cur && _M_in_beg < _M_in_cur;
    if (!haveSpace)
      {
	// acquire new space - if current pos != end pos we have to copy the chars
        assert(_M_in_end-_M_in_beg>=0);
	int_type oldSize=_M_in_end-_M_in_beg;
	int_type pbSize=1000;
        int_type newSize=oldSize+pbSize;
	char_type* newBuf=new char_type[newSize];
	assert(_M_in_beg==_M_in_cur);
	traits_type::copy(newBuf+pbSize,_M_in_cur,oldSize);
	freeBuf();
	_M_in_beg=newBuf;
	_M_in_cur=_M_in_beg+pbSize;
	_M_in_end=_M_in_beg+newSize;
      }
    *(--_M_in_cur)=traits_type::to_char_type(c);
    return c;
  }

  /* todo implement it - default just calls uflow - if _M_in_end - _M_in_cur < s
     int_type xsgetn(char_type* b, streamsize s)
     {
     }
  
     int sync() 
     {
     return 0;
     }
  */
  

  void init()
  {
    _M_mode = std::ios_base::in | std::ios_base::out; // sockets are always read/write ?
    _M_buf_unified = false; // we want to use different buffers for input/output
  }

  void freeBuf()
  {
    if (_M_in_beg)
      delete [] _M_in_beg;
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

    \return true on success false otherwise
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
