//  ----------------------------------------------------------------------------
//  MODULE    : PImageFile 
//  LANGUAGE  : C 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 13, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)pimgfile.h  1.2 14:49:42 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef PImageFile_h
  #define PImageFile_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include  "ri_sys.h"
#endif

#ifndef FPXBaselineIO_h
  #include  "fpxlibio.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PImageFile;

//  Classes Definitions
//  -------------------

  // File handle : this has to be derived depending on the type of file to implement

  class PImageFile : public PToolkitObject  {

      public:
                 PImageFile ();
                 PImageFile (ref_FicNom fName);
    virtual         ~PImageFile ();
        
    virtual void      SignaleErreur();
    virtual void      CompressionError();
    virtual void      DoNotCloseFileWhenDelete();
    virtual void      CloseFileWhenDelete();
    virtual unsigned long GetFileSize();
    
        long      nbErrors;
        
      protected:
      
    virtual FPXStatus   GetError() = 0;   // This changes depending on the file type
    
      private:
        FicNom      fileName;     // Name of the file
  };


//  'inline' Functions
//  ------------------

inline PImageFile::~PImageFile ()

{
}


//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // PImageFile_h
//  ----------------------------------------------------------------------------
