#include <iostream>

namespace foo 
{
  template <typename X, typename Y>
  X &operator|(X &x, Y &y)
  {
    x.foo(y);
    return x;
  }
}

struct Stream
{
  template <typename X>
  void foo(X x)
  {
    std::cout << x << std::endl;
  }
};

int main()
{
  int x=10;
  Stream s;
  {
    using namespace foo;
    s|x;
  }
#if 0
  s|x;
#endif
}
