//  ----------------------------------------------------------------------------
//  MODULE    : OLEProperties.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file contains  OLE Property Set object 
// 
//  COMMENT   : An OLEProperty class contains a variant structure which holds 
//          all the data types defined in the property set (see also variant.h 
//          and OLEcomm.h ). Including, 
//
//              PROPERTY TYPE       DATA TYPE
//                VT_EMPTY,           no bytes
//                VT_NULL,            no bytes
//                VT_I1,    VT_UI1,   char/ unsigned char
//                VT_I2,    VT_UI2,   short/ unsigned short
//                VT_I4,    VT_UI4,   long/ unsigned long
//                VT_R4,              float
//                VT_R8,              double
//                VT_INT,   VT_UINT,  int/unsigned int
//                VT_I8,    VT_UI8,   int/unsigned int
//                VT_BOOL,            boolean (short)
//                VT_ERROR,           SCODE (unsigned long)
//                VT_CY,              CY ({ long, unsigned long })
//                VT_DATE,            DATE (double)
//                VT_FILETIME,        FILETIME ({unsigned long, unsigned long})
//                VT_LPSTR,           char * 
//                VT_BSTR,            char * 
//                VT_LPWSTR,          unsigned short * 
//                VT_BLOB,            BLOB ({unsigned long, unsigned char *})
//                VT_CF,              CLIPDATA ({unsigned long, unsigned long, unsigned char *})
//                VT_CLSID,           CLSID (two short, one long, 8 bytes)
//                VT_VARIANT,         VARIANT (four short, one double)
//                VT_VECTOR,          VECTOR (unsigned long, BYTE *)
//                VT_STREAM,          LPSTR 
//                VT_STORAGE,         LPSTR 
//                VT_STREAMED_OBJECT, LPSTR 
//                VT_STORED_OBJECT,   LPSTR 
//                VT_BLOB_OBJECT,     LPSTR 
//
//        OLEProperty class also supports data types defined in the FPXBaselineIO.h, 
//        Including, 
//
//                FPXStr,                     { unsigned long, unsigned char * } 
//                FPXShortArray,              { unsigned long, unsigned short * } 
//                FPXLongArray,               { unsigned long, unsigned long * } 
//                FPXRealArray,               { unsigned long, float * } 
//                FPXWideStr,                 { unsigned long, unsigned short * } 
//                FPXWideStrArray,            { unsigned long, FPXWideStr * } 
//                FPXStrArray,                { unsigned long, FPXStr * } 
//                FPXfiletime,                { unsigned long, unsigned long } 
//                FPXSpacialFrequencyResponseBlockArray  { unsigned long, FPXSpacialFrequencyResponseBlock* }
//                FPXCFA_PatternBlockArray    { unsigned long, FPXCFA_PatternBlock* }
//                FPXOECF_BlockArray          { unsigned long, FPXOECF_Block*}
//                FPXScannedImageSizeBlock    { float, float, FPXResolutionUnit }
//
//  SCCSID            : @(#)oleprop.cpp 1.5 10:45:17 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include  <iostream.h>
  #include  <fstream.h>
  
#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef FlashPixUtils_h
  #include "fpxutils.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEProperties_h
  #include  "oleprop.h"
#endif
#ifndef OLEPropertySet_h
  #include  "oleprops.h"
#endif
  
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
#ifdef _WINDOWS
  #pragma code_seg ("OLEProperties")
#else
  #pragma segment OLEProperties
#endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods
//  ----------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
//          OLEProperties functions
//-------------------------------------------------------------------------------------------

// This function creates an OLEProperty class from a property set and section with property ID
// and type specified
OLEProperty::OLEProperty (OLEPropertySet* parentPropSet, OLEPropertySection *parentPropSection, 
    DWORD properID, DWORD properType)
{
  parPropSet    = parentPropSet;                  // Keep the property set from which it is created
  OLEPropSec    = parentPropSection;              // Keep the property section from which it is created
  propID        = properID;                       // Store the property ID
  V_VT(&val)    = (unsigned short) properType;    // Store the property type
  len           = parPropSet->SizeVT(properType); // Get the size of property 
  Clear();                                        // Reset the variant structure in which alldata is 
  // going to be stored
  pDict = 0;                                      // FIX_DICT_PROP - clear dictionary pointer
}

OLEProperty::~OLEProperty ()
{
  // If the data has been dynamically allocated, delete it
  switch (val.vt)
  {
    case VT_LPSTR:
    case VT_BSTR: {
      delete V_UI1REF(&val);
      break; 
    }
    case VT_LPWSTR: {
      delete (unsigned short *)V_BYREF(&val); 
      break; 
    }
    case VT_BLOB: {
      BLOB *pblob = (BLOB *)V_BYREF(&val);  
      DeleteBLOB(pblob);  
      break; 
    }
    case VT_CF: {
      CLIPDATA *pcf = (CLIPDATA *)V_BYREF(&val);  
      DeleteCF(pcf);  
      break;
    }
  }   
  
  // If the data is VECTOR, delete it depending on the subtype
  if ( V_VT(&val) & VT_VECTOR ) 
  {
    VECTOR *pver = (VECTOR *)V_BYREF(&val); 
    DeleteVECTOR(pver, V_VT(&val)); 
  }
  
  // If the data is DICT_PROP, delete it and it's contents
  if ( pDict && ( GetPropType() == DICT_PROP_TYPE )) 
    DeleteDICTIONARY(pDict); 
  
}


//-------------------------------------------------------------------------------------
//  The following functions extract VT data types defined in variant.h from the 
//  OLEProperty class through operator =
//-------------------------------------------------------------------------------------

OLEProperty::operator short() const   { return short(V_I2(&val)); }
OLEProperty::operator WORD() const                  { return (WORD)(V_I2(&val)); }
OLEProperty::operator DWORD() const           { return (DWORD)(V_I4(&val)); }
OLEProperty::operator long() const    { return long(V_I4(&val)); }
OLEProperty::operator float() const   { return float(V_R4(&val)); }
OLEProperty::operator double() const    { return double(V_R8(&val)); }
OLEProperty::operator Boolean() const   { return (V_BOOL(&val) != 0); }
OLEProperty::operator CY() const    { return CY(V_CY(&val)); }
OLEProperty::operator FILETIME() const          { return *(FILETIME *)(&V_CY(&val)); }

OLEProperty::operator char*() const   
{ 
  char* str = (char *)V_UI1REF(&val); 
  
  return DuplicateStr(str); 
}

OLEProperty::operator WCHAR*() const  
{ 
  WCHAR* wstr = (WCHAR *)V_BYREF(&val); 
  
  return DuplicateWideStr(wstr); 
}

OLEProperty::operator CLSID*() const    
{ 
  CLSID *clsid = (CLSID *)V_BYREF(&val); 
  
  if ( !clsid )
    return NULL; 
  
  CLSID *cd = new CLSID;  
  if ( !cd )
    return NULL; 
  
  memcpy(cd, clsid, sizeof(CLSID)); 
  
  return cd; 
}

OLEProperty::operator BLOB*() const   
{ 
  BLOB *pblob = (BLOB *)V_BYREF(&val);
  
  return DuplicateBLOB(pblob);      
}

OLEProperty::operator CLIPDATA*() const 
{   
  CLIPDATA *pClipData = (CLIPDATA *)V_BYREF(&val);
  
  return DuplicateCF(pClipData);      
}

OLEProperty::operator VECTOR*() const   
{ 
  VECTOR *pvec = (VECTOR *)V_BYREF(&val);
  
  return DuplicateVECTOR(pvec, V_VT(&val));       
}

//-------------------------------------------------------------------------------------
//  The following functions assign VT data types defined in variant.h to the 
//  OLEProperty class through operator =
//-------------------------------------------------------------------------------------

short&    OLEProperty::operator=(const short& v)    { Clear(); V_I2(&val) = v; return V_I2(&val); }
WORD&   OLEProperty::operator=(const WORD& v)   { Clear(); V_I2(&val) = v; return (WORD&)V_I2(&val); }
long&   OLEProperty::operator=(const long& v)   { Clear(); V_I4(&val) = v; return V_I4(&val); }
DWORD&    OLEProperty::operator=(const DWORD& v)    { Clear(); V_I4(&val) = v; return (DWORD&)V_I4(&val); }
float&    OLEProperty::operator=(const float& v)    { Clear(); V_R4(&val) = v; return V_R4(&val); }
double&   OLEProperty::operator=(const double& v)   { Clear(); V_R8(&val) = v; return V_R8(&val); }
Boolean&  OLEProperty::operator=(const Boolean& v)  { Clear(); V_BOOL(&val) = v; return (Boolean&)V_BOOL(&val); }
CY&     OLEProperty::operator=(const CY& v)     { Clear(); V_CY(&val) = v; return V_CY(&val); }
FILETIME& OLEProperty::operator=(const FILETIME& v) { Clear(); V_CY(&val) = *((CY *)&v); return *((FILETIME *)&V_CY(&val)); }
// FILETIME&  OLEProperty::operator=(const FILETIME& v) { Clear(); *(FILETIME*)(V_CY(&val)) = v; return *(FILETIME*)(V_CY(&val)); }

// This function assigns LPSTR to the OLEProperty class through operator =
void OLEProperty::operator=(const char * v)
{
  // Save the string into variant, new memory is allocated
  len = VTtoVariant(&val, v); 
}

// This function assigns LPWSTR to the OLEProperty class through operator =
void OLEProperty::operator=(const WCHAR * v)
{
  // Save the wide string into variant, new memory is allocated
  len = VTtoVariant(&val, v); 

}

// This function assigns BLOB to the OLEProperty class through operator =
void OLEProperty::operator=(const BLOB* pblob)
{
  // Save the blob into variant, new memory is allocated
  len = VTtoVariant(&val, pblob); 

}

// This function assigns CLIPDATA to the OLEProperty class through operator =
void OLEProperty::operator=(const CLIPDATA * pcf)
{
  // Save the clipdata into variant, new memory is allocated
  len = VTtoVariant(&val, pcf); 

}

// This function assigns CLSID to the OLEProperty class through operator =
void OLEProperty::operator=(const CLSID * pcls)
{
  // Save the clsid into variant, new memory is allocated
  len = VTtoVariant(&val, pcls); 

}

// This function assigns VECTOR to the OLEProperty class through operator =
void OLEProperty::operator=(const VECTOR * pvector)
{
  // Save the vector into variant, new memory is allocated
  len = VTtoVariant(&val, pvector); 

}


//-------------------------------------------------------------------------------------
//  The following functions save VT type data into variant structure
//-------------------------------------------------------------------------------------

// This function duplicate the string and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const char * v)
{
  char* str; 
  
  // Duplicate a string
  if ( !( str = DuplicateStr(v)) ) 
    return 0; 
  
  // Delete the variant contents if it has been allocated before
  if (V_UI1REF(pvar))
    delete V_UI1REF(pvar); 
  
  // Clear the variant
  V_R8(pvar) = 0;

  // Save the string into variant.bstrVal
#if defined(_WINDOWS) 
  V_UI1REF(pvar) = (unsigned char *)str;
#else
  V_UI1REF(pvar) = (char *)str;
#endif

  return strlen(str);
}

// This function duplicate the wide string and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const WCHAR * v)
{
  WCHAR* wstr; 
  
  // Duplicate a wide string
  if ( !( wstr = DuplicateWideStr(v)) ) 
    return 0; 

  // Delete the variant contents if it has been allocated before
  if (V_BYREF(pvar))
    delete (char *)V_BYREF(pvar); 
  
  // Clear the variant
  V_R8(pvar) = 0;
  
  // Save the wide string into variant.byref
  V_BYREF(pvar) = (WCHAR *)wstr;

  return wcslen(wstr) * sizeof(WCHAR);
}

// This function duplicate the blob and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const BLOB * pblob)
{
  BLOB* pb; 
  
  // Duplicate a blob
  if ( !( pb = DuplicateBLOB(pblob)) ) 
    return 0; 
  
  // Delete the variant contents if it has been allocated before
  if (V_BYREF(pvar)) {
    BLOB *tempBlob = (BLOB *)V_BYREF(pvar); 
    DeleteBLOB(tempBlob);     
  }
    
  // Clear the variant
  V_R8(pvar) = 0;
  
  // Save the blob into variant.byref
  V_BYREF(pvar) = pb;

  return sizeof(DWORD) + pb->cbSize;
}

// This function duplicate the clipdata and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const CLIPDATA * pcf)
{
  CLIPDATA* pc; 
  
  // Duplicate a clipdata
  if ( !( pc = DuplicateCF(pcf)) ) 
    return 0; 
  
  // Delete the variant contents if it has been allocated before
  if (V_BYREF(pvar)) {
    CLIPDATA *tempcf = (CLIPDATA *)V_BYREF(pvar); 
    DeleteCF(tempcf);     
  }
    
  // Clear the variant
  V_R8(pvar) = 0;
  
  // Save the clipdata into variant.byref
  V_BYREF(pvar) = pc;

  return sizeof(DWORD) + pc->cbSize;
}

// This function duplicate the clsid and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const CLSID * pcls)
{
  // Allocate and copy the memory
  CLSID * pclsid  = new CLSID;    
  if (pclsid == NULL) {
    return 0;
  }
  memcpy(pclsid, pcls, sizeof(CLSID)); 

  // Delete the variant contents if it has been allocated before
  if (V_BYREF(pvar))
    delete (char *)V_BYREF(pvar); 
  
  // Clear the variant
  V_R8(pvar) = 0;
  
  // Save the clsid into variant.byref
  V_BYREF(pvar) = pclsid;

  return sizeof(CLSID);
}

// This function duplicate the vector and save it into variant
DWORD VTtoVariant(VARIANT* pvar, const VECTOR * pvector)
{
  DWORD type; 
  
  // If the type is not vector, return NULL
  if ( !(pvar->vt & VT_VECTOR) )
    return 0; 
  
  // Get the subtype of vector
  type  = pvar->vt; 
  type  ^= VT_VECTOR;
  
  // Allocate a new vector
  VECTOR * pvec = DuplicateVECTOR(pvector, type); 
  if ( !pvec ) {
    return 0;
  }

  // If the VECTOR already exists, delete it first
  VECTOR *pver = (VECTOR *)V_BYREF(pvar); 
  if ( pver )
    DeleteVECTOR(pver, V_VT(pvar)); 
  
  V_R8(pvar) = 0;

  // Put the vector pointer into variant.byref
  V_BYREF(pvar)   = pvec;

  return TRUE;
}


//-------------------------------------------------------------------------------------
//  The following functions extract data types defined in FPXBaselineIO.h from the 
//  OLEProperty class through operator =
//-------------------------------------------------------------------------------------

// This function extracts FPXStr from OLEProperty class through operator =
// The FPXStr is stored as VT_LPSTR type in OLEProperty class
OLEProperty::operator FPXStr() const  
{ 
  // Get the string pointer which is referenced by V_BSTR
  char *buf = (char *)V_UI1REF(&val);
  
  // Return the FPX string
  return LPSTRToFPXStr(buf);    
}

// This function extracts FPXWideStr from OLEProperty class through operator =
// The FPXWideStr is stored as VT_LPWSTR type in OLEProperty class
OLEProperty::operator FPXWideStr() const  
{ 
  return LPWSTRToFPXWideStr((LPWSTR)V_BYREF(&val)); 
}

// This function extracts FPXShortArray from OLEProperty class through operator =
// The FPXShortArray is stored as VT_UI2 | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXShortArray() const   
{ 
  return VectorToFPXShortArray((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXLongArray from OLEProperty class through operator =
// The FPXLongArray is stored as VT_UI4 | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXLongArray() const  
{ 
  return VectorToFPXLongArray((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXRealArray from OLEProperty class through operator =
// The FPXRealArray is stored as VT_R4 | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXRealArray() const  
{ 
  return VectorToFPXRealArray((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXClsIDArray from OLEProperty class through operator =
// The FPXClsIDArray is stored as VT_CLSID | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXClsIDArray() const   
{ 
  return VectorToFPXClsIDArray((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXStrArray from OLEProperty class through operator =
// The FPXStrArray is stored as VT_LPSTR | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXStrArray() const   
{ 
  return VectorToFPXStrArray((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXWideStrArray from OLEProperty class through operator =
// The FPXWideStrArray is stored as VT_LPWSTR | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXWideStrArray() const   
{ 
  return VectorToFPXWideStrArray((VECTOR *)V_BYREF(&val)); 
}
// PTCH_DCG - /**
// This function extracts FPXSpacialFrequencyResponseBlockArray from OLEProperty class 
// through operator = . The FPXSpacialFrequencyResponseBlockArray is stored as VT_VARIANT 
// | VT_VECTOR type in OLEProperty class
OLEProperty::operator FPXSpacialFrequencyResponseBlock() const  
{ 
  return VectorToFPXSpacialFrequencyResponseBlock((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXCFA_PatternBlockArray from OLEProperty class through 
// operator = . The FPXCFA_PatternBlockArray is stored as VT_VARIANT | VT_VECTOR type 
// in OLEProperty class
OLEProperty::operator FPXCFA_PatternBlock() const   
{ 
  return VectorToFPXCFA_PatternBlock((VECTOR *)V_BYREF(&val)); 
}

// This function extracts FPXOECF_BlockArray from OLEProperty class through 
// operator = . The FPXOECF_BlockArray is stored as VT_VARIANT | VT_VECTOR type 
// in OLEProperty class
OLEProperty::operator FPXOECF_Block() const   
{ 
  return VectorToFPXOECF_Block((VECTOR *)V_BYREF(&val)); 
}
// PTCH_DCG - **/
// This function extracts FPXScannedImageSizeBlock from OLEProperty class through 
// operator = . The FPXScannedImageSizeBlock is stored as VT_VARIANT | VT_VECTOR type 
// in OLEProperty class
OLEProperty::operator FPXScannedImageSizeBlock() const  
{ 
  return VectorToFPXScannedImageSizeBlock((VECTOR *)V_BYREF(&val)); 
}

OLEProperty::operator FPXOpticalFilterArray() const   
{ 
  return VectorToFPXOpticalFilterArray((VECTOR *)V_BYREF(&val)); 
}

//-------------------------------------------------------------------------------------
//  The following functions assign data types defined in FPXBaselineIO.h to the 
//  OLEProperty class through operator =
//-------------------------------------------------------------------------------------

// This function assigns FPXStr to the OLEProperty class through operator =
// The FPXStr is stored as VT_LPSTR type in OLEProperty class
const FPXStr& OLEProperty::operator=(const FPXStr& fpxstr)
{
  // Assign the FPXStr to the variant
  char * str = FPXStrToLPSTR(fpxstr); 
  *this = str; 
  
  if ( str )
    delete str; 
    
  return fpxstr;
}

// This function assigns FPXWideStr to the OLEProperty class through operator =
// The FPXWideStr is stored as VT_LPWSTR type in OLEProperty class
const FPXWideStr& OLEProperty::operator=(const FPXWideStr& fpxwstr)
{
  // Assign the FPXWideStr to the variant
  LPWSTR wstr = FPXWideStrToLPWSTR(fpxwstr);
  *this = wstr;  
    
  if ( wstr )
    delete wstr; 
    
  return fpxwstr;
}

// This function assigns FPXShortArray to the OLEProperty class through operator =
// The FPXShortArray is stored as VT_UI2 | VT_VECTOR type in OLEProperty class
const FPXShortArray& OLEProperty::operator=(const FPXShortArray& sa)
{
  // Convert the FPXShortArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXShortArrayToVector(sa);
  *this = vec; 
  DeleteVECTOR(vec, VT_I2);  
   
  return sa;
}

// This function assigns FPXLongArray to the OLEProperty class through operator =
// The FPXLongArray is stored as VT_UI4 | VT_VECTOR type in OLEProperty class
const FPXLongArray& OLEProperty::operator=(const FPXLongArray& la)
{
  // Convert the FPXLongArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXLongArrayToVector(la);
  *this = vec; 
  DeleteVECTOR(vec, VT_I4);  

  return la;
}

// This function assigns FPXRealArray to the OLEProperty class through operator =
// The FPXRealArray is stored as VT_R4 | VT_VECTOR type in OLEProperty class
const FPXRealArray& OLEProperty::operator=(const FPXRealArray& fa)
{
  // Convert the FPXRealArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXRealArrayToVector(fa);
  *this = vec; 
  DeleteVECTOR(vec, VT_R4);  
   
  return fa;
}

// This function assigns FPXClsIDArray to the OLEProperty class through operator =
// The FPXClsIDArray is stored as VT_CLSID | VT_VECTOR type in OLEProperty class
const FPXClsIDArray& OLEProperty::operator=(const FPXClsIDArray& ca)
{
  // Convert the FPXClsIDArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXClsIDArrayToVector(ca);
  *this = vec; 
  DeleteVECTOR(vec, VT_CLSID);  
   
  return ca;
}

// This function assigns FPXStrArray to the OLEProperty class through operator =
// The FPXStrArray is stored as VT_LPSTR | VT_VECTOR type in the OLEProperty class
const FPXStrArray& OLEProperty::operator=(const FPXStrArray& sa)
{
  // Convert the FPXStrArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXStrArrayToVector(sa);
  *this = vec; 
  DeleteVECTOR(vec, VT_LPSTR);  
   
  return sa;
}

// This function assigns FPXWideStrArray to the OLEProperty class through operator =
// The FPXWideStrArray is stored as VT_LPWSTR | VT_VECTOR type in the OLEProperty class
const FPXWideStrArray& OLEProperty::operator=(const FPXWideStrArray& wa)
{
  // Convert the FPXWideStrArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXWideStrArrayToVector(wa);
  *this = vec; 
  DeleteVECTOR(vec, VT_LPWSTR);  
   
  return wa;
}
// PTCH_DCG - /**
// This function assigns FPXSpacialFrequencyResponseBlockArray to the OLEProperty class 
// through operator = . The FPXSpacialFrequencyResponseBlockArray is stored as VT_VARIANT 
// | VT_VECTOR type in the OLEProperty class
const FPXSpacialFrequencyResponseBlock& OLEProperty::operator=(const FPXSpacialFrequencyResponseBlock& sfra)
{
  // Convert the FPXSpacialFrequencyResponseBlockArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXSpacialFrequencyResponseBlockToVector(sfra);
  *this = vec; 
  DeleteVECTOR(vec, VT_VARIANT);  
   
  return sfra;
}

// This function assigns FPXCFA_PatternBlockArray to the OLEProperty class through operator =
// The FPXCFA_PatternBlockArray is stored as VT_VARIANT | VT_VECTOR type in the OLEProperty class
const FPXCFA_PatternBlock& OLEProperty::operator=(const FPXCFA_PatternBlock& cpba)
{
  // Convert the FPXCFA_PatternBlockArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXCFA_PatternBlockToVector(cpba);
  *this = vec; 
  DeleteVECTOR(vec, VT_VARIANT);  
   
  return cpba;
}

// This function assigns FPXOECF_BlockArray to the OLEProperty class through operator =
// The FPXOECF_BlockArray is stored as VT_VARIANT | VT_VECTOR type in the OLEProperty class
const FPXOECF_Block& OLEProperty::operator=(const FPXOECF_Block& oba)
{
  // Convert the FPXOECF_BlockArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXOECF_BlockToVector(oba);
  *this = vec; 
  DeleteVECTOR(vec, VT_VARIANT);  
   
  return oba;
}
// PTCH_DCG - **/
// This function assigns FPXScannedImageSizeBlock to the OLEProperty class through operator =
// The FPXScannedImageSizeBlock is stored as VT_VARIANT | VT_VECTOR type in the OLEProperty class
const FPXScannedImageSizeBlock& OLEProperty::operator=(const FPXScannedImageSizeBlock& sisb)
{
  // Convert the FPXScannedImageSizeBlock to vector and assign to the OLEProperty
  VECTOR* vec = FPXScannedImageSizeBlockToVector(sisb);
  *this = vec; 
  DeleteVECTOR(vec, VT_VARIANT);  
   
  return sisb;
}

const FPXOpticalFilterArray& OLEProperty::operator=(const FPXOpticalFilterArray& la)
{
  // Convert the FPXLongArray to vector and assign to the OLEProperty
  VECTOR* vec = FPXOpticalFilterArrayToVector(la);
  *this = vec; 
  DeleteVECTOR(vec, VT_I4);  

  return la;
}

//-------------------------------------------------------------------------------------------
//   Global Functions
//
//  The following functions convert VT data types defined in variant.h to  those defined in 
//  FPXBaselineIO.h and vice versa 
//-------------------------------------------------------------------------------------------


// This function converts FPXStr to LPSTR
char* FPXStrToLPSTR(const FPXStr& fpxstr )    
{ 
  char* pstr;
  
  // Here we must allocate and copy the memory since we have to add a '\0' 
  // to the end of fpxstr.ptr 
  pstr = new char[fpxstr.length + 1];     
  if (pstr == NULL) {
    return NULL;
  }
  memcpy(pstr, fpxstr.ptr, fpxstr.length); 
  
  // Add a '\0' to the end of string 
  pstr[fpxstr.length] = '\0';
    
  return pstr; 
}

// This function converts FPXWideStr to LPWSTR
LPWSTR FPXWideStrToLPWSTR(const FPXWideStr& fpxwstr )   
{ 
  LPWSTR pwstr;
  
  // Here we must allocate and copy the memory since we have to add a '\0' 
  // to the end of fpxwstr.ptr 
  pwstr = new WCHAR[fpxwstr.length + 1];    
  if (pwstr == NULL) {
    return NULL;
  }
  memcpy(pwstr, fpxwstr.ptr, fpxwstr.length * sizeof(WCHAR)); 
  
  // Add a '\0' to the end of string 
  pwstr[fpxwstr.length] = 0;
    
  return pwstr; 
}

// PTCH_DCG - added method to copy an array of bytes in an FPXStr to a vector
//  of (unsigned) bytes. This is not NULL-terminated, but is padded to 32 bits
// This function converts FPXStr to VECTOR
VECTOR* FPXStrToVector(const FPXStr& sa )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_I1, sa.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->prgb, sa.ptr, pvec->cElements);
    
  return pvec; 
}

// This function converts FPXShortArray to VECTOR
VECTOR* FPXShortArrayToVector(const FPXShortArray& sa )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_I2, sa.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->prgw, sa.ptr, pvec->cElements * sizeof(short));
    
  return pvec; 
}

// This function converts FPXLongArray to VECTOR
VECTOR* FPXLongArrayToVector(const FPXLongArray& la )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_I4, la.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->prgdw, la.ptr, pvec->cElements * sizeof(long));
    
  return pvec; 
}

// This function converts FPXRealArray to VECTOR
VECTOR* FPXRealArrayToVector(const FPXRealArray& fa )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_R4, fa.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->prgflt, fa.ptr, pvec->cElements * sizeof(float));
    
  return pvec; 
}

// This function converts FPXClsIDArray to VECTOR
VECTOR* FPXClsIDArrayToVector(const FPXClsIDArray& ca )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_CLSID, ca.length);    
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->pclsid, ca.ptr, pvec->cElements * sizeof(CLSID));
    
  return pvec; 
}

// This function converts FPXStrArray to VECTOR
VECTOR* FPXStrArrayToVector(const FPXStrArray& sa )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_LPSTR, sa.length);    
  if (pvec == NULL) {
    return NULL;
  }
  
  // Here we must allocate and copy the memory since we have to add a '\0' 
  // to the end of string in FPXStrArray 
  for ( DWORD i= 0; i < pvec->cElements; i++ )
  {
    // Allocate the memory as for each string in the vector and copy the memory
    pvec->prgpsz[i] = new char[sa.ptr[i].length + 1];     
    if (pvec->prgpsz[i] == NULL) {
      return NULL;
    }

    memcpy(pvec->prgpsz[i], sa.ptr[i].ptr, sa.ptr[i].length * sizeof(char));
    
    // Add the '\0' to the end of the string
    pvec->prgpsz[i][sa.ptr[i].length] = '\0'; 
  }
  
  return pvec; 
}

// This function converts FPXWideStrArray to VECTOR
VECTOR* FPXWideStrArrayToVector(const FPXWideStrArray& wa )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_LPWSTR, wa.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Here we must allocate and copy the memory since we have to add a '\0' 
  // to the end of wide string in FPXWideStrArray 
  for ( DWORD i= 0; i < pvec->cElements; i++ )
  {
    // Allocate the memory as for each wide string in the vector and copy the memory
    pvec->prgpwz[i] = new WCHAR[wa.ptr[i].length + 1];    
    if (pvec->prgpwz[i] == NULL) {
      return NULL;
    }
    memcpy(pvec->prgpwz[i], wa.ptr[i].ptr, wa.ptr[i].length * sizeof(WCHAR));
    
    // Add the '\0' to the end of the wide string
    pvec->prgpwz[i][wa.ptr[i].length] = 0; 
  }
  
  return pvec; 
}

// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts FPXSpacialFrequencyResponseBlock to VECTOR
VECTOR* FPXSpacialFrequencyResponseBlockToVector(const FPXSpacialFrequencyResponseBlock& sfra )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  // Each FPXSpacialFrequencyResponseBlock has four elements ( two unsigned long, one 
  // FPXWideStrArray, one FPXRealArray)
  pvec = AllocVECTOR(VT_VARIANT, 4);    
  if (pvec == NULL) {
    return NULL;
  }

  V_VT(&pvec->pvar[0]) = VT_UI4;              // Save the variant type
  V_I4(&pvec->pvar[0]) = sfra.number_of_columns;      // Save the number_of_columns

  V_VT(&pvec->pvar[1]) = VT_UI4;              // Save the variant type
  V_I4(&pvec->pvar[1]) = sfra.number_of_rows;       // Save the number_of_rows

  V_VT(&pvec->pvar[2]) = VT_LPWSTR | VT_VECTOR;       // Save the variant type
  V_BYREF(&pvec->pvar[2]) = FPXWideStrArrayToVector(sfra.column_headings);  // Save the column_headings

  V_VT(&pvec->pvar[3]) = VT_R4 | VT_VECTOR;           // Save the variant type
  V_BYREF(&pvec->pvar[3]) = FPXRealArrayToVector(sfra.data);  // Save the data

  return pvec; 
}
  
// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts FPXCFA_PatternBlock to VECTOR
VECTOR* FPXCFA_PatternBlockToVector(const FPXCFA_PatternBlock& cpba )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  // Each FPXCFA_PatternBlock has three elements ( two unsigned short, one FPXStr) 
  pvec = AllocVECTOR(VT_VARIANT, 3);    
  if (pvec == NULL) {
    return NULL;
  }

  V_VT(&pvec->pvar[0]) = VT_UI2;              // Save the variant type
  V_I2(&pvec->pvar[0]) = cpba.cfa_repeat_rows;      // Save the cfa_repeat_rows

  V_VT(&pvec->pvar[1]) = VT_UI2;              // Save the variant type
  V_I2(&pvec->pvar[1]) = cpba.cfa_repeat_cols;      // Save the cfa_repeat_cols

  V_VT(&pvec->pvar[2]) = VT_UI1 | VT_VECTOR;        // Save the variant type
#ifdef _WINDOWS
  V_UI1REF(&pvec->pvar[2]) = (unsigned char *)FPXStrToVector(cpba.cfa_array);   // Save the cfa_array
#else
  V_UI1REF(&pvec->pvar[2]) = (char *)FPXStrToVector(cpba.cfa_array);  
#endif  

  return pvec; 
}

// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts FPXOECF_BlockArray to VECTOR
VECTOR* FPXOECF_BlockToVector(const FPXOECF_Block& oeba )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  // Each FPXOECF_Block has four elements ( two unsigned short, one FPXWideStrArray, one
  // FPXRealArray)
  pvec = AllocVECTOR(VT_VARIANT, 4);    
  if (pvec == NULL) {
    return NULL;
  }

  V_VT(&pvec->pvar[0]) = VT_UI2;              // Save the variant type
  V_I2(&pvec->pvar[0]) = oeba.number_of_columns;      // Save the number_of_columns

  V_VT(&pvec->pvar[1]) = VT_UI2;              // Save the variant type
  V_I2(&pvec->pvar[1]) = oeba.number_of_rows;       // Save the number_of_rows

  V_VT(&pvec->pvar[2]) = VT_LPWSTR | VT_VECTOR;                   // Save the variant type
  V_BYREF(&pvec->pvar[2]) = FPXWideStrArrayToVector(oeba.column_headings);  // Save the column_headings

  V_VT(&pvec->pvar[3]) = VT_R4 | VT_VECTOR;               // Save the variant type
  V_BYREF(&pvec->pvar[3]) = FPXRealArrayToVector(oeba.data);  // Save the data

  return pvec; 
}

// This function converts FPXScannedImageSizeBlock to VECTOR
VECTOR* FPXScannedImageSizeBlockToVector(const FPXScannedImageSizeBlock& sisb )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = new VECTOR;  
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and allocate the memory. 
  // Each FPXScannedImageSizeBlock has three elements ( two float, one unsigned long) 
  pvec->cElements = 3;  
  pvec->pvar = new VARIANT[pvec->cElements];  
  if (pvec->pvar == NULL) {
    return NULL;
  }
  
  // Save the three elements in the FPXScannedImageSizeBlock to a variant
  V_VT(&pvec->pvar[0]) = VT_R4;         // Save the variant type
  V_R4(&pvec->pvar[0]) = sisb.original_size_x;  // Save the original_size_x

  V_VT(&pvec->pvar[1]) = VT_R4;         // Save the variant type
  V_R4(&pvec->pvar[1]) = sisb.original_size_y;  // Save the original_size_y

  V_VT(&pvec->pvar[2]) = VT_UI4;          // Save the variant type
  V_I4(&pvec->pvar[2]) = sisb.original_size_unit;   // Save the original_size_unit

  return pvec; 
}
  
// This function converts FPXLongArray to VECTOR
VECTOR* FPXOpticalFilterArrayToVector(const FPXOpticalFilterArray& la )   
{ 
  VECTOR *pvec;
  
  // Allocate a new vector
  pvec = AllocVECTOR(VT_I4, la.length);   
  if (pvec == NULL) {
    return NULL;
  }
  
  // Set the vector length and assign the pointer
  memcpy(pvec->prgdw, la.ptr, pvec->cElements * sizeof(long));
    
  return pvec; 
}
  
// This function converts LPSTR to FPXStr
FPXStr& LPSTRToFPXStr( char* pstr )
{
  FPXStr *fpxstr = new FPXStr;
  
  if (pstr) {
    // Allocate for FPXStr
    fpxstr->length  = strlen(pstr) +1; 
    fpxstr->ptr   = new BYTE[fpxstr->length]; 
    if (fpxstr->ptr == NULL) {
      fpxstr->length = 0;
      return *fpxstr;
    }

    memcpy(fpxstr->ptr, pstr, fpxstr->length);
  } else {
    fpxstr->length  = 0;
    fpxstr->ptr   = NULL; 
  }

  return *fpxstr;
}

// This function converts LPWSTR to FPXWideStr
FPXWideStr& LPWSTRToFPXWideStr( LPWSTR pwstr )
{
  FPXWideStr *fpxwstr = new FPXWideStr; 
  
  if (pwstr) {
    // Allocate for FPXWideStr
    fpxwstr->length   = wcslen(pwstr) + 1; 
    fpxwstr->ptr    = (LPWSTR)new WCHAR[fpxwstr->length]; 
    if (fpxwstr->ptr == NULL) {
      fpxwstr->length = 0;
      return *fpxwstr;
    }

    memcpy(fpxwstr->ptr, pwstr, fpxwstr->length * sizeof(WCHAR));
  } else {
    fpxwstr->length = 0;
    fpxwstr->ptr  = NULL; 
  }

  return *fpxwstr;
}

// This function converts VECTOR to string of bytes with a long count
FPXStr& VectorToStr( VECTOR* vec )
{
  FPXStr *sa = new FPXStr; 
  
  if (vec) {
    // Allocate for FPXShortArray
    sa->length  = vec->cElements; 
    sa->ptr   = new unsigned char[sa->length];  
    if (sa->ptr == NULL) {
      sa->length = 0;
      return *sa;
    }
    memcpy(sa->ptr, vec->prgw, sa->length * sizeof(char));
  } else {
    sa->length  = 0; 
    sa->ptr   = NULL;
  }
  
  return *sa;
}

// This function converts VECTOR to FPXShortArray
FPXShortArray& VectorToFPXShortArray( VECTOR* vec )
{
  FPXShortArray *sa = new FPXShortArray; 
  
  if (vec) {
    // Allocate for FPXShortArray
    sa->length  = vec->cElements; 
    sa->ptr   = new unsigned short[sa->length]; 
    if (sa->ptr == NULL) {
      sa->length = 0;
      return *sa;
    }
    memcpy(sa->ptr, vec->prgw, sa->length * sizeof(short));
  } else {
    sa->length  = 0; 
    sa->ptr   = NULL;
  }
  
  return *sa;
}

// This function converts VECTOR to FPXLongArray
FPXLongArray& VectorToFPXLongArray( VECTOR* vec )
{
  FPXLongArray *la = new FPXLongArray; 
  
  if (vec) {
    // Allocate for FPXLongArray
    la->length  = vec->cElements; 
    la->ptr   = new unsigned long[la->length];    
    if (la->ptr == NULL) {
      la->length = 0;
      return *la;
    }

    memcpy(la->ptr, vec->prgdw, la->length * sizeof(long));
  } else {
    la->length  = 0; 
    la->ptr   = NULL;
  }

  return *la;
}

// This function converts VECTOR to FPXRealArray
FPXRealArray& VectorToFPXRealArray( VECTOR* vec )
{
  FPXRealArray *fa = new FPXRealArray; 
  
  if (vec) {
    // Allocate for FPXRealArray
    fa->length  = vec->cElements; 
    fa->ptr   = new float[fa->length];    
    if (fa->ptr == NULL) {
      fa->length = 0;
      return *fa;
    }

    memcpy(fa->ptr, vec->prgflt, fa->length * sizeof(float));
  } else {
    fa->length  = 0; 
    fa->ptr   = NULL;
  }

  return *fa;
}

// This function converts VECTOR to FPXClsIDArray
FPXClsIDArray& VectorToFPXClsIDArray( VECTOR* vec )
{
  FPXClsIDArray *ca = new FPXClsIDArray; 
  
  if (vec) {
    // Allocate for FPXClsIDArray
    ca->length  = vec->cElements; 
    ca->ptr   = new CLSID[ca->length];    
    if (ca->ptr == NULL) {
      ca->length = 0;
      return *ca;
    }

    memcpy(ca->ptr, vec->pclsid, ca->length * sizeof(CLSID));
  } else {
    ca->length  = 0; 
    ca->ptr   = NULL;
  }

  return *ca;
}

// This function converts VECTOR to FPXStrArray
FPXStrArray& VectorToFPXStrArray( VECTOR* vec )
{
  FPXStrArray *sa = new FPXStrArray; 

  if (vec) {
    // Set the array length and pointer
    sa->length  = vec->cElements; 
    sa->ptr   = new FPXStr[sa->length]; 
    if (sa->ptr == NULL) {
      sa->length = 0;
      return *sa;
    }

    for ( DWORD i= 0; i < sa->length; i++ ) {
      sa->ptr[i].length   = strlen(vec->prgpsz[i]); 
      sa->ptr[i].ptr    = new BYTE[sa->ptr[i].length];  
      if (sa->ptr[i].ptr == NULL) {
        sa->ptr[i].length = 0;
        return *sa;
      }

      memcpy(sa->ptr[i].ptr, vec->prgpsz[i], sa->ptr[i].length); 
    }
    
  } else {
    sa->length  = 0; 
    sa->ptr   = NULL;
  }
  
  return *sa;
}

// This function converts VECTOR to FPXWideStrArray
FPXWideStrArray& VectorToFPXWideStrArray( VECTOR* vec )
{
  FPXWideStrArray *wa = new FPXWideStrArray; 

  if (vec) {
    // Set the array length and allocate the memory
    wa->length  = vec->cElements; 
    wa->ptr   = new FPXWideStr[wa->length]; 
    if (wa->ptr == NULL) {
      wa->length = 0;
      return *wa;
    }

    for ( DWORD i= 0; i < wa->length; i++ ) {
      wa->ptr[i].length   = wcslen(vec->prgpwz[i]) + 1;
      wa->ptr[i].ptr    = new WCHAR[wa->ptr[i].length]; 
      if (wa->ptr[i].ptr == NULL) {
        wa->ptr[i].length = 0;
        return *wa;
      }
      memcpy(wa->ptr[i].ptr, vec->prgpwz[i], wa->ptr[i].length * sizeof(WCHAR)); 
    }
  
  } else {
    wa->length  = 0; 
    wa->ptr   = NULL;
  }
  
  return *wa;
}

// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts VECTOR to FPXSpacialFrequencyResponseBlockArray
FPXSpacialFrequencyResponseBlock& VectorToFPXSpacialFrequencyResponseBlock( VECTOR* pvec )
{
  FPXSpacialFrequencyResponseBlock *sfra = new FPXSpacialFrequencyResponseBlock; 

  if (pvec) {
    // Each FPXSpacialFrequencyResponseBlock has four elements ( two unsigned long, one 
    // FPXStrArray, one FPXRealArray)
    sfra->number_of_columns = V_I4(&pvec->pvar[0]);   // Save the number_of_columns
    sfra->number_of_rows  = V_I4(&pvec->pvar[1]);   // Save the number_of_rows
    sfra->column_headings   = VectorToFPXWideStrArray((VECTOR *)V_BYREF(&pvec->pvar[2])); // Save the column_headings
    sfra->data        = VectorToFPXRealArray((VECTOR *)V_BYREF(&pvec->pvar[3]));  // Save the data
  }

  return *sfra;
}

// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts VECTOR to FPXCFA_PatternBlockArray
FPXCFA_PatternBlock& VectorToFPXCFA_PatternBlock( VECTOR* pvec )
{
  FPXCFA_PatternBlock *cpba = new FPXCFA_PatternBlock; 

  if (pvec) {
    cpba->cfa_repeat_rows   = V_I2(&pvec->pvar[0]);   // Save the cfa_repeat_rows
    cpba->cfa_repeat_cols = V_I2(&pvec->pvar[1]);   // Save the cfa_repeat_cols
#ifdef _WINDOWS   
    cpba->cfa_array = VectorToStr((VECTOR *)V_BYREF(&pvec->pvar[2])); // Save the cfa_array
//    cpba->cfa_array = LPSTRToFPXStr((char *)V_UI1REF(&pvec->pvar[2]));  // Save the cfa_array
#else
    cpba->cfa_array = VectorToStr((VECTOR *)V_BYREF(&pvec->pvar[2])); // Save the cfa_array
#endif  
  }
  return *cpba;
}

// PTCH_DCG - removed array implementation and updated string type to meet spec
// This function converts VECTOR to FPXOECF_BlockArray
FPXOECF_Block& VectorToFPXOECF_Block( VECTOR* pvec )
{
  FPXOECF_Block *oba = new FPXOECF_Block; 
  
  oba->number_of_columns  = V_I2(&pvec->pvar[0]);   // Save the number_of_columns
  oba->number_of_rows   = V_I2(&pvec->pvar[1]);   // Save the number_of_rows
  oba->column_headings  = VectorToFPXWideStrArray((VECTOR *)V_BYREF(&pvec->pvar[2])); // Save the column_headings
  oba->data       = VectorToFPXRealArray((VECTOR *)V_BYREF(&pvec->pvar[3]));  // Save the data
  
  return *oba;
}

// This function converts VECTOR to FPXScannedImageSizeBlock
FPXScannedImageSizeBlock& VectorToFPXScannedImageSizeBlock( VECTOR* pvec )
{
  FPXScannedImageSizeBlock *sisb = new FPXScannedImageSizeBlock; 

  if (pvec) {
    // Save three variants to a FPXScannedImageSizeBlock 
    sisb->original_size_x     = V_R4(&pvec->pvar[0]);   // Save the original_size_x
    sisb->original_size_y   = V_R4(&pvec->pvar[1]);   // Save the original_size_y
    sisb->original_size_unit  = (FPXResolutionUnit)V_I4(&pvec->pvar[2]);    // Save the original_size_unit
  } else {
    sisb->original_size_x     = (float) 0.0;
    sisb->original_size_y   = (float) 0.0;
    sisb->original_size_unit  = (FPXResolutionUnit)0;
  }
  
  return *sisb;
}

// This function converts VECTOR to FPXOpticalFilterArray
FPXOpticalFilterArray& VectorToFPXOpticalFilterArray( VECTOR* vec )
{
  FPXOpticalFilterArray *la = new FPXOpticalFilterArray; 

  if(vec)
  {
    la->length  = vec->cElements; 
    la->ptr   = (FPXSpecialEffectsOpticalFilter*)new unsigned long[la->length];   
    if (la->ptr == NULL) {
      la->length = 0;
      return *la;
    }
    memcpy(la->ptr, vec->prgdw, la->length * sizeof(long));
  } else {
    la->length  = 0; 
    la->ptr   = NULL;
  }
  return *la;
}
