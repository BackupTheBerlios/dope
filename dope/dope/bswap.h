/* byte swap
   taken from XTL 1.4.1 (config.h)
   modified by Jens Thiele
*/
/*
 * Automatic configuration file for XTL
 *
 * Copyright (C) 2000 Asger Nielsen, alstrup@sophusmedical.dk
 * Copyright (C) 2000 Angus Leeming, a.leeming@ic.ac.uk
 * Copyright (C) 2000 Jose' Orlando Pereira, jop@di.uminho.pt
 *
 * XTL - eXternalization Template Library - http://gsd.di.uminho.pt/~jop/xtl
 * Copyright (C) 1998-2000 Jose' Orlando Pereira, Universidade do Minho
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 * $Id: bswap.h,v 1.4 2003/05/09 15:38:13 karme Exp $
 */

#ifndef DOPE_BSWAP_H
#define DOPE_BSWAP_H

typedef long long longlong;
typedef unsigned long long unsignedlonglong;

// Don't use byteswap.h on Alpha machines because its buggy
#if defined(__linux__) && !(defined(__alpha) || defined(__alpha__))
#	include <byteswap.h>
#else
inline unsigned short bswap_16(unsigned short b) {
  return ((b&0xff00)>>8)|((b&0x00ff)<<8);
}

inline unsigned int bswap_32(unsigned int b) {
	return ((b&0xff000000)>>24)|((b&0x00ff0000)>>8)|
		((b&0x0000ff00)<<8)|((b&0x000000ff)<<24);
}

inline unsignedlonglong bswap_64(unsignedlonglong b) { 
	return ( ((b&0xff00000000000000LL) >> 56) |
		 ((b&0x00ff000000000000LL) >> 40) |
		 ((b&0x0000ff0000000000LL) >> 24) |
		 ((b&0x000000ff00000000LL) >> 8) |
		 ((b&0x00000000ff000000LL) << 8) |
		 ((b&0x0000000000ff0000LL) << 24) |
		 ((b&0x000000000000ff00LL) << 40) |
		 ((b&0x00000000000000ffLL) << 56) );
}
#endif

#define _DOPE_BSWAP(T,S) inline T& bswap(T &x){x=bswap_##S(x);return x;}
_DOPE_BSWAP(short,16)
_DOPE_BSWAP(unsigned short,16)
_DOPE_BSWAP(int,32)
_DOPE_BSWAP(unsigned int,32)
_DOPE_BSWAP(longlong,64)
_DOPE_BSWAP(unsignedlonglong,64)
#undef _DOPE_BSWAP
#endif
