//  ----------------------------------------------------------------------------
//  MODULE    : OLECore.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an object which is common to all enhanced 
//          OLE objects 
//  COMMENT   : Enhanced OLE objects which are platform independent 
//    SCCSID            : @(#)olecore.h 1.4 10:45:14 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef OLECore_h
  #define OLECore_h 
//  ----------------------------------------------------------------------------

  #ifndef SystemToolkit_h
    #include "ri_sys.h"
  #endif
  
  #ifndef OLECommun_h
    #include "olecomm.h"
  #endif

  #ifndef OLEProperties_h
    #include "oleprop.h"
  #endif
  
  #include "fpxlibio.h"

//  ----------------------------------------------------------------------------
//  Constants
//  ----------------------------------------------------------------------------

// These strings are used to print the types of the properties in the test code

static const char* variantName[] =
{
    "VT_EMPTY",     // 0
  "VT_NULL",
  "VT_I2",
  "VT_I4",
  "VT_R4",
  "VT_R8",      // 5
  "VT_CY",
  "VT_DATE",
  "VT_BSTR",
  "VT_DISPATCH",
  "VT_ERROR",     // 10 
  "VT_BOOL",
  "VT_VARIANT",
  "VT_UNKNOWN",
  "?",
  "?",        // 15
  "VT_I1",
  "VT_UI1",
  "VT_UI2",
  "VT_UI4",
  "VT_I8",      // 20
  "VT_UI8",
  "VT_INT",
  "VT_UINT",
  "VT_VOID",
  "VT_HRESULT",   // 25
  "VT_PTR",
  "VT_SAFEARRAY",
  "VT_CARRAY",
  "VT_USERDEFINED",
  "VT_LPSTR",     // 30
  "VT_LPWSTR",
  "?",
  "?",
  "?",
  "?",        // 35
  "?",
  "?",
  "?",
  "?",
  "?",        // 40
  "?",
  "?",
  "?",
  "?",
  "?",        // 45
  "?",
  "?",
  "?",
  "?",
  "?",        // 50
  "?",
  "?",
  "?",
  "?",
  "?",        // 55
  "?",
  "?",
  "?",
  "?",
  "?",        // 60
  "?",
  "?",
  "?",
  "VT_FILETIME",    // 64
  "VT_BLOB",
  "VT_STREAM",
  "VT_STORAGE",
  "VT_STREAMED_OBJECT",
  "VT_STORED_OBJECT",
  "VT_BLOB_OBJECT", // 70
  "VT_CF",
  "VT_CLSID",
  "VT_VECTOR",
  "VT_ARRAY" 
};

//  ----------------------------------------------------------------------------


// Size of FPX Stream header
#define DEFAULTSTREAMHEADERLENGTH   28

// If the byte-order indicator from the property list header isn't equal to
// this, then the bytes must be switched.
#define BYTE_ORDER_INDICATOR  0xfffe

// Format version for the property list header. Always zero.
#define FORMAT_VERSION      0

// A null GUID 
const GUID BlankGUID = { 0, 0, 0, { 0, 0,  0,  0,  0,  0,  0,  0 } };

// This is the originating OS word for the property list header.
#undef ORIGINATING_OS_VERSION
#ifdef _MAC
#define ORIGINATING_OS_VERSION  0x0001  // Macintosh
#endif
#ifdef _WIN32
#define ORIGINATING_OS_VERSION  0x0002  // 32-bit Windows
#endif
#ifdef __unix
#define ORIGINATING_OS_VERSION  0x0003  // generic UNIX identifier
#endif
#ifndef ORIGINATING_OS_VERSION
#define ORIGINATING_OS_VERSION  0x0000  // 16-bit Windows
#endif

//  ----------------------------------------------------------------------------
  
//  Classes declarations 
//  --------------------

  class PToolkitObject;
  class OLECore;
  class OLEBlob; 
  class List; 
  
//  Classes definitions
//  -------------------

  // OLE core class Definitions:
  
  class  OLECore : public PToolkitObject {
  
    public:
               OLECore();
               virtual ~OLECore();
              
          OSErr   Error() { return lastError; };      
          OSErr     TranslateOLEError(HRESULT err);  // Translate the scode message 
          FPXStatus getFPXStatus() { return fpxStatus; };
          void    setFPXStatus(FPXStatus status) {fpxStatus = status; };
          FPXStatus OLEtoFPXError(HRESULT err);

      virtual   void  AddRef();
      virtual   void  Release();      

    protected:
      OSErr     lastError;    // Current error status
      FPXStatus fpxStatus;    // Current error status in FPX error format
      long    userCount;    // The number of objects reference this object
  };    


  // Blob class Definitions:
  
  class OLEBlob : public PToolkitObject {
  public:
        OLEBlob();
        OLEBlob(const unsigned long size);
        OLEBlob(BLOB* inputBlob);
        ~OLEBlob();
    
    Boolean operator=(BLOB* inputBlob); 
    
    // Read VT type data from blob
    DWORD   ReadVT_I1(BYTE *pb); 
    DWORD ReadVT_I2(WORD  *pw);
    DWORD ReadVT_I4(DWORD *pdw);
    DWORD ReadVT_R4(float *pflt);
    DWORD ReadVT_R8(double *pdbl);
    DWORD ReadVT_LPSTR(char **ppsz);
    DWORD ReadVT_LPWSTR(WCHAR **ppwsz);
    DWORD   ReadVT_VECTOR(BYTE **ppByte); 

    // Write VT type data to blob
    DWORD   WriteVT_I1(BYTE   b); 
    DWORD WriteVT_I2(WORD   pw);    
    DWORD WriteVT_I4(DWORD  pdw);
    DWORD WriteVT_R4(float  pflt);
    DWORD WriteVT_R8(double   pdbl);
    DWORD WriteVT_LPSTR(char  *psz);
    DWORD WriteVT_LPWSTR(WCHAR *pwsz);
    DWORD   WriteVT_VECTOR(BYTE *pbyte, DWORD veclen); 
    
    // Set the pointer to the beginning of buffer
    void  ResetPointer();

    // Set the pointer to the position which is of offset to the current position
    void  SetPointer(long offset);
    
    // Get the BLOB struct in OLEBlob
    BLOB* GetBlob();
    
    // Get the BLOB size in OLEBlob
    DWORD GetBlobSize();
    
  private:
  
    // Generic read/write functions for blob
    Boolean Read(void * dataPtr, const long size);
    Boolean Write(const void *dataPtr, const long size);

    BLOB  blob;     // Blob struct as it is defined by Microsoft in OAIDL.H
    BYTE* buffer;     // The buffer itself containing the raw data
    BYTE* bufPtr;     // The current pointer on this buffer
  };


  // Link list class Definitions:
  
  class  List : public PToolkitObject {
  
    public:
             List();
             List(void* thePtr, const char *theName);
            ~List();
              
      Boolean   Add(void* thePtr, const char *theName); // Add an OLE object to the list  
      Boolean   Delete(const char *theName);      // Delete an OLE object in the list 
      void*   Search(const char *theName);      // Search an OLE object in the list 
      List*   Locate(const char *theName);      // Locate an OLE object in the list 
      List*   GetEndOfList();             // Find the last element in the list  
      
      long    GetUserCount() { return userCount; }; // Return list

      void    AddRef(); // Add an OLE object to the list  
      void    Release();  // Add an OLE object to the list  

    protected:
      char*     name;     // Name of OLE object
      void*   ptr;    // Pointer to OLE object 
      List*   next;     // Pointer to next element
      long    userCount;  // The number of objects reference this object
  };    

  
//  Functions 'inline'
//  ------------------

inline BLOB* OLEBlob::GetBlob()           { return &blob; }
inline DWORD OLEBlob::GetBlobSize()         { return blob.cbSize; }

inline void  OLEBlob::ResetPointer()        { bufPtr = buffer;  }
inline void  OLEBlob::SetPointer(long offset)   { bufPtr = ((bufPtr + offset) < buffer) ? buffer : (bufPtr + offset);
                            bufPtr = (bufPtr > buffer + blob.cbSize) ? buffer + blob.cbSize : bufPtr; }

//  Global variables
//  ----------------

  extern List * openRootStorageList;    // List for all opened OLE root storage (referenced by OLEFile name)    


//  Global functions            
//  ----------------

  // This function replaces substring with another one in a specified string,
  // return NULL if failed
  char*   strrep(const char* s1, const char *t1, const char *t2, char *s2); 
  
  // This function duplicate a string. Notice: the returned string is allocated and should be deleted  
  // after use. 
  char*   DuplicateStr(const char* s); 
  
  // This function duplicate a wide char string. Notice: the returned string is allocated and should be deleted  
  // after use. 
  WCHAR*  DuplicateWideStr(const WCHAR* ws); 
  
  // This function duplicate a blob. Notice: the returned blob is allocated and should be deleted  
  // after use. Return null if fails 
  BLOB*   DuplicateBLOB(const BLOB* pblob); 
  
  // This function delete a blob 
  void  DeleteBLOB(BLOB* pblob); 
  
  // This function duplicate a clipdata. Notice: the returned clipdata is allocated and should be deleted  
  // after use. Return null if fails 
  CLIPDATA* DuplicateCF(const CLIPDATA* pClipData); 
  
  // This function delete a clipdata 
  void  DeleteCF(CLIPDATA* pcf);
  
  // This function allocate memory for vector. Notice: the returned vector is allocated and should be deleted  
  // after use. Return null if fails 
  VECTOR* AllocVECTOR(long type, long elemCount); 
  
  // This function duplicate a vector. Notice: the returned vector is allocated and should be deleted  
  // after use. Return null if fails 
  VECTOR* DuplicateVECTOR(const VECTOR* pvector, long type); 
  
  // This function delete a vector 
  void  DeleteVECTOR(VECTOR* pvec, long type); 
  
#ifndef USE_UNICODE
  // This function converts the Unicode string to ASCII string
  char*   WideCharToMultiByte(const WCHAR * pwchar);
  
  // This function converts ASCII string to Unicode string
  WCHAR * MultiByteToWideChar(const char * pchar);
  
  // This function calculates the length of Unicode string
  // DWORD wcslen(const WCHAR * pwchar); 
#endif

  // This function returns the name string of a variant type
  char*       VariantName(DWORD vtType); 
  
  // Generic routine for switching endian order in an arbitrary data type. 
  void      SwapBytes(unsigned char*, short );

  HRESULT     OLEInit();      // Initialize the OLE library
  Boolean     OLEUninit();    // Uninitialize the OLE library

  // FIX_DICT_PROP - Added AllocDICT_PROP and DeleteDICT_PROP.
  // This function allocates entries for dictionary. Notice: the returned vector 
  //  is allocated and should be deleted after use. Return null if fails 
  DICTIONARY* AllocDICTIONARY(long elemCount); 
    
  // This function deletes a dictionary 
  void  DeleteDICTIONARY(DICTIONARY* pDict); 

//  ----------------------------------------------------------------------------
  #endif // OLECore_h
//  ----------------------------------------------------------------------------
