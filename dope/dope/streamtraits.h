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
   \file streamtraits.h
   \brief stream traits
   \author Jens Thiele
*/

#ifndef DOPE_STREAMTRAITS_H
#define DOPE_STREAMTRAITS_H

//! true value for template based evaluation
struct True
{};
//! false value for template based evaluation
struct False
{};

//! traits of a layer
template <int X>
struct LayerTrait
{
  //! layer number (0|1|2)
  static const int layer=X;
};

//! no extensions
struct NoExtension
{
  //! stream does not support comments
  typedef False commentExtension;
};

//! traits of an input stream
/*!
  \todo i think i will remove the concept of input/output
  completely since i decided to drop const output streams
  but on the other hand it might be useful to know that
  this stream is at least designed to be const
*/
struct InStreamTraits
{
  typedef True InStream;
  typedef False OutStream;
};
//! traits of an output stream
struct OutStreamTraits
{
  typedef False InStream;
  typedef True OutStream;
};

//! traits of an layer 2 stream without any extensions
struct Layer2StreamTraits : public LayerTrait<2>, public NoExtension
{
};

//! traits of an layer 2 input stream without any extension
struct Layer2InStreamTraits : public Layer2StreamTraits, public InStreamTraits
{
};

//! traits of an layer 2 output stream without any extension
struct Layer2OutStreamTraits : public Layer2StreamTraits, public OutStreamTraits
{
};

#endif
