//  ----------------------------------------------------------------------------
//  MODULE    : FPXThumbnail.h (part of BufferDesc.cpp)
//  LANGUAGE  : C++
//  CREATOR   : Glenn Nelson
//  CREAT. DATE : 17 July 1996
//  DESCRIPTION :
//  COMMENTS  : Pixel Buffer Handling:
//              : This file may sometimes need to be included with BufferDesc.h
//  SCCSID      : @(#)thmbnail.h  1.1 11:59:26 18 Dec 1996
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

#ifndef FPXThumbnail_h
#define FPXThumbnail_h

#ifndef OLECommun_h
  #include "olecomm.h"  // needed only for definition of CLIPDATA
#endif

  // Create a thumbnail image in windows DIB format
  Boolean CreateThumbnail(unsigned char* buf, FPXBaselineColorSpace baseSpace, unsigned short
                thumbNailWidth, unsigned short thumbNailHeight, CLIPDATA * clipData);

  // Create window DIB default palette, which is grayscale ramp of 256 RGBQUAD
  void SetDefaultPalette(unsigned char *palBuffer); 
  
  // Convert RGB format to BGR (Windows DIB format) 
  void writeDIB24(unsigned char *obp, unsigned char *pic24, unsigned long w, unsigned long h);
  void writeDIB1(unsigned char *obp, unsigned char *pic24, unsigned long w, unsigned long h);

#endif    // FPXThumbnail_h

