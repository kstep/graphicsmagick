//  ----------------------------------------------------------------------------
//  MODULE    : ColorTwist
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Monday, March 25, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//      SCCSID          : @(#)coltwist.cpp  1.7 12:47:27 01 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifdef macintosh
    #pragma segment ColorTwist
  #endif
  #include "coltwist.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

  #include <stdlib.h>
  #include <math.h>

#ifndef Debug_h
  #include "debug.h"
#endif

#ifndef Numbers_h
  #include "numbers.h"
#endif
#ifndef FPXBaselineIO_h
  #include "fpxlibio.h"
#endif

//  Constants
//  ---------

#define CLAMP(A,L,H)  ((A)<=(L) ? (unsigned char)(L) : (A)<(H) ? (unsigned char)(A) : (unsigned char)(H))

//  Variables
//  ---------

// PTCH_201 Replaced the old "portfolioLut[]" with the new "PyccToSrgbLUT" ...
static unsigned char PyccToSrgbLUT[] = {
0,1,1,2,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,23,
24,25,26,28,29,30,31,33,34,35,36,38,39,40,41,43,44,45,47,48,49,51,52,53,
55,56,57,59,60,61,63,64,65,67,68,70,71,72,74,75,76,78,79,81,82,83,85,86,
88,89,91,92,93,95,96,98,99,101,102,103,105,106,108,109,111,112,113,115,
116,118,119,121,122,123,125,126,128,129,130,132,133,134,136,137,138,140,
141,142,144,145,146,148,149,150,152,153,154,155,157,158,159,160,162,163,
164,165,166,168,169,170,171,172,174,175,176,177,178,179,180,182,183,184,
185,186,187,188,189,190,191,192,194,195,196,197,198,199,200,201,202,203,
204,204,205,206,207,208,209,210,211,212,213,213,214,215,216,217,217,218,
219,220,221,221,222,223,223,224,225,225,226,227,227,228,229,229,230,230,
231,231,232,233,233,234,234,235,235,236,236,236,237,237,238,238,238,239,
239,240,240,240,241,241,241,242,242,242,242,243,243,243,244,244,244,244,
245,245,245,245,245,246,246,246,246,246,247,247,247,247,247,247,248,248,
248,248,248,248,249,249,249,249,249,249,249,249,249,250,250,250,250,250,
250,250,250,250,250,251,251,251,251,251,251,251,251,251,251,251,251,251,
251,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,252,
253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
254,254,254,254,254,254,254,254,254,254,254,254,254,254,254,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
// PTCH_201 .... end of LUT replacement


// PTCH_302 .... Added the following LUT 
// The following LUT was obtained from KIES by Chris Hauf and was
//  originally called def_rgb2ycc_shapetab.
// PTCH_LUTCLIP - make LUT an array of unsigned ints
static unsigned int SrgbToPyccLUT[256] = {
    0, 1, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
   21, 22, 23,
   23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 36,
   37, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49,
   50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 60, 61, 62,
   63, 63, 64, 65, 66, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 73, 74,
   75, 75, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86,
   87, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98,
   99, 99, 100, 101, 101, 102, 103, 104, 104, 105, 106, 107, 107, 108,
  109, 110, 110, 111, 112, 113, 113, 114, 115, 116, 116, 117, 118, 119,
  119, 120, 121, 122, 123, 123, 124, 125, 126, 127, 127, 128, 129, 130,
  131, 132, 132, 133, 134, 135, 136, 137, 137, 138, 139, 140, 141, 142,
  143, 144, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155,
  155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 167, 168, 169,
  170, 171, 172, 173, 174, 175, 177, 178, 179, 180, 182, 183, 184, 185,
  187, 188, 190, 191, 193, 194, 196, 197, 199, 201, 203, 204, 206, 208,
  210, 213, 215, 218, 220, 223, 226, 230, 233, 237, 242, 247, 253, 259,
  268, 278, 292, 309, 327, 342
};

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------


//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

// Constructors of a color twist matrix :
// --------------------------------------

// Default is Identity :
PColorTwist::PColorTwist ()
{
  
  T11 = (float)1; T12 = (float)0; T13 = (float)0; T14 = (float)0;
  T21 = (float)0; T22 = (float)1; T23 = (float)0; T24 = (float)0;
  T31 = (float)0; T32 = (float)0; T33 = (float)1; T34 = (float)0;
                 T44 = (float)1;
                 
  applyPortfolioLut = FALSE;
  applyAlpha = FALSE;
}

// Copy construction
PColorTwist::PColorTwist (const PColorTwist& T)
{
  T11 = T.T11; T12 = T.T12; T13 = T.T13; T14 = T.T14;
  T21 = T.T21; T22 = T.T22; T23 = T.T23; T24 = T.T24;
  T31 = T.T31; T32 = T.T32; T33 = T.T33; T34 = T.T34;
                       T44 = T.T44;
  applyPortfolioLut = FALSE;
  applyAlpha = FALSE;
}

// Using a matrix as specfied in FPXBaselineIO.h
PColorTwist::PColorTwist (const FPXColorTwistMatrix& T)
{
  T11 = T.byy;  T12 = T.byc1;  T13 = T.byc2;  T14 = T.dummy1_zero;
  T21 = T.bc1y; T22 = T.bc1c1; T23 = T.bc1c2; T24 = T.dummy2_zero;
  T31 = T.bc2y; T32 = T.bc2c1; T33 = T.bc2c2; T34 = T.dummy3_zero;
                            T44 = T.dummy7_one;
  applyPortfolioLut = FALSE;
  applyAlpha = FALSE;
}

// Explicit construction
PColorTwist::PColorTwist (float t11, float t12, float t13, float t14,
              float t21, float t22, float t23, float t24,
              float t31, float t32, float t33, float t34,
              float t44)
{
  T11 = t11; T12 = t12; T13 = t13; T14 = t14;
  T21 = t21; T22 = t22; T23 = t23; T24 = t24;
  T31 = t31; T32 = t32; T33 = t33; T34 = t34;
                   T44 = t44;
  applyPortfolioLut = FALSE;
  applyAlpha = FALSE;
}

// Predefined transformations
// Define different types of transformation used in FlashPix. See Appendix B: Colortwist matrices
// for a definition of these transformations and an explanation of the notation used here.
PColorTwist::PColorTwist (FPXTransfoColor transfo)
{
  switch (transfo) {
    case ycc_to_YCC8:   // Normalized PhotoYCC to 8-bit PhotoYCC
      T11 = (float)181.9; T12 = (float)0;     T13 = (float)0;      T14 = (float)0;
      T21 = (float)0;     T22 = (float)111.4; T23 = (float)0;      T24 = (float)156.0;
      T31 = (float)0;     T32 = (float)0;     T33 = (float)135.64; T34 = (float)137.0;
                                           T44 = (float)1;
      break;
    case YCC8_to_ycc:   // 8-bit PhotoYCC to normalized PhotoYCC
      T11 = (float)0.005498; T12 = (float)0;        T13 = (float)0;        T14 = (float)0;
      T21 = (float)0;        T22 = (float)0.008977; T23 = (float)0;        T24 = (float)-1.4;
      T31 = (float)0;        T32 = (float)0;        T33 = (float)0.007373; T34 = (float)-1.01;
                                         T44 = (float) 1;
      break;
    case ycc_to_rgb:    // Normalized PhotoYCC to normalized PhotoRGB
      T11 = (float)1; T12 = (float)0;      T13 = (float)1;      T14 = (float)0;
      T21 = (float)1; T22 = (float)-0.194; T23 = (float)-0.509; T24 = (float)0;
      T31 = (float)1; T32 = (float)1;      T33 = (float)0;      T34 = (float)0;
                                    T44 = (float)1;
      break;
    case rgb_to_ycc:    // Normalized PhotoRGB to normalized PhotoYCC
      T11 = (float)0.299;  T12 = (float)0.587;  T13 = (float)0.114;  T14 = (float)0;
      T21 = (float)-0.299; T22 = (float)-0.587; T23 = (float)0.886;  T24 = (float)0;
      T31 = (float)0.701;  T32 = (float)-0.587; T33 = (float)-0.114; T34 = (float)0;
                                       T44 = (float)1;
      break;
    case RGB8_to_rgb:   // 8-bit NIF RGB to normalized PhotoRGB
      T11 = (float)0.004047; T12 = (float)0;        T13 = (float)0;        T14 = (float)0;
      T21 = (float)0;        T22 = (float)0.004047; T23 = (float)0;        T24 = (float)0;
      T31 = (float)0;        T32 = (float)0;        T33 = (float)0.004047; T34 = (float)0;
                                         T44 = (float)1;
      break;
    case rgb_to_RGB8:   // Normalized PhotoRGB to 8-bit NIF RGB
      T11 = (float)247.1; T12 = (float)0;     T13 = (float)0;     T14 = (float)0;
      T21 = (float)0;     T22 = (float)247.1; T23 = (float)0;     T24 = (float)0;
      T31 = (float)0;     T32 = (float)0;     T33 = (float)247.1; T34 = (float)0;
                                    T44 = (float)1;
      break;
                // PTCH_302 added the following new matrix case
    case rgb_to_mono:   // Normalized RGB to normalized monochrome 
      T11 = (float)0.299;    T12 = (float)0.587;    T13 = (float)0.114;    T14 = (float)0;
      T21 = (float)0;        T22 = (float)0;        T23 = (float)0;        T24 = (float)0;
      T31 = (float)0;        T32 = (float)0;        T33 = (float)0;        T34 = (float)0;
                                             T44 = (float)1;
      break;
    default:        // Default is Identity
      T11 = (float)1; T12 = (float)0; T13 = (float)0; T14 = (float)0;
      T21 = (float)0; T22 = (float)1; T23 = (float)0; T24 = (float)0;
      T31 = (float)0; T32 = (float)0; T33 = (float)1; T34 = (float)0;
                              T44 = (float)1;
      break;
  }
}

PColorTwist::~PColorTwist ()
{
}

// "=" operator
PColorTwist& PColorTwist::operator=(const PColorTwist& T)
{
  T11 = T.T11; T12 = T.T12; T13 = T.T13; T14 = T.T14;
  T21 = T.T21; T22 = T.T22; T23 = T.T23; T24 = T.T24;
  T31 = T.T31; T32 = T.T32; T33 = T.T33; T34 = T.T34;
                       T44 = T.T44;
  return *this;
}

// Composition of color transformations : use : T = T1 * T2;
// CAUTION : This function IS NOT commutative
PColorTwist operator*(const PColorTwist& T1, const PColorTwist& T2)
{ 
  PColorTwist T;
  
  T.T11 = T1.T11*T2.T11 + T1.T12*T2.T21 + T1.T13*T2.T31;
  T.T12 = T1.T11*T2.T12 + T1.T12*T2.T22 + T1.T13*T2.T32;
  T.T13 = T1.T11*T2.T13 + T1.T12*T2.T23 + T1.T13*T2.T33;
  T.T14 = T1.T11*T2.T14 + T1.T12*T2.T24 + T1.T13*T2.T34 + T1.T14*T2.T44;
  
  T.T21 = T1.T21*T2.T11 + T1.T22*T2.T21 + T1.T23*T2.T31;
  T.T22 = T1.T21*T2.T12 + T1.T22*T2.T22 + T1.T23*T2.T32;
  T.T23 = T1.T21*T2.T13 + T1.T22*T2.T23 + T1.T23*T2.T33;
  T.T24 = T1.T21*T2.T14 + T1.T22*T2.T24 + T1.T23*T2.T34 + T1.T24*T2.T44;
  
  T.T31 = T1.T31*T2.T11 + T1.T32*T2.T21 + T1.T33*T2.T31;
  T.T32 = T1.T31*T2.T12 + T1.T32*T2.T22 + T1.T33*T2.T32;
  T.T33 = T1.T31*T2.T13 + T1.T32*T2.T23 + T1.T33*T2.T33;
  T.T34 = T1.T31*T2.T14 + T1.T32*T2.T24 + T1.T33*T2.T34 + T1.T34*T2.T44;
  
  T.T44 = T1.T44*T2.T44;

  return T;
}

// Composition of color transformations : use : T1 *= T; (<=> T1 = T1 * T)
// CAUTION : This function IS NOT commutative
PColorTwist& PColorTwist::operator*=(const PColorTwist& T)
{ 
  *this = *this * T;
  return *this;
}

// Inversion of the transformation :
// ---------------------------------

void PColorTwist::Inverse ()
{
  // Not implemented yet!!!
  assert(FALSE);
}

void PColorTwist::UseAlphaChannel(void)
{
  applyAlpha = TRUE;
}

void PColorTwist::UsePortfolioLut(void)
{
  applyPortfolioLut = TRUE;
}

// Apply the transformation :
// --------------------------
// CAUTION: For the moment, we make the hypothesis that T44 is equal to 1 and that a
// pixel is 4 bytes wide.

void PColorTwist::ApplyToBuffer(unsigned char* buffer, long size)
{
  register int c1, c2, c3;
  float alpha_scale;
  unsigned char *red_pixel,*green_pixel,*blue_pixel;
  unsigned char *alpha_pixel;
  
  red_pixel   = buffer;
  green_pixel = buffer + 1;
  blue_pixel  = buffer + 2;
  if(applyAlpha)
    alpha_pixel = buffer + 3;
  else
    alpha_scale = 1.0;
  
  
  if (applyPortfolioLut) { // image data is YCC so a lut must be used to handle the hightlight data
  
    while (size--) {
      if(applyAlpha)
        alpha_scale = (float)(*alpha_pixel)/(float)255.0;
      c1 = (int)((T11 * *red_pixel) + (T12 * *green_pixel) + (T13 * *blue_pixel) + (T14 * alpha_scale));
      c2 = (int)((T21 * *red_pixel) + (T22 * *green_pixel) + (T23 * *blue_pixel) + (T24 * alpha_scale));
      c3 = (int)((T31 * *red_pixel) + (T32 * *green_pixel) + (T33 * *blue_pixel) + (T34 * alpha_scale));

      // fill *red_pixel
      if (c1 < 0)
        *red_pixel = 0;
      else {
        if ((unsigned int) c1 > (sizeof (PyccToSrgbLUT) - 1))    // PTCH_201 changed "portfolioLut"
          c1 = (sizeof (PyccToSrgbLUT) - 1);    // PTCH_201   to "PyccToSrgbLUT"
        *red_pixel = PyccToSrgbLUT[c1];       // PTCH_201   in these 3 lines
      }
      
      // fill *green_pixel
      if (c2 < 0)
        *green_pixel = 0;
      else {
        if ((unsigned int) c2 > (sizeof (PyccToSrgbLUT) - 1))    // PTCH_201 chg'ed "portfolioLut"
          c2 = (sizeof (PyccToSrgbLUT) - 1);    // PTCH_201   to "PyccToSrgbLUT"
        *green_pixel = PyccToSrgbLUT[c2];     // PTCH_201   in these 3 lines
      }

      // fill *blue_pixel
      if (c3 < 0)
        *blue_pixel = 0;
      else {
        if ((unsigned int) c3 > (sizeof (PyccToSrgbLUT) - 1))    // PTCH_201 chg'ed "portfolioLut"
          c3 = (sizeof (PyccToSrgbLUT) - 1);    // PTCH_201   to "PyccToSrgbLUT"
        *blue_pixel = PyccToSrgbLUT[c3];      // PTCH_201   in these 3 lines
      }
      
      red_pixel   += 4;
      green_pixel += 4;
      blue_pixel  += 4;
      if(applyAlpha)
        alpha_pixel += 4;
    }
    
  } else {
  
      while (size--) {
        if(applyAlpha)
          alpha_scale = (float)(*alpha_pixel)/(float)255.0;
        c1 = (int)((T11 * *red_pixel) + (T12 * *green_pixel) + (T13 * *blue_pixel) +
            (T14 * alpha_scale));
        c2 = (int)((T21 * *red_pixel) + (T22 * *green_pixel) + (T23 * *blue_pixel) + 
            (T24 * alpha_scale));
        c3 = (int)((T31 * *red_pixel) + (T32 * *green_pixel) + (T33 * *blue_pixel) +
              (T34 * alpha_scale));

        *red_pixel   = CLAMP(c1,0,255);
        *green_pixel = CLAMP(c2,0,255);
        *blue_pixel  = CLAMP(c3,0,255);

        red_pixel   += 4;
        green_pixel += 4;
        blue_pixel  += 4;
        if(applyAlpha)
          alpha_pixel += 4;
      }
  }
}

// PTCH_302 - added the following new method
// Apply the RBG-to-YCC transformation. This uses a shaping LUT to increase the
//  contrast slightly prior to applying the colortwist matrix.
// --------------------------
// This routine assumes that T44 is equal to 1 and that a pixel is 4 bytes wide.
void PColorTwist::ApplyRGBtoYCCLut(unsigned char* buffer, long size)
{
  register int  c1, c2, c3;
  register int  rInt, gInt, bInt;     // PTCH_LUTCLIP - allow values > 255 from LUT
  float       alpha_scale;
  unsigned char   *red_pixel, *green_pixel, *blue_pixel;
  unsigned char   *alpha_pixel;
  
  red_pixel   = buffer;
  green_pixel = buffer + 1;
  blue_pixel  = buffer + 2;
  if(applyAlpha)
    alpha_pixel = buffer + 3;
  else
    alpha_scale = 1.0;
  
  while (size--) {
    if(applyAlpha)
      alpha_scale = (float)(*alpha_pixel)/(float)255.0;
    
    // PTCH_LUTCLIP - assign the LUT output to ints (rather than chars) because the
    //  LUT returns values greater than 255
    rInt = SrgbToPyccLUT[*red_pixel];
    gInt = SrgbToPyccLUT[*green_pixel]; 
    bInt = SrgbToPyccLUT[*blue_pixel];
    
    // PTCH_LUTCLIP - use the intermediate ints in calc, rather than the buffer chars
    c1 = (int)((T11 * rInt) + (T12 * gInt) + (T13 * bInt) + (T14 * alpha_scale));
    c2 = (int)((T21 * rInt) + (T22 * gInt) + (T23 * bInt) + (T24 * alpha_scale));
    c3 = (int)((T31 * rInt) + (T32 * gInt) + (T33 * bInt) + (T34 * alpha_scale));

    *red_pixel   = CLAMP(c1,0,255);
    *green_pixel = CLAMP(c2,0,255);
    *blue_pixel  = CLAMP(c3,0,255);
    
    red_pixel   += 4;
    green_pixel += 4;
    blue_pixel  += 4;
    if(applyAlpha)
      alpha_pixel += 4;
  }
}


/**
Boolean PColorTwist::ApplyToPixelBuffer(Pixel* pixels, FPXBaselineColorSpace colorSpace, long size)
{
  register int c1, c2, c3;
  unsigned char *red_pixel,*green_pixel,*blue_pixel;

  switch (colorSpace) {
    case  SPACE_32_BITS_RGB:  // The 24 bits are stored in the LSB part of the long
    case  SPACE_32_BITS_ARGB:
    case  SPACE_32_BITS_YCC:
    case  SPACE_32_BITS_AYCC:
      {
      red_pixel   = ((unsigned char*)pixels) + 1;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 3;
      } break;
    case  SPACE_32_BITS_RGBA:
    case  SPACE_32_BITS_YCCA:
      {
      red_pixel   = ((unsigned char*)pixels);
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 2;
      } break;
    case  SPACE_32_BITS_M:
      {
      red_pixel   = ((unsigned char*)pixels) + 3;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 1;
      } break;
    case  SPACE_32_BITS_MA:
      {
      red_pixel   = ((unsigned char*)pixels) + 2;
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 0;
      } break;

    default:
      {
      return FALSE;  // Invalid color space
      }
  }
    
  while (size--) {
    c1 = (int)((T11 * *red_pixel) + (T12 * *green_pixel) + (T13 * *blue_pixel) + T14);
    c2 = (int)((T21 * *red_pixel) + (T22 * *green_pixel) + (T23 * *blue_pixel) + T24);
    c3 = (int)((T31 * *red_pixel) + (T32 * *green_pixel) + (T33 * *blue_pixel) + T34);

    *red_pixel   = CLAMP(c1,0,255);
    *green_pixel = CLAMP(c2,0,255);
    *blue_pixel  = CLAMP(c3,0,255);

    red_pixel   += 4;
    green_pixel += 4;
    blue_pixel  += 4;
  }
  return TRUE;
}
**/
Boolean PColorTwist::ApplyToPixelBuffer(Pixel* pixels, FPXBaselineColorSpace colorSpace, long size)
{
  register int c1, c2, c3;
  unsigned char *red_pixel,*green_pixel,*blue_pixel;
  unsigned char *alpha_pixel;
  unsigned char clampLimit;
  float alpha_scale;
  
  if(!applyAlpha)
    alpha_scale = 1.0;
  
  clampLimit  = 255;
  alpha_pixel = 0;
  
  switch (colorSpace) {
    case  SPACE_32_BITS_ARGB: // The 24 bits are stored in the LSB part of the long
    case  SPACE_32_BITS_AYCC:
      alpha_pixel = ((unsigned char*)pixels) + 0;
    case  SPACE_32_BITS_RGB:
    case  SPACE_32_BITS_YCC:
      {
      red_pixel   = ((unsigned char*)pixels) + 1;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 3;
      } break;
    case  SPACE_32_BITS_RGBA:
    case  SPACE_32_BITS_YCCA:
      {
      red_pixel   = ((unsigned char*)pixels);
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 2;
      alpha_pixel = ((unsigned char*)pixels) + 3;
      } break;
    case  SPACE_32_BITS_M:
      {
      red_pixel   = ((unsigned char*)pixels) + 3;
      green_pixel = ((unsigned char*)pixels) + 2;
      blue_pixel  = ((unsigned char*)pixels) + 1;
      } break;
    case  SPACE_32_BITS_MA:
      {
      red_pixel   = ((unsigned char*)pixels) + 2;
      green_pixel = ((unsigned char*)pixels) + 1;
      blue_pixel  = ((unsigned char*)pixels) + 0;
      alpha_pixel = ((unsigned char*)pixels) + 3;
      } break;

    default:
      {
      return FALSE;  // Invalid color space
      }
  }
  
  while (size--) {
    if(applyAlpha)
      alpha_scale = (float)(*alpha_pixel)/(float)255.0;
    if(alpha_pixel)
      clampLimit = *alpha_pixel;

    c1 = (int)((T11 * *red_pixel) + (T12 * *green_pixel) + (T13 * *blue_pixel) + T14 * alpha_scale);
    c2 = (int)((T21 * *red_pixel) + (T22 * *green_pixel) + (T23 * *blue_pixel) + T24 * alpha_scale);
    c3 = (int)((T31 * *red_pixel) + (T32 * *green_pixel) + (T33 * *blue_pixel) + T34 * alpha_scale);

    *red_pixel   = CLAMP(c1,0,clampLimit);
    *green_pixel = CLAMP(c2,0,clampLimit);
    *blue_pixel  = CLAMP(c3,0,clampLimit);

    red_pixel   += 4;
    green_pixel += 4;
    blue_pixel  += 4;
    if(alpha_pixel)
      alpha_pixel += 4;
  }
  return TRUE;
}

void PColorTwist::ConvertToFPX (FPXColorTwistMatrix* T)
{
  T->byy  = T11;  T->byc1  = T12;  T->byc2  = T13;  T->dummy1_zero = T14;
  T->bc1y = T21;  T->bc1c1 = T22;  T->bc1c2 = T23;  T->dummy2_zero = T24;
  T->bc2y = T31;  T->bc2c1 = T32;  T->bc2c2 = T33;  T->dummy3_zero = T34;
  T->dummy7_one = T44;
}

Boolean PColorTwist::IsIdentity()
{
return((T11==1.0)&&(T22==1.0)&&(T33==1.0)&&(T44==1.0)&&
     (T12==0.0)&&(T13==0.0)&&(T14==0.0)&&
     (T21==0.0)&&(T23==0.0)&&(T24==0.0)&&
     (T31==0.0)&&(T32==0.0)&&(T34==0.0));
}
//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
