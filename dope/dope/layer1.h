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
   \file layer1.h
   \brief Layer1 Stream Concept
   \author Jens Thiele
*/

/*! Concept of Layer1 streams 

  This isn't code to be compiled - it is just to document the concept
*/

class Layer1InStream
{
  Layer1InStream &in(bool &data);
  Layer1InStream &in(char &data);
  Layer1InStream &in(unsigned char &data);
  Layer1InStream &in(short &data);
  Layer1InStream &in(unsigned short &data);
  Layer1InStream &in(int &data);
  Layer1InStream &in(unsigned int &data);
  Layer1InStream &in(long &data);
  Layer1InStream &in(unsigned long &data);
  Layer1InStream &in(float &data);
  Layer1InStream &in(double &data);
  //! read c bytes
  Layer1InStream &in(char *data, size_t c);
};
class Layer1OutStream
{
  Layer1OutStream &out(bool data);
  Layer1OutStream &out(char data);
  Layer1OutStream &out(unsigned char data);
  Layer1OutStream &out(short data);
  Layer1OutStream &out(unsigned short data);
  Layer1OutStream &out(int data);
  Layer1OutStream &out(unsigned int data);
  Layer1OutStream &out(long data);
  Layer1OutStream &out(unsigned long data);
  Layer1OutStream &out(float data);
  Layer1OutStream &out(double data);
  //! write c bytes
  Layer1OutStream &out(const char * const data, size_t c);
};
