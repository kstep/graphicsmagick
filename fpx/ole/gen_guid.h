//  ----------------------------------------------------------------------------
//  MODULE    : Gen_guid 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, May 2, 1996 
//  DESCRIPTION : Guid generation compatible Mac and Windows.
//  SCCSID      : @(#)gen_guid.h  1.1 11:53:27 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
//  ----------------------------------------------------------------------------
  #ifndef Gen_guid_h
  #define Gen_guid_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

//  Classes Definitions
//  -------------------

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

extern "C" {

  HRESULT GenGuid(CLSID *clsid);
  
} // extern "C"

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // Gen_guid_h
//  ----------------------------------------------------------------------------
