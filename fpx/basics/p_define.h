//  ------------------------------------------------------------------------------------------------
//  MODULE      : FIDefines
//  LANGAGE     : C and C++
//  CREATOR     : Laurent SABORET 04/02/96, from FITypes.h by Laurent ALBERT
//  DESCRIPTION : Header included everywhere. Defines compilation conditions.
//  COMMENT   : Mustn't include ApplCommun.h
//  SCCSID      : @(#)p_define.h  1.1 11:50:04 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
#ifndef FIDefines_h
#define FIDefines_h
//  ------------------------------------------------------------------------------------------------


//  Set the platform define
//  -----------------------

// Are we on PC ?
#ifdef MSDOS
    #ifndef _WINDOWS
      #define _WINDOWS
    #endif
#endif
#ifdef WIN32
    #ifndef _WINDOWS
      #define _WINDOWS
    #endif
#endif

// Are we on Macintosh ?
#ifdef __MWERKS__                           // If Code Warrior
  #ifndef macintosh
      #define macintosh
  #endif
#endif


//  Includes
//  --------

#ifdef macintosh
  // Includes ConditionalMacros.h if Universal headers. To define USE_UNIVERS_HDR.
  #ifndef __TYPES__
    #include <Types.h>
  #endif
#endif


//  Definitions
//  -----------

// Are we using Apple's Universal headers ?
#undef USE_UNIVERS_HDR
#ifdef macintosh
    #ifdef __CONDITIONALMACROS__
        #ifndef GENERATING68K
            #define USE_UNIVERS_HDR 1       // Universal Headers 1
        #else
            #define USE_UNIVERS_HDR 2       // Universal Headers 2
        #endif
    #endif
#endif

// Other defines:
#ifdef macintosh
    #ifdef __MWERKS__
        #ifdef powerc                       // If Code Warrior PPC
            #define DEFINE_INT32_FUNCTIONS  // Define long and int32 IO functions
            #define newAllocAray            // Redefine new[]
            #define ENABLE_FREE_NULL        // free(NULL) doesn't crash
            #define USE_EXCEPTIONS          // Use exceptions mecanism
        #else                               // If Code Warrior 68K
            #define  DEFINE_INT32_FUNCTIONS
            #define newAllocAray
            #define ENABLE_FREE_NULL
            #define USE_EXCEPTIONS
        #endif
    #else
        #ifdef powerc                                       // If MPW PPC
            #define DEFINE_INT32_FUNCTIONS
            #define newAllocAray
            #define ENABLE_FREE_NULL
            #define USE_EXCEPTIONS
        #else
            #ifdef applec                                   // If MPW 68K
                #define DEFINE_INT32_FUNCTIONS
                #define newAllocAray
                #undef  ENABLE_FREE_NULL
                #define USE_EXCEPTIONS
            #else
                // error
            #endif
        #endif
    #endif
#else
    #ifdef __alpha                                              // If DEC ALPHA
        #define  DEFINE_INT32_FUNCTIONS
        #define  newAllocAray
        #undef   ENABLE_FREE_NULL
        #define  USE_EXCEPTIONS
    #else
        #ifdef _WINDOWS                         // If IBM PC
     #if _MSC_VER < 1000      // If Visual C++ 2.x
        #define DEFINE_INT32_FUNCTIONS
        #undef  newAllocAray
        #undef  ENABLE_FREE_NULL
        #define USE_EXCEPTIONS
     #else
        #define DEFINE_INT32_FUNCTIONS
        #define  newAllocAray
        #undef  ENABLE_FREE_NULL
        #define USE_EXCEPTIONS
     #endif
        #else
           // FIXME: portability
           #define DEFINE_INT32_FUNCTIONS
           #define newAllocAray
           #define ENABLE_FREE_NULL
           #define USE_EXCEPTIONS
        #endif
    #endif
#endif

// Define __bigEndian
#ifdef WORDS_BIGENDIAN
    #define __bigEndian
#else
    #ifdef macintosh                                                // If MacIntosh
       #define __bigEndian
    #else
       #ifdef __alpha                                              // If DEC ALPHA
           #undef __bigEndian
       #else
           #ifdef IRIS                                             // If Silicon Graphics
               #define __bigEndian
           #else
               #ifdef _WINDOWS                                     // If IBM PC
                   #undef __bigEndian
               #else
                  // error
               #endif
           #endif
       #endif
   #endif
#endif

//  ------------------------------------------------------------------------------------------------
#endif // FIDefines_h
//  ------------------------------------------------------------------------------------------------
