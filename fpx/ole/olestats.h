//  ----------------------------------------------------------------------------
//  MODULE    : OLEEnumStatstg.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines the OLEEnumStatstg object 
//  SCCSID      : @(#)olestats.h  1.1 11:53:31 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef OLEEnumStatstg_h
  #define OLEEnumStatstg_h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECore_h
  #include "olecore.h"
#endif
  class OLEStorage;
    
//  Constants
//  ---------
  
//  Class declarations 
//  ------------------

  class OLEEnumStatstg;
  
//  Class definitions
//  -----------------

  class OLEEnumStatstg : public OLECore {
        
  public:
    // This constructor creates a OLE statstg object from an open storage
            OLEEnumStatstg(OLEStorage * parentStorage, IEnumSTATSTG * currentEnumStatstg);
            virtual ~OLEEnumStatstg();
    
    // This function returns true if next object exists, the information is 
    // returned the STATSTG object
    virtual Boolean Next( STATSTG *);

    // This function returns true if the object specified in STATSTG is storage
    virtual Boolean IsStorage( STATSTG *);

    // This function returns true if the object specified in STATSTG is property set
    virtual Boolean IsStream( STATSTG * );

    // This function skips the number of objects specified from current position
    virtual Boolean IsPropertySet( STATSTG * );

    // This function skips the number of objects specified from current position
    virtual Boolean Skip( unsigned long );

    // This function sets the current object to the first one in the storage
    virtual Boolean Reset();
  
  private:
    OLEStorage *  parStorage;   // open storage to which OLEEnumStatstg belongs 
    IEnumSTATSTG *  enumStatstg;  // pointer of IEnumSTATSTG object
  };


//  ----------------------------------------------------------------------------
  #endif // OLEEnumStatstg_h
//  ----------------------------------------------------------------------------
