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
   \file typenames.h
   \brief type traits - map types to names
   \author Jens Thiele
*/

#ifndef DOPE_TYPENAMES
#define DOPE_TYPENAMES

#include "dope.h"
#include <string>
#include <list>
#include <vector>
#include <map>

#ifdef DOPE_USE_RTTI
#include <typeinfo>
#endif

#define LT '<'
#define GT '>'
#define SEP ','
#define PTR '*'
#define REF '&'
#define ARRAY "[]"

//! type name type
typedef std::string TypeNameType;

//! type name trait class
/*!
  traits class to get the name of a type
*/
template <typename X>
struct TypeNameTrait
{
  typedef X value_type;
  
  static DOPE_INLINE TypeNameType name() DOPE_FCONST
  {
#ifdef DOPE_USE_RTTI
    return typeid(X).name();
#else
    DOPE_FATAL("type is not registered");
    //    #warning Type has no registered type name
    return "unknown";
#endif
  }
};

//! partial specialization for const
template <typename X>
struct TypeNameTrait<const X>
{
  static DOPE_INLINE TypeNameType name()  DOPE_FCONST
  {
    return TypeNameType("const ")+TypeNameTrait<X>::name();
  }
};

//! partial specialization for pointers
template <typename X>
struct TypeNameTrait<X *>
{
  static DOPE_INLINE TypeNameType name() DOPE_FCONST
  {
    return TypeNameTrait<X>::name()+PTR;
  }
};

//! partial specialization for references
template <typename X>
struct TypeNameTrait<X &>
{
  static DOPE_INLINE TypeNameType name() DOPE_FCONST
  {
    return TypeNameTrait<X>::name()+REF;
  }
};

//! partial specialization for arrays
template <typename X>
struct TypeNameTrait<X[]>
{
  static DOPE_INLINE TypeNameType name() DOPE_FCONST
  {
    return TypeNameTrait<X>::name()+ARRAY;
  }
};

//! declare TypeNameTrait for a builtin type
#define DOPE_TYPE(type) \
struct TypeNameTrait<type> \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return #type; \
  } \
}

//! declare TypeNameTrait for a class (without templates)
#define DOPE_CLASS(type) \
struct TypeNameTrait<type> \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return #type; \
  } \
}

DOPE_TYPE(char);
DOPE_TYPE(unsigned char);
DOPE_TYPE(short);
DOPE_TYPE(unsigned short);
DOPE_TYPE(int);
DOPE_TYPE(unsigned int);
DOPE_TYPE(long);
DOPE_TYPE(unsigned long);
DOPE_TYPE(float);
DOPE_TYPE(double);
DOPE_TYPE(bool);
DOPE_CLASS(std::string);

//! declare TypeNameTrait for a class with one template argument
/*!
  \note the template argument must have a TypeNameTrait, too
*/
#define DOPE_CLASS_T1(type) \
template <typename T> \
struct TypeNameTrait<type<T> > \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return TypeNameType(#type)+LT+TypeNameTrait<T>::name()+GT; \
  } \
}

DOPE_CLASS_T1(std::list);
DOPE_CLASS_T1(std::vector);

//! declare TypeNameTrait for a class with two template arguments
/*!
  \note the template arguments each must have a TypeNameTrait, too
*/
#define DOPE_CLASS_T2(type) \
template <typename K, typename V> \
struct TypeNameTrait<type<K,V> > \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return TypeNameType(#type)+LT+TypeNameTrait<K>::name()+SEP+TypeNameTrait<V>::name()+GT; \
  } \
}

DOPE_CLASS_T2(std::pair);
DOPE_CLASS_T2(std::map);

//! declare TypeNameTrait for a class with 3 template arguments
/*!
  \note the template arguments each must have a TypeNameTrait, too
*/
#define DOPE_CLASS_T3(type) \
template <typename T1, typename T2, typename T3> \
struct TypeNameTrait<type<T1,T2,T3> > \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return TypeNameType(#type)+LT +TypeNameTrait<T1>::name() \
                              +SEP+TypeNameTrait<T2>::name() \
                              +SEP+TypeNameTrait<T3>::name() \
                              +GT; \
  } \
}

//! declare TypeNameTrait for a class with 4 template arguments
/*!
  \note the template arguments each must have a TypeNameTrait, too
*/
#define DOPE_CLASS_T4(type) \
template <typename T1, typename T2, typename T3,typename T4> \
struct TypeNameTrait<type<T1,T2,T3,T4> > \
{ \
  static DOPE_INLINE TypeNameType name() DOPE_FCONST \
  { \
    return TypeNameType(#type)+LT +TypeNameTrait<T1>::name() \
                              +SEP+TypeNameTrait<T2>::name() \
                              +SEP+TypeNameTrait<T3>::name() \
                              +SEP+TypeNameTrait<T4>::name() \
                              +GT; \
  } \
}

#undef LT
#undef GT
#undef SEP
#undef PTR
#undef REF
#undef ARRAY

#endif
