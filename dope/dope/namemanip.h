#ifndef DOPE_NAMEMANIP_H
#define DOPE_NAMEMANIP_H

todo remove this when i implemented another manipulator/extension

#include "streamtraits.h"
#include "manip.h"

//! member name stream manipulator
class MName : public StreamManipulator
{
public:
  MName(const char *mname=NULL) : m_mname(mname)
  {
  }
  const char *m_mname;
  template <typename Layer2>
  void composite(Layer2 &layer2,True) const
  {
    layer2.name(m_mname);
  }
  template <typename Layer2>
  void composite(Layer2 &layer2,False) const
  {}
};

/*
template <typename Layer2>
inline void composite(Layer2 &layer2, MName &m)
{
  typedef typename Layer2::Traits::nameExtension nameExt;
  m.composite(layer2,nameExt());
  }*/

template <typename Layer2>
inline void composite(Layer2 &layer2, const MName &m)
{
  typedef typename Layer2::Traits::nameExtension nameExt;
  m.composite(layer2,nameExt());
  //  static_cast<MName &>(m).composite(layer2,nameExt());
}

#endif
