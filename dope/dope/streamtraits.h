#ifndef DOPE_STREAMTRAITS_H
#define DOPE_STREAMTRAITS_H

struct True
{};
struct False
{};

template <int X>
struct LayerTrait
{
  static const int layer=X;
};

struct NoExtension
{
  typedef False nameExtension;
  typedef False typeExtension;
  typedef False fooExtension;
};

struct InStreamTraits
{
  typedef True InStream;
  typedef False OutStream;
};

struct OutStreamTraits
{
  typedef False InStream;
  typedef True OutStream;
};

struct Layer2StreamTraits : public LayerTrait<2>, public NoExtension
{
};

struct Layer2InStreamTraits : public Layer2StreamTraits, public InStreamTraits
{
};

struct Layer2OutStreamTraits : public Layer2StreamTraits, public OutStreamTraits
{
};

#endif
