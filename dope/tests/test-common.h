#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>

// use stl types
#include <dope/dopestl.h>
// DOPE_CLASS macro
#include <dope/typenames.h>
// anyToString
#include <dope/utils.h>

//! macro to prevent typing the membername twice
#define SIMPLE(d) simple(d,#d)

//! sample class to pickle
/*!
  this class should test all features
*/
class foo
{
private:
  foo(const foo&);
  void operator=(const foo&);
  
public:
  foo() : dynCVec1(NULL), dynCVec2(NULL)
  {
  }
  
#define VECSIZE 100
  //! set some values
  void setSome()
  {
    i=7;
    b=false;
    c='A';
    sc=-10;
    uc=200;
    cppstr="hello world !&<\">;:-";
    p.first='Z';
    p.second=true;
    dynCVec1Size=VECSIZE;
    dynCVec1=new int[dynCVec1Size];
    for (size_t j=0;j<dynCVec1Size;++j)
      dynCVec1[j]=j;
    dynCVec2Size=VECSIZE;
    dynCVec2=new std::pair<std::string, int>[dynCVec2Size];
    for (size_t j=0;j<dynCVec2Size;++j)
      dynCVec2[j]=std::pair<std::string, int>(anyToString(j)+" in a string",j);
    fixedCVector[0]=255;
    fixedCVector[1]=255<<8;
    std::vector<int> tmp(VECSIZE);
    std::vector<int> tmp2(VECSIZE);
    for (size_t j=0;j<tmp.size();++j)
      {
	tmp[j]=j;
	tmp2[j]=VECSIZE-j;
      }
    il.push_back(tmp);
    il.push_back(std::vector<int>(0));
    il.push_back(tmp2);
    cppcv.push_back('X');
    m["hallo"]=0xAAAAAAAA;
    m["supi"]=0x89ABCDEF;
  }
#undef VECSIZE

  //! destroy member values (set other values)
  void litter()
  {
    i=-1;
    b=true;
    c='L';
    uc=10;
    sc=12;
    p.first='L';
    fixedCVector[0]=666;
    fixedCVector[1]=666;
    dynCVec1Size=0;
    if (dynCVec1) {
      delete [] dynCVec1;
      dynCVec1=NULL;
    }
    dynCVec2Size=0;
    if (dynCVec2) {
      delete [] dynCVec2;
      dynCVec2=NULL;
    }
  }
  
  ~foo()
  {
    if (dynCVec1)
      delete [] dynCVec1;
    if (dynCVec2)
      delete [] dynCVec2;
  }

  //! this method lists all memberes that should be pickled
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(i).SIMPLE(b).SIMPLE(c).SIMPLE(sc).SIMPLE(uc)
      .dynCVector(dynCVec1,dynCVec1Size,"dynCVec1")
      .dynCVector(dynCVec2,dynCVec2Size,"dynCVec2")
      .fixedCVector(fixedCVector,2,"fixedCVector")
      .SIMPLE(cppstr).SIMPLE(il).SIMPLE(p).SIMPLE(cppcv).SIMPLE(m);
  }

  // some basic types
  int i;
  bool b;
  char c;
  signed char sc;
  unsigned char uc;

  // dynamic c vectors - created with new[] and destroyed with delete[]
  size_t dynCVec1Size;
  int* dynCVec1;
  size_t dynCVec2Size;
  std::pair<std::string, int>* dynCVec2;

  //! fixed sized c vector
  int fixedCVector[2];

  // STL classes and containers
  std::string cppstr;
  std::list<std::vector<int> > il;
  std::pair<char,bool> p;
  std::vector<char> cppcv;
  std::map<std::string,int> m;
};
//! register this type / its name
DOPE_CLASS(foo);

//! the pickle method in the global scope
/*! 
  in this case we simply call our member function
*/
template <typename Layer2>
inline void composite(Layer2 &layer2, foo &f)
{
  f.composite(layer2);
}

/*
this was a hack for const output streams

template <typename Layer2>
inline void composite(Layer2 &layer2, const foo &f)
{
  static_cast<foo &>(f).composite(layer2);
}
*/

//! generic test class
/*!
  - initialize foo 
  - serialize it (and save in buf)
  - fill foo with litter 
  - deserialize it
  - serialize it (and save in buf2)
  - compare buf1 and buf2
*/
template <typename InStream, typename OutStream>
class Test
{
public:
  int main(int argc,char **argv)
  {
    if ((argc==2)&&((std::string(argv[1])=="-h")||(std::string(argv[1])=="--help")))
      {
	std::cout << "Usage: " << argv[0] << " [-v]\nThis is a test program. It returns 0 on success otherwise false.\nWith -v the stream is dumped to stdout - Warning: Some tests write binary data\n";
	return 1;
      }
    bool dump = ((argc==2)&&(std::string(argv[1])=="-v"));
    // we write to memory instead of a file
    std::stringbuf buf;
    std::stringbuf buf2;
    try{
      {
	foo f;
	f.setSome();
	OutStream o(buf);
	o.SIMPLE(f);
	if (dump)
	  std::cout << buf.str() << "\n";
      }
      {
	// read foo
	foo f;
	f.litter();
	InStream x(buf);
	x.SIMPLE(f);
	// write again
	OutStream o(buf2);
	o.SIMPLE(f);
	// compare both external representations
	// \todo compare both internal representations too
	if (dump)
	  std::cout << "----\n" << buf2.str() << std::flush;
	if (buf.str()==buf2.str())
	  return 0;
      }
    }
    catch (const std::exception &error){
      std::cerr << "Uncaught std::exception: "<<error.what()<<std::endl;
    }
    return 1;
  }
};
