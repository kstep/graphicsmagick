//  ------------------------------------------------------------------------------------------------
//  MODULE      : FITypes
//  LANGAGE     : C and C++
//  CREATEUR    : Laurent ALBERT
//  DATE        : Mardi 19 Octobre 1993
//  DESCRIPTION : Define LivePicture Inc's basic types.
//  COMMENT   : Mustn't include ApplCommun.h
//          LS 04/02/96: This file isn't supposed to be included everywhere anymore
//                 Extract FIDefines.h, MacCompatibility.h, FIErrors.h and SwapBytes.h
//  SCCSID      : @(#)b_types.h 1.1 11:49:56 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef FITypes_h
#define FITypes_h
//  ------------------------------------------------------------------------------------------------


//  Includes
//  --------

#ifndef FIDefines_h
    #include "p_define.h"
#endif

// Defines common types
#include <stddef.h>

// Defines common Mac types
#ifdef macintosh
  #include <Types.h>
#endif

//  Types and constants
//  -------------------

// Portable integer types
#ifdef macintosh                                                // If MacIntosh
    typedef signed long  int32;
    typedef signed short int16;
    typedef signed char  int8;

    typedef unsigned long  unsigned32;
    typedef unsigned short unsigned16;
    typedef unsigned char  unsigned8;
#else
    #ifdef __alpha                                              // If DEC ALPHA
        typedef signed int   int32;
        typedef signed short int16;
        typedef signed char  int8;

        typedef unsigned int   unsigned32;
        typedef unsigned short unsigned16;
        typedef unsigned char  unsigned8;
    #else
        #ifdef _WINDOWS                                     // If IBM PC
           typedef signed long     int32;
           typedef signed short    int16;
           typedef char            int8;

           typedef unsigned long  unsigned32;
           typedef unsigned short unsigned16;
           typedef unsigned char  unsigned8;
        #else
      // FIXME: Portability
           typedef signed long  int32;
           typedef signed short int16;
           typedef char  int8;

           typedef unsigned long  unsigned32;
           typedef unsigned short unsigned16;
           typedef unsigned char  unsigned8;
        #endif
    #endif
#endif

  // Added by GDN
  typedef unsigned16  uint16;
  typedef unsigned32  uint32;
  
// Portable boolean type
#ifndef macintosh

  #ifndef false
    #define false 0
  #endif
  #ifndef true
    #define true 1
  #endif

  #ifndef Boolean_ID 
    #define Boolean_ID
    typedef unsigned char Boolean;
  #endif
#endif

// MWP 4/29/96: Use these for TRUE & FALSE
#ifndef FALSE
    #define FALSE   0
#endif

#ifndef TRUE
    #define TRUE    1
#endif


/* Portable error code type */
#ifndef macintosh
    typedef int16 OSErr;        
#endif

  // Added by GDN
  typedef unsigned char *StrPtr;
  typedef const unsigned char *ConstStrPtr;

  typedef void* ptr;
  
//  Constants
//  ---------

    #ifndef NULL
      #define NULL 0
    #endif
    
  enum UniteMetrique { uMm, uCm, uInch, uPixel, uPoint, uScreenPixel, uDpi, uDegrees, uMegabytes };

//  ------------------------------------------------------------------------------------------------
#endif // FITypes_h
//  ------------------------------------------------------------------------------------------------
