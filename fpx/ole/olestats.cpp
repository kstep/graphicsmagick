//  ------------------------------------------------------------------------------------------
//  MODULE    : OLEEnumStatstg.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file contains the OLEEnumStatstg object 
//
//  COMMENT   : The OLEEnumStatstg object implements the IEnumSTATSTG interface which 
//          enumnerates every object within an open storage. The returned information
//          is in the STATSTG type. An instance of the IEnumSTATSTG is returned by 
//          calling IStorage::EnumElements. The OLEEnumStatstg object contains a pointer
//          to the IEnumSTATSTG and provides following methods,
//
//            Boolean OLEEnumStatstg::Next( STATSTG * stsg); 
//            Boolean OLEEnumStatstg::IsStorage( STATSTG * stsg);
//            Boolean OLEEnumStatstg::IsStream( STATSTG * stsg);
//            Boolean OLEEnumStatstg::IsPropertySet( STATSTG * stsg); 
//            Boolean OLEEnumStatstg::Skip( unsigned long numStorage ); 
//            Boolean OLEEnumStatstg::Reset( ); 
//
//  SCCSID      : @(#)olestats.cpp  1.2 14:47:42 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 
//  ----------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
#ifndef OLEEnumStatstg_h
  #include  "olestats.h"
#endif
  
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Functions 'intern'
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLEStorage
//  ----------------------------------------------------------------------------


// This constructor creates a OLE statstg object from an open storage
OLEEnumStatstg::OLEEnumStatstg (OLEStorage * parentStorage, 
                IEnumSTATSTG * currentEnumStatstg)
{
  enumStatstg = currentEnumStatstg; // store the IEnumSTATSTG object
  parStorage = parentStorage;     // store the parent storage pointer
  //enumStatstg->AddRef();
}


OLEEnumStatstg::~OLEEnumStatstg()
{
  if (enumStatstg) enumStatstg->Release();
}


// This function returns true if next object exists, the information is returned the STATSTG object
Boolean OLEEnumStatstg::Next( STATSTG * stsg)
{
  if (enumStatstg) {
    // Check to see if next object exists or not
    if ( enumStatstg->Next(1, stsg, NULL) == S_OK )
      return true;
  } else  
    return false;
  
  return false; 
}


// This function returns true if the object specified in STATSTG is storage
Boolean OLEEnumStatstg::IsStorage( STATSTG * stsg)
{
  // Return true if it is a storage. The object type is contained in STATSTG.type
  if ( stsg->type == STGTY_STORAGE )
    return true;
  
  return false;
}


// This function returns true if the object specified in STATSTG is stream
Boolean OLEEnumStatstg::IsStream( STATSTG * stsg)
{
  // Return true if it is a stream. The object type is contained in STATSTG.type, 
  // for property set which is also a stream, the first letter of type name is not 5.
  if ( stsg->type == STGTY_STREAM && stsg->pwcsName[0] != '\5')
    return true;
  
  return false;
}


// This function returns true if the object specified in STATSTG is property set
Boolean OLEEnumStatstg::IsPropertySet( STATSTG * stsg)
{
  // Return true if it is a property set. The object type is contained in STATSTG.type. 
  // Property set is specified by the first letter of type name which is 5.
  if ( stsg->pwcsName[0] == '\5' )
    return true;
  
  return false;
}


// This function skips the number of objects specified from current position
Boolean OLEEnumStatstg::Skip( unsigned long numStorage )
{
  HRESULT err; 
  if (enumStatstg) {
    // Skips the number of objects
    if (FAILED(err = enumStatstg->Skip(numStorage)))
    {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
  } else  
    return false;
  
  return true;
}


// This function sets the current object to the first one in the storage
Boolean OLEEnumStatstg::Reset( )
{
  HRESULT err;
  if (enumStatstg) {
    // Reset the position
    if (FAILED(err = enumStatstg->Reset()))
    {
      fpxStatus = OLEtoFPXError(err);
      lastError = TranslateOLEError(err);
      return false;
    } 
  } else  
    return false;
  
  return true;
}

