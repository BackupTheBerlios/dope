/*! Concept of Layer2 streams

  This isn't code to be compiled - it is just to document the concept
*/

template <typename Layer1>
class Layer2Stream
{
protected:
  Layer1 &layer1;
public:
  Layer2Stream(Layer1 &_layer1) : layer1(_layer1)
  {}

  typedef InStreamTag|OutStreamTag StreamCategory;
  typedef StreamLayerTag<3> StreamLayer;
  
  Layer2Stream &simple(bool data, MemberName mname);
  Layer2Stream &simple(char data, MemberName mname);
  Layer2Stream &simple(unsigned char data, MemberName mname);
  Layer2Stream &simple(short data, MemberName mname);
  Layer2Stream &simple(unsigned short data, MemberName mname);
  Layer2Stream &simple(int data, MemberName mname);
  Layer2Stream &simple(unsigned int data, MemberName mname);
  Layer2Stream &simple(long data, MemberName mname);
  Layer2Stream &simple(unsigned long data, MemberName mname);
  Layer2Stream &simple(float data, MemberName mname);
  Layer2Stream &simple(double data, MemberName mname);

  //! simple for all types using the global composite function
  template <class T>
  DOPE_INLINE Layer2Stream &simple(T& data, MemberName mname) {
    ::composite(*this, data, mname);
    return *this;
  }

  // methods not named simple - because the meaning of the type is ambiguous
  
  //! C-style string (0-terminated) into newly allocated memory
  /*! ATTENTION:
    if memory was allocated it must be freed manually before
  */
  DOPE_INLINE Layer2Stream &dynCString(char* &data, MemberName mname);
  
  //! C-style string (0-terminated) with no more than max chars with previously allocated memory
  /*! in the moment an exception is thrown if the string to read is bigger than max
    - alternatively one could cut the string (like XTL) does */
  DOPE_INLINE Layer2Stream &fixedCString(char* data, size_t max, MemberName mname);

  //! C-vector of variable size
  /*! ATTENTION:
    previously allocated memory must be freed manually before */
  template <class T>
  DOPE_INLINE Layer2Stream& dynCVector(const T data[], size_t &size, MemberName mname);

  //! C-vector of fixed size
  template <class T>
  DOPE_INLINE Layer2Stream& fixedCVector(const T data[], size_t size, MemberName mname);

  //! types conforming to the STL container concept
  /*!
    requires T to have a copy constructor 
  */
  template <class T>
  DOPE_INLINE Layer2Stream& container(T const& data);
};
