/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
  \file dope/socket.h
  \brief socket wrapper
  \author Jens Thiele
*/

#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include "timestamp.h"

typedef unsigned short int Port;
typedef std::string HostAddress;
typedef std::pair<HostAddress, Port> InternetAddress;

//! minimalistic socket wrapper
class Socket{
private:
public:
  //! create "client socket object"
  Socket(const InternetAddress &_iaddr, bool runInit=false);
  //! create "server socket object"
  Socket(Port _port, bool runInit=false);
  //! use ready system socket
  Socket(unsigned long int _haddr, Port _port, int _handle, bool runInit=false);


  //! you sould really know what you do if you use it
  /*!
    \todo remove it - if possible
  */
  Socket(const Socket&);
  //! you sould really know what you do if you use it
  /*!
    \todo remove it - if possible
  */
  Socket();

  /*!
    create system socket
    if it is a "client socket":
    connect to internet address
    if it is a "server socket":
    bind socket to address and listen
  */
  bool init();

  //! accept new connection
  /*!
    \note: only useful with a server socket

    \returns new socket or NULL
  */
  Socket * accept();
  
  int getHandle() const 
  {return handle;}

  bool getCloseOnDelete() 
  {return closeOnDelete;}

  void setCloseOnDelete(bool _c)
  {closeOnDelete=_c;}

  ~Socket();

  //! set socket to blocking or non-blocking mode - default is blocking
  void setBlocking(bool block);

  //! test for input
  /*!
    \param timeout from man 2 select: upper bound on the amount of time elapsed
    before select returns. It may be zero, causing  select  to
    return  immediately.  If  timeout  is  NULL  (no timeout),
    select can block indefinitely.

    \return true if data is available otherwise false
  */
  bool inAvail(const TimeStamp* timeout=NULL);

  //! see man read
  int read(void *buf, size_t count);
  //! see man write
  int write(const void *buf, size_t count);

  //! inititialize socket library if needed
  /*!
    you can call this more than once
  */
  static bool globalInit();
  //! inititialize socket library if needed
  static bool globalDeinit();
protected:
  void reuse();
  bool bind();
  bool listen();
  bool connect();
  void close();
  
  bool server;
  bool closeOnDelete;
  int handle;
  InternetAddress iaddr;
  //! binary host address (ipv4)
  unsigned long int haddr;
};

#endif
