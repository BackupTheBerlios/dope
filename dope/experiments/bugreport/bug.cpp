#include <fstream>

#define TOMUCH 1000
int main()
{
  std::ifstream i("bug.cpp");
  std::streambuf &s=*i.rdbuf();
  char cb[TOMUCH];
  std::streamsize r=s.sgetn(cb,TOMUCH);
  // this is the problem
  // if you unget when previously get failed due to eof
  s.sungetc();
  // this would trigger the same problem
  // s.sputbackc('A');
  // and i think the real problem is pbackfail (_IO_default_pbackfail (genops.c))
  // which they call both
}

