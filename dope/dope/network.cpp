#include "network.h"
#include "utils.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h> // timeval

#if defined(DOPE_RESTORE_CONNECT)
#define connect DOPE_RESTORE_CONNECT
#endif

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



bool NetStreamBufServer::init(){
  FD_ZERO(&active_fd_set);
  if (!server_socket.init())
    return false;
  FD_SET(server_socket.getHandle(),&active_fd_set);
  return true;
}

bool NetStreamBufServer::select(const TimeStamp *timeout){
  bool ret=false;
  fd_set read_fd_set = active_fd_set;
  timeval ctimeout;
  if (timeout) {
    ctimeout.tv_sec=timeout->getSec();
    ctimeout.tv_usec=timeout->getUSec();
  }
  // Find the largest file descriptor
  // todo - this should not be done on each select
  int maxfd;
  if (client_sockets.empty())
    maxfd=server_socket.getHandle();
  else {
    // client_sockets is a map => sorted 
    maxfd=client_sockets.rbegin()->first;
    // does the std require a map to be sorted ascending ?
    assert(maxfd>=client_sockets.begin()->first);
  }
  
  while (::select (++maxfd, &read_fd_set, NULL, NULL, (timeout) ? (&ctimeout) : NULL) < 0)
    {
      if (errno!=EINTR)
	DOPE_FATAL("select failed");
    }
  // which sockets have input pending ?
  for (int i = 0; i < maxfd; ++i)
    if (FD_ISSET (i, &read_fd_set))
      {
	if (i == server_socket.getHandle())
	  {
	    /* Connection request on original socket. */
	    DOPE_SMARTPTR<Socket> newSocketPtr(server_socket.accept());
	    if (newSocketPtr.get()) {
	      int nhandle=newSocketPtr->getHandle();
	      // Accepted connection
	      // add to active_fd_set
	      FD_SET (nhandle, &active_fd_set);
	      // we want to copy this socket object and don't want to close the handle when leaving this scope
	      newSocketPtr->setCloseOnDelete(false);
	      DOPE_SMARTPTR<NetStreamBuf> nbufptr(new NetStreamBuf(*newSocketPtr));
	      client_sockets[nhandle]=nbufptr;
	      newConnection.emit(nhandle,nbufptr);
	    }
	  }
	else
	  {
	    assert(client_sockets.find(i)!=client_sockets.end());
	    try {
	      ret=true;
	      dataAvailable.emit(i,client_sockets[i]);
	    }catch(const ReadError &e) {
	      DOPE_WARN("Warning read error occured: "<<e.what()<<" => close stream");
	      closeStream(i);
	    }catch(const std::range_error &e) {
	      DOPE_WARN(e.what());
	    }
	  }
      }
  return ret;
}


