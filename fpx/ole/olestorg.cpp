//  ----------------------------------------------------------------------------
//  MODULE    : OLEStorages.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an enhanced OLE storage object 
//  COMMENT   : Enhanced OLE objects are platform independent
//      SCCSID        : @(#)olestorg.cpp  1.3 12:37:38 08 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h 

  #include  <stdio.h>
  #include  <iostream.h>
  #include  <fstream.h>


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

#ifndef OLEFiles_h
  #include  "olefiles.h"
#endif
#ifndef OLEStreams_h
  #include  "olestrm.h"
#endif

#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif

#ifndef OLECustomLink_h
  #include  "oleclink.h"
#endif
  
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
#ifdef _WINDOWS
  #pragma code_seg ("OLEStorage")
#else
  #pragma segment OLEStorage
#endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Functions 'intern'
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLEStorage
//  ----------------------------------------------------------------------------


OLEStorage::OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage)
{
  oleStorage = currentStorage;
  parStorage = parentStorage;
  parFile    = NULL;
  customLink = NULL;
  clsID    = BlankGUID;
    
  openOLEObjList = new List; 
  if ( oleStorage )
    oleStorage->AddRef();
}


OLEStorage::OLEStorage(const CLSID& classID, OLEStorage * parentStorage, IStorage * currentStorage)
{
  oleStorage = currentStorage;
  parStorage = parentStorage;
  parFile    = NULL;
  customLink = NULL;
  clsID    = classID;
  if (oleStorage)
    oleStorage->SetClass(clsID);
    
  openOLEObjList = new List; 
  if ( oleStorage )
    oleStorage->AddRef();
}


OLEStorage::OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage, List *theOpenOLEObjList)
{
  oleStorage = currentStorage;
  parStorage = parentStorage;
  parFile    = NULL;
  customLink = NULL;
  clsID    = BlankGUID;
    
  openOLEObjList = theOpenOLEObjList;
  if ( openOLEObjList )
    openOLEObjList->AddRef();
  else 
    openOLEObjList = new List;
    
  if ( oleStorage )
    oleStorage->AddRef();
}

OLEStorage::OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage, OLECustomLink * theCustomLink)
{
  oleStorage = currentStorage;
  parStorage = parentStorage;
  parFile    = NULL;
  customLink = theCustomLink;
  clsID    = BlankGUID;
    
  openOLEObjList = new List;
  if ( oleStorage )
    oleStorage->AddRef();
}

OLEStorage::OLEStorage(OLEFile  * parentFile, IStorage * currentStorage)
{
  oleStorage = currentStorage;
  parStorage = NULL;
  parFile    = parentFile;
  customLink = NULL;
  clsID    = BlankGUID;
    
  openOLEObjList = new List;
  if ( oleStorage )
    oleStorage->AddRef();
}


OLEStorage::OLEStorage(OLEFile  * parentFile, IStorage * currentStorage, List * theOpenOLEObjList)
{
  oleStorage = currentStorage;
  parStorage = NULL;
  parFile    = parentFile;
  customLink = NULL;
  clsID    = BlankGUID;
    
  openOLEObjList = theOpenOLEObjList;
  if ( openOLEObjList )
    openOLEObjList->AddRef();
  else 
    openOLEObjList = new List;
    
  if ( oleStorage )
    oleStorage->AddRef();
}


OLEStorage::OLEStorage(const CLSID& classID, OLEFile  * parentFile, IStorage * currentStorage)
{
  oleStorage = currentStorage;
  parStorage = NULL;
  parFile    = parentFile;
  customLink = NULL;
  clsID    = classID;
  if (oleStorage)
    oleStorage->SetClass(clsID);
    
  openOLEObjList = new List; 
  if ( oleStorage )
    oleStorage->AddRef();
}


OLEStorage::~OLEStorage()
{
  Release();
}


// Check if it is referenced by others
Boolean OLEStorage::IsReferenced ()
{
  if ( openOLEObjList->GetUserCount() <= 1 )
    return FALSE;
  else
    return TRUE;
}


void OLEStorage::GetFileName (FicNom* fileName)
{
  if (parStorage)
    parStorage->GetFileName(fileName);
  else
    parFile->GetFileName(fileName);
}


// Add reference to the IStorage object contained in the OLEStorage
void OLEStorage::AddRef()
{
  OLECore::AddRef();
   
  if (oleStorage)
    oleStorage->AddRef();
  
  if (openOLEObjList)
    openOLEObjList->AddRef();
}


// Release the IStorage object contained in the OLEStorage
void OLEStorage::Release()
{
  if (customLink) {
    delete customLink;
    customLink = NULL;
  } 
  else if (oleStorage)
    oleStorage->Release();
  
  if (openOLEObjList)
    openOLEObjList->Release();
    
  // If it is the root storage and is not referenced by others, delete it from the list 
  if ( parFile )
    if ( !IsReferenced() )
      openRootStorageList->Delete(parFile->GetFileName());
      
  OLECore::Release();
}


// Open a stream within current storage
Boolean OLEStorage::OpenStream(const char * name, OLEStream ** currentStream, DWORD mode)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
  
    LPSTREAM ppStm;
    
    if ( !openOLEObjList )
      return FALSE; 
    
    // Search the opened object list, if the object already open just add its reference
    if ( (ppStm = (IStream *)openOLEObjList->Search((char *)name)) == NULL ) {
      
      err = oleStorage->OpenStream(OLE_STR(name), 0, mode, 0, &ppStm);
#ifdef macintosh
  #ifndef powerc
      if( (ppStm == NULL) && (mode == OLE_READWRITE_MODE))
  #else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
  #endif
#else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
#endif
        // Try to open at least in read mode if the read/write failed
        err = oleStorage->OpenStream(OLE_STR(name), 0, OLE_READ_ONLY_MODE, 0, &ppStm);
    
      if ( !FAILED(err) ) 
        // Add stream to the list
        openOLEObjList->Add(ppStm, name);       
    }
        
    if (FAILED(err)) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      // Create an OLEStream
      if ( (*currentStream = new OLEStream(this, ppStm)) != NULL ) 
        return true; 
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } 
  else  
    return false;
}


// Create a stream within current storage
Boolean OLEStorage::CreateStream(const char * name, OLEStream ** newStream)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    LPSTREAM ppStm;
    if (FAILED(err = oleStorage->CreateStream(OLE_STR(name), OLE_CREATE_MODE, 0, 0, &ppStm))) { 
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    }
    else {
      if ( !openOLEObjList ) 
        return FALSE; 
        
      // Add the object to the list
      openOLEObjList->Add(ppStm, name); 
      
      // Create an OLEStream
      if (( *newStream = new OLEStream(this, ppStm)) != NULL )
        return true;
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


Boolean OLEStorage::OpenStorage(const char * name, OLEStorage ** currentStorage, DWORD mode)
{
  HRESULT        err         = NOERROR;
  LPSTORAGE      ppStg       = NULL;
  LPSTORAGE      ppLStg      = NULL;
  OLECustomLink* customLink  = NULL;
  OLEStorage*    openStorage = NULL;
  List *         theList     = NULL;
  
  if (oleStorage) {
  
    if ( !openOLEObjList )  return FALSE; 
    
    // Search the opened object list, if the object already open just add its reference
    if ( (openStorage = (OLEStorage *)openOLEObjList->Search((char *)name)) == NULL ) {
      
      // Open the names storage
      err = oleStorage->OpenStorage(OLE_STR(name), 0, mode, NULL, 0, &ppStg);
#ifdef macintosh
  #ifndef powerc
      if( (ppStg == NULL) && (mode == OLE_READWRITE_MODE))
  #else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
  #endif
#else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
#endif
        // Try to open at least in read mode if the read/write failed
        err = oleStorage->OpenStorage(OLE_STR(name), 0, OLE_READ_ONLY_MODE, NULL, 0, &ppStg);
        
      // Exit if error
      if (FAILED(err))
        goto error;
      if(ppStg == NULL)
        goto error;

      // Read the classID of the newly opened storage
      STATSTG statstg;
      err = ppStg->Stat(&statstg, STATFLAG_NONAME);

      if (FAILED(err))
        goto error;

      CLSID idCustomMoniker = ID_CustomMoniker;
      if (statstg.clsid == idCustomMoniker) {
      
        // Create a custom link handle
        if ((customLink = new OLECustomLink(&OLEStorage(this,ppStg))) == NULL) {
          lastError = (short) STG_E_INSUFFICIENTMEMORY; 
          fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
          goto error_set;       // CHG_FILE_ERR - jump past re-set of error value
        }
        
        // Release the current storage (useless now we've read the custom link inside...)
        // For the moment, this leads to an exception in OLE.
        // ppStg->Release();
        // ppStg = NULL;
        
        // Get the pointer to the linked OLE IStorage
        ppLStg = customLink->GetLinkedIStorage();
        
        if (ppLStg != NULL) {
          // Create a storage object
          if (( *currentStorage = new OLEStorage(this, ppLStg, customLink)) == NULL) {
            lastError = (short) STG_E_INSUFFICIENTMEMORY; 
            fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
            goto error_set;     // CHG_FILE_ERR - jump past re-set of error value
          }
          // Add object to the list
          openOLEObjList->Add(*currentStorage, name); 
        } else {
          // We can't open the link to the source storage
          lastError = (short) OLE_E_CANT_BINDTOSOURCE;
          fpxStatus = FPX_OLE_FILE_ERROR;
          goto error_set;       // CHG_FILE_ERR - jump past re-set of error value
        }
      }
      else {
        // Treat like a normal storage
        if (( *currentStorage = new OLEStorage(this, ppStg)) == NULL) {
          lastError = (short) STG_E_INSUFFICIENTMEMORY; 
          fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
          goto error_set;       // CHG_FILE_ERR - jump past re-set of error value
        }
        
        // Add object to the list
        openOLEObjList->Add(*currentStorage, name);   
      }
    } else {
      // Create a new storage from current ole storage
      theList   = openStorage->GetList();
      ppStg     = openStorage->GetStorage();
      if (( *currentStorage = new OLEStorage(this, ppStg, theList)) == NULL) {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        goto error_set;         // CHG_FILE_ERR - jump past re-set of error value
      }
    }   
  } 
  else  // No pointer to an OLE storage
    goto error;
  
  return TRUE;

error:
  // Handle all the error cases at once
  if (FAILED(err)) {
    lastError = TranslateOLEError(err);
    fpxStatus = OLEtoFPXError(err);
  }
error_set:                    // CHG_FILE_ERR - jump here to preserve error value
  if (ppStg)
    ppStg->Release();
  if (customLink)
    delete customLink;
  return FALSE;
}


Boolean OLEStorage::OpenStorage(const CLSID& , const char * name, OLEStorage ** currentStorage, DWORD mode)
{
  // We do nothing with the class ID for now but we should compare it with the one included
  // in the open storage using IStorage::Stat()
  return OpenStorage(name,currentStorage,mode);
}


// Create a storage within current storage
Boolean OLEStorage::CreateStorage(const CLSID& classID, const char * name, OLEStorage ** newStorage)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
  
    LPSTORAGE ppStg;
    if (FAILED(err = oleStorage->CreateStorage(OLE_STR(name), OLE_CREATE_MODE, 0, 0, &ppStg)) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      if ( !openOLEObjList ) 
        return FALSE; 
        
      // Add the object to the list
      openOLEObjList->Add(ppStg, name); 
      
      // Create an OLEStorage
      if (( *newStorage = new OLEStorage(classID, this, ppStg)) != NULL )
        return true;
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


// Create a storage within current storage
Boolean OLEStorage::CreateStorage(const char * name, OLEStorage ** newStorage)
{
  CLSID classID = BlankGUID; 
  return CreateStorage(classID,name,newStorage);
}


// This function creates a property set specified by its name and a CLSID provided by the user
Boolean OLEStorage::CreatePropertySet(const CLSID& classID, const char *name, OLEPropertySet ** newPropertySet)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    LPSTREAM ppStm;
    if (FAILED(err = oleStorage->CreateStream(OLE_STR(name), OLE_CREATE_MODE, 0, 0, &ppStm)) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    }
    else {
      if ( !openOLEObjList ) 
        return FALSE; 
        
      // Add the property set to the list
      openOLEObjList->Add(ppStm, name); 
      
      // Create an OLEPropertySet
      if (( *newPropertySet = new OLEPropertySet(classID, this, ppStm)) != NULL )
        return true;
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


// This function creates a property set specified by its name within an open OLE storage. 
// using a default CLSID 
Boolean OLEStorage::CreatePropertySet(const char *name, OLEPropertySet ** newPropertySet)
{
  CLSID classID = BlankGUID; 
  return CreatePropertySet(classID,name,newPropertySet);
}


// This function opens a property set specified by its name and a CLSID provided by the user
Boolean OLEStorage::OpenPropertySet(const CLSID& classID, const char *name, OLEPropertySet ** curPropertySet, DWORD mode)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
  
    LPSTREAM ppStm;
    if ( !openOLEObjList )  return FALSE;
     
    // Search the opened object list, if the object already open just add its reference
    // NOTE: property set is a special kind of stream
    if ( (ppStm = (IStream *)openOLEObjList->Search(name)) == NULL ) {

      err = oleStorage->OpenStream(OLE_STR(name), 0, mode, 0, &ppStm);
#ifdef macintosh
  #ifndef powerc
      if( (ppStm == NULL) && (mode == OLE_READWRITE_MODE))
  #else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
  #endif
#else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
#endif
        // Try to open at least in read mode if the read/write failed
        err = oleStorage->OpenStream(OLE_STR(name), 0, OLE_READ_ONLY_MODE, 0, &ppStm);
    
      if ( !FAILED(err) || ppStm != NULL) 
        // Add stream to the list
        openOLEObjList->Add(ppStm, name);       
    }

    if (FAILED(err) || ppStm == NULL) {
      lastError = TranslateOLEError(err);
      if (err != NOERROR)
        fpxStatus = OLEtoFPXError(err);
      else
        fpxStatus = FPX_OLE_FILE_ERROR;
      return false;
    } 
    else {
      // Create an OLEPropertySet
      if (( *curPropertySet = new OLEPropertySet(classID, this, ppStm)) != NULL )
        return true;
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


// This function opens a property set specified by its name within a open OLE storage 
// using a default CLSID 
Boolean OLEStorage::OpenPropertySet(const char *name, OLEPropertySet ** curPropertySet, DWORD mode)
{
  CLSID classID = BlankGUID; 
  return OpenPropertySet(classID,name, curPropertySet, mode);
}


// This function creates a header stream specified by its name and a CLSID provided by the user
Boolean OLEStorage::CreateHeaderStream(const CLSID& classID, const char *name, OLEHeaderStream ** newHeaderStream)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    LPSTREAM ppStm;
    if (FAILED(err = oleStorage->CreateStream(OLE_STR(name), OLE_CREATE_MODE, 0, 0, &ppStm)) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    }
    else {
      if ( !openOLEObjList ) 
        return FALSE; 
        
      // Add the stream to the list
      openOLEObjList->Add(ppStm, name); 
      
      // Create an OLEHeaderStream
      if (( *newHeaderStream = new OLEHeaderStream(classID, this, ppStm)) != NULL ) {
        // write 28byte FPX header when creating a FPX stream
        (*newHeaderStream)->WriteHeader();

        return true;
      }
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


// This function creates a header stream specified by its name within an open OLE storage. 
// using a default CLSID 
Boolean OLEStorage::CreateHeaderStream(const char *name, OLEHeaderStream ** newHeaderStream)
{
  CLSID classID = BlankGUID; 
  return CreateHeaderStream(classID,name,newHeaderStream);
}


// This function opens a header stream specified by its name and a CLSID provided by the user
Boolean OLEStorage::OpenHeaderStream(const CLSID& classID, const char *name, OLEHeaderStream ** curHeaderStream, DWORD mode)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
  
    LPSTREAM ppStm;
    if ( !openOLEObjList )  return FALSE; 
    
    // Search the opened object list, if the object already open just add its reference
    if ( (ppStm = (IStream *)openOLEObjList->Search(name)) == NULL ) {
      
      err = oleStorage->OpenStream(OLE_STR(name), 0, mode, 0, &ppStm);
#ifdef macintosh
  #ifndef powerc
      if( (ppStm == NULL) && (mode == OLE_READWRITE_MODE))
  #else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
  #endif
#else
      if (FAILED(err) && (mode == OLE_READWRITE_MODE))
#endif
        // Try to open at least in read mode if the read/write failed
        err = oleStorage->OpenStream(OLE_STR(name), 0, OLE_READ_ONLY_MODE, 0, &ppStm);
    
      if ( !FAILED(err) ) 
        // Add stream to the list
        openOLEObjList->Add(ppStm, name);       
    }
    
    if (FAILED(err)) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      // Create an OLEHeaderStream
      if (( *curHeaderStream = new OLEHeaderStream(classID, this, ppStm)) != NULL )
        return true;
      else {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    } 
  } else
    return false;
}


// This function opens a header stream specified by its name within an open OLE storage 
// using a default CLSID 
Boolean OLEStorage::OpenHeaderStream(const char *name, OLEHeaderStream ** curHeaderStream, DWORD mode)
{
  CLSID classID = BlankGUID; 
  return OpenHeaderStream(classID,name,curHeaderStream,mode);
}

// This function dumps the stats of all objects within an open stroage to a stream
Boolean OLEStorage::DumpStats( ofstream& statStream )
{
#if   defined(__GNUC__)
  // This function is only used for debugging the contents of a storage so
  // it is not *STRICTLY* necessary. 
  return false;
#else
  // The number of indentation in the embedding storages
  static long level = 0;
  
  OLEEnumStatstg* pEnumStatstg; 
  STATSTG     statstg;
  HRESULT     err = NOERROR; 
  char      str[512]; 
  LPMALLOC    pMalloc = NULL; 
  
  CoGetMalloc(MEMCTX_TASK, &pMalloc); 
  
  if ( !EnumElements(&pEnumStatstg) ) 
    return false; 
  
  while (pEnumStatstg->Next(&statstg)) {
    long i = level;
    while (i--)
      statStream.put('\t');
      
    if (pEnumStatstg->IsStorage(&statstg))
      sprintf(str, "This object is a storage, its name is %s, ", statstg.pwcsName); 
    if (pEnumStatstg->IsStream(&statstg))
      sprintf(str, "This object is a stream, its name is %s, ", statstg.pwcsName); 
    if (pEnumStatstg->IsPropertySet(&statstg))
      sprintf(str, "This object is a property set, its name is %s, ", statstg.pwcsName); 
    statStream.write(str, strlen(str)); 
    sprintf(str, "its size is %lu%lu\n", statstg.cbSize.HighPart, statstg.cbSize.LowPart);
    statStream.write(str, strlen(str)); 

    if (pEnumStatstg->IsStorage(&statstg)) {      
      LPSTORAGE pStg;

      if (FAILED(err = oleStorage->OpenStorage(OLE_STR(statstg.pwcsName), 0, OLE_READWRITE_MODE, NULL, 0, &pStg)) ) {
        if (FAILED(err = oleStorage->OpenStorage(OLE_STR(statstg.pwcsName), 0, OLE_READ_ONLY_MODE, NULL, 0, &pStg)) ) {
          lastError = TranslateOLEError(err);
          fpxStatus = OLEtoFPXError(err);
          return false;
        }
      }
      
      OLEStorage *currentStorage = new OLEStorage(this, pStg);  
      if (currentStorage==NULL) {
        lastError = SEVERITY_ERROR;
        fpxStatus = FPX_OLE_FILE_ERROR;
        return false;
      } 
      level++;
      currentStorage->DumpStats(statStream);
      level--;
      delete currentStorage;
    } 
    pMalloc->Free(statstg.pwcsName); 
  }
  
  pMalloc->Release(); 
  delete pEnumStatstg; 
  
  return true; 
#endif // __GNUC__
}   

// The following functions are the same as the one defined for IStream's

Boolean OLEStorage::Commit( )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    if (FAILED(err = oleStorage->Commit(0)) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
      
    return true;
  } else
    return false;
}


Boolean OLEStorage::Revert( )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    if (FAILED(err = oleStorage->Revert()) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    }       
    return true;
  } else
    return false;
}


Boolean OLEStorage::EnumElements( OLEEnumStatstg ** ppEnumStatstg)
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    IEnumSTATSTG * oleEnumStatstg; 
    if (FAILED(err = oleStorage->EnumElements( 0, NULL, 0, &oleEnumStatstg))) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      *ppEnumStatstg = new OLEEnumStatstg(this, oleEnumStatstg);      
      if ( !*ppEnumStatstg) {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    }
  } else
    return false;
    
  return true;
}


Boolean OLEStorage::Stat( STATSTG* pStatstg )
{
  HRESULT err = NOERROR;
  
  if (oleStorage) {
    if (FAILED(err = oleStorage->Stat(pStatstg, STATFLAG_NONAME))) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } else
      return true;
  } else
    return false;
}


Boolean OLEStorage::GetSize( unsigned long *size )
{
  Boolean ok = TRUE;
  STATSTG stat;
  *size = 0;

  ok = Stat(&stat);
  if (ok)
    *size = (unsigned long)(stat.cbSize.LowPart);

  return ok;
}


// Rename an element under a storage
Boolean OLEStorage::RenameElement( const char *oldName, const char *newName )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    if (FAILED(err = oleStorage->RenameElement(OLE_STR(oldName), OLE_STR(newName)))) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      return true;
    }
  } else
    return false;
}


// Remove an element under a storage
Boolean OLEStorage::DestroyElement( const char *name )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    if (FAILED(err = oleStorage->DestroyElement(OLE_STR(name)))) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else {
      return true;
    }
  } else
    return false;
}


// Copy an OLEStorage to another OLEStorage
Boolean OLEStorage::CopyTo( OLEStorage ** ppStorage )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    LPSTORAGE pStg = NULL;
    if (FAILED(err = oleStorage->CopyTo(0, NULL, NULL, pStg))) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
    else
    {
      *ppStorage = new OLEStorage(this, pStg);
      if ( !*ppStorage) {
        lastError = (short) STG_E_INSUFFICIENTMEMORY; 
        fpxStatus = FPX_MEMORY_ALLOCATION_FAILED;
        return false;
      }
    }
  } else
    return false;
    
  return true;
}


// Copy an OLEStorage to IStorage
Boolean OLEStorage::CopyTo( IStorage * pStorage )
{
  HRESULT err = NOERROR;
  if (oleStorage) {
    LPSTORAGE pStg = NULL;
    if ( FAILED(err = oleStorage->CopyTo(0, NULL, NULL, pStorage)) ) {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);
      return false;
    } 
  }
  else
    return false;
    
  return true;
}



