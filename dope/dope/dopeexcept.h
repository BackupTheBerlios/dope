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
   \file dopeexcept.h
   \brief Exceptions
   \author Jens Thiele
*/

#ifndef DOPE_EXCEPT_H
#define DOPE_EXCEPT_H

#include <stdexcept>
#include <errno.h>
#include <string.h>

//! if for some reason a read fails completely - this exception is thrown
struct ReadError : public std::runtime_error
{
  ReadError(const std::string &e) : std::runtime_error(e)
  {
  }
};

//! resource not found
struct ResourceNotFound : public std::runtime_error
{
  ResourceNotFound(const std::string &r,const std::string &error)
    : std::runtime_error(std::string("Resource: \""+r+"\" not available: "+error))
  {}
};

//! file not found
struct FileNotFound : public ResourceNotFound
{
  FileNotFound(const std::string &fname) : ResourceNotFound(std::string("file:")+fname,strerror(errno))
  {}
};

//! if converting a string to another type fails
struct StringConversion : public std::runtime_error
{
  StringConversion(const std::string &s, const std::string &tname) 
    : std::runtime_error(std::string("Can't convert \"")+s+"\" to "+tname)
  {
  }
};

#endif
