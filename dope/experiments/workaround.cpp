#include <string>
#include <iostream>
#include <fstream>

#define TOMUCH 4000

template <typename X>
class workaround
{
protected:
  X &x;
  std::streamsize missed;
public:
  workaround(X &_x) : x(_x), missed(0)
  {
  }

  std::streamsize sgetn(char *b, std::streamsize n)
  {
    std::streamsize r=x.sgetn(b,n);
    return r;
  }
  int sputbackc(char c)
  {
    return x.sputbackc(c);
  }
};


int main()
{
  workaround<std::streambuf> s(*cin.rdbuf());
  char cb[TOMUCH];
  unsigned r=s.sgetn(cb,TOMUCH);
  assert(r>10);
  // this is the problem
  // if you unget when previously get failed
  for (unsigned i=r-1;i>10;--i)
    s.sputbackc(cb[i]);
  char b[51];
  s.sgetn(b,50);
  b[50]=0;
  std::cout << b << std::endl;
}

