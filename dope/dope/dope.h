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
   \file dope.h
   \brief Main API header
   \author Jens Thiele
*/

#ifndef DOPE_DOPE_H
#define DOPE_DOPE_H

#include "dopeconfig.h"

//! should we use inline
#define DOPE_INLINE inline

//! should we use RTTI for typenames - PROBLEM: the names are platform dependant
// #define DOPE_USE_RTTI 1

//! should a output stream use const references ? - see also TODO
#define DOPE_CONSTOUT 
// #define DOPE_CONSTOUT const

//! special compiler directive to markup functions which always return the same value
#define DOPE_FCONST
/*
  #define DOPE_FCONST __attribute__ ((const))
*/

//! switch of all runtime asserts and other debugging aids
/*
  #define NDEBUG
*/

#ifdef DOPE_USE_BOOST

// boost::remove_const
#include <boost/type_traits.hpp>

//! smart pointer type to use - reference implementation is boost::shared_ptr
/*!
  this is a define because it is a template type
  => you can't use a typedef - or is there somethin like this:
  template <typename X>
  typedef foo<X> bar<X>;
*/
#include <boost/smart_ptr.hpp>
#define DOPE_SMARTPTR boost::shared_ptr

#else
#error you do not want to use boost => you have to replace boost::remove_const and boost::shared_ptr
#endif // DOPE_USE_BOOST

typedef const char * MemberName;

//! how to remove const (static_cast will at least produce warnings - if not an error)
/*
#define DOPE_REMOVE_CONST_CAST static_cast
*/
#define DOPE_REMOVE_CONST_CAST const_cast

extern int dope_micro_version;
extern int dope_minor_version;
extern int dope_major_version;


/*
  \def DOPE_CHECK(expr)
  \brief DOPE_CHECK is like assert but takes also effect when NDEBUG is defined

  it seems there is no assert which is not disabled by a \#define NDEBUG 

  \todo PRETTY_FUNCTION is gcc specific - what about FILE and LINE ?
*/
#include <cstdio>
#ifndef NDEBUG
#include <cassert>
#define DOPE_CHECK(expr) assert(expr);
#else
# define DOPE_CHECK(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   fatal(__FILE__,__FILE__,__PRETTY_FUNCTION__,"assertion failed"__STRING(expr))))
#endif // NDEBUG

#ifdef NDEBUG
inline void fatal(const char *file,int line,const char *func, const char *msg) 
{
  std::cerr << "FATAL: " << file << ":" << line << ":" << func << ": " << msg << "(errno="<<errno<<":"<<strerror(errno)<<"\n";
  terminate();
}
#endif


/*!
  \def DOPE_FATAL(msg)
  \brief fatal error - exit with a short message
*/
#define DOPE_FATAL(msg) {std::cerr << "FATAL: " << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << ": " << msg << "(errno="<<errno<<":"<<strerror(errno)<<"\n";terminate();}
/*! 
  \def DOPE_WARN(msg)
  \brief print a warning message
*/
#define DOPE_WARN(msg) {std::cerr << "WARNING: " << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << ": " << msg << "(errno="<<errno<<":"<<strerror(errno)<<"\n";}

#endif // DOPE_DOPE_H

