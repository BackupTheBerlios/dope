/*
 * Copyright (C) 2003 Jens Thiele <karme@berlios.de>
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
  \file dope/smartptr.h
  \brief class smartptr
  \author Jens Thiele
*/

#ifndef DOPE_SMARTPTR_H
#define DOPE_SMARTPTR_H

#include "dope.h"

#ifdef DOPE_USE_BOOST_SHARED_PTR

//! smart pointer type to use - reference implementation is boost::shared_ptr
/*!
  this is a define because it is a template type
  => you can't use a typedef - or is there somethin like this:
  template <typename X>
  typedef foo<X> bar<X>;
  dope should use a namespace and then import shared_ptr into its namespace
  but this would still not solve the problem to alias a template class
  closest solution I have found is a traits like concept - example:
	template<typename X, typename Y = allocator<X> >
	struct List
	{
	        typedef list<X,Y> T;
	};

	List<int>::T l;
*/
#include <boost/smart_ptr.hpp>
#define DOPE_SMARTPTR boost::shared_ptr

#else
#error you must define DOPE_SMARTPTR here

#endif



#endif
