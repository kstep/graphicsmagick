//  ----------------------------------------------------------------------------
//  MODULE    : BufferDesc
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT
//  CREAT. DATE : Monday, March 25, 1996
//  DESCRIPTION :
//  COMMENTS  :
//      SCCSID          : @(#)buffdesc.cpp  1.10 12:46:27 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

#ifndef BufferDesc_h
  #include "buffdesc.h"
#endif
  #include "thmbnail.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include <stdlib.h>
  #include <math.h>
#ifdef _WINDOWS
  #include <windows.h>
#endif

#ifndef Debug_h
  #include "debug.h"
#endif
#ifndef ColorTwist_h
  #include "coltwist.h"
#endif
#ifndef Compresseur32Vers24_h
    #include "cp32to24.h"
#endif
#ifndef OLECore_h
    #include "olecore.h"
#endif
#ifndef FlashPixUtils_h
    #include "fpxutils.h"
#endif
#ifndef SwapBytes_h
  #include "swapbyte.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

// Init alpha bytes to 255 (opaque) for each pixel starting at buffer
static void InitByteTo255 (unsigned char* buffer, long size)
{
  while (size--) {
    *buffer = 255;
    buffer += 4;
  }
}

// Init alpha bytes to 0 (transparent) for each pixel starting at buffer
static void InitByteTo0 (unsigned char* buffer, long size)
{
  while (size--) {
    *buffer = 0;
    buffer += 4;
  }
}
/*
// Rotate on the left by 8 bits on each 32 bits long and wrap the last one
static void Shift8BitsLeft (unsigned char* buffer, long size)
{
  register unsigned long  tmp;
  register unsigned long* all = (unsigned long*)(buffer);
  while (size--) {
    tmp = (*all) >> 24;
    (*all) = ((*all) << 8) | tmp;
    all++;
  }
} */
//Updated the procedure to work on both architedture --**IM-05/16/97
// Rotate on the left by 8 bits on each 32 bits long and wrap the last one
static void Shift8BitsLeft (unsigned char* buffer, long size)
{
  register unsigned long  tmp;
  register unsigned long* all = (unsigned long*)(buffer);
  while (size--) {

#ifdef  IN_LITTLE_ENDIAN
    *all = (unsigned long) SwapBytes( (int32) *all );
#endif

    tmp = (*all) >> 24;
    (*all) = ((*all) << 8) | tmp;

#ifdef  IN_LITTLE_ENDIAN
    *all = (unsigned long) SwapBytes( (int32) *all );
#endif

    all++;
  }
} 
/*
// Rotate on the right by 8 bits on each 32 bits long and wrap the last one
static void Shift8BitsRight (unsigned char* buffer, long size)
{
  register unsigned long  tmp;
  register unsigned long* all = (unsigned long*)(buffer);
  while (size--) {
    tmp = (*all) << 24;
    (*all) = ((*all) >> 8) | tmp;
    all++;
  }
}
*/

// Rotate on the right by 8 bits on each 32 bits long and wrap the last one
static void Shift8BitsRight (unsigned char* buffer, long size)
{
  register unsigned long  tmp;
  register unsigned long* all = (unsigned long*)(buffer);
  while (size--) {

#ifdef  IN_LITTLE_ENDIAN
    *all = (unsigned long) SwapBytes( (int32) *all );
#endif
    tmp = (*all) << 24;
    (*all) = ((*all) >> 8) | tmp;

#ifdef  IN_LITTLE_ENDIAN
    *all = (unsigned long) SwapBytes( (int32) *all );
#endif
    all++;
  }
}

// Copy a byte and erase its ancient address be 2 bytes on the right (don't move the rest)
static void MoveByteRightBy2(unsigned char* buffer, long size)
{
  register unsigned char* next;
  next = buffer + 2;
  while (size--) {
    *next = *buffer;
    *buffer = 0;
    buffer += 4;
    next   += 4;
  }
}

// Copy a byte and erase its ancient address be 2 bytes on the left (don't move the rest)
static void MoveByteLeftBy2(unsigned char* buffer, long size)
{
  register unsigned char* next;
  next = buffer - 2;
  while (size--) {
    *next = *buffer;
    *buffer = 0;
    buffer += 4;
    next   += 4;
  }
}


// Apply the transformations :
// ---------------------------
// CAUTION: For the moment, we make the hypothesis that T44 is equal to 1 and that a
// pixel is 4 bytes wide.

void ConvertRGBtoYCC(unsigned char* buffer, long size)
{
  PColorTwist tRGBTorgb(RGB8_to_rgb);   // 8-bit NIF RGB to normalized PhotoRGB
  PColorTwist trgbToycc(rgb_to_ycc);    // Normalized PhotoRGB to normalized PhotoYCC
  PColorTwist tyccToYCC(ycc_to_YCC8);   // Normalized PhotoYCC to 8-bit PhotoYCC

  PColorTwist tRGBtoYCC;
  tRGBtoYCC = trgbToycc * tRGBTorgb;
  tRGBtoYCC = tyccToYCC * tRGBtoYCC;

  // PTCH_302 Added the following call to apply a shaping LUT to the output of
  //  the matrix transform.
  tRGBtoYCC.ApplyRGBtoYCCLut(buffer,size);
}

void ConvertYCCtoRGB(unsigned char* buffer, long size, Boolean useAlpha)
{
  PColorTwist tYCCtoycc(YCC8_to_ycc);   // 8-bit PhotoYCC to normalized PhotoYCC
  PColorTwist tyccTorgb(ycc_to_rgb);    // Normalized PhotoYCC to normalized PhotoRGB
  PColorTwist trgbToRGB(rgb_to_RGB8);   // Normalized PhotoRGB to 8-bit NIF RGB
  
  PColorTwist tYCCtoRGB;
  tYCCtoRGB = tyccTorgb * tYCCtoycc;
  tYCCtoRGB = trgbToRGB * tYCCtoRGB;

  tYCCtoRGB.UsePortfolioLut();
  if(useAlpha)
    tYCCtoRGB.UseAlphaChannel();
  tYCCtoRGB.ApplyToBuffer(buffer,size);
}


// PTCH_302 ConvertYCCtoMonochrome() was completely rewritten
static void ConvertYCCtoMonochrome(unsigned char* buffer, long size)
{
  PColorTwist tRGBtorgb(RGB8_to_rgb); 
  PColorTwist trgbTomono(rgb_to_mono);
  PColorTwist tmonoToMONO(rgb_to_RGB8);

    ConvertYCCtoRGB(buffer, size, FALSE);

  PColorTwist tRGBtoMONO;
  tRGBtoMONO = trgbTomono  * tRGBtorgb;
  tRGBtoMONO = tmonoToMONO * tRGBtoMONO;

  tRGBtoMONO.ApplyToBuffer(buffer,size);

/* pre-PTCH_302 code
  while (size--) {
    *(buffer+2) = *buffer;
    *buffer     = 0;
    *(buffer+1) = 0;
    buffer += 4;
  } 
*/
}

// PTCH_302 ConvertRGBtoMonochrome() was completely rewritten
static void ConvertRGBtoMonochrome(unsigned char* buffer, long size)
{
  PColorTwist tRGBtorgb(RGB8_to_rgb); 
  PColorTwist trgbTomono(rgb_to_mono);
  PColorTwist tmonoToMONO(rgb_to_RGB8);

  PColorTwist tRGBtoMONO;
  tRGBtoMONO = trgbTomono  * tRGBtorgb;
  tRGBtoMONO = tmonoToMONO * tRGBtoMONO;

  tRGBtoMONO.ApplyToBuffer(buffer,size);

/* pre-PTCH_302 code
  ConvertRGBtoYCC(buffer,size);
  ConvertYCCtoMonochrome(buffer,size);
*/
}

static void ConvertMonochrometoRGB(unsigned char* buffer, long size)
{
  while (size--) {
    *buffer     = *(buffer+2);
    *(buffer+1) = *(buffer+2);
    buffer += 4;
  }
}

// PTCH_302 ConvertMonochrometoYCC() was completely rewritten
static void ConvertMonochrometoYCC(unsigned char* buffer, long size)
{
  ConvertMonochrometoRGB(buffer, size);
  ConvertRGBtoYCC(buffer, size);
}



//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------
void FPXBufferDesc::InitImageDesc(FPXBaselineColorSpace colorSpace)
{
  colorSpaceType = colorSpace;

  // Allocate an image descriptor
  FPXdesc = new FPXImageDesc;
  if (FPXdesc==NULL) {
    return;
  }
  localDesc = true;

  // Set the things common to all Baseline descriptors
  FPXdesc->components[0].myColorType.myDataType = DATA_TYPE_UNSIGNED_BYTE;
  FPXdesc->components[1].myColorType.myDataType = DATA_TYPE_UNSIGNED_BYTE;
  FPXdesc->components[2].myColorType.myDataType = DATA_TYPE_UNSIGNED_BYTE;
  FPXdesc->components[3].myColorType.myDataType = DATA_TYPE_UNSIGNED_BYTE;
    
  FPXdesc->components[0].horzSubSampFactor = 1;
  FPXdesc->components[1].horzSubSampFactor = 1;
  FPXdesc->components[2].horzSubSampFactor = 1;
  FPXdesc->components[3].horzSubSampFactor = 1;

  FPXdesc->components[0].vertSubSampFactor = 1;
  FPXdesc->components[1].vertSubSampFactor = 1;
  FPXdesc->components[2].vertSubSampFactor = 1;
  FPXdesc->components[3].vertSubSampFactor = 1;

  FPXdesc->components[0].columnStride = 4;
  FPXdesc->components[1].columnStride = 4;
  FPXdesc->components[2].columnStride = 4;
  FPXdesc->components[3].columnStride = 4;

  FPXdesc->components[0].lineStride = width * 4;
  FPXdesc->components[1].lineStride = width * 4;
  FPXdesc->components[2].lineStride = width * 4;
  FPXdesc->components[3].lineStride = width * 4;

  // Set the specific part according to the color space type
  switch (colorSpaceType) {
    case SPACE_32_BITS_RGB:
      FPXdesc->numberOfComponents = 3;
      FPXdesc->components[0].myColorType.myColor = NIFRGB_R;
      FPXdesc->components[1].myColorType.myColor = NIFRGB_G;
      FPXdesc->components[2].myColorType.myColor = NIFRGB_B;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_ARGB:
      FPXdesc->numberOfComponents = 4;
      FPXdesc->components[0].myColorType.myColor = ALPHA;
      FPXdesc->components[1].myColorType.myColor = NIFRGB_R;
      FPXdesc->components[2].myColorType.myColor = NIFRGB_G;
      FPXdesc->components[3].myColorType.myColor = NIFRGB_B;
      FPXdesc->components[0].theData = (unsigned char*)(buffer);
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[3].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_RGBA:
      FPXdesc->numberOfComponents = 4;
      FPXdesc->components[0].myColorType.myColor = NIFRGB_R;
      FPXdesc->components[1].myColorType.myColor = NIFRGB_G;
      FPXdesc->components[2].myColorType.myColor = NIFRGB_B;
      FPXdesc->components[3].myColorType.myColor = ALPHA;
      FPXdesc->components[0].theData = (unsigned char*)(buffer);
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[3].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_YCC:
      FPXdesc->numberOfComponents = 3;
      FPXdesc->components[0].myColorType.myColor = PHOTO_YCC_Y;
      FPXdesc->components[1].myColorType.myColor = PHOTO_YCC_C1;
      FPXdesc->components[2].myColorType.myColor = PHOTO_YCC_C2;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_AYCC:
      FPXdesc->numberOfComponents = 4;
      FPXdesc->components[0].myColorType.myColor = ALPHA;
      FPXdesc->components[1].myColorType.myColor = PHOTO_YCC_Y;
      FPXdesc->components[2].myColorType.myColor = PHOTO_YCC_C1;
      FPXdesc->components[3].myColorType.myColor = PHOTO_YCC_C2;
      FPXdesc->components[0].theData = (unsigned char*)(buffer);
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[3].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_YCCA:
      FPXdesc->numberOfComponents = 4;
      FPXdesc->components[0].myColorType.myColor = PHOTO_YCC_Y;
      FPXdesc->components[1].myColorType.myColor = PHOTO_YCC_C1;
      FPXdesc->components[2].myColorType.myColor = PHOTO_YCC_C2;
      FPXdesc->components[3].myColorType.myColor = ALPHA;
      FPXdesc->components[0].theData = (unsigned char*)(buffer);
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 1;
      FPXdesc->components[2].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[3].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_M:
      FPXdesc->numberOfComponents = 1;
      FPXdesc->components[0].myColorType.myColor = MONOCHROME;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_AM:
      FPXdesc->numberOfComponents = 2;
      FPXdesc->components[0].myColorType.myColor = ALPHA;
      FPXdesc->components[1].myColorType.myColor = MONOCHROME;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_MA:
      FPXdesc->numberOfComponents = 2;
      FPXdesc->components[0].myColorType.myColor = MONOCHROME;
      FPXdesc->components[1].myColorType.myColor = ALPHA;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 2;
      FPXdesc->components[1].theData = (unsigned char*)(buffer) + 3;
      break;
    case SPACE_32_BITS_O:
      FPXdesc->numberOfComponents = 1;
      FPXdesc->components[0].myColorType.myColor = ALPHA;
      FPXdesc->components[0].theData = (unsigned char*)(buffer) + 3;
      break;
    default:    // Not authorized in Baseline
      assert(false);
      break;
  }
}


FPXBufferDesc::FPXBufferDesc (unsigned char* theBuffer, long theWidth, long theHeight, FPXBaselineColorSpace colorSpace)
{
  // Set the private datas
  width  = theWidth;
  height = theHeight;

  buffer = theBuffer;
  localBuffer = false;
  useInternalBuffer = true;                   // PTCH_102

  // Init the color space description
  InitImageDesc(colorSpace);
}

FPXBufferDesc::FPXBufferDesc (long theColor, long theWidth, long theHeight, FPXBaselineColorSpace colorSpace)
{
  // Set the private datas
  width  = theWidth;
  height = theHeight;

  localBuffer = true;
  buffer = new unsigned char [width * height * 4];
  if (buffer==NULL) {
    return;
  }
  useInternalBuffer = false;                    // PTCH_102

  // Init the color space description
  InitImageDesc(colorSpace);
  
  // Fill the buffer with the background color: copy 32 bits at once
  long* pt = (long*)(buffer);
  for (long i = 0; i < height; ++i)
    for (long j = 0; j < width; ++j, ++pt)
      *pt = theColor;
}

FPXBufferDesc::FPXBufferDesc (FPXImageDesc* desc, long theWidth, long theHeight, 
                unsigned char *internalBuffer)
{
  width     = theWidth;
  height    = theHeight;
  FPXdesc   = desc;
  localDesc = false;

  FPXColorspace colorSpace;
  ExtractFPXColorSpaceFromFPXImageDesc(*desc, &colorSpace);
  colorSpaceType = AnalyseFPXColorSpace(colorSpace);
  useInternalBuffer = false;

  if (IsASupportedDescriptor(*desc,width)) {
    localBuffer = false;
    buffer = FPXdesc->components[0].theData - (4 - FPXdesc->numberOfComponents);
  }
  else if (internalBuffer != NULL) {
    localBuffer = true;
    useInternalBuffer = true;
    buffer = internalBuffer;
  }
  else {
    localBuffer = true;
    buffer = new unsigned char [width * height * 4];
    if (buffer==NULL) {
      return;
    }
  }
}

FPXBufferDesc::~FPXBufferDesc ()
{
  if (localDesc)
    delete FPXdesc;
    
  if (localBuffer && !useInternalBuffer)
  {
    delete [] buffer;
    buffer = NULL;
  }
}

void FPXBufferDesc::UpdateBuffer ()
{
  // If the pixels are stored in a local buffer, they have to be transfered from the
  // image descriptor to the buffer
  if (localBuffer) {
    register long i, j;
    
    register long incLine0 = FPXdesc->components[0].lineStride;
    register long incLine1 = FPXdesc->components[1].lineStride;
    register long incLine2 = FPXdesc->components[2].lineStride;
    register long incLine3 = FPXdesc->components[3].lineStride;
    register long incCol0  = FPXdesc->components[0].columnStride;
    register long incCol1  = FPXdesc->components[1].columnStride;
    register long incCol2  = FPXdesc->components[2].columnStride;
    register long incCol3  = FPXdesc->components[3].columnStride;
    
    unsigned char* pt0;
    unsigned char* pt1;
    unsigned char* pt2;
    unsigned char* pt3;

    unsigned char* pix;
    long  nbChan = FPXdesc->numberOfComponents;
    register long  incCol;
    
    if (nbChan == 1) {
      pix    = buffer + 3;
      incCol = 4;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        for (i = 0; i < width; i++, pix += incCol) {
          *pix  = *pt0;
           pt0 +=  incCol0;
        }
      }
    } else if (nbChan == 2) {
      pix    = buffer + 2;
      incCol = 3;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        for (i = 0; i < width; i++, pix += incCol) {
          *pix++ = *pt0;    *pix  = *pt1;
           pt0  +=  incCol0; pt1 +=  incCol1;
        }
      }
    } else if (nbChan == 3) {
      pix    = buffer + 1;
      incCol = 2;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        pt2 = (unsigned char*)(FPXdesc->components[2].theData) + j*incLine2;
        for (i = 0; i < width; i++, pix += incCol) {
          *pix++ = *pt0;    *pix++ = *pt1;    *pix  = *pt2;
           pt0  +=  incCol0; pt1  +=  incCol1; pt2 +=  incCol2;
        }
      }
    } else if (nbChan == 4) {
      pix    = buffer;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        pt2 = (unsigned char*)(FPXdesc->components[2].theData) + j*incLine2;
        pt3 = (unsigned char*)(FPXdesc->components[3].theData) + j*incLine3;
        for (i = 0; i < width; i++) {
          *pix++ = *pt0;    *pix++ = *pt1;    *pix++ = *pt2;    *pix++ = *pt3;
           pt0  +=  incCol0; pt1  +=  incCol1; pt2  +=  incCol2; pt3  +=  incCol3;
        }
      }
    }

  }
}

void FPXBufferDesc::UpdateDescriptor ()
{
  // If the pixels are stored in a local buffer, they have to be transfered to the
  // image descriptor
  if (localBuffer) {
    register long i, j;
    
    register long incLine0 = FPXdesc->components[0].lineStride;
    register long incLine1 = FPXdesc->components[1].lineStride;
    register long incLine2 = FPXdesc->components[2].lineStride;
    register long incLine3 = FPXdesc->components[3].lineStride;
    register long incCol0  = FPXdesc->components[0].columnStride;
    register long incCol1  = FPXdesc->components[1].columnStride;
    register long incCol2  = FPXdesc->components[2].columnStride;
    register long incCol3  = FPXdesc->components[3].columnStride;
    
    unsigned char* pt0;
    unsigned char* pt1;
    unsigned char* pt2;
    unsigned char* pt3;

    unsigned char* pix;
    long  nbChan = FPXdesc->numberOfComponents;
    register long  incCol;
    
    if (nbChan == 1) {
      pix    = buffer + 3;
      incCol = 4;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        for (i = 0; i < width; i++, pix += incCol) {
          *pt0  = *pix;
           pt0 += incCol0;
        }
      }
    } else if (nbChan == 2) {
      pix    = buffer + 2;
      incCol = 3;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        for (i = 0; i < width; i++, pix += incCol) {
          *pt0  = *pix++;  *pt1  = *pix;
           pt0 +=  incCol0; pt1 +=  incCol1;
        }
      }
    } else if (nbChan == 3) {
      pix    = buffer + 1;
      incCol = 2;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        pt2 = (unsigned char*)(FPXdesc->components[2].theData) + j*incLine2;
        for (i = 0; i < width; i++, pix += incCol) {
          *pt0  = *pix++;  *pt1  = *pix++;  *pt2  = *pix;
           pt0 +=  incCol0; pt1 +=  incCol1; pt2 +=  incCol2;
        }
      }
    } else if (nbChan == 4) {
      pix    = buffer;
      for (j = 0; j < height; j++) {
        pt0 = (unsigned char*)(FPXdesc->components[0].theData) + j*incLine0;
        pt1 = (unsigned char*)(FPXdesc->components[1].theData) + j*incLine1;
        pt2 = (unsigned char*)(FPXdesc->components[2].theData) + j*incLine2;
        pt3 = (unsigned char*)(FPXdesc->components[3].theData) + j*incLine3;
        for (i = 0; i < width; i++) {
          *pt0  = *pix++;  *pt1  = *pix++;  *pt2  = *pix++;  *pt3  = *pix++;
           pt0 +=  incCol0; pt1 +=  incCol1; pt2 +=  incCol2; pt3 +=  incCol3;
        }
      }
    }
  }
}


//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------

// Tells if the space contains an alpha channel
Boolean IsAlphaBaseline(FPXBaselineColorSpace baseSpace)
{
  Boolean isAlpha = true;
  if ((baseSpace == SPACE_32_BITS_RGB) ||
    (baseSpace == SPACE_32_BITS_YCC) ||
    (baseSpace == SPACE_32_BITS_M))
    isAlpha = false;
  return isAlpha;
}

// Get the position of the alpha channel in a 32 bits pixel according to the color space used
long GetAlphaOffsetBaseline(FPXBaselineColorSpace baseSpace)
{
  long offset;
  switch (baseSpace) {
    case SPACE_32_BITS_ARGB:
    case SPACE_32_BITS_AYCC:
    case SPACE_32_BITS_RGB:
    case SPACE_32_BITS_YCC:
    case SPACE_32_BITS_M:
      offset = 0;
      break;
    case SPACE_32_BITS_AM:
      offset = 2;
      break;
    case SPACE_32_BITS_O:
    case SPACE_32_BITS_RGBA:
    case SPACE_32_BITS_YCCA:
    case SPACE_32_BITS_MA:
      offset = 3;
      break;
    default:        // This shouldn't happen
      assert(false);    // Makes a beep for debug purposes
      offset = 0;
      break;
  }
  return offset;
}

// Give the number of channels of a baseline color space description
long GetNbChannel(FPXBaselineColorSpace baseSpace)
{
  long channelNumber;
  switch (baseSpace) {
    case SPACE_32_BITS_ARGB:
    case SPACE_32_BITS_RGBA:
    case SPACE_32_BITS_AYCC:
    case SPACE_32_BITS_YCCA:
      channelNumber = 4;
      break;
    case SPACE_32_BITS_RGB:
    case SPACE_32_BITS_YCC:
      channelNumber = 3;
      break;
    case SPACE_32_BITS_AM:
    case SPACE_32_BITS_MA:
      channelNumber = 2;
      break;
    case SPACE_32_BITS_M:
    case SPACE_32_BITS_O:
      channelNumber = 1;
      break;
    default:  // We go for the worst case to do our computation, but this shouldn't happen
      assert(false);    // Makes a beep for debug purposes
      channelNumber = 4;
      break;
  }
  return channelNumber;
}

// This function update a FPXColorspace structure according to the kind of
// baseline space chosen
void CreateFPXColorSpace (FPXBaselineColorSpace baseSpace, FPXColorspace* colorSpace)
{
  switch (baseSpace) {
    case SPACE_32_BITS_RGB: {
      colorSpace->numberOfComponents = 3;
      colorSpace->theComponents[0].myColor = NIFRGB_R;
      colorSpace->theComponents[1].myColor = NIFRGB_G;
      colorSpace->theComponents[2].myColor = NIFRGB_B;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_ARGB: {
      colorSpace->numberOfComponents = 4;
      colorSpace->theComponents[0].myColor = ALPHA;
      colorSpace->theComponents[1].myColor = NIFRGB_R;
      colorSpace->theComponents[2].myColor = NIFRGB_G;
      colorSpace->theComponents[3].myColor = NIFRGB_B;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[3].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_RGBA: {
      colorSpace->numberOfComponents = 4;
      colorSpace->theComponents[0].myColor = NIFRGB_R;
      colorSpace->theComponents[1].myColor = NIFRGB_G;
      colorSpace->theComponents[2].myColor = NIFRGB_B;
      colorSpace->theComponents[3].myColor = ALPHA;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[3].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_YCC: {
      colorSpace->numberOfComponents = 3;
      colorSpace->theComponents[0].myColor = PHOTO_YCC_Y;
      colorSpace->theComponents[1].myColor = PHOTO_YCC_C1;
      colorSpace->theComponents[2].myColor = PHOTO_YCC_C2;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_AYCC: {
      colorSpace->numberOfComponents = 4;
      colorSpace->theComponents[0].myColor = ALPHA;
      colorSpace->theComponents[1].myColor = PHOTO_YCC_Y;
      colorSpace->theComponents[2].myColor = PHOTO_YCC_C1;
      colorSpace->theComponents[3].myColor = PHOTO_YCC_C2;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[3].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_YCCA: {
      colorSpace->numberOfComponents = 4;
      colorSpace->theComponents[0].myColor = PHOTO_YCC_Y;
      colorSpace->theComponents[1].myColor = PHOTO_YCC_C1;
      colorSpace->theComponents[2].myColor = PHOTO_YCC_C2;
      colorSpace->theComponents[3].myColor = ALPHA;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[2].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[3].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_M: {
      colorSpace->numberOfComponents = 1;
      colorSpace->theComponents[0].myColor = MONOCHROME;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_AM: {
      colorSpace->numberOfComponents = 2;
      colorSpace->theComponents[0].myColor = ALPHA;
      colorSpace->theComponents[1].myColor = MONOCHROME;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_MA: {
      colorSpace->numberOfComponents = 2;
      colorSpace->theComponents[0].myColor = MONOCHROME;
      colorSpace->theComponents[1].myColor = ALPHA;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      colorSpace->theComponents[1].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case SPACE_32_BITS_O: {
      colorSpace->numberOfComponents = 1;
      colorSpace->theComponents[0].myColor = ALPHA;
      colorSpace->theComponents[0].myDataType = DATA_TYPE_UNSIGNED_BYTE;
      break;
      }
    case NON_AUTHORIZED_SPACE:
      break;
  }
}

// This function analyses a FPXColorSpace given by a user of the Toolkit and checks
// if it's consistent and handled by the Baseline implementation.
// It returns one of the 10 authorized model or NON_AUTHORIZED_SPACE if nothing fits.
FPXBaselineColorSpace AnalyseFPXColorSpace (FPXColorspace& colorSpace)
{
  FPXBaselineColorSpace spaceFound = NON_AUTHORIZED_SPACE;
  
  // The data type must be DATA_TYPE_UNSIGNED_BYTE
  for (long i = 0; i < colorSpace.numberOfComponents; i++) {
    if (colorSpace.theComponents[i].myDataType != DATA_TYPE_UNSIGNED_BYTE) {
      assert(false);
      return spaceFound;
    }
  }
  
  // Switch according to the number of components
  if (colorSpace.numberOfComponents == 1) {
    if    (colorSpace.theComponents[0].myColor == MONOCHROME)
      spaceFound = SPACE_32_BITS_M;
    else if (colorSpace.theComponents[0].myColor == ALPHA)
      spaceFound = SPACE_32_BITS_O;
  } else if (colorSpace.numberOfComponents == 2) {
    if    ((colorSpace.theComponents[0].myColor == ALPHA) &&
           (colorSpace.theComponents[1].myColor == MONOCHROME))
      spaceFound = SPACE_32_BITS_AM;
    else if ((colorSpace.theComponents[0].myColor == MONOCHROME) &&
         (colorSpace.theComponents[1].myColor == ALPHA))
      spaceFound = SPACE_32_BITS_MA;
  } else if (colorSpace.numberOfComponents == 3) {
    if    ((colorSpace.theComponents[0].myColor == NIFRGB_R) &&
         (colorSpace.theComponents[1].myColor == NIFRGB_G) &&
         (colorSpace.theComponents[2].myColor == NIFRGB_B))
      spaceFound = SPACE_32_BITS_RGB;
    else if ((colorSpace.theComponents[0].myColor == PHOTO_YCC_Y) &&
         (colorSpace.theComponents[1].myColor == PHOTO_YCC_C1) &&
         (colorSpace.theComponents[2].myColor == PHOTO_YCC_C2))
      spaceFound = SPACE_32_BITS_YCC;
  } else if (colorSpace.numberOfComponents == 4) {
    if    ((colorSpace.theComponents[0].myColor == ALPHA) &&
         (colorSpace.theComponents[1].myColor == NIFRGB_R) &&
         (colorSpace.theComponents[2].myColor == NIFRGB_G) &&
         (colorSpace.theComponents[3].myColor == NIFRGB_B))
      spaceFound = SPACE_32_BITS_ARGB;
    else if ((colorSpace.theComponents[0].myColor == NIFRGB_R) &&
         (colorSpace.theComponents[1].myColor == NIFRGB_G) &&
         (colorSpace.theComponents[2].myColor == NIFRGB_B) &&
         (colorSpace.theComponents[3].myColor == ALPHA))
      spaceFound = SPACE_32_BITS_RGBA;
    else if ((colorSpace.theComponents[0].myColor == ALPHA) &&
         (colorSpace.theComponents[1].myColor == PHOTO_YCC_Y) &&
         (colorSpace.theComponents[2].myColor == PHOTO_YCC_C1) &&
         (colorSpace.theComponents[3].myColor == PHOTO_YCC_C2))
      spaceFound = SPACE_32_BITS_AYCC;
    else if ((colorSpace.theComponents[0].myColor == PHOTO_YCC_Y) &&
         (colorSpace.theComponents[1].myColor == PHOTO_YCC_C1) &&
         (colorSpace.theComponents[2].myColor == PHOTO_YCC_C2) &&
         (colorSpace.theComponents[3].myColor == ALPHA))
      spaceFound = SPACE_32_BITS_YCCA;
  }

  assert(spaceFound != NON_AUTHORIZED_SPACE);
  return spaceFound;
}

void ExtractFPXColorSpaceFromFPXImageDesc(FPXImageDesc& desc, FPXColorspace* colorSpace)
{
  colorSpace->numberOfComponents = (short) desc.numberOfComponents;
  for (long i = 0; i < (long) desc.numberOfComponents; i++) {
    colorSpace->theComponents[i] = desc.components[i].myColorType;
  }
}

Boolean IsA32bitsBufferDescriptor(FPXImageDesc& desc, long width)
{
  Boolean is32 = true;
  long i;
  
  // Special case if only one channel
  if (desc.numberOfComponents == 1) {
    if (desc.components[0].columnStride != 4)
      is32 = false;
  }
  
  // Check the horizontal subsampling factors
  if (is32) {
    for (i = 0; i < (long) desc.numberOfComponents; i++)
      is32 &= (desc.components[i].horzSubSampFactor == 1);
  }

  // Check the vertical subsampling factors
  if (is32) {
    for (i = 0; i <(long) desc.numberOfComponents; i++)
      is32 &= (desc.components[i].vertSubSampFactor == 1);
  }

  // Check the column increment
  if (is32) {
    for (i = 0; i < (long) desc.numberOfComponents; i++)
      is32 &= (desc.components[i].columnStride == 4);
  }

  // Check the line increment
  if (is32) {
    long lineStride = width * 4;
    for (i = 0; i < (long)desc.numberOfComponents; i++)
      is32 &= (desc.components[i].lineStride == lineStride);
  }

  // Check the buffer differences (must be one byte)
  if (is32) {
    for (i = 0; i < (long)(desc.numberOfComponents-1); i++)
      is32 &= ((desc.components[i+1].theData - desc.components[i].theData) == 1);
  }
  
  return is32;
}

Boolean IsASupportedDescriptor(FPXImageDesc& desc, long width)
{
  Boolean isSupported = true;
  
  FPXColorspace colorSpace;
  ExtractFPXColorSpaceFromFPXImageDesc(desc, &colorSpace);
  
  FPXBaselineColorSpace baselineSpace = AnalyseFPXColorSpace(colorSpace);
  if (baselineSpace == NON_AUTHORIZED_SPACE)
    isSupported = false;
  
  if (isSupported) {
    isSupported = IsA32bitsBufferDescriptor(desc,width);
  }
  
  return isSupported;
}

// Check to see if all pixels in a tile are same and return the pixel value
Boolean IsTileAllSamePixel(Pixel *entireTile, short width, short height, Pixel* singleColorPixel)
{
  register unsigned long numofpixel = width * height;
  
  *singleColorPixel = entireTile[0];
  for ( unsigned long i = 1; i< numofpixel; i++ )
    if ( singleColorPixel->rouge != entireTile[i].rouge ||
        singleColorPixel->vert != entireTile[i].vert ||
        singleColorPixel->bleu != entireTile[i].bleu )
      return false;
  
  return true;
}

// Handle all the color conversion / 32 bits interleaving options available in Baseline.
// Basically, it's just moving bytes and converting 24bits per pixel colors... it's long so...
// We tried to minimize the quantities of computation for each individual case. Could have been done
// differently (and less efficiently) by going systematically to some kind of space and converting to all
// others... Or using a twist matrix (to much computation so...).
void ConvertPixelBuffer(unsigned char* buffer, long size, FPXBaselineColorSpace source, FPXBaselineColorSpace destination)
{
  switch (source) {
    case SPACE_32_BITS_RGB:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          // Nothing to do
          break;
        case SPACE_32_BITS_ARGB:
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_RGBA:
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_YCC:
          ConvertRGBtoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_AYCC:
          ConvertRGBtoYCC(buffer+1,size);
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_YCCA:
          ConvertRGBtoYCC(buffer+1,size);
          InitByteTo255(buffer, size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_M:
          ConvertRGBtoMonochrome(buffer+1,size);
          break;
        case SPACE_32_BITS_AM:
          ConvertRGBtoMonochrome(buffer+1,size);
          InitByteTo255(buffer+2,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertRGBtoMonochrome(buffer+1,size);
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_O:
          ConvertRGBtoMonochrome(buffer+1,size);
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_ARGB:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_ARGB:
          // Nothing to do
          break;
        case SPACE_32_BITS_RGBA:
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_YCC:
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          ConvertRGBtoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_AYCC:
          ConvertRGBtoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_YCCA:
          ConvertRGBtoYCC(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          //**IM--05/16/97--Maybe it's better to have:
          //Shift8BitsLeft(buffer,size);
          //ConvertRGBtoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_M:
          ConvertRGBtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          ConvertRGBtoMonochrome(buffer+1,size);
          MoveByteRightBy2(buffer,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertRGBtoMonochrome(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_O:
          ConvertRGBtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_RGBA:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          Shift8BitsRight(buffer,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_ARGB:
          Shift8BitsRight(buffer,size);
          break;
        case SPACE_32_BITS_RGBA:
          // Nothing to do
          break;
        case SPACE_32_BITS_YCC:
          Shift8BitsRight(buffer,size);
          ConvertRGBtoYCC(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AYCC:
          Shift8BitsRight(buffer,size);
          ConvertRGBtoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_YCCA:
          ConvertRGBtoYCC(buffer,size);
          break;
        case SPACE_32_BITS_M:
          Shift8BitsRight(buffer,size);
          ConvertRGBtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          ConvertRGBtoMonochrome(buffer,size);
          Shift8BitsRight(buffer,size);
          MoveByteRightBy2(buffer,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertRGBtoMonochrome(buffer,size);
          break;
        case SPACE_32_BITS_O:
          Shift8BitsRight(buffer,size);
          ConvertRGBtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_YCC:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          ConvertYCCtoRGB(buffer+1,size,FALSE);
          break;
        case SPACE_32_BITS_ARGB:
          ConvertYCCtoRGB(buffer+1,size,FALSE);
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_RGBA:
          //ConvertYCCtoRGB(buffer+1,size,TRUE);
          //InitByteTo255(buffer,size);
          //Shift8BitsLeft(buffer,size);
          //**IM--05/16/97--If Opacity is used never use buffer + 1,..., TRUE
          Shift8BitsLeft(buffer,size); //get YCCA
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA using the right Opacity
          InitByteTo255(buffer+3,size);//set Opacity 255;
          break;
        case SPACE_32_BITS_YCC:
          // Nothing to do
          break;
        case SPACE_32_BITS_AYCC:
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_YCCA:
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_M:
          ConvertYCCtoMonochrome(buffer+1,size);
          break;
        case SPACE_32_BITS_AM:
          ConvertYCCtoMonochrome(buffer+1,size);
          InitByteTo255(buffer+2,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertYCCtoMonochrome(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          InitByteTo255(buffer+3,size);
          break;
        case SPACE_32_BITS_O:
          ConvertYCCtoMonochrome(buffer+1,size);
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_AYCC:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          Shift8BitsLeft(buffer,size); //get YCCA--**IM--05/16/97
          //ConvertYCCtoRGB(buffer+1,size,TRUE);
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA
          Shift8BitsRight(buffer,size);//get ARGB
          break;
        case SPACE_32_BITS_ARGB:
          //ConvertYCCtoRGB(buffer+1,size,TRUE);
          Shift8BitsLeft(buffer,size); //get YCCA--**IM--05/16/97
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA
          Shift8BitsRight(buffer,size);//get ARGB
          break;
        case SPACE_32_BITS_RGBA:
          //ConvertYCCtoRGB(buffer+1,size,TRUE);
          //Shift8BitsLeft(buffer,size);
          Shift8BitsLeft(buffer,size);//get YCCA --**IM--05/16/97
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA
          break;
        case SPACE_32_BITS_YCC:
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AYCC:
          // Nothing to do
          break;
        case SPACE_32_BITS_YCCA:
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_M:
          ConvertYCCtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          ConvertYCCtoMonochrome(buffer+1,size);
          MoveByteRightBy2(buffer,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertYCCtoMonochrome(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_O:
          ConvertYCCtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_YCCA:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          //Shift8BitsRight(buffer,size);
          //ConvertYCCtoRGB(buffer+1,size);
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA
          Shift8BitsRight(buffer,size); //get ARGB
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_ARGB:
        //  Shift8BitsRight(buffer,size);
        //  ConvertYCCtoRGB(buffer+1,size,TRUE);
          ConvertYCCtoRGB(buffer,size,TRUE);//get RGBA --**IM--05/02/97--fixed bug 17998
          Shift8BitsRight(buffer,size);//get ARGB
          break;
        case SPACE_32_BITS_RGBA:
          ConvertYCCtoRGB(buffer,size,TRUE);
          break;
        case SPACE_32_BITS_YCC:
          Shift8BitsRight(buffer,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AYCC:
          Shift8BitsRight(buffer,size);
          break;
        case SPACE_32_BITS_YCCA:
          // Nothing to do
          break;
        case SPACE_32_BITS_M:
          Shift8BitsRight(buffer,size);
          ConvertYCCtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          ConvertYCCtoMonochrome(buffer,size);
          Shift8BitsRight(buffer,size);
          MoveByteRightBy2(buffer,size);
          break;
        case SPACE_32_BITS_MA:
          ConvertYCCtoMonochrome(buffer,size);
          break;
        case SPACE_32_BITS_O:
          Shift8BitsRight(buffer,size);
          ConvertYCCtoMonochrome(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_M:
    case SPACE_32_BITS_O:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          ConvertMonochrometoRGB(buffer+1,size);
          break;
        case SPACE_32_BITS_ARGB:
          ConvertMonochrometoRGB(buffer+1,size);
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_RGBA:
          ConvertMonochrometoRGB(buffer+1,size);
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_YCC:
          ConvertMonochrometoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_AYCC:
          ConvertMonochrometoYCC(buffer+1,size);
          InitByteTo255(buffer,size);
          break;
        case SPACE_32_BITS_YCCA:
          ConvertMonochrometoYCC(buffer+1,size);
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_M:
          // Nothing to do
          break;
        case SPACE_32_BITS_AM:
          InitByteTo255(buffer+2,size);
          break;
        case SPACE_32_BITS_MA:
          InitByteTo255(buffer,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_O:
          // Nothing to do
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_AM:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoRGB(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_ARGB:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoRGB(buffer+1,size);
          break;
        case SPACE_32_BITS_RGBA:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoRGB(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_YCC:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoYCC(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AYCC:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_YCCA:
          MoveByteLeftBy2(buffer+2,size);
          ConvertMonochrometoYCC(buffer+1,size);
          Shift8BitsLeft(buffer,size);
          break;
        case SPACE_32_BITS_M:
          // InitByteTo0(buffer+2,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          // Nothing to do
          break;
        case SPACE_32_BITS_MA:
          Shift8BitsLeft(buffer,size);
          MoveByteRightBy2(buffer+1,size);
          break;
        case SPACE_32_BITS_O:
          // InitByteTo0(buffer+2,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
    case SPACE_32_BITS_MA:
      switch (destination) {
        case SPACE_32_BITS_RGB:
          Shift8BitsRight(buffer,size);
          ConvertMonochrometoRGB(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_ARGB:
          Shift8BitsRight(buffer,size);
          ConvertMonochrometoRGB(buffer+1,size);
          break;
        case SPACE_32_BITS_RGBA:
          ConvertMonochrometoRGB(buffer,size);
          break;
        case SPACE_32_BITS_YCC:
          Shift8BitsRight(buffer,size);
          ConvertMonochrometoYCC(buffer+1,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AYCC:
          Shift8BitsRight(buffer,size);
          ConvertMonochrometoYCC(buffer+1,size);
          break;
        case SPACE_32_BITS_YCCA:
          ConvertMonochrometoYCC(buffer,size);
          break;
        case SPACE_32_BITS_M:
          Shift8BitsRight(buffer,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case SPACE_32_BITS_AM:
          Shift8BitsRight(buffer,size);
          MoveByteRightBy2(buffer,size);
          break;
        case SPACE_32_BITS_MA:
          // Nothing to do
          break;
        case SPACE_32_BITS_O:
          Shift8BitsRight(buffer,size);
          // InitByteTo0(buffer,size); now we prefer to preserve the alpha channel if any
          break;
        case NON_AUTHORIZED_SPACE:
          break;
      }
      break;
  default:
    {
    }
  }
}

// Create a thumbnail image in windows DIB format
Boolean CreateThumbnail(unsigned char* buf, FPXBaselineColorSpace baseSpace, 
            unsigned short thumbNailWidth, unsigned short thumbNailHeight, 
            CLIPDATA * clipData)
{
  Ptr buffer;
  long  thumbnailSize;
  ptr_Compresseur monCompresseur;
  BITMAPINFOHEADER  bmpInfoHdr;

  // Get the TLC_32Vers24 compressor
  if ( !(monCompresseur = (*tousLesCodecs)[TLC_32Vers24]) )
    return FALSE;

  // Thumbnails cannot be stored in YCC. Convert to RGB if necessary
  if ( (baseSpace == SPACE_32_BITS_YCC) || (baseSpace == SPACE_32_BITS_YCCA) ) {
    ConvertPixelBuffer( (unsigned char*)(buf), thumbNailWidth * thumbNailHeight, 
            baseSpace, SPACE_32_BITS_RGB);
    baseSpace = SPACE_32_BITS_RGB;
  }
    
  // In the case of simple compression, we have to set the number of channels and the direction
  // of the compression. This is optimized only for the most common cases.
  Boolean leftShift = FALSE;
  if ( (baseSpace == SPACE_32_BITS_RGBA) || (baseSpace == SPACE_32_BITS_YCCA) ) 
    leftShift = TRUE;
  
  // For thumbnail, 4 channels is compressed to 3 channels image, and 2 channels is compressed to 1 channel image
  short nbChannels = (short) GetNbChannel(baseSpace);
  switch(nbChannels)
  {
    case 4: 
    case 3: 
    {
      nbChannels = 3;
      ((obj_Compresseur32Vers24 *)monCompresseur)->SetCompressionParameters(nbChannels,leftShift);

      if ( !((obj_Compresseur32Vers24 *)monCompresseur)->Compresse((Ptr)buf, thumbNailWidth, thumbNailHeight,
          &buffer, &thumbnailSize))
        return FALSE;
      break;
    }
    case 2: 
    {
      // For SPACE_32_BITS_MA, first convert to SPACE_32_BITS_AM image, then compressed to 1 channel image 
      // For SPACE_32_BITS_AM, it can be considered as 1 channel image
      if ( baseSpace == SPACE_32_BITS_MA )
        ConvertPixelBuffer((unsigned char*)(buf),thumbNailWidth*thumbNailHeight,SPACE_32_BITS_MA,SPACE_32_BITS_AM);

      nbChannels = 1;
      ((obj_Compresseur32Vers24 *)monCompresseur)->SetCompressionParameters(nbChannels, leftShift);
      if ( !((obj_Compresseur32Vers24 *)monCompresseur)->Compresse((Ptr)buf, thumbNailWidth, thumbNailHeight,
          &buffer, &thumbnailSize))
        return FALSE;
        
      break; 
    }
    case 1: 
    {
      ((obj_Compresseur32Vers24 *)monCompresseur)->SetCompressionParameters(nbChannels,leftShift);

      if ( !((obj_Compresseur32Vers24 *)monCompresseur)->Compresse((Ptr)buf, thumbNailWidth, thumbNailHeight,
          &buffer, &thumbnailSize))
        return FALSE;
      break; 
    }
    default: 
      return FALSE;
  }
  
  // PTCH_203 Recalculate "thumbnailSize" and account for required DIB line padding ...
  if (nbChannels >= 3) {
    int padBytes = (4 - ((thumbNailWidth * 3) % 4)) & 0x03; 
    thumbnailSize = (thumbNailWidth * 3 + padBytes) * thumbNailHeight;
  }
  else {
      int padWidth = ((thumbNailWidth + 3) /4) * 4;
    thumbnailSize = padWidth * thumbNailHeight;
  }
  // PTCH_203 ...end of change
  
  // Alloc for clipdata. For single channel image, additional space for palette is also included
  if ( nbChannels == 1 ) 
    clipData->cbSize = thumbnailSize + sizeof(BITMAPINFOHEADER) + 2 * sizeof(long) + DefaultPaletteSize * sizeof(RGBQUAD);
  else
    clipData->cbSize = thumbnailSize + sizeof(BITMAPINFOHEADER) + 2 * sizeof(long);
    
  clipData->pClipData = new unsigned char[clipData->cbSize];      // PTCH_103 mod:  + 300];
  
  if ( !clipData->pClipData )
    return FALSE;
    
  // Write thumbnail header
  unsigned long ignore1     = 0xFFFFFFFF;         // Write ignore1 of thumbnail header
  unsigned long ignore2     = 0x00000008;         // Write ignore2 of thumbnail header

  // Write bitmap info header
  bmpInfoHdr.biSize     = sizeof(BITMAPINFOHEADER); // Write size of bitmap info header
  bmpInfoHdr.biWidth      = thumbNailWidth;       // Write bitmap width
  bmpInfoHdr.biHeight     = thumbNailHeight;      // Write bitmap width
  bmpInfoHdr.biPlanes     = 1;            // Write bitmap planes
  /**
  if ( nbChannels == 1 ) 
    bmpInfoHdr.biBitCount   = 0x3d08;           // Write bitmap bit count for single channel
  else
    bmpInfoHdr.biBitCount   = 0x3d24;           // Write bitmap bit count for multi channels
  **/
  if ( nbChannels == 1 ) 
    bmpInfoHdr.biBitCount   = 8;          // Write bitmap bit count for single channel

  else
    bmpInfoHdr.biBitCount   = 24;           // Write bitmap bit count for multi channels  

  bmpInfoHdr.biCompression  = BI_RGB;           // Write bitmap compression
  bmpInfoHdr.biSizeImage    = thumbnailSize;      // Write bitmap size
  bmpInfoHdr.biXPelsPerMeter  = 75 * 39;          // Write X dpi per meter
  bmpInfoHdr.biYPelsPerMeter  = 75 * 39;          // Write Y dpi per meter
  bmpInfoHdr.biClrUsed    = 0;            // Write biClrUsed
  bmpInfoHdr.biClrImportant   = 0;            // Write biClrImportant
  
  // Swap bytes if necessary
#ifdef  IN_BIG_ENDIAN
  ignore1 = (unsigned long) SwapBytes( (int32) ignore1 );
  ignore2 = (unsigned long) SwapBytes( (int32) ignore2 );
  
  bmpInfoHdr.biSize          = (long) SwapBytes( (int32) bmpInfoHdr.biSize );
  bmpInfoHdr.biWidth         = (long) SwapBytes( (int32) bmpInfoHdr.biWidth );
  bmpInfoHdr.biHeight        = (long) SwapBytes( (int32) bmpInfoHdr.biHeight );
  bmpInfoHdr.biPlanes        = (unsigned short) SwapBytes( (int16) bmpInfoHdr.biPlanes );
  bmpInfoHdr.biBitCount      = (unsigned short) SwapBytes( (int16) bmpInfoHdr.biBitCount );
  bmpInfoHdr.biCompression   = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biCompression );
  bmpInfoHdr.biSizeImage     = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biSizeImage );
  bmpInfoHdr.biXPelsPerMeter = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biXPelsPerMeter );
  bmpInfoHdr.biYPelsPerMeter = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biYPelsPerMeter );
  bmpInfoHdr.biClrUsed       = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biClrUsed );
  bmpInfoHdr.biClrImportant  = (unsigned long) SwapBytes( (int32) bmpInfoHdr.biClrImportant );
#endif

  // Copy thumbnail header into clipdata
  memcpy(clipData->pClipData, &ignore1, sizeof(long));
  memcpy(clipData->pClipData + sizeof(long), &ignore2, sizeof(long));
  
  // Copy bitmap info header into clipdata
  memcpy(clipData->pClipData + 2 * sizeof(long), &bmpInfoHdr, sizeof(BITMAPINFOHEADER));
  
  // Get a new position for clipData->pClipData
  unsigned char * ptr = clipData->pClipData + 2 * sizeof(long) + sizeof(BITMAPINFOHEADER); 
  
  // Get window DIB format data
  switch(nbChannels)
  {
    case 1:
    {
      // For single channel images, create a default palette
      SetDefaultPalette(ptr); 
      
      // Get the new buffer pointer
      ptr += DefaultPaletteSize * sizeof(RGBQUAD); 
      
      // Write the thumbnail data
      //memcpy(ptr, (unsigned char *)buffer, thumbnailSize);
      writeDIB1((unsigned char *)buffer, ptr, thumbNailWidth, thumbNailHeight);
      break;
    }
    case 3:
      // Convert RGB format to BGR (Windows DIB format) 
      writeDIB24((unsigned char *)buffer, ptr, thumbNailWidth, thumbNailHeight);
      break;
      
    default:
      return FALSE;
  }

#ifdef _WINDOWS
  clipData->ulClipFmt = -1;
#else
#ifdef macintosh
  clipData->ulClipFmt = -2;
#endif    
#endif    

  return TRUE;
}

// Create window DIB default palette, which is grayscale ramp of 256 RGBQUAD
void SetDefaultPalette(unsigned char *palBuffer)
{
  register unsigned long *all; 
  
  // Set the grayscale ramp of RGBQUADs
  all = (unsigned long *)(palBuffer); 
  for ( long i = 0; i < DefaultPaletteSize; i++ ) {
    *all =  i << 24 | i << 16 | i << 8 | 0 ;

#ifdef  IN_LITTLE_ENDIAN

    *all = (unsigned long) SwapBytes( (int32) *all );

#endif

    
    all++; 
  }
}

// Convert RGB format to BGR (Windows DIB format) 
void writeDIB24(unsigned char *obp, unsigned char *pic24, unsigned long w, unsigned long h)
{
  long i, j, padb;
  unsigned char *pp;

  padb = (4 - ((w * 3) % 4)) & 0x03;    // # of pad bytes to write at EOscanline */

  // DIBs are upside down... how typical...
  for ( i = h - 1; i >= 0; i-- ) {
     pp = pic24 + (i * (w * 3 + padb));

    for (j = 0; j < (long) w; j++) {
      pp[2] = *obp++;   /* B   (again, how typical...) */
      pp[1] = *obp++;   /* G */
      pp[0] = *obp++;   /* R */
      pp+=3;
    }
    for (j = 0; j < padb; j++)
      *pp++ = 0;
  }
}

void writeDIB1(unsigned char *obp, unsigned char *pic8, unsigned long w, unsigned long h)
{
  unsigned long j, padw;
  long i;
  unsigned char *pp;

  padw = ((w + 3)/4) * 4;  /* 'w', padded to be a multiple of 32 */

  for (i=h-1; i>=0; i--) {
    pp = pic8 + (i * padw);  

    for (j=0; j<w; j++) *pp++ = *obp++;
    for ( ; j<padw; j++) *pp++ = 0;
  }
}  
//  - EOF ----------------------------------------------------------------------
