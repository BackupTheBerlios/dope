#ifndef DOPE_ADAPTER2TO1_H
#define DOPE_ADAPTER2TO1_H

#include "dope.h"
#include "streamtraits.h"
#include <string>

//! layer 2 to layer 1 adapter - just throws away all the addional information
template <typename Layer1>
class Layer2InAdapter
{
protected:
  Layer1 &layer1;
public:
  Layer2InAdapter(Layer1 &_layer1) : layer1(_layer1)
  {}

  typedef Layer2InStreamTraits Traits;

#define DOPE_SIMPLE(type) \
DOPE_INLINE Layer2InAdapter &simple(type &data, MemberName) \
  { \
    layer1.in(data); \
    return *this; \
  } \
DOPE_INLINE Layer2InAdapter &simple(type &data) \
  { \
    layer1.in(data); \
    return *this; \
  }

  DOPE_SIMPLE(bool);
  DOPE_SIMPLE(char);
  DOPE_SIMPLE(signed char);
  DOPE_SIMPLE(unsigned char);
  DOPE_SIMPLE(short);
  DOPE_SIMPLE(unsigned short);
  DOPE_SIMPLE(int);
  DOPE_SIMPLE(unsigned int);
  DOPE_SIMPLE(long);
  DOPE_SIMPLE(unsigned long);
  DOPE_SIMPLE(float);
  DOPE_SIMPLE(double);

#undef DOPE_SIMPLE

  //! simple for std::string which is in here because it isn't a composite
  DOPE_INLINE Layer2InAdapter &simple(std::string &data, MemberName = NULL)
  {
    // todo think about a solution without copy overhead - is there a (std::) way to pass a pointer to std::string ?
    char *str=NULL;
    dynCString(str);
    assert(str);
    data=str;
    delete [] str;
    return *this;
  }

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE Layer2InAdapter &simple(T& data, MemberName = NULL) {
    ::composite(*this, data);
    return *this;
  }
  
  // methods not named simple - because the meaning of the type is ambiguous

  template <typename T>
  Layer2InAdapter &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }
  
  //! C-style string (0-terminated) into newly allocated memory
  /*! ATTENTION:
    if memory was allocated it must be freed manually before
  */
  DOPE_INLINE Layer2InAdapter &dynCString(char* &data, MemberName = NULL)
  {
    size_t s;
    simple(s);
    assert(s<10000); // todo remove this again
    data=new char[s+1];
    layer1.in(data,s);
    data[s]=0;
    return *this;
  }
  
  //! C-style string (0-terminated) with no more than max chars with previously allocated memory
  /*! in the moment an exception is thrown if the string to read is bigger than max
    - alternatively one could cut the string (like XTL) does */
  DOPE_INLINE Layer2InAdapter &fixedCString(char* data, size_t max, MemberName = NULL)
  {
    size_t s;
    simple(s);
    if (s>max)
      std::overflow_error(__PRETTY_FUNCTION__);
    layer1.in(data,s);
    data[s]=0;
    return *this;
  }

  //! C-vector of variable size
  /*! ATTENTION:
    previously allocated memory must be freed manually before 
  */
  template <class T>
  DOPE_INLINE Layer2InAdapter& dynCVector(T* &data, size_t &size, MemberName = NULL) {
    simple(size);
    data=new T[size];
    for(size_t i=0;i<size;i++)
      simple(data[i]);
    return *this;
  }

  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE Layer2InAdapter& fixedCVector(T data[], size_t size, MemberName = NULL) {
    for(size_t i=0;i<size;i++)
      simple(data[i]);
    return *this;
  }

  //! types conforming to the STL container concept
  /*!
    requires T::value_type to have a copy constructor 
  */
  template <class T>
  DOPE_INLINE Layer2InAdapter& container(T& data) {
    data.clear();
    size_t size;
    simple(size);
    for(size_t i=0;i<size;++i) {
      typename T::value_type v;
      simple(v);
      data.insert(data.end(),v);
    }
    return *this;
  }
protected:  
  DOPE_INLINE Layer2InAdapter &CString(char *data)
  {
    size_t s=std::strlen(data);
    simple(s);
    layer1.in(data,s);
    return *this;
  }

};

template <typename Layer1>
class Layer2OutAdapter
{
protected:
  Layer1 &layer1;
public:
  Layer2OutAdapter(Layer1 &_layer1) : layer1(_layer1)
  {}

  typedef Layer2OutStreamTraits Traits;

  void flush()
  {
    layer1.flush();
  }

#define DOPE_SIMPLE(type) \
DOPE_INLINE Layer2OutAdapter &simple(type data, MemberName) \
  { \
    layer1.out(data); \
    return *this; \
  } \
DOPE_INLINE Layer2OutAdapter &simple(type data) \
  { \
    layer1.out(data); \
    return *this; \
  }

  DOPE_SIMPLE(bool);
  DOPE_SIMPLE(char);
  DOPE_SIMPLE(signed char);
  DOPE_SIMPLE(unsigned char);
  DOPE_SIMPLE(short);
  DOPE_SIMPLE(unsigned short);
  DOPE_SIMPLE(int);
  DOPE_SIMPLE(unsigned int);
  DOPE_SIMPLE(long);
  DOPE_SIMPLE(unsigned long);
  DOPE_SIMPLE(float);
  DOPE_SIMPLE(double);

#undef DOPE_SIMPLE

  //! simple for std::string which is in here because it isn't a composite
  DOPE_INLINE Layer2OutAdapter &simple(DOPE_CONSTOUT std::string &data, MemberName = NULL)
  {
    return dynCString(data.c_str());
  }

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE Layer2OutAdapter &simple(DOPE_CONSTOUT T& data, MemberName = NULL) {
    ::composite(*this, data);
    return *this;
  }

  template <typename T>
  Layer2OutAdapter &ext(const T& data)
  {
    ::composite(*this,data);
    return *this;
  }

  // methods not named simple - because the meaning of the type is ambiguous
  
  //! C-style string (0-terminated) into newly allocated memory
  /*! ATTENTION:
    if memory was allocated it must be freed manually before
  */
  DOPE_INLINE Layer2OutAdapter &dynCString(const char * data, MemberName = NULL)
  {
    return CString(data);
  }
  
  //! C-style string (0-terminated) with no more than max chars with previously allocated memory
  DOPE_INLINE Layer2OutAdapter &fixedCString(char* data, size_t max, MemberName = NULL)
  {
    return CString(data);
  }

  //! C-array (variable size)
  template <class T>
  DOPE_INLINE Layer2OutAdapter& dynCVector(T data[], size_t size, MemberName = NULL) {
    simple(size);
    for(unsigned i=0;i<size;i++)
      simple(data[i]);
    return *this;
  }

  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE Layer2OutAdapter& fixedCVector(T data[], size_t size, MemberName = NULL) {
    for(size_t i=0;i<size;i++)
      simple(data[i]);
    return *this;
  }

  //! types conforming to the STL container concept
  /*
    \todo if DOPE_CONSTOUT == "const" we have to use the const_iterator
  */
  template <class T>
  DOPE_INLINE Layer2OutAdapter& container(T& data) {
    size_t size=data.size();
    simple(size);
    //    for(typename T::const_iterator i=data.begin();i!=data.end();++i)
    for(typename T::iterator i=data.begin();i!=data.end();++i)
      simple(*i);
    return *this;
  }
protected:  
  DOPE_INLINE Layer2OutAdapter &CString(const char *data)
  {
    assert(data);
    size_t s=std::strlen(data);
    simple(s);
    layer1.out(data,s);
    return *this;
  }
};

#endif
