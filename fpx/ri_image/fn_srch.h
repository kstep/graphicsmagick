//  ------------------------------------------------------------------------------------------------
//  MODULE    : FicNomSearchIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Laurent Saboret
//  DATE    : Monday, March 14th 1994
//  DESCRIPTION : 
//  COMMENT   : LS, 2 February 1995 : SelectionneSauvegarde() is moved into FicNomSaveIVUE.cp
//  SCCSID      : @(#)fn_srch.h 1.2 14:49:39 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifndef FicNomSearchIVUE_h
  #define FicNomSearchIVUE_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------
#ifndef FicNom_h
  #include "filename.h"
#endif
  
//  Constants
//  ---------

  enum SearchImagePref {searchOnFolder, searchOnLocalDrive, searchOnDistantDrive};

//  Class declarations 
//  ------------------

  class FicNomSearchIVUE;
  typedef FicNomSearchIVUE* ptr_FicNomSearchIVUE;
  typedef FicNomSearchIVUE& ref_FicNomSearchIVUE;

//  Class definitions 
//  -----------------

  // Standard file search class 
  class FicNomSearchIVUE : public FicNom 
  {
  public:
                FicNomSearchIVUE();
#ifdef macintosh
                FicNomSearchIVUE(const FSSpec& specs);
#endif              
                FicNomSearchIVUE(const ChaineFic& fileName, OSType fileType);
                FicNomSearchIVUE(const FicNom& fileName);
                FicNomSearchIVUE(const FicNomSearchIVUE& fileName);
  
        OSErr     Search();
        OSErr     SearchStrategy ();
        Boolean     SelectImage (SearchImagePref searchImagePref, OSType typeA, OSType typeB);
  };

//  Functions 'inline'
//  ------------------

//  Functions 'extern'
//  ------------------

//  Variables 'extern'
//  ------------------

//  ------------------------------------------------------------------------------------------------
  #endif // FicNomSearchIVUE_h
//  ------------------------------------------------------------------------------------------------
