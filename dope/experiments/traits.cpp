#include <iostream>

struct RawOutStreamTraits : public Layer2OutStreamTraits
{
  typedef True typeExtension;
};

struct RawOutStream
{
  typedef RawOutStreamTraits Traits;
};

struct XMLInStreamTraits : public Layer2InStreamTraits
{
  typedef True typeExtension;
  typedef True nameExtension;
};

struct XMLInStream
{
  typedef XMLInStreamTraits Traits;
};

std::ostream &operator<<(std::ostream &o, True)
{
  o << "True";
  return o;
}

std::ostream &operator<<(std::ostream &o, False)
{
  o << "False";
  return o;
}

#define PRINT(d) std::cout << #d ": "<< typename Layer2::Traits::d() <<"\n"

template <typename Layer2>
struct Info
{
  static void print() 
  {
    typedef typename Layer2::Traits L;
    int layer=L::layer;
    std::cout << "Layer: "<< layer << "\n";
    PRINT(InStream);
    PRINT(OutStream);
    PRINT(nameExtension);
    PRINT(typeExtension);
    PRINT(fooExtension);
  }
};

int main()
{
  Info<RawOutStream>::print();
  Info<XMLInStream>::print();
}
