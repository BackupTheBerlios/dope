#include "network.h"
#include "utils.h"

// ---------------------------------------------------------------------------
// 'lib-netstream++' is a library that helps you to build networked 
//  applications with c++ or to be a base to build further abstract
//  coomunication libraries
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


/// This namespace contains a network stream class corresponding to the iostream and fstream classes of the standard C++ library and some helper classes related to network programing
HostAddress::HostAddress(const char *name)
{
  set_from_name(name);
}
  
bool HostAddress::set_from_number_and_dot(const char *name){
  struct in_addr addr_in_struct;
  if (!inet_aton (name, &addr_in_struct))
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "set_from_number_and_dot: address not valid";
#endif
      return false;
    }
  host_address=addr_in_struct.s_addr;
  return true;
}
  
bool HostAddress::set_from_name(const char *name){
  hostent *he_ptr=gethostbyname(name);
  if (!he_ptr)
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw std::runtime_error(std::string(__PRETTY_FUNCTION__)+":hostname not valid:"+name);
#endif
      return false;
    }
  if (he_ptr->h_length!=sizeof(host_address))
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "set_from_name: problem with size of host adress returned by gethostbyname";
#endif
      return false;
    }
  host_address=((struct in_addr *)(he_ptr->h_addr))->s_addr;
  std::cout << "host address: "<<host_address<< std::endl;
  return true;
}
in_addr HostAddress::get_host_address() const {
  in_addr ret_v_in_addr;
  ret_v_in_addr.s_addr=host_address;
  return ret_v_in_addr;
}

InternetAddress::InternetAddress(const HostAddress &_in_host_addr,const Port &_port) : in_host_addr(_in_host_addr), port(_port){}

sockaddr *InternetAddress::get_socket_address(){
  sock_addr_in.sin_family=AF_INET;
  sock_addr_in.sin_addr=in_host_addr.get_host_address();
  sock_addr_in.sin_port=port.get_nportno();
  return (sockaddr *)&sock_addr_in;
}

Socket::Socket(const InternetAddress &_internet_address, bool runInit) 
  : close_on_delete(true),socket_handle(-1),internet_address(_internet_address)
{
  if (runInit)
    init();
}

Socket::Socket(const Port &_port, bool runInit)
  : close_on_delete(true),socket_handle(-1),internet_address(HostAddress(),_port)
{
  if (runInit)
    init();
}

Socket::Socket(const InternetAddress &_internet_address, int _socket_handle, bool runInit) 
  : close_on_delete(true),socket_handle(_socket_handle),internet_address(_internet_address)
{
  if (runInit)
    init();
}

bool Socket::init(){
  /* Create the socket. */
  socket_handle = socket (PF_INET, SOCK_STREAM, 0);
  if (socket_handle < 0)
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "Socket::init: can't create socket";
#endif
      return false;
    }
  if (!internet_address.get_accept_connections())
    return connect_socket();
#ifndef LIB_NET_NO_EXCEPTIONS
  try{
#endif
    if (!reuse_socket())
      DOPE_WARN("resuse_socket failed");
#ifndef LIB_NET_NO_EXCEPTIONS
  }catch (const char *const errstr){
    std::cerr << errstr << std::endl;
  }
#endif
  if (!bind_socket())
    return false;
  return listen_socket();
}

bool Socket::reuse_socket(){
  int reuser=1;
  int len=sizeof(reuser);
  if (setsockopt(socket_handle,SOL_SOCKET,SO_REUSEADDR,(void *)&reuser,len)<0) // (void *) cause of solaris
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "Socket::reuse_socket(): setsockopt SO_REUSEADDR failed";
#endif
      return false;
    }
  return true;
};

bool Socket::bind_socket(){
  if (bind (socket_handle, internet_address.get_socket_address(), sizeof (*internet_address.get_socket_address())) < 0)
    {
      if (close(socket_handle)<0)
	DOPE_WARN("close failed");
      socket_handle=-1;
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "Socket::bind_socket: can't bind socket";
#endif
      return false;
    }
  return true;
}
bool Socket::listen_socket(){
  if (listen (socket_handle, 1) < 0)
    {
      if (close(socket_handle)<0)
	DOPE_WARN("close failed");
      socket_handle=-1;
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "Socket::listen_socket: can't listen on socket";
#endif
      return false;
    }
  return true;
}
bool Socket::connect_socket(){
  if (connect(socket_handle, internet_address.get_socket_address(), sizeof (*internet_address.get_socket_address()))<0)
    {
      if (close(socket_handle)<0)
	DOPE_WARN("close failed");
      socket_handle=-1;
      sockaddr_in *sa=(sockaddr_in *)internet_address.get_socket_address();
      std::string ipstring;
      for (int i=0;i<4;++i)
	ipstring+=anyToString(int(((char *)(&(sa->sin_addr)))[i]))+".";
      std::cerr << "Can't connect to: "<<ipstring<<std::endl;
#ifndef LIB_NET_NO_EXCEPTIONS
      throw "Socket::connect_socket: can't connect";
#endif
      return false;
    }
  return true;
}
Socket::~Socket(){
  if (close_on_delete && (socket_handle!=-1))
    if (close(socket_handle)<0)
      DOPE_WARN("close failed");
}


bool NetStreamBufServer::init(){
  FD_ZERO(&active_fd_set);
  if (!server_socket.init())
    return false;
  FD_SET(server_socket.get_handle(),&active_fd_set);
  return true;
}

/** Block until input arrives on one or more active sockets.
   * create a new socket for a new connection
   */
bool NetStreamBufServer::select(const TimeStamp *timeout){
  fd_set read_fd_set = active_fd_set;
  timeval ctimeout;
  if (timeout) {
    ctimeout.tv_sec=timeout->getSec();
    ctimeout.tv_usec=timeout->getUSec();
  }
  if (::select (FD_SETSIZE, &read_fd_set, NULL, NULL, (timeout) ? (&ctimeout) : NULL) < 0)
    {
#ifndef LIB_NET_NO_EXCEPTIONS
      throw std::logic_error(__PRETTY_FUNCTION__);
#endif
      return false;
    }
  // which sockets have input pending ?
  for (int i = 0; i < FD_SETSIZE; ++i)
    if (FD_ISSET (i, &read_fd_set))
      {
	if (i == server_socket.get_handle())
	  {
	    /* Connection request on original socket. */
	    int new_socket_handle;
	    struct sockaddr_in clientname;
	    size_t struct_size = sizeof (clientname);
	    new_socket_handle = accept (server_socket.get_handle(),
					(struct sockaddr *) &clientname,
					&struct_size);
	    if (new_socket_handle < 0)
	      {
		if ((errno!=EINTR)&&(errno!=EWOULDBLOCK)) 
		  {
		    // unrecoverable error
#ifndef LIB_NET_NO_EXCEPTIONS
		    throw "NetStreamBufServer::wait_for_any_traffic: can't accept connection";
#endif
		    return false;
		  }
		// recoverable error but no new connection
	      }
	    else
	      {
		// Accepted connection
		// add to active_fd_set
		FD_SET (new_socket_handle, &active_fd_set);
		// create corresponding Socket object - don't call init because it is inited
		InternetAddress adr(HostAddress(clientname.sin_addr.s_addr),Port(clientname.sin_port,false));
		Socket new_socket(adr,new_socket_handle);
		// we want to copy this socket object and don't want to close the handle when leaving this scope
		new_socket.set_close_on_delete(false);
		DOPE_SMARTPTR<NetStreamBuf> nbufptr(new NetStreamBuf(new_socket));
		client_sockets[new_socket_handle]=nbufptr;
		newConnection.emit(new_socket_handle,nbufptr);
	      }
	  }
	else
	  {
	    assert(client_sockets.find(i)!=client_sockets.end());
	    try {
	      dataAvailable.emit(i,client_sockets[i]);
	    }catch(const ReadError &e) {
	      DOPE_WARN("Warning read error occured: "<<e.what()<<" => close stream");
	      closeStream(i);
	    }catch(const std::range_error &e) {
	      DOPE_WARN(e.what());
	    }
	  }
      }
  return true;
}


