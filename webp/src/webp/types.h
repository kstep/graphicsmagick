// Copyright 2010 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
//  Common types
//
// Author: Skal (pascal.massimino@gmail.com)

#ifndef WEBP_WEBP_TYPES_H_
#define WEBP_WEBP_TYPES_H_

#include <stddef.h>  // for size_t

#ifndef _MSC_VER
#include <inttypes.h>
#ifdef __STRICT_ANSI__
#define WEBP_INLINE
#else  /* __STRICT_ANSI__ */
#define WEBP_INLINE inline
#endif
#else
typedef signed   char int8_t;
typedef unsigned char uint8_t;
typedef signed   short int16_t;
typedef unsigned short uint16_t;
typedef signed   int int32_t;
typedef unsigned int uint32_t;
#if defined(_MSC_VER) && (_MSC_VER<=1200)
  typedef unsigned __int64 uint64_t;
  typedef __int64 int64_t;
#else
  typedef unsigned long long int uint64_t;
  typedef long long int int64_t;
#endif

#define WEBP_INLINE __forceinline
#endif  /* _MSC_VER */

/* This part added for GraphicsMagick */
#if defined(_MT) && defined(_DLL) && !defined(_WEBPDLL_) && !defined(_LIB)
#   define _WEBPDLL_
#endif
#if defined(_WEBPDLL_)
#   if defined(_VISUALC_)
#       pragma warning( disable : 4273 )
#   endif
#   if !defined(_WEBPLIB_)
#       define WEBP_EXTERN(type) extern __declspec(dllimport) type
#       pragma message( "libwebp compiling as DLL import" )
#   else
#       define WEBP_EXTERN(type) extern __declspec(dllexport) type
#       pragma message( "libwebp compiling as DLL export" )
#   endif
#endif
/* End of GraphicsMagick part. */


#ifndef WEBP_EXTERN
// This explicitly marks library functions and allows for changing the
// signature for e.g., Windows DLL builds.
#define WEBP_EXTERN(type) extern type
#endif  /* WEBP_EXTERN */

// Macro to check ABI compatibility (same major revision number)
#define WEBP_ABI_IS_INCOMPATIBLE(a, b) (((a) >> 8) != ((b) >> 8))

#endif  /* WEBP_WEBP_TYPES_H_ */
