//  ----------------------------------------------------------------------------
//  MODULE    : PTile
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 13, 1996 
//  DESCRIPTION :  
//      The PTile object provides the base behavior for individual tiles. It
//    also provides methods for handling lists of tiles which have pixel memory
//    allocated to them.
//      Not all behaviors are implemented here. For example, reading and writing
//    are in the PTileFlashPix object which is derived from PTile.
//  COMMENTS  :   
//  MODIFIED  : 11/96   PBH   Rewrote memory allocation; added comments
//        : 2/26/97 PBH   PTCH_101 fixes buffer reuse bugs; adds debug calls
//  SCCSID      : @(#)ptile.cpp 1.5 15:15:42 14 Apr 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "ptile.h"
//  ----------------------------------------------------------------------------


//  Includes
//  --------

#ifndef Memoire_h
  #include  "b_memory.h"
#endif
#ifndef Debug_h
  #include  "debug.h"
#endif
#ifndef Numbers_h
  #include  "numbers.h"
#endif

#ifndef PHierarchicalImage_h
  #include "ph_image.h"
#endif
#ifndef PResolutionLevel_h
  #include "pr_level.h"
#endif
#ifndef PImageFile_h
  #include "pimgfile.h"
#endif

#ifndef PTileFlashPix_h
  #include "ptil_fpx.h"
#endif

#ifndef PResolutionFlashPix_h // PTCH_104 added #include required for code change
  #include "pres_fpx.h"
#endif  



//  Constants
//  ---------

  #define SW 0
  #define SE 1
  #define NW 2
  #define NE 3

//  Variables
//  ---------

  unsigned char*  PTile::invertLUT  = NULL;     // static LUT of inverted values

  PTile*      PTile::first    = NULL;     // Head of linked-list of tiles that have memory
  PTile*      PTile::last     = NULL;     // Tail of linked-list of tiles that have memory
  PTile**     PTile::locked   = NULL;     // An array of ptrs to tiles that have been locked
  long        PTile::indexLocked  = 0;    // Current index into the 'locked' array
  long        PTile::allocSize  = 0;      // Accumulate used during Purge() of memory
  long        PTile::allocTiles = 0;      // Debug use

  Ptr         PTile::decompressBuffer = NULL;
  long        PTile::decompressSize = 0;
  Boolean     PTile::decompressLock = false;


//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

// 2x2 decimation:
//
//    pNW(1/4)    pNE(1/4)
//
//    pSW(1/4)    pSE(1/4)
//

static void ConvolStandard (Pixel* source, long width, long height, Pixel* dest, long pixelsPerLine)

{
  register Pixel *pt, *pSE, *pSW, *pNE, *pNW;
  register long i, j, lineWidth;

  // Compute loop control values
  long oddLine    = width & 0x01;
  long halfWidth  = width  >> 1;
  long halfHeight = height >> 1;
  
  // Initialize loop pointers
  pt = dest;
  lineWidth = pixelsPerLine - halfWidth;

  // Check and handle degenerated cases...
  if ((halfWidth == 0) && (halfHeight == 0)) {
    *pt = *source;
  } else if (halfWidth == 0) {
    for (j = 0; j < halfHeight; j++, pt += lineWidth, source += (2*width))
      *pt = *source;
  } else if (halfHeight == 0) {
    for (i = 0; i < halfWidth; i++, pt++, source += 2)
      *pt = *source;
  } else {

    // Convolution loop
    pSW = source;
    pSE = pSW + 1;
    pNW = pSW + width;
    pNE = pNW + 1;
    for (j = 0; j < halfHeight; j++, pt += lineWidth) {
      for (i = 0; i < halfWidth; i++, pt++, pSW+=2, pSE+=2, pNW+=2, pNE+=2) {
        pt->alpha = (pSW->alpha + pSE->alpha + pNW->alpha + pNE->alpha) >> 2;
        pt->rouge = (pSW->rouge + pSE->rouge + pNW->rouge + pNE->rouge) >> 2;
        pt->vert  = (pSW->vert  + pSE->vert  + pNW->vert  + pNE->vert)  >> 2;
        pt->bleu  = (pSW->bleu  + pSE->bleu  + pNW->bleu  + pNE->bleu)  >> 2;
      }
      if (oddLine) {
        pSW++; pSE++; pNW++; pNE++;
      }
      pSW+=width; pSE+=width; pNW+=width; pNE+=width; 
    }
    
  }
}

// 4x4 decimation:
//
//    p11(1/4)    p12(1/2)    p13(1/2)    p14(1/4)
//
//    p21(1/2)     p22(1)      p23(1)     p24(1/2)
//
//    p31(1/2)     p32(1)      p33(1)     p34(1/2)
//
//    p41(1/4)    p42(1/2)    p43(1/2)    p44(1/4)
//

#ifdef _WINDOWS
  #pragma optimize ("", off)
#endif

static void ConvolGaussian4 (Pixel* source, long width, long height, Pixel* dest, long lineOffset)

{
#ifdef macintosh
  #pragma global_optimizer off
#endif

  // Simplification when width or height too small for the kernel
  if ((width < 6) || (height < 6)) {
    ConvolStandard (source, width, height, dest, lineOffset);
    return;
  }

  Pixel *pt, *p11, *p12, *p13, *p14, *p21, *p22, *p23, *p24, *p31, *p32, *p33, *p34, *p41, *p42, *p43, *p44;
  long i, j, lineWidth;

  // Compute loop control values
  long oddLine    = width & 0x01;
  long halfWidth  = width  >> 1;
  long halfHeight = height >> 1;
  
  // Initialize loop pointers
  pt = dest;
  lineWidth = lineOffset-halfWidth;

  // Init center pointers
  p22 = source;
  p23 = p22 + 1;
  p32 = p22 + width;
  p33 = p32 + 1;
  
  // Top line
  for (i=0; i<halfWidth; i++, pt++, p22+=2, p23+=2, p32+=2, p33+=2) { 
    pt->alpha = (p22->alpha + p23->alpha + p32->alpha + p33->alpha) >> 2;
    pt->rouge = (p22->rouge + p23->rouge + p32->rouge + p33->rouge) >> 2;
    pt->vert  = (p22->vert  + p23->vert  + p32->vert  + p33->vert)  >> 2;
    pt->bleu  = (p22->bleu  + p23->bleu  + p32->bleu  + p33->bleu)  >> 2;
  }

  // Next line
  if (oddLine) {
    p22++; p23++; p32++; p33++;
  }
  p22+=width, p23+=width, p32+=width, p33+=width; 
  pt += lineWidth;

  // Init edge and corner pointers
  p12 = p22 - width; p11 = p12 - 1; p13 = p12 + 1; p14 = p12 + 2;
  p21 = p22 - 1; p24 = p23 + 1;
  p31 = p32 - 1; p34 = p33 + 1;
  p42 = p32 + width; p41 = p42 - 1; p43 = p42 + 1; p44 = p42 + 2;


  // For every line (except the bottom most one)
  for (j=1; j<(halfHeight-1); j++, pt += lineWidth) {
  
    // Left pixel of the line
    pt->alpha = (p22->alpha + p23->alpha + p32->alpha + p33->alpha) >> 2;
    pt->rouge = (p22->rouge + p23->rouge + p32->rouge + p33->rouge) >> 2;
    pt->vert  = (p22->vert  + p23->vert  + p32->vert  + p33->vert)  >> 2;
    pt->bleu  = (p22->bleu  + p23->bleu  + p32->bleu  + p33->bleu)  >> 2;
    
    // Next pixel
    pt++; 
    p11+=2; p12+=2; p13+=2; p14+=2; 
    p21+=2; p22+=2; p23+=2; p24+=2; 
    p31+=2; p32+=2; p33+=2; p34+=2; 
    p41+=2; p42+=2; p43+=2; p44+=2; 
    
    // For every pixel (except the right most one)
    for (i=1; i<(halfWidth-1); i++, pt++) {
      pt->alpha  = (p22->alpha+p23->alpha+p32->alpha+p33->alpha + 
            ((p12->alpha+p13->alpha+p21->alpha+p24->alpha+p31->alpha+p34->alpha+p42->alpha+p43->alpha)>>1) + 
            ((p11->alpha+p14->alpha+p41->alpha+p44->alpha)>>2)) / 9;
      pt->rouge  = (p22->rouge+p23->rouge+p32->rouge+p33->rouge + 
            ((p12->rouge+p13->rouge+p21->rouge+p24->rouge+p31->rouge+p34->rouge+p42->rouge+p43->rouge)>>1) + 
            ((p11->rouge+p14->rouge+p41->rouge+p44->rouge)>>2)) / 9;
      pt->vert  = (p22->vert+p23->vert+p32->vert+p33->vert + 
            ((p12->vert+p13->vert+p21->vert+p24->vert+p31->vert+p34->vert+p42->vert+p43->vert)>>1) + 
            ((p11->vert+p14->vert+p41->vert+p44->vert)>>2)) / 9;
      pt->bleu  = (p22->bleu+p23->bleu+p32->bleu+p33->bleu + 
            ((p12->bleu+p13->bleu+p21->bleu+p24->bleu+p31->bleu+p34->bleu+p42->bleu+p43->bleu)>>1) + 
            ((p11->bleu+p14->bleu+p41->bleu+p44->bleu)>>2)) / 9;
      p11+=2; p12+=2; p13+=2; p14+=2; 
      p21+=2; p22+=2; p23+=2; p24+=2; 
      p31+=2; p32+=2; p33+=2; p34+=2; 
      p41+=2; p42+=2; p43+=2; p44+=2; 
    }

    // Right pixel of the line
    if (oddLine) {
      pt->alpha  = (p22->alpha+p23->alpha+p32->alpha+p33->alpha + 
            ((p12->alpha+p13->alpha+p21->alpha+p24->alpha+p31->alpha+p34->alpha+p42->alpha+p43->alpha)>>1) + 
            ((p11->alpha+p14->alpha+p41->alpha+p44->alpha)>>2)) / 9;
      pt->rouge  = (p22->rouge+p23->rouge+p32->rouge+p33->rouge + 
            ((p12->rouge+p13->rouge+p21->rouge+p24->rouge+p31->rouge+p34->rouge+p42->rouge+p43->rouge)>>1) + 
            ((p11->rouge+p14->rouge+p41->rouge+p44->rouge)>>2)) / 9;
      pt->vert  = (p22->vert+p23->vert+p32->vert+p33->vert + 
            ((p12->vert+p13->vert+p21->vert+p24->vert+p31->vert+p34->vert+p42->vert+p43->vert)>>1) + 
            ((p11->vert+p14->vert+p41->vert+p44->vert)>>2)) / 9;
      pt->bleu  = (p22->bleu+p23->bleu+p32->bleu+p33->bleu + 
            ((p12->bleu+p13->bleu+p21->bleu+p24->bleu+p31->bleu+p34->bleu+p42->bleu+p43->bleu)>>1) + 
            ((p11->bleu+p14->bleu+p41->bleu+p44->bleu)>>2)) / 9;
      p11+=3; p12+=3; p13+=3; p14+=3; 
      p21+=3; p22+=3; p23+=3; p24+=3; 
      p31+=3; p32+=3; p33+=3; p34+=3; 
      p41+=3; p42+=3; p43+=3; p44+=3;
      pt++;
    } else {
      pt->alpha = (p22->alpha + p23->alpha + p32->alpha + p33->alpha) >> 2;
      pt->rouge = (p22->rouge + p23->rouge + p32->rouge + p33->rouge) >> 2;
      pt->vert  = (p22->vert  + p23->vert  + p32->vert  + p33->vert)  >> 2;
      pt->bleu  = (p22->bleu  + p23->bleu  + p32->bleu  + p33->bleu)  >> 2;
      p11+=2; p12+=2; p13+=2; p14+=2; 
      p21+=2; p22+=2; p23+=2; p24+=2; 
      p31+=2; p32+=2; p33+=2; p34+=2; 
      p41+=2; p42+=2; p43+=2; p44+=2; 
      pt++;
    }
    
    // Next line
    p11+=width; p12+=width; p13+=width; p14+=width; 
    p21+=width; p22+=width; p23+=width; p24+=width; 
    p31+=width; p32+=width; p33+=width; p34+=width; 
    p41+=width; p42+=width; p43+=width; p44+=width; 
  }

  // Bottom line
  for (i=0; i<halfWidth; i++, pt++, p22+=2, p23+=2, p32+=2, p33+=2) {
    pt->alpha = (p22->alpha + p23->alpha + p32->alpha + p33->alpha) >> 2;
    pt->rouge = (p22->rouge + p23->rouge + p32->rouge + p33->rouge) >> 2;
    pt->vert  = (p22->vert  + p23->vert  + p32->vert  + p33->vert)  >> 2;
    pt->bleu  = (p22->bleu  + p23->bleu  + p32->bleu  + p33->bleu)  >> 2;
  }

#ifdef macintosh 
  #pragma global_optimizer on
#endif
}

#ifdef _WINDOWS 
  #pragma optimize ("", on)
#endif

// 3x3 decimation: non standard in FlashPix 
//
//    pNW(1/4)     pN(1/2)    pNE(1/4)
//
//    pW(1/2)        pC(1)        pE(1/2)
//
//    pSW(1/2)     pS(1/2)    pSE(1/4)
//
//

static void ConvolGaussian3 (Pixel* source, long width, long height,
                             Pixel* dest, long pixelsPerLine)

{
  // Simplification when width or height too small for the kernel
  if ((width < 4) || (height < 4)) {
    ConvolStandard (source, width, height, dest, pixelsPerLine);
    return;
  }

  register Pixel *pt, *pSE, *pSW, *pNE, *pNW, *pC, *pS, *pN, *pE, *pW;
  long i, j, lineWidth;

  // Compute loop control values
  long oddLine    = width & 0x01;
  long halfWidth  = width  >> 1;
  long halfHeight = height >> 1;
  
  // Initialize loop pointers
  pt = dest;
  lineWidth = pixelsPerLine - halfWidth;

  // Convolution loop
  pC = source;
  *pt++ = *pC;                                      // SW corner
  pC += 2;
  pW = pC - 1;
  pE = pC + 1;
  for (i=1; i<halfWidth; i++, pt++, pC+=2, pW+=2, pE+=2) {              // Southern line
    pt->alpha = (pC->alpha + ((pE->alpha+pW->alpha)>>1) + (pC->alpha<<1)) >> 2;
    pt->rouge = (pC->rouge + ((pE->rouge+pW->rouge)>>1) + (pC->rouge<<1)) >> 2;
    pt->vert  = (pC->vert + ((pE->vert+pW->vert)>>1) + (pC->vert<<1)) >> 2;
    pt->bleu  = (pC->bleu + ((pE->bleu+pW->bleu)>>1) + (pC->bleu<<1)) >> 2;
  }
  if (oddLine) {
    pC++; pW++; pE++;
  }
  pC+=width; pW+=width; pE+=width; 
  pN = pC + width;
  pNW = pN - 1;
  pNE = pN + 1;
  pS = pC - width;
  pSW = pS - 1;
  pSE = pS + 1;
  pt += lineWidth;
  for (j=1; j<halfHeight; j++, pt += lineWidth) {
    pt->alpha = (pC->alpha + ((pN->alpha+pS->alpha)>>1) + (pC->alpha<<1)) >> 2;   // Western pixel of the line
    pt->rouge = (pC->rouge + ((pN->rouge+pS->rouge)>>1) + (pC->rouge<<1)) >> 2;
    pt->vert  = (pC->vert + ((pN->vert+pS->vert)>>1) + (pC->vert<<1)) >> 2;
    pt->bleu  = (pC->bleu + ((pN->bleu+pS->bleu)>>1) + (pC->bleu<<1)) >> 2;
    pt++; pC += 2; pN += 2; pS += 2; pE += 2; pW += 2; pSW += 2; pSE += 2; pNW += 2; pNE += 2; 
    for (i=1; i<halfWidth; i++) {
      pt->alpha = (pC->alpha + ((pN->alpha+pS->alpha+pE->alpha+pW->alpha)>>1) + ((pNE->alpha+pSE->alpha+pNW->alpha+pSW->alpha)>>2)) >> 2;
      pt->rouge = (pC->rouge + ((pN->rouge+pS->rouge+pE->rouge+pW->rouge)>>1) + ((pNE->rouge+pSE->rouge+pNW->rouge+pSW->rouge)>>2)) >> 2;
      pt->vert  = (pC->vert + ((pN->vert+pS->vert+pE->vert+pW->vert)>>1) + ((pNE->vert+pSE->vert+pNW->vert+pSW->vert)>>2)) >> 2;
      pt->bleu  = (pC->bleu + ((pN->bleu+pS->bleu+pE->bleu+pW->bleu)>>1) + ((pNE->bleu+pSE->bleu+pNW->bleu+pSW->bleu)>>2)) >> 2;
      pt++; pC += 2; pN += 2; pS += 2; pE += 2; pW += 2; pSW += 2; pSE += 2; pNW += 2; pNE += 2; 
    }
    if (oddLine) {
      pC++; pN++; pS++; pW++; pE++; pSW++; pSE++; pNW++; pNE++;
    }
    pC+=width; pN+=width; pS+=width; pW+=width; pE+=width; pSW+=width; pSE+=width; pNW+=width; pNE+=width; 
  }
}

// 3x3 decimation: non standard in FlashPix 
//
//             pN(1/2)
//
//    pW(1/2)        pC(1)        pE(1/2)
//
//             pS(1/2)
//
//

static void ConvolGaussSimplified (Pixel* source, long width, long height, Pixel* dest, long pixelsPerLine)

{
  // Simplification when width or height too small for the kernel
  if ((width < 4) || (height < 4)) {
    ConvolStandard (source, width, height, dest, pixelsPerLine);
    return;
  }

  register Pixel *pt, *pC, *pS, *pN, *pE, *pW;
  long i, j, lineWidth;

  // Compute loop control values
  long oddLine    = width & 0x01;
  long halfWidth  = width  >> 1;
  long halfHeight = height >> 1;
  
  // Initialize loop pointers
  pt = dest;
  lineWidth = pixelsPerLine - halfWidth;

  // Convolution loop
  pC = source;
  *pt++ = *pC;                                        // SW corner
  pC += 2;
  pW = pC - 1;
  pE = pC + 1;
  for (i=1; i<halfWidth; i++, pt++, pC+=2, pW+=2, pE+=2) {              // Southern line
    pt->alpha = (pC->alpha + ((pE->alpha+pW->alpha)>>1)) >> 1;
    pt->rouge = (pC->rouge + ((pE->rouge+pW->rouge)>>1)) >> 1;
    pt->vert  = (pC->vert + ((pE->vert+pW->vert)>>1)) >> 1;
    pt->bleu  = (pC->bleu + ((pE->bleu+pW->bleu)>>1)) >> 1;
  }
  if (oddLine) {
    pC++; pW++; pE++;
  }
  pC+=width; pW+=width; pE+=width; 
  pN = pC + width;
  pS = pC - width;
  pt += lineWidth;
  for (j=1; j<halfHeight; j++, pt += lineWidth) {
    pt->alpha = (pC->alpha + ((pN->alpha+pS->alpha)>>1)) >> 1;            // Western pixel of the line
    pt->rouge = (pC->rouge + ((pN->rouge+pS->rouge)>>1)) >> 1;
    pt->vert  = (pC->vert + ((pN->vert+pS->vert)>>1)) >> 1;
    pt->bleu  = (pC->bleu + ((pN->bleu+pS->bleu)>>1)) >> 1;
    pt++; pC += 2; pN += 2; pS += 2; pE += 2; pW += 2;
    for (i=1; i<halfWidth; i++) {
      pt->alpha = (pC->alpha + ((pN->alpha+pS->alpha+pE->alpha+pW->alpha)>>2))>>1;
      pt->rouge = (pC->rouge + ((pN->rouge+pS->rouge+pE->rouge+pW->rouge)>>2))>>1;
      pt->vert  = (pC->vert + ((pN->vert+pS->vert+pE->vert+pW->vert)>>2))>>1;
      pt->bleu  = (pC->bleu + ((pN->bleu+pS->bleu+pE->bleu+pW->bleu)>>2))>>1;
      pt++; pC += 2; pN += 2; pS += 2; pE += 2; pW += 2;
    }
    if (oddLine) {
      pC++; pN++; pS++; pW++; pE++;
    }
    pC+=width; pN+=width; pS+=width; pW+=width; pE+=width;
  }
}

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PTile class
//
//  Manage a tile of a resolution level.
//  ----------------------------------------------------------------------------

PTile::PTile()

{
  // Allocate and compute the inversion LUT
  AllocInvertTable();
}

// Allocate and compute the inversion LUT
void PTile::AllocInvertTable()

{
  // if LUT inexisting
  if (!invertLUT) {
    // Reserve LUT
    FastAllocArray(invertLUT, unsigned char,256);
    // Initialize LUT with inverted values
    if (invertLUT) {
      for (long i = 0; i < 256; i++)
        invertLUT[i] = (unsigned char) (0xFF - i);
    }
  }
}

// suppress the 'pixels' buffer from the list of buffer in memory and delete it
PTile::~PTile()

{
  if (pixels || rawPixels) {
    if (pixels) {
      FastDeleteArray(pixels,Pixel);
      pixels = NULL;
#ifdef Memoire_Debug
      allocSize -= width * height * sizeof(Pixel);
      VISU2 "Destruction of an image tile pixels \n" FIN
      VISU2 "    Memory size available      = %d\n", allocSize FIN
#endif
    }
    if (rawPixels) {
      FastDeleteArray(rawPixels,Pixel);
      rawPixels = NULL;
#ifdef Memoire_Debug
      allocSize -= width * height * sizeof(Pixel);
      VISU2 "Destruction of an image tile rawPixels \n" FIN
      VISU2 "    Memory size available      = %d\n", allocSize FIN
#endif
    }
    Dispose();
  }
}


// Initialize a tile with compressor assignment
// Use this constructor in Create mode
void PTile::InitializeCreate (PResolutionLevel* father, long width, long height, long id)

{
  fatherSubImage    = father;     // Point to the father sub-image
  this->height    = (short)height;
  this->width     = (short)width;
  rawPixels     = NULL;
  freshPixels     = 0;
  pixels        = NULL;
  pixelsStale     = false;
  idCodec       = TLC_Aucun;
  decompressorIsMissing = false;
  posPixelFic     = -1;
  tileSize      = 0;
  identifier      = id;
  previous      = NULL;
  next        = NULL;
}

// Initialize a tile with file assignment
// Use this constructor in Read mode

void PTile::InitializeRead (PResolutionLevel* father, long offset, long sizetile,
                            long id, long /*compression*/, long /*nouse*/)

{
  fatherSubImage    = father;   // Point to the father sub-image
  height        = 0;
  width       = 0;
  rawPixels     = NULL;
  freshPixels     = 0;
  pixels        = NULL;
  pixelsStale     = false;
  idCodec       = TLC_Aucun;
  decompressorIsMissing = false;
  posPixelFic     = offset;
  tileSize      = sizetile;
  identifier      = id;
  previous      = NULL;
  next        = NULL;
  
  register long TILE_WIDTH = fatherSubImage->fatherFile->tileWidth;
  register long TILE_MASK  = fatherSubImage->fatherFile->maskTileWidth;

  long curLine = id/fatherSubImage->nbTilesW;
  long curCol =  id%fatherSubImage->nbTilesW;
  
  // Height of tile in pixels
  if (curLine == (fatherSubImage->nbTilesH - 1))
    height = ((fatherSubImage->realHeight - 1) & TILE_MASK) + 1;
  else
    height = (short) TILE_WIDTH;
    
  // Width of tile in pixels
  if (curCol == (fatherSubImage->nbTilesW-1))
    width = ((fatherSubImage->realWidth - 1) & TILE_MASK) + 1;
  else
    width = (short) TILE_WIDTH;
}

// ---------------------------------------------------------------------------------
// Allocate memory to the 'pixels' buffer stored in this tile.
// Returns 0 if successful, else -1 if failed.
//
long PTile::AllocatePixels ()
{
  if (AllocatePixelMemory( &pixels) == 0) {
    TouchPixelsBuffer( );               // Set access time stamp
    if ((this != first) && (this->previous == NULL))  // If not on the list
      Insert();                   //  then add it to the list
    return 0;
  }
  return -1;
}


// ---------------------------------------------------------------------------------
// Allocate memory to the 'rawPixels' buffer stored in this tile.
// Returns 0 if successful, else -1 if failed.
//
long PTile::AllocateRawPixels ()
{
  if (AllocatePixelMemory( &rawPixels) == 0) {
    TouchRawPixelsBuffer( );              // Set access time stamp
    if ((this != first) && (this->previous == NULL))  // If not on the list
      Insert();                   //  then add it to the list
    return 0;
  }
  return -1;
}


// ---------------------------------------------------------------------------------
// Allocate memory for a buffer of pixel data If possible, allocate from the global 
//  FPX pool; if not, then recycle memory that is currently being used by some 
//  other tile.
// The size of the buffer is assumed to be square and all buffers are the same size
//  as determined by the "father file".
// Note that the call to AvailableMemory() will trigger a call to FreeAncientBuffers()
//  if memory is low.
// Returns 0 if successful, else -1 if failed.
//

long PTile::AllocatePixelMemory( Pixel **memAddress)
{
  long  numFreeBytes;
  //  long  tileSize  = fatherSubImage->fatherFile->tileSize;
  long  sizeNeeded  = width * height * sizeof( Pixel);      // PTCH_101 new

  *memAddress = NULL;
  
#ifdef Memoire_Debug                        // PTCH_101 new - start...
    VISU2 "AllocatePixelMemory: tile %d:%d sizeNeeded = %d\n", 
      this->fatherSubImage->identifier, this->identifier, sizeNeeded FIN
    VISU2 "    Memory size available      = %d\n", allocSize FIN
#endif                                // PTCH_101 new - ...end
  
  // Get amount of memory currently available
  GtheSystemToolkit->AvailableMemory (&numFreeBytes); 

//  if (tileSize < numFreeBytes) {                     PTCH_101 rep
  if (sizeNeeded < numFreeBytes) {                // PTCH_101 mod
    // Allocate new memory from the global memory pool
    FastAllocArray( *memAddress, Pixel, width * height);
    if (*memAddress == NULL)
      return -1;
    allocSize += (width * height * sizeof( Pixel));       // PTCH_101 new
  }
  else {
    // No more global memory available, so search for the least recently used
    //  buffer of tile memory and assign it to this tile.
    PTile *foundTile;
    long  isRawPixelsBuffer;

// PTCH_101 replace the following
//    if (FindOldestTileBuffer( &foundTile, &isRawPixelsBuffer) != 0)
//      return -1;
// with
#ifdef Memoire_Debug
    VISU2 "AllocatePixelMemory: call FindOldestTileBuffer\n", sizeNeeded FIN
#endif

    if (FindOldestTileBuffer( &foundTile, &isRawPixelsBuffer, sizeNeeded) != 0) {
      // Failed to find a buffer for reuse. Calling FastAllocArray() will force
      //  a Purge() to occur.  If that fails, there is nowhere left that we can
      //  look for memory.
      
      FastAllocArray( *memAddress, Pixel, width * height);
      if (*memAddress == NULL)
        return -1;
      else {
        allocSize += (width * height * sizeof( Pixel));
        return 0;
      }
    }
    
#ifdef Memoire_Debug
    VISU2 "AllocatePixelMemory: FindOldestTileBuffer returned tile %d:%d\n", 
      foundTile->fatherSubImage->identifier, foundTile->identifier FIN
#endif
// PTCH_101 - end of replacement

    if (isRawPixelsBuffer) {
      if (foundTile->freshPixels)                 // PTCH_101 new
        foundTile->WriteTile();                 // PTCH_101 new
      *memAddress = foundTile->rawPixels;
      foundTile->rawPixels = NULL;
      foundTile->rawPixelsTime = 0;
      if( foundTile->pixels == NULL)
        foundTile->Dispose();
    }
    else {
      *memAddress = foundTile->pixels;
      foundTile->pixels = NULL;
      foundTile->pixelsTime = 0;
      if( foundTile->rawPixels == NULL)
        foundTile->Dispose();
    }
  }
  return 0;
}


// PTCH_101 - Replace the following
// ---------------------------------------------------------------------------------
// Find the least recently used tile buffer. Return the tile it belongs to and which
//  tile is oldest (e.g., if 'rawPixels' is the buffer found, then set 'isRawPixelsBuffer'
//  otherwise return 'isRawPixelsBuffer' as zero to indicate it's the 'pixels' buffer
//  that is oldest).
// Returns 0 if successful, else -1 if failed.
//
// long PTile::FindOldestTileBuffer( PTile **foundTile, long *isRawPixelsBuffer)
// with

// ---------------------------------------------------------------------------------
// Find the least recently used tile buffer that contains at least 'minSize'. 
//  Return the tile it belongs to and which buffer is oldest (e.g., if 'rawPixels' is
//  the buffer found, then set 'isRawPixelsBuffer'; otherwise return 'isRawPixelsBuffer'
//  as zero to indicate it's the 'pixels' buffer that is oldest).
// Returns 0 if successful, else -1 if failed (i.e., there are no allocated buffers!)
//
long PTile::FindOldestTileBuffer( PTile **foundTile, long *isRawPixelsBuffer, long minSize)

// PTCH_101 - end replacement
{
  PTile *currTile  = first;
  time_t  oldestTime = 0;


  // Find the first buffer on the list that isn't locked. Use one of it's buffers
  //  as the "oldest".
  *foundTile = 0;
  while (currTile) {
//    if ( !currTile->IsLocked()) {                          PTCH_101 rep
    if ( !currTile->IsLocked()                          // PTCH_101 new
     && ((currTile->width * currTile->height * sizeof( Pixel)) >= (unsigned long) minSize)) { // PTCH_101 new
      if (currTile->rawPixels) {
        oldestTime = currTile->rawPixelsTime;
        *isRawPixelsBuffer = 1;
      }
      else {
        oldestTime = currTile->pixelsTime;
        *isRawPixelsBuffer = 0;
      }
      *foundTile = currTile;
      break;
    }
    currTile = currTile->next;
  }
  
  // Starting with the current tile, continue checking for a buffer that
  // is older than the first one we found, above. 
  while (currTile) {
//    if (!currTile->IsLocked()) {                           PTCH_101 rep
    if ( !currTile->IsLocked()                          // PTCH_101 new
     && ((currTile->width * currTile->height * sizeof( Pixel)) >= (unsigned long) minSize)) { // PTCH_101 new
      if (currTile->rawPixels)
        if (currTile->rawPixelsTime < oldestTime) {
          *foundTile = currTile;
          oldestTime = currTile->rawPixelsTime;
          *isRawPixelsBuffer = 1;
        }
      if (currTile->pixels)
        if (currTile->pixelsTime < oldestTime) {
          *foundTile = currTile;
          oldestTime = currTile->pixelsTime;
          *isRawPixelsBuffer = 0;
        }
    }
    currTile = currTile->next;
  }
  return ((*foundTile == NULL)? -1 : 0);
}


// ---------------------------------------------------------------------------------
// Search through all of the buffers allocated by any tiles. If a buffer has not
//  been accessed within the number of minutes in 'numMinutesOld', then deallocate 
//  the memory -- returning it to the global pool for other (possibly non-tile) use.
//
void PTile::FreeAncientBuffers( long numMinutesOld)
{
  PTile *currTile = first;
  PTile *nextTile = currTile->next;
  time_t  ancientTime, timeNow;
  
  // Calculate the minimum time for the last access to the tile. If a tile
  //  has not been accessed within this amount of time, then we will consider
  //  it to be "ancient"
#ifdef _WINDOWS
  timeNow = GetCurrentTime();
  ancientTime = GetCurrentTime() - (numMinutesOld * 60L * CLOCKS_PER_SEC);
#else
  timeNow = clock();
  ancientTime = clock() - (numMinutesOld * 60L * CLOCKS_PER_SEC);
#endif

  for (currTile = first; currTile; currTile = nextTile) {
    nextTile = currTile->next;
    
    if (currTile->IsLocked() == false) {
//      // Free 'rawPixels' or 'pixels' if either is ancient          PTCH_101 rep
//      if (currTile->rawPixels)                        PTCH_101 rep

      // Free unmodified 'rawPixels' or 'pixels' if either is ancient   // PTCH_101 new
      if (currTile->rawPixels && (currTile->freshPixels == 0))      // PTCH_101 new
        if (currTile->rawPixelsTime < ancientTime) 
          currTile->FreeRawPixelsBuffer( );
      if (currTile->pixels)
        if (currTile->pixelsTime < ancientTime) 
          currTile->FreePixelsBuffer( );
    }
  }
}


// ---------------------------------------------------------------------------------
// Release the memory being used to cache pixels in this tile
// 
void PTile::FreePixelsBuffer( )
{
  if (pixels) {
    FastDeleteArray (pixels, Pixel);
    pixels = NULL;
    pixelsTime = 0;
  }
  if (rawPixels == NULL)
    Dispose();
}


// ---------------------------------------------------------------------------------
// Release the memory being used to cache raw pixels in this tile
// 
void PTile::FreeRawPixelsBuffer( )
{
  if (rawPixels) {
    if (freshPixels)                    // PTCH_101 new
      WriteTile();                    // PTCH_101 new
    FastDeleteArray (rawPixels, Pixel);
    rawPixels = NULL;
    rawPixelsTime = 0;
  }
  if (pixels == NULL)
    Dispose();
}



// ---------------------------------------------------------------------------------
// Check all of the 'rawPixel' buffers allocated by any tiles. If a buffer has
//  'freshPixels' (i.e., it has some pixels that have been modified but not written
//  to the file yet), then write the 'rawPixels' to the file.
// This is called to ensure that the file is updated and that all lower resolutions
//  have been updated (including the thumbnail).
// This does not free any memory.
//
void PTile::FlushModifiedTiles( )
{
  PTile *currTile;

  for (currTile = first; currTile; currTile = currTile->next)
    if (currTile->freshPixels)
      if (currTile->rawPixels)
        currTile->WriteTile();
}


// ---------------------------------------------------------------------------------
// Release memory used by tiles of images : after several tests, we have decided to
// free up ALL the memory used by the images instead of what is just necessary 
// in order to avoid the allocation of small tiles into the area reserved for the large
// ones. All other strategy will lead to fragment the memory or to release the more
// recently used images. The only tiles leaved in the memory are those belonging
// to the currently locked image file or those whitch are locked themself.
//
Boolean PTile::Purge(long* size, Boolean purgeAll)
{
#ifdef Memoire_Debug
// FOR DEBUG PURPOSE
VISU2 "\n\nEnter in PTile::Purge()\n" FIN
if (size) {
VISU2 "Size requested = %d\n", *size FIN
} else {
VISU2 "Soft purge\n" FIN
}
VISU2 "Memory size available      = %d\n", allocSize FIN
// END DEBUG PURPOSE
#endif

  long    freedSize = 0;
  long    theSize = 0;
  Boolean   status = false;
  Boolean   forced = false;
  PTile   *currTile, *nextTile;


  // Calling Purge() results in default of size=NULL, so ...
  if ( size != NULL ) theSize = *size;
  
  // If Purge requested with a size, force deletion of tiles
  if ( purgeAll)
    forced = true;
  
  // Free all the tiles already saved in a file
  for (currTile = first; currTile; currTile = nextTile) {
    nextTile = currTile->next;
    freedSize += currTile->Free(forced);
  }
  
  // Free all the tiles not entirely changed in memory (save them in a file before freeing the memory) 
  if ( theSize==0 || (freedSize < theSize) || purgeAll)
    for (currTile = first; currTile; currTile = nextTile) {
      nextTile = currTile->next;
      freedSize += currTile->Free(forced, true);
    }
  
  // Free eventually the decompression buffer
  if (forced) {
    if ( theSize==0 || (freedSize < theSize) || purgeAll)
      freedSize += PurgeDecompress ();
  }
  
  // Update size if requested
  if (theSize) {
    if (freedSize >= theSize) {     // If more than required, exit with success
      if (size) *size = 0;
    }
    else {              // Else update size and exit with error
      if (size) *size -= freedSize;
      status = true;
    }
  }

#ifdef Memoire_Debug
// FOR DEBUG PURPOSE 
VISU2 "Exit from PTile::Purge()\n" FIN
VISU2 "Memory frees   = %d\n", freedSize FIN
if (size) {
VISU2 "Size remaining = %d\n", *size FIN
}
VISU2 "Exit status = " FIN
if (status) {
VISU2 "Error\n\n" FIN
} else {
VISU2 "OK\n\n" FIN
}
// END DEBUG PURPOSE
#endif

  return status;
}


// ---------------------------------------------------------------------------------
// Compute the quantity of memory purgeable without purging the locked tiles
//
long PTile::GetPurgeableMemory()

{
  // Init purgeable memory size
  long purgeableMemory = 0;
  
  // For each tile allocated...
  PTile* ptTile = first;
  while (ptTile) {
    // If the image and the tile is not locked...
    if (!(GtheSystemToolkit->IsLockedImage(ptTile->fatherSubImage->fatherFile)) 
    && !(ptTile->IsLocked())) {
      // ... and if the tile contains pixels and is not being written...
      if ((ptTile->pixels != NULL) && (ptTile->freshPixels == 0))
        // ... then this tile is purgeable.
        purgeableMemory += ptTile->width * ptTile->height * sizeof(Pixel);  // compute memory purgeable
      if (ptTile->rawPixels)
        purgeableMemory += ptTile->width * ptTile->height * sizeof(Pixel);  // compute memory purgeable
    }
    // Jump to the next tile
    ptTile = ptTile->next;
  }
  return purgeableMemory;
}


// ---------------------------------------------------------------------------------
// Remove a tile from the list of tiles that own memory buffers
//
void PTile::Dispose()

{
  if (this == last)
    last = previous;
  else
    next->previous = previous;
  if (this == first)
    first = next;
  else
    previous->next = next;
    
  previous  = NULL;
  next      = NULL;
}


// ---------------------------------------------------------------------------------
// Insert tile at the end of the list
//
void PTile::Insert()

{
  previous   = last;
  next       = NULL;
  if (last)
    last->next = this;
  else
    first = this;
    
  last = this;
}


// ---------------------------------------------------------------------------------
// Free any buffers assigned to this tile
// Unless 'forced', tiles which are not the maximum size will not be freed.
// Unless 'freeIncomplete' is set, tiles that still have 'freshPixels' (not written
//  to the file yet, will not be freed. If 'freeIncomplete', then the buffer will
//  be written to the file and then freed.
//
long PTile::Free (Boolean forced, Boolean freeIncomplete)
{
  register long TILE_WIDTH = fatherSubImage->fatherFile->tileWidth;
  register long size = 0;

#ifdef Memoire_Debug
  VISU2 "Free the tile: %d:%d, forced = %d\n", fatherSubImage->identifier, identifier, forced FIN
#endif
  
  // If the tile belongs to the locked Image or is locked, do not free the tile
  if (GtheSystemToolkit->IsLockedImage(fatherSubImage->fatherFile) || IsLocked()) {

#ifdef Memoire_Debug
    if (Toolkit_IsLockedIVUE(fatherSubImage->fatherFile)) {
    VISU2 "   Can't free the tile because the file is locked\n" FIN
    }
    if (IsLocked()) {
    VISU2 "   Can't free the tile because the tile is locked\n" FIN
    }
#endif
  } 
  else {
    if (IsLocked())
      if (forced == false)
        goto RETURN;
        
    // There's no reason to protect 'pixels' which is used for display, unless it is
    //  less than a full tile in size and deallocation is not 'forced'
    if (pixels) {
      if ((forced) 
      || ((width == TILE_WIDTH) && (height == TILE_WIDTH))) {
        FastDeleteArray( pixels,Pixel);       // free this memory buffer
        pixels = NULL;
        pixelsTime = 0;
        size = width * height * sizeof(Pixel);    // compute memory freed
      }
      else {
        ;
#ifdef Memoire_Debug
        VISU2 "   Can't free pixels because it is not maximum size\n" FIN
#endif
      }
    }
    // If there are 'rawPixels' allocated, we can only free them if there are
    //  no 'freshPixels' (i.e., no pixel modifications that have not been saved
    //  to the file) or if the request is to 'freeIncomplete' (in which case
    //  we save what is there to the file and deallocate it)
    if (rawPixels) {
      if (freshPixels) {      
        if (forced || freeIncomplete) { 
          if (WriteTile()) {
#ifdef Memoire_Debug
            VISU2 "   Can't free rawPixels because error while writing it in the file\n" FIN
#endif
            goto RETURN;  // if an error occur during writing keep the tile in memory
          }
        }
        else {
#ifdef Memoire_Debug
//            VISU2 "   Can't free rawPixels because it's modified and not forced\n" FIN
#endif
          goto RETURN;    // Didn't save changes to rawPixels, so can't free yet
        }
      }
        
      // Release only tile whose size is the maximum (avoid border tiles management),
      //  unless the request is 'forced'
      if ((forced || freeIncomplete) 
      || ((width == TILE_WIDTH) && (height == TILE_WIDTH))) {
        FastDeleteArray (rawPixels,Pixel);      // free this memory buffer
        rawPixels = NULL;
        rawPixelsTime = 0;
        size += (width * height * sizeof(Pixel)); // compute memory freed
      }
      else {
#ifdef Memoire_Debug
//        VISU2 "   Can't free rawPixels because it's modified and not forced\n" FIN
#endif
        ;
      }
    }
    // If all tile buffer memory has been deallocated, remove the tile from the tile list
    if ((pixels == NULL) && (rawPixels == NULL)) {
      UnLock();
      Dispose();
    }
  }
 RETURN:
 
#ifdef Memoire_Debug
  allocSize -= size;
  if (size) {
    VISU2 "   Freed %d bytes\n", size FIN
  }
#endif
  return(size);
}


// ---------------------------------------------------------------------------------
// Return size of the tile. Return true if error, false if OK.
//
Boolean PTile::GetInfo (long* width, long* height, Typ_Compression* compr) 

{
  Boolean status = false;
  
  *width  = this->width; 
  *height = this->height;
  *compr  = (Typ_Compression)(idCodec);
  return (status);
}

// ---------------------------------------------------------------------------------
long PTile::GetCompression ()
{
  return 0;
}

// ---------------------------------------------------------------------------------
long PTile::GetCompressionSubtype ()
{
  return 0;
}

// ---------------------------------------------------------------------------------
void PTile::SetCompression (long theCompression)
{
  compression = theCompression;  
  idCodec   = ConvertCompressionOption();
}

// ---------------------------------------------------------------------------------
void PTile::SetCompressionSubtype (long theCompressionSubType)
{
  compressionSubtype = theCompressionSubType; 
}

// ---------------------------------------------------------------------------------
void PTile::SetQualityFactor (unsigned char theQualityFactor)
{
  qualityFactor = theQualityFactor; 
}

// ---------------------------------------------------------------------------------
TLC_IdCodec PTile::ConvertCompressionOption ()
{
   return TLC_Aucun;
}

// ---------------------------------------------------------------------------------
// Write the set of raw pixels in the tile
FPXStatus PTile::WriteRectangle (Pixel* pix, long width, long height, long rowOffset, 
        long x0, long y0, short plan)
{
  register Pixel* pt;
  register long   i;
  FPXStatus   status = FPX_OK;
  
  // Read or allocate the tile if necessary
  if (rawPixels == NULL)
    if ((status = ReadRawPixels()) != FPX_OK)
      return status;
  
  assert (rawPixels);
  // Compute first pixel of the tile to be written
  pt = rawPixels + (y0 * this->width) + x0;
  
  // Copy pixels to the tile
  if (plan == ActiveChannel_All) {
    // Line by line if all planes available to copy
    for (i = 0; i < height; i++, pt += this->width, pix += rowOffset)
      BlockMove(pix,pt,width*sizeof(Pixel));
  } else {
    // Pixel by pixel if planes specified
    register long j;
    register unsigned char *src, *dst;
    for (i = 0; i < height; i++, pt += this->width, pix += rowOffset) {
      src = (unsigned char *)(pix) + plan; 
      dst = (unsigned char *)(pt)  + plan;
      for (j = 0; j < width; j++, src += sizeof(Pixel), dst += sizeof(Pixel))
        *dst = *src;
    }
  }

  // PTCH_104 start of addition....
  // convert data from passed format into image/file format
  FPXBaselineColorSpace base = ((PResolutionFlashPix*)fatherSubImage)->GetBaselineSpace();
  FPXBaselineColorSpace used = fatherSubImage->fatherFile->GetUsedColorSpace();
  
  pt = rawPixels + (y0 * this->width) + x0;
  for (i = 0; i < height; i++, pt += this->width, pix += rowOffset)
    ConvertPixelBuffer((unsigned char*)pt, width, used, base);
  // .... PTCH_104 end of addition
  
  // Update amount of fresh pixels and mark pixels as stale
  freshPixels += width*height;
  pixelsStale = true;

  // If everything refreshed (so, we hope thatÐ)
  if (freshPixels >= (this->width * this->height))
    status = WriteTile();

  return status;
}


// ---------------------------------------------------------------------------------
// Compute convolution of a set of pixels in the tile
//
FPXStatus PTile::Convolution (Pixel* pix, long srcWidth, long srcHeight, long quadrant)

{
  Pixel   *pt;
  short   halfTileWidth = (short ) ((fatherSubImage->fatherFile->tileWidth) >> 1);
  short     writtenWidth, writtenHeight;
  FPXStatus status = FPX_OK;

  // Read or allocate the tile if necessary
  if (rawPixels == NULL)
    if ((status = ReadRawPixels()) != FPX_OK)
      return status;;
  assert(rawPixels);
  
  // Compute first pixel of the tile to be written and size to be written
  switch (quadrant) {
    case SW:
      writtenWidth  = MIN(halfTileWidth,width);
      writtenHeight = MIN(halfTileWidth,height);
      pt = rawPixels; 
      break;
    case SE:
      writtenWidth  = width  - halfTileWidth;
      writtenHeight = MIN(halfTileWidth,height);
      pt = rawPixels + halfTileWidth;
      break;
    case NW:
      writtenWidth  = MIN(halfTileWidth,width);
      writtenHeight = height - halfTileWidth;
      pt = rawPixels + (halfTileWidth * width);
      break;
    case NE:
      writtenWidth  = width  - halfTileWidth;
      writtenHeight = height - halfTileWidth;
      pt = rawPixels + (halfTileWidth * width) + halfTileWidth;
      break;
  }

  // Compute convolution : for the moment, 3 convolution algorithms are available
  //    convolution takes alpha channel into account, loops are duplicated to improve speed.
  switch (fatherSubImage->fatherFile->convolution) {
    case Convolution_Cross:
      if (fatherSubImage->identifier <= 2)
        ConvolStandard (pix, srcWidth, srcHeight, pt, width);
      else
        ConvolGaussSimplified (pix, srcWidth, srcHeight, pt, width);
      break;
    case Convolution_Gauss:
      ConvolGaussian4 (pix, srcWidth, srcHeight, pt, width);
      break;
    case Convolution_Standard:
    case Convolution_Other:
    default:
      ConvolStandard (pix, srcWidth, srcHeight, pt, width);
      break;
  }
  
  // Take the extra column into account if any
  if ((writtenWidth > (srcWidth / 2)) && (srcWidth % 2)) {
    Pixel* src  = pix + (srcWidth - 1);
    Pixel* dest = pt  + (writtenWidth - 1);
    for (long i = 0; i < writtenHeight; ++i, dest += width, src  += (2 * srcWidth))
      *dest = *src;
  }

  // Take the extra line into account if any
  if ((writtenHeight > (srcHeight / 2)) && (srcHeight % 2)) {
    Pixel* src  = pix + ((srcHeight - 1) * srcWidth);
    Pixel* dest = pt  + ((writtenHeight - 1) * width);
    for (long i = 0; i < writtenWidth; ++i, ++dest, src += 2)
      *dest = *src;
  }

  // Update amount of fresh pixels
  freshPixels += writtenWidth * writtenHeight;
  
  // If everything refreshed (so, we hope thatÐ)
  if (freshPixels >= (width * height))
    status = WriteTile();                   // Write tile on disk 
  
  return status;
}


// ---------------------------------------------------------------------------------
// Advanced tile writing
//
FPXStatus PTile::WriteTile()
{
  FPXStatus status = FPX_OK;
  
  // Read or allocate the tile if necessary
  if (rawPixels == NULL)
    status = FPX_ERROR;
  else {
    status = Write();   // Write tile on disk
        
    // Decimate if automatic decimation is on
    if ((status == FPX_OK) && fatherSubImage->fatherFile->automaticDecimation)
      status = DecimateTile();
  }
  freshPixels = 0;
  return status;
}


// ---------------------------------------------------------------------------------
// Tile Decimation decision and call
//
FPXStatus PTile::DecimateTile()
{
  FPXStatus   status = FPX_OK;
  Boolean   wasLocked;

  // Read or allocate the tile if necessary
  if (rawPixels == NULL)
    if ((status = ReadRawPixels()) != FPX_OK)
      return status;
  
   // Decimate the tile in the next subimage (if any)
   if ((status == FPX_OK) && fatherSubImage->next) {
     long y = identifier/fatherSubImage->nbTilesW;
     long x = identifier - y*fatherSubImage->nbTilesW;
     wasLocked = IsLocked();
     Lock();
     status = fatherSubImage->next->Convolution (x, y, rawPixels, width, height);
     if (wasLocked == false)
      UnLock();
   }
   
   Free();    // Delete pixels from memory
   return status;
}


// ---------------------------------------------------------------------------------
// Read a set of unmodified, raw pixels from the tile. This would be called in a 
//  read-modify-write application where contrast, filtering and colortwist should
//  not be applied to the data being modified.
//
//  SHOULD THIS HAVE A COLORSPACE ARG???
//
FPXStatus PTile::ReadRectangleRawData (Pixel* pix, long width, long height, long rowOffset, 
          long x0, long y0)
{

  return FPX_UNIMPLEMENTED_FUNCTION;
}


// ---------------------------------------------------------------------------------
// Read a set of display-modified pixels from the tile. This should be called when
//  data is intended for display and should have contrast, filtering and colortwist
//  applied to it.
//
FPXStatus PTile::ReadRectangle (Pixel* pix, long width, long height, long rowOffset, 
          long x0, long y0)
{
  register  Pixel *pt;
  register  long  i;
  FPXStatus     status;
  
  // Read the tile if necessary
  status = Read();
  if (status)       // must exit if read didn't failed
    return status;
  
  // Compute first pixel of the tile to be read
  pt = pixels + (y0 * this->width) + x0;
  
  short plan = Toolkit_ActiveChannel();
  // Copy pixels to the tile
  if (plan == ActiveChannel_All) {
    // Copy pixels to the buffer
    for (i = 0; i < height; i++, pt += this->width, pix += rowOffset)
      BlockMove(pt,pix,width*sizeof(Pixel));
  } else {
    // Pixel by pixel if planes specified
    register long j;
    register unsigned char *src, *dst;
    for (i = 0; i < height; i++, pt += this->width, pix += rowOffset) {
      src = (unsigned char *)(pt)  + plan;
      dst = (unsigned char *)(pix) + plan; 
      for (j = 0; j < width; j++, src += sizeof(Pixel), dst += sizeof(Pixel))
        *dst = *src;
    }
  }
  return status;
}


// ---------------------------------------------------------------------------------
// Write the tile in the disk file. Use compression if necessary. 
//
FPXStatus PTile::Write()
{
  return FPX_OK;
}

// ---------------------------------------------------------------------------------
// Read a tile from the disk file and store it in the "pixels[]" buffer.
// If decompressor is missing, return false and "pixels" is set to a default pattern.
//
FPXStatus PTile::Read()
{
  return FPX_OK;
}


FPXStatus PTile::ReadRawPixels()
{
  return FPX_OK;
}


// ---------------------------------------------------------------------------------
void PTile::AllocDecompress (long size)
{
  // If the desompressor buffer previously allocated is too small
  if (size > decompressSize) {
  
    // Free the old buffer
    FastDeleteArray(decompressBuffer, char);
    
    FastAllocArray(decompressBuffer, int8, size);
    if (decompressBuffer)
      decompressSize = size;
    else
      decompressSize = 0;
  }
}

// ---------------------------------------------------------------------------------
long PTile::PurgeDecompress ()

{
  long size = 0;
  
  if (!decompressLock) {
    FastDeleteArray(decompressBuffer, char);
    decompressBuffer = NULL;
    size = decompressSize;
    decompressSize = 0;
  }
  return size;
}

// ---------------------------------------------------------------------------------
// Inverse alpha channel in tile
//
long PTile::InverseAlpha()

{
  if (pixels && invertLUT) {
    unsigned char* pt = (unsigned char*)(pixels);
    pt += fatherSubImage->alphaOffset;
    long i, j;
    for (i = 0; i < height; i++)
      for (j = 0; j < width; j++, pt+=4)
        *pt = invertLUT[*pt];
  }
  
  return 0;
}

// ---------------------------------------------------------------------------------
// Add a tile in the list of locked tiles
//
void PTile::Lock()
{
  if (!IsLocked()) {
    if (locked == NULL) {
      locked = new PTile*[10];
      if (locked==NULL) {
        return;
      }
    }
    if (indexLocked == 10) {  // there must be only 10 tiles locked at a time
      assert(false);      // erase the first and shift the others
      for (long i = 1; i < indexLocked; i++)
        locked[i-1] = locked[i];
      locked[9] = this;
    } else {
      locked[indexLocked] = this;
      indexLocked++;
    }
  }
}


// ---------------------------------------------------------------------------------
// Delete a tile from the list of locked tiles
//
void PTile::UnLock()

{
  long i,j;
  // search the current tile in the list
  for (i = 0; i < indexLocked; i++) {
    if (this == locked[i])
      break;
  }
  if (i < indexLocked) {
    // shift all the tiles following the current one to delete it from the list
    for (j = i+1; j < indexLocked; j++)
      locked[j-1] = locked[j];
    indexLocked--;
  }
}


// ---------------------------------------------------------------------------------
// test if the current tile is in the list of locked ones
//
Boolean PTile::IsLocked()

{
  if (locked == NULL)
    return false;
  Boolean isLocked = false;
  for (long i = 0; i < indexLocked; i++)
    isLocked = isLocked || (this == locked[i]);
  return isLocked;
}

// ---------------------------------------------------------------------------------
void PTile::ClearStaticArrays()
{
  if (invertLUT) {
    FastDeleteArray(invertLUT, unsigned char);
    invertLUT = NULL;
  }
  
  if (locked) {
    delete [] locked;
    locked = NULL;
    indexLocked = 0;
  }
  
}

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
