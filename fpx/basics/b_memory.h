//  ------------------------------------------------------------------------------------------------
//  MODULE    : Memoire
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Vendredi 16 Octobre 1992
//  DESCRIPTION : 
//  COMMENTAIRE :
//  SCCSID      : @(#)b_memory.h  1.1 11:49:55 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Memoire_h
  #define Memoire_h
  #ifndef ApplCommun_h
    #include  "common_a.h"
  #endif // ApplCommun_h
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  <stddef.h>
  #include  <stdlib.h>

#if defined(USE_LEGACY_INCLUDES)
# include  <new.h>
#else
# include <new>
#endif

//  Constantes
//  ----------

// This is the standard selection for Intel Windows machines
// Standard arrays creators and destructors

// make sure that new calls calloc

  #define AllocArray(tabElem,typeElem,nbElem) tabElem = new typeElem[nbElem]

  #define DeleteArray(tabElem,typeElem) delete [] tabElem

  #define FastAllocArray(tabElem,typeElem,nbElem) tabElem = new typeElem[nbElem]

  #define FastDeleteArray(tabElem,typeElem) delete [] tabElem

  #define CallocArray(tabElem,typeElem,nbElem) tabElem = new typeElem[nbElem]

  #define FastCallocArray(tabElem,typeElem,nbElem) tabElem = new typeElem[nbElem]

//  ------------------------------------------------------------------------------------------------
  #endif // Memoire_h
//  ------------------------------------------------------------------------------------------------
