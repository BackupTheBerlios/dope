// test template specialization in the context of derived classes
#include <iostream>
struct Base
{
};

struct Foo : public Base
{
};

struct Bar : public Base
{
};

struct MBase
{
};

struct Misc : public MBase
{
};


void foo(Base *base)
{
  std::cout << "Base ptr Specialization\n";
}


template <typename X>
void foo(X &x)
{
  std::cout << "No Specialization\n";
}

template <typename X>
void foo(X *x)
{
  std::cout << "Ptr Specialization\n";
}

void foo(Base &base)
{
  std::cout << "Base Specialization\n";
}

void foo(Foo &foo)
{
  std::cout << "foo Specialization\n";
}

template <typename X>
void wrap_foo(X x)
{
}

int main()
{
  Foo f;
  foo(f);
  Bar bar;
  foo(bar);
  foo(&bar);
  wrap_foo(typeid(bar),bar);
  Misc m;
  wrap_foo(typeid(m),m);
}
