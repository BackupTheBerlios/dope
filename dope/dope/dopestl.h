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
