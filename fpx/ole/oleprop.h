//  ----------------------------------------------------------------------------
//  MODULE    : OLEProperty.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang 
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines OLE Property object 
//  COMMENT   : Enhanced OLE objects which are independent of platform
//    SCCSID            : @(#)oleprop.h 1.5 10:45:20 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEProperties_h
  #define OLEProperties_h
  
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef FPXBaselineIO_h
  #include "fpxlibio.h" 
#endif
#ifndef SystemToolkit_h
  #include "ri_sys.h"
#endif

  class OLEPropertySection; // to avoid include OLEPropertySet.h
  class OLEPropertySet;   // to avoid include OLEPropertySet.h


// FIX_DICT_PROP ...start addition 
//  Property set dictionary declarations (formerly in OLEPROPS.H)
//  -----------------------------------------
  typedef struct tagENTRY
  {
    DWORD dwPropID; 
    DWORD cb; 
    char  *sz;
  } ENTRY; 
  
  typedef struct tagDICTIONARY
  {
    DWORD cbEntries; 
    ENTRY *rgEntry; 
  } DICTIONARY; 

  // FIX_DICT_PROP ...end of addition 
  // Property types are enum'ed in VARIANT.H which is not a file officially under the
  //  control of FlashPix. Since it can't be edited, a property type was added here
  //  which hopefully will not collide with any changes to the list in VARIANT.H
  //
  #define DICT_PROP_TYPE  0x0C00
  
// FIX_DICT_PROP ...end of addition 
  
//  Constants
//  ---------

  // Property id zero
  #define PROPERTYIDZERO  0
  
  // Property id one
  #define PROPERTYIDONE 1

  // Code page 
  #define CODEPAGE    1200
  
//  Class declarations 
//  ------------------

  class OLEProperty;
  class OLEBlob;

  
//  Class definitions
//  -----------------

  class OLEProperty : public PToolkitObject {

  friend class OLEPropertySection; 
  
  public:
                  OLEProperty (OLEPropertySet* parentPropSet, 
                        OLEPropertySection *parentPropSection, 
                        DWORD properID, 
                        DWORD properType);
                  ~OLEProperty();
    
    // Set the variant structure to zero
    virtual   void      Clear()               {  V_R8(&val) = 0;    }; 
    
    // Get the property ID 
    virtual   unsigned long GetPropID()             {  return propID;     }; 
    
    // Get the property type
    virtual   unsigned long GetPropType()             {  return V_VT(&val); }; 
    
    // Get the property offset
    virtual   unsigned long GetPropOffset()           {  return propOffset; }; 
    
    // Set the property offset
    virtual   void      SetPropOffset(unsigned long off)    {  propOffset = off;  }; 
    
    // Following functions define the VT data types in variant.h that can be assigned 
    // to a OLEProperty class through operator=
    virtual   short&      operator=(const short& v);
    virtual   WORD&     operator=(const WORD& v);
    virtual   long&     operator=(const long& v);
    virtual   DWORD&      operator=(const DWORD& v);
    virtual   float&      operator=(const float& v);
    virtual   double&     operator=(const double& v);
    virtual   Boolean&    operator=(const Boolean& v);
    virtual   CY&       operator=(const CY& v);
    virtual   FILETIME&   operator=(const FILETIME& v);
    virtual   void      operator=(const char* v);
    virtual   void      operator=(const WCHAR* v);
    virtual   void      operator=(const BLOB* v);
    virtual   void      operator=(const CLIPDATA* v);
    virtual   void      operator=(const CLSID* v);
    virtual   void      operator=(const VECTOR* v);
    
    // Following functions define the VT data types in variant.h that can be extracted 
    // from an OLEProperty class through operator=
    virtual   operator short() const;
    virtual   operator WORD() const;
    virtual   operator DWORD() const;
    virtual   operator long() const;
    virtual   operator float() const;
    virtual   operator double() const;
    virtual   operator Boolean() const;
    virtual   operator CY() const;
    virtual   operator FILETIME() const;
    virtual   operator char*() const;
    virtual   operator WCHAR*() const;
    virtual   operator BLOB*() const;
    virtual   operator CLIPDATA*() const;
    virtual   operator CLSID*() const;
    virtual   operator VECTOR*() const;

    // Following functions define the data types in FPXBaselineIO.h that can be 
    // assigned to an OLEProperty class through operator=
    virtual   const FPXStr&     operator=(const FPXStr& fpxstr);
    virtual   const FPXWideStr&   operator=(const FPXWideStr& fpxwstr);
    virtual   const FPXShortArray&  operator=(const FPXShortArray& sa);
    virtual   const FPXLongArray& operator=(const FPXLongArray& la);
    virtual   const FPXRealArray& operator=(const FPXRealArray& fa);
    virtual   const FPXClsIDArray&  operator=(const FPXClsIDArray& ca);
    virtual   const FPXWideStrArray&operator=(const FPXWideStrArray& wa);
    virtual   const FPXStrArray&  operator=(const FPXStrArray& sa);
    // PTCH_DCG - reinstated the following 3 prototypes
    virtual   const FPXSpacialFrequencyResponseBlock& operator=( 
            const FPXSpacialFrequencyResponseBlock& sfra);
    virtual   const FPXCFA_PatternBlock&  operator=(const  FPXCFA_PatternBlock& cpba); 
    virtual   const FPXOECF_Block&      operator=(const  FPXOECF_Block& oba); 
    virtual   const FPXScannedImageSizeBlock&   operator=(const  FPXScannedImageSizeBlock& susb);
    virtual   const FPXOpticalFilterArray&  operator=(const  FPXOpticalFilterArray& ofa); 
  
    // Following functions define the data types in FPXBaselineIO.h that can be 
    // extracted from an OLEProperty class through operator=
    virtual   operator FPXStr() const;    
    virtual   operator FPXWideStr() const;    
    virtual   operator FPXShortArray() const;   
    virtual   operator FPXLongArray() const;    
    virtual   operator FPXRealArray() const;    
    virtual   operator FPXClsIDArray() const;   
    virtual   operator FPXStrArray() const;   
    virtual   operator FPXWideStrArray() const;   
    // PTCH_DCG - reinstated the following 3 prototypes
    virtual   operator FPXSpacialFrequencyResponseBlock() const;    
    virtual   operator FPXCFA_PatternBlock() const;   
    virtual   operator FPXOECF_Block() const;   
    virtual   operator FPXScannedImageSizeBlock() const;
    virtual   operator FPXOpticalFilterArray() const;   
  
  protected:
    DWORD       len;  // Length of the property
    VARIANT       val;  // Value of the property itself (or null)
    
    // FIX_DICT_PROP - Added a dictionary property member. This is an alternative
    //  to a VARIANT. A DICTIONARY is unlike other properties and never occurs in
    //  a VARIANT. One difference of significance is that the property ID for the
    //  dictionary is 0.
    //  The OLEProperty may contain one or the other, but not both.
    //  
    //  
    DICTIONARY*     pDict;      // Pointer to dictionary property (or null)
    
    OLEPropertySet*   parPropSet;   // The OLEPropertySet from which this is created
    OLEPropertySection* OLEPropSec;   // The OLEPropertySection from which this is created
    DWORD       propOffset;     // The offset of this property 
    DWORD       propID;     // The ID of this property
  };

  
//-------------------------------------------------------------------------------------------
//   Global Functions
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
//  The following functions save VT type data into variant structure
//-------------------------------------------------------------------------------------

  // This function duplicate the string and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const char * v); 

  // This function duplicate the wide string and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const WCHAR * v); 
  
  // This function duplicate the blob and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const BLOB * pblob); 
  
  // This function duplicate the clipdata and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const CLIPDATA * pcf);
  
  // This function duplicate the clsid and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const CLSID * pcls); 

  // This function duplicate the vector and save it into variant
  DWORD VTtoVariant(VARIANT* pvar, const VECTOR * pvector);
  
//-------------------------------------------------------------------------------------------
//  The following functions convert VT data types defined in variant.h to  those defined in 
//  FPXBaselineIO.h and vice versa 
//-------------------------------------------------------------------------------------------

  // This function converts FPXStr to LPSTR
  char* FPXStrToLPSTR(const FPXStr& fpxstr );     

  // This function converts FPXWideStr to LPWSTR
  LPWSTR FPXWideStrToLPWSTR(const FPXWideStr& fpxwstr );    

  // PTCH_DCG - added prototype
  // This function converts an array of bytes in FPXStr to vector
  VECTOR* FPXStrToVector(const FPXStr& sa );  

  // This function converts FPXShortArray to VECTOR
  VECTOR* FPXShortArrayToVector(const FPXShortArray& sa );    

  // This function converts FPXLongArray to VECTOR
  VECTOR* FPXLongArrayToVector(const FPXLongArray& la );    

  // This function converts FPXRealArray to VECTOR
  VECTOR* FPXRealArrayToVector(const FPXRealArray& fa );    
  
  // This function converts FPXClsIDArray to VECTOR
  VECTOR* FPXClsIDArrayToVector(const FPXClsIDArray& ca ); 
      
  // This function converts FPXStrArray to VECTOR
  VECTOR* FPXStrArrayToVector(const FPXStrArray& sa );    
  
  // This function converts FPXWideStrArray to VECTOR
  VECTOR* FPXWideStrArrayToVector(const FPXWideStrArray& wa );    

  // PTCH_DCG - reinstated
  // This function converts FPXSpacialFrequencyResponseBlock to VECTOR
  VECTOR* FPXSpacialFrequencyResponseBlockToVector(const FPXSpacialFrequencyResponseBlock& sfra );    

  // This function converts FPXCFA_PatternBlock to VECTOR
  VECTOR* FPXCFA_PatternBlockToVector(const FPXCFA_PatternBlock& cpba );    

  // This function converts FPXOECF_Block to VECTOR
  VECTOR* FPXOECF_BlockToVector(const FPXOECF_Block& oeba );    

  // This function converts FPXScannedImageSizeBlock to VECTOR
  VECTOR* FPXScannedImageSizeBlockToVector(const FPXScannedImageSizeBlock& sisb );  
  
  // This function converts FPXOpticalFilterArray to VECTOR
  VECTOR* FPXOpticalFilterArrayToVector(const FPXOpticalFilterArray& sisb );      

  // This function converts LPSTR to FPXStr
  FPXStr& LPSTRToFPXStr( char* pstr );
  
  // This function converts LPWSTR to FPXWideStr
  FPXWideStr& LPWSTRToFPXWideStr( LPWSTR pwstr );

  // This function converts string of bytes to string with long count
  FPXStr& VectorToStr( VECTOR* vec );
  
  // This function converts VECTOR to FPXShortArray
  FPXShortArray& VectorToFPXShortArray( VECTOR* vec );
  
  // This function converts VECTOR to FPXLongArray
  FPXLongArray& VectorToFPXLongArray( VECTOR* vec );
  
  // This function converts VECTOR to FPXRealArray
  FPXRealArray& VectorToFPXRealArray( VECTOR* vec );
  
  // This function converts VECTOR to FPXClsIDArray
  FPXClsIDArray& VectorToFPXClsIDArray( VECTOR* vec ); 
  
  // This function converts VECTOR to FPXStrArray
  FPXStrArray& VectorToFPXStrArray( VECTOR* vec );
  
  // This function converts VECTOR to FPXWideStrArray
  FPXWideStrArray& VectorToFPXWideStrArray( VECTOR* vec );
  
  // PTCH_DCG - reinstated next 3
  // This function converts VECTOR to FPXSpacialFrequencyResponseBlock
  FPXSpacialFrequencyResponseBlock& VectorToFPXSpacialFrequencyResponseBlock( VECTOR* pvec );

  // This function converts VECTOR to FPXCFA_PatternBlock
  FPXCFA_PatternBlock& VectorToFPXCFA_PatternBlock( VECTOR* pvec );

  // This function converts VECTOR to FPXOECF_Block
  FPXOECF_Block& VectorToFPXOECF_Block( VECTOR* pvec );
  
  // This function converts VECTOR to FPXScannedImageSizeBlock
  FPXScannedImageSizeBlock& VectorToFPXScannedImageSizeBlock( VECTOR* pvec );
  
  // This function converts VECTOR to FPXOpticalFilterArray
  FPXOpticalFilterArray& VectorToFPXOpticalFilterArray( VECTOR* pvec );
  
  
//  ----------------------------------------------------------------------------
  #endif // OLEProperties_h
//  ----------------------------------------------------------------------------
