//  ----------------------------------------------------------------------------
//  MODULE    : PImageFile
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 13, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)pimgfile.cpp  1.1 11:46:41 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "pimgfile.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

//  Constants
//  ---------

  #define ERROR_FILE_ALERT      674
  #define COMPRESSOR_ERROR_ALERT    681

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PImageFile class
//  
//  Manage all access to an Image file. 
//  ----------------------------------------------------------------------------

PImageFile::PImageFile ()
{
  nbErrors = 0;
}

PImageFile::PImageFile (ref_FicNom fName)
{
  fileName = fName;
  nbErrors = 0;
}

void PImageFile::SignaleErreur()
{
  GtheSystemToolkit->AddErrorToList(ERROR_FILE_ALERT, GetError(), fileName);
}

void PImageFile::CompressionError()
{
  // We use codecErr to signal a Compressor Error
  GtheSystemToolkit->AddErrorToList(COMPRESSOR_ERROR_ALERT, -1, fileName);
}

void PImageFile::DoNotCloseFileWhenDelete()
{
}

void PImageFile::CloseFileWhenDelete()
{
}

unsigned long PImageFile::GetFileSize()
{
  return 0;
}
            
//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
