#ifndef DOPE_EXCEPT_H
#define DOPE_EXCEPT_H

#include <stdexcept>

//! if for some reason a read fails completely - this exception is thrown
class ReadError : public std::runtime_error
{
public:
  ReadError(const std::string &e) : std::runtime_error(e)
  {
  }
};


#endif
