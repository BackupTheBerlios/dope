#include <string>
#include <iostream>

class Stream
{
public:
  Stream &name(const std::string &s)
  {
    std::cout << s;
    return *this;
  }
};

class Manipulator
{
public:
  Manipulator(std::string _s) : s(_s)
  {
  }
  
  Stream &manip(Stream &t) const
  {
    return t.name(s);
  }

  std::string s;
};

Stream &operator|(Stream &s,const Manipulator &m)
{
  return m.manip(s);
}

int main()
{
  Stream s;
  s|Manipulator("hallo\n");
}
