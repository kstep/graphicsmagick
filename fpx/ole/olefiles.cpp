//  ----------------------------------------------------------------------------
//  MODULE    : OLEFiles.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines  OLE file object and log file object
//  COMMENT   : Enhanced OLE objects which are independent of platform
//      SCCSID        : @(#)olefiles.cpp  1.4 12:37:31 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  Includes
//  --------



#ifndef OLECommun_h
  #include "olecomm.h"
#endif

#ifndef OLECore_h
  #include "olecore.h"
#endif

#ifndef OLEFiles_h
  #include  "olefiles.h"
#endif

#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif

#ifndef FicNom_h
  #include "filename.h"
#endif

//  Constants
//  ---------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
 
static OLECHAR * AsciiToOLECHAR(const char * filename); // PTCH_WS_FIX - added prototype

//  ----------------------------------------------------------------------------
//  Methods of OLEFile
//  ----------------------------------------------------------------------------


OLEFile::OLEFile(const FicNom& filefic, const char* theStorageName) 
      : isReadOnly(FALSE),
        fileFic(filefic)        
{
  fileFic.CopyToCStr(fileName);

  setFPXStatus(FPX_OK);
  if (theStorageName) {
    isEmbeddedStorage = true;
    strcpy(storageName, theStorageName);
  } else
    isEmbeddedStorage = false;
  rootStorage = NULL;
  embeddedStorage = NULL;
}


OLEFile::OLEFile(const char* theFilename, const char* theStorageName) 
        : isReadOnly(FALSE),
          fileFic(theFilename)
{
  setFPXStatus(FPX_OK);
  strcpy(fileName, theFilename);
  if (theStorageName) {
    isEmbeddedStorage = true;
    strcpy(storageName, theStorageName);
  } else
    isEmbeddedStorage = false;
  rootStorage = NULL;
  embeddedStorage = NULL;
}


OLEFile::~OLEFile()
{
  Release(); 
}


void OLEFile::GetFileName (FicNom* fileName)
{
  *fileName = fileFic;
}


void OLEFile::Release()
{
  if (embeddedStorage) {
    // Releases the storage
    delete embeddedStorage;
    embeddedStorage = NULL;
  }
  
  if (rootStorage) {
    // If the root storage is not referenced by others, delete it from the list 
    if ( !rootStorage->IsReferenced() )
      openRootStorageList->Delete(fileName);
      
    // Releases the root storage
    rootStorage->Release();
    rootStorage = NULL;
  }
  
  OLECore::Release();
}


Boolean OLEFile::OpenOLEFile(CLSID& classID, OLEStorage ** currentStorage, DWORD mode)
{
  HRESULT   err;
  IStorage *  oleStorage = NULL;
  IStorage *  theOleStorage = NULL;
  List *    theList = NULL;
  
  // Already open: get out of here...
  if (embeddedStorage) {
    if (currentStorage) {
      // Create a new storage from root storage
      theOleStorage   = embeddedStorage->GetStorage();
      theList     = embeddedStorage->GetList();
      *currentStorage = new OLEStorage(this, theOleStorage, theList);
    }
    return true;
  }
  else if (rootStorage) {
    if (currentStorage) {
      // Create a new storage from root storage
      theOleStorage   = rootStorage->GetStorage();
      theList     = rootStorage->GetList();
      *currentStorage = new OLEStorage(this, theOleStorage, theList);
    }
    return true;
  }
  
  // Check openRootStorageList to see if the rootStorage has been opened by other files
  if ( !openRootStorageList )
    return FALSE; 
  
  // Search the opened object list, if the object already open just add its reference
  if ( (rootStorage = (OLEStorage *)openRootStorageList->Search(fileName)) == NULL ) {
      
#ifdef macintosh
    // FileFsp functions are mac-specific and have no Windows SDK equivalent
    FSSpec  pFSSpec; 

    fileFic.CopyToFSSpec (pFSSpec);

    err = StgOpenStorageFSp (&pFSSpec, NULL, mode, NULL, 0, &oleStorage);
    if (FAILED(err) && (mode == OLE_READWRITE_MODE)) {
      isReadOnly = 1;

      // Try to open at least in read mode if the read/write failed
      err = StgOpenStorageFSp (&pFSSpec, NULL, OLE_READ_ONLY_MODE, NULL, 0, &oleStorage);
    }
#else
    // PTCH_WS_FIX - convert ASCII "fileName" to new wide char string "newName"
    OLECHAR *newName = AsciiToOLECHAR (fileName);

    // Check if it's really a file
    // PTCH_WS_FIX - replaced "fileName" with "newName" in StgIsStorageFile() call
    if (FAILED( err = StgIsStorageFile(OLE_STR(newName)))) {
      lastError = TranslateOLEError(err);   
      fpxStatus = OLEtoFPXError(err);
      return false; 
    }

    // PTCH_WS_FIX - replaced "fileName" with "newName" in StgOpenStorage() call
    // CHG_FILE_ERR - OLE err for missing file is 'invalid handle'. Return 'file not found' instead
    err = StgOpenStorage (OLE_STR(newName), NULL, mode, NULL, 0, &oleStorage);
    if (FAILED(err) && (mode == OLE_READWRITE_MODE)) {
      isReadOnly = 1;
      // Try to open at least in read mode if the read/write failed
      // PTCH_WS_FIX - replaced "fileName" with "newName" in StgOpenStorage() call
      err = StgOpenStorage (OLE_STR(newName), NULL, OLE_READ_ONLY_MODE, NULL, 0, &oleStorage);
    }
#endif
  
    if (FAILED(err)) {
      lastError = TranslateOLEError(err);   
      fpxStatus = FPX_FILE_NOT_FOUND;
      Release();

      if (currentStorage) 
        *currentStorage = NULL;
      return FALSE;
    } 
    else {
      // else, create the root storage
      rootStorage = new OLEStorage(this, oleStorage);
      if (rootStorage == NULL) {
        return FALSE;
      }

      // Add object to the list
      openRootStorageList->Add(rootStorage, fileName);      

      // For root storage, the reference is one less than other storage
      oleStorage->Release();
      
      // Open the embedded storage if any
      if (isEmbeddedStorage) {
        Boolean ok = rootStorage->OpenStorage (classID, storageName, &embeddedStorage);
        if (!ok) {
          Release();
          return FALSE;
        }
        else
          if (currentStorage) 
            *currentStorage = embeddedStorage;
      } 
      else
        if (currentStorage) {
          // Create a new storage from current ole storage
          theList     = rootStorage->GetList();
          *currentStorage = new OLEStorage(this, oleStorage, theList);
        }
      return true;
    } 
  }
  else {
    // Add reference
    rootStorage->AddRef();
    
    // Open the embedded storage if any
    if (isEmbeddedStorage) {
      Boolean ok = rootStorage->OpenStorage (classID, storageName, &embeddedStorage);
      if (!ok) {
        Release();
        return FALSE;
      }
      else
        if (currentStorage)
          *currentStorage = embeddedStorage;
    } 
    else
      if (currentStorage) {
        // Create a new storage from current ole storage
        theList     = rootStorage->GetList();
        oleStorage    = rootStorage->GetStorage();
        *currentStorage = new OLEStorage(this, oleStorage, theList);
      }
    return true;
  }
}


Boolean OLEFile::CreateOLEFile(CLSID& classID, OLEStorage ** newStorage)
{
  HRESULT   err;
  IStorage *  oleStorage;
  IStorage *  theOleStorage = NULL;
  List *    theList = NULL;

  // Already created: get out of here...
  if (embeddedStorage) {
    if (newStorage) {
      // Create a new storage from root storage
      theOleStorage   = embeddedStorage->GetStorage();
      theList     = embeddedStorage->GetList();
      *newStorage   = new OLEStorage(this, theOleStorage, theList);
    }
    return true;
  }
  else if (rootStorage) {
    if (newStorage)   {
      // Create a new storage from root storage
      theOleStorage   = rootStorage->GetStorage();
      theList     = rootStorage->GetList();
      *newStorage   = new OLEStorage(this, theOleStorage, theList);
    }
    return true;
  }

#ifdef macintosh
  // FileFsp functions are mac-specific and have no Windows SDK equivalent
  FSSpec  pFSSpec;

  fileFic.CopyToFSSpec (pFSSpec);

  err = StgCreateDocfileFSp (&pFSSpec, 'FOOO', 'FPix', NULL, OLE_CREATE_MODE, 0, &oleStorage);
#else
  // PTCH_WS_FIX - convert ASCII "fileName" to new wide char string "newName"
  OLECHAR *newName = AsciiToOLECHAR (fileName);

  // PTCH_WS_FIX - replaced "fileName" with "newName" in StgCreateDocfile() call
   err = StgCreateDocfile   (OLE_STR(newName), OLE_CREATE_MODE, 0, &oleStorage);
#endif

  if (FAILED(err)) {
    // If error
    lastError = TranslateOLEError(err);   
    fpxStatus = OLEtoFPXError(err);
    rootStorage = NULL;
    if (newStorage) *newStorage = NULL;
    return false;
  } else {
    // else, create the root storage
    rootStorage = new OLEStorage(classID,this, oleStorage);
    
    // For root storage, the reference is one less than other storage
    oleStorage->Release();
    
    // Open the embedded storage if any
    if (isEmbeddedStorage) {
      Boolean ok = rootStorage->CreateStorage (classID, storageName, &embeddedStorage);
      if (!ok)
        Release();
      else
        if (newStorage) {
          //embeddedStorage->AddRef(); 
          *newStorage = embeddedStorage;
        }
    } else
      if (newStorage) {
        // Create a new storage from current root storage
        theList   = rootStorage->GetList();
        *newStorage = new OLEStorage(this, oleStorage, theList);
        //rootStorage->AddRef();
        //*newStorage = rootStorage; 
      }
    return true;
  } 
}


Boolean OLEFile::GetCLSID (CLSID* classID)
{
  // Open the file if necessary, exit if error
  if (!OpenOLEFile(*classID))
    return false;
  
  // Read the stats 
  STATSTG stat;
  if (isEmbeddedStorage)
    embeddedStorage->Stat(&stat);
  else
    rootStorage->Stat(&stat);
  *classID = stat.clsid;
  
  return true;
}


Boolean OLEFile::GetFileSize (unsigned long* size)
{
  // Open the file if necessary, exit if error
  CLSID id;
  if (!OpenOLEFile(id))
    return false;
  
  // Read the stats 
  STATSTG stat;
  if (isEmbeddedStorage)
    embeddedStorage->Stat(&stat);
  else
    rootStorage->Stat(&stat);
  *size = (unsigned long)(stat.cbSize.LowPart);
  
  return true;
}

// PTCH_WS_FIX - added the following routine, AsciiToOLECHAR() 
// convert an ascii filename, stream, or storagename into
// into the correct form for the OLE call.
// caller owns the OLECHAR string returned.
static OLECHAR * AsciiToOLECHAR(const char * filename)
{
        if (filename == NULL)
                return NULL;

#if defined(macintosh) || defined(__unix)
        // find the length of filename.
        long len = strlen(filename);

        // a new string.
        OLECHAR * newString = new OLECHAR[len+1];

        // and copy the bytes to whatever.
        long i;
        for(i=0; i<len+1; i++)
                newString[i] = (OLECHAR)filename[i];
                
#else // windows
        // find the length of filename.
        long len = strlen(filename);

        // a new string.
        WCHAR * newString = new WCHAR[len+10];

        // and convert the bytes to whatever.
        long i;
        i = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,filename,len,newString,len+10);

        // and that pesky little NULL at the end...
        newString[i] = 0x0000;
#endif
        return newString;
}




