//  -----------------------------------------------------------------------------------
//  MODULE    : OLEHeaderStream.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Yue Zhang
//  DATE    : Tuesday, Janurary 23, 1996
//  DESCRIPTION : This file contains the OLEHeaderStream object which is an OLEStream 
//          object with header
//
//  COMMENT   : OLEHeaderStream differs from a standard stream in that the certain 
//          amount of bytes from the begining of the stream is reserved for header
//          information and not counted when determining offsets into the stream nor 
//          determining the length of stream. It is a derived class from OLEStream. 
//
//  SCCSID      : @(#)olehstrm.cpp  1.1 11:53:29 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif
#ifndef OLEHeaderStream_h
  #include  "olehstrm.h"
#endif
#ifndef OLEStorages_h
  #include  "olestorg.h"
#endif
  
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Functions 'intern'
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of OLEHeaderStream
//  ----------------------------------------------------------------------------


// This constructor creates a OLEHeaderStream from an open storage based on given class ID and stream
OLEHeaderStream::OLEHeaderStream(const GUID& clsID, OLEStorage * parentStorage, IStream * currentStream) : 
  OLEStream(parentStorage, currentStream)
{
  propSetHdr.wByteOrder   = BYTE_ORDER_INDICATOR;   // Byte-order indicator
  propSetHdr.wFormat    = FORMAT_VERSION;     // Version of format
  propSetHdr.dwOSVer    = ORIGINATING_OS_VERSION; // OS version
  propSetHdr.clsID    = clsID;          // Identifier of the header stream
  propSetHdr.cSections  = 1;            // Number of property sections. Set
                            // to 1 by default (see FPX spec III.1.7)
  
  streamStartPos = DEFAULTSTREAMHEADERLENGTH;     // Position excluding header (default)
  Seek();                       // Skip the header and point to the beginning of the stream
}


OLEHeaderStream::~OLEHeaderStream()
{
}

// Save everything in the stream including the header
Boolean OLEHeaderStream::Commit( )
{

  // Call the parent stream function
  OLEStream::Commit();
  
  return true;
}

// Read everything in the stream including the header
Boolean OLEHeaderStream::Revert( )
{
  // Call the parent stream function
  OLEStream::Revert();
  
  // Read the stream header
  ReadHeader(); 
  
  return true;
}

// Read the stream header of the stream 
void OLEHeaderStream::ReadHeader( )
{
  // Position the stream pointer to the ABSOLUTE beginning of the stream
  OLEStream::Seek(0);

  // Read the header:
  
  Read(&propSetHdr.wByteOrder, sizeof(short));  // Read the byte order indicator  
  
  // Set the swap byte flag if necessary
  fSwapBytes = (BYTE_ORDER_INDICATOR != propSetHdr.wByteOrder ) ? true : false; 

#ifdef  IN_BIG_ENDIAN
  // Swap bytes if the swap byte flag is set 
  if ( fSwapBytes )
    SwapBytes((BYTE *)&propSetHdr.wByteOrder, sizeof(short));   
#endif

  Read(&propSetHdr.wFormat, sizeof(short));   // Read the format version  
  
#ifdef  IN_BIG_ENDIAN
  // Swap bytes if the swap byte flag is set 
  if ( fSwapBytes )
    SwapBytes((BYTE *)&propSetHdr.wFormat, sizeof(short));  
#endif

  ReadVT_I4(&propSetHdr.dwOSVer);       // Read the OS version  
  ReadVT_CLSID(&propSetHdr.clsID);      // Read the Identifier of the header stream
  ReadVT_I4(&propSetHdr.cSections);       // Read the number of sections
}

// Write the header of the stream 
Boolean OLEHeaderStream::WriteHeader( )
{
  WORD w; 
  Boolean status = TRUE;
  // Position the stream pointer to the absolute beginning of the stream
  OLEStream::Seek(0);

  // Write the header:
  
  // Write the byte order indicator
  w = propSetHdr.wByteOrder; 
#ifdef  IN_BIG_ENDIAN
  // Swap bytes if the swap byte flag is set 
  if ( fSwapBytes ) 
    SwapBytes((BYTE *)&w, sizeof(short)); 
#endif
  status = Write(&w, sizeof(short)); 
  
  if(status == FALSE)
    return status;
    
  // Write the format version
  w = propSetHdr.wFormat; 
#ifdef  IN_BIG_ENDIAN
  // Swap bytes if the swap byte flag is set 
  if ( fSwapBytes ) 
    SwapBytes((BYTE *)&w, sizeof(short)); 
#endif
  status = Write(&w, sizeof(short)); 
    
  status = WriteVT_I4(&propSetHdr.dwOSVer);         // Write the OS version
  status = WriteVT_CLSID(&propSetHdr.clsID);        // Write the Identifier
  status = WriteVT_I4(&propSetHdr.cSections);       // Write the number of sections
  return status;
}

// CAUTION: the Seek(), GetEndOfFile() and SetSize() functions take the stream offset into account
// and call the parent stream function.
// Set the stream pointer to desired position. The default mode is from the begining
Boolean OLEHeaderStream::Seek( long offset, const unsigned long mode )
{
  // If the mode is from the begining of the stream, include the header length
  if ( mode == STREAM_SEEK_SET )
    offset += streamStartPos;
  return OLEStream::Seek(offset,mode);
}

// Get the end pointer of the stream, excluding the header length
Boolean OLEHeaderStream::GetEndOfFile( long* endPosition )
{
  Boolean status = OLEStream::GetEndOfFile(endPosition);
  endPosition -= streamStartPos;
  return status;
}

// Copy the stream to a new one
Boolean OLEHeaderStream::CopyTo( LPSTREAM pstm, unsigned long copyLen )
{
  Boolean status = OLEStream::CopyTo(pstm, copyLen + streamStartPos);
  return status;
}

// Set the new size of the stream, including the header length
Boolean OLEHeaderStream::SetSize( unsigned long sizeOfStream )
{
  sizeOfStream += streamStartPos;
  return OLEStream::SetSize(sizeOfStream);
}

