/*
 * Copyright (C) 2003 Jens Thiele <karme@berlios.de>
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
  \file dope/l1nboinstream.h
  \brief network byte order (raw) streams
  \author Jens Thiele
*/

#ifndef L1NBOINSTREAM_H
#define L1NBOINSTREAM_H

#include "dope.h"
#include "adapter2to1.h"
#include "dopeexcept.h"
#include "bswap.h"

/*! network byte order (raw) layer1 stream */
//! simple network byte order (raw) layer 1 input stream
/*
  \note you havve to take care with the size of data types
*/
template <typename Layer0>
class L1NBOInStream
{
protected:
  Layer0 &layer0;
public:
  L1NBOInStream(Layer0 &_layer0) : layer0(_layer0)
  {}
  template <typename Data>
  DOPE_INLINE L1NBOInStream & in(Data &data)
  {
    std::streamsize s=sizeof(Data);
    if (layer0.sgetn((char *)&data,s)!=s)
      throw ReadError(std::string(__PRETTY_FUNCTION__));
#if (__BYTE_ORDER == __BIG_ENDIAN)
    data=bswap(data);
#endif
    return *this;
  }

  DOPE_INLINE L1NBOInStream & in(char *data, std::streamsize size)
  {
    if (layer0.sgetn(data,size)!=size)
      throw ReadError(std::string(__PRETTY_FUNCTION__));
    return *this;
  }
protected:
};

//! helper to get the initialization order right
template <typename L0>
struct NBOInStreamBase
{
  NBOInStreamBase(L0 &_l0) : l0(_l0), l1(l0), l2(l1)
  {}

  L0 &l0;
  L1NBOInStream<L0> l1;
  Layer2InAdapter<L1NBOInStream<L0> > l2;
};

//! layer2 raw input stream using the Layer2InAdapter
template <typename L0>
struct NBOInStream : public NBOInStreamBase<L0>, public Layer2InAdapter<L1NBOInStream<L0> >
{
  NBOInStream(L0 &_l0) : NBOInStreamBase<L0>(_l0), Layer2InAdapter<L1NBOInStream<L0> > (l2)
  {
  }
};

//! simple raw layer 1 output stream
/*!
  \note you have to take care with the size of data types
*/
template <typename Layer0>
class L1NBOOutStream
{
private:
  Layer0 &layer0;
public:
  typedef L1NBOOutStream<Layer0> type;

  L1NBOOutStream(Layer0 &_layer0) : layer0(_layer0)
  {}
  
  void flush()
  {
    layer0.pubsync();
  }
  
  template <typename Data>
  DOPE_INLINE L1NBOOutStream &out(Data data)
  {
#if (__BYTE_ORDER == __BIG_ENDIAN)
    data=bswap(data);
#endif
    layer0.sputn((char *)&data,sizeof(Data));
    return *this;
  }
  DOPE_INLINE L1NBOOutStream &out(const char * const data, size_t size)
  {
    layer0.sputn(data,size);
    return *this;
  }
};

//! helper to get the initialization order right
template <typename L0>
struct NBOOutStreamBase
{
  NBOOutStreamBase(L0 &_l0) : l0(_l0), l1(l0), l2(l1)
  {}

  L0 &l0;
  L1NBOOutStream<L0> l1;
  Layer2OutAdapter<L1NBOOutStream<L0> > l2;
};

//! layer2 raw output stream using the Layer2OutAdapter
template <typename L0>
struct NBOOutStream : public NBOOutStreamBase<L0>, public Layer2OutAdapter<L1NBOOutStream<L0> >
{
  NBOOutStream(L0 &_l0) : NBOOutStreamBase<L0>(_l0), Layer2OutAdapter<L1NBOOutStream<L0> >(l2)
  {
  }
};



#endif
