#include <string>
#include <iostream>
#include <fstream>

#define TOMUCH 10000
int main()
{
  std::ifstream i("bug.cpp");
  std::streambuf &s=*i.rdbuf();
  char cb[TOMUCH];
  std::streamsize r=s.sgetn(cb,TOMUCH);
  std::cout << r << std::endl;
  std::cout << int(cb[r-1]) << "(" << cb[r-1] << ")"<<std::endl;
  std::cout << s.sgetc() << std::endl;
  
  // this is the problem
  // if you unget when previously get failed
  std::cout << s.sputbackc(' ') << "\n";
}

