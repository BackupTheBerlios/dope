/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file namemanip.h
   \brief name manipulator (obsolete)
   \author Jens Thiele
*/

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
