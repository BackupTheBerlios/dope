#ifndef DOPE_UTILS_H
#define DOPE_UTILS_H

#include <strstream>
#include <string>

template<typename X>
std::string anyToString(X z)
{
  std::ostrstream o;
  // todo do this for float/double only and think about precision 
  // i don't want to loose any information but it should look good anyway
  // seems like this is a problem
  // i once fixed it but can't remember
  o.setf(std::ios::fixed);
  o.precision(50);
  o.unsetf(std::ios::showpoint);
  o << z <<std::ends;
  char *s=o.str();
  std::string erg=s;
  delete [] s;
  return erg;
};
inline std::string anyToString(std::string s)
{
  return s;
}

template<typename X>
X &stringToAny(const std::string &s,X &x)
{
  std::strstreambuf buf;
  std::istream ist(&buf);
  std::ostream ost(&buf);
  ost << s;
  ist >> x;
  return x;
};
// seems stupid but is useful
inline std::string &stringToAny(const std::string &s,std::string &res)
{
  return (res=s);
}

#endif
