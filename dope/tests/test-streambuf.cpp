// test program for a working std::streambuf
// and example how to wrap a std::streambuf (to write a own streambuf)

#include <iostream>
#include <exception>
#include <streambuf>
#include <sstream>

template <typename _S>
class Wrapper
  : public std::basic_streambuf<typename _S::char_type, typename _S::traits_type>
{
protected:
  typedef _S S;
  S &s;
public:
  typedef typename S::char_type char_type;
  typedef typename S::traits_type traits_type;
  typedef typename S::int_type int_type;

  Wrapper(S &_s) : s(_s)
  {}
  
  int sync() 
  {
    return s.pubsync();
  }
  
  std::streamsize showmanyc()
  {
    // we do not have any buffer => return how many characters are avvailable in our underlying stream
    return s.in_avail();
  }

  int_type underflow()
  {
    return s.sbumpc();
  }

  //! it seems we have to overwrite uflow if we don't use a buffer
  int_type uflow()
  {
    return underflow();
  }
  
  int_type pbackfail(int_type c = traits_type::eof())
  {
    return s.sputbackc(c);
  }
  
  int_type overflow(int_type c = traits_type::eof())
  {
    return s.sputc(c);
  }
};

bool test(std::streambuf &buf) 
{
  std::istream in(&buf);
  std::ostream out(&buf);

  // first test output
  std::string teststring("Hello World!");
  out << teststring << std::endl;
  std::string line;
  if (!getline(in,line)) {
    //    std::cout << "getline failed\n";
    return false;
  }
  //  std::cout << line << std::endl;
  if (line!=teststring) {
    //    std::cout << "Error: "<< line << "!=" << teststring << std::endl;
    return false;
  }
  line+="\n";
  for (int i=line.size();i;) {
    std::streambuf::int_type r(buf.sputbackc(line[--i]));
    if (r==std::streambuf::traits_type::eof()) {
      //      std::cout << "Error: Putback failed" << std::endl;
      return false;
    }else{
      //      std::cout << __FILE__ << ":" << __LINE__ << ": putback: "<<r<<std::endl;
    }
  }
  line.clear();
  if (!getline(in,line)) {
    //    std::cout << "getline2 failed\n";
    return false;
  }
  if (line!=teststring) {
    //    std::cout << "Error2: "<< line << "!=" << teststring << std::endl;
    return false;
  }
  return true;
}


int main(int argc,char *argv[])
{
  try{
    int r=0;
    {
      // test that stringbuf works as expected
      std::stringbuf buf;
      r=test(buf);
      //      std::cout.flush();
    }
    {
      // test that our wrapper works as expected
      std::stringbuf buf;
      Wrapper<std::stringbuf> wbuf(buf);
      r=test(wbuf);
      //      std::cout.flush();
    }
    //    std::cout << r << std::endl;
    return !r;
  }
  catch (const std::exception &error){
    std::cerr << "Uncaught std::exception: "<<error.what()<<std::endl;
  }
  return 1;
}
