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
   \file dopestl.h
   \brief make STL types streamable
   \author Jens Thiele
*/

#ifndef DOPE_STL_H
#define DOPE_STL_H

#include "dope.h"
#include <utility>
#include <vector>
#include <list>
#include <map>

//! pair composite
template <class Stream, class T1, class T2>
DOPE_INLINE void composite(Stream& stream, std::pair<T1,T2>& data) {
  stream.simple(data.first,"first").simple(data.second,"second");
}

template <class Stream, class T1, class T2>
DOPE_INLINE void composite(Stream& stream, std::pair<const T1,T2>& data) {
  stream.simple(DOPE_REMOVE_CONST_CAST<T1&>(data.first),"first").simple(data.second,"second");
}


template <class Stream, class T1, class T2>
DOPE_INLINE void composite(Stream& stream, const std::pair<T1,T2>& data) {
  stream.simple(data.first,"first").simple(data.second,"second");
}

//! list composite
template <class Stream, class T>
DOPE_INLINE void composite(Stream& stream, std::list<T>& data) {
	stream.container(data);
}
template <class Stream, class T>
DOPE_INLINE void composite(Stream& stream, const std::list<T>& data) {
	stream.container(data);
}

//! vector composite
template <class Stream, class T>
DOPE_INLINE void composite(Stream& stream, std::vector<T>& data) {
	stream.container(data);
}
template <class Stream, class T>
DOPE_INLINE void composite(Stream& stream, const std::vector<T>& data) {
	stream.container(data);
}

//! map composite
template <class Stream, class T1, class T2>
DOPE_INLINE void composite(Stream& stream, std::map<T1, T2>& data) {
	stream.container(data);
}
template <class Stream, class T1, class T2>
DOPE_INLINE void composite(Stream& stream, const std::map<T1, T2>& data) {
	stream.container(data);
}
#endif
