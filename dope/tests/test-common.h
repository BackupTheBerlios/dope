#include <iostream>
#include <exception>
#include <sstream>
#include <fstream>

// use stl types
#include <dope/dopestl.h>
// DOPE_DEF_SIMPLETYPE macro
#include <dope/typenames.h>
// anyToString
#include <dope/utils.h>

#define SIMPLE(d) simple(d,#d)

class foo;
DOPE_DEF_SIMPLETYPE(foo)
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
  void setSome()
  {
    i=7;
    b=false;
    c='A';
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

  void litter()
  {
    i=-1;
    b=true;
    c='L';
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

  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.SIMPLE(i).SIMPLE(b).SIMPLE(c).dynCVector(dynCVec1,dynCVec1Size,"dynCVec1")
      .dynCVector(dynCVec2,dynCVec2Size,"dynCVec2").fixedCVector(fixedCVector,2,"fixedCVector")
      .SIMPLE(cppstr).SIMPLE(il).SIMPLE(p).SIMPLE(cppcv).SIMPLE(m);
  }

  int i;
  bool b;
  char c;
  size_t dynCVec1Size;
  int* dynCVec1;
  size_t dynCVec2Size;
  std::pair<std::string, int>* dynCVec2;
  int fixedCVector[2];
  std::string cppstr;
  std::list<std::vector<int> > il;
  std::pair<char,bool> p;
  std::vector<char> cppcv;
  std::map<std::string,int> m;
};

template <typename Layer2>
inline void composite(Layer2 &layer2, foo &f)
{
  f.composite(layer2);
}

/*
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
	  std::cout << buf.str() << "\n----\n" << buf2.str() << std::flush;
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
