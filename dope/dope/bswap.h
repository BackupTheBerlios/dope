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
 * $Id: bswap.h,v 1.1 2003/04/29 18:11:14 karme Exp $
 */

#ifndef DOPE_BSWAP_H
#define DOPE_BSWAP_H

#if defined(__GNUC__) 
#	if __GNUC__ >2 || ( __GNUC__ == 2 && __GNUC_MINOR__ >=95 ) // >= G++ 2.95
#		define XTL_CONFIG_SIMPLE_CONTAINERS
#	endif

	typedef long long longlong;
	typedef unsigned long long unsignedlonglong;

#elif defined (__DECCXX)
#      	define XTL_CONFIG_SIMPLE_CONTAINERS

	typedef long long int longlong;
	typedef unsigned long long int unsignedlonglong;

#elif defined (__sgi)
#      	define XTL_CONFIG_SIMPLE_CONTAINERS

	typedef long long int longlong;
	typedef unsigned long long int unsignedlonglong;

#elif defined (_WIN32)

#	define XTL_CONFIG_COMPOSITE_BUG
#	define XTL_CONFIG_LACK_ENDIAN_H

	typedef __int64  longlong;
	typedef unsigned __int64  unsignedlonglong;

	// Disable some stupid warnings
#	pragma warning(disable:4127 4786 4800)

	// Work around for broken for-scoping with Visual C++ 6.0
#	define for if(0);else for

	// Enable a bunch of useful warnings
#	pragma warning(3:4019 4032 4057 4061 4125 4130 4152 4189 4201 4706)

#else
#if HAVE_LONG_LONG
	typedef long long longlong;
	typedef unsigned long long unsignedlonglong;
#endif
#endif

// Establish __BYTE_ORDER
#ifdef __linux__
#	include <endian.h>
#else
#	define __LITTLE_ENDIAN 1234
#	define __BIG_ENDIAN    4321
#endif

#if defined(__alpha) || defined(__alpha__)
#	ifndef __BYTE_ORDER
#		define __BYTE_ORDER __LITTLE_ENDIAN
#	endif
                                            
#elif defined __sgi
#	ifndef __BYTE_ORDER
#		define __BYTE_ORDER __BIG_ENDIAN
#	endif
#endif

#if ((__BYTE_ORDER != __LITTLE_ENDIAN) && (__BYTE_ORDER != __BIG_ENDIAN))
#	error "Architecture not supported."
#endif

// Don't use byteswap.h on Alpha machines because its buggy
#if defined(__linux__) && !(defined(__alpha) || defined(__alpha__))
#	include <byteswap.h>
#else
inline unsigned short bswap_16(unsigned short b) {
	return ((b&0xff00)>>8)|((b&0x00ff)>>8);
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

#define _DOPE_BSWAP(T,S) inline T bswap(T x){return bswap_##S(x);}
_DOPE_BSWAP(short,16)
_DOPE_BSWAP(unsigned short,16)
_DOPE_BSWAP(int,32)
_DOPE_BSWAP(unsigned int,32)
_DOPE_BSWAP(longlong,64)
_DOPE_BSWAP(unsignedlonglong,64)
#undef _DOPE_BSWAP

#endif