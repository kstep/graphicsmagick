//  ------------------------------------------------------------------------------------------------
//  MODULE      : MacCompatibility
//  LANGAGE     : C and C++
//  CREATOR     : Laurent SABORET 04/02/96, from FITypes.h and MacMem.h by Laurent ALBERT
//  DESCRIPTION : Define for all platforms common Mac types, constants and functions
//          Mustn't include ApplCommun.h
//
//  COMMENT     LS 04/09/96: Merge with Sophia's server
//  SCCSID      : @(#)mac_comp.h  1.1 11:50:03 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef MacCompatibility_h
#define MacCompatibility_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef FIDefines_h
    #include "p_define.h"
#endif

#ifndef FIErrors_h
    #include "b_error.h"
#endif

  #include <memory.h>
  #include <string.h>
  #include <stdlib.h>

// Mac general headers
#ifdef macintosh          

    #ifndef __QUICKDRAW__
        #include <Quickdraw.h>
    #endif
    
#endif


//  Types and constants
//  -------------------

// Let's define Macintosh's types and constants on other machines :
#ifndef macintosh

    #define nil 0

    typedef unsigned char Byte;
    typedef int8 SignedByte;
    typedef int8 *Ptr;
    typedef Ptr *Handle;            /*  pointer to a master pointer */

    typedef int32 Fixed;            /* fixed point arithmatic type */
    typedef Fixed *FixedPtr;
    typedef int32 Fract;
    typedef Fract *FractPtr;

    typedef unsigned char Str255[256],Str63[64],Str32[33],Str31[32],Str27[28],Str15[16],*StringPtr,**StringHandle;

    typedef const unsigned char *ConstStr255Param;
    typedef ConstStr255Param ConstStr63Param,ConstStr32Param,ConstStr31Param,
     ConstStr27Param,ConstStr15Param;

    typedef unsigned long OSType;
    typedef OSType *OSTypePtr;

#ifndef RGB_DEFINED
    struct RGBColor {
     unsigned16 red;                /*magnitude of red component*/
     unsigned16 green;              /*magnitude of green component*/
     unsigned16 blue;               /*magnitude of blue component*/
    };
    typedef struct RGBColor RGBColor;
#define RGB_DEFINED
#endif
    typedef unsigned16 SmallFract;
    
    struct HSVColor {
        SmallFract hue;             /*Fraction of circle, red at 0*/
        SmallFract saturation;      /*0-1, 0 for gray, 1 for pure color*/
        SmallFract value;           /*0-1, 0 for black, 1 for max intensity*/
    };
    typedef struct HSVColor HSVColor;

    struct CMYKColor {
        unsigned short                  cyan;
        unsigned short                  magenta;
        unsigned short                  yellow;
        unsigned short                  black;
    };
    typedef struct CMYKColor CMYKColor;

    enum {
        MaxSmallFract = 0x0000FFFF
    };
    
#endif // #ifndef macintosh


//  Functions
//  ---------

// Let's define Macintosh's functions on other machines :
#ifndef macintosh

  #define BlockMove(srcptr, dstptr, byteCount) memmove((dstptr), (srcptr), (byteCount))
    
#endif


//  ------------------------------------------------------------------------------------------------
#endif // MacCompatibility_h
//  ------------------------------------------------------------------------------------------------
