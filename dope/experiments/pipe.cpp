#include <iostream>

/*
template <typename L2>
struct dopestream
{
  typedef L2 Layer2;
};

template <typename L2, typename X>
dopestream<L2> &operator|(dopestream<L2> &s, X &x)
{
  L2 *sptr=dynamic_cast<L2 *>(&s);
  assert(sptr);
  sptr->foo(x);
}
*/

typedef const char * MemberName;


class foostream 
{
public:
  template <typename X>
  void foo(X &x, MemberName n)
  {
    std::cout << n << "=" << x << std::endl;
    x+=1;
  }
};

template <typename L2>
struct Pipe
{
  typedef L2 Layer2;
  
  Pipe(L2 &_l2) : l2(_l2)
  {}
  
  L2 &l2;
  template <typename X>
  void foo(X &x, MemberName n)
  {
    l2.foo(x,n);
  }
};

template <typename X>
struct Simple
{
  X &x;
  MemberName mname;

  Simple(X &_x, MemberName _mname) : x(_x), mname(_mname)
  {
  }
};

template <typename L2, typename X>
Pipe<L2> &operator|(Pipe<L2> &p, const Simple<X> &s)
{
  p.foo(s.x,s.mname);
  return p;
}

template <typename X>
Simple<X> simple(X &x, MemberName mname)
{
  return Simple<X>(x,mname);
}

#define SIMPLE(x) simple(x,#x)
int main()
{
  foostream f;
  Pipe<foostream> pipe(f);
  int i=10;
  pipe|SIMPLE(i)|SIMPLE(i)|SIMPLE(i)|SIMPLE(i)|SIMPLE(i)|SIMPLE(i);
  return 0;
}
