//  ----------------------------------------------------------------------------
//  MODULE    : OLEStorages.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an enhanced OLE storage object 
//  COMMENT   : Enhanced OLE objects are platform independent
//  SCCSID      : @(#)olestorg.h  1.1 11:53:31 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEStorages_h
  #define OLEStorages_h
  
//  ----------------------------------------------------------------------------

//  Includes
//  --------
  #include <fstream.h>

#ifndef OLECore_h
  #include  "olecore.h"
#endif
  class OLEEnumStatstg;
  class OLEFile;
  class OLEStream;
  class OLEHeaderStream;
  class OLEPropertySet;
  class OLECustomLink;


//  Constants
//  ---------
  
//  Class declarations 
//  ------------------

  class OLEStorage;
  
//  Class definitions
//  -----------------

  class OLEStorage : public OLECore
  {
  
    friend class OLECustomLink;
    
  public:
              // This constructor creates an OLEStorage within a parent OLE storage
              OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage);
              OLEStorage(REFCLSID classID, OLEStorage * parentStorage, IStorage * currentStorage);
              OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage, List *theOpenOLEObjList);
              OLEStorage(OLEStorage * parentStorage, IStorage * currentStorage, OLECustomLink * theCustomLink);
    
              // This constructor creates an OLEStorage within a parent OLE file
              OLEStorage(OLEFile * parentFile, IStorage * currentStorage);
              OLEStorage(REFCLSID classID, OLEFile * parentFile, IStorage * currentStorage);
              OLEStorage(OLEFile  * parentFile, IStorage * currentStorage, List * theOpenOLEObjList);
    
              ~OLEStorage();
    
    // The following methods return true if success (ok) and false otherwise (!ok)
    
    // This function opens a stream specified by its name within an open OLE storage
    virtual Boolean   OpenStream(const char * name, OLEStream ** currentStream, DWORD mode = OLE_READWRITE_MODE);
    
    // This function creates a stream specified by its name within an open OLE storage
    virtual Boolean   CreateStream(const char * name, OLEStream ** newStream);        

    // This function opens a storage specified by its name within an open OLE storage
    // using a default CLSID 
    virtual Boolean   OpenStorage(const char * name, OLEStorage ** currentStorage, DWORD mode = OLE_READWRITE_MODE);

    // This function opens a storage specified by its name and a CLSID provided by the user
    virtual Boolean   OpenStorage(REFCLSID classID, const char * name, OLEStorage ** currentStorage, DWORD mode = OLE_READWRITE_MODE);

    // This function creates a storage specified by its name within an open OLE storage
    // using a default CLSID 
    virtual Boolean   CreateStorage(const char * name, OLEStorage ** newStorage);

    // This function creates a storage specified by its name and a CLSID provided by the user
    virtual Boolean   CreateStorage(REFCLSID classID, const char * name, OLEStorage ** newStorage);

    // This function opens a property set specified by its name within an open OLE storage 
    // using a default CLSID 
    virtual Boolean   OpenPropertySet(const char *name, OLEPropertySet **currentPropertySet, DWORD mode = OLE_READWRITE_MODE);

    // This function opens a property set specified by its name and a CLSID provided by the user
    virtual Boolean   OpenPropertySet(REFCLSID classID, const char *name, OLEPropertySet **currentPropertySet, DWORD mode = OLE_READWRITE_MODE);

    // This function creates a property set specified by its name within an open OLE storage
    // using a default CLSID 
    virtual Boolean   CreatePropertySet(const char *name, OLEPropertySet **newPropertySet);
    
    // This function creates a property set specified by its name and its CLSID provided by the user
    virtual Boolean   CreatePropertySet(REFCLSID classID, const char *name, OLEPropertySet **newPropertySet);
    
    // This function opens a header stream specified by its name within an open OLE storage 
    // using a default CLSID 
    virtual Boolean   OpenHeaderStream(const char *name, OLEHeaderStream **currentHeaderStream, DWORD mode = OLE_READWRITE_MODE);

    // This function opens a header stream specified by its name and its CLSID provided by the user
    virtual Boolean   OpenHeaderStream(REFCLSID classID, const char *name, OLEHeaderStream **currentHeaderStream, DWORD mode = OLE_READWRITE_MODE);

    // This function creates a header stream specified by its name within an open OLE storage. 
    // using a default CLSID 
    virtual Boolean   CreateHeaderStream(const char *name, OLEHeaderStream **newHeaderStream);
    
    // This function creates a header stream specified by its name and its CLSID provided by the user
    virtual Boolean   CreateHeaderStream(REFCLSID classID, const char *name, OLEHeaderStream **newHeaderStream);
    
      virtual OLEFile * GetFile()     { return parFile;    };   // Return parent file
    virtual OLEStorage* GetParStorage() { return parStorage; };   // Return parent storage
    virtual IStorage*   GetStorage()  { return oleStorage; };   // Return IStorage
    virtual List*     GetList()     { return openOLEObjList; }; // Return list
    
    virtual Boolean   IsReferenced();               // Check if it is referenced by others    
    
    // This function dumps the stats of all objects within a open stroage to a stream
    virtual Boolean   DumpStats( ofstream& statStream ); 
    
    // The following functions are the same as the one defined for IStream's
    virtual Boolean   Commit(); 
    virtual Boolean   Revert(); 
    virtual Boolean   EnumElements( OLEEnumStatstg ** ); 
    virtual Boolean   DestroyElement( const char * ); 
    virtual Boolean   RenameElement( const char *, const char * ); 
    virtual Boolean   CopyTo( OLEStorage ** ); 
    virtual Boolean   CopyTo( IStorage * ); 
    virtual Boolean   Stat( STATSTG* pStatstg );
    virtual Boolean   GetSize( unsigned long *size );
    
    // Special functions
        void    GetFileName (FicNom* fileName);
    
    // Add reference to the IStorage object contained in the OLEStorage
    virtual   void  AddRef();
    virtual   void  Release();

  private:
    IStorage *     oleStorage;     // The real OLE pointer to the storage...
    CLSID          clsID;          // The Identifier of the storage
    OLEStorage *   parStorage;     // Parent storage
    OLEFile *      parFile;        // Parent file
    OLECustomLink* customLink;     // If the storage is linked to another storage, we maintain the link
    List *         openOLEObjList; // List for all opened OLE objects within the storage   
  };


//  ----------------------------------------------------------------------------
  #endif // OLEStorages_h
//  ----------------------------------------------------------------------------
