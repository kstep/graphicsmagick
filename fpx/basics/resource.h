//  ------------------------------------------------------------------------------------------------
//  MODULE    : Ressource
//  LANGAGE   : C++
//  CREATEUR  : Adolfo VIDE
//  DATE    : Jeudi 15 Octobre 1992
//  DESCRIPTION : Functions to access resources
//  COMMENTAIRE :
//  SCCSID      : @(#)resource.h  1.1 11:50:07 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef Ressource_h
  #define Ressource_h
  #ifndef ApplCommun_h
    #include  "common_a.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include "b_string.h"


//  Fonctions 'extern'
//  ------------------
  
  // Get the value of a 'STR ' resource. Return an empty string if it doesn't exist.
  Chaine ReadResString(short resourceNumber);
  
  // Mix 4 unsigned shorts into 1 string
  void InterleaveFourNumbers(const unsigned16 number[4], Str255 result);
  // Mix 4 unsigned shorts into 1 string
  Boolean UnInterleaveFourNumbers(const Str255 interleavedString, unsigned16 number[4]);

//  Macros
//  ------

//  Functions 'inline'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // Ressource_h
//  ------------------------------------------------------------------------------------------------
