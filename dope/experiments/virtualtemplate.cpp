/*
das kann nicht gehen da nicht klar ist wieviele virtuelle methoden und damit einträge
in die virtual table müssen. bzw. es wäre sehr schwierig zu realisieren
man koennte die virtual table als hash table realisieren die die paramter hashed
was aber dann zur laufzeit fehler geben kann wenn nicht alle methoden compiliert wurden die
benutzt werden was ja das eigentliche problem ist


wie kann man prinzipiell das gleiche erreichen ?
*/

#include <iostream>

class base
{
public:
  template <typename X>
  virtual void foo(X &x)
  {
    std::cout << x << std::endl;
  }
};

class derived : public base
{
public:
  template <typename X>
  virtual void foo(X &x)
  {
    std::cout << "derived: " << x << std::endl;
  }
};


class Base
{
public:
  template <typename X>
  void foo(X &x);
}

template <typename X>
class Wrapper
{
public:
  Wrapper(X &_x) : x(_x)
  {
  }
  X& x;

  template <typename Y>
  void foo(Y &Y)
  {
    x.foo(y);
  }
};


int main()
{
}
