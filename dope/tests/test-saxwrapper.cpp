#include <iostream>
#include <dope/saxwrapper.h>
#include <dope/dopeexcept.h>

#include <string>

template <typename L0>
class Parser : public ParserBase<L0>
{
public:
  Parser(L0 &_layer0) : ParserBase<L0>(_layer0)
  {}
  virtual ~Parser()
  {}
  
  void startElement(const char *name, const char **atts)
  {
    std::cerr << "Element " << name << std::endl;
    if (!atts)
      return;
    for (int i=0;atts[i]!=NULL;i+=2){
      DOPE_ASSERT(atts[i+1]!=NULL);
      std::cerr << "Attribute: " << atts[i] << " Value: \""<<atts[i+1]<<"\""<<std::endl;
    }
  }
  void endElement(const char*)
  {}
  
  void characters(const char* s, int len)
  {
    std::cerr << "Characters: \""<<std::string(s,len)<<"\""<< std::endl;
  }
};

int main()
{
  Parser<std::streambuf> parser(*std::cin.rdbuf());
  parser.parse();
}

