#include <sstream>
#include <iostream>
#include <exception>
int main(int argc,char *argv[])
{
  int i=1000;
  std::string s;
  std::stringbuf buf;
  std::istream ist(&buf);
  std::ostream ost(&buf);
  ost << s;
  if (!(ist >> i))
    std::cerr << "failed\n";
  if (ist.good()) 
    std::cerr << "good\n";
  return 0;
}
