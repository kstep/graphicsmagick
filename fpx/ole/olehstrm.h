//  ----------------------------------------------------------------------------
//  MODULE    : OLEHeaderStream.h
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file defines an OLE stream object with header
//  COMMENT   : Enhanced OLE objects are platform independent
//  SCCSID      : @(#)olehstrm.h  1.1 11:53:29 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

  #ifndef OLEHeaderStream_h
  #define OLEHeaderStream_h
  
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLEStreams_h
  #include  "olestrm.h"
#endif
  class OLEStorage;
  
//  Constants
//  ---------
  
//  Class declarations 
//  ------------------

//  OLE property list structures declarations
//  -----------------------------------------

  typedef struct tagPROPERTYSET
  {
    WORD      wByteOrder; 
    WORD      wFormat; 
    DWORD     dwOSVer; 
    CLSID     clsID; 
    DWORD     cSections; 
  } PROPERTYSETHEADER; 
  

  class OLEHeaderStream;
  
//  Class definitions
//  -----------------

  class OLEHeaderStream : public OLEStream {

  public:
    // This constructor creates a OLEHeaderStream from an open storage based 
    // on given class ID and stream
            OLEHeaderStream(const GUID& clsID, OLEStorage * parentStorage, LPSTREAM currentStream);
            virtual ~OLEHeaderStream();
    
    // Read the stream header of the stream 
    virtual void  ReadHeader(); 
    // DAG
    // Write the header of the stream 
    virtual Boolean WriteHeader(); 

    // Set the stream pointer to desired position. The default mode is from the begining
    virtual Boolean Seek( const long offset = 0, const unsigned long mode = STREAM_SEEK_SET);

    // Get the end pointer of the stream, excluding the header length
    virtual Boolean GetEndOfFile( long* endPosition);
    
    // Copy the stream to a new one
    virtual Boolean CopyTo( LPSTREAM pstm, unsigned long copyLen ); 
    
    // Set the new size of the stream, including the header length
    virtual Boolean SetSize( unsigned long sizeOfStream );

    // Save everything in the stream including the header
    virtual Boolean Commit(); 

    // Read everything in the stream including the header
    virtual Boolean Revert(); 

  protected:
    PROPERTYSETHEADER   propSetHdr;     // Header of the stream
    unsigned long     streamStartPos;   // Starting offset in bytes: 28 for FPX streams
                        // (default) but 0 for property sets...
  };


//  ----------------------------------------------------------------------------
  #endif // OLEHeaderStream_h
//  ----------------------------------------------------------------------------
