/*
  Hi

  What's that all about

  Yet another pickling framework

  Concepts:

  C++/OOP inherent: class, object

  Inspector: 
  helps inspecting a class, describes a class and operations that
  should happen on inspection - where you can distinguish between non-modifying
  operations and modifying operations
  Examples: see below

  Processor:
  takes an object and processes the object with the help of an inspector
  Examples: 
  streams/pickling, meta-object factory, interpreters, command line parsing,...
*/

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

/* prerequisites */
struct True
{};
struct False
{};
struct InspectorBase
{
  typedef True Defined;
};
template <typename T>
struct Inspector
{
  typedef False Defined;
};
typedef const char* const MemberName;



/*! 
  example processor:
  writing the object to stdout - working on a const object
*/
struct DebugStream
{
  template <typename I, typename X>
  DebugStream &operator()(I &i, const X& x, MemberName n)
  {
    return process(i,x,n,I::Defined);
  }

  //! no inspector passed => work with default inspector
  template <typename X>
  DebugStream &operator()(const X& x, MemberName n=NULL)
  {
    Inspector<X> i;
    return this->operator()(i,x,n);
  }
protected:
  //! inspector is defined
  template <typename I, typename X>
  DebugStream &process(I &i, const X& x, MemberName n, True) 
  {
    cout << i.getTypeName() << " " << (n ? n : "?") << " = {"<<endl;
    i(*this,x);
    cout << "}" << endl;
    return *this;
  }
  //! inspector is not defined => assume it is a type which is handled by cout
  template <typename I, typename X>
  DebugStream &process(I &i, const X& x, MemberName n, False)
  {
    cout << (n ? n : "?") << " = " << x << endl;
    return *this;
  }
};

/*! 
  example processor:
  modifying the object with data from an input stream 
*/
struct InputStream
{
  //! operate using passed inspector
  template <typename X, typename I>
  InputStream &operator()(I &i,X& x, MemberName n)
  {
    return process(i,x,n,I::Defined);
  }

  //! no inspector passed => work with default inspector
  template <typename X>
  InputStream &operator()(X& x, MemberName n)
  {
    Inspector<X> i;
    return this->operator()(i,x,n);
  }

protected:
  //! inspector defined
  template <typename X, typename I>
  InputStream &process(I &i,X& x, MemberName n, True)
  {
    return i(*this,x);
  }

  //! inspector not defined
  template <typename X, typename I>
  InputStream &process(I &i,X& x, MemberName n, False)
  {
    cin >> x;
    return *this;
  }
};



/* example class */

/* hand written

//! example struct/class to pickle or inspect
struct Foo
{
  Foo() : i(10), j(23){}
  
  int i;
  int j;
  string s;
  float f;
  };

//! example default inspector for this class
struct Inspector<Foo> : public InspectorBase
{
  const char* const getTypeName()
  {
    // return the type name
    return "Foo";
  }
  
  //! \note X is a template to work on const as well as non-const
  template <typename Layer2, typename X>
  Layer2 &operator()(Layer2 &l2, X &x)
  {
    // you list all your members here
    return l2(x.i,"i")(x.j,"j")(x.s,"s")(x.f,"f");
  }
};

*/


/* using preprocessor */

#define M(t,m) t m;
#define STRUCTR(name,members) \
struct name {members }

#define INSPECTOR(name,members) \
struct Inspector<name> : public InspectorBase { \
const  char * const getTypeName() { return #name; } \
template <typename Layer2, typename X> \
Layer2 &operator()(Layer2 &l2, X &x) { \
return l2 members ; } };

#define STRUCT(name,members) \
#define M(t,m) t m; \
STRUCTR(t,m) \
#undef M \
#define M(t,m) (x.m,#m) \
INSPECTOR(t,m) \
#undef M

#undef M
STRUCT(Foo,
       M(int,i)
       M(int,j)
       M(string,s)
       M(float,f)
);

/*

#undef M
#define M(t,m) (x.m,#m)



INSPECTOR(Foo, \
       M(int,i) \
       M(int,j) \
       M(string,s) \
       M(float,f)
);
*/


/*! 
  example special inspector for this class - only inspects a part of
  the Foo class - you can think of this as a different view 
*/
struct FooInspector : public InspectorBase
{
  const char* const getTypeName()
  {
    return "PartOfFoo";
  }

  //! \note X is a template to work on const as well as non-const
  template <typename Layer2, typename X>
  Layer2 &operator()(Layer2 &l2, X &x)
  {
    return l2(x.i,"i");
  }
};

/*! 
  another example of a special inspector for this class

  seperate "read"/"write" or get/set or inspect/process
  you should avoid doing stuff in inspection to give the
  processors the most felxibility
  
  f.e. you would perhaps assume a read/write process does
  sequential inspection - but perhaps this is not case
  with all processors (f.e. a processor might stop before
  all members are inspected and later start from the beginning)
*/
struct FooInspector2 : public InspectorBase
{
  const char* const getTypeName()
  {
    return "ji2Foo";
  }

  template <typename Layer2>
  Layer2 &operator()(Layer2 &l2, const Foo &x)
  {
    cerr << "Simple write\n";
    return l2(x.i,"i")(x.j,"j");
  }

  template <typename Layer2>
  Layer2 &operator()(Layer2 &l2, Foo &x)
  {
    l2(x.i,"i");
    x.j=x.i*x.i;
    cerr << "On read did something\n";
    return l2;
  }
};


int main(){
  const Foo foo;
  DebugStream os;
  FooInspector i;
  os(i,foo,"foo");
  InputStream is;
  Foo foo2;
  is(i,foo2,"foo2");
  os(i,foo2,"foo2");
  is(foo2,"foo2");
  os(foo2,"foo2");
  FooInspector2 i2;
  is(i2,foo2,"foo2");
  os(i2,foo2,"foo2");
}
