//  ----------------------------------------------------------------------------
//  MODULE    : FlashPixFormat 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, February 15, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID          : @(#)fpxformt.cpp  1.3 13:16:17 02 Jun 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "fpxformt.h"
//  ----------------------------------------------------------------------------

#ifndef macintosh
  #ifndef false
    #define false 0
  #endif
  #ifndef true
    #define true 1
  #endif
#endif

//  Includes
//  --------

  #include  <string.h>
  #include  <stdio.h>

#if defined(USE_LEGACY_INCLUDES)
#  include <fstream.h>
#else
#  include <fstream>
   using namespace std;
#endif
  
#ifdef macintosh
  #include  <Errors.h>
#endif  

#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
#ifndef OLEStreams_h
  #include  "olestrm.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif
#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif
#ifndef OLEFiles_h
  #include  "olefiles.h"
#endif

#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif



//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PFlashPixFile class
//  
//  Manage all access to the properties of a FlashPix file. 
//  ----------------------------------------------------------------------------


PFlashPixFile::PFlashPixFile (ref_FicNom fName, const char* theStorageName, mode_Ouverture theMode) : PImageFile(fName)
{
  CLSID clsID = ID_Baseline;
  Boolean ok;

  // Init the root stuff
  oleFile     = NULL;
  parentStorage = NULL;
  rootStorage   = NULL;

  // Init the OLE Access Flag Mode
  switch (theMode) {
      case mode_Lecture:
        mode = OLE_READ_ONLY_MODE;
        break;
      case mode_Modification:
        mode = OLE_READWRITE_MODE;
        break;
      case mode_Ecrasement:
      case mode_Reset:
        mode = OLE_CREATE_MODE;
        break;
  }
  
  // Set the pointers to NULL to start with...
  imageContentsPropertySet  = NULL;

  imageInfoPropertySet    = NULL;

  summaryInfoPropertySet    = NULL;

  // Create the OLE file record
  oleFile = new OLEFile(fName,theStorageName);
  
  // Open or create the root storage 
  if (mode == OLE_CREATE_MODE)
    ok = oleFile->CreateOLEFile (clsID, &rootStorage);
  else
    ok = oleFile->OpenOLEFile (clsID, &rootStorage, mode);

  // Init the rest or raise an error
  if (ok)
    Init ();
  else
    SignaleErreur();

}

PFlashPixFile::PFlashPixFile (OLEStorage* theOwningStorage, const char* theStorageName, mode_Ouverture theMode) : PImageFile()
{
  CLSID clsID = ID_Baseline;
  Boolean ok = true;  // djk 052297 AI17850: initialize

  // Init the root stuff
  oleFile     = NULL;
  // BEGIN: djk 052297 AI17850: handle sub-storages correctly
  ///parentStorage  = theOwningStorage;
  parentStorage = NULL;
  // END: djk 052297 AI17850: handle sub-storages correctly
  rootStorage   = NULL;

  // Init the OLE Access Flag Mode
  switch (theMode) {
      case mode_Lecture:
        mode = OLE_READ_ONLY_MODE;
        break;
      case mode_Modification:
        mode = OLE_READWRITE_MODE;
        break;
      case mode_Ecrasement:
      case mode_Reset:
        mode = OLE_CREATE_MODE;
        break;
  }
  
  // Set the pointers to NULL to start with...
  imageContentsPropertySet  = NULL;

  imageInfoPropertySet    = NULL;

  summaryInfoPropertySet    = NULL;

  // BEGIN: djk 052297 AI17850: handle sub-storages correctly
  if (theStorageName)
  {
    parentStorage = theOwningStorage;

    // Open or create the root storage 
    if (mode == OLE_CREATE_MODE)
      ok = parentStorage->CreateStorage (clsID, theStorageName, &rootStorage);
    else
      ok = parentStorage->OpenStorage (clsID, theStorageName, &rootStorage, mode);
  }
  else
  {
    parentStorage = NULL;
    rootStorage = theOwningStorage;
  }
  // END: djk 052297 AI17850: handle sub-storages correctly

  // Init the rest or raise an error
  if (ok)
    Init ();
  else
    SignaleErreur();
}

void PFlashPixFile::Init ()
{
  Boolean err = false;
  
  // Names of the property sets
  char summaryInfoName [33];
  char imageContentName [33];
  char imageInfoName [33];
  char iccProfileName [33];
  
  GetSummaryInfoName(summaryInfoName);
  GetImageContentName(imageContentName);
  GetImageInfoName(imageInfoName);

  // Create the basic OLE stuff: root storage, summary property set and image content property set 
  if (mode == OLE_CREATE_MODE) {

    // Summary Info property set (required)
    GUID summaryInfoGuid = ID_SummaryInfo;
    if (rootStorage->CreatePropertySet (summaryInfoGuid, summaryInfoName, &summaryInfoPropertySet)) 
      InitSummaryInfoPropertySet();
    else
      err = true;   // rootStorage->getFPXStatus();

    // Image Content property set (required)
    GUID imageContentGuid = ID_ImageContent;
    if ( !rootStorage->CreatePropertySet(imageContentGuid, imageContentName, &imageContentsPropertySet)) 
      err = true;

    // Create the default ICC profile with index 1
    GUID iccProfileGuid = ID_ICCProfiles;
    OLEHeaderStream*  iccprofilestream = NULL; 
    GetICCProfileName(iccProfileName, 1);
    if (!rootStorage->CreateHeaderStream(iccProfileGuid, iccProfileName, &iccprofilestream))
      err = true;
    
    // Create the default CompObj
    if ( !CreateCompObj() )  
      err = TRUE;
    
  } else {

    // Summary Info property set (required)
    GUID summaryInfoGuid = ID_SummaryInfo;
    if (rootStorage->OpenPropertySet (summaryInfoGuid, summaryInfoName, &summaryInfoPropertySet, mode)) {
      if ( summaryInfoPropertySet->Revert())
        GetSummaryInfoPropertySet();
      else
        err = true;
    } else
      err = true;

    // Image Content property set (required)
    GUID imageContentGuid = ID_ImageContent;
    if (rootStorage->OpenPropertySet(imageContentGuid, imageContentName, &imageContentsPropertySet, mode)) {
      if ( !imageContentsPropertySet->Revert())
        err = true;
    } else
      err = true;

    // Image Info property set (optionnal)
    GUID imageInfoGuid = ID_ImageInfo;
    if (rootStorage->OpenPropertySet(imageInfoGuid, imageInfoName, &imageInfoPropertySet, mode)) {
      if ( !imageInfoPropertySet->Revert())
        err = true;
    } else 
      imageInfoPropertySet = NULL;
      
  }
  
  // Check if an error occured during the init
  if (err)
    SignaleErreur();
}

PFlashPixFile::~PFlashPixFile ()
{
  // CAUTION: Elements have to be deleted in the reverse order of their creation...
  
  // Commit() makes sure that everything has been written on the disk (flush
  // all ole buffers for the elements)
  if (mode != OLE_READ_ONLY_MODE)
    Commit();
  
  // Delete an element releases the OLE object.
  if (imageContentsPropertySet) {
    delete imageContentsPropertySet;
    imageContentsPropertySet = NULL;  
  }
  if (imageInfoPropertySet) {
    delete imageInfoPropertySet;
    imageInfoPropertySet = NULL;  
  }
  if (summaryInfoPropertySet) {
    delete summaryInfoPropertySet;
    summaryInfoPropertySet = NULL;  
  }

  if (oleFile) {
    // Release the file. CAUTION: the rootStorage is release by the file...
    // NOTE: The above comment has been changed, the rootStorage has to be released separately.
    delete rootStorage;   
    rootStorage = NULL; 
    
    delete oleFile;
    oleFile = NULL;     
  } 
  else if (parentStorage) {
    // CAUTION: do not delete or close the parent storage: it may be still in used by the application...
    delete rootStorage;
    rootStorage = NULL; 
  }
}

Boolean PFlashPixFile::CreateStorage(CLSID& classID, const char * name, OLEStorage ** res)
{
  if (rootStorage)
    return rootStorage->CreateStorage(classID, name, res);
  else
    return false;
}

Boolean PFlashPixFile::OpenStorage(CLSID& classID, const char * name, OLEStorage ** res)
{
  if (rootStorage)
    return rootStorage->OpenStorage(classID, name, res);      
  else
    return false;
}

// Save the ICC profile in root storage by its index
Boolean PFlashPixFile::SetICCProfile(WORD index, char *pICCProfileStr)
{
  char iccProfileName [33];
  GUID iccProfileGuid = ID_ICCProfiles;
  OLEHeaderStream * curICCProfile; 
  
  GetICCProfileName(iccProfileName, index);
  
  if (rootStorage) 
    if ( rootStorage->OpenHeaderStream(iccProfileGuid, iccProfileName, &curICCProfile) )
      curICCProfile->WriteVT_LPSTR(pICCProfileStr);
    else 
      if ( rootStorage->CreateHeaderStream(iccProfileGuid, iccProfileName, &curICCProfile) )
        curICCProfile->WriteVT_LPSTR(pICCProfileStr);
      else 
        return false; 
  else
    return false;

  return true;
}

// Get the ICC profile in root storage by its index
Boolean PFlashPixFile::GetICCProfile(WORD index, char **ppICCProfileStr)
{
  char iccProfileName [33];
  GUID iccProfileGuid = ID_ICCProfiles;
  OLEHeaderStream * curICCProfile; 
  
  GetICCProfileName(iccProfileName, index);
  
  if (rootStorage)
    if ( rootStorage->OpenHeaderStream(iccProfileGuid, iccProfileName, &curICCProfile) )
      curICCProfile->ReadVT_LPSTR(ppICCProfileStr);
    else 
      return false; 
  else
    return false;
        
  return true;
}

// This should probably go somewhere else
// FIXME:
#if     defined(_UNIX)

#define USERCLASSTYPE_APPNAME   0

extern HRESULT OleRegGetUserType(REFCLSID clsid, DWORD whatever, LPOLESTR FAR* pszUserType);

extern HRESULT StringFromCLSID(REFCLSID clsid, LPOLESTR FAR* lplpsz);

#endif

// Create CompObj in root storage 
Boolean PFlashPixFile::CreateCompObj()
{
  OLEStream * curCompObj; 

  if (rootStorage->CreateStream("\1CompObj", &curCompObj)) {
  
    // Write the header 
    DWORD temp;
    temp = 0xfffe0001; 
    curCompObj->WriteVT_I4(&temp);
    
    temp = 0x00000a03; 
    curCompObj->WriteVT_I4(&temp);

    temp = (unsigned long)-1; 
    curCompObj->WriteVT_I4(&temp);

    CLSID clsID = ID_Baseline;
    curCompObj->WriteVT_CLSID(&clsID);

    // Get new user type name; if error, set to NULL string
    OLECHAR chZero = 0;
    LPOLESTR lpszNew = NULL;
    if (OleRegGetUserType(clsID, USERCLASSTYPE_APPNAME, &lpszNew) != S_OK)
      lpszNew = &chZero;
#ifdef macintosh
    curCompObj->WriteVT_LPSTR_NoPad(lpszNew);
#elif _WINDOWS
    char str[128];
    WideCharToMultiByte(CP_ACP, 0, lpszNew, -1, str, 128, NULL, NULL);
    curCompObj->WriteVT_LPSTR_NoPad(str);
#endif      

    // Get and write clsID string
    LPOLESTR FAR clsIDStr;      
    StringFromCLSID(clsID, &clsIDStr);
#ifdef macintosh
    curCompObj->WriteVT_LPSTR_NoPad(clsIDStr);
#elif _WINDOWS
    WideCharToMultiByte(CP_ACP, 0, clsIDStr, -1, str, 128, NULL, NULL);
    curCompObj->WriteVT_LPSTR_NoPad(str);
#endif

    // Write prog id ( for now just write length = 0 )
#ifdef macintosh
    curCompObj->WriteVT_LPSTR_NoPad(&chZero);
#elif _WINDOWS
    char progID = 0; 
    curCompObj->WriteVT_LPSTR_NoPad(&progID);
#endif      
    
    // Write magic number
    temp = 0x71B239F4; 
    curCompObj->WriteVT_I4(&temp);
    
    // Write unicode user type ( for now just write length = 0 )
#ifdef macintosh
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(lpszNew));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(lpszNew);
#endif      
    
    // Write unicode clipboard format 
#ifdef macintosh
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(&chZero));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(&chZero);
#endif      
    
    // Write unicode prog id ( for now just write length = 0 )
#ifdef macintosh
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(&chZero));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(&chZero);
#endif    
  } 
  else
    return FALSE; 
    
  return TRUE; 
}

// Read CompObj in root storage 
Boolean PFlashPixFile::ReadCompObj(char **ppUserType, char **ppClipFormat)
{
  OLEStream * curCompObj; 
  
  if (rootStorage)
    if ( rootStorage->OpenStream("\1CompObj", &curCompObj) ) {
      curCompObj->Seek(sizeof(COMPOBJHEADER));
      
      // Read user type string
      curCompObj->ReadVT_LPSTR_NoPad(ppUserType);

      // Read clipboard format string
      curCompObj->ReadVT_LPSTR_NoPad(ppClipFormat);
    }
    else 
      return false; 
  else
    return false;
        
  return true;
}

// Write CompObj in root storage 
Boolean PFlashPixFile::WriteCompObj(char *pUserType, char *pClipFormat)
{
  OLEStream * curCompObj; 
  
  if (rootStorage)
    if ( rootStorage->OpenStream("\1CompObj", &curCompObj) ) {
      curCompObj->Seek(sizeof(COMPOBJHEADER));
      
      // Write user type string
      curCompObj->WriteVT_LPSTR_NoPad(pUserType);

      // Write clipboard format string
      curCompObj->WriteVT_LPSTR_NoPad(pClipFormat);
    }
    else 
        return false; 
  else
    return false;
        
  return true;
}

unsigned long PFlashPixFile::GetFileSize()
{
/*
  unsigned long size = 0;
  if (oleFile)
    oleFile->GetFileSize(&size);
  else if (parentStorage)
    parentStorage->GetSize(&size);
  return size;
*/
// Actually, the previous code should work but for whatever reason, the cbsize field
// of the stats is not updated by OLE... Thank you Microsoft...
// Waiting for a bug fix, the following do the job (sort of...)
  Boolean err = false;
  OLEProperty* aProp;
  long  size = 0;
            
  if(GetImageContentProperty (PID_HiResWidth, &aProp))  // full resolution width
    size = long(*aProp);
  else err = true;
  
  if(GetImageContentProperty (PID_HiResHeight, &aProp)) // full resolution height
    size *= long(*aProp);
  else err = true;
  
  if (!err)
    return (size*3);
  else
    return 0;

}


FPXStatus PFlashPixFile::GetError()
{
  return Erreur();
}


FPXStatus PFlashPixFile::Erreur() const
{
  long    error = 0;
  FPXStatus   status = FPX_OK;
  
  if (summaryInfoPropertySet)
    status = summaryInfoPropertySet->getFPXStatus();
  
  if (status == 0) {
    if (imageContentsPropertySet)
      status = imageContentsPropertySet->getFPXStatus();
    else
      status = FPX_OLE_FILE_ERROR;
  }
    
  if (status == FPX_OK) {
    // It's not an error not to have an Image Info property set (optionnal)
    if (imageInfoPropertySet)
      error = imageInfoPropertySet->getFPXStatus();
  }
  
  if (status == FPX_OK)               // If no error durng property set access
    if (oleFile != NULL)            //  and the oleFIle object exists, then
      status = oleFile->getFPXStatus();   //  get the most recent OLE file error
  
  return status;
}


Boolean PFlashPixFile::Commit()
{
  Boolean ok = true;
  if (summaryInfoPropertySet)
    ok &= summaryInfoPropertySet->Commit();
  if (imageContentsPropertySet)
    ok &= imageContentsPropertySet->Commit();
  // Image Info property set is optionnal
  if (imageInfoPropertySet)
    ok &= imageInfoPropertySet->Commit();
  if (rootStorage)
    ok &= rootStorage->Commit();
  return ok;
}

Boolean PFlashPixFile::GetSummaryInfoProperty (DWORD pID, OLEProperty ** res)
{
  return summaryInfoPropertySet->GetProperty(pID, res);
}

Boolean PFlashPixFile::SetSummaryInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  return summaryInfoPropertySet->NewProperty(pID, propType, res);
}

Boolean PFlashPixFile::GetImageContentProperty (DWORD pID, OLEProperty ** res)
{
  return imageContentsPropertySet->GetProperty(pID, res);
}

Boolean PFlashPixFile::SetImageContentProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  return imageContentsPropertySet->NewProperty(pID, propType, res);
}

Boolean PFlashPixFile::GetImageInfoProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = true;
  
  // If absent, there is an error
  if (imageInfoPropertySet == NULL)
    ok = false;
  
  // Get the property
  if (ok)
    ok = imageInfoPropertySet->GetProperty(pID, res);
    
  return ok;
}

Boolean PFlashPixFile::SetImageInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = true;
  char imageInfoName [33];
  GetImageInfoName(imageInfoName);
  
  // If absent, the Image Info property set has to be created
  if (imageInfoPropertySet == NULL) {
    GUID imageInfoGuid = ID_ImageInfo;
    if ( !rootStorage->CreatePropertySet(imageInfoGuid, imageInfoName, &imageInfoPropertySet)) 
      ok = false;
  }
  
  // Add the property to the section
  if (ok)
    ok = imageInfoPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}

Boolean  PFlashPixFile::InitSummaryInfoPropertySet()
{
  OLEProperty* aProp;
  Boolean err = false;
  FILETIME ft; 
  
  if (summaryInfoPropertySet->GetProperty (PROPERTYIDONE, &aProp))    // Change the code page to 1252
    *aProp = (short)(1252);
  else if (summaryInfoPropertySet->NewProperty (PROPERTYIDONE, VT_I2, &aProp))
    *aProp = (short)(1252);
  else 
    err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (PID_EDITTIME, TYP_EDITTIME, &aProp))    // edit time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty (PID_LASTPRINTED, TYP_LASTPRINTED, &aProp))  // last printed
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty (PID_CREATE_DTM, TYP_CREATE_DTM, &aProp))  // create date and time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty(PID_LASTSAVE_DTM, TYP_LASTSAVE_DTM, &aProp)) // last save date and time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty (PID_PAGECOUNT, TYP_PAGECOUNT, &aProp))    // page count
    *aProp = long(0);
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty (PID_WORDCOUNT, TYP_WORDCOUNT, &aProp))    // word count
    *aProp = long(0);
  else err = true;
  
  if(summaryInfoPropertySet->NewProperty (PID_CHARCOUNT, TYP_CHARCOUNT, &aProp))    // char count
    *aProp = long(0);
  else err = true;
  
  if (!err)
    return summaryInfoPropertySet->Commit();
  else
    return false;
}

Boolean  PFlashPixFile::GetSummaryInfoPropertySet()
{
  OLEProperty* aProp;
  Boolean err = false;
  FILETIME ft; 
    
  if( summaryInfoPropertySet->GetProperty (PID_EDITTIME, &aProp) )    // edit time
    ft = FILETIME(*aProp);
  else err = true;
  
  if( summaryInfoPropertySet->GetProperty (PID_LASTPRINTED, &aProp) )   // last printed
    ft = FILETIME(*aProp);
  else err = true;
  
  if( summaryInfoPropertySet->GetProperty (PID_CREATE_DTM, &aProp) )    // create date and time
    ft = FILETIME(*aProp);
  else err = true;
  
  if( summaryInfoPropertySet->GetProperty (PID_LASTSAVE_DTM, &aProp) )  // last save date and time
    ft = FILETIME(*aProp);
  else err = true;
    
  return !err;
}

Boolean  PFlashPixFile::InitImageInfoPropertySet()
{
  OLEProperty*  aProp;
  Boolean     noErr = TRUE;
  FILETIME    ft; 
  
  // Set File Source Group 
  if( SetImageInfoProperty (PID_FileSource, TYP_FileSource, &aProp) ) // File Source  
    *aProp = (long)0;         // Unidentified
  else noErr = FALSE;

  if( SetImageInfoProperty (PID_SceneType, TYP_SceneType, &aProp) ) // Scene Type 
    *aProp = (long)0;         // Unidentified
  else noErr = FALSE;

  // Set capture date
  if( SetImageInfoProperty (PID_CaptureDate, TYP_CaptureDate, &aProp) ) {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = (FILETIME)ft;          
  }
  else noErr = FALSE;

  // Set scan date
  if( SetImageInfoProperty (PID_ScanDate, TYP_ScanDate, &aProp) ) {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = (FILETIME)ft;          
  }
  else noErr = FALSE;

  // Set last modified scan date
  if( SetImageInfoProperty (PID_LastModifiedDate, TYP_LastModifiedDate, &aProp) ) {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = (FILETIME)ft;          
  }
  else noErr = FALSE;

  return noErr;
} 

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------


//  - EOF ----------------------------------------------------------------------
