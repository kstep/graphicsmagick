//  ----------------------------------------------------------------------------
//  MODULE    : FileFlashPixView
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, April 4, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//    SCCSID      : @(#)f_fpxvw.cpp 1.5 12:24:50 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifdef macintosh        
    #pragma segment FileFlashPixView
  #endif
//  ----------------------------------------------------------------------------
  #include "f_fpxvw.h"
  #include "olecomm.h"
//  ----------------------------------------------------------------------------

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
//  Methods of the PFileFlashPixView class
//  
//  Manage all access to the properties of a FlashPix file. 
//  ----------------------------------------------------------------------------

PFileFlashPixView::PFileFlashPixView (FicNom& fName, const char* theStorageName, 
                      mode_Ouverture theMode, 
                      long theVisibleOutputIndex) 
              : PImageFile(fName),
                readOnlyMode(FALSE)
{
  CLSID clsID = ID_ImageView;
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

  // Create the OLE file record
  oleFile = new OLEFile(fName,theStorageName);
  // Open or create the root storage 
  if (mode == OLE_CREATE_MODE)
    ok = oleFile->CreateOLEFile (clsID, &rootStorage);
  else
    ok = oleFile->OpenOLEFile (clsID, &rootStorage, mode);

  // CHG_VIS_OUT - set index into the Global info. Prperty Set's Visible Outputs
  //  array to 0. Core images only have one visible output and it is accessed thru
  //  the 0th element of this array. Use of visibleOutputIndex overrides core behavior.
  visibleOutputIndex = theVisibleOutputIndex;

  // Init the rest or raise an error
  if (ok)
    Init ();
  else
    SignaleErreur();
}


PFileFlashPixView::PFileFlashPixView (OLEStorage* theOwningStorage, const char* theStorageName, 
                    mode_Ouverture theMode,
                    long theVisibleOutputIndex) 
              : PImageFile(),
                readOnlyMode(FALSE)
{
  CLSID clsID = ID_ImageView;
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

  // CHG_VIS_OUT - set index into the Global info. Prperty Set's Visible Outputs
  //  array to 0. Core images only have one visible output and it is accessed thru
  //  the 0th element of this array. Use of visibleOutputIndex overrides core behavior.
  visibleOutputIndex = theVisibleOutputIndex;

  // Init the rest or raise an error
  if (ok)
    Init ();
  else
    SignaleErreur();
}


void PFileFlashPixView::Init ()
{
  Boolean err = FALSE;
  
  // Set the pointers to NULL to start with...        
  globalInfoPropertySet = NULL;

  resultDescPropertySet = NULL;
  sourceDescPropertySet = NULL;
  operationPropertySet  = NULL;
  transformPropertySet  = NULL;
  summaryInfoPropertySet  = NULL;
  extensionListPropertySet= NULL;
  
  // Names of the property sets
  char summaryInfoName [33];
  char globalInfoName [33];
  char resultDescName [33];
  char sourceDescName [33];
  char operationName [33];
  char transformName [33];
  char extensionName [33];
  
  // PTCH_OBJFIX - Index/numbers used to form property names. The following are usually 
  //  correct, but are tested once the file is opened. The spec supports more complex
  //  linkages which are determined if not in create mode. 
  sourceDescPropertyIndex = 1;
  resultDescPropertyIndex = 2;
  transformPropertyIndex  = 1;
  operationPropertyIndex  = 1;
  
  GetSummaryInfoName(summaryInfoName);
  GetGlobalInfoName(globalInfoName);
  GetExtensionName(extensionName);

  // Create the basic View stuff: summary property set and the requires property sets
  if (mode == OLE_CREATE_MODE) {
    // CAUTION: in the reference implementation code, we ALWAYS create FPX Image View with transformation,
    // so, some of the optionnal property sets are ALWAYS created by this code.
  
    // PTCH_OBJFIX - Make default names for indexed objects
    GetImageDescName(resultDescName,resultDescPropertyIndex);
    GetImageDescName(sourceDescName,sourceDescPropertyIndex);
    GetTransformName(transformName,transformPropertyIndex);

    // Summary Info property set (required)
    GUID summaryInfoGuid = ID_SummaryInfo;
    if (rootStorage->CreatePropertySet (summaryInfoGuid, summaryInfoName, &summaryInfoPropertySet)) 
      InitSummaryInfoPropertySet();
    else
      err = TRUE;
      
    // Global Info property set (required)
    GUID globalInfoGuid = ID_GlobalInfo;
    if ( !rootStorage->CreatePropertySet(globalInfoGuid, globalInfoName, &globalInfoPropertySet)) 
      err = TRUE;

    // Source Description property set (required)
    GUID sourceDescGuid = ID_ImageDescription;
    if ( !rootStorage->CreatePropertySet(sourceDescGuid, sourceDescName, &sourceDescPropertySet)) 
      err = TRUE;

    // Result Description property set (optionnal)
    resultDescPropertySet = NULL;

    // Operation property set (optionnal)
    operationPropertySet = NULL;

    // Transformation property set (optionnal)
    transformPropertySet = NULL;

    // Extension List property set (optionnal)
    extensionListPropertySet = NULL;

    // Create the default CompObj
    if ( !CreateCompObj() )  
      err = TRUE;
    
  } 
  else {
    // PTCH_OBJFIX - this section has been rewritten to properly determine which object is
    //  the Source Desription object, rather than just assume it's "Data Object 000001".
    //  See RFC_101-7_Misc_3.pdf for a description of the index/path being walked here.
    OLEProperty* aProp; 

    // Summary Info property set (required)
    GUID summaryInfoGuid = ID_SummaryInfo;
    if (rootStorage->OpenPropertySet (summaryInfoGuid, summaryInfoName, &summaryInfoPropertySet, mode)) {
      if ( summaryInfoPropertySet->Revert() )
        GetSummaryInfoPropertySet();
      else
        err = TRUE;     
    } else
      err = TRUE;
      
    // Global Info property set (required)
    GUID globalInfoGuid = ID_GlobalInfo;
    if (rootStorage->OpenPropertySet(globalInfoGuid, globalInfoName, &globalInfoPropertySet, mode))
      globalInfoPropertySet->Revert();
    else
      err = TRUE;
    
    // To find the Source and Result Description Objects and the Data Object Storage, a
    //  series of indices stored in properties must be followed -- starting at Global Info
    if (GetGlobalInfoProperty (PID_VisibleOutputs, &aProp)) {
      char      objectName [33];

      // To find the Source Description Object, use the the Visible Output value in the
      //  GlobalInfo Property as an index. Core only supports one visible output, so just 
      //  use the first value in the array.
      // CHG_VIS_OUT - use the member 'visibleOutputIndex' to access the element
      //  in visible outputs. This will support non-core images such as those
      //  with multiple frames
      FPXLongArray  visibleOutputs = (FPXLongArray)(*aProp);
      if ((unsigned long) visibleOutputIndex >= visibleOutputs.length) {
        visibleOutputIndex = 0;
        err = TRUE;
      }
      long  dataObjectIndx = visibleOutputs.ptr[visibleOutputIndex];
    
      // Use the Visible Output index to make the name of a Description Data Object. This is 
      //  usually the Source Description Object, but it may not be.
      OLEPropertySet  *imgPropSet;
      GetImageDescName (objectName, dataObjectIndx);
      GUID sourceDescGuid = ID_ImageDescription;
      
      if (rootStorage->OpenPropertySet (sourceDescGuid, objectName, &imgPropSet, mode)) {
        imgPropSet->Revert();

        // If the 'creator' is 0, then this is the Source Description Object. If not,
        //  then creator is the index of the Transform Object and this object is the
        //  Result Description Object.
        if (imgPropSet->GetProperty (PID_Creator, &aProp)) {
          long  creatorIndex = (long)(*aProp);
          
          if (creatorIndex == 0) {
            sourceDescPropertySet = imgPropSet;
            sourceDescPropertyIndex = dataObjectIndx;
            
            // CHG_OBJ_FIX - set the transform and operation object indices to 0,
            //    since there are none for this image. This pervents a future
            //    problem where multiple images may exist and these objects
            //    do exist, but they're not for this particular image.
            transformPropertyIndex  = 0;
            operationPropertyIndex  = 0;
          }
          else {
            // Since there is a creator index, the object we opened is the
            //  result. The Source Description Objects index is found by using
            //  the creator as an index to the Transform Object that creates
            //  final image. The Transforms 'Input Data Object' will then
            //  indicate which Description object is the Source Dscr Object.
            // Core only supports one value in the Input Data Object array.
          
            resultDescPropertySet = imgPropSet;
            resultDescPropertyIndex = dataObjectIndx;
            
            GetTransformName (objectName, creatorIndex);
            GUID transformGuid = ID_Transform;
            
            if (rootStorage->OpenPropertySet (transformGuid, objectName, 
                                              &transformPropertySet, mode)) {
              transformPropertySet->Revert();

              if (GetTransformProperty (PID_InputObjectList, &aProp)) {
                FPXLongArray  inputDataList = (FPXLongArray)(*aProp);
                sourceDescPropertyIndex = inputDataList.ptr[0];
                transformPropertyIndex  = creatorIndex;
                
                if (GetTransformProperty (PID_OperationNumber, &aProp))
                  operationPropertyIndex  = (long)(*aProp);
              }
              else  err = TRUE;     // GetProperty failed
            } 
            else  err = TRUE;       // OpenPropertySet failed
          }
        }
        else  err = TRUE;           // GetSourceDescProperty failed
      }
      else  err = TRUE;             // OpenPropertySet failed
    }
    else  err = TRUE;               // GetGlobalInfoProperty failed
    

    // Source Description property set (required)
    // PTCH_OBJFIX - Don't need to open this if it was done earlier.
    if (sourceDescPropertySet == NULL) {
      GetImageDescName(sourceDescName,sourceDescPropertyIndex);
      GUID sourceDescGuid = ID_ImageDescription;
      if (rootStorage->OpenPropertySet(sourceDescGuid, sourceDescName, &sourceDescPropertySet, mode)) 
        sourceDescPropertySet->Revert();
      else
        err = TRUE;
    }

    // Result Description property set (optional)
    // PTCH_OBJFIX - Don't need to open this if it was done earlier.
    if (resultDescPropertySet == NULL) {
      GetImageDescName(resultDescName,resultDescPropertyIndex);
      GUID resultDescGuid = ID_ImageDescription;
      if (rootStorage->OpenPropertySet(resultDescGuid, resultDescName, &resultDescPropertySet, mode))
        resultDescPropertySet->Revert();
      else
        resultDescPropertySet = NULL;
    }

    // Operation property set (optional)
    GetOperationName(operationName,operationPropertyIndex);
    GUID operationGuid = ID_Operation;
    if (rootStorage->OpenPropertySet(operationGuid, operationName, &operationPropertySet, mode)) 
      operationPropertySet->Revert();
    else
      operationPropertySet = NULL;

    // Transformation property set (optional)
    // PTCH_OBJFIX - Don't need to open this if it was done earlier.
    if (transformPropertySet == NULL) {
      GetTransformName(transformName,transformPropertyIndex);
      GUID transformGuid = ID_Transform;
      if (rootStorage->OpenPropertySet(transformGuid, transformName, &transformPropertySet, mode)) 
        transformPropertySet->Revert();
      else
        transformPropertySet = NULL;
    }

    // Extension List property set (optional)
    GUID extensionGuid = ID_Extension;
    if (rootStorage->OpenPropertySet(extensionGuid, extensionName, &extensionListPropertySet, 
                                     mode)) 
      extensionListPropertySet->Revert();
    else
      extensionListPropertySet = NULL;
  }
  
  // Check if an error occured during the init
  if (err)
    SignaleErreur();
}


FPXStatus PFileFlashPixView::getStatus(void)
{
  FPXStatus status;
  status = oleFile->getFPXStatus(); 
  return status;
}


PFileFlashPixView::~PFileFlashPixView ()
{
  Boolean status;
  // CAUTION: Elements have to be deleted in the reverse order of their creation...
  
  // Commit() makes sure that everything has been written on the disk (flush
  // all ole buffers for the elements)
  if (mode != OLE_READ_ONLY_MODE)
    status = Commit();


  // Delete an element releases the OLE object.
  if (globalInfoPropertySet) {
    delete globalInfoPropertySet;
    globalInfoPropertySet = NULL; 
  }
  if (resultDescPropertySet) {
    delete resultDescPropertySet;
    resultDescPropertySet = NULL; 
  }
  if (sourceDescPropertySet) {
    delete sourceDescPropertySet;
    sourceDescPropertySet = NULL; 
  }
  if (operationPropertySet) {
    delete operationPropertySet;
    operationPropertySet = NULL;  
  }
  if (transformPropertySet) {
    delete transformPropertySet;
    transformPropertySet = NULL;  
  }
  if (summaryInfoPropertySet) {
    delete summaryInfoPropertySet;
    summaryInfoPropertySet = NULL;  
  }
  if (extensionListPropertySet) {
    delete extensionListPropertySet;
    extensionListPropertySet = NULL;  
  }

  // At last, release the file. CAUTION: the rootStorage is release by the file...
  // NOTE: The above comment has been changed, the rootStorage has to be released separately.
  // BEGIN: djk 052397 AI17850
  // Always release anything referenced by FPX! The root storage refers to the FPX root,
  // which may be anywhere within a DocFile.
  delete rootStorage;   
  rootStorage = NULL; 

  if (oleFile) {
  // END: djk 052397 AI17850
    delete oleFile;
    oleFile     = NULL; 
  }
}


Boolean PFileFlashPixView::CreateStorage(CLSID& classID, const char * name, OLEStorage ** res)
{
  if (rootStorage)
    return rootStorage->CreateStorage(classID, name, res);
  else
    return FALSE;
}


Boolean PFileFlashPixView::OpenStorage(CLSID& classID, const char * name, OLEStorage ** res)
{
  if (rootStorage)
    return rootStorage->OpenStorage(classID, name, res);
  else
    return FALSE;
}


FPXStatus PFileFlashPixView::GetError()
{
  return Erreur();
}


FPXStatus PFileFlashPixView::Erreur() const
{
  long    error = 0;
  FPXStatus status = FPX_OK;
  
  if (error == noErr) {
    if (summaryInfoPropertySet)
      error = summaryInfoPropertySet->Error();
    else
      error = -1;
  }
  
  if (error == noErr) {
    if (globalInfoPropertySet)
      error = globalInfoPropertySet->Error();
    else
      error = -1;
  }
    
  if (error == noErr) {
    if (resultDescPropertySet)
      error = resultDescPropertySet->Error();
    else
      error = -1;
  }
    
  if (error == noErr) {
    if (sourceDescPropertySet)
      error = sourceDescPropertySet->Error();
    else
      error = -1;
  }
    
  if (error == noErr) {
    if (operationPropertySet)
      error = operationPropertySet->Error();
    else
      error = -1;
  }
    
  if (error == noErr) {
    if (transformPropertySet)
      error = transformPropertySet->Error();
    else
      error = -1;
  }
    
  if (error == noErr) {
    if (extensionListPropertySet)
      error = extensionListPropertySet->Error();
    else
      error = -1;
  }
    
  if (error) {                  // If there is an error durng property set access
    status = oleFile->getFPXStatus();     //  get the most recent OLE file error
    if (status == FPX_OK)           // If there was no error saved in OLE, then
      status = FPX_FILE_READ_ERROR;     //  make a wild guess that it's a read error
  }
  return status;
}


Boolean PFileFlashPixView::Commit()
{
  Boolean ok = TRUE;
  if (summaryInfoPropertySet)
    ok &= summaryInfoPropertySet->Commit();
  if (globalInfoPropertySet)
    ok &= globalInfoPropertySet->Commit();
  if (resultDescPropertySet)
    ok &= resultDescPropertySet->Commit();
  if (sourceDescPropertySet)
    ok &= sourceDescPropertySet->Commit();
  if (operationPropertySet)
    ok &= operationPropertySet->Commit();
  if (transformPropertySet)
    ok &= transformPropertySet->Commit();
  if (extensionListPropertySet)
    ok &= extensionListPropertySet->Commit();
  if (rootStorage)
    ok &= rootStorage->Commit();
  return ok;
}

// This should probably go somewhere else
// FIXME:
#if   defined(_UNIX)

#define USERCLASSTYPE_APPNAME 0

HRESULT OleRegGetUserType(REFCLSID clsid, DWORD whatever, LPOLESTR FAR* pszUserType)
{
    if (*pszUserType == NULL) {
        *pszUserType = new OLECHAR[512];
    }
    // *pszUserType = OLESTR("Flashpix Toolkit Application"); FIXME
    wcscpy(*pszUserType,OLESTR("Flashpix Toolkit Application"));
    return S_OK;
}

HRESULT StringFromCLSID(REFCLSID clsid, LPOLESTR FAR* lplpsz)
{
   
    *lplpsz = new OLECHAR[512];
    char lpsz[512];
   
    WORD  Data4 = clsid.Data4[0]<<8 & clsid.Data4[1];
    WORD  Data5 = clsid.Data4[2]<<8 & clsid.Data4[3];  
    DWORD Data6 = clsid.Data4[4]<<24 & clsid.Data4[5]<<16 & clsid.Data4[6]<<8 & clsid.Data4[7];

    sprintf(lpsz, "{%08x-%04x-%04x-%04x-%04x%08x}", (unsigned int) clsid.Data1, clsid.Data2,
            clsid.Data3, Data4, Data5, (unsigned int) Data6);
    INIT_OLESTR(*lplpsz, lpsz);
    return S_OK;
}

#endif    

// Create CompObj in root storage 
Boolean PFileFlashPixView::CreateCompObj()
{
  OLEStream * curCompObj; 

  if (rootStorage->CreateStream("\1CompObj", &curCompObj)) {
  
    // Write the header 
    DWORD temp;
    temp = 0xfffe0001; 
    curCompObj->WriteVT_I4(&temp);
    
    temp = 0x00000a03; 
    curCompObj->WriteVT_I4(&temp);

    temp = (DWORD) -1; 
    curCompObj->WriteVT_I4(&temp);

    CLSID clsID = ID_ImageView;
    curCompObj->WriteVT_CLSID(&clsID);

    // Get new user type name; if error, set to NULL string
    OLECHAR chZero = 0;
    LPOLESTR lpszNew = NULL;
    if (OleRegGetUserType(clsID, USERCLASSTYPE_APPNAME, &lpszNew) != S_OK)
    {
      lpszNew = new OLECHAR[1];
      lpszNew[0]=chZero;
    }

#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPSTR_NoPad(lpszNew);
#elif _WINDOWS
    char str[1024];
    WideCharToMultiByte(CP_ACP, 0, lpszNew, -1, str, 128, NULL, NULL);
    curCompObj->WriteVT_LPSTR_NoPad(str);
#endif      

    // Get and write clsID string
    LPOLESTR FAR clsIDStr;      
    StringFromCLSID(clsID, &clsIDStr);
#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPSTR_NoPad(clsIDStr);
#elif _WINDOWS
    WideCharToMultiByte(CP_ACP, 0, clsIDStr, -1, str, 128, NULL, NULL);
    curCompObj->WriteVT_LPSTR_NoPad(str);
#endif

    // Write prog id ( for now just write length = 0 )
#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPSTR_NoPad(&chZero);
#elif _WINDOWS
    char progID = 0; 
    curCompObj->WriteVT_LPSTR_NoPad(&progID);
#endif      
    
    // Write magic number
    temp = 0x71B239F4; 
    curCompObj->WriteVT_I4(&temp);
    
    // Write unicode user type ( for now just write length = 0 )
#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(lpszNew));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(lpszNew);
#endif      
    
    // Write unicode clipboard format 
#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(&chZero));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(&chZero);
#endif      
    
    // Write unicode prog id ( for now just write length = 0 )
#if defined(macintosh) || defined(__unix)
    curCompObj->WriteVT_LPWSTR_NoPad(MultiByteToWideChar(&chZero));
#elif _WINDOWS
    curCompObj->WriteVT_LPWSTR_NoPad(&chZero);
#endif    

    // We may need to call some other memory methods.
    delete[] lpszNew;  // FIXME: Crashes here with assertion error.
  } 
  else
    return FALSE; 
    
  return TRUE; 
}


// Read CompObj in root storage 
Boolean PFileFlashPixView::ReadCompObj(char **ppUserType, char **ppClipFormat)
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
Boolean PFileFlashPixView::WriteCompObj(char *pUserType, char *pClipFormat)
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


// Get property in summary info property set
Boolean PFileFlashPixView::GetSummaryInfoProperty (DWORD pID, OLEProperty ** res)
{
  return summaryInfoPropertySet->GetProperty(pID, res);
}

// Set property in summary info property set
Boolean PFileFlashPixView::SetSummaryInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  return summaryInfoPropertySet->NewProperty(pID, propType, res);
}


// Get property in global info property set
Boolean PFileFlashPixView::GetGlobalInfoProperty (DWORD pID, OLEProperty ** res)
{
  return globalInfoPropertySet->GetProperty(pID, res);
}


// Set property in global info property set
Boolean PFileFlashPixView::SetGlobalInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  return globalInfoPropertySet->NewProperty(pID, propType, res);
}


// Get property in result description property set
Boolean PFileFlashPixView::GetResultDescProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = TRUE;
  
  // If absent, there is an error
  if (resultDescPropertySet == NULL)
    ok = FALSE;
  
  // Get the property
  if (ok)
    ok = resultDescPropertySet->GetProperty(pID, res);
    
  return ok;
}


// Set property in result description property set
Boolean PFileFlashPixView::SetResultDescProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = TRUE;
  char resultDescName [33];
  GetImageDescName(resultDescName,2);   // Baseline limitation. See VIII.2.1.
  
  // If absent, the Result Description property set has to be created
  if (resultDescPropertySet == NULL) {
    GUID resultDescGuid = ID_ImageDescription;
    if ( !rootStorage->CreatePropertySet(resultDescGuid, resultDescName, &resultDescPropertySet))     
      ok = FALSE;
  }
  
  // Add the property to the section
  if (ok)
    ok = resultDescPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}


// Get property in source description property set
Boolean PFileFlashPixView::GetSourceDescProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = TRUE;
  
  // If absent, there is an error
  if (sourceDescPropertySet == NULL)
    ok = FALSE;
  
  // Get the property
  if (ok)
    ok = sourceDescPropertySet->GetProperty(pID, res);
    
  return ok;
}


// Set property in source description property set
Boolean PFileFlashPixView::SetSourceDescProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = TRUE;
  char sourceDescName [33];
  GetImageDescName(sourceDescName,1);   // Baseline limitation. See VIII.2.1.
  
  // If absent, the Source Description property set has to be created
  if (sourceDescPropertySet == NULL) {
    GUID sourceDescGuid = ID_ImageDescription;
    if ( !rootStorage->CreatePropertySet(sourceDescGuid, sourceDescName, &sourceDescPropertySet)) 
      ok = FALSE;
  }
  
  // Add the property to the section
  if (ok)
    ok = sourceDescPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}


// Get property in operation property set
Boolean PFileFlashPixView::GetOperationProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = TRUE;
  
  // If absent, there is an error
  if (operationPropertySet == NULL)
    ok = FALSE;
  
  // Get the property
  if (ok)
    ok = operationPropertySet->GetProperty(pID, res);
    
  return ok;
}


// Set property in operation property set
Boolean PFileFlashPixView::SetOperationProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = TRUE;
  char operationName [33];
  GetOperationName(operationName,1);    // Baseline limitation. See VIII.2.1.
  
  // If absent, the Operation property set has to be created
  if (operationPropertySet == NULL) {
    GUID operationGuid = ID_Operation;
    if ( !rootStorage->CreatePropertySet(operationGuid, operationName, &operationPropertySet)) 
      ok = FALSE;
  }
  
  // Add the property to the section
  if (ok)
    ok = operationPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}

// Get property in transform property set
Boolean PFileFlashPixView::GetTransformProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = TRUE;
  
  // If absent, there is an error
  if (transformPropertySet == NULL)
    ok = FALSE;
  
  // Get the property
  if (ok)
    ok = transformPropertySet->GetProperty(pID, res);
    
  return ok;
}


// Set property in transform property set
Boolean PFileFlashPixView::SetTransformProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = TRUE;
  char transformName [33];
  GetTransformName(transformName,1);    // Baseline limitation. See VIII.2.1.
  
  // If absent, the Transformation property set has to be created
  if (transformPropertySet == NULL) {
    GUID transformGuid = ID_Transform;
    if ( !rootStorage->CreatePropertySet(transformGuid, transformName, &transformPropertySet)) 
      ok = FALSE;
  }
  
  // Add the property to the section
  if (ok)
    ok = transformPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}

// Get property in extension property set
Boolean PFileFlashPixView::GetExtensionProperty (DWORD pID, OLEProperty ** res)
{
  Boolean ok = TRUE;
  
  // If absent, there is an error
  if (extensionListPropertySet == NULL)
    ok = FALSE;
  
  // Get the property
  if (ok)
    ok = extensionListPropertySet->GetProperty(pID, res);
    
  return ok;
}


// Set property in extension property set
Boolean PFileFlashPixView::SetExtensionProperty (DWORD pID, DWORD propType, OLEProperty ** res)
{
  Boolean ok = TRUE;
  char extensionName [33];
  GetExtensionName(extensionName);
  
  // If absent, the Extension property set has to be created
  if (extensionListPropertySet == NULL) {
    GUID extensionGuid = ID_Extension;
    if ( !rootStorage->CreatePropertySet(extensionGuid, extensionName, &extensionListPropertySet)) 
      ok = FALSE;
  }
  
  // Add the property to the section
  if (ok)
    ok = extensionListPropertySet->NewProperty(pID, propType, res);
    
  return ok;
}


// Get extension number for specified extension name
Boolean PFileFlashPixView::GetExtensionNumber (LPWSTR theExtensionName, short *extNumber)
{
  char      extensionName [33];
  OLEProperty*  aProp;
  FPXWideStr    targetWStr;
  
  // Make a wide string descriptor for the ext name we're looking for
  targetWStr.length = FPX_LPWSTRlen( theExtensionName);
  targetWStr.ptr = theExtensionName;
  
  GetExtensionName(extensionName);
  
  // If absent, the extension property set has to be created
  if (extensionListPropertySet == NULL) {
    GUID extensionGuid = ID_Extension;
    if ( !rootStorage->CreatePropertySet(extensionGuid, extensionName, &extensionListPropertySet)) 
      return FALSE;
  }
  
  // If extension list array does't exist in Extension property set, create one with no entry
  if ( !GetExtensionProperty (PID_UsedExtensionNumber, &aProp) ) {
  
    fpxExtensionList.length = 0;
    fpxExtensionList.ptr  = NULL;
    if ( SetExtensionProperty (PID_UsedExtensionNumber, TYP_UsedExtensionNumber, &aProp) ) 
      *aProp = fpxExtensionList; 
  }
  else {
    fpxExtensionList = (FPXShortArray)(*aProp); 
  
    // Compare the extension name with each of ones in extension list array
    for ( long i = 0; i < (long) fpxExtensionList.length; i++ )
      if ( GetExtensionProperty (PID_ExtensionName(fpxExtensionList.ptr[i]), &aProp) ) {
      
        // Get extension name stored in property 
        LPWSTR    extName = *aProp; 
        FPXWideStr  thisWStr;
        
        // Make a wide string descriptor for ext prop in file
        thisWStr.length = FPX_LPWSTRlen( extName);
        thisWStr.ptr = extName;
        
        // Compare the name with specified one
        if ( !FPX_WideStrcmp(&targetWStr, &thisWStr) ) {
        
          // Return the extension number in extension list array
          *extNumber = fpxExtensionList.ptr[i]; 
          return TRUE; 
        }
      }
  }
      
  // Since no extension number is found, return false
  return FALSE;
}


// Set a new extension number
Boolean PFileFlashPixView::SetExtensionNumber (short *extNumber)
{
  char extensionName [33];
  OLEProperty* aProp;
  
  GetExtensionName(extensionName);
  
  // If absent, the Extension property set has to be created
  if (extensionListPropertySet == NULL) {
    GUID extensionGuid = ID_Extension;
    if ( !rootStorage->CreatePropertySet(extensionGuid, extensionName, &extensionListPropertySet)) 
      return FALSE;
  }
  
  // If extension list array does't exist in Extension property set, create one
  if ( !GetExtensionProperty (PID_UsedExtensionNumber, &aProp) ) {
  
    fpxExtensionList.length = 0;
    fpxExtensionList.ptr  = NULL;
    
    // Reallocate the extension list array and array count ( currently the extension is same as array index)
    if ( Renew((short **)&fpxExtensionList.ptr, 0, (short)++fpxExtensionList.length) )  
      if ( SetExtensionProperty (PID_UsedExtensionNumber, TYP_UsedExtensionNumber, &aProp) ) {
        *aProp = fpxExtensionList; 
        *extNumber = 0; 
      }
      else 
        return FALSE; 
    else 
      return FALSE; 
  }
  else {
    
    // Reallocate the extension list array and array count ( currently the extension is same as array index)
    if ( Renew((short **)&fpxExtensionList.ptr, (short) fpxExtensionList.length, (short) fpxExtensionList.length + 1) )  
      if ( SetExtensionProperty (PID_UsedExtensionNumber, TYP_UsedExtensionNumber, &aProp) ) {
      
        // Get the new extension number
        *extNumber = fpxExtensionList.ptr[fpxExtensionList.length]; 
        
        // Set the new extension list to property set
        ++fpxExtensionList.length; 
        *aProp = fpxExtensionList;        
      }
      else  
        return FALSE;
    else  
      return FALSE;
  }
      
  return TRUE;
}

// Reallocate the array and keep the content in old array
Boolean PFileFlashPixView::Renew( short **ppa, short newEntry, short newSize)
{
  // If array already exist, delete it first
  if ( *ppa )
  {
    // Allocate a temporary array with new size
    short*  temp = new short [newSize]; 
    if (!temp)    return FALSE; 
    
    // Transfer the data in old array into new one
    for ( long i = 0; i< newSize-1; i++ )   
      temp[i] = *ppa[i]; 
      
    // Save the new number in last entry of array
    temp[newSize - 1] = newEntry;
    
    // Delete the old array and assign the temporary one
    delete *ppa;  
    *ppa = temp; 
  }
  else
  {
    // Allocate a new array with new size
    *ppa = new short [newSize];   
    if (!*ppa)    return FALSE; 

    // Save the new number in last entry of array
    *ppa[newSize - 1] = newEntry;
  } 

  return TRUE;
}


// Initialize the summary info property set
Boolean  PFileFlashPixView::InitSummaryInfoPropertySet()
{
  OLEProperty* aProp;
  Boolean err = FALSE;
  FILETIME ft; 
  
  if (summaryInfoPropertySet->GetProperty (PROPERTYIDONE, &aProp))  // Change the code page to 1252
    *aProp = (short)(1252);
  else if (summaryInfoPropertySet->NewProperty (PROPERTYIDONE, VT_I2, &aProp))
    *aProp = (short)(1252);
  else 
    err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(10), VT_FILETIME, &aProp)) // edit time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(11), VT_FILETIME, &aProp)) // last printed
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(12), VT_FILETIME, &aProp)) // create date and time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(13), VT_FILETIME, &aProp)) // last save date and time
  {
    FPXUpdateTime((FPXfiletime *)&ft); 
    *aProp = FILETIME(ft);
  }
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(14), VT_I4, &aProp))   // page count
    *aProp = long(0);
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(15), VT_I4, &aProp))   // word count
    *aProp = long(0);
  else err = TRUE;
  
  if(summaryInfoPropertySet->NewProperty (long(16), VT_I4, &aProp))   // char count
    *aProp = long(0);
  else err = TRUE;
  
  if (!err)
    return summaryInfoPropertySet->Commit();
  else
    return FALSE;
}


Boolean  PFileFlashPixView::GetSummaryInfoPropertySet()
{
  OLEProperty*  aProp;
  Boolean     err = FALSE;
  FILETIME    ft;
    
  if (summaryInfoPropertySet->GetProperty (long(10), &aProp))   // edit time
    ft = FILETIME(*aProp);
  else err = TRUE;
  
  if (summaryInfoPropertySet->GetProperty (long(11), &aProp))   // last printed
    ft = FILETIME(*aProp);
  else err = TRUE;
  
  if (summaryInfoPropertySet->GetProperty (long(12), &aProp))   // create date and time
    ft = FILETIME(*aProp);
  else err = TRUE;
  
  if (summaryInfoPropertySet->GetProperty (long(13), &aProp))   // last save date and time
    ft = FILETIME(*aProp);
  else err = TRUE;
  
  return !err;
}

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
