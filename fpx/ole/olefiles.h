//  ----------------------------------------------------------------------------
//  MODULE    : OLEFiles.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines  OLE file object 
//  COMMENT   : Enhanced OLE objects which are independent of platform
//  SCCSID      : @(#)olefiles.h  1.2 14:47:41 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEFiles_h
  #define OLEFiles_h
  
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef FicNom_h
  #include "filename.h"
#endif

  class OLEStorage;
  
//  Constants
//  ---------
  
//  Classes declarations 
//  --------------------

  class OLEFile;
  
//  Classes definitions
//  -------------------

  class OLEFile : public OLECore
  {
    public:
               OLEFile (const char* theFilename, const char* theStorageName = NULL);
               OLEFile (const FicNom& filefic, const char* theStorageName = NULL);
              virtual ~OLEFile ();

          Boolean GetCLSID (CLSID* classID);
          Boolean GetFileSize (unsigned long* size);
              
          Boolean OpenOLEFile   (CLSID& classID, OLEStorage ** currentStorage = NULL, DWORD mode = OLE_READWRITE_MODE);
          Boolean CreateOLEFile (CLSID& classID, OLEStorage ** newStorage = NULL);
          
          void  GetFileName (FicNom* fileName);
          char* GetFileName ()  { return fileName; }; 
          
          Boolean isFileReadOnly() { return isReadOnly; };
          
    protected:
    virtual   void  Release();
    
      char      fileName[255];
      char      storageName[255];
      Boolean     isEmbeddedStorage;
      Boolean     isReadOnly;
      FicNom      fileFic;
    
      OLEStorage *  rootStorage;    // Root storage of the file (see OLEStorages.h)
      OLEStorage *  embeddedStorage;  // Embedded storage of the file (see OLEStorages.h)
  };    


//  ----------------------------------------------------------------------------
  #endif // OLEFiles_h
//  ----------------------------------------------------------------------------
