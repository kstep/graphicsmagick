//  ----------------------------------------------------------------------------
//  MODULE    : OLEStream.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, January 23, 1996
//  DESCRIPTION : This file defines an enhanced OLE stream object 
//  COMMENT   : Enhanced OLE objects are platform independent 
//      SCCSID            : @(#)olestrm.h 1.3 10:45:32 12 Sep 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEStreams_h
  #define OLEStreams_h
  
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h   
  #include "olecore.h"
#endif
  class OLEStorage;   // to avoid include OLEStorages.h

  
//  Constants
//  ---------
  
//  Class declarations 
//  ------------------

  class OLEStream;
  
//  Class definitions
//  -----------------

  class OLEStream : public OLECore{
  
  public:
    // Create an OLEStream object from parent storage. 
        OLEStream(OLEStorage * parentStorage, LPSTREAM currentStream);
        virtual ~OLEStream();
    
    // Generic OLE stream read function
    virtual Boolean Read( void *, unsigned long );
    
    // Generic OLE stream write function
    virtual Boolean Write( const void*, unsigned long );
    
    // Set the stream to a new position. There are 3 modes,
    virtual Boolean Seek( const long offset = 0, const unsigned long mode = STREAM_SEEK_SET);

    // Get the position of end of the stream
    virtual Boolean GetEndOfFile( long* endPosition);

    // Set the size of the stream
    virtual Boolean SetSize( unsigned long );

    // Copy the stream to a new one
    virtual Boolean CopyTo( LPSTREAM, unsigned long ); 

    // Save the changes made to the stream
    virtual Boolean Commit( );

    // Discard the changes made to the stream
    virtual Boolean Revert( );
        
    // This is a generic function that takes a property of the given type out of
    // the given stream (at its current position) and puts it at the location
    // pointed to by pData. 
    virtual DWORD ReadVT(DWORD dwType, BYTE **pData);

    // This is a generic function that takes a property of the given type out of
    // the given stream (at its current position) and puts it at the location
    // pointed to by variant. 
    virtual DWORD ReadVT(VARIANT * variant);

    // Reads a 2-byte WORD value with padding to 32-bit from a stream. The bytes is swapped
    virtual DWORD ReadVT_I2(WORD *pw);

    // Reads a 2-byte WORD value without padding to 32-bit from a stream. The bytes is swapped
    virtual DWORD   ReadVT_I2_NoPad(WORD *pw); 
    
    // Reads a 4-byte DWORD value from a stream. The bytes is swapped
    virtual DWORD ReadVT_I4(DWORD *pdw);

    // Reads a 32-bit IEEE floating point value from a stream. The bytes is swapped
    virtual DWORD ReadVT_R4(float *pflt);

    // Reads a 64-bit IEEE floating point value from a stream. The bytes is swapped
    virtual DWORD ReadVT_R8(double *pdbl);

    // Reads an 8-byte two's complement integer (scaled by 10,000) from a stream. 
    //  It's usually used for currency amounts (hence the CY type).The bytes is swapped
    virtual DWORD ReadVT_CY(CY *pcy);

    // Reads an 8-byte integer from a stream. The bytes is swapped
    virtual DWORD ReadVT_I8(LARGE_INTEGER *pli);

    // Reads a boolean from a stream. The bytes is swapped 
    virtual DWORD ReadVT_BOOL(BOOL *pf);

    // Reads a character string from a stream. The array is dynamically allocated, 
    // and the caller should delete it after using it.
    virtual DWORD ReadVT_LPSTR(char **ppsz);

    // Reads a character string without padding to 32-bit from a stream. Return total bytes read. The array is 
    // dynamically allocated, and should be deleted after use.
    virtual DWORD   ReadVT_LPSTR_NoPad(char **ppsz); 
    
    // Reads a Unicode (16-bit) character string from a stream.
    // The array is dynamically allocated, and the caller should delete it after
    // using it.
    virtual DWORD ReadVT_LPWSTR(WCHAR **ppwsz);

    // Reads a Unicode (16-bit) character string without padding to 32-bit from a stream. Return total bytes read. 
    // The array is dynamically allocated, and should be deleted after use.
    virtual DWORD   ReadVT_LPWSTR_NoPad(WCHAR **ppwsz); 
    
    // Reads a FILETIME structure from a stream. The bytes is swapped 
    virtual DWORD ReadVT_FILETIME(FILETIME *pft);

    // Reads a BLOB (length + raw block of data) from a stream. The BYTE array
    // within the BLOB structure is dynamically allocated, so it should be deleted 
    // by the caller after being used.
    virtual DWORD ReadVT_BLOB(BLOB **);

    // Reads a CLSID from a stream (which is really a DWORD, two WORDs, and 8 bytes).
    // The bytes is swapped
    virtual DWORD ReadVT_CLSID(CLSID *);
    virtual DWORD ReadVT_CLSID(CLSID **);

    // Reads a VARIANT from a stream (which is really four unsigned short, and a double).
    // The bytes is swapped
    virtual DWORD   ReadVT_VARIANT(VARIANT *pvar);

    // FIX_DICT_PROP - added ReadDICT_ENTRIES
    // Reads the entries for a dictionary from a stream (each of which is essentially 
    // similar to a PID and a BSTR).
    // The bytes is swapped
    virtual DWORD   ReadDICT_ENTRIES(DICTIONARY *pDict);

    // Reads a clipboard data structure (length + format + raw block of data)
    // from a stream. The BYTE array within the CLIPDATA structure is dynamically
    // allocated, so it should be deleted by the caller after being used.
    virtual DWORD ReadVT_CF(CLIPDATA **);

    // Reads a vector data structure (cElements * length of each element), and return total bytes 
    // been read. The vector type is like VT_XX | VT_VECTOR. Right now 11 types are supported 
    // ( char, WORD, DWORD, float, double, LPSTR, LPWSTR, BLOB, CLIPDATA, CLSID, VARIANT). 
    // The vector structure is dynamically allocated, so it should be deleted 
    // by the caller after being used.
    virtual DWORD   ReadVT_VECTOR(DWORD dwType, VECTOR **ppVector); 
    
    // This is a generic function that writes a property of any type in pData to a stream.
    virtual DWORD WriteVT(DWORD dwType, BYTE *pData);

    // This is a generic function that writes a property of any type to a stream.
    virtual DWORD WriteVT(VARIANT * variant);

    // Write a 2-byte WORD value to a stream with padded to 32-bit boundary, so the actual data 
    // write to the stream is 4 bytes. The bytes are swapped.
    virtual DWORD WriteVT_I2(WORD* pw);

    // Write a 2-byte WORD value to a stream without padded to 32-bit boundary, so the actual data 
    // write to the stream is 2 bytes. The bytes are swapped.
    virtual DWORD   WriteVT_I2_NoPad(WORD* pw);
    
    // Write a 4-byte DWORD value to a stream. The bytes are swapped.
    virtual DWORD WriteVT_I4(DWORD* pdw);

    // Write a 32-bit IEEE floating point value to a stream. The bytes are swapped
    virtual DWORD WriteVT_R4(float* pflt);

    // Write a 64-bit IEEE floating point value to a stream. The bytes are swapped
    virtual DWORD WriteVT_R8(double* pdbl);

    // Write a 8-byte two's complement integer (scaled by 10,000) to a stream.
    // It's usually used for currency amounts (hence the CY type). The bytes are swapped
    virtual DWORD WriteVT_CY(CY *pcy);

    // Write a 8-byte integer to a stream. The bytes are swapped
    virtual DWORD WriteVT_I8(LARGE_INTEGER *pli);

    // Write a boolean to a stream. The bytes are swapped. An OLE boolean is defined to 
    // be a 2-byte WORD containing 0 (FALSE) or -1 (TRUE). It is padded to a 32-bit boundary.
    virtual DWORD WriteVT_BOOL(BOOL* pf);

    // Writes a zero-terminated character string padded to 32-bit boundary to a stream.
    virtual DWORD WriteVT_LPSTR(char *psz);

    // Writes a zero-terminated character string without padded to 32-bit boundary to a stream.
    virtual DWORD   WriteVT_LPSTR_NoPad(char *psz); 
    
    // Writes a zero-terminated wide character string padded to 32-bit boundary to a stream.
    virtual DWORD WriteVT_LPWSTR(WCHAR *pwsz);

    // Writes a zero-terminated wide character string without padded to 32-bit boundary to a stream.
    virtual DWORD   WriteVT_LPWSTR_NoPad(WCHAR *pwsz); 
    
    // Writes a FILETIME structure to a stream. The bytes is swapped 
    virtual DWORD WriteVT_FILETIME(FILETIME *pft);

    // Writes a BLOB (length + raw block of data) to a stream. The bytes is swapped
    virtual DWORD WriteVT_BLOB(BLOB *pblob);

    // Writes a clipboard data structure (length + format + raw block of data)
    // to a stream.
    virtual DWORD WriteVT_CF(CLIPDATA *pclipdata);

    // Writes a CLSID to a stream (which is really a DWORD, two WORDs, and 8 bytes).
    virtual DWORD WriteVT_CLSID(CLSID *pclsid);
    
    // Writes a VARIANT into a stream (which is really four WORDs, and a double). The bytes is swapped
    virtual DWORD   WriteVT_VARIANT(VARIANT *pvar);

    // Writes a vector data structure (cElements * length of each element). The total bytes 
    // written to the disk must be padded to 32-bit and returned. The vector type is like 
    // VT_XX | VT_VECTOR. Right now 11 types are supported (char, WORD, DWORD, float, double, 
    // LPSTR, LPWSTR, BLOB, CLIPDATA, CLSID, VARIANT).
    virtual DWORD   WriteVT_VECTOR(DWORD dwType, VECTOR *pvector);

    // FIX_DICT_PROP - added WriteDICT_ENTRIES
    // Writes the entries for a dictionary to a stream (each of which is essentially 
    // similar to a PID and a BSTR).
    // The bytes is swapped
    virtual DWORD   WriteDICT_ENTRIES(DICTIONARY *pDict);

    // Returns the size of an element of the specified type
    DWORD       SizeVT(DWORD dwType);

    // Returns a string representation of a VT type data. The returned string is dynamically
    // allocated, so it should be deleted after use.
    Boolean     VTtoString(VARIANT *variant, char **ppsz);
    Boolean     VTtoString(BYTE *pData, DWORD vtType, char **ppsz);

    virtual   void  AddRef(); // Add reference to the IStream
    virtual   void  Release();  // Release the IStream
    
          LPSTREAM  GetStream() { return oleStream; } // Release the IStream
    
  protected:
    Boolean     fSwapBytes;   // Swap bytes flag, swap the bytes if true
    
  private:
    LPSTREAM    oleStream;    // The actual OLE stream
    OLEStorage *  parStorage;   // Parent storage
  };


#define ReadVT_DATE(pdbl)       ReadVT_R8(pdbl)
#ifndef _UNICODE
#define ReadVT_BSTR(pprgch)       ReadVT_LPSTR(pprgch)
#else
#define ReadVT_BSTR(pprgch)       ReadVT_LPWSTR(pprgch)
#endif
#define ReadVT_STREAM(pprgch)     ReadVT_LPSTR(pprgch)
#define ReadVT_STORAGE(pprgch)      ReadVT_LPSTR(pprgch)
#define ReadVT_STREAMED_OBJECT(pprgch)  ReadVT_STREAM(pprgch)
#define ReadVT_STORED_OBJECT(pprgch)  ReadVT_STORAGE(pprgch)
#define ReadVT_BLOB_OBJECT(pprgch)    ReadVT_STREAMED_OBJECT(pprgch)

#define WriteVT_DATE(dbl)       WriteVT_R8(dbl)
#ifndef _UNICODE
#define WriteVT_BSTR(prgch)       WriteVT_LPSTR(prgch)
#else
#define WriteVT_BSTR(prgch)       WriteVT_LPWSTR(prgch)
#endif
#define WriteVT_STREAM(prgch)     WriteVT_LPSTR(prgch)
#define WriteVT_STORAGE(prgch)      WriteVT_LPSTR(prgch)
#define WriteVT_STREAMED_OBJECT(prgch)  WriteVT_STREAM(prgch)
#define WriteVT_STORED_OBJECT(prgch)  WriteVT_STORAGE(prgch)
#define WriteVT_BLOB_OBJECT(pprgch)   WriteVT_STREAMED_OBJECT(pprgch)


//  ----------------------------------------------------------------------------
  #endif // OLEStreams_h
//  ----------------------------------------------------------------------------
