//  ----------------------------------------------------------------------------
//  MODULE    : FlashPixFormat 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Thursday, February 15, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)fpxformt.h  1.2 14:43:35 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef FlashPixFormat_h
  #define FlashPixFormat_h
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
#ifndef OLECommun_h
  #include "olecomm.h" 
#endif
#ifndef FPXBaselineIO_h
  #include  "fpxlibio.h"
#endif

  class OLEFile;        // to not include OLEFiles.h
  class OLEStorage;     // to not include OLEStorages.h
  class OLEStream;      // to not include OLEStreams.h
  class OLEPropertySet;   // to not include OLEPropertySet.h
  class OLEPropertySection; // to not include OLEPropertySet.h
  class OLEProperty;      // to not include OLEProperties.h

//  Constants
//  ---------

//  Types Declarations
//  ------------------
      
//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

  class PFlashPixFile;

//  Classes Definitions
//  -------------------

  // File handle

  class PFlashPixFile : public PImageFile  {
  
    public:
                 PFlashPixFile (ref_FicNom fName, const char* theStorageName, mode_Ouverture mode);
                 PFlashPixFile (OLEStorage* theOwningStorage, const char* theStorageName, mode_Ouverture mode);
                ~PFlashPixFile ();
        
        void      Init ();
        
        Boolean     OpenStorage  (CLSID& classID, const char * name, OLEStorage ** res);
        Boolean     CreateStorage(CLSID& classID, const char * name, OLEStorage ** res);
        
        virtual unsigned long GetFileSize();
        FPXStatus   Erreur() const;       // Result of last action on the file

        // Access the ICC profile in root storage by its index
        Boolean     SetICCProfile(WORD index, char *pICCProfileStr); 
        Boolean     GetICCProfile(WORD index, char **ppICCProfileStr); 
        
        // Create CompObj in root storage 
        Boolean     CreateCompObj(); 
        
        // Read and write CompObj in root storage 
        Boolean     ReadCompObj(char **ppUserType, char **ppClipFormat); 
        Boolean     WriteCompObj(char *pUserType, char *pClipFormat); 
        
        Boolean     Commit();
        Boolean     GetSummaryInfoProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetSummaryInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetImageContentProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetImageContentProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     GetImageInfoProperty (DWORD pID, OLEProperty ** res);
        Boolean     SetImageInfoProperty (DWORD pID, DWORD propType, OLEProperty ** res);
        Boolean     InitSummaryInfoPropertySet();
        Boolean     GetSummaryInfoPropertySet();
        Boolean     InitImageInfoPropertySet(); 
        
      protected:
      
    virtual FPXStatus     GetError();

    private :
        DWORD       mode;           // Open Access Mode to the OLE file
        
        OLEFile*      oleFile;          // the OLE file handle...
        OLEStorage *    parentStorage;        // ... or the parent storage handle
        OLEStorage *    rootStorage;        // the OLE root storage handle
        
        OLEPropertySet*   imageContentsPropertySet; // Image Contents property set handle

        OLEPropertySet*   imageInfoPropertySet;   // Image Info property set handle

        OLEPropertySet*   summaryInfoPropertySet;   // Summary Info property set handle
  };

//  'inline' Functions
//  ------------------

//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // FlashPixFormat_h
//  ----------------------------------------------------------------------------
