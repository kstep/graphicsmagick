//  ------------------------------------------------------------------------------------------------
//  MODULE    : ApplCommun
//  LANGAGE   : C++
//  DESCRIPTION : Header included everywhere in the Application folder. For backward compatibility.
//  COMMENTS  :   Doesn't compile with a C compiler
//        : LS 04/02/96: This file isn't supposed to be included everywhere anymore
//                   Move the Pixel type to Pixel.h
//  SCCSID      : @(#)common_a.h  1.1 11:49:58 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef ApplCommun_h
  #define ApplCommun_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef FIDefines_h
    #include  "p_define.h"
#endif
#ifndef FITypes_h
  #include  "b_types.h"
#endif
#ifndef MacCompatibility_h
  #include  "mac_comp.h"
#endif
#ifndef FIErrors_h
    #include  "b_error.h"
#endif
#ifndef Pixel_h
  #include  "pixel.h"
#endif


//  Misc
//  ----

  // To avoid an "unused parameter" compiler warning (seems not to work on PC)
  #define obligatoire , 0
  #define mandatory   , 0


//  ------------------------------------------------------------------------------------------------
  #endif // ApplCommun_h
//  ------------------------------------------------------------------------------------------------
