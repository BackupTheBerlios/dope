/*
 * Copyright (C) 2001-2002 Jens Thiele <karme@unforgettable.com>
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
 *  
 */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "typenames.h"

//! class to wrap time functions
/*
  thin wrapper class around gettimeofday or similar functions
*/
class TimeStamp
{
public:
  // default constructor - time is set to 0
  TimeStamp();
  //! set time
  TimeStamp(int sec, int usec);
  //! set from seconds
  TimeStamp(float sec);
  //! copy other stamp
  TimeStamp(const TimeStamp &o);

  ~TimeStamp();
  
    
  //! set to current time
  void now();
  
  //! difference between 2 times
  /*!
    \attention The o time must be smaller then this time (*this>=o)
    \todo think about it
  */
  TimeStamp operator-(const TimeStamp &o) const;
  TimeStamp &operator-=(const TimeStamp &o);

  TimeStamp operator+(const TimeStamp &o) const;
  TimeStamp &operator+=(const TimeStamp &o);
  
  //! sleep
  void sleep() const;

  bool operator<(const TimeStamp &o) const
  {
    return m_stamp<o.m_stamp;
  }
  bool operator==(const TimeStamp &o) const
  {
    return m_stamp==o.m_stamp;
  }
  bool operator>(const TimeStamp &o) const
  {
    return m_stamp>o.m_stamp;
  }
  
  template <typename Layer2>
  inline void composite(Layer2 &layer2)
  {
    layer2.simple(m_stamp,"stamp");
  }

  //! get seconds - suitable to fill a signed timeval struct
  int getSec() const 
  {
    // todo check if it fits
    return m_stamp/1000000LL;
  }
  //! get micro-seconds - suitable to fill a signed timeval struct
  int getUSec() const
  {
    return m_stamp%1000000LL;
  }
protected:
  void init();
  void deinit();
  
  //! the stamp
  /*!
    \todo use stdint.h and int64_t conforming to C99
  */
  long long m_stamp;
};
DOPE_CLASS(TimeStamp);

template <typename Layer2>
inline void composite(Layer2 &layer2, TimeStamp &s)
{
  s.composite(layer2);
}


#endif
