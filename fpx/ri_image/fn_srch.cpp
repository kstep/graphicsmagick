//  ------------------------------------------------------------------------------------------------
//  MODULE    : FicNomSearchIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Laurent Saboret
//  DATE    : Monday, March 14th 1994
//  DESCRIPTION : 
//  COMMENT   : LS, 2 February 1995 : SelectionneSauvegarde() is moved into FicNomSaveIVUE.cp
//  SCCSID      : @(#)fn_srch.cpp 1.2 14:49:38 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//  ------------------------------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment FicNomSearchIVUE
  #endif
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
  #include "fn_srch.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifdef macintosh
  #include  <Errors.h>
#endif
  #include  "debug.h"

//  Constants
//  ---------

  #define nbMaxNames  30
  
//  Types
//  -----

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------
//  Functions 'intern'
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods of FicNomSearchIVUE
//  ------------------------------------------------------------------------------------------------


FicNomSearchIVUE::FicNomSearchIVUE()

{
  type = 'IVUE';
}
  
FicNomSearchIVUE::FicNomSearchIVUE(const ChaineFic& fileName, OSType fileType) : FicNom(fileName, fileType)

{
}

#ifdef macintosh

FicNomSearchIVUE::FicNomSearchIVUE(const FSSpec& specs) : FicNom(specs)

{
}

#endif

FicNomSearchIVUE::FicNomSearchIVUE(const FicNom& fileName) : FicNom(fileName)

{
}

FicNomSearchIVUE::FicNomSearchIVUE(const FicNomSearchIVUE& fileName) : FicNom(fileName)

{
}



OSErr FicNomSearchIVUE::Search ()

{
  return FicNom::Search();
}

OSErr FicNomSearchIVUE::SearchStrategy ()

{
  Boolean found =  (Boolean)Exist();

  if (found)
    return noErr;
  else
    return fnfErr;
}

Boolean FicNomSearchIVUE::SelectImage (SearchImagePref /*searchImagePref*/,
                                       OSType /*typeA*/, OSType /*typeB*/)

{
  Boolean found = FALSE;
  
  return found;
}
