//  ------------------------------------------------------------------------------------------------
//  MODULE    : Commun
//  LANGAGE   : C(++)
//  DESCRIPTION : Header included everywhere but by Application project and PlutonCommun.h
//  COMMENTAIRE : Contains global declarations.
//  SCCSID      : @(#)common.h  1.1 11:49:57 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef Commun_h
#   define Commun_h
//  ------------------------------------------------------------------------------------------------

/**
 * Under VISUALC we have single threaded static libraries, or
 * mutli-threaded DLLs using the multithreaded runtime DLLs.
 **/
#if defined(_VISUALC_)
#   if defined(_MT) && defined(_DLL) && !defined(_FPXDLL_) && !defined(_LIB)
#      define _FPXDLL_
#   endif
#   if defined(_FPXDLL_)
#      pragma warning( disable: 4273 )  /* Disable the stupid dll linkage warnings */
#      if !defined(_FPXLIB_)
#         define JPEGEXPORT extern __declspec(dllimport)
#         define FPXEXPORT extern __declspec(dllimport)
#         pragma message( "Compiling as dll import" ) 
#      else
#         define JPEGEXPORT extern __declspec(dllexport)
#         define FPXEXPORT extern __declspec(dllexport)
#         pragma message( "Compiling as dll export" ) 
#      endif
#   else
#      define FPXEXPORT extern
#      define JPEGEXPORT extern
#      pragma message( "Compiling as simple library" ) 
#   endif
#   pragma warning(disable : 4018)
#   pragma warning(disable : 4244)
#   pragma warning(disable : 4142)
#else
#   define FPXEXPORT
#   define JPEGEXPORT
#endif

#ifndef ApplCommun_h
  #include  "common_a.h"
#endif

#ifndef PlutonCommun_h
  #include  "common_b.h"
#endif

//  ------------------------------------------------------------------------------------------------
  #endif // Commun_h
//  ------------------------------------------------------------------------------------------------
