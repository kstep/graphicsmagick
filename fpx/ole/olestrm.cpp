//------------------------------------------------------------------------------------------------
//  MODULE    : OLEStreams.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file contains  OLEStream object 
//
//  COMMENT   : An OLEStream class contains an IStream in OLE library. Besides supporting 
//          I/O functions in IStream, OLEStream class also provides a family of I/O 
//          functions in forms of ReadVT_xx and WriteVT_xx to read/write VT_xx types data
//          defined in variant.h. In ReadVT_xx and WriteVT_xx functions, swapping bytes
//          is performed when the file encounted is created on different platforms ( 
//          Intel or Motorola).
//
//          I/O functions:
//            Read()/Write():       read/write any bytes of data, no swap bytes
//            ReadVT_xx()/WriteVT_xx(): read/write VT type of data, swap bytes, including,
//                        short, long, float, double, BOOL, CY, LPSTR,
//                        LPWSTR, FILETIME, BLOB, CLIPDATA, CLSID, VECTOR, 
//                        VARIANT
//
//      SCCSID            : @(#)olestrm.cpp 1.7 10:45:28 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

//  Includes
//  --------
  #include  <stdio.h>
  #include  <iostream.h>

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEStreams_h
  #include  "olestrm.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
  
//  Constants
//  ---------

// This is the largest string authorized in this version. The purpose is to avoid 
// allocation of humonguous arrays due to corrupted OLE files.
#define LARGEST_STRING 1024

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
#ifdef _WINDOWS
  #pragma code_seg ("OLEStream")
#else
  #pragma segment OLEStream
#endif
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLEStream
//  ----------------------------------------------------------------------------


// Create an OLEStream object from parent storage. 
OLEStream::OLEStream(OLEStorage * parentStorage, IStream * currentStream)
{
  // Set the bytes swap flag
#ifdef IN_BIG_ENDIAN
  fSwapBytes = TRUE;
#else
  fSwapBytes = FALSE;
#endif 

  parStorage = parentStorage;
  
  oleStream  = currentStream;
  if ( oleStream )
    oleStream->AddRef();
}

OLEStream::~OLEStream()
{
  Release();

}

// Add reference to the IStream object contained in the OLEStream
void OLEStream::AddRef()
{
  OLECore::AddRef();
  
  if (oleStream) {
    oleStream->AddRef();
  }
}

// Release the IStream object contained in the OLEStream
void OLEStream::Release()
{
  if (oleStream) {
    oleStream->Release();
  }
  OLECore::Release();
}

// OLEStream::Read(), generic OLE stream read function
// Read the data of bufLen bytes to dataBuf from the stream, bytes are not swapped after
// read. Return NULL if fails
Boolean OLEStream::Read(void * dataBuf, unsigned long bufLen)
{
  HRESULT err;
  if (oleStream) {
    if (FAILED(err = oleStream->Read(dataBuf, bufLen, NULL)))
    {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);     // CHG_FILE_ERR - improve reporting
      return FALSE;
    } 
  } else  
    return FALSE;
  
  return TRUE;
}

// OLEStream::Write(), generic OLE stream write function
// Write the data of bufLen bytes in dataBuf to the stream, bytes are not swapped before
// write. Return NULL if fails
Boolean OLEStream::Write(const void * dataBuf, unsigned long bufLen)
{
  HRESULT err;
  if (oleStream) {
    if (FAILED(err = oleStream->Write(dataBuf, bufLen, NULL)))
    {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);     // CHG_FILE_ERR - improve reporting
      return FALSE;
    } 
  } else  
    return FALSE;
  
  return TRUE;
}

// Set the stream to a new position. There are 3 modes,
// STREAM_SEEK_BEG: set the stream pointer to the offset from beginning of stream 
// STREAM_SEEK_CUR: set the stream pointer to the offset from current of stream 
// STREAM_SEEK_END: set the stream pointer to the offset from end of stream 
Boolean OLEStream::Seek( const long offset, const unsigned long mode )
{
  LARGE_INTEGER oleOffset; 
  HRESULT err;
  
  // Set the offset to a ULARGE_INTEGER
  LISet32(oleOffset, offset); 
  
  if (oleStream) {
    if (FAILED(err = oleStream->Seek(oleOffset, mode, NULL)))       
    {
      lastError = TranslateOLEError(err);
      fpxStatus = OLEtoFPXError(err);     // CHG_FILE_ERR - improve reporting
      return FALSE;
    } 
  } else  
    return FALSE;
  
  return TRUE;
}

// Get the position of end of the stream
Boolean OLEStream::GetEndOfFile( long* endPosition)
{
  LARGE_INTEGER oleOffset; 
  ULARGE_INTEGER FAR position; 
  HRESULT err;

  // Initialize the positions
  LISet32(oleOffset, 0); 
  *endPosition = 0;
  
  if (oleStream) {
    if (FAILED(err = oleStream->Seek(oleOffset, STREAM_SEEK_END, &position)))       
    {
      lastError = TranslateOLEError(err);
      return FALSE;
    }
    // Save the position
    *endPosition = position.LowPart;
  } else  
    return FALSE;
  
  return TRUE;  
}

// Set the size of the stream
Boolean OLEStream::SetSize( unsigned long sizeOfStream )
{
  ULARGE_INTEGER oleSize; 
  HRESULT err;
  
  // Set the length of stream to a ULARGE_INTEGER
  ULISet32(oleSize, sizeOfStream); 

  if (oleStream) {
    if (FAILED(err = oleStream->SetSize(oleSize)))
    {
      lastError = TranslateOLEError(err);
      return FALSE;
    } 
  } else  
    return FALSE;
  
  return TRUE;
}

// Copy the stream to a new one
Boolean OLEStream::CopyTo( LPSTREAM pstm, unsigned long copyLen )
{
  ULARGE_INTEGER oleLen; 
  HRESULT err;
  
  // Set the length of stream to a ULARGE_INTEGER
  ULISet32(oleLen, copyLen); 
  
  if (oleStream) {
    if (FAILED(err = oleStream->CopyTo(pstm, oleLen, NULL, NULL)))
    {
      lastError = TranslateOLEError(err);
      return FALSE;
    } 
  } else  
    return FALSE;
  
  return TRUE;
}

// Save the changes made to the stream
Boolean OLEStream::Commit( )
{
  HRESULT err;
  if (oleStream) {
    if (FAILED(err = oleStream->Commit(0))) 
    {
      lastError = TranslateOLEError(err);
      return FALSE;
    } 
      
    return TRUE;
  } else
    return FALSE;
}

// Discard the changes made to the stream
Boolean OLEStream::Revert( )
{
  HRESULT err;
  if (oleStream) {
    if (FAILED(err = oleStream->Revert()) ) 
    {
      lastError = TranslateOLEError(err);
      return FALSE;
    } 
    return TRUE;
  } else
    return FALSE;
}


// NOTE: All read functions return the number of bytes of data stored
// IN MEMORY, and all write functions return the number of bytes of data
// written to THE STREAM. They are often different. This is a convention to
// make stream reading/writing very easy.

//----------------------- ReadVT_xx functions -----------------------------/

// OLEStream::ReadVT()
// This is a generic function that takes a property of the given type out of
// the given stream (at its current position) and puts it at the location
// pointed to by pData. 
DWORD OLEStream::ReadVT(DWORD dwType, BYTE **pData)
{
  // If type is VT_VECTOR, read it
  if (dwType & VT_VECTOR)
  {
    return ReadVT_VECTOR(dwType, (VECTOR **)pData); 
  }

  // Read the data into pData according to the VT type
  switch(dwType)
  {
    case VT_I1:
    case VT_UI1:
      return ReadVT_I2((WORD *)*pData);
    case VT_I2:
    case VT_UI2:
      return ReadVT_I2((WORD *)*pData);
    case VT_I4:
    case VT_UI4:
    case VT_ERROR:
      return ReadVT_I4((DWORD *)*pData);
    case VT_R4:
      return ReadVT_R4((float *)*pData);
    case VT_R8:
      return ReadVT_R8((double *)*pData);
    case VT_CY:
      return ReadVT_CY((CY *)*pData);
    case VT_DATE:
      return ReadVT_DATE((double *)*pData);
    case VT_BOOL:
      return ReadVT_BOOL((BOOL *)*pData);
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
      return ReadVT_I8((LARGE_INTEGER *)*pData);
    case VT_LPSTR:
      return ReadVT_LPSTR((char **)pData);
    case VT_LPWSTR:
      return ReadVT_LPWSTR((WCHAR **)pData);
    case VT_BSTR:
      return ReadVT_LPWSTR((WCHAR **)pData);
    case VT_FILETIME:
      return ReadVT_FILETIME((FILETIME *)*pData);
    case VT_BLOB:
      return ReadVT_BLOB((BLOB **)pData);
    case VT_STREAM:
      return ReadVT_STREAM((char **)pData);
    case VT_STORAGE:
      return ReadVT_STORAGE((char **)pData);
    case VT_STREAMED_OBJECT:
      return ReadVT_STREAMED_OBJECT((char **)pData);
    case VT_STORED_OBJECT:
      return ReadVT_STORED_OBJECT((char **)pData);
    case VT_BLOB_OBJECT:
      return ReadVT_BLOB_OBJECT((char **)pData);
    case VT_CF:
      return ReadVT_CF((CLIPDATA **)pData);
    case VT_CLSID:
      return ReadVT_CLSID((CLSID **)pData);
    default:
      return FALSE;
  }
}

// OLEStream::ReadVT()
// This is a generic function that takes a property of the given type out of
// the given stream (at its current position) and puts it at the location
// pointed to by variant. 
DWORD OLEStream::ReadVT(VARIANT * variant)
{
  // If type is VT_VECTOR, read it
  if (variant->vt & VT_VECTOR)
  {
    return ReadVT_VECTOR(variant->vt, (VECTOR **)&V_BYREF(variant)); 
  }

  // Read the data into variant according to the VT type
  switch(variant->vt)
  {
    case VT_I1:
    case VT_UI1:
      return ReadVT_I2((WORD *)&V_I2(variant));
    case VT_I2:
    case VT_UI2:
      return ReadVT_I2((WORD *)&V_I2(variant));
    case VT_I4:
    case VT_UI4:
    case VT_ERROR:
      return ReadVT_I4((DWORD *)&V_I4(variant));
    case VT_R4:
      return ReadVT_R4((float *)&V_R4(variant));
    case VT_R8:
      return ReadVT_R8((double *)&V_R8(variant));
    case VT_CY:
      return ReadVT_CY((CY *)&V_CY(variant));
    case VT_DATE:
      return ReadVT_DATE((double *)&V_DATE(variant));
    case VT_BOOL:
      return ReadVT_BOOL((BOOL *)&V_BOOL(variant));
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
      return ReadVT_I8((LARGE_INTEGER *)&V_CY(variant));
    case VT_LPSTR:
#ifdef _UNICODE
      return ReadVT_LPSTR((LPSTR *)&V_UI1REF(variant));
#else
      return ReadVT_LPSTR((char **)&V_BSTR(variant));
#endif
    case VT_LPWSTR:
      return ReadVT_LPWSTR((WCHAR **)&V_BYREF(variant));
    case VT_BSTR:
      return ReadVT_LPWSTR((WCHAR **)&V_BYREF(variant));
    case VT_FILETIME:
      return ReadVT_FILETIME((FILETIME *)&V_CY(variant));
    case VT_BLOB:
      return ReadVT_BLOB((BLOB **)&V_BYREF(variant));
    case VT_STREAM:
      return ReadVT_STREAM((char **)&V_BYREF(variant));
    case VT_STORAGE:
      return ReadVT_STORAGE((char **)&V_BYREF(variant));
    case VT_STREAMED_OBJECT:
      return ReadVT_STREAMED_OBJECT((char **)&V_BYREF(variant));
    case VT_STORED_OBJECT:
      return ReadVT_STORED_OBJECT((char **)&V_BYREF(variant));
    case VT_BLOB_OBJECT:
      return ReadVT_BLOB_OBJECT((char **)&V_BYREF(variant));
    case VT_CF:
      return ReadVT_CF((CLIPDATA **)&V_BYREF(variant));
    case VT_CLSID:
      return ReadVT_CLSID((CLSID **)&V_BYREF(variant));
    default:
      return FALSE;
  }
}

// ReadVT_I2()
// Reads a 2-byte WORD value from a stream. The bytes is swapped
// Caution: Since value in the stream is zero-padded to a 32-bit boundary, actual read
// data is 4 bytes despite it is a short type.
DWORD OLEStream::ReadVT_I2(WORD *pw)
{
  WORD  wPad;   // Padded short

  // Read a short type and a padded short
  if ( !Read(pw, sizeof (WORD)) || !Read(&wPad, sizeof (WORD)))
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)pw, sizeof(short));   
#endif

  // Return the total bytes read, which is size of short ( 2 bytes )
  return sizeof (short);
}

// ReadVT_I2_NoPad()
// Reads a 2-byte WORD value without padding to 32-bit from a stream. The bytes is swapped
DWORD OLEStream::ReadVT_I2_NoPad(WORD *pw)
{
  // Read a short type and a padded short
  if ( !Read(pw, sizeof (WORD)) )
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)pw, sizeof(short));   
#endif

  // Return the total bytes read, which is size of short ( 2 bytes )
  return sizeof (short);
}

// ReadVT_I4()
// Reads a 4-byte DWORD value from a stream. The bytes is swapped
DWORD OLEStream::ReadVT_I4(DWORD *pdw)
{
  if ( !Read(pdw, sizeof (DWORD)))
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)pdw, sizeof(long));   
#endif

  // Return the total bytes read, which is size of long ( 4 bytes )
  return sizeof (DWORD);
}

// ReadVT_R4()
// Reads a 32-bit IEEE floating point value from a stream. The bytes is swapped
DWORD OLEStream::ReadVT_R4(float *pflt)
{
  if ( !Read(pflt, sizeof (float)))
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)pflt, sizeof(float));   
#endif

  // Return the total bytes read, which is size of float ( 4 bytes )
  return sizeof (float);
}

// ReadVT_R8()
// Reads a 64-bit IEEE floating point value from a stream. The bytes is swapped
DWORD OLEStream::ReadVT_R8(double *pdbl)
{
  if ( !Read(pdbl, sizeof(double)))
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)pdbl, sizeof(double));  
#endif

  // Return the total bytes read, which is size of double ( 8 bytes )
  return sizeof(double);
}

// ReadVT_CY()
// Reads an 8-byte two's complement integer (scaled by 10,000) from a stream. 
//  It's usually used for currency amounts (hence the CY type).The bytes is swapped
DWORD OLEStream::ReadVT_CY(CY *pcy)
{
#ifdef macintosh
  if ( !ReadVT_I4((DWORD *)&pcy->Hi) || !ReadVT_I4(&pcy->Lo) )
#else
  if ( !ReadVT_I4(&pcy->Lo) || !ReadVT_I4((DWORD *)&pcy->Hi) )
#endif
    return FALSE;

  // Return the total bytes read, which is size of two DWORDs ( 8 bytes )
  return sizeof (CY);
}

// ReadVT_I8()
// Reads an 8-byte integer from a stream. The bytes is swapped
DWORD OLEStream::ReadVT_I8(LARGE_INTEGER *pli)
{
  if ( !ReadVT_I4(&pli->LowPart) || !ReadVT_I4((DWORD *)&pli->HighPart) )
    return FALSE;

  // Return the total bytes read, which is size of two DWORDs ( 8 bytes )
  return sizeof (LARGE_INTEGER);
}

// ReadVT_BOOL()
// Reads a boolean from a stream. The bytes is swapped 
// An OLE boolean is defined to be a 2-byte WORD containing 0 (FALSE) or 
// -1 (TRUE). It is padded to a 32-bit boundary. A local boolean is not 
// guaranteed to be anything, so we read in a dword and cast it as a TRUE 
// or FALSE BOOL according to its value.
DWORD OLEStream::ReadVT_BOOL(BOOL *pf)
{
  WORD w;

  // Read the data
  if (!ReadVT_I2(&w))
    return FALSE;

  // Set the boolean value
  *pf = (w==0)? FALSE: TRUE;

  // Return the total bytes read, which is size of BOOL ( 2 bytes )
  return sizeof (BOOL);
}

// ReadVT_LPSTR()
// Reads a character string from a stream. Return total bytes read . The array is 
// dynamically allocated, and should be deleted after use.
DWORD OLEStream::ReadVT_LPSTR(char **ppsz)
{
  DWORD dwLength, rem;
  char *psz;

  // Read the length count including terminating NULL
  if (!ReadVT_I4(&dwLength))
    return FALSE;

  // If it is null character, return the size of string
  if ( !dwLength )
    return sizeof(DWORD);

  // Limit the size of a string to a reasonnable value
  if (dwLength > LARGEST_STRING)
    dwLength = LARGEST_STRING;

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (dwLength % 4) ? 4 : 0;
  rem -= dwLength % 4; 
  
  // Allocate memory 
  psz = new char[dwLength];
  *ppsz = psz;
  
  if (!psz) 
    return FALSE;

  // Read the string
  if ( !Read(psz, dwLength * sizeof (char)) )
    return FALSE;

  // Set the stream pointer to next 32-bit boundary in order to read next correctly
  Seek(rem, STREAM_SEEK_CUR); 
    
  // Return the total bytes read from stream, including bytes count and actual data length
  return sizeof(DWORD) + dwLength;
}

// ReadVT_LPSTR_NoPad()
// Reads a character string without padding to 32-bit from a stream. Return total bytes read. The array is 
// dynamically allocated, and should be deleted after use.
DWORD OLEStream::ReadVT_LPSTR_NoPad(char **ppsz)
{
  DWORD dwLength;
  char *psz;

  // Read the length count including terminating NULL
  if (!ReadVT_I4(&dwLength))
    return FALSE;

  // If it is null character, return the size of string
  if ( !dwLength )
    return sizeof(DWORD);

  // Limit the size of a string to a reasonnable value
  if (dwLength > LARGEST_STRING)
    dwLength = LARGEST_STRING;

  // Allocate memory 
  psz = new char[dwLength];
  *ppsz = psz;
  
  if (!psz) 
    return FALSE;

  // Read the string
  if ( !Read(psz, dwLength * sizeof (char)) )
    return FALSE;

  // Return the total bytes read from stream, including bytes count and actual data length
  return sizeof(DWORD) + dwLength;
}

// ReadVT_LPWSTR()
// Reads a Unicode (16-bit) character string from a stream. Return total bytes read. 
// The array is dynamically allocated, and should be deleted after use.
DWORD OLEStream::ReadVT_LPWSTR(WCHAR **ppwsz)
{
  DWORD dwLength, rem;
  WCHAR *pwsz = NULL;

  // Read the WCHAR count including terminating NULL
  if (!ReadVT_I4(&dwLength))
    return FALSE;

  // If it is null character, return the size of string
  if ( !dwLength )
    return sizeof(DWORD);

  // Limit the size of a string to a reasonnable value
  if (dwLength > LARGEST_STRING)
    dwLength = LARGEST_STRING;
  
  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = ((dwLength * 2) % 4) ? 4 : 0;
  rem -= (dwLength * 2) % 4; 
  
  // Allocate memory
  pwsz = new WCHAR[dwLength];
  *ppwsz = pwsz;
  
  if (!pwsz) 
    return FALSE;
  
  // Read the wide char. Here we don't use ReadVT_I2 since unicode is not padded to 32-bit
  for ( DWORD i = 0; i < dwLength; i++ )  {
    if ( !ReadVT_I2_NoPad(&pwsz[i]) )
      return FALSE;
  }
  
  // Set the stream pointer to next 32-bit boundary in order to read next correctly
  Seek(rem, STREAM_SEEK_CUR); 
        
  // Return the total bytes read from stream, including WCHAR count and actual data
  return sizeof (DWORD) + (dwLength) * sizeof (WCHAR);
}

// ReadVT_LPWSTR_NoPad()
// Reads a Unicode (16-bit) character string without padding to 32-bit from a stream. Return total bytes read. 
// The array is dynamically allocated, and should be deleted after use.
DWORD OLEStream::ReadVT_LPWSTR_NoPad(WCHAR **ppwsz)
{
  DWORD dwLength;
  WCHAR *pwsz = NULL;

  // Read the WCHAR count including terminating NULL
  if (!ReadVT_I4(&dwLength))
    return FALSE;

  // If it is null character, return the size of string
  if ( !dwLength )
    return sizeof(DWORD);

  // Limit the size of a string to a reasonnable value
  if (dwLength > LARGEST_STRING)
    dwLength = LARGEST_STRING;
  
  // Allocate memory
  pwsz = new WCHAR[dwLength];
  *ppwsz = pwsz;
  
  if (!pwsz) 
    return FALSE;
  
  // Read the wide char. Here we don't use ReadVT_I2 since unicode is not padded to 32-bit
  for ( DWORD i = 0; i < dwLength; i++ )  {
    if ( !ReadVT_I2_NoPad(&pwsz[i]) )
      return FALSE;
  }
  
  // Return the total bytes read from stream, including WCHAR count and actual data
  return sizeof (DWORD) + (dwLength) * sizeof (WCHAR);
}

// ReadVT_FILETIME()
// Reads a FILETIME structure from a stream. The bytes is swapped 
DWORD OLEStream::ReadVT_FILETIME(FILETIME *pft)
{
  // Read the filetime
  if (!ReadVT_I4(&pft->dwLowDateTime) || !ReadVT_I4(&pft->dwHighDateTime))
    return FALSE;

  // Return the total bytes read, which is size of FILETIME ( 16 bytes )
  return sizeof (FILETIME);
}

// ReadVT_BLOB()
// Reads a BLOB (length + raw block of data) from a stream. The BYTE array
// within the BLOB structure is dynamically allocated, so it should be deleted 
// by the caller after being used.
DWORD OLEStream::ReadVT_BLOB(BLOB **ppBlob)
{
  DWORD   rem;
  BLOB *  pblob;

  pblob = new BLOB;
  if ( pblob==NULL ) {
    return FALSE;
  }
    
  // Read the size of the blob
  if (!ReadVT_I4(&pblob->cbSize))
    return FALSE;

  if (pblob->cbSize == 0)
    return sizeof (DWORD);

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (pblob->cbSize % 4) ? 4 : 0;
  rem -= pblob->cbSize % 4; 
  
  // Allocate the memory
  pblob->pBlobData = new BYTE[pblob->cbSize];
  if (!pblob->pBlobData)
    return FALSE;

  // Read blob according to its size
  if ( !Read(pblob->pBlobData, pblob->cbSize))
    return FALSE;

  *ppBlob = pblob; 
  
  // Set the stream pointer to next 32-bit boundary in order to read next correctly
  Seek(rem, STREAM_SEEK_CUR); 

  // Return the total bytes read, which is blob count (4 bytes) plus 
  // the total length of BLOB
  return pblob->cbSize + sizeof(DWORD);
}

// ReadVT_CLSID() with input of CLSID *
// Reads a CLSID from a stream (which is really a DWORD, two WORDs, and 8 bytes).
// The bytes is swapped
DWORD OLEStream::ReadVT_CLSID(CLSID *pclsid)
{
  // Read the first three integers in CLSID
  // Here we use Read() for short type, not ReadVT_I2(), so bytes are not swapped
  if (!ReadVT_I4(&pclsid->Data1) || !Read((WORD *)&pclsid->Data2, sizeof(short)) ||
    !Read((WORD *)&pclsid->Data3, sizeof(short)) )
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes ) {
    SwapBytes((BYTE *)&pclsid->Data2, sizeof(short));   
    SwapBytes((BYTE *)&pclsid->Data3, sizeof(short));   
  }
#endif

  // Read the last 8 bytes in CLSID
  if ( !Read(pclsid->Data4, 8*sizeof(char)))
    return FALSE;

  // Return the total bytes read, which is size of CLSID ( 16 bytes )
  return sizeof (CLSID);
}

// ReadVT_CLSID() with input of CLSID **
// Just allocate the handle if none and call the method using a CLSID *
DWORD OLEStream::ReadVT_CLSID(CLSID **ppClsid)
{
  if (!*ppClsid) {
    *ppClsid = new CLSID;   
    if (*ppClsid==NULL) {
      return FALSE;
    }
  }
  return ReadVT_CLSID (*ppClsid);
}

// ReadVT_VARIANT()
// Reads a VARIANT from a stream (which is really four unsigned short, and a double).
// The bytes is swapped
DWORD OLEStream::ReadVT_VARIANT(VARIANT *pvar)
{
  // Read a variant structure
  // Here we use Read() for short type, not ReadVT_I2(), so bytes are not swapped
  if ( !Read((WORD *)&pvar->vt,       sizeof(short)) || 
     !Read((WORD *)&pvar->wReserved1, sizeof(short)) ||
     !Read((WORD *)&pvar->wReserved2, sizeof(short)) ||
     !Read((WORD *)&pvar->wReserved3, sizeof(short)) ||
     !Read((double *)&pvar->dblVal,   sizeof(double)) )
    return FALSE;

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes ) {
    SwapBytes((BYTE *)&pvar->vt,     sizeof(short));  
    SwapBytes((BYTE *)&pvar->wReserved1, sizeof(short));  
    SwapBytes((BYTE *)&pvar->wReserved2, sizeof(short));  
    SwapBytes((BYTE *)&pvar->wReserved3, sizeof(short));  
    SwapBytes((BYTE *)&pvar->dblVal,   sizeof(double));   
  }
#endif

  // Return the total bytes read, which is size of VARIANT ( 16 bytes )
  return sizeof (VARIANT);
}

// ReadVT_CF()
// Reads a clipboard data structure (length + format + raw block of data)
// from a stream. The BYTE array within the CLIPDATA structure is dynamically
// allocated, so it should be deleted by the caller after being used.
DWORD OLEStream::ReadVT_CF(CLIPDATA **ppClipData)
{
  DWORD     rem; 
  CLIPDATA *  pclipdata; 
  
  pclipdata = new CLIPDATA;
  if (pclipdata==NULL) 
    return FALSE;
    
  // Read the size and clip format
  if ( !ReadVT_I4(&pclipdata->cbSize) )
    return FALSE;

  if (pclipdata->cbSize == 0)
    return sizeof (DWORD);

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (pclipdata->cbSize % 4) ? 4 : 0;
  rem -= pclipdata->cbSize % 4; 

  // Allocate the memory
  pclipdata->pClipData = new BYTE[pclipdata->cbSize];
  if (!pclipdata->pClipData)
    return FALSE;

  // Read clipdata according to its size
  if ( !Read(pclipdata->pClipData, pclipdata->cbSize))
    return FALSE;

  *ppClipData = pclipdata; 

  // Set the stream pointer to next 32-bit boundary in order to read correctly
  Seek(rem, STREAM_SEEK_CUR); 

  // Return the total bytes read, which include clipdata count, clipdata format, and 
  // the total length of CLIPDATA
  return sizeof(DWORD) + pclipdata->cbSize;
}


// ReadVT_VECTOR()
// Reads a vector data structure (cElements * length of each element), and return total bytes 
// been read. The vector type is like VT_XX | VT_VECTOR. Right now 11 types are supported 
// ( char, WORD, DWORD, float, double, LPSTR, LPWSTR, BLOB, CLIPDATA, CLSID, VARIANT). 
// The vector structure is dynamically allocated, so it should be deleted 
// by the caller after being used.
DWORD OLEStream::ReadVT_VECTOR(DWORD dwType, VECTOR **ppVector)
{
  VECTOR *  pvector;
  DWORD     i, strLen = 0; 
  
  // Erase the VT_VECTOR bit, so remaining is the data type in vector
  if (dwType & VT_VECTOR)
    dwType ^= VT_VECTOR;

  // Read the vector length
  if ( !ReadVT_I4(&strLen))
    return FALSE;

  // If number of vector is 0, return with total length of vector of byte count( 4 bytes )
  if (strLen == 0)
    return sizeof(DWORD);

  // Allocate a vector
  pvector = AllocVECTOR(dwType, strLen);
  if ( !pvector ) 
    return 0;
  else
    *ppVector = pvector;     
  
  // Read the data according to its type
  switch (dwType)
  {
    case VT_I1:
    case VT_UI1:
    {   
      // Read each char in the vector. It is same as string just without byte count at the beginning.
      for ( i= 0; i < pvector->cElements; i++ )
        if ( !Read(&pvector->prgb[i], sizeof(char)))
          return FALSE;

      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // the total length of char *. 
      return pvector->cElements * sizeof(char) + sizeof(DWORD); 
    }
    case VT_I2:
    case VT_UI2:    
    case VT_BOOL:   
    {
      // Read each short in the vector. Here we don't use ReadVT_I2 since the short type in 
      // vector is not 32-bit padded.
      for ( i= 0; i < pvector->cElements; i++ ) 
        if ( !ReadVT_I2_NoPad(&pvector->prgw[i])) 
          return FALSE;
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // the total length of boolean. 
      return pvector->cElements * sizeof(short) + sizeof(DWORD);
    } 
    case VT_I4:
    case VT_UI4:    
    case VT_ERROR:
    {
      // Read each long in the vector
      for ( i= 0; i < pvector->cElements; i++ )
        if ( !ReadVT_I4(&pvector->prgdw[i]))
          return FALSE;
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // the total length of longs
      return pvector->cElements * sizeof(DWORD) + sizeof(DWORD); 
    }
    case VT_R4:
    {
      // Read each float in the vector
      for ( i= 0; i < pvector->cElements; i++ )
        if ( !ReadVT_R4(&pvector->prgflt[i]))
          return FALSE;
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // the total length of float
      return pvector->cElements * sizeof(float) + sizeof(DWORD);
    }
    case VT_R8:
    {
      // Read each double in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !ReadVT_R8(&pvector->prgdbl[i]))
          return FALSE;
      }
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // the total length of doubles
      return pvector->cElements * sizeof(double) + sizeof(DWORD);
    }
    case VT_LPSTR:
    {
      DWORD len = 0; 
      char * pstr; 

      memset(pvector->prgpsz, 0, pvector->cElements * sizeof(char *)); 
      
      // Read each string in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
        strLen = ReadVT_LPSTR(&pstr);

        // Save the string into vector  
        pvector->prgpsz[i] = pstr; 
        
        // Add the total length of strings (bytes counts)
        len += strLen; 
      }
      
      // Return the total bytes read, which is vector elements count (4 bytes) 
      // plus total length of strings
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_BSTR:
    case VT_LPWSTR:
    {
      DWORD len = 0; 
      WCHAR * pwstr; 

      memset(pvector->prgpwz, 0, pvector->cElements * sizeof(WCHAR *)); 

      // Read each wide string in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
        strLen = ReadVT_LPWSTR(&pwstr);

        // Save the wide string into vector 
        pvector->prgpwz[i] = pwstr; 

        // Add the total length of wide strings (bytes counts)
        len += strLen; 
      }
      
      // Return the total bytes read, which is vector elements  count (4 bytes) plus 
      // total length of wide strings
      return len * sizeof(char) + sizeof(DWORD); 
    }
    case VT_BLOB:
    {
      DWORD len = 0; 
            
      // Read each blob in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
      
        // If read wrong, continue next
        if ( !( strLen = ReadVT_BLOB(&pvector->prgblob[i])) )
          continue;
        
        // Add the total length of blobs (bytes counts)
        len += strLen; 
      }
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // total length of blobs
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_CF:
    {
      DWORD len = 0; 
            
      // Read each cf in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
      
        // If read wrong, continue next
        if ( !( strLen = ReadVT_CF(&pvector->pcd[i])) )
          continue;
        
        // Add the total length of cfs (bytes counts)
        len += strLen; 
      }
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // total length of cfs
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_CLSID:
    {
      CLSID *pcls = NULL; 
            
      // Read each clsid in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {
      
        // If read wrong, continue next
        if ( !( strLen = ReadVT_CLSID(&pcls)) )
          continue;
        
        // Save the clsid into vector 
        pvector->pclsid[i] = *pcls; 
      }
      
      // Return the total bytes read, which is vector elements count (4 bytes) plus 
      // total length of clsid
      return pvector->cElements * sizeof(CLSID) + sizeof(DWORD);
    }
    unsigned long tmpType;
    case VT_VARIANT:
    {
      // Reset the memory 
      memset(pvector->pvar, 0, pvector->cElements * sizeof(VARIANT));
      
      // Read each element in the vector
      for ( i= 0; i < pvector->cElements; i++ ) {

        // Read the variant type first ( unsigned long )
        ReadVT_I4(&tmpType);
        pvector->pvar[i].vt = tmpType;
        //ReadVT_I4((DWORD *)&pvector->pvar[i].vt);
        strLen += sizeof(long);
        
        // Read the data into the variant accordingly. The length(bytes counts) is 
        // returned in strLen. (Before call ReadVT(), variant must be set to zero)
        strLen += ReadVT(&pvector->pvar[i]);  
        
      }
      
      // Return the total length of vector(bytes counts), which is total length fix types 
      // (number of DWORDs) plus total variable types length (in strLen), plus the vector
      // elements count (4 bytes). 
      return sizeof(DWORD) + strLen;
    }
    default:
      {
      // BUG: Any types not listed above are not supported. 
      //cout << "The type is not supported.";
      return FALSE;
      }
  }
}


// FIX_DICT_PROP - added new method: ReadDICT_ENTRIES()
// 
// Reads the entries in a dictionary data structure, and return total bytes that
// been read. It assumes that the next thing in the stream is the first of one or
//  more entries. It also assumes that pDict->cEntries has already been allocated
//  with an array of pDict->cEntries elements.
DWORD OLEStream::ReadDICT_ENTRIES(DICTIONARY *pDict)
{
  ENTRY *   pEntry;
  DWORD     i, rem, strLen = 0;
  
  pEntry = pDict->rgEntry;        // Get ptr to start of array of entries
  
  for(i=0; i < pDict->cbEntries; i++, pEntry++) {
    if( !ReadVT_I4(&pEntry->dwPropID))
      return false;

    if( !ReadVT_I4(&pEntry->cb))
      return false;
    
    strLen += (sizeof(DWORD) * 2);    // Increment for the 2 VT_I4 reads
    
    // The string size read into 'cbName' includes the null terminator.
    //  There is no string padding to account for in dictionary strings.
    //  Allocate memory for the string and read it in.
    pEntry->sz = new char[pEntry->cb];
    if( !pEntry->sz )
      return false;
    
    if ( !Read( pEntry->sz, pEntry->cb) )
      return false;
      
    strLen += pEntry->cb;     // Increment for the string read
  }

  // Get the remaining bytes required to pad to the next 32-bit boundary
  rem = (strLen % 4) ? 4 : 0;
  rem -= strLen % 4; 
  
  return( strLen + rem);
}


//--------------------------- WriteVT_xx functions ---------------------------------/

// WriteVT()
// This is a generic function that writes a property of any type in pData to a stream.
DWORD OLEStream::WriteVT(DWORD dwType, BYTE *pData)
{
  // If the data is VECTOR, write it
  if (dwType & VT_VECTOR)
  {
    return WriteVT_VECTOR(dwType, (VECTOR *)pData); 
  }

  // Write the data according to its type 
  switch(dwType)
  {
    case VT_I1:
    case VT_UI1:
      return WriteVT_I2((WORD *)pData);
    case VT_I2:
    case VT_UI2:
      return WriteVT_I2((WORD *)pData);
    case VT_I4:
    case VT_UI4:
      return WriteVT_I4((DWORD *)pData);
    case VT_R4:
      return WriteVT_R4((float *)pData);
    case VT_R8:
      return WriteVT_R8((double *)pData);
    case VT_CY:
      return WriteVT_CY((CY *)pData);
    case VT_DATE:
      return WriteVT_DATE((double *)pData);
    case VT_BOOL:
      return WriteVT_BOOL((BOOL *)pData);
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
      return WriteVT_I8((LARGE_INTEGER *)pData);
    case VT_LPSTR:
      return WriteVT_LPSTR(*(char **)pData);
    case VT_LPWSTR:
      return WriteVT_LPWSTR(*(WCHAR **)pData);
    case VT_BSTR:
      return WriteVT_LPWSTR(*(WCHAR **)pData);
    case VT_FILETIME:
      return WriteVT_FILETIME((FILETIME *)pData);
    case VT_BLOB:
      return WriteVT_BLOB((BLOB *)pData);
    case VT_STREAM:
      return WriteVT_STREAM(*(char **)pData);
    case VT_STORAGE:
      return WriteVT_STORAGE(*(char **)pData);
    case VT_STREAMED_OBJECT:
      return WriteVT_STREAMED_OBJECT(*(char **)pData);
    case VT_STORED_OBJECT:
      return WriteVT_STORED_OBJECT(*(char **)pData);
    case VT_BLOB_OBJECT:
      return WriteVT_BLOB_OBJECT(*(char **)pData);
    case VT_CF:
      return WriteVT_CF((CLIPDATA *)pData);
    case VT_CLSID:
      return WriteVT_CLSID((CLSID *)pData);
    default:
      return FALSE;
  }
}

// WriteVT()
// This is a generic function that writes a property of any type to a stream.
DWORD OLEStream::WriteVT(VARIANT * variant)
{
  // If the data is VECTOR, write it
  if (variant->vt & VT_VECTOR)
  {
    return WriteVT_VECTOR(variant->vt, (VECTOR *)V_BYREF(variant)); 
  }

  // Write the data according to its type 
  switch(variant->vt)
  {
    case VT_I1:
    case VT_UI1:
      return WriteVT_I2((WORD *)&V_I2(variant));
    case VT_I2:
    case VT_UI2:
      return WriteVT_I2((WORD *)&V_I2(variant));
    case VT_I4:
    case VT_UI4:
      return WriteVT_I4((DWORD *)&V_I4(variant));
    case VT_R4:
      return WriteVT_R4(&V_R4(variant));
    case VT_R8:
      return WriteVT_R8(&V_R8(variant));
    case VT_CY:
      return WriteVT_CY((CY *)&V_CY(variant));
    case VT_DATE:
      return WriteVT_DATE(&V_R8(variant));
    case VT_BOOL:
      return WriteVT_BOOL((BOOL *)&V_BOOL(variant));
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
      return WriteVT_I8((LARGE_INTEGER *)&V_CY(variant));
    case VT_LPSTR:
#ifdef _UNICODE
      return WriteVT_LPSTR((LPSTR)V_UI1REF(variant));
#else
      return WriteVT_LPSTR(V_BSTR(variant));
#endif
    case VT_LPWSTR:
      return WriteVT_LPWSTR((WCHAR *)V_BYREF(variant));
    case VT_BSTR:
      return WriteVT_LPWSTR((WCHAR *)V_BYREF(variant));
    case VT_FILETIME:
      return WriteVT_FILETIME((FILETIME *)&V_R8(variant));
    case VT_BLOB:
      return WriteVT_BLOB((BLOB *)V_BYREF(variant));
    case VT_STREAM:
      return WriteVT_STREAM((char *)V_BYREF(variant));
    case VT_STORAGE:
      return WriteVT_STORAGE((char *)V_BYREF(variant));
    case VT_STREAMED_OBJECT:
      return WriteVT_STREAMED_OBJECT((char *)V_BYREF(variant));
    case VT_STORED_OBJECT:
      return WriteVT_STORED_OBJECT((char *)V_BYREF(variant));
    case VT_BLOB_OBJECT:
      return WriteVT_BLOB_OBJECT((char *)V_BYREF(variant));
    case VT_CF:
      return WriteVT_CF((CLIPDATA *)V_BYREF(variant));
    case VT_CLSID:
      return WriteVT_CLSID((CLSID *)V_BYREF(variant));
    default:
      return FALSE;
  }
}

// WriteVT_I2()
// Write a 2-byte WORD value to a stream. The bytes are swapped.
// Caution: Since the value in the stream is zero-padded to a 32-bit boundary, the actual data 
// write to the stream is 4 bytes despite it is a short type. 
DWORD OLEStream::WriteVT_I2(WORD* pw)
{
  WORD w, wPad = 0;

  w = *pw;
  
  // Swap bytes if flag is set
  #ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)&w, sizeof(short));   
  #endif

  // Write the data with padded bytes
  if ( !Write(&w, sizeof(WORD)) || !Write(&wPad, sizeof(WORD)))
    return FALSE;

  // Return the total bytes write, which is size of long ( 4 bytes )
  // Although the it is short type, the data is padded to a 32-bit 
  // boundary so the actual data write to stream is 4 bytes.
  return sizeof (DWORD);
}

// WriteVT_I2_NoPad()
// Write a 2-byte WORD value to a stream without padded to 32-bit boundary, so the actual data 
// write to the stream is 2 bytes. The bytes are swapped.
DWORD OLEStream::WriteVT_I2_NoPad(WORD* pw)
{
  WORD w;

  w = *pw;
  
  // Swap bytes if flag is set
  #ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)&w, sizeof(short));   
  #endif

  // Write the data without padded bytes
  if ( !Write(&w, sizeof(WORD)) )
    return FALSE;

  // Return the total bytes write, which is size of short ( 2 bytes )
  return sizeof (WORD);
}

// WriteVT_I4()
// Write a 4-byte DWORD value to a stream. The bytes are swapped.
DWORD OLEStream::WriteVT_I4(DWORD* pdw)
{
  DWORD dw; 
  
  dw = *pdw; 

  // Swap bytes if flag is set
  #ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)&dw, sizeof(long));   
  #endif

  // Write the data 
  if ( !Write(&dw, sizeof (DWORD)))
    return FALSE;

  // Return the total bytes write, which is size of long ( 4 bytes )
  return sizeof (DWORD);
}

// WriteVT_R4()
// Write a 32-bit IEEE floating point value to a stream. The bytes are swapped
DWORD OLEStream::WriteVT_R4(float* pflt)
{
  float flt; 
  
  flt = *pflt; 

  // Swap bytes if flag is set
  #ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)&flt, sizeof(float));   
  #endif

  // Write the data 
  if ( !Write(&flt, sizeof (float)))
    return FALSE;

  // Return the total bytes write, which is size of float ( 4 bytes )
  return sizeof (float);
}

// WriteVT_R8()
// Write a 64-bit IEEE floating point value to a stream. The bytes are swapped
DWORD OLEStream::WriteVT_R8(double* pdbl)
{
  double dbl; 
  
  dbl = *pdbl; 

  // Swap bytes if flag is set
  #ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes )
    SwapBytes((BYTE *)&dbl, sizeof(double));  
  #endif

  // Write the data 
  if ( !Write(&dbl, sizeof (double)))
    return FALSE;

  // Return the total bytes write, which is size of double ( 8 bytes )
  return sizeof (double);
}

// WriteVT_CY()
// Write a 8-byte two's complement integer (scaled by 10,000) to a stream.
// It's usually used for currency amounts (hence the CY type). The bytes are swapped
DWORD OLEStream::WriteVT_CY(CY *pcy)
{
#ifdef macintosh
  if ( !WriteVT_I4((DWORD *)&pcy->Hi) || !WriteVT_I4(&pcy->Lo) )
#else
  if ( !WriteVT_I4(&pcy->Lo) || !WriteVT_I4((DWORD *)&pcy->Hi) )
#endif
    return FALSE;

  // Return the total bytes write, which is size of two DWORDs ( 8 bytes )
  return sizeof (CY);
}

// WriteVT_I8()
// Write a 8-byte integer to a stream. The bytes are swapped
DWORD OLEStream::WriteVT_I8(LARGE_INTEGER *pli)
{
  if ( !WriteVT_I4(&pli->LowPart) || !WriteVT_I4((DWORD *)pli->HighPart) )
    return FALSE;
    
  // Return the total bytes write, which is size of two DWORDs ( 8 bytes )
  return sizeof(LARGE_INTEGER);
}

// WriteVT_BOOL()
// Write a boolean to a stream. The bytes are swapped. An OLE boolean is defined to 
// be a 2-byte WORD containing 0 (FALSE) or -1 (TRUE). It is padded to a 32-bit boundary.
DWORD OLEStream::WriteVT_BOOL(BOOL* pf)
{
  DWORD dw; // Intermediate dword because bools are padded to 32 bits

  // Set the OLE boolean, 0 (FALSE) or -1 (TRUE) 
  dw = *pf ? -1 : 0;
  
  if (!WriteVT_I4(&dw))
    return FALSE;

  // Although the BOOL is short type, the OLE boolean is padded to a 32-bit 
  // boundary so the actual data write to stream is 4 bytes.
  return sizeof (DWORD);
}

// WriteVT_LPSTR()
// Writes a zero-terminated character string padded to 32-bit boundary to a stream.
DWORD OLEStream::WriteVT_LPSTR(char *psz)
{
  DWORD dwLength, rem;

  dwLength = strlen(psz); 
  
  if ( dwLength == 0 ) {
    // Write the count of string
    WriteVT_I4(&dwLength);
    return sizeof(DWORD); 
  }
  else
    dwLength++; // Count includes terminating zero if it is not null

  // Write the count of string
  if (!WriteVT_I4(&dwLength))
    return FALSE;

  // Write the string to the stream
  if ( !Write(psz, dwLength * sizeof (char)) )
    return FALSE;

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (dwLength % 4) ? 4 : 0;
  rem -= dwLength % 4; 
  
  // Set the stream pointer to next 32-bit boundary
  Seek(rem, STREAM_SEEK_CUR); 
    
  // Return the total bytes write to stream, including bytes count, actual data length and
  // padded bytes       
  return sizeof(DWORD) + rem + dwLength;
}

// WriteVT_LPSTR_NoPad()
// Writes a zero-terminated character string without padded to 32-bit boundary to a stream.
DWORD OLEStream::WriteVT_LPSTR_NoPad(char *psz)
{
  DWORD dwLength;

  dwLength = strlen(psz); 
  
  if ( dwLength == 0 ) {
    // Write the count of string
    WriteVT_I4(&dwLength);
    return sizeof(DWORD); 
  }
  else
    dwLength++; // Count includes terminating zero if it is not null

  // Write the count of string
  if (!WriteVT_I4(&dwLength))
    return FALSE;

  // Write the string to the stream
  if ( !Write(psz, dwLength * sizeof (char)) )
    return FALSE;

  // Return the total bytes write to stream, including bytes count, actual data length and
  // padded bytes       
  return sizeof(DWORD) + dwLength;
}

// WriteVT_LPWSTR()
// Writes a zero-terminated wide character string padded to 32-bit boundary to a stream.
// Return total bytes write to stream in bytes ( not in WCHAR )
DWORD OLEStream::WriteVT_LPWSTR(WCHAR *pwsz)
{
  DWORD dwLength, rem = 0, wcount;

  wcount = wcslen(pwsz);  // Get wide char count doesn't include terminating zero
  
  if ( wcount == 0 ) {
    // CHG_MT_EXT_STR - since wide strings are null-terminated, and the terminiator is
    //  included in the count, an empty string containing a null and a length of 1
    //  is written
    wcount = 1;
    WriteVT_I4(&wcount);    // Write length = 1
    wcount = 0;
    WriteVT_I4(&wcount);    // Write null terminator
    return(sizeof(DWORD) *2);
  }
  else
    wcount++; // Count includes terminating zero if it is not null

  dwLength = sizeof (WCHAR) * wcount;   // Get the bytes count including terminating NULL
  
  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (dwLength % 4) ? 4 : 0;
  rem -= dwLength % 4; 
  
  // Write the WCHAR count
  if (!WriteVT_I4(&wcount))
    return FALSE;

  // Write the wide char string to the stream
  for ( DWORD i = 0; i < wcount; i++ )  {

    // Write the wide char  
    if ( !WriteVT_I2_NoPad(&pwsz[i]))
      return FALSE;
  }

  // Set the stream pointer to next 32-bit boundary
  Seek(rem, STREAM_SEEK_CUR); 
        
  // Return the total bytes write to stream, including WCHAR count, actual data length and
  // padded bytes       
  return sizeof (DWORD) + (dwLength + rem) * sizeof (char);
}

// WriteVT_LPWSTR_NoPad()
// Writes a zero-terminated wide character string without padded to 32-bit boundary to a stream.
// Return total bytes write to stream in bytes ( not in WCHAR )
DWORD OLEStream::WriteVT_LPWSTR_NoPad(WCHAR *pwsz)
{
  DWORD dwLength, wcount;

  dwLength = wcslen(pwsz);  // Get wide char count doesn't include terminating zero
  
  if ( dwLength == 0 ) {
    // Write the count of wide string
    WriteVT_I4(&dwLength);
    return sizeof(DWORD); 
  }
  else  
    dwLength++; // Count includes terminating zero if it is not null

   wcount = sizeof (WCHAR) * dwLength;
  // Write the WCHAR count
  if (!WriteVT_I4(&wcount))
    return FALSE;

  // Write the wide char string to the stream
  for ( DWORD i = 0; i < dwLength; i++ )  {
  
    // Write the wide char  
    if ( !WriteVT_I2_NoPad(&pwsz[i]))
      return FALSE;
  }

  // Return the total bytes write to stream, including WCHAR count, actual data length and
  // padded bytes       
  return sizeof (DWORD) + (dwLength) * sizeof (WCHAR);
}

// WriteVT_FILETIME()
// Writes a FILETIME structure to a stream. The bytes is swapped 
DWORD OLEStream::WriteVT_FILETIME(FILETIME *pft)
{
  if (!WriteVT_I4(&pft->dwLowDateTime) || !WriteVT_I4(&pft->dwHighDateTime))
    return FALSE;

  // Return the total bytes write, which is size of two DWORDs ( 8 bytes )
  return sizeof (FILETIME);
}

// OLEStream::WriteVT_BLOB()
// Writes a BLOB (length + raw block of data) to a stream. The bytes is swapped
DWORD OLEStream::WriteVT_BLOB(BLOB *pblob)
{
  WORD rem = 0; 
  
  // Write the blob count
  if (!WriteVT_I4(&pblob->cbSize))
    return FALSE;

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (pblob->cbSize % 4) ? 4 : 0;
  rem -= (unsigned short) (pblob->cbSize % 4); 
  
  // Write the blob according to its size
  if ( !Write(pblob->pBlobData, pblob->cbSize) )
    return FALSE;

  // Set the stream pointer to next 32-bit boundary
  Seek(rem, STREAM_SEEK_CUR); 

  // Return the total bytes write to stream, including blob count, actual data length and
  // padded bytes       
  return sizeof (DWORD) + pblob->cbSize + rem;
}

// WriteVT_VARIANT()
// Writes a VARIANT into a stream (which is really four WORDs, and a double). The bytes is swapped
DWORD OLEStream::WriteVT_VARIANT(VARIANT *pvar)
{
  VARIANT var; 
  
  var = *pvar; 
  
  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes ) {
    SwapBytes((BYTE *)&var.vt,      sizeof(short));   
    SwapBytes((BYTE *)&var.wReserved1,  sizeof(short));   
    SwapBytes((BYTE *)&var.wReserved2,  sizeof(short));   
    SwapBytes((BYTE *)&var.wReserved3,  sizeof(short));   
    SwapBytes((BYTE *)&var.dblVal,    sizeof(double));  
  }
#endif

  if ( !Write((WORD *)&var.vt,     sizeof(short)) || 
     !Write((WORD *)&var.wReserved1, sizeof(short)) ||
     !Write((WORD *)&var.wReserved2, sizeof(short)) ||
     !Write((WORD *)&var.wReserved3, sizeof(short)) ||
     !Write((double *)&var.dblVal,   sizeof(double)) )
    return FALSE;

  // Return the total bytes write, which is size of VARIANT ( 16 bytes )
  return sizeof (VARIANT);
}

// OLEStream::WriteVT_CF()
// Writes a clipboard data structure (length + format + raw block of data)
// to a stream.
DWORD OLEStream::WriteVT_CF(CLIPDATA *pclipdata)
{
  WORD rem; 
    
  // Write the clipdata count and format
  if ( !WriteVT_I4(&pclipdata->cbSize) )
    return FALSE;

  // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
  rem = (pclipdata->cbSize % 4) ? 4 : 0;
  rem -= (unsigned short) (pclipdata->cbSize % 4); 

  // Write the clipdata according to its size
  if ( !Write(pclipdata->pClipData, pclipdata->cbSize))
    return FALSE;

  // Set the stream pointer to next 32-bit boundary
  Seek(rem, STREAM_SEEK_CUR); 

  // Return the total bytes write to stream, including clipdata count and format, actual data 
  // length and padded bytes      
  return sizeof (DWORD) + pclipdata->cbSize + rem;
}

// OLEStream::WriteVT_CLSID()
// Writes a CLSID to a stream (which is really a DWORD, two WORDs, and 8 bytes).
DWORD OLEStream::WriteVT_CLSID(CLSID *pclsid)
{
  CLSID cls; 
  
  cls = *pclsid; 

  // Swap bytes if flag is set
#ifdef  IN_BIG_ENDIAN
  if ( fSwapBytes ) {
    SwapBytes((BYTE *)&cls.Data2, sizeof(short));   
    SwapBytes((BYTE *)&cls.Data3, sizeof(short));   
  }
#endif

  if (!WriteVT_I4(&cls.Data1) || !Write((WORD *)&cls.Data2, sizeof(short)) ||
    !Write((WORD *)&cls.Data3, sizeof(short)))
    return FALSE; 
    
  if ( !Write(cls.Data4, 8*sizeof(char)))
    return FALSE;

  // Return the total bytes write, which is size of CLSID ( 16 bytes )
  return sizeof (CLSID);
}

// WriteVT_VECTOR()
// Writes a vector data structure (cElements * length of each element). The total bytes 
// written to the disk must be padded to 32-bit and returned. The vector type is like 
// VT_XX | VT_VECTOR. Right now 11 types are supported (char, WORD, DWORD, float, double, 
// LPSTR, LPWSTR, BLOB, CLIPDATA, CLSID, VARIANT).
DWORD OLEStream::WriteVT_VECTOR(DWORD dwType, VECTOR *pvector)
{
  DWORD   i, rem;
  DWORD strLen = 0;
      
  
  // Erase the VT_VECTOR bit, so remaining is the data type in vector
  if (dwType & VT_VECTOR)
    dwType ^= VT_VECTOR;
  
  // If vector pointer is NULL, return FALSE
  //if ( !pvector)  return FALSE; 
  if ( !pvector)
  { 
    strLen = 0;
    if(!WriteVT_I4(&strLen))
      return FALSE;
    return sizeof(DWORD);
  }
  // Write the count of vector elements. Return FALSE if wrong
  if ( !WriteVT_I4(&pvector->cElements))
    return FALSE;

  // If nothing in the vector, return size of vector count
  if ( pvector->cElements == 0 )
    return sizeof(DWORD);

  // Write the data and according to its type
  switch (dwType)
  {
    case VT_I1:
    case VT_UI1:
      // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
      rem = (pvector->cElements % 4) ? 4 : 0;
      rem -= pvector->cElements % 4; 
      
      // Write each char in the vector. It is same as string just without byte count.
      // PTCH_DCG - Pass 'prgb' to Write, not '&prgb', since it's an array
      if ( !Write( pvector->prgb, pvector->cElements * sizeof(char)) )
        return FALSE;

      // Set the stream pointer to next 32-bit boundary
      Seek(rem, STREAM_SEEK_CUR); 
                
      // Return the total bytes write to stream, including vector count, actual data length 
      // and padded bytes       
      return (pvector->cElements + rem) * sizeof(char) + sizeof(DWORD);       
    case VT_I2:
    case VT_UI2:    
    case VT_BOOL:   
      // Get the remaining bytes to 32-bit for it is going to be padded to 32-bit boundary
      rem = pvector->cElements % 2;     
      
      for ( i= 0; i < pvector->cElements; i++ ) {
        // Write each short in the vector. Here we  use WriteVT_I2_NoPad since the short 
        // type in vector is not 32-bit padded.
        if ( !WriteVT_I2_NoPad(&pvector->prgw[i]) )
          return FALSE;         
      }
      
      // Set the stream pointer to next 32-bit boundary
      Seek(rem * sizeof(short), STREAM_SEEK_CUR); 
      
      // Return the total bytes write to stream, including vector count, actual data length 
      // and padded bytes       
      return (pvector->cElements + rem) * sizeof(short) + sizeof(DWORD); 
    case VT_I4:
    case VT_UI4:    
    case VT_ERROR:  
      // Write each long in the vector to the stream  
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !WriteVT_I4(&pvector->prgdw[i]))
          return FALSE;
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of longs      
      return pvector->cElements * sizeof(DWORD) + sizeof(DWORD); 
    case VT_R4:
      // Write each float in the vector to the stream 
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !WriteVT_R4(&pvector->prgflt[i]))
          return FALSE;
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of floats       
      return pvector->cElements * sizeof(float) + sizeof(DWORD); 
    case VT_R8:
      // Write each double in the vector to the stream  
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !WriteVT_R8(&pvector->prgdbl[i]))
          return FALSE;
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of doubles      
      return pvector->cElements * sizeof(double) + sizeof(DWORD); 
    case VT_LPSTR:
    {
      DWORD len = 0; 
      
      // Write each LPSTR in the vector to the stream, the length is returned in strLen
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !( strLen = WriteVT_LPSTR(pvector->prgpsz[i])) )
          return FALSE;
        len += strLen; 
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of strings      
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_BSTR:
    case VT_LPWSTR:
    {
      DWORD len = 0; 
      
      // Write each LPWSTR in the vector to the stream, the length in byte is returned in strLen
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !( strLen = WriteVT_LPWSTR(pvector->prgpwz[i])) )
          return FALSE;
        len += strLen; 
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of wide strings       
      return len * sizeof(char) + sizeof(DWORD); 
    }
    case VT_BLOB:
    {
      DWORD len = 0; 
      
      // Write each blob in the vector to the stream, the length in byte is returned in strLen
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !( strLen = WriteVT_BLOB(pvector->prgblob[i])) )
          return FALSE;
        len += strLen; 
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of blobs      
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_CF:
    {
      DWORD len = 0; 
      
      // Write each cilpdata in the vector to the stream, the length in byte is returned in strLen
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !( strLen = WriteVT_CF(pvector->pcd[i])) )
          return FALSE;
        len += strLen; 
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of clipdatas      
      return len * sizeof(char) + sizeof(DWORD);
    }
    case VT_CLSID:
    {
      // Write each CLSID in the vector to the stream, the length in byte is returned in strLen
      for ( i= 0; i < pvector->cElements; i++ ) {
        if ( !( strLen = WriteVT_CLSID((CLSID *)&pvector->pclsid[i])) )
          return FALSE;
      }
      
      // Return the total bytes write to stream, which is vector elements count (4 bytes) plus 
      // the total length of CLSIDs       
      return pvector->cElements * sizeof(CLSID) + sizeof(DWORD);
    }
    unsigned long tmpVal;
    case VT_VARIANT:
    {
      for ( i= 0; i < pvector->cElements; i++ ) {
        
        // Write a variant type first ( unsigned long) 
        tmpVal = pvector->pvar[i].vt;
        //WriteVT_I4((DWORD *)&pvector->pvar[i].vt);
        WriteVT_I4(&tmpVal);
        strLen += sizeof(long);
        
        // Write the variant accordingly. The total bytes write to the stream is returned 
        // in strLen.   
        strLen += WriteVT(&pvector->pvar[i]);

      }

      // Return the total length of vector(bytes counts), which is the vector elements count 
      // (4 bytes) plus the total length of vector elements
      return sizeof(DWORD) + strLen;
    }
    default:
    {
      // BUG: Any types not listed above are not supported. 
      //cout << "The type is not supported.";
      return FALSE;
    }
  }
}


// FIX_DICT_PROP - added new method: ReadDICT_ENTRIES()
// 
// Writes the entries in a dictionary data structure, and returns total bytes that
// written. It assumes that the last thing written to the stream was the count of
//  the number of entries. 
DWORD OLEStream::WriteDICT_ENTRIES(DICTIONARY *pDict)
{
  ENTRY *   pEntry;
  DWORD     i, rem, writeCnt, dwLength = 0;
  
  pEntry = pDict->rgEntry;        // Get ptr to start of array of entries
  
  for(i=0; i < pDict->cbEntries; i++, pEntry++) {
    if( !WriteVT_I4(&pEntry->dwPropID))
      return false;

    if( !WriteVT_I4(&pEntry->cb))
      return false;
    
    dwLength += (sizeof(DWORD) * 2);  // Increment for the 2 VT_I4 writes

    // Write the string to the stream
    if ( !Write(pEntry->sz, pEntry->cb) )
      return false;
    dwLength += pEntry->cb;     // Increment for the string written
  }

  // Get the remaining bytes required to pad to the next 32-bit boundary
  rem = (dwLength % 4) ? 4 : 0;
  rem -= dwLength % 4; 
  
  return( dwLength + rem);
}


// OLEStream::SizeVT()
// Returns the size of an element of the specified type
DWORD OLEStream::SizeVT(DWORD dwType)
{
  switch(dwType)
  {
    case VT_I1:
    case VT_UI1:
      return sizeof (char);
    case VT_I2:
    case VT_UI2:
      return sizeof (WORD);
    case VT_I4:
    case VT_UI4:
    case VT_ERROR:
      return sizeof (DWORD);
    case VT_R4:
      return sizeof (float);
    case VT_R8:
      return sizeof (double);
    case VT_CY:
      return sizeof (CY);
    case VT_DATE:
      return sizeof (double);
    case VT_BOOL:
      return sizeof (BOOL);
    case VT_I8:
    case VT_UI8:
    case VT_INT:
    case VT_UINT:
      return sizeof (LARGE_INTEGER);
    case VT_LPSTR:
    case VT_STREAM:
    case VT_STORAGE:
    case VT_STREAMED_OBJECT:
    case VT_STORED_OBJECT:
    case VT_BLOB_OBJECT:
      return sizeof(char *);
    case VT_BSTR:
    case VT_LPWSTR:
      return sizeof(WCHAR *);
    case VT_FILETIME:
      return sizeof (FILETIME);
    case VT_BLOB:
      return sizeof (BLOB);
    case VT_CF:
      return sizeof (CLIPDATA);
    case VT_CLSID:
      return sizeof (CLSID);
    case VT_VECTOR:
      return sizeof (VECTOR);
    case DICT_PROP_TYPE:
      return sizeof (DICTIONARY);
    default:
      return 0;
  }
}

// VTtoString()
// Returns a string representation of a VT type data. The returned string is dynamically
// allocated, so it should be deleted after use.
Boolean OLEStream::VTtoString(VARIANT *variant, char **ppsz)
{
  char  *psz=NULL, *pstr=NULL; 
  char  pstr1[512]; 
  DWORD len, vtType, i; 
  
  if (variant->vt & VT_VECTOR)  {
    VECTOR *pvec = (VECTOR *)V_BYREF(variant);
    
    psz   = new char [ 512 * pvec->cElements ]; 
    if (psz==NULL) {
      return FALSE;
    }
    sprintf(psz, "[VECTOR of %lu elements, type is %s.]\n", 
        pvec->cElements, VariantName(variant->vt ^ VT_VECTOR));
    
    vtType = variant->vt ^ VT_VECTOR;     
    for ( i = 0; i < pvec->cElements; i++ ) 
    {
      if ( vtType == VT_VARIANT )
      {
        VTtoString(&pvec->pvar[i], &pstr); 
        sprintf(pstr1, "%50s    %s\n", VariantName(pvec->pvar[i].vt), pstr);          

        strcat(psz, pstr1); 
        delete pstr; 
        continue; 
      }
      
      switch (vtType)
      {
        case VT_I1:
        case VT_UI1:
          VTtoString(&pvec->prgb[i], vtType, &pstr); 
          break; 
        case VT_I2:
        case VT_UI2:
        case VT_BOOL:
          VTtoString((BYTE *)&pvec->prgw[i], vtType, &pstr); 
          break; 
        case VT_I4:
        case VT_UI4:
        case VT_ERROR:
          VTtoString((BYTE *)&pvec->prgdw[i], vtType, &pstr); 
          break; 
        case VT_R4:
          VTtoString((BYTE *)&pvec->prgflt[i], vtType, &pstr); 
          break; 
        case VT_R8:
          VTtoString((BYTE *)&pvec->prgdbl[i], vtType, &pstr); 
          break; 
        case VT_LPSTR:
          VTtoString((BYTE *)pvec->prgpsz[i], vtType, &pstr); 
          break; 
        case VT_BSTR:
        case VT_LPWSTR:
          VTtoString((BYTE *)pvec->prgpwz[i], vtType, &pstr); 
          break; 
        case VT_BLOB:
          VTtoString((BYTE *)pvec->prgblob[i], vtType, &pstr); 
          break; 
        case VT_CF:
          VTtoString((BYTE *)pvec->pcd[i], vtType, &pstr); 
          break; 
        case VT_CLSID:
          VTtoString((BYTE *)&pvec->pclsid[i], vtType, &pstr); 
          break; 
        default:
          pstr = new char[256]; 
          if (pstr==NULL) {
            return FALSE;
          }
          sprintf(pstr, "The type is not supported in vector"); 
          break;
      }
      strcat(psz, "\t\t\t\t\t\t\t\t\t\t\t"); 
      strcat(psz, pstr); 
      strcat(psz, "\n"); 
      delete pstr; 
    } 
      
    *ppsz = psz; 
    
    return TRUE;
  }
  
  switch(variant->vt)
  {
    case VT_I1:
    case VT_UI1:
      psz = new char[2];  
      if (psz==NULL) {
        return FALSE;
      }
      psz[0] = char(V_I4(variant));
      psz[1] = '\0';
      break;
    case VT_I2:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }
    
      sprintf(psz, "%d", V_I2(variant));
      break;
    case VT_UI2:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }
  
      sprintf(psz, "%u", V_I2(variant));
      break;
    case VT_I4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%ld", V_I4(variant));
      break;
    case VT_UI4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%lu", V_I4(variant));
      break;
    case VT_R4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%f", V_R4(variant));
      break;
    case VT_R8:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%f", V_R8(variant));
      break;
    case VT_CY:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

#ifdef macintosh
      sprintf(psz, "$%li.%li", V_CY(variant).Hi, V_CY(variant).Lo);
#else
      sprintf(psz, "$%li.%li", V_CY(variant).Lo, V_CY(variant).Hi);
#endif
      break;
    case VT_DATE:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }
 
      sprintf(psz, "%f s", V_DATE(variant));
      break;
    case VT_BOOL:
      psz = new char [256];
      if (psz==NULL) {
        return FALSE;
      }

      strcpy(psz, V_BOOL(variant) ? "TRUE" : "FALSE");
      break;
    case VT_ERROR:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%ld", V_ERROR(variant));
      break;
    case VT_I8:
    case VT_INT:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      LARGE_INTEGER li;
      li = *(LARGE_INTEGER *)&V_CY(variant);
      sprintf(psz, "%ld%ld", li.HighPart, li.LowPart);
      break;
    case VT_UI8:
    case VT_UINT:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      li = *(LARGE_INTEGER *)&V_CY(variant);
      sprintf(psz, "%lu%lu", li.HighPart, li.LowPart);
      break;
    case VT_LPSTR:
    case VT_STREAM:
    case VT_STORAGE:
    case VT_STREAMED_OBJECT:
    case VT_STORED_OBJECT:
    case VT_BLOB_OBJECT:
#ifdef _UNICODE
      if ( V_UI1REF(variant) )
      {
        len = strlen((LPSTR)V_UI1REF(variant)) + 1;   
        psz = new char [len];  
        strcpy(psz, (LPSTR)V_UI1REF(variant));
      }
      else
      {
        len = 1;   
        psz = new char [len]; 
        if (psz==NULL) {
          return FALSE;
        }

        *psz = 0;
      }
      break;
#else
      if ( V_BSTR(variant) )
      {
        len = strlen(V_BSTR(variant)) + 1;   
        psz = new char [len];   
        if (psz==NULL) {
          return FALSE;
        }
 
        strcpy(psz, V_BSTR(variant));
      }
      else
      {
        len = 1;   
        psz = new char [len]; 
        if (psz==NULL) {
          return FALSE;
        }
  
        *psz = 0;
      }
      break;

#endif
    case VT_BSTR:
    case VT_LPWSTR:
#ifdef _WINDOWS 
      WideCharToMultiByte(CP_ACP, 0, (WCHAR *)V_BYREF(variant), -1, psz, 128, NULL, NULL);
#else
  #ifdef _MAC
      psz = WideCharToMultiByte((WCHAR *)V_BYREF(variant));
  #endif 
#endif
      break;
    case VT_FILETIME:
#ifdef _WINDOWS 
      SYSTEMTIME st;
      FileTimeToSystemTime((FILETIME *)&V_CY(variant), &st);
      sprintf(psz, "%02i/%02i/%02i %02i:%02i:%02i",
          st.wMonth, st.wDay, st.wYear,
          st.wHour, st.wMinute, st.wSecond);
#else
  #ifdef _MAC
      unsigned long mactime; 
      psz = new char [256];
      if (psz==NULL) {
        return FALSE;
      }

      CoFileTimeToMacDateTime((FILETIME *)&V_CY(variant), &mactime); 
      sprintf(psz, "%lu", mactime);
  #endif 
#endif 
      break;
    case VT_BLOB:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "[%lu byte BLOB]", ((BLOB *)V_BYREF(variant))->cbSize);
      break;
    case VT_CF:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "[%lu byte CF]", ((CLIPDATA *)V_BYREF(variant))->cbSize);
      break;   
    case VT_CLSID:
      CLSID *pclsid;
      pclsid = (CLSID *)V_BYREF(variant);
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
          pclsid->Data1, pclsid->Data2, pclsid->Data3,
          pclsid->Data4[0], pclsid->Data4[1], pclsid->Data4[2], 
          pclsid->Data4[3], pclsid->Data4[4], pclsid->Data4[5],
          pclsid->Data4[6], pclsid->Data4[7]);
      break;
      
    default:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      strcpy(psz, "Unknown type!");
      break;
  }

  *ppsz = psz; 
  return TRUE;
}

// VTtoString()
// Returns a string representation of the data of a VT type. The data is in the pdata. The 
// returned string is dynamically allocated, so it should be deleted after use.
Boolean OLEStream::VTtoString(BYTE *pData, DWORD vtType, char **ppsz)
{
  char  *psz=NULL;
  char  *pstr=NULL; 
  char  pstr1[512]; 
  DWORD len, i; 
  
  if ( vtType & VT_VECTOR )  {
    VECTOR *pvec = (VECTOR *)pData;
    
    psz   = new char [ 512 * pvec->cElements ]; 
    if (psz==NULL) {
      return FALSE;
    }

    sprintf(psz, "[VECTOR of %lu elements, type is %s.]\n", 
        pvec->cElements, VariantName(vtType ^ VT_VECTOR));
    
    vtType = vtType ^ VT_VECTOR;    
    for ( i = 0; i < pvec->cElements; i++ ) 
    {
      if ( vtType == VT_VARIANT )
      {
        VTtoString(&pvec->pvar[i], &pstr); 
        sprintf(pstr1, "%50s    %s\n", VariantName(pvec->pvar[i].vt), pstr);          

        strcat(psz, pstr1); 
        delete pstr; 
        continue; 
      }
      
      switch (vtType)
      {
        case VT_I1:
        case VT_UI1:
          VTtoString(&pvec->prgb[i], vtType, &pstr); 
          break; 
        case VT_I2:
        case VT_UI2:
        case VT_BOOL:
          VTtoString((BYTE *)&pvec->prgw[i], vtType, &pstr); 
          break; 
        case VT_I4:
        case VT_UI4:
        case VT_ERROR:
          VTtoString((BYTE *)&pvec->prgdw[i], vtType, &pstr); 
          break; 
        case VT_R4:
          VTtoString((BYTE *)&pvec->prgflt[i], vtType, &pstr); 
          break; 
        case VT_R8:
          VTtoString((BYTE *)&pvec->prgdbl[i], vtType, &pstr); 
          break; 
        case VT_LPSTR:
          VTtoString((BYTE *)pvec->prgpsz[i], vtType, &pstr); 
          break; 
        case VT_BSTR:
        case VT_LPWSTR:
          VTtoString((BYTE *)pvec->prgpwz[i], vtType, &pstr); 
          break; 
        case VT_BLOB:
          VTtoString((BYTE *)pvec->prgblob[i], vtType, &pstr); 
          break; 
        case VT_CF:
          VTtoString((BYTE *)pvec->pcd[i], vtType, &pstr); 
          break; 
        case VT_CLSID:
          VTtoString((BYTE *)&pvec->pclsid[i], vtType, &pstr); 
          break; 
        default:
          pstr = new char[256]; 
          if (pstr==NULL) {
            return FALSE;
          } 

          sprintf(pstr, "The type is not supported in vector"); 
          break;
      }
      strcat(psz, "\t\t\t\t\t\t\t\t\t\t\t"); 
      strcat(psz, pstr); 
      strcat(psz, "\n"); 
      delete pstr; 
    }       
    *ppsz = psz; 
    
    return TRUE;
  }
  
  switch(vtType)
  {
    case VT_I1:
    case VT_UI1:
      psz = new char[2];  
      if (psz==NULL) {
        return FALSE;
      }

      psz[0] = char(*(DWORD *)pData);
      psz[1] = '\0';
      break;
    case VT_I2:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%d", *(WORD *)pData);
      break;
    case VT_UI2:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%u", *(WORD *)pData);
      break;
    case VT_I4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%ld", *(long *)pData);
      break;
    case VT_UI4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%lu", *(DWORD *)pData);
      break;
    case VT_R4:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%f", *(float *)pData);
      break;
    case VT_R8:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%f", *(double *)pData);
      break;
    case VT_CY:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

#ifdef macintosh
      sprintf(psz, "$%li.%li", ((CY *)pData)->Hi, ((CY *)pData)->Lo);
#else
      sprintf(psz, "$%li.%li", ((CY *)pData)->Lo, ((CY *)pData)->Hi);
#endif
      break;
    case VT_DATE:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%f s", *(DATE *)pData);
      break;
    case VT_BOOL:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      strcpy(psz, *(BOOL *)pData ? "TRUE" : "FALSE");
      break;
    case VT_ERROR:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%ld", *(DWORD *)pData);
      break;
    case VT_I8:
    case VT_INT:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      LARGE_INTEGER li;
      li = *(LARGE_INTEGER *)pData;
      sprintf(psz, "%ld%ld", li.HighPart, li.LowPart);
      break;
    case VT_UI8:
    case VT_UINT:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      li = *(LARGE_INTEGER *)pData;
      sprintf(psz, "%lu%lu", li.HighPart, li.LowPart);
      break;
    case VT_LPSTR:
    case VT_STREAM:
    case VT_STORAGE:
    case VT_STREAMED_OBJECT:
    case VT_STORED_OBJECT:
    case VT_BLOB_OBJECT:
      if ( pData )
      {
        len = strlen((char *)pData) + 1;   
        psz = new char [len]; 
        if (psz==NULL) {
          return FALSE;
        }

        strcpy(psz, (char *)pData);
      }
      else
      {
        len = 1;   
        psz = new char [len];  
        if (psz==NULL) {
          return FALSE;
        }

        *psz = 0;
      }
      break;
    case VT_BSTR:
    case VT_LPWSTR:
#ifdef _WINDOWS
      WideCharToMultiByte(CP_ACP, 0, (WCHAR *)pData, -1, psz, 128, NULL, NULL);
#else
  #ifdef _MAC
      psz = WideCharToMultiByte((WCHAR *)pData);
  #endif
#endif
      break;
    case VT_FILETIME:
#ifdef _WINDOWS
      SYSTEMTIME st;
      FileTimeToSystemTime((FILETIME *)pData, &st);
      sprintf(psz, "%02i/%02i/%02i %02i:%02i:%02i",
          st.wMonth, st.wDay, st.wYear,
          st.wHour, st.wMinute, st.wSecond);
#else
  #ifdef _MAC
      unsigned long mactime; 
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      CoFileTimeToMacDateTime((FILETIME *)pData, &mactime); 
      sprintf(psz, "%lu", mactime);
  #endif
#endif 
      break;
    case VT_BLOB:
      psz = new char [256];
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "[%lu byte BLOB]", ((BLOB *)pData)->cbSize);
      break;
    case VT_CF:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "[%lu byte CF]", ((CLIPDATA *)pData)->cbSize);
      break;   
    case VT_CLSID:
      CLSID *pclsid;
      pclsid = (CLSID *)pData;
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      sprintf(psz, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
          pclsid->Data1, pclsid->Data2, pclsid->Data3,
          pclsid->Data4[0], pclsid->Data4[1], pclsid->Data4[2], 
          pclsid->Data4[3], pclsid->Data4[4], pclsid->Data4[5],
          pclsid->Data4[6], pclsid->Data4[7]);
      break;
       
    default:
      psz = new char [256]; 
      if (psz==NULL) {
        return FALSE;
      }

      strcpy(psz, "Unknown type!");
      break;
  }

  *ppsz = psz; 
  return TRUE;
}
