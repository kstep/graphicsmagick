//  ----------------------------------------------------------------------------
//  MODULE    : IVUEImageView 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Friday, April 5, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)ri_imgvw.h  1.1 11:46:43 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef IVUEImageView_h
  #define IVUEImageView_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include "ri_sys.h"
#endif
#ifndef AdvancedIVUE_h
  #include  "viewimg.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PIVUEImageView;

//  Classes Definitions
//  -------------------

  class PIVUEImageView : public ViewImage {

  public:
                 // Open the file in Create Mode 
                 PIVUEImageView (FicNom& refName);
                 // Create an IVUE Image View from an already opened image
                 PIVUEImageView (PRIImage* theImage);
        virtual     ~PIVUEImageView ();
  };

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // IVUEImageView_h
//  ----------------------------------------------------------------------------
