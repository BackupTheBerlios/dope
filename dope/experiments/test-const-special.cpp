// test const specialization

#include <iostream>

struct Foo
{
  int i;
};

template <typename X>
void foo(X &x)
{
  std::cout << "non-const\n";
  x.i=10;
}

template <typename X>
void foo(const X &x)
{
  std::cout << "const\n";
}

int main()
{
  Foo f;
  foo(f);
  foo(static_cast<Foo &>(Foo()));
}
