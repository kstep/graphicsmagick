//  ------------------------------------------------------------------------------------------------
//  MODULE    : Debug
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Vendredi 25 Septembre 1992
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)debug.h 1.2 09:04:26 20 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------

#ifndef Debug_h
#define Debug_h

//  --------
//  Includes
//  --------

  #include <stdio.h>

  #include "b_types.h"

#ifdef macintosh
  #include <files.h>
  #include <OSUtils.h>
#endif


//  ----------------------------------------------------------
//  Declarations des classes et/ou structures et types derives
//  ----------------------------------------------------------

//  ----------------------------------------
//  Definitions des classes et/ou structures
//  ----------------------------------------

//  ----------------------------------
//  Declarations des fonctions inlines
//  ----------------------------------

  // Simple trace. Example : VISU "toto = %d\n", toto FIN

#ifdef NDEBUG
  #define VISU  { if (debug == NULL) DebugInit("debug.tmp"); fprintf(debug,
#else
  #define VISU  { if (debug == NULL) DebugInit("debug.tmp"); dbg_PrintIndent(); fprintf(debug,
#endif

  #define FIN   ); }






  // Trace followed by flush() (doesn't loose datas if the application crashes)
  
  #define VISU2 VISU  

#ifdef macintosh
  #define FIN2  ); fflush(debug); FlushVol(NULL, debugVolRefNum); }
#else
  #define FIN2  ); fflush(debug); }
#endif





  // Assert(expr) beeps and writes the error's location if 'expr' is false (only in DEBUG mode)

#undef assert 
#ifndef NDEBUG
  #ifdef macintosh
  #define assert(expr)  { if (!(expr))                                   \
                {                                        \
                SysBeep(0);                                  \
                VISU2 "File ""%s""; line %d # Assertion fausse\n", __FILE__, __LINE__ FIN2   \
                }                                        \
              }
  #else
  #define assert(expr)  { if (!(expr))                                   \
                { VISU2 "File ""%s""; line %d # Assertion fausse\n", __FILE__, __LINE__ FIN2 } \
              }
  #endif
#else
  #define assert(expr)  { ; }
#endif


  // Methods to enter/return from a method, using indentation
#ifndef NDEBUG
  #define   DBG_ENTER(name)     { dbg_EnterFunction( (name) ); }
  #define   DBG_RETURN(val)     { dbg_ExitFunction(); return (val);}
  #define   DBG_VOID_RETURN     { dbg_ExitFunction(); return; }
  #define   DBG_PRINT       { if (debug == NULL) DebugInit("debug.tmp"); dbg_PrintIndent();  fprintf(debug,
#else
  #define   DBG_ENTER(name)     { ; }
  #define   DBG_RETURN(val)     { return (val); }
  #define   DBG_VOID_RETURN     { return; }
  #define   DBG_PRINT       { if (debug == NULL) DebugInit("debug.tmp"); fprintf(debug,
#endif
//  -----------------------------------
//  Declarations des variables externes
//  -----------------------------------

  extern FILE* debug;
  extern short debugVolRefNum;

//  -----------------------------------
//  Declarations des fonctions externes
//  -----------------------------------

  extern void DebugInit(char* nom);
  extern void DebugEnd();
  
#ifndef NDEBUG
  extern void dbg_EnterFunction(char * name);
  extern void dbg_ExitFunction( );
  extern void dbg_PrintIndent( );
#endif

#endif // Debug_h
