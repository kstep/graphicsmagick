//  ------------------------------------------------------------------------------------------------
//  MODULE      : SwapBytes
//  LANGAGE     : C++
//  CREATOR     : Laurent SABORET 04/02/96, from FITypes.h by Laurent ALBERT
//  DESCRIPTION : Contains byte ordering functions
//  COMMENT   : Doesn't compile with a C compiler
//  SCCSID      : @(#)swapbyte.h  1.1 11:50:08 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef SwapBytes_h
#define SwapBytes_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

    #include "common_a.h"

//  Functions
//  ---------

// Convert back and forth little endian <-> big endian integers
inline int32 SwapBytes (int32 x)
{
    return  (((unsigned32)x >> 24) & 0x000000FF) |
            (((unsigned32)x >> 8)  & 0x0000FF00) |
            (((unsigned32)x << 8)  & 0x00FF0000) |
            (((unsigned32)x << 24) & 0xFF000000) ;
}

// Convert back and forth little endian <-> big endian integers
inline int16 SwapBytes (int16 x)
{
    return  (((unsigned short)x >> 8)  & 0x00FF) |
            (((unsigned short)x << 8)  & 0xFF00) ;
}

//  ------------------------------------------------------------------------------------------------
#endif // SwapBytes_h
//  ------------------------------------------------------------------------------------------------
