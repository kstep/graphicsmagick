//  ----------------------------------------------------------------------------
//  MODULE    : FileFlashPixView
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, April 4, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID          : @(#)f_fpxvw.h 1.4 12:47:17 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef FileFlashPixView_h
  #define FileFlashPixView_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef PImageFile_h
  #include  "pimgfile.h"
#endif
#ifndef Fichier_h
  #include  "a_file.h"  // just for Mode
#endif
#ifndef FPXBaselineIO_h
  #include  "fpxlibio.h"
#endif

#ifdef macintosh
  #ifndef OLECommun_h
    #include "olecomm.h"
  #endif
#endif //macintosh

  class OLEFile;        // to not include OLEFiles.h
  class OLEStorage;     // to not include OLEStorages.h
  class OLEStream;      // to not include OLEStreams.h
  class OLEPropertySet;   // to not include OLEPropertySet.h
  class OLEPropertySection; // to not include OLEPropertySet.h
  class OLEProperty;      // to not include OLEProperties.h
#ifdef _WINDOWS
  #ifndef GUID_DEFINED
    #define GUID_DEFINED
    typedef struct _GUID
    {
      unsigned long Data1;
        unsigned short Data2;
        unsigned short Data3;
        unsigned char Data4[8];
    } GUID;
  #endif /* GUID_DEFINED */
  typedef GUID CLSID;
  #ifndef DWORD
    #define DWORD unsigned long
    #define DWORD_LOCAL_DEFINE
  #endif //DWORD
#endif // _WINDOWS
//  Constants
//  ---------

//  Types Declarations
//  ------------------

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PFileFlashPixView;

//  Classes Definitions
//  -------------------

  // File handle

  class PFileFlashPixView : public PImageFile  {
  
    friend class PFlashPixImageView;
    
    public:
                // CHG_VIS_OUT - added an optional argument that allows a visible
                //  output index to be specified. Normally, this is 0, which is
                //  correct for a core file that contains a single image.
                 PFileFlashPixView (FicNom& fName, const char* theStorageName, 
                          mode_Ouverture mode, long visibleOutputIndex = 0);
                 PFileFlashPixView (OLEStorage* theOwningStorage, const char* theStorageName,
                          mode_Ouverture mode, long visibleOutputIndex = 0);
                ~PFileFlashPixView ();
        
        void      Init ();

        Boolean     OpenStorage  (CLSID& classID, const char * name, OLEStorage ** res);
        Boolean     CreateStorage(CLSID& classID, const char * name, OLEStorage ** res);
        
        FPXStatus   Erreur() const;       // Result of last action on the file
        Boolean     Commit();
        FPXStatus   getStatus(); // { return oleFile->getFPXStatus(); };
        

        // Create CompObj in root storage 
        Boolean     CreateCompObj(); 
        
        // Read and write CompObj in root storage 
        Boolean     ReadCompObj(char **ppUserType, char **ppClipFormat); 
        Boolean     WriteCompObj(char *pUserType, char *pClipFormat); 

        Boolean     GetSummaryInfoProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetSummaryInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetGlobalInfoProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetGlobalInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetResultDescProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetResultDescProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetSourceDescProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetSourceDescProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetOperationProperty  (DWORD pID, OLEProperty ** res);
        Boolean     SetOperationProperty  (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetTransformProperty  (DWORD pID, OLEProperty ** res);
        Boolean     SetTransformProperty  (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetExtensionProperty  (DWORD pID, OLEProperty ** res);
        Boolean     SetExtensionProperty  (DWORD pID, DWORD propType, OLEProperty ** res);

        Boolean     GetExtensionNumber (LPWSTR theExtensionName, short *extNumber);
        Boolean     SetExtensionNumber (short *extNumber); 
        Boolean     Renew( short **ppa, short newEntry, short newSize);
        
        Boolean     InitSummaryInfoPropertySet();
        Boolean     GetSummaryInfoPropertySet();
        
        OLEStorage*   GetRootStorage() { return rootStorage; }
        
        Boolean     isReadOnly() { return readOnlyMode; }
        
        // PTCH_OBJFIX - added method to return the index of the Source Description Data
        //  object. This is also the index of the data storage containing the image data.
        long      GetSourceDescNumber() { return sourceDescPropertyIndex; }
        
      protected:
        OLEFile*      oleFile;          // the OLE file handle...
        Boolean       readOnlyMode;
                    
      
    virtual FPXStatus     GetError();

    private :
        DWORD       mode;   // Open Access Mode to the OLE file
        //OLEFile*      oleFile;          // the OLE file handle...

        OLEStorage *    parentStorage;        // ... or the parent storage handle
        OLEStorage *    rootStorage;        // the OLE root storage handle

        OLEPropertySet*   summaryInfoPropertySet;   // Summary Info property set handle
        
        OLEStream*      compObj;          // The CompObj stream

        OLEPropertySet*   globalInfoPropertySet;    // Global Info property set handle
        long        visibleOutputIndex;     // Which elem in Vis output is display image 

        OLEPropertySet*   resultDescPropertySet;    // Result Description property set handle
        long        resultDescPropertyIndex;  // Index number used to form it's name;

        OLEPropertySet*   sourceDescPropertySet;    // Source Description property set handle
        long        sourceDescPropertyIndex;  // Index number used to form it's name;

        OLEPropertySet*   transformPropertySet;   // Transform property set handle
        long        transformPropertyIndex;   // Index number used to form it's name;

        OLEPropertySet*   operationPropertySet;   // Operation property set handle
        long        operationPropertyIndex;   // Index number used to form it's name;

        OLEPropertySet*   extensionListPropertySet; // Extension list property set handle
        FPXShortArray   fpxExtensionList;       // Extension list number array
  };
  
#ifdef _WINDOWS
  #ifdef DWORD_LOCAL_DEFINE
    #undef DWORD
  #endif // DWORD_LOCAL_DEFINE
#endif
//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // FileFlashPixView_h
//  ----------------------------------------------------------------------------
