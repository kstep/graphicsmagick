//  ----------------------------------------------------------------------------
//  MODULE    : OLECustomLink 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, May 26, 1996 
//  DESCRIPTION :  
//  COMMENTS  : Most of the inspirationnal code concerning the monikers can be
//        found in OLE 2.07:SDK:Sample Code (OLE):Outline:Source:doccntr.c
//  SCCSID      : @(#)oleclink.cpp  1.1 11:53:27 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "oleclink.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  <stdio.h>
  #include  <stdlib.h>

#if defined(USE_LEGACY_INCLUDES)
#  include <iostream.h>
#  include <fstream.h>
#else
#  include <iostream>
#  include <fstream>
   using namespace std;
#endif

#ifdef macintosh
  #include  <coguid.h>
#endif

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif
#ifndef OLEFiles_h
  #include  "olefiles.h"
#endif

//  Constants
//  ---------

  // Copied from NIFTYUtils.h to avoid a link from the OLE folder to the FlashPix folder
  
  #define StreamPrefix        '\005'
  #define FMT_CustomLink        "%cCustom Link"

  // Custom Monikers
  #define ID_LocalFileMoniker ID_FlashPixSuffix(0x00030000)
  #define ID_URLMoniker   ID_FlashPixSuffix(0x00030100)

  // Custom Monikers Property Set
  #define PID_OLEMoniker      0x00010000
  #define PID_AbsoluteFilePath  0x00020001
  #define PID_RelativeFilePath  0x00020002
  #define PID_AbsoluteStoragePath 0x00020003
  #define PID_PhysicalVolumeName  0x00020004
  #define PID_VirtualVolumeName 0x00020005
  #define PID_LastMountPoint    0x00020006
//  #define PID_Connection      0x00020103
//  #define PID_HostAddress     0x00020104
  
  #define TYP_OLEMoniker      VT_BLOB
  #define TYP_AbsoluteFilePath  VT_LPWSTR
  #define TYP_RelativeFilePath  VT_LPWSTR
  #define TYP_AbsoluteStoragePath VT_LPWSTR
  #define TYP_PhysicalVolumeName  VT_LPWSTR
  #define TYP_VirtualVolumeName VT_LPWSTR
  #define TYP_LastMountPoint    VT_LPWSTR
//  #define TYP_Connection      VT_LPWSTR
//  #define TYP_HostAddress     VT_LPWSTR

#define POINT '.'
#define SLASH '/'

//  Variables
//  ---------

  // Custom Link Property Set Descriptor
  typedef struct FPXCustomLink {

                FPXbool         linkedIStorageIsValid;
                IStorage*             linkedIStorage;
                
                FPXbool         absoluteFilePathIsValid;
                FPXWideStr              absoluteFilePath;
                
                FPXbool         relativeFilePathIsValid;
                FPXWideStr              relativeFilePath;
                
                FPXbool         absoluteStoragePathIsValid;
                FPXWideStr              absoluteStoragePath;
                
                FPXbool         physicalVolumeNameIsValid;
                FPXWideStr              physicalVolumeName;
                
                FPXbool         virtualVolumeNameIsValid;
                FPXWideStr              virtualVolumeName;
                
                FPXbool         lastMountPointIsValid;
                FPXWideStr              lastMountPoint;
                
  } FPXCustomLink;

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

static void ByteSwapCLSID (CLSID* clsid)
{
#ifdef  IN_BIG_ENDIAN
  SwapBytes((BYTE *)(&(clsid->Data1)), sizeof(unsigned long));  
  SwapBytes((BYTE *)(&(clsid->Data2)), sizeof(unsigned short));   
  SwapBytes((BYTE *)(&(clsid->Data3)), sizeof(unsigned short));   
#endif
}

static void GetCustomLinkName  (char* buffer)
{
  if (buffer)
    sprintf (buffer, FMT_CustomLink, StreamPrefix);
}

static Boolean PatternUp (char* stdName)
// Return TRUE if the stdName starts with a '../' sequence
{
  if (*stdName++ != POINT)
    return FALSE;
  else if (*stdName++ != POINT)
    return FALSE;
  else if (*stdName++ != SLASH)
    return FALSE;
  return TRUE;
}

static Boolean ExtractStorageName (char** namePtr, unsigned long* length, char* stdName)
{
  if (*length) {
    char *ptr = *namePtr;
    // Extract the current storage name
    while ((*ptr != SLASH) && (*length)) {
      *stdName++ = (char) *ptr++;
      (*length)--;
    }
    // Skip the next slash character and point to the next storage name
    if ((*ptr == SLASH) && (*length)) {
      ptr++;
      (*length)--;
    }
    *namePtr = ptr;
    return TRUE;
  } else
    return FALSE;
}

static OLEStorage* AnalyseStorageUp (char** namePtr, unsigned long* length, OLEStorage* currStorage)
{
  if (*length) {
    char *ptr = *namePtr;
    OLEStorage* tempStorage = NULL;
    if (*ptr == SLASH) {
      // This is an absolute path -> Go to the top of the storage tree
      tempStorage = currStorage->GetParStorage();
      while (tempStorage) {
        currStorage = tempStorage;
        tempStorage = currStorage->GetParStorage();
      }
      ptr++;
      (*length)--;
    } else {
      // Analyse the "previous storage" patterns
      while (PatternUp(ptr)) {
        tempStorage = currStorage->GetParStorage();
        currStorage = tempStorage;
        ptr       += 3;
        (*length) -= 3;
      }
    }
    // Reassign the name pointer to the current pointed character (i.e. the beginning of a storage name)
    *namePtr = ptr;
  }
  // Return the found storage
  return currStorage;
}

// Normalize a file name according to the characteristics of the platform
// CAUTION: this is a very straightforwad implementation of the FlashPix spec,
// particularly, we do not handle special character cases as '/' on a Mac (not
// a delimiter) or ":" on Windows (not a delimiter). The legal character set is
// supposed to be the intersection of all legal character sets...
#ifdef macintosh
static void NormalizeFileName (char* name, char* normName)
{
  char* stdName = name;
  unsigned long remains = strlen(name);
    
  // Solve the pattern up list
  if (PatternUp(stdName))
    *normName++ = ':';      // On Mac, pattern up starts with 2 semicolons...
  while (PatternUp(stdName)) {
    *normName++ = ':';      // ...and then add one more for erach step up.
    stdName += 3;
    remains -= 3;
  }
  
  // Copy the rest of the name
  while (remains) {
    if (*stdName == SLASH) 
      *normName = ':';
    else
      *normName = *stdName;
    normName++;
    stdName++;
    remains--;
  }
  
  // End with a '\0'
  *normName = '\0';
}
#else // WINDOWS
static void NormalizeFileName (char* name, char* normName)
{
  char* stdName = name;
  unsigned long remains = strlen(name);
  
  // Solve the pattern up list
  while (PatternUp(stdName)) {
    *normName++ = '.';
    *normName++ = '.';
    *normName++ = '\\';
    stdName += 3;
    remains -= 3;
  }
  
  // Copy the rest of the name
  while (remains) {
    if (*stdName == SLASH) 
      *normName = '\\';
    else
      *normName = (char) *stdName;
    normName++;
    stdName++;
    remains--;
  }
  
  // End with a '\0'
  *normName = '\0';
}
// UNIX IS NOT PROVIDED AT THIS POINT... (easy though...)
#endif

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

OLECustomLink::OLECustomLink(OLEStorage* initialStorage)
{
  // Init linked pointers
  linkedStorage = NULL;
  linkedFile    = NULL;
  
  // Allocate local variables
  FPXCustomLink   customLinkDescriptor;       // The structure containing the properties
  OLEPropertySet*   customLinkPropertySet = NULL;   // Custom Link property set handle

  // Name of the Custom Link property set
  char customLinkName [33];
  GetCustomLinkName(customLinkName);

  // Open and read the Custom Link property set
  GUID customLinkGuid = ID_LocalFileMoniker;
  if (initialStorage->OpenPropertySet (customLinkGuid, customLinkName, &customLinkPropertySet, OLE_READWRITE_MODE)) {
    if (customLinkPropertySet->Revert()) {
      // Read the data from the property set (update the property set record and resolve the OLE Moniker if any)
      GetCustomLinkPropertySet (customLinkPropertySet, &customLinkDescriptor);
    } else {
      // If error when reading, delete the property set record
      delete customLinkPropertySet;
      customLinkPropertySet = NULL;
    }
  }

  // Resolve the link if the reading has been successful
  if (customLinkPropertySet)
    ResolveLink(initialStorage,customLinkDescriptor);

  // Delete the property set (useless from that point...)
  if (customLinkPropertySet)
    delete customLinkPropertySet;
}

OLECustomLink::~OLECustomLink()
{
  // CAUTION: delete the storage before the file which contains it...
  if (linkedStorage)
    delete linkedStorage;
  if (linkedFile)
    delete linkedFile;

}

// Read the Custom Link property set (straightforward...)

Boolean OLECustomLink::GetCustomLinkPropertySet (OLEPropertySet* customLinkSet, FPXCustomLink* customLinkDescriptor)
{
#if defined(_UNIX)
  // TODO: put moniker support into oless for UNIX (if needed??)
  return false;
#else

  OLEProperty* aProp;
  Boolean ok = true;
  
  if (customLinkSet->GetProperty (PID_OLEMoniker, &aProp)) {
  
    // Try to read and resolve the OLE Moniker
    // ---------------------------------------
    // Nothing is really done in OLE to read and write an OLE moniker in a Blob, so the following
    // code is tricky and debatable (as a solution...). Therefore, it's heavily commented...
    
    // Declare a Blob and use the property pointer to set it
    OLEBlob linkBlob((BLOB*)(*aProp));
    
    // If the Blob size is not NULL...
    if (linkBlob.GetBlobSize() > 0) {
    
      // Declare a pointer to a simple stream of bytes and read the Blob into it.
      // This simply makes tmp points to the stream of byte, skipping the Blob specific stuff (size and others if any)
      BYTE *tmp;
      linkBlob.ReadVT_VECTOR(&tmp);

      // The trick is to write this Blob in a temporary stream and to do as if it was a moniker really written by OLE
      // (I know it's ugly, but it's the sucky FlashPix spec...)
        IStream*  lpStream;
        HRESULT   hrErr = NOERROR;
        
      // Create the temporary stream
          CreateStreamOnHGlobal(NULL, TRUE, &lpStream);
          // Write the monikers info in this stream
          if (hrErr == NOERROR)
            hrErr = lpStream->Write(tmp, linkBlob.GetBlobSize(), NULL);
          // Resolve the OLE moniker
          if (hrErr == NOERROR)
            hrErr = BindMonikerToStorage(lpStream, &customLinkDescriptor->linkedIStorage);
      // Delete the temporary stream
          lpStream->Release();

      // If it worked, use the moniker, else, we're gonna have to parse the names (see ResolveLink())
      if ((hrErr != NOERROR) && (customLinkDescriptor->linkedIStorage != NULL))
        customLinkDescriptor->linkedIStorageIsValid = TRUE;
      else
        customLinkDescriptor->linkedIStorageIsValid = FALSE;
    } else
      customLinkDescriptor->linkedIStorageIsValid = FALSE;
  } else
    customLinkDescriptor->linkedIStorageIsValid = FALSE;

  // Read the explicit paths information (No interpretation. See ResolveLink() for that)
  // -----------------------------------------------------------------------------------
  
  if (customLinkSet->GetProperty (PID_AbsoluteFilePath, &aProp)) {
    customLinkDescriptor->absoluteFilePath = (FPXWideStr)(*aProp);
    customLinkDescriptor->absoluteFilePathIsValid = TRUE;
  } else
    customLinkDescriptor->absoluteFilePathIsValid = FALSE;
  
  if (customLinkSet->GetProperty (PID_RelativeFilePath, &aProp)) {
    customLinkDescriptor->relativeFilePath = (FPXWideStr)(*aProp);
    customLinkDescriptor->relativeFilePathIsValid = TRUE;
  } else
    customLinkDescriptor->relativeFilePathIsValid = FALSE;
  
  if (customLinkSet->GetProperty (PID_AbsoluteStoragePath, &aProp)) {
    customLinkDescriptor->absoluteStoragePath = (FPXWideStr)(*aProp);
    customLinkDescriptor->absoluteStoragePathIsValid = TRUE;
  } else
    customLinkDescriptor->absoluteStoragePathIsValid = FALSE;
  
  if (customLinkSet->GetProperty (PID_PhysicalVolumeName, &aProp)) {
    customLinkDescriptor->physicalVolumeName = (FPXWideStr)(*aProp);
    customLinkDescriptor->physicalVolumeNameIsValid = TRUE;
  } else
    customLinkDescriptor->physicalVolumeNameIsValid = FALSE;
  
  if (customLinkSet->GetProperty (PID_VirtualVolumeName, &aProp)) {
    customLinkDescriptor->virtualVolumeName = (FPXWideStr)(*aProp);
    customLinkDescriptor->virtualVolumeNameIsValid = TRUE;
  } else
    customLinkDescriptor->virtualVolumeNameIsValid = FALSE;
  
  if (customLinkSet->GetProperty (PID_LastMountPoint, &aProp)) {
    customLinkDescriptor->lastMountPoint = (FPXWideStr)(*aProp);
    customLinkDescriptor->lastMountPointIsValid = TRUE;
  } else
    customLinkDescriptor->lastMountPointIsValid = FALSE;
  
  return ok;
#endif
}

HRESULT OLECustomLink::BindMonikerToStorage (IStream* lpStream, IStorage** pStg)
{
#if defined(_UNIX)
        return false;
        // TODO: put Moniker support in for UNIX (if needed??)
#else 
    IMoniker *pmk = NULL;
    HRESULT status;
    IBindCtx *pbc = NULL;
    LARGE_INTEGER offset;
    CLSID clsid;
    BIND_OPTS bindopts;

    *pStg  = NULL;

    status = CreateBindCtx(0, &pbc);
    bindopts.cbStruct = sizeof(BIND_OPTS);
    status = pbc->GetBindOptions(&bindopts);
    bindopts.grfMode  = STGM_CREATE | STGM_DELETEONRELEASE;
    bindopts.grfFlags = 0;
    status = pbc->SetBindOptions(&bindopts);

    offset.LowPart  = 0;
    offset.HighPart = 0;
    status = lpStream->Seek(offset, STREAM_SEEK_SET, NULL);
    lpStream->Read(&clsid, sizeof(CLSID), NULL);
    ByteSwapCLSID(&clsid);

  // Sorry: the macro IID_IMoniker defined in coguid.h doesn't link...
  IID imn = {0x0000000fL, 0, 0, {0xC0,0,0,0,0,0,0,0x46}};
    status = CoCreateInstance(clsid, NULL, CLSCTX_SERVER, imn, (void **)&pmk); /* this line is ok */

    status = pmk->Load(lpStream);

  // Sorry: the macro IID_IStorage defined in coguid.h doesn't link...
  IID ist = {0x0000000bL, 0, 0, {0xC0,0,0,0,0,0,0,0x46}};
    status = pmk->BindToStorage(pbc, NULL, ist, (void **)pStg);

    pbc->Release();
    pmk->Release();

    return status;
#endif
}

// CAUTION : don't call this method if customLinkDescriptor is not properly updated
Boolean OLECustomLink::ResolveLink (OLEStorage* initialStorage, const FPXCustomLink& customLinkDescriptor)
{
  // Try to resolve the OLE moniker first
  // ------------------------------------

  if (customLinkDescriptor.linkedIStorageIsValid)
    linkedStorage = new OLEStorage (initialStorage, customLinkDescriptor.linkedIStorage);

  // If failed, parse the name: in that case the storage path is required (see spec III.4.1.1)
  // --------------------------
  
  if ((linkedStorage == NULL) && customLinkDescriptor.absoluteStoragePathIsValid) {
  
    // Allocate some temporary arrays and variables
    char fileName[255];
    CLSID id;
    OLEStorage* currStorage = NULL;
    Boolean ok;

    // Analyse the relative file path first
    if (customLinkDescriptor.relativeFilePathIsValid) {
    
      // Create a relative file path conform to the platform constraints
      char* namePtr = 0;
      
#ifdef _WINDOWS 
      // Convert the wide string into a normal char string
      WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(customLinkDescriptor.relativeFilePath.ptr), -1, namePtr, 128, NULL, NULL);
#else
      // Convert the wide string into a normal char string
      namePtr = WideCharToMultiByte((WCHAR *)(customLinkDescriptor.relativeFilePath.ptr));
#endif
      NormalizeFileName (namePtr,fileName);
      delete [] namePtr;
      
      // Create the OLEFile and open its root storage
#ifdef _WINDOWS 
      linkedFile = new OLEFile (fileName);
#else
      // On Mac, we have to copy the directory and the volume of the original file
      FicNom relativeName(fileName);
      FicNom initialName;
      initialStorage->GetFileName(&initialName);
      relativeName.directory = initialName.directory;
      relativeName.volume    = initialName.volume;
      
      linkedFile = new OLEFile (relativeName);
#endif
      ok = linkedFile->OpenOLEFile(id,&currStorage,OLE_READWRITE_MODE);
      
      if (ok)
        // Parse the storage path. The result can be NULL in case of an internal error.
        linkedStorage = ParseStoragePath (currStorage, customLinkDescriptor);

      // Close the file if we failed to open the sub storages (whatever happened...)
      if (linkedStorage == NULL) {
        delete linkedFile;
        linkedFile = NULL;
      }
    }
    
    // If failed, analyse the absolute file path
    if ((linkedStorage == NULL) && customLinkDescriptor.absoluteFilePathIsValid) {
    
      char volumeName[255];
      volumeName[0] = '\0';

      // Extract the volume name (or equivalent)
      if (customLinkDescriptor.physicalVolumeNameIsValid) {
        char* namePtr = 0;
#ifdef _WINDOWS 
        // Convert the wide string into a normal char string
        WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(customLinkDescriptor.physicalVolumeName.ptr), -1, namePtr, 128, NULL, NULL);
#else
        // Convert the wide string into a normal char string
        namePtr = WideCharToMultiByte((WCHAR *)(customLinkDescriptor.physicalVolumeName.ptr));
#endif
        NormalizeFileName (namePtr,volumeName);
        delete [] namePtr;
      } else if (customLinkDescriptor.virtualVolumeNameIsValid) {
        char* namePtr = 0;
#ifdef _WINDOWS 
        // Convert the wide string into a normal char string
        WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(customLinkDescriptor.virtualVolumeName.ptr), -1, namePtr, 128, NULL, NULL);
#else
        // Convert the wide string into a normal char string
        namePtr = WideCharToMultiByte((WCHAR *)(customLinkDescriptor.virtualVolumeName.ptr));
#endif
        NormalizeFileName (namePtr,volumeName);
        delete [] namePtr;
      } else if (customLinkDescriptor.lastMountPointIsValid) {
        // This is only useful on UNIX machines... 
        // Not implemented in this version of the Reference Implementation
      }
      
      // Create the absolute file path conform to the platform constraints
      char* namePtr = 0;
#ifdef _WINDOWS 
      // Convert the wide string into a normal char string
      WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(customLinkDescriptor.absoluteFilePath.ptr), -1, namePtr, 128, NULL, NULL);
#else
      // Convert the wide string into a normal char string
      namePtr = WideCharToMultiByte((WCHAR *)(customLinkDescriptor.absoluteFilePath.ptr));
#endif
      NormalizeFileName (namePtr,fileName);
      delete [] namePtr;
      strcat(volumeName,fileName);
      
      // Create the OLEFile and open its root storage
      linkedFile = new OLEFile (volumeName);
      ok = linkedFile->OpenOLEFile(id,&currStorage,OLE_READWRITE_MODE);
      
      if (ok)
        // Parse the storage path. The result can be NULL in case of an internal error.
        linkedStorage = ParseStoragePath (currStorage, customLinkDescriptor);

      // Close the file if we failed to open the sub storages
      if (linkedStorage == NULL) {
        delete linkedFile;
        linkedFile = NULL;
      }
    }

    // If failed, consider the storage path as local to the file
    if (linkedStorage == NULL)
      // Parse the storage path. The result can be NULL in case of an internal error.
      linkedStorage = ParseStoragePath (initialStorage, customLinkDescriptor);
  }
  
  // Return TRUE if success
  return (linkedStorage != NULL);
}


// Parse the storage path
OLEStorage* OLECustomLink::ParseStoragePath (OLEStorage* initialStorage, const FPXCustomLink& customLinkDescriptor)
{
  // Init local variables
  Boolean ok;
  CLSID id;
  char storageName[255];
  OLEStorage* currStorage = NULL;
  OLEStorage* tempStorage = NULL;
  unsigned long   length  = customLinkDescriptor.absoluteStoragePath.length;

  // Convert to 8 bits character set first
  char* fullName = 0;
#ifdef _WINDOWS 
  // Convert the wide string into a normal char string
  WideCharToMultiByte(CP_ACP, 0, (WCHAR *)(customLinkDescriptor.absoluteStoragePath.ptr), -1, fullName, 128, NULL, NULL);
#else
  // Convert the wide string into a normal char string
  fullName = WideCharToMultiByte((WCHAR *)(customLinkDescriptor.absoluteStoragePath.ptr));
#endif
  char* namePtr = fullName;
    
  // Analyse the up part and get the real initial storage
  initialStorage = AnalyseStorageUp (&namePtr, &length, initialStorage);
  
  // Analyse the substorage list: for the first one, we must not close the storage because it belongs
  // to the opened file (root or somewhere in the storage list)
  ExtractStorageName (&namePtr, &length, storageName);
  ok = initialStorage->OpenStorage (id, storageName, &currStorage, OLE_READWRITE_MODE);

  if (ok) {
    // Continue for each sub storage: close the door behind you each time you open a new one
    while (ExtractStorageName (&namePtr, &length, storageName)) {
      // Open the storage inside the current one
      ok = currStorage->OpenStorage (id, storageName, &tempStorage, OLE_READWRITE_MODE);
      delete currStorage; // The current storage is now useless
      currStorage = NULL; // Reinit to avoid bad assignment of linkedStorage
      if (ok)
        currStorage = tempStorage;
      else
        break;
    }
    // Assign the found value to the linkedStorage. Can be NULL in case of a previous error.
    linkedStorage = currStorage;
  }
  delete [] fullName;
  return currStorage;
}
      

IStorage* OLECustomLink::GetLinkedIStorage()
{
  if (linkedStorage)
    return linkedStorage->oleStorage;
  else
    return NULL;
}

  
//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------
/*

Moniker's creation : not implemented yet

void CreateStorageMoniker (IStorage *pRoot, CString *storageName, PString *fileName, CString *storagePath, FSSpec *pSpec)
{
    NIFStorage monikerStorage;
    NIFPropertyStream monikerStream;
    HRESULT value;
    IMoniker *ppmk;
    IStream *lpStream;
    Str255 string;
    UBIT8 *OLEmoniker;
    STATSTG statStg;
    LARGE_INTEGER offset;
    CLSID clsid;

    value = monikerStorage.CreateStorage(pRoot, storageName, STGM_CREATE | STGM_SHARE_EXCLUSIVE | STGM_READWRITE);

  if (value == S_OK) {
        monikerStorage.SetClass(NIF_STORAGE_LINK_CLSID); // add CLSID of storage
        value = monikerStorage.CreateStream(&monikerStream, STREAM_CUSTOM_MONIKER, OPEN_NIFTY_WRITE, NIF_STORAGE_LINK_CLSID);
        if (value == S_OK) {
            monikerStream.AddSection(&NIF_STORAGE_LINK_CLSID);
            Pstrcpy((UBIT8 *)string, (UBIT8 *)fileName);
            Pstrcat((UBIT8 *)string, (UBIT8 *)"\p\0");
            monikerStream.AddProperty((UBIT8 *)&NIF_STORAGE_LINK_CLSID, NIF_RELATIVE_FILE_PATH, NIF_RELATIVE_FILE_PATH_TYPE, (UBIT32)string[0], (UBIT8 *)(string+1));
            monikerStream.AddProperty((UBIT8 *)&NIF_STORAGE_LINK_CLSID, NIF_ABSOLUTE_STORAGE_PATH, NIF_ABSOLUTE_STORAGE_PATH_TYPE, (UBIT32)(strlen(storagePath)+1), (UBIT8 *)storagePath);
            value = CreateFileMonikerFSp(pSpec, &ppmk);
            CreateStreamOnHGlobal(NULL, TRUE, &lpStream);
            ppmk->GetClassID(&clsid);
            byteswapCLSID(&clsid);
            value = lpStream->Write(&clsid, sizeof(CLSID), NULL);
            ppmk->Save(lpStream, TRUE);
            value = lpStream->Stat(&statStg, STATFLAG_NONAME);
            OLEmoniker = (UBIT8 *)ALLOCATE(statStg.cbSize.LowPart);
            if(OLEmoniker) {
        // seek to beginning of stream
                offset.LowPart = 0;
                offset.HighPart = 0;
                value = lpStream->Seek(offset,STREAM_SEEK_SET, NULL);
                value = lpStream->Read(OLEmoniker,statStg.cbSize.LowPart, NULL);
                monikerStream.AddProperty((UBIT8 *)&NIF_STORAGE_LINK_CLSID, NIF_OLE_MONIKER, NIF_OLE_MONIKER_TYPE, (UBIT32)statStg.cbSize.LowPart, (UBIT8 *)OLEmoniker);
                FREE(OLEmoniker);
            }
            lpStream->Release();
            ppmk->Release();
            monikerStream.Release();
        }
        monikerStorage.Release();
  }
}

*/

//  - EOF ----------------------------------------------------------------------
