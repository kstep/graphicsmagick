//  ----------------------------------------------------------------------------
//  MODULE    : OLEPropertySet.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an OLE Property Set object 
//  COMMENT   : Enhanced OLE objects are platform independent 
//      SCCSID            : @(#)oleprops.h  1.3 10:45:26 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEPropertySet_h
  #define OLEPropertySet_h
  
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLEHeaderStream_h
  #include  "olehstrm.h"
#endif
#ifndef SystemToolkit_h
  #include "ri_sys.h"
#endif
  class OLEStorage;   // to avoid include OLEStorages.h
  class OLEProperty;    // to avoid include OLEProperties.h
  
//  Constants
//  ---------
  
  
//  OLE property list structures declarations
//  -----------------------------------------


//  Property set section header declarations
//  -----------------------------------------

  typedef struct tagSECTIONHEADER
  {
    GUID  sectionID; 
    DWORD sectionOffset; 
  } SECTIONHEADER; 
  
  typedef struct tagPROPERTYHEADER
  {
    DWORD cbSection; 
    DWORD cProperties; 
  } PROPERTYHEADER; 
  
  
//  Property set dictionary declarations
//  -----------------------------------------
// FIX_DICT_PROP - see OLEPROP.H 
  
//  Class declarations 
//  ------------------

  class OLEPropertySet;
  class OLEPropertySection;
  
//  Class definitions
//  -----------------

  class OLEPropertySet : public OLEHeaderStream {
  
  public:
                OLEPropertySet(const GUID& classID, OLEStorage * parentStorage, LPSTREAM currentStream);
                ~OLEPropertySet();
    
    virtual Boolean     GetProperty(DWORD propID, OLEProperty ** ppNewProperty); 
    virtual Boolean     NewProperty( DWORD propID, DWORD propType, OLEProperty ** ppNewProperty );
    virtual Boolean     DeleteProperty(DWORD propID ); 
    virtual Boolean     GetSection(const GUID& sectionID, OLEPropertySection** propSet); 
    
    virtual Boolean     Commit(); 
    virtual Boolean     Revert(); 
    virtual Boolean     Display(ofstream * logStream); 

  private:
    OLEPropertySection*   pPropSection;   // Array of property sections
  };


  class OLEPropertySection : public PToolkitObject {
  
  friend class OLEPropertySet; 
  
  public:
                // Use this constrctor when reading a section
                OLEPropertySection( OLEPropertySet * parentPropSet);

                // Use this constructor when creating a section
                OLEPropertySection( OLEPropertySet * parentPropSet, const GUID& theSecID );

                ~OLEPropertySection();
  
  protected:
    virtual Boolean     GetProperty(DWORD propID, OLEProperty ** ppNewProperty ); 
    virtual Boolean     GetProperty(DWORD propID, OLEProperty ** ppNewProperty, DWORD propCount); 
    virtual Boolean     NewProperty( DWORD propID, DWORD propType, OLEProperty ** ppNewProperty );
    virtual Boolean     DeleteProperty(DWORD propID ); 

    virtual Boolean     Renew( OLEProperty* newProp, short propCount);    
    virtual unsigned long GetSecOffset()            {   return sectionOffset; }; 
    virtual void      SetSecOffset( unsigned long off ) {   sectionOffset = off;  }; 
    virtual unsigned long NumOfProp()             {   return numOfProp;     }; 
    virtual unsigned long GetSectionSize()          {   return sectionSize;   }; 
    virtual GUID&     SectionID()             {   return secID;         }; 
    virtual void      SetPropOffsetSecSize(); 
    
    virtual Boolean     Read(); 
    virtual Boolean     Write(); 
    virtual Boolean     Display(ofstream * pLogStream); 

    OLEPropertySet *    parPropSet;   // Parent property set
    OLEProperty **      ppOLEProp;    // Array of properties
    unsigned long       numOfProp;    // Number of properties
    unsigned long     sectionOffset;  // Section offset in the property set stream
    unsigned long     sectionSize;  // Section size (in bytes)
    GUID          secID;      // Section Format Identifier
};

//  ----------------------------------------------------------------------------
  #endif // OLEPropertySet_h
//  ----------------------------------------------------------------------------
