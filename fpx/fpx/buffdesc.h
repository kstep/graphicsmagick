//  ----------------------------------------------------------------------------
//  MODULE    : BufferDesc
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT
//  CREAT. DATE : Monday, March 25, 1996
//  DESCRIPTION :
//  COMMENTS  : Color Space and Pixel Buffer Handling:
//          This class will handle all possibilities of color interface
//            and interleaving between the Toolkit and the application.
//
// The idea is that in Baseline, every channels is 8 bits (1 byte) depth, so there
// is no need for very fancy buffer handling. Then, there is nothing with more
// than 4 channels, so 32 bits pixels are enough. So, the idea is to handle
// everything in a long for every color space and to take advantage of all 32 bits
// functionnalities to move pixels around. I know, that's overkill for monochrome
// but this shouldn't be a very common case. Last but not least, the buffer
// containing the data is supposed to be contiguous, so, you just have to give
// the address of a buffer and its width and height to update all the required
// pointers in the FPXImageDesc.
//
//  SCCSID      : @(#)buffdesc.h  1.3 13:22:25 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef BufferDesc_h
  #define BufferDesc_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef SystemIVUE_h
  #include  "ri_sys.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------

  // Avoid to include FPXBaselineIO.h
  struct FPXImageDesc;
  struct FPXColorspace;

//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------

//  Classes Definitions
//  -------------------

  class FPXBufferDesc : public PToolkitObject {
  
  public:
            FPXBufferDesc (unsigned char* theBuffer, long theWidth, long theHeight, FPXBaselineColorSpace colorSpace = SPACE_32_BITS_RGB);
            FPXBufferDesc (long theColor, long theWidth, long theHeight, FPXBaselineColorSpace colorSpace = SPACE_32_BITS_RGB);
            FPXBufferDesc (FPXImageDesc* desc, long theWidth, long theHeight, unsigned char *theBuffer = NULL);
           ~FPXBufferDesc ();

      Pixel*          Get32BitsBuffer();
      FPXImageDesc*       GetFPXdesc(); 
      FPXBaselineColorSpace GetBaselineColorSpace();

      void          UpdateDescriptor();
      void          UpdateBuffer();
      
  private:
  
      void          InitImageDesc(FPXBaselineColorSpace colorSpace);
  
      FPXBaselineColorSpace colorSpaceType;
      long          width;
      long          height;

      unsigned char*      buffer;
      Boolean         localBuffer;
      Boolean         useInternalBuffer;

      FPXImageDesc*     FPXdesc;
      Boolean         localDesc;
  };
  
//  'inline' Functions
//  ------------------

inline Pixel* FPXBufferDesc::Get32BitsBuffer()
{
  return (Pixel*)(buffer);
}

inline FPXImageDesc* FPXBufferDesc::GetFPXdesc()
{
  return FPXdesc;
}

inline FPXBaselineColorSpace FPXBufferDesc::GetBaselineColorSpace()
{
  return colorSpaceType;
}

//  'extern' Functions
//  ------------------

  FPXBaselineColorSpace AnalyseFPXColorSpace (FPXColorspace& colorSpace);
  void CreateFPXColorSpace (FPXBaselineColorSpace baseSpace, FPXColorspace* colorSpace);
  Boolean IsAlphaBaseline(FPXBaselineColorSpace baseSpace);
  long GetAlphaOffsetBaseline(FPXBaselineColorSpace baseSpace);
  long GetNbChannel(FPXBaselineColorSpace baseSpace);

  void ExtractFPXColorSpaceFromFPXImageDesc(FPXImageDesc& desc, FPXColorspace* colorSpace);
  Boolean IsA32bitsBufferDescriptor(FPXImageDesc& desc, long width);
  Boolean IsASupportedDescriptor(FPXImageDesc& desc, long width);

  // Check to see if all pixels in a tile are same and return the pixel value
  Boolean IsTileAllSamePixel(Pixel *entireTile, short width, short height, Pixel* singleColorPixel);

  void ConvertPixelBuffer(unsigned char* buffer, long size, FPXBaselineColorSpace source, FPXBaselineColorSpace destination);
  void ConvertRGBtoYCC(unsigned char* buffer, long size);
  void ConvertYCCtoRGB(unsigned char* buffer, long size, Boolean useAlpha);
//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // BufferDesc_h
//  ----------------------------------------------------------------------------
