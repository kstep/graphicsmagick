//  ----------------------------------------------------------------------------
//  MODULE    : OLECore.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an object which is common to all enhanced 
//          OLE objects 
//  COMMENT   : Enhanced OLE objects which are platform independent 
//    SCCSID            : @(#)olecore.cpp 1.5 10:45:11 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
  

//  Includes
//  --------
#ifndef Debug_h
  #include "debug.h"
#endif

#if defined(_WINDOWS)
  #include "b_error.h"
#endif
#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEProperties_h
  #include "oleprop.h"
#endif

//  Constants
//  ---------

//  Global variables
//  ----------------

  List * openRootStorageList = NULL; 
  
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLECore
//  ----------------------------------------------------------------------------

OLECore::OLECore( )
{
  lastError = noErr;
  fpxStatus = FPX_OK;
  userCount  = 1;
}
    
OLECore::~OLECore( )
{

}

void OLECore::AddRef()
{
  userCount++; 
}

void OLECore::Release()
{
  userCount--;
}

FPXStatus OLECore::OLEtoFPXError(HRESULT err)
{
  FPXStatus terr;
  unsigned short errCode, errFac, errSev;
  SCODE sCode = GetScode(err);
  
  errSev = SCODE_SEVERITY(sCode);
  errCode = SCODE_CODE(sCode);
  errFac = SCODE_FACILITY(sCode);
  
  switch (sCode) {
    case S_OK:  terr = FPX_OK;
          break;
    case STG_E_INVALIDFUNCTION:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_FILENOTFOUND:
          terr = FPX_FILE_NOT_FOUND;
          break;
    case STG_E_PATHNOTFOUND:
          terr = FPX_FILE_NOT_FOUND;
          break;
    case STG_E_TOOMANYOPENFILES:
          terr = FPX_FILE_SYSTEM_FULL;
          break;
    case STG_E_ACCESSDENIED:
          terr = FPX_FILE_WRITE_ERROR;
          break;
    case STG_E_INVALIDHANDLE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_INSUFFICIENTMEMORY:
          terr = FPX_MEMORY_ALLOCATION_FAILED;
          break;
    case STG_E_INVALIDPOINTER:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_NOMOREFILES:
          terr = FPX_FILE_SYSTEM_FULL;
          break;
    case STG_E_DISKISWRITEPROTECTED:
          terr = FPX_ERROR;
          break;
    case STG_E_SEEKERROR:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_WRITEFAULT:
          terr = FPX_FILE_WRITE_ERROR;
          break;
    case STG_E_READFAULT:
          terr = FPX_FILE_READ_ERROR;
          break;
    case STG_E_SHAREVIOLATION:
          terr = FPX_FILE_IN_USE;
          break;
    case STG_E_LOCKVIOLATION:
          terr = FPX_FILE_IN_USE;
          break;
    case STG_E_FILEALREADYEXISTS:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case STG_E_INVALIDPARAMETER:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_MEDIUMFULL:
          terr = FPX_FILE_SYSTEM_FULL;
          break;
    case STG_E_ABNORMALAPIEXIT:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_INVALIDHEADER:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case STG_E_INVALIDNAME:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case STG_E_UNKNOWN:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_UNIMPLEMENTEDFUNCTION:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_INVALIDFLAG:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_INUSE:
          terr = FPX_FILE_IN_USE;
          break;
    case STG_E_NOTCURRENT:
          terr = FPX_FILE_NOT_OPEN_ERROR;
          break;
    case STG_E_REVERTED:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_CANTSAVE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_OLDFORMAT:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case STG_E_OLDDLL:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case STG_E_SHAREREQUIRED:
          terr = FPX_FILE_IN_USE;
          break;
    case STG_E_NOTFILEBASEDSTORAGE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case STG_E_EXTANTMARSHALLINGS:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_UNEXPECTED:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_NOTIMPL:
          terr = FPX_UNIMPLEMENTED_FUNCTION;
          break;
    case E_OUTOFMEMORY:
          terr = FPX_MEMORY_ALLOCATION_FAILED;
          break;
    case E_INVALIDARG:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_NOINTERFACE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_POINTER:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_HANDLE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_ABORT:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_FAIL:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case E_ACCESSDENIED:
          terr = FPX_FILE_WRITE_ERROR;
          break;
    case OLE_E_OLEVERB:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_ADVF:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_ENUM_NOMORE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_ADVISENOTSUPPORTED:
          terr = FPX_UNIMPLEMENTED_FUNCTION;
          break;
    case OLE_E_NOCONNECTION:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_NOTRUNNING:
          terr = FPX_SEVER_INIT_ERROR;
          break;
    case OLE_E_NOCACHE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_BLANK:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_CLASSDIFF:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case OLE_E_CANT_GETMONIKER:
          terr = FPX_INVALID_FORMAT_ERROR;
          break;
    case OLE_E_CANT_BINDTOSOURCE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_STATIC:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_PROMPTSAVECANCELLED:
          terr = FPX_USER_ABORT;
          break;
    case OLE_E_INVALIDRECT:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_WRONGCOMPOBJ:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_INVALIDHWND:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_NOT_INPLACEACTIVE:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_CANTCONVERT:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_NOSTORAGE:
          terr = FPX_FILE_NOT_OPEN_ERROR;
          break;
#ifdef macintosh  // Mac specific errors from ole2.h
    case OLE_E_NOEXTENSION:
          terr = FPX_SEVER_INIT_ERROR;
          break;
    case OLE_E_VERSEXTENSION:
          terr = FPX_SEVER_INIT_ERROR;
          break;
    case OLE_E_IPBUSY:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_NOT_FRONT_PROCESS:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_WRONG_MENU:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_MENU_NOT_PATCHED:
          terr = FPX_OLE_FILE_ERROR;
          break;
    case OLE_E_MENUID_NOT_HASHED:
          terr = FPX_OLE_FILE_ERROR;
          break;
#endif // macintosh
    default:  terr = FPX_OLE_FILE_ERROR;

          break;
  }
  
  return terr;
}
  
// This function translates the error in form of scode to normal error code
OSErr OLECore::TranslateOLEError(HRESULT err)
{
  OSErr terr;
  // DAG
  unsigned short errCode, errFac, errSev;
  SCODE sCode = GetScode(err);
  
  errSev = SCODE_SEVERITY(sCode);
  errCode = SCODE_CODE(sCode);
  errFac = SCODE_FACILITY(sCode);
  // end DAG

  switch (sCode) {
    case S_OK:  terr = noErr;
          break;
    case STG_E_INVALIDFUNCTION:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_FILENOTFOUND:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_PATHNOTFOUND:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_TOOMANYOPENFILES:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_ACCESSDENIED:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_INVALIDHANDLE:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_INSUFFICIENTMEMORY:
          terr = SEVERITY_ERROR;
          break;
    case STG_E_INVALIDPOINTER:
          terr = -1;
          break;
    case STG_E_NOMOREFILES:
          terr = -1;
          break;
    case STG_E_DISKISWRITEPROTECTED:
          terr = -1;
          break;
    case STG_E_SEEKERROR:
          terr = -1;
          break;
    case STG_E_WRITEFAULT:
          terr = -1;
          break;
    case STG_E_READFAULT:
          terr = -1;
          break;
    case STG_E_SHAREVIOLATION:
          terr = -1;
          break;
    case STG_E_LOCKVIOLATION:
          terr = -1;
          break;
    case STG_E_FILEALREADYEXISTS:
          terr = -1;
          break;
    case STG_E_INVALIDPARAMETER:
          terr = -1;
          break;
    case STG_E_MEDIUMFULL:
          terr = -1;
          break;
    case STG_E_ABNORMALAPIEXIT:
          terr = -1;
          break;
    case STG_E_INVALIDHEADER:
          terr = -1;
          break;
    case STG_E_INVALIDNAME:
          terr = -1;
          break;
    case STG_E_UNKNOWN:
          terr = -1;
          break;
    case STG_E_UNIMPLEMENTEDFUNCTION:
          terr = -1;
          break;
    case STG_E_INVALIDFLAG:
          terr = -1;
          break;
    case STG_E_INUSE:
          terr = -1;
          break;
    case STG_E_NOTCURRENT:
          terr = -1;
          break;
    case STG_E_REVERTED:
          terr = -1;
          break;
    case STG_E_CANTSAVE:
          terr = -1;
          break;
    case STG_E_OLDFORMAT:
          terr = -1;
          break;
    case STG_E_OLDDLL:
          terr = -1;
          break;
    case STG_E_SHAREREQUIRED:
          terr = -1;
          break;
    case STG_E_NOTFILEBASEDSTORAGE:
          terr = -1;
          break;
    case STG_E_EXTANTMARSHALLINGS:
          terr = -1;
          break;
    case E_UNEXPECTED:
          terr = -1;
          break;
    case E_NOTIMPL:
          terr = -1;
          break;
    case E_OUTOFMEMORY:
          terr = -1;
          break;
    case E_INVALIDARG:
          terr = -1;
          break;
    case E_NOINTERFACE:
          terr = -1;
          break;
    case E_POINTER:
          terr = -1;
          break;
    case E_HANDLE:
          terr = -1;
          break;
    case E_ABORT:
          terr = -1;
          break;
    case E_FAIL:
          terr = -1;
          break;
    case E_ACCESSDENIED:
          terr = -1;
          break;
    case OLE_E_OLEVERB:
          terr = -1;
          break;
    case OLE_E_ADVF:
          terr = -1;
          break;
    case OLE_E_ENUM_NOMORE:
          terr = -1;
          break;
    case OLE_E_ADVISENOTSUPPORTED:
          terr = -1;
          break;
    case OLE_E_NOCONNECTION:
          terr = -1;
          break;
    case OLE_E_NOTRUNNING:
          terr = -1;
          break;
    case OLE_E_NOCACHE:
          terr = -1;
          break;
    case OLE_E_BLANK:
          terr = -1;
          break;
    case OLE_E_CLASSDIFF:
          terr = -1;
          break;
    case OLE_E_CANT_GETMONIKER:
          terr = -1;
          break;
    case OLE_E_CANT_BINDTOSOURCE:
          terr = -1;
          break;
    case OLE_E_STATIC:
          terr = -1;
          break;
    case OLE_E_PROMPTSAVECANCELLED:
          terr = -1;
          break;
    case OLE_E_INVALIDRECT:
          terr = -1;
          break;
    case OLE_E_WRONGCOMPOBJ:
          terr = -1;
          break;
    case OLE_E_INVALIDHWND:
          terr = -1;
          break;
    case OLE_E_NOT_INPLACEACTIVE:
          terr = -1;
          break;
    case OLE_E_CANTCONVERT:
          terr = -1;
          break;
    case OLE_E_NOSTORAGE:
          terr = -1;
          break;
#ifdef macintosh  // Mac specific errors from ole2.h
    case OLE_E_NOEXTENSION:
          terr = -1;
          break;
    case OLE_E_VERSEXTENSION:
          terr = -1;
          break;
    case OLE_E_IPBUSY:
          terr = -1;
          break;
    case OLE_E_NOT_FRONT_PROCESS:
          terr = -1;
          break;
    case OLE_E_WRONG_MENU:
          terr = -1;
          break;
    case OLE_E_MENU_NOT_PATCHED:
          terr = -1;
          break;
    case OLE_E_MENUID_NOT_HASHED:
          terr = -1;
          break;
#endif // macintosh
    default:  terr = -1;
          break;
  }
  
  return terr;
}


//-------------------------------------------------------------------------------------------
//          OLEBlob functions
//-------------------------------------------------------------------------------------------

// Create a blob by default size
OLEBlob::OLEBlob()
{
  // Set size and allocate memory for blob
  blob.cbSize = 0;
  buffer = NULL;
  blob.pBlobData = buffer;

  // Set the pointer to the beginning of blob data
  ResetPointer();
}

// Create a blob by size
OLEBlob::OLEBlob(const unsigned long size)
{
  // Set size and allocate memory for blob
  blob.cbSize = size;
  buffer = new BYTE[size];  
  if (buffer==NULL) {
    blob.cbSize=0;
  }

  blob.pBlobData = buffer;

  // Set the pointer to the beginning of blob data
  ResetPointer();
}

// Create a blob from another blob
OLEBlob::OLEBlob(BLOB* inputBlob)
{
  // Set size and allocate memory for blob
  blob.cbSize = inputBlob->cbSize;
  buffer = new BYTE[blob.cbSize]; 
  if (buffer==NULL) {
    blob.cbSize=0;
  }
  else {    
    // Copy memory 
    memcpy(buffer, inputBlob->pBlobData, inputBlob->cbSize);
  }
  blob.pBlobData = buffer;

  // Set the pointer to the beginning of blob data
  ResetPointer();
}

OLEBlob::~OLEBlob()
{
  if (blob.cbSize) 
    delete[] buffer;
}

// Assign a blob 
Boolean OLEBlob::operator=(BLOB* inputBlob)
{
  // Check the validity of input blob
  if ( !inputBlob)
    return false;
  
  if (blob.cbSize)
    delete buffer;
    
  // Set size and allocate memory for blob
  blob.cbSize = inputBlob->cbSize;
  buffer = new BYTE[blob.cbSize];
  
  if ( !buffer)
    return false;
  
  // Copy memory 
  memcpy(buffer, inputBlob->pBlobData, inputBlob->cbSize);
  blob.pBlobData = buffer;

  // Set the pointer to the beginning of blob data
  ResetPointer();

  return true;
}

// Generic read function for blob
Boolean OLEBlob::Read(void *dataPtr, const long size)
{
  if ((long)(bufPtr + size - buffer) > (long)blob.cbSize)
    return false;

  // Read the data from the blob buffer referenced by bufPtr  
  memcpy(dataPtr, bufPtr, size);
  bufPtr += size;
  
  return true;
}

// Read a unsigned char type from blob. Return the total bytes read, which is 1 bytes ( char )
DWORD OLEBlob::ReadVT_I1(BYTE *pb)
{
  // Read a unsigned char type from the blob buffer referenced by bufPtr  
  if ( !Read(pb, sizeof(char)) )
    return false;
  
  return sizeof(char);
}

// Read a short type from blob. Return the total bytes read, which is 2 bytes ( short )
DWORD OLEBlob::ReadVT_I2(WORD *pw)
{
  // Read a short type from the blob buffer referenced by bufPtr  
  if ( !Read(pw, sizeof(short)) )
    return false;
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)pw, sizeof(short));   
  #endif

  return sizeof(short);
}

// Read a long type from blob. Return the total bytes read, which is 4 bytes ( long )
DWORD OLEBlob::ReadVT_I4(DWORD *pdw)
{
  // Read a long type from the blob buffer referenced by bufPtr 
  if ( !Read(pdw, sizeof(long)) )
    return false;
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)pdw, sizeof(long));   
  #endif

  return sizeof(long);
}

// Read a float type from blob. Return the total bytes read, which is 4 bytes ( float )
DWORD OLEBlob::ReadVT_R4(float *pflt)
{
  // Read a float type from the blob buffer referenced by bufPtr  
  if ( !Read(pflt, sizeof(float)) )
    return false;
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)pflt, sizeof(float));   
  #endif

  return sizeof(float);
}

// Read a double type from blob. Return the total bytes read, which is 8 bytes ( double )
DWORD OLEBlob::ReadVT_R8(double *pdbl)
{
  // Read a double type from the blob buffer referenced by bufPtr 
  if ( !Read(pdbl, sizeof(double)) )
    return false;
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)pdbl, sizeof(double));  
  #endif

  return sizeof(double);
}

// Read a byte array from blob. Return the total array length 
DWORD OLEBlob::ReadVT_VECTOR(BYTE **ppByte)
{
  // Allocating memory for string
  *ppByte = new BYTE[blob.cbSize];  
  if (*ppByte==NULL) {
    return false;
  }
  
  // Read the string  
  if ( !Read(*ppByte, blob.cbSize) )
    return false;
  
  return blob.cbSize;
}

// Read a string from blob. Return the total string length 
DWORD OLEBlob::ReadVT_LPSTR(char **ppsz)
{
  DWORD dwLength;
  
  // Read the string length first including the terminating NULL
  if ( !ReadVT_I4(&dwLength) )
    return false;
  
  // Allocating memory for string
  *ppsz = new char[dwLength];   
  if (*ppsz==NULL) {
    return false;
  }
  
  // Read the string  
  if ( !Read(*ppsz, dwLength) )
    return false;
  
  return dwLength;
}

// Read a wide string from blob. Return the total length ( bytes count )
DWORD OLEBlob::ReadVT_LPWSTR(WCHAR **ppwsz)
{
  DWORD dwLength;
  
  // Read the WCHAR count first including the terminating NULL
  if ( !ReadVT_I4(&dwLength) )
    return false;
  
  // Allocating memory for LPWSTR
  *ppwsz = new WCHAR[dwLength]; 
  if (*ppwsz==NULL) {
    return false;
  }
  
  // Read the LPWSTR  
  if ( !Read(*ppwsz, dwLength * sizeof(WCHAR)) )
    return false;
  
  return dwLength * sizeof(WCHAR);
}

// Generic write function for blob
Boolean OLEBlob::Write(const void *dataPtr, const long size)
{
  // Get the already used buffer length 
  DWORD usedSize = bufPtr - buffer; 
  
  // If current allocate memory is not enough for new data, reallocate memory
  if (( usedSize + size ) > blob.cbSize)  {
  
    // Allocate memory for new length
    BYTE *tempBuf = new BYTE[usedSize + size];
    if ( !tempBuf)
      return false;
        
    // Copy the old buffer to new one and delete it
    memcpy(tempBuf, buffer, usedSize * sizeof(char)); 
    if ( buffer )
      delete buffer; 
    
    // Reset the buffer and buffer pointer
    buffer = tempBuf; 
    bufPtr = buffer + usedSize; 
    
    // Reset the blob size and buffer
    blob.cbSize = usedSize + size;
    blob.pBlobData = buffer;
  }
    
  // Write the data to the blob buffer referenced by bufPtr 
  memcpy(bufPtr, dataPtr, size);
  bufPtr += size;
  
  return true;
}

// Write a unsigned char type to blob. Return the total bytes write, which is 1 bytes ( char )
DWORD OLEBlob::WriteVT_I1(BYTE b)
{
  // Write a unsigned char type to the blob buffer referenced by bufPtr 
  if ( !Write(&b, sizeof(char)) )
    return false;
  
  return sizeof(char);
}

// Write a short type to blob. Return the total bytes write, which is 2 bytes ( short )
DWORD OLEBlob::WriteVT_I2(WORD pw)
{
  WORD w = pw; 
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)&w, sizeof(short));   
  #endif

  // Write a short type to the blob buffer referenced by bufPtr 
  if ( !Write(&w, sizeof(short)) )
    return false;
  
  return sizeof(short);
}

// Write a long type to blob. Return the total bytes write, which is 4 bytes ( long )
DWORD OLEBlob::WriteVT_I4(DWORD pdw)
{
  DWORD dw = pdw; 
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)&dw, sizeof(long));   
  #endif

  // Write a long type to the blob buffer referenced by bufPtr  
  if ( !Write(&dw, sizeof(long)) )
    return false;
  
  return sizeof(long);
}

// Write a float type to blob. Return the total bytes write, which is 4 bytes ( float )
DWORD OLEBlob::WriteVT_R4(float pflt)
{
  float flt = pflt; 
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)&flt, sizeof(float));   
  #endif

  // Write a float type to the blob buffer referenced by bufPtr 
  if ( !Write(&flt, sizeof(float)) )
    return false;
  
  return sizeof(float);
}

// Write a double type to blob. Return the total bytes write, which is 8 bytes ( double )
DWORD OLEBlob::WriteVT_R8(double pdbl)
{
  double dbl = pdbl; 
  
  // Swap bytes if necessary
  #ifdef  IN_BIG_ENDIAN
    SwapBytes((BYTE *)&dbl, sizeof(double));  
  #endif

  // Write a double type to the blob buffer referenced by bufPtr  
  if ( !Write(&dbl, sizeof(double)) )
    return false;
  
  return sizeof(double);
}

// Write a byte array to blob. Return the total array length
DWORD OLEBlob::WriteVT_VECTOR(BYTE *pbyte, DWORD veclen)
{
  // Write the byte array to the blob buffer referenced by bufPtr 
  if ( !Write(pbyte, veclen * sizeof(char)) )
    return false;
  
  return veclen * sizeof(char);
}

// Write a string to blob. Return the total string length
DWORD OLEBlob::WriteVT_LPSTR(char *psz)
{
  DWORD dwLength = strlen(psz) + 1; // Count includes terminating zero
  
  // Write the string length, including terminating NULL  
  if ( !WriteVT_I4(dwLength) )
    return false;
  
  // Write the string to the blob buffer referenced by bufPtr 
  if ( !Write(psz, dwLength * sizeof(char)) )
    return false;
  
  return dwLength * sizeof(char);
}

// Write a wide string to blob. Return the total length in bytes 
DWORD OLEBlob::WriteVT_LPWSTR(WCHAR *pwsz)
{
  DWORD wcount; 
  
  wcount = wcslen(pwsz) + 1;        // Get the WCHAR count
  
  // Write the WCHAR count, including terminating NULL  
  if ( !WriteVT_I4(wcount) )
    return false;
  
  // Write the wide string to the blob buffer referenced by bufPtr  
  if ( !Write(pwsz, wcount * sizeof(WCHAR)) )
    return false;
  
  return wcount * sizeof(WCHAR);
}

//  ----------------------------------------------------------------------------
//  Methods of List
//  ----------------------------------------------------------------------------

// Create a list. This constructor should be used only for the list root
List::List()
{
  // Set the initial reference count to one
  userCount = 1;
  
  // Set the initial pointer to null
  name = DuplicateStr("");
  ptr  = NULL;
  next = NULL;
}

// Create a new record in the list
List::List(void* thePtr, const char *theName)
{
  // Set the initial reference count to one
  userCount = 1;
  
  // Set the initial pointer to the passed values
  name = DuplicateStr(theName);
  ptr  = thePtr; 
  next = NULL;
}

// Delete a list
List::~List()
{
  delete[] name; 
}

void List::AddRef()
{
  userCount++; 
}

void List::Release()
{
  userCount--;
  if ( userCount <= 0 ) {
    delete name;
    name = NULL;
    ptr  = NULL;
    userCount = 0;
  } 
}

// Add an ole object to the list
Boolean List::Add(void* thePtr, const char *theName)
{
  // Add the ole object to the list only if it is not in the list yet. 
  if ( !Search(theName) ) {
  
    // Add the ole object to the end of list
    List* pl = GetEndOfList(); 

    pl->next = new List(thePtr, theName);
    
    if ( pl->next == NULL )
      return FALSE; 
  }
  
  return TRUE;
}

// Delete an ole object in the list
Boolean List::Delete(const char *theName)
{
  List* pl = NULL; 
  
  // Delete the ole object to the list only if it is in the list. 
  if ( (pl = Locate(theName)) != NULL ) {
  
    // Store the next list pointer in a temporary place
    List* tmp = pl->next->next; 
    
    // Delete current list pointer
    delete  pl->next;   
    
    // Save the next list pointer
    pl->next = tmp;
    
  } else
    return FALSE;
    
  return TRUE;  
}

// Locate a List object in the list. 
// Return the address of the List object immediately BEFORE the searched object.
// TRICK: this works ONLY because there is ALWAYS a dummy record at the beginning of the list.
List* List::Locate(const char *theName)
{
  if (next == NULL)
    return NULL;
  else if ( !strcmp(next->name, theName) )
    return this;
  else
    return next->Locate (theName);
}

// Search an ole object in the list. Return NULL if none found
void* List::Search(const char *theName)
{
  if ( !strcmp (name, theName) )
    return ptr;
  else if (next)
    return next->Search(theName);
  else
    return NULL;
}

// Find the last element in the list
List* List::GetEndOfList()
{
  if (next)
    return next->GetEndOfList();
  else
    return this;
}


//  ----------------------------------------------------------------------------
//  Global Functions
//  ----------------------------------------------------------------------------

// This function replaces substring with another one in a specified string, return NULL 
// if failed
char* strrep(const char* s1, const char *t1, const char *t2, char *s2)
{
  const char *ptr;
        char *newstr; 
  
  // If the substring don't exist, return NULL
  if ( !(ptr=strstr(s1, t1)))   return NULL; 
  
  newstr = new char[strlen(s1)+strlen(t2)+1];   
  if (newstr==NULL) {
    return NULL;
  }
  strcpy(newstr, s1); 
  newstr[ptr-s1]= '\0';
  strcat(newstr, t2); 
  newstr[ptr-s1 + strlen(t2)]= '\0';
  strcpy(s2, newstr); 
  delete newstr; 

  return s2; 
}

// This function duplicate a string. Notice: the returned string is allocated and should be deleted  
// after use. Return null if fails 
char* DuplicateStr(const char* s)
{
  char *str = new char[strlen(s) + 1]; 
  
  if ( !str )
    return NULL; 
    
  strcpy(str, s); 
  return str; 
}

// This function duplicate a wide char string. Notice: the returned string is allocated and should be deleted  
// after use. Return null if fails 
WCHAR* DuplicateWideStr(const WCHAR* ws)
{
  WCHAR *wstr = new WCHAR[wcslen(ws) + 1]; 
  
  if ( !wstr )
    return NULL; 
    
  memcpy(wstr, ws, sizeof(WCHAR) * (wcslen(ws) + 1)); 
  return wstr; 
}

// This function duplicate a blob. Notice: the returned blob is allocated and should be deleted  
// after use. Return null if fails 
BLOB* DuplicateBLOB(const BLOB* pblob)
{
  BLOB *pb = NULL; 
  
  if ( pblob ) {
    pb = new BLOB; 
    pb->cbSize = pblob->cbSize;
    pb->pBlobData= new BYTE [pb->cbSize];  
    
    if ( !pb->pBlobData )
      return NULL; 
       
    memcpy(pb->pBlobData, pblob->pBlobData, pb->cbSize);
  }
  else 
    return NULL; 
  
  return pb;      
}

// This function delete a blob 
void DeleteBLOB(BLOB* pblob)
{
  if ( pblob ) {
    if ( pblob->pBlobData ) {
      delete pblob->pBlobData; 
      pblob->cbSize = 0;
    }
    delete pblob;   
  }   
}

// This function duplicate a clipdata. Notice: the returned clipdata is allocated and should be deleted  
// after use. Return null if fails 
CLIPDATA* DuplicateCF(const CLIPDATA* pClipData)
{ 
  CLIPDATA *pcf = NULL; 
  
  if ( pClipData ) {
    pcf = new CLIPDATA; 
    pcf->cbSize = pClipData->cbSize;
    pcf->pClipData= new BYTE [pcf->cbSize];  
    
    if ( !pcf->pClipData )
      return NULL; 
       
    memcpy(pcf->pClipData, pClipData->pClipData, pcf->cbSize); 
  }
  else 
    return NULL; 
  
  return pcf;       
}

// This function delete a clipdata 
void DeleteCF(CLIPDATA* pcf)
{
  if ( pcf ) {
    if ( pcf->pClipData ) {
      delete pcf->pClipData; 
      pcf->cbSize = 0;
    }
    delete pcf;
  }       
}

// This function allocate memory for vector. Notice: the returned vector is allocated and should be deleted  
// after use. Return null if fails 
VECTOR* AllocVECTOR(long type, long elemCount)
{ 
  VECTOR *pvec = new VECTOR;  
  if ( !pvec ) 
    return NULL;
    
  pvec->cElements = elemCount; 

  // Erase the VT_VECTOR bit, so remaining is the data type in vector
  if (type & VT_VECTOR)
    type ^= VT_VECTOR;
  
  switch (type)
  {
    // If the subtype is char/unsigned char, use BYTE *prgb
    case VT_I1:
    case VT_UI1: {    
      pvec->prgb = new BYTE[pvec->cElements * sizeof(char)];
      if ( !pvec->prgb )
        return NULL;
      break; 
    }
    
    // If the subtype is short/unsigned short, use WORD *prgw
    case VT_I2:
    case VT_UI2:    
    case VT_BOOL: {   
      pvec->prgw = new WORD [pvec->cElements];
      if ( !pvec->prgw )
        return NULL;
      break; 
    }
    
    // If the subtype is long/unsigned long, use DWORD *prgdw
    case VT_I4:
    case VT_UI4:    
    case VT_ERROR: {    
      pvec->prgdw = new DWORD [pvec->cElements];
      if ( !pvec->prgdw )
        return NULL;
      break; 
    }
    
    // If the subtype is float, use float *prgflt
    case VT_R4: {
      pvec->prgflt = new float [pvec->cElements];
      if ( !pvec->prgflt )
        return NULL;
      break; 
    }
    
    // If the subtype is double, use double *prgdbl
    case VT_R8: {
      pvec->prgdbl = new double [pvec->cElements];
      if ( !pvec->prgdbl )
        return NULL;
      break; 
    }
    
    // If the subtype is LPSTR, use char **prgpsz
    case VT_LPSTR: {
      pvec->prgpsz = new char* [pvec->cElements];   
      if (pvec->prgpsz==NULL) 
        return NULL;
      break; 
    }
    
    // If the subtype is LPSTR/BSTR, use char **prgpwz
    case VT_LPWSTR: 
    case VT_BSTR: {
      pvec->prgpwz = new WCHAR* [pvec->cElements];    
      if (pvec->prgpwz==NULL) 
        return NULL;
      break; 
    } 

    // If the subtype is BLOB, use BLOB **prgblob
    case VT_BLOB: {
      pvec->prgblob = new BLOB* [pvec->cElements];
      if ( !pvec->prgblob )
        return NULL;
      break; 
    } 
        
    // If the subtype is CLIPDATA, use CLIPDATA **pcd
    case VT_CF: {
      pvec->pcd = new CLIPDATA* [pvec->cElements];
      if ( !pvec->pcd )
        return NULL;
      break; 
    } 
        
    // If the subtype is CLSID, use CLSID *pclsid
    case VT_CLSID: {
      pvec->pclsid = new CLSID [pvec->cElements];
      if ( !pvec->pclsid )
        return NULL;
      break; 
    } 
        
    // If the subtype is VARIANT, use VARIANT *pvar
    case VT_VARIANT: {
      pvec->pvar = new VARIANT [pvec->cElements];
      if ( !pvec->pvar )
        return NULL;
      break; 
    } 
        
    default: 
      return NULL;
  }

  return pvec;      
}

// This function duplicate a vector. Notice: the returned vector is allocated and should be deleted  
// after use. Return null if fails 
VECTOR* DuplicateVECTOR(const VECTOR* pvector, long type)
{ 
  DWORD len; 
  
  // If original vector is invalid, return false
  if ( !pvector ) 
    return NULL;
  
  // Allocate the memory for a new vector 
  VECTOR* pvec = AllocVECTOR(type, pvector->cElements);
  if ( !pvec ) 
    return NULL;
     
  // Erase the VT_VECTOR bit, so remaining is the data type in vector
  if (type & VT_VECTOR)
    type ^= VT_VECTOR;
  
  // Duplicate the vector according to its type
  switch (type)
  {
    // If the subtype is char/unsigned char, use BYTE *prgb
    case VT_I1:
    case VT_UI1: {    
      memcpy(pvec->prgb, pvector->prgb, pvec->cElements * sizeof(char));

      // Set the size of vector
      len =  pvec->cElements * sizeof(char) + sizeof(DWORD); 
      break; 
    }

    // If the subtype is short/unsigned short, use WORD *prgw
    case VT_I2:
    case VT_UI2:    
    case VT_BOOL: {   
      memcpy(pvec->prgw, pvector->prgw, pvec->cElements * sizeof(short));

      // Set the size of vector
      len =  pvec->cElements * sizeof(short) + sizeof(DWORD); 
      break; 
    }

    // If the subtype is long/unsigned long, use DWORD *prgdw
    case VT_I4:
    case VT_UI4:    
    case VT_ERROR: {    
      memcpy(pvec->prgdw, pvector->prgdw, pvec->cElements * sizeof(long));

      // Set the size of vector
      len =  pvec->cElements * sizeof(long) + sizeof(DWORD); 
      break; 
    }
    
    // If the subtype is float, use float *prgflt
    case VT_R4: {
      memcpy(pvec->prgflt, pvector->prgflt, pvec->cElements * sizeof(float));

      // Set the size of vector
      len =  pvec->cElements * sizeof(float) + sizeof(DWORD); 
      break; 
    }

    // If the subtype is double, use double *prgdbl
    case VT_R8: {
      memcpy(pvec->prgdbl, pvector->prgdbl, pvec->cElements * sizeof(double));

      // Set the size of vector
      len =  pvec->cElements * sizeof(double) + sizeof(DWORD); 
      break; 
    }
    
    // If the subtype is LPSTR/BSTR, use char **prgpsz
    case VT_LPSTR: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) {

        // Duplicate each string in the array
        if ( !(pvec->prgpsz[i] = DuplicateStr(pvector->prgpsz[i])) ) {
          pvec->cElements = i - 1; 
          break; 
        }
      }
      break; 
    }

    // If the subtype is LPWSTR, use char **prgpwz
    case VT_LPWSTR: 
    case VT_BSTR: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) {

        // Duplicate each wide string in the array
        if ( !(pvec->prgpwz[i] = DuplicateWideStr(pvector->prgpwz[i])) ) {
          pvec->cElements = i - 1; 
          break; 
        }
      }
      break; 
    }
    
    // If the subtype is BLOB, use BLOB **prgblob
    case VT_BLOB: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) {

        // Duplicate each BLOB in the array
        if ( !(pvec->prgblob[i] = DuplicateBLOB(pvector->prgblob[i])) ) {
          pvec->cElements = i - 1; 
          break; 
        }
      }
      break; 
    }

    // If the subtype is CLIPDATA, use CLIPDATA **pcd
    case VT_CF: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) {

        // Duplicate each CLIPDATA in the array
        if ( !(pvec->pcd[i] = DuplicateCF(pvector->pcd[i])) ) {
          pvec->cElements = i - 1; 
          break; 
        }
      }
      break; 
    }

    // If the subtype is CLSID, use CLSID *pclsid
    case VT_CLSID: {
      memcpy(pvec->pclsid, pvector->pclsid, sizeof(CLSID) * pvec->cElements);
      break; 
    }
    
    // If the subtype is VARIANT, use VARIANT *pvar
    case VT_VARIANT: {
      memcpy(pvec->pvar, pvector->pvar, pvec->cElements * sizeof(VARIANT));

      // Copy the variable length elements in the vector
      for ( DWORD i = 0; i < pvec->cElements; i++ ) {
      
        // If variant type is vector, recursively duplicate vector 
        if ( pvec->pvar[i].vt & VT_VECTOR ) {
        
          VECTOR *pv = (VECTOR *)V_BYREF(&pvector->pvar[i]); 
          len = VTtoVariant(&pvec->pvar[i], pv); 
          continue; 
        }
        
        // Duplicate according to type in variant 
        switch(pvec->pvar[i].vt)
        {
          case VT_LPSTR:
            len = VTtoVariant(&pvec->pvar[i], (char*)V_UI1REF(&pvector->pvar[i])); 
            break;

          case VT_LPWSTR:
            len = VTtoVariant(&pvec->pvar[i], (LPWSTR)V_BYREF(&pvector->pvar[i])); 
            break;

          case VT_BLOB:
            len = VTtoVariant(&pvec->pvar[i], (BLOB *)V_BYREF(&pvector->pvar[i])); 
            break;
            
          case VT_CF:
            len = VTtoVariant(&pvec->pvar[i], (CLIPDATA *)V_BYREF(&pvector->pvar[i])); 
            break;
            
          case VT_CLSID:
            VTtoVariant(&pvec->pvar[i], (CLSID *)V_BYREF(&pvector->pvar[i])); 
            break;
            
        }
        
        if ( !len ) {
          pvec->cElements = i - 1; 
          break; 
        }
      }
      break; 
    }
    
    default: 
      return NULL;
  }

  return pvec;      
}

// This function delete a vector 
void  DeleteVECTOR(VECTOR* pvec, long type)
{
  // Erase the VT_VECTOR bit, so remaining is the data type in vector
  if (type & VT_VECTOR)
    type ^= VT_VECTOR;
  
  if ( !pvec ) 
    return; 
    
  switch(type)
  {
    case VT_I1:
    case VT_UI1: {
      if (pvec->cElements)  delete [] pvec->prgb; 
      break;
    }
    case VT_I2:
    case VT_UI2:
    case VT_BOOL: {
      if (pvec->cElements)  delete [] pvec->prgw; 
      break; 
    }
    case VT_I4:
    case VT_UI4:
    case VT_ERROR: {
      if (pvec->cElements)  delete [] pvec->prgdw; 
      break; 
    }
    case VT_R4: {
      if (pvec->cElements)  delete [] pvec->prgflt; 
      break; 
    }
    case VT_R8: {
      if (pvec->cElements)  delete [] pvec->prgdbl; 
      break; 
    }
    case VT_LPSTR:
    {
      for ( DWORD i = 0; i < pvec->cElements; i++ )
        delete [] pvec->prgpsz[i]; 
      if (pvec->cElements)  delete [] pvec->prgpsz; 
      break; 
    }
    case VT_LPWSTR: 
    case VT_BSTR: 
    {
      for ( DWORD i = 0; i < pvec->cElements; i++ )
        delete [] pvec->prgpwz[i]; 
      if (pvec->cElements)  
        delete [] pvec->prgpwz; 
      break; 
    }
    case VT_BLOB: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) 
        DeleteBLOB(pvec->prgblob[i]);             
      if (pvec->cElements)  
        delete pvec->prgblob; 
      break; 
    }
    case VT_CF: {
      for ( DWORD i = 0; i < pvec->cElements; i++ ) 
        DeleteCF(pvec->pcd[i]); 
      if (pvec->cElements)  
        delete pvec->pcd; 
      break; 
    }
    case VT_CLSID: {
      if (pvec->cElements)  
        delete [] pvec->pclsid; 
      break; 
    }
    case VT_VARIANT: {
      if (pvec->cElements)  
        delete [] pvec->pvar; 
      break; 
    }
  }
  
  if ( pvec ) 
    delete pvec; 
}


// FIX_DICT_PROP - Added allocator and deallocator for dictionary property content.

// This function allocate memory for dictionary. An array of entries is allocated.
//  each entry is initialized to null. The caller must allocate the strings for the
//  entries and fill-in the entries.
// Returns pointer to allocated & initialized dictionary (or null)
DICTIONARY* AllocDICTIONARY(long elemCount)
{ 
  // Allocate/construct the property
  DICTIONARY *pDict = new DICTIONARY; 
  if ( !pDict ) 
    return NULL;
  
  // Allocate the array of entries and then initialize it.
  pDict->rgEntry = new ENTRY [elemCount];
  if ( !pDict->rgEntry ) {
    delete pDict; 
    return NULL;
  }
  pDict->cbEntries = elemCount; 
  
  // Initialize the array of entries.
  for(unsigned long i = 0; i < pDict->cbEntries; i++) {
    pDict->rgEntry[i].dwPropID  = 0;
    pDict->rgEntry[i].cb    = 0;
    pDict->rgEntry[i].sz    = 0;
  }
  
  return pDict;
}


// This function deletes a dictionary property -- including all of it's contents 
void  DeleteDICTIONARY(DICTIONARY* pDict)
{
  if ( !pDict ) 
    return;
  
  if (pDict->rgEntry) {
    // Deallocate all of the strings contained in the array of entries 
    for(unsigned long i = 0; i < pDict->cbEntries; i++)
      if( pDict->rgEntry[i].sz )
        delete [] pDict->rgEntry[i].sz;
  
    // Deallocate the array of entries
    delete pDict->rgEntry;
  }
  
  delete pDict;   // Delete the dictonary
}


#ifndef USE_UNICODE
// WideCharToMultiByte()
// Generic routine for converting UniCode string to ASCII string. Return the ASCII string, 
// NULL if failed. The ASCII string is allocated, so should be deleted after use
char* WideCharToMultiByte(const WCHAR * pwchar)
{
  // Determine the length of wide char string
  DWORD wcharlen = wcslen(pwchar); 
  
  // Allocate memory including terminating NULL
  char *pchar = new char[wcharlen + 1];
  
  // Return if fails
  if ( !pchar ) return NULL; 
  
  for (DWORD i = 0; i < wcharlen; i++ ) 
  { 
    // Only low byte of Unicode is used 
    pchar[i] = pwchar[i] & 0x00FF; 
  }
  
  // Add a '\0' to the end of string
  pchar[wcharlen] = '\0'; 
  
  return pchar; 
}

// MultiByteToWideChar()
// Generic routine for converting ASCII string to Unicode string. Returned the wide char
// string, NULL if fails. The wide char string is allocated, and so should be deleted after use
LPWSTR MultiByteToWideChar(const char * pchar)
{
  // Determine the length of string
  DWORD charlen = strlen(pchar); 
  
  // Allocate memory including terminating NULLs
  LPWSTR pwchar = new WCHAR[charlen+1]; 
  
  // Return if fails
  if ( !pwchar )  return NULL; 
  
  for (DWORD i = 0; i < charlen; i++ ) 
  {   
    // Unicode's high byte is set to 0, and low byte same as the string byte
    pwchar[i] = pchar[i] | 0x00 << 8;   
  }
  
  // Last wide character is set to zero
  pwchar[charlen] = 0; 
  
  return pwchar; 
}

// This function calculates the length of Unicode string which is terminated by two consecutive NULLs
// DWORD wcslen(const WCHAR * pwchar) 
// {
//  DWORD i= 0; 
  
//  while (pwchar[i] != 0)
//    i++;    
//  return i; 
// }

#endif

// This function returns the name string of a variant type, return NULL if no match
// given type
char* VariantName( DWORD vtType )
{
  if ( vtType & VT_VECTOR ) return (char *)variantName[73]; 
  
  if ( vtType & VT_ARRAY )  return (char *)variantName[74]; 
  
  return (char *)variantName[vtType]; 
}

// SwapBytes()
// Generic routine for switching endian order in an arbitrary data type. 
void SwapBytes(unsigned char *pbData, short cBytes)
{
  unsigned char bSwap;
  
  if (cBytes < 2)
    return;

  for (short ib = cBytes - 1; ib >= 1; ib -= 2)
  {
    bSwap = pbData[cBytes - 1 - ib];
    pbData[cBytes - 1 - ib] = pbData[ib];
    pbData[ib] = bSwap;
  }
}

HRESULT OLEInit()
{
  HRESULT res = S_OK;
  
#ifdef macintosh  
#ifndef powerc        // If Code Warrior 68k
  
  res = InitOleManager(OLEMGR_BIND_NORMAL);
  if(res != S_OK)
    res = InitOleManager(OLEMGR_BIND_NORMAL);
  if (res == S_OK)
#endif // powerc
#endif // macintosh
#ifdef _WINDOWS
  res = OleInitialize(NULL);
#endif
// on UNIX, for now, there is no need for OleInitialize
  
  return res;
}
//This function uninitializes the OLE library
Boolean OLEUninit()
{
#ifdef macintosh  
  #ifndef powerc  
    UninitOleManager();
  #endif // powerc
#endif // macintosh
#ifdef _WINDOWS
        OleUninitialize();
#endif

  
  return true;
}



