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
*/
#include <stdio.h>
#ifndef NDEBUG
#include <assert.h>
#define DOPE_CHECK(expr) assert(expr);
#else
# define DOPE_CHECK(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   (DOPE_FATAL("assertion failed"__STRING(expr)))))
#endif // NDEBUG

/*!
  \def DOPE_FATAL(msg)
  \brief fatal error - exit with a short message
*/
#define DOPE_FATAL(msg) {std::cerr << "FATAL: " << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << msg << std::endl;exit(1);}
/*! 
  \def DOPE_WARN(msg)
  \brief print a warning message
*/
#define DOPE_WARN(msg) {std::cerr << "WARN: " << __FILE__ << ":" << __LINE__ << msg;}

#endif // DOPE_DOPE_H

