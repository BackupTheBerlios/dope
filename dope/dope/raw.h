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
   \file raw.h
   \brief raw layer1 streams
   \author Jens Thiele
*/

#ifndef DOPE_RAWINSTREAM_H
#define DOPE_RAWINSTREAM_H

/*! raw layer1 stream */

#include "dope.h"
#include "adapter2to1.h"
#include "dopeexcept.h"

//! simple raw layer 1 input stream
/*!
  \note this protocol is platform dependant (byte-order)
 */
template <typename Layer0>
class L1RawInStream
{
private:
  Layer0 &layer0;
public:
  L1RawInStream(Layer0 &_layer0) : layer0(_layer0)
  {}
  
  template <typename Data>
  DOPE_INLINE L1RawInStream & in(Data &data)
  {
    std::streamsize s=sizeof(Data);
    if (layer0.sgetn((char *)&data,s)!=s)
      throw ReadError(std::string(__PRETTY_FUNCTION__));
    return *this;
  }
  DOPE_INLINE L1RawInStream & in(char *data, std::streamsize size)
  {
    if (layer0.sgetn(data,size)!=size)
      throw ReadError(std::string(__PRETTY_FUNCTION__));
    return *this;
  }
};

//! helper to get the initialization order right
template <typename L0>
struct RawInStreamBase
{
  RawInStreamBase(L0 &_l0) : l0(_l0), l1(l0), l2(l1)
  {}

  L0 &l0;
  L1RawInStream<L0> l1;
  Layer2InAdapter<L1RawInStream<L0> > l2;
};

//! layer2 raw input stream using the Layer2InAdapter
template <typename L0>
struct RawInStream : public RawInStreamBase<L0>, public Layer2InAdapter<L1RawInStream<L0> >
{
  RawInStream(L0 &_l0) : RawInStreamBase<L0>(_l0), Layer2InAdapter<L1RawInStream<L0> > (l2)
  {
  }
};

//! simple raw layer 1 output stream
/*!
  \note this protocol is platform dependant
 */
template <typename Layer0>
class L1RawOutStream
{
private:
  Layer0 &layer0;
public:
  typedef L1RawOutStream<Layer0> type;

  L1RawOutStream(Layer0 &_layer0) : layer0(_layer0)
  {}
  
  void flush()
  {
    layer0.pubsync();
  }
  
  template <typename Data>
  DOPE_INLINE L1RawOutStream &out(Data data)
  {
    layer0.sputn((char *)&data,sizeof(Data));
    return *this;
  }
  DOPE_INLINE L1RawOutStream &out(const char * const data, size_t size)
  {
    layer0.sputn(data,size);
    return *this;
  }
};

//! helper to get the initialization order right
template <typename L0>
struct RawOutStreamBase
{
  RawOutStreamBase(L0 &_l0) : l0(_l0), l1(l0), l2(l1)
  {}

  L0 &l0;
  L1RawOutStream<L0> l1;
  Layer2OutAdapter<L1RawOutStream<L0> > l2;
};

//! layer2 raw output stream using the Layer2OutAdapter
template <typename L0>
struct RawOutStream : public RawOutStreamBase<L0>, public Layer2OutAdapter<L1RawOutStream<L0> >
{
  RawOutStream(L0 &_l0) : RawOutStreamBase<L0>(_l0), Layer2OutAdapter<L1RawOutStream<L0> >(l2)
  {
  }
};



#endif
