#include <iostream>

namespace Streams 
{
  struct Stream
  {
    template <typename X>
    void foo(X x)
    {
      std::cout << x << std::endl;
    }
  };
}

template <typename X, typename Y>
typename Streams::X &operator|(typename Streams::X &x, Y &y)
{
  x.foo(y);
  return x;
}

int main()
{
  int x=10;
  Streams::Stream s;
  s|x;
#if 0
  x|float(10)
;
#endif
}
