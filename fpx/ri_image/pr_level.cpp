//  ----------------------------------------------------------------------------
//  MODULE    : PResolutionLevel 
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 13, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)pr_level.cpp  1.3 15:10:59 14 Apr 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
  #include "pr_level.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#ifndef Memoire_h
  #include  "b_memory.h"
#endif

#ifndef PHierarchicalImage_h
  #include "ph_image.h"
#endif
#ifndef PTile_h
  #include "ptile.h"
#endif
#ifndef PImageFile_h
  #include "pimgfile.h"
#endif

#ifndef Numbers_h
  #include  "numbers.h"
#endif
#ifndef Couleur_h
  #include  "fpx_color.h"
#endif
#ifndef CorrectLut_h
  #include  "corr_lut.h"
#endif
//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Methods of the PResolutionLevel class : this is the Resolution Level object.
//
//  Manage an array on PTile objects which contain pixels.
//  Each PResolutionLevel points to the next PResolutionLevel which has a resolution 
//  1/2 of the current PResolutionLevel.
//  ----------------------------------------------------------------------------


//  ----------------------------------------------------------------------------
// Initialize resolution information and create the next sub-image
// Use this constructor in Create mode
 PResolutionLevel::PResolutionLevel (PHierarchicalImage* father, long width, long height, long* whatImage)
{
  fatherFile  = father;
  posFic      = 0;            // Position at the begining os the file
  identifier  = *whatImage;
  
  register long TILE_WIDTH = fatherFile->tileWidth;
  register long TILE_SHIFT = fatherFile->log2TileWidth;

  // Compute number of tiles in height and width
  nbTilesH    = (short)((height + TILE_WIDTH - 1) >> TILE_SHIFT);
  nbTilesW    = (short)((width  + TILE_WIDTH - 1) >> TILE_SHIFT);
  realHeight  = height;
  realWidth   = width;
  tiles       = NULL;
  
  isAlpha     = false;  // No alpha by default
  premultiplied = false;  // No premultiplication by default (in general... different in Core FlashPix...)
  alphaOffset   = 0;    // Default is aRGB, thus no offset...

  currentLine = 0;
  
  next      = NULL;
  
  // Make a test to create or not the next sub image
  if ((realHeight > TILE_WIDTH) || (realWidth > TILE_WIDTH))
    imageStatus = 0;
  else 
    imageStatus = -1;
    
  // Increment sub-image number "whatImage"
  (*whatImage)++;
}


//  ----------------------------------------------------------------------------
// Initialize resolution information and create the next sub-image
// Use this constructor in Read or Write mode
PResolutionLevel::PResolutionLevel (PHierarchicalImage* father, long* offset, long id)
{
  fatherFile  = father;
  posFic    = *offset;
  identifier  = id;

  nbTilesH  = 0;
  nbTilesW  = 0;
  realHeight  = 0;
  realWidth   = 0;
  tiles   = NULL;

  isAlpha     = false;  // No alpha by default
  premultiplied = false;  // No premultiplication by default (in general... different in Core FlashPix...)
  alphaOffset   = 0;    // Default is aRGB, thus no offset...

  currentLine = 0;
  imageStatus = 0;

  next    = NULL;
}


//  ----------------------------------------------------------------------------
// Clean resolution level and delete all related objects
PResolutionLevel::~PResolutionLevel()
{
  if (next) {
    delete next;
    next = NULL;
  }
}

//  ----------------------------------------------------------------------------
void PResolutionLevel::InitWhenCreate()
{
}

//  ----------------------------------------------------------------------------
void PResolutionLevel::InitWhenReading()
{
}

//  ----------------------------------------------------------------------------
PResolutionLevel* PResolutionLevel::Previous()
{
  PResolutionLevel* previous = NULL;
  if (identifier != 0)
    previous = fatherFile->subImages[identifier-1];
  return previous;
}


//  ----------------------------------------------------------------------------
// Allocate resolution levels array and compute resolution information
FPXStatus PResolutionLevel::Allocation ()
{
  register long TILE_WIDTH = fatherFile->tileWidth;
  register long TILE_SHIFT = fatherFile->log2TileWidth;
  register long TILE_MASK  = fatherFile->maskTileWidth;

  short   i;
  short   j;
  FPXStatus status = FPX_OK;

  // Update level infos if necessary: this happen only in rare case like when regenerating a flat file
  if (!nbTilesH || !nbTilesW) {
    PResolutionLevel* previous = Previous();
    long width  = previous->realWidth;
    long height = previous->realHeight;
    width  = (width  + 1) / 2;
    height = (height + 1) / 2;
    nbTilesH    = (short)((height + TILE_WIDTH - 1) >> TILE_SHIFT);
    nbTilesW    = (short)((width  + TILE_WIDTH - 1) >> TILE_SHIFT);
    realHeight  = height;
    realWidth   = width;
  }

  // Allocate tiles array and init each tile

  if (nbTilesH && nbTilesW) {

    if ((status = AllocTilesArray()) != FPX_OK)
      return status;
    if (tiles) {
      PTile* tile = tiles;
      long index = 0;
      for (i = 0; i < (nbTilesH-1); i++, tile++) {
        for (j = 0; j < (nbTilesW-1); j++, tile++)
          tile->InitializeCreate(this,TILE_WIDTH,TILE_WIDTH,index++);             // Init each tile of the line
        tile->InitializeCreate(this,((realWidth - 1) & TILE_MASK)+1,TILE_WIDTH,index++);    // Init last tile of the line
      }
      for (j = 0; j < (nbTilesW-1); j++, tile++)
        tile->InitializeCreate(this,TILE_WIDTH,((realHeight - 1) & TILE_MASK)+1,index++);         // Init last line of tiles
      tile->InitializeCreate(this,((realWidth - 1) & TILE_MASK)+1,((realHeight - 1) & TILE_MASK)+1,index++);  // Init last tile
    } else {
      realHeight = 0;
      realWidth  = 0;
      nbTilesH   = 0;
      nbTilesW   = 0;
    }
  } else {
    // If we can't allocate the tiles array, invalid the sub image record
    tiles = NULL;
    realHeight = 0;
    realWidth  = 0;
    nbTilesH   = 0;
    nbTilesW   = 0;
  }
  
  return status;
}


//  ----------------------------------------------------------------------------
Boolean PResolutionLevel::HasBeenUsed ()
{
  return (tiles != NULL);
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::AllocTilesArray ()
{
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Inverse alpha channel in all opened display tiles
FPXStatus PResolutionLevel::InverseAlpha()
{
  if (!HasBeenUsed())
    return FPX_OK;
    
  register long i = nbTilesH * nbTilesW;
  register PTile* tile = tiles;
  
  while (i--)
    tile++->InverseAlpha();
    
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Read a resolution level and initialize the buffer 'pixels'
FPXStatus PResolutionLevel::Read ()
{
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Write the tile header of a sub-image :
FPXStatus PResolutionLevel::Write()
{
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Write all remaining buffers of all sub-images
// CAUTION : recursive function ...
FPXStatus PResolutionLevel::CloseSubImage ()
{
  FPXStatus status = FPX_OK;
  
  if ((fatherFile->mode != mode_Lecture) && HasBeenUsed()) {
  
    // Write all remaining buffers of all sub-images 
    FlushModifiedTiles();
    
    // Write the tile header of a sub-image :
    if (status == FPX_OK)
      status = Write();
  }
  if (next)
    status = next->CloseSubImage();
  
  return status;
}


//  ----------------------------------------------------------------------------
// Write a rectangle in a resolution level
FPXStatus PResolutionLevel::WriteRectangle (long x0, long y0, long x1, long y1, Pixel* pix, short plan)
{
  FPXStatus status = FPX_OK;
  long    x, y, X, Y, X0 = 0, Y0 = 0;
  PTile   *tile;
  Pixel   *cur_pix;
  long    h, w;
  long    rowOffset = x1 - x0 + 1;
    
  register long TILE_WIDTH  = fatherFile->tileWidth;
  register long TILE_SHIFT  = fatherFile->log2TileWidth;
  register long TILE_MASK   = fatherFile->maskTileWidth;

  // Test input values
  if ((x1 < x0) || (y1 < y0))
    return FPX_BAD_COORDINATES;       // Inconsistent bounds of rectangle
  if ((x0 >= realWidth) || (y0 >= realHeight) || (x1 < 0) || (y1 < 0))
    return FPX_BAD_COORDINATES;       // Attempt to write outside the image
    
  // Clipping
  if (x0 < 0) {
    pix -= x0; x0 = 0;
  }
  if (y0 < 0) {
    pix -= y0*rowOffset; y0 = 0;
  }
  if (x1 >= realWidth)
    x1 = realWidth - 1;
  if (y1 >= realHeight)
    y1 = realHeight - 1;

  X0 = x0 & (~TILE_MASK);
  Y0 = y0 & (~TILE_MASK);
  
  // PTCH_105 - added following to implement progress callbacks....
  long  totalTileCount, tilesWritten;
  totalTileCount = ((x1 - x0 + TILE_WIDTH - 1) >> TILE_SHIFT) *
           ((y1 - y0 + TILE_WIDTH - 1) >> TILE_SHIFT);
  tilesWritten = 0;
  // PTCH_105 - ....end of addition
  
  // Write on each tile
  Y = Y0;
  y = y0 & TILE_MASK;
  h = ((y1-Y+1) >= TILE_WIDTH ? TILE_WIDTH - y : (y1-y0+1));

  while (Y <= y1) {
    X = X0;
    x = x0 & TILE_MASK;
    w = ((x1-X+1) >= TILE_WIDTH ? TILE_WIDTH - x : (x1-x0+1));
    cur_pix = pix;
    tile = tiles + (Y>>TILE_SHIFT)*nbTilesW + (X>>TILE_SHIFT);
    while (X <= x1) {
      status = tile->WriteRectangle (cur_pix, w, h, rowOffset, x, y, plan);
      if (status) // stop the writing if one tile couldn't be writen
        return status;
      tile++;
      cur_pix += w;
      x = 0;
      X += TILE_WIDTH;
      w = ((x1-X+1) >= TILE_WIDTH ? TILE_WIDTH : (x1-X+1));
      
      // PTCH_105 - added following to implement progress callbacks....
      if ( GtheSystemToolkit->fnctProgFunc) {
         tilesWritten++;
         if (GtheSystemToolkit->fnctProgFunc( totalTileCount, tilesWritten))
          return FPX_USER_ABORT;
      }
      // PTCH_105 - ....end of addition
    }
    pix += rowOffset * h;
    y = 0;
    Y += TILE_WIDTH;
    h = ((y1-Y+1) >= TILE_WIDTH ? TILE_WIDTH : (y1-Y+1));
  }
  
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::FlushModifiedTiles()
{
  FPXStatus status = FPX_OK;

  if (tiles) {
    long n = nbTilesH * nbTilesW;
    for ( long i = 0; (i < n) && (status == FPX_OK); i++) {
      if (tiles[i].HasFreshPixels())
        status = tiles[i].WriteTile();
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Just call the decimation function for each tile of the level
FPXStatus PResolutionLevel::DecimateLevel ()
{
  FPXStatus status  = FPX_OK;
  
  if (tiles) {
    long n = nbTilesH * nbTilesW;
    for (long i = 0; (i < n) && (status == FPX_OK); ++i)
      status = tiles[i].DecimateTile();
  } else
    status = FPX_ERROR;
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::ReadRectangle (long x0, long y0, long x1, long y1, Pixel* pix)
{
  FPXStatus status = FPX_OK;
  FPXStatus currentStatus = FPX_OK;
  long    x, y, X, Y, X0 = 0, Y0 = 0;
  PTile   *tile;
  Pixel   *cur_pix = pix;
  long    h, w;
  long    rowOffset = x1 - x0 + 1;

  register long TILE_WIDTH  = fatherFile->tileWidth;
  register long TILE_SHIFT  = fatherFile->log2TileWidth;
  register long TILE_MASK   = fatherFile->maskTileWidth;
       Pixel BACKGROUND   = fatherFile->backgroundUsed;

  // Test input values
  if ((x1 < x0) || (y1 < y0))
    return FPX_BAD_COORDINATES;       // Inconsistent bounds of rectangle

// VISU2 "obj_SubImageIO::ReadRectangle : x0 = %d, y0 = %d, x1 = %d, y1 = %d\n", x0, y0, x1, y1 FIN

  // Compute cropping for this scale :
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = fatherFile->cropX1 >> identifier;
  long cropy1 = fatherFile->cropY1 >> identifier;

  // Take cropping into account : px and py are given in the cropped coordinates => translate first
  x0 += cropx0;
  y0 += cropy0;
  x1 += cropx0;
  y1 += cropy0;
    
  if ((x0 >= cropx1) || (y0 >= cropy1) || (x1 < cropx0) || (y1 < cropy0)) {
    for (y = y0; y <= y1; y++)
      for (x = x0; x <= x1; x++)
        *pix++ = BACKGROUND;
    return FPX_BAD_COORDINATES;       // Attempt to read outside the image
  }
    
  // Clipping : if clipping
  if ((x1 >= cropx1) || (y1 >= cropy1) || (x0 < cropx0) || (y0 < cropy0)) {
    // Init rectangle with BACKGROUND color
    for (y = y0; y <= y1; y++)
      for (x = x0; x <= x1; x++)
        *cur_pix++ = BACKGROUND;
    if (x0 < cropx0) {
      pix += (cropx0 - x0);
      x0 = cropx0;
    }
    if (y0 < cropy0) {
      pix += (cropy0 - y0)*rowOffset;
      y0 = cropy0;
    }
    if (x1 >= cropx1)
      x1 = cropx1-1;
    if (y1 >= cropy1)
      y1 = cropy1-1;
  }

  X0 = x0 & (~TILE_MASK);
  Y0 = y0 & (~TILE_MASK);
  
  // Read each tile and copy pixels to output rectangle
  Y = Y0;
  y = y0 & TILE_MASK;
  h = ((y1-Y+1) >= TILE_WIDTH ? TILE_WIDTH - y : (y1-y0+1));
  while (Y <= y1) {
    X = X0;
    x = x0 & TILE_MASK;
    w = ((x1-X+1) >= TILE_WIDTH ? TILE_WIDTH - x : (x1-x0+1));
    cur_pix = pix;
    tile = tiles + (Y>>TILE_SHIFT)*nbTilesW + (X>>TILE_SHIFT);
    while (X <= x1) {
      // read a rectangle in a tile, if a fatal error occur stop the loop
      if ((currentStatus = tile->ReadRectangle (cur_pix, w, h, rowOffset, x, y))) 
        if (currentStatus == FPX_MEMORY_ALLOCATION_FAILED)
          return currentStatus;     // Stop if no memory available
        else                // otherwise
          status = currentStatus;     // save the error for return later
        
      tile++;
      cur_pix += w;
      x = 0;
      X += TILE_WIDTH;
      w = ((x1-X+1) >= TILE_WIDTH ? TILE_WIDTH : (x1-X+1));
    }
    pix += rowOffset * h;
    y = 0;
    Y += TILE_WIDTH;
    h = ((y1-Y+1) >= TILE_WIDTH ? TILE_WIDTH : (y1-Y+1));
  }
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::Convolution (long x, long y, Pixel* pix, long width, long height)
{
  // CAUTION : no line or column is allocated for tiles of one line or one column
  // They contribution to the convoluted image is 0 -> skip these tiles
  if ((width <= 1) || (height <= 1))
    return FPX_OK;
  
  long numTile  = ((y>>1)*nbTilesW) + (x>>1);
  long quadrant = (x&0x01) | ((y&0x01)<<1);
  return tiles[numTile].Convolution (pix, width, height, quadrant);
}


//  ----------------------------------------------------------------------------
// Read a rectangular area of a sub-image. Read on disk if necessary.
// If decompressor is missing, return false and fill "table[]" with "BACKGROUND".
//
// Returns an FPX_ error (e.g., FPX_OK, etc.)
//
FPXStatus PResolutionLevel::Read(long* px, long* py, Pixel* table)
{
  register long TILE_WIDTH  = fatherFile->tileWidth;
  register long TILE_SHIFT  = fatherFile->log2TileWidth;
  register long TILE_MASK   = fatherFile->maskTileWidth;
       Pixel BACKGROUND = fatherFile->backgroundUsed;
       Boolean useAlpha = fatherFile->useAlphaChannel | (isAlpha & premultiplied);
  unsigned char alphaOffset = fatherFile->alphaOffset;

  PTile   *tile;
  FPXStatus readStatus, status = FPX_OK;
  int     i;
  
  // Compute cropping for this scale :
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = fatherFile->cropX1 >> identifier;
  long cropy1 = fatherFile->cropY1 >> identifier;

  // ----- if table[] clip the edge of the image ----- //
  
  if ((px[P00] >= cropx1) || (py[P00] >= cropy1) || (px[P00] < cropx0) || (py[P00] < cropy0) ||
    (px[P30] >= cropx1) || (py[P30] >= cropy1) || (px[P30] < cropx0) || (py[P30] < cropy0) ||
    (px[P03] >= cropx1) || (py[P03] >= cropy1) || (px[P03] < cropx0) || (py[P03] < cropy0) ||
    (px[P33] >= cropx1) || (py[P33] >= cropy1) || (px[P33] < cropx0) || (py[P33] < cropy0)) {
    
    if (useAlpha && PTile::invertLUT) {
      short alpha, beta;
      unsigned char* srcPix;
      unsigned char* desPix;
      long index;
      for (i = 0; i < 16; ++i, ++table) {
        if ((px[i] < cropx1) && (py[i] < cropy1) && (px[i] >= cropx0) && (py[i] >= cropy0)) {
          tile = tiles + (py[i] >> TILE_SHIFT) * nbTilesW + (px[i] >> TILE_SHIFT);
          if ((readStatus = tile->Read()))
            status = readStatus;
          if (!tile->pixels) {      // If error reading data
            *table = BACKGROUND;
//          VISU2 "obj_SubImageIO::Read, read error 1 => BACKGROUND\n" FIN
          } else {
            srcPix = (unsigned char*)(tile->pixels + ((px[i] & TILE_MASK) + (py[i] & TILE_MASK) * tile->width));
            alpha = *(srcPix + alphaOffset);
            beta  = PTile::invertLUT[alpha];
            desPix = (unsigned char*)(table);
            if (premultiplied)
              for (index = 4; index; --index, ++desPix, ++srcPix)
                *desPix = (unsigned char)((*desPix*beta)/0xFF + *srcPix);
            else
              for (index = 4; index; --index, ++desPix, ++srcPix)
                *desPix = (unsigned char)((*desPix*beta + *srcPix*alpha)/0xFF);
          }
        }
      }
    } else {
      for(i = 0; i < 16; ++i, ++table) {
        if ((px[i] < cropx1) && (py[i] < cropy1) && (px[i] >= cropx0) && (py[i] >= cropy0)) {
          tile = tiles + (py[i] >> TILE_SHIFT) * nbTilesW + (px[i] >> TILE_SHIFT);
          if ((readStatus = tile->Read()))
            status = readStatus;
          if (!tile->pixels) {      // If error reading data
            *table = BACKGROUND;
//          VISU2 "obj_SubImageIO::Read, read error 1 => BACKGROUND\n" FIN
          }
          else
            *table = tile->pixels[(px[i] & TILE_MASK) + (py[i] & TILE_MASK) * tile->width];
        } else if (Toolkit_WriteOnBackground())
          *table = BACKGROUND;
      }
    }
  } else {
  
    long p = px[P00] >> TILE_SHIFT;
    long q = py[P00] >> TILE_SHIFT;

    // ----- table[] meets several tiles boundaries -----//

    if (((px[P03] >> TILE_SHIFT) != p) || ((py[P03] >> TILE_SHIFT) != q) ||
      ((px[P30] >> TILE_SHIFT) != p) || ((py[P30] >> TILE_SHIFT) != q) ||
      ((px[P33] >> TILE_SHIFT) != p) || ((py[P33] >> TILE_SHIFT) != q)) {
      if (useAlpha && PTile::invertLUT) {
        short alpha, beta;
        unsigned char* srcPix;
        unsigned char* desPix;
        long index;
        for(i = 0; i < 16; ++i, ++table) {
          tile = tiles + (py[i] >> TILE_SHIFT) * nbTilesW + (px[i] >> TILE_SHIFT);
          if ((readStatus = tile->Read()))
            status = readStatus;
          if (!tile->pixels) {    // If error reading data
            *table = BACKGROUND;
//            VISU2 "obj_SubImageIO::Read, read error 2 => BACKGROUND\n" FIN
          }
          else {
            srcPix = (unsigned char*)(tile->pixels + ((px[i] & TILE_MASK) + (py[i] & TILE_MASK) * tile->width));
            alpha = *(srcPix + alphaOffset);
            beta  = PTile::invertLUT[alpha];
            desPix = (unsigned char*)(table);
            if (premultiplied)
              for (index = 4; index; --index, ++desPix, ++srcPix)
                *desPix = (unsigned char)((*desPix*beta)/0xFF + *srcPix);
            else
              for (index = 4; index; --index, ++desPix, ++srcPix)
                *desPix = (unsigned char)((*desPix*beta + *srcPix*alpha)/0xFF);
          }
        }
      } else {
        for(i = 0; i < 16; ++i, ++table) {
          tile = tiles + (py[i] >> TILE_SHIFT) * nbTilesW + (px[i] >> TILE_SHIFT);
          if ((readStatus = tile->Read()))
            status = readStatus;
          if (!tile->pixels) {    // If error reading data
            *table = BACKGROUND;
//            VISU2 "obj_SubImageIO::Read, read error 2 => BACKGROUND\n" FIN
          }
          else
            *table = tile->pixels[(px[i] & TILE_MASK) + (py[i] & TILE_MASK) * tile->width];
        }
      }
    } else {
    
    // ----- table[] is included within a single tile -> Optimization of the reading loop -----//
  
      tile = tiles + q * nbTilesW + p;
      if ((readStatus = tile->Read()))
        status = readStatus;
      if (!tile->pixels) {      // If error reading data
        for (i = 0; i < 16; ++i, ++table)
          *table = BACKGROUND;
//          VISU2 "obj_SubImageIO::Read, read error 3 => BACKGROUND\n" FIN
      }
      else {              // most common case
        Pixel* pix = tile->pixels;
        long l = tile->width;
        
        if (l == TILE_WIDTH) {

        // --- If the tile Width is the tileWidth -> use fast acces algorithm ---//
        
          if (useAlpha && PTile::invertLUT) {
            short alpha, beta;
            unsigned char* srcPix;
            unsigned char* desPix;
            if (premultiplied) {
              for (i = 0; i < 16; ++i, ++table) {
                srcPix = (unsigned char*)(pix + ((px[i] & TILE_MASK) + ((py[i] & TILE_MASK) << TILE_SHIFT)));
                alpha  = *(srcPix + alphaOffset);
                beta   = PTile::invertLUT[alpha];
                desPix = (unsigned char*)(table);
                for (long index = 4; index; --index, ++desPix, ++srcPix)
                  *desPix = (unsigned char)((*desPix*beta)/0xFF + *srcPix);
              }
            } else {
              for (i = 0; i < 16; ++i, ++table) {
                srcPix = (unsigned char*)(pix + ((px[i] & TILE_MASK) + ((py[i] & TILE_MASK) << TILE_SHIFT)));
                alpha  = *(srcPix + alphaOffset);
                beta   = PTile::invertLUT[alpha];
                desPix = (unsigned char*)(table);
                for (long index = 4; index; --index, ++desPix, ++srcPix)
                  *desPix = (unsigned char)((*desPix*beta + *srcPix*alpha)/0xFF);
              }
            }
          } else {
            for (i = 0; i < 16; ++i, ++table)
              *table = pix[(px[i] & TILE_MASK) + ((py[i] & TILE_MASK) << TILE_SHIFT)];
          }
        } else {
  
        // --- If table[] is in the last tile of the line. ---//

          if (useAlpha && PTile::invertLUT) {
            short alpha, beta;
            unsigned char* srcPix;
            unsigned char* desPix;
            if (premultiplied) {
              for (i = 0; i < 16; ++i, ++table) {
                srcPix = (unsigned char*)(pix + ((px[i] & TILE_MASK) + (py[i] & TILE_MASK) * l));
                alpha  = *(srcPix + alphaOffset);
                beta   = PTile::invertLUT[alpha];
                desPix = (unsigned char*)(table);
                for (long index = 4; index; --index, ++desPix, ++srcPix)
                  *desPix = (unsigned char)((*desPix*beta)/0xFF + *srcPix);
              }
            } else {
              for (i = 0; i < 16; ++i, ++table) {
                srcPix = (unsigned char*)(pix + ((px[i] & TILE_MASK) + (py[i] & TILE_MASK) * l));
                alpha  = *(srcPix + alphaOffset);
                beta   = PTile::invertLUT[alpha];
                desPix = (unsigned char*)(table);
                for (long index = 4; index; --index, ++desPix, ++srcPix)
                  *desPix = (unsigned char)((*desPix*beta + *srcPix*alpha)/0xFF);
              }
            }
          } else {
            for (i = 0; i < 16; ++i, ++table)
              *table = pix[(px[i] & TILE_MASK) + (py[i] & TILE_MASK) * l];
          }
        }
      }
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::ReadInterpolated (long* px, long* py, Pixel* table)
{
  register long TILE_SHIFT  = fatherFile->log2TileWidth;
  register long TILE_MASK   = fatherFile->maskTileWidth;
       Pixel  BACKGROUND = fatherFile->backgroundUsed;

  PTile     * tile;
  long      ax, ay, bx, by, i, j, xi, yi, x0, y0, x1, y1, n, c0, c1, c2, c3, index;
  Pixel     *pix0, *pix1, *pix2, *pix3;

  Boolean     alphaPresent= fatherFile->existAlphaChannel;
  Boolean     useAlpha  = fatherFile->useAlphaChannel | (isAlpha & premultiplied);
  unsigned char   alphaOffset = fatherFile->alphaOffset;
  FPXStatus     status    = FPX_OK;
  
  // Compute cropping for this scale :
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = fatherFile->cropX1 >> identifier;
  long cropy1 = fatherFile->cropY1 >> identifier;

  if (px[P00] > px[P30]) {
    ax = px[P30];
    bx = px[P00];
  } else {
    bx = px[P30];
    ax = px[P00];
  }
  if (px[P03] > px[P33]) {
    x0 = px[P33];
    x1 = px[P03];
  } else {
    x1 = px[P33];
    x0 = px[P03];
  }
  if (ax > x0) ax = x0;
  if (bx < x1) bx = x1;


  if (py[P00] > py[P30]) {
    ay = py[P30];
    by = py[P00];
  } else {
    by = py[P30];
    ay = py[P00];
  }
  if (py[P03] > py[P33]) {
    y0 = py[P33];
    y1 = py[P03];
  } else {
    y1 = py[P33];
    y0 = py[P03];
  }
  if (ay > y0) ay = y0;
  if (by < y1) by = y1;


  ax = (ax - 2) >> 8;
  ay = (ay - 2) >> 8;
  bx = (bx + 257) >> 8;
  by = (by + 257) >> 8;

  xi = ax >> TILE_SHIFT;
  yi = ay >> TILE_SHIFT;

  if ((ax < cropx0) || (ay < cropy0) || (bx >= cropx1) || (by >= cropy1) ||
      (xi != (bx >> TILE_SHIFT))  || (yi != (by >> TILE_SHIFT))) {
    FPXStatus readStatus = FPX_OK;
    for (index = 0; index < 16; index++)
      if( (readStatus = ReadMeanInterpolated (px[index], py[index], table[index])) )
        status = readStatus;    
    return (status);
  }

  tile = tiles + yi * nbTilesW + xi;
  status = tile->Read();
  if (status) {             // If error reading on disk and pixels not created
    *table = BACKGROUND;
//    VISU2 "obj_SubImageIO::ReadInterpolated, read error without clipping => BACKGROUND\n" FIN
    return (status);
  }

  if (useAlpha && PTile::invertLUT) {
    short alpha, beta;
    Pixel srcPix;
    for (index = 0; index < 16; ++index, ++table) { 
      xi = px[index];
      yi = py[index];
    
      x0 = xi >> 8;
      y0 = yi >> 8;
      
      n = (i = (x0 & TILE_MASK)) + (j = (y0 & TILE_MASK)) * tile->width;
  
      pix0 = tile->pixels + n;
      pix1 = pix0 + 1;
      pix2 = pix0 + tile->width;
      pix3 = pix2 + 1;

      ax = xi & 0xff;
      ay = yi & 0xff;
      bx = 256 - ax;  
      by = 256 - ay;  

      c0 = by * bx; 
      c1 = by * ax; 
      c2 = ay * bx; 
      c3 = ay * ax; 
    
      srcPix.alpha = (unsigned char)((c0 * pix0->alpha + c1 * pix1->alpha + c2 * pix2->alpha + c3 * pix3->alpha) >> 16);
      srcPix.rouge = (unsigned char)((c0 * pix0->rouge + c1 * pix1->rouge + c2 * pix2->rouge + c3 * pix3->rouge) >> 16); 
      srcPix.vert  = (unsigned char)((c0 * pix0->vert + c1 * pix1->vert + c2 * pix2->vert + c3 * pix3->vert) >> 16); 
      srcPix.bleu  = (unsigned char)((c0 * pix0->bleu + c1 * pix1->bleu + c2 * pix2->bleu + c3 * pix3->bleu) >> 16);
      
      unsigned char* sPix = (unsigned char*)(&srcPix);
      unsigned char* dPix = (unsigned char*)(table);
      alpha      = *(sPix + alphaOffset);
      beta       = PTile::invertLUT[alpha];
      
      long index2;
      
      if (premultiplied)
        for (index2 = 4; index2; --index2, ++dPix, ++sPix)
          *dPix = (unsigned char)(*dPix*beta/0xFF + *sPix);
      else
        for (index2 = 4; index2; --index2, ++dPix, ++sPix)
          *dPix = (unsigned char)((*dPix*beta + *sPix*alpha)/0xFF);
    }
  } else {
    for (index = 0; index < 16; ++index, ++table) { 
      xi = px[index];
      yi = py[index];
    
      x0 = xi >> 8;
      y0 = yi >> 8;
      
      n = (i = (x0 & TILE_MASK)) + (j = (y0 & TILE_MASK)) * tile->width;
  
      pix0 = tile->pixels + n;
      pix1 = pix0 + 1;
      pix2 = pix0 + tile->width;
      pix3 = pix2 + 1;

      ax = xi & 0xff;
      ay = yi & 0xff;
      bx = 256 - ax;  
      by = 256 - ay;  

      c0 = by * bx; 
      c1 = by * ax; 
      c2 = ay * bx; 
      c3 = ay * ax; 
    
      table->rouge = (unsigned char)((c0 * pix0->rouge + c1 * pix1->rouge + c2 * pix2->rouge + c3 * pix3->rouge) >> 16); 
      table->vert = (unsigned char)((c0 * pix0->vert + c1 * pix1->vert + c2 * pix2->vert + c3 * pix3->vert) >> 16); 
      table->bleu = (unsigned char)((c0 * pix0->bleu + c1 * pix1->bleu + c2 * pix2->bleu + c3 * pix3->bleu) >> 16); 
      if (alphaPresent)
        table->alpha = (unsigned char)((c0 * pix0->alpha + c1 * pix1->alpha + c2 * pix2->alpha + c3 * pix3->alpha) >> 16);
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::ReadMeanInterpolated (long xi, long yi, Pixel& pixel)
{
  PTile     *tile;
  long      x0, y0;
  Boolean     WriteOnBackground = Toolkit_WriteOnBackground();
  Boolean     useAlpha = fatherFile->useAlphaChannel | (isAlpha & premultiplied);
  FPXStatus   status = FPX_OK;
  
  register long  TILE_SHIFT = fatherFile->log2TileWidth;
  register long  TILE_MASK  = fatherFile->maskTileWidth;
       Pixel BACKGROUND = fatherFile->backgroundUsed;
  unsigned char alphaOffset = fatherFile->alphaOffset;

  x0 = xi >> 8;
  y0 = yi >> 8;
    
  // Compute cropping for this scale :
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = fatherFile->cropX1 >> identifier;
  long cropy1 = fatherFile->cropY1 >> identifier;

  if ((x0 >= cropx1) || (y0 >= cropy1) || (x0 < cropx0) || (y0 < cropy0) || (xi < -255) || (yi < -255)) {
    if (WriteOnBackground && !useAlpha)
      pixel = BACKGROUND;
  }
  else {
    long      ax, ay, bx, by, i, j, x1, y1, n, c0, c1, c2, c3;
    Pixel     pix0, pix1, pix2, pix3;


    if ((xi >= 0) && (yi >= 0)) {
      tile = tiles + (y0 >> TILE_SHIFT) * nbTilesW + (x0 >> TILE_SHIFT);
      status = tile->Read();
      if (status) {         // If error reading on disk and pixels not created
        pixel = BACKGROUND;
//        VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 1 => BACKGROUND\n" FIN
        return (status);
      }
        
      pix0 = tile->pixels[n = (i = (x0 & TILE_MASK)) + (j = (y0 & TILE_MASK)) * tile->width];
    
      if ((i == tile->width - 1) || (j == tile->height - 1)) {
        x1 = x0 + 1;
        y1 = y0 + 1;
        if (x1 < realWidth) {
          tile = tiles + (y0 >> TILE_SHIFT) * nbTilesW + (x1 >> TILE_SHIFT);
          status = tile->Read();
          if (status) {     // If error reading on disk and pixels not created
            pixel = BACKGROUND;
//            VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 2 => BACKGROUND\n" FIN
            return (status);
          }
          pix1 = tile->pixels[(x1 & TILE_MASK) + (y0 & TILE_MASK) * tile->width];
        } else {
          if (WriteOnBackground) pix1 = BACKGROUND;
          else pix1 = pixel;
        }
        if (y1 < realHeight) {
          tile = tiles + (y1 >> TILE_SHIFT) * nbTilesW + (x0 >> TILE_SHIFT);
          status = tile->Read();
          if (status) {     // If error reading on disk and pixels not created
            pixel = BACKGROUND;
//            VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 3 => BACKGROUND\n" FIN
            return (status);
          }
          pix2 = tile->pixels[(x0 & TILE_MASK) + (y1 & TILE_MASK) * tile->width];
        } else {
          if (WriteOnBackground) pix2 = BACKGROUND;
          else pix2 = pixel;
        }
  
        if ((x1 < realWidth) && (y1 < realHeight)) {
          tile = tiles + (y1 >> TILE_SHIFT) * nbTilesW + (x1 >> TILE_SHIFT);
          status = tile->Read();
          if (status) {     // If error reading on disk and pixels not created
            pixel = BACKGROUND;
//            VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 4 => BACKGROUND\n" FIN
            return (status);
          }
          pix3 = tile->pixels[(x1 & TILE_MASK) + (y1 & TILE_MASK) * tile->width];
        } else {
          if (WriteOnBackground) pix3 = BACKGROUND;
          else pix3 = pixel;
        }
      }
      else {
        pix1 = tile->pixels[n + 1];
        pix2 = tile->pixels[n += tile->width];
        pix3 = tile->pixels[n + 1];
      }
      ax = xi & 0xff;
      ay = yi & 0xff;
    }
    else {
      if (WriteOnBackground) pix0 = BACKGROUND;
      else pix0 = pixel;

      if (xi < 0) {
        x0 = -1;
        ax = 256 + xi;
      } else ax = xi & 0xff;
        
      if (yi < 0) {
        y0 = -1;
        ay = 256 + yi;
      } else ay = yi & 0xff;
      
      
      x1 = x0 + 1;
      y1 = y0 + 1;

      if ((x1 < realWidth) && (y0 >= 0)) {
        tile = tiles + (y0 >> TILE_SHIFT) * nbTilesW + (x1 >> TILE_SHIFT);
        status = tile->Read();
        if (status) {       // If error reading on disk and pixels not created
          pixel = BACKGROUND;
//          VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 5 => BACKGROUND\n" FIN
          return (status);
        }
        pix1 = tile->pixels[(x1 & TILE_MASK) + (y0 & TILE_MASK) * tile->width];
      } else {
        if (WriteOnBackground) pix1 = BACKGROUND;
        else pix1 = pixel;
      }
      
      if ((y1 < realHeight) && (x0 >= 0)) {
        tile = tiles + (y1 >> TILE_SHIFT) * nbTilesW + (x0 >> TILE_SHIFT);
        status = tile->Read();
        if (status) { // If error reading on disk and pixels not created
          pixel = BACKGROUND;
//          VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 6 => BACKGROUND\n" FIN
          return (status);
        }
        pix2 = tile->pixels[(x0 & TILE_MASK) + (y1 & TILE_MASK) * tile->width];
      } else {
        if (WriteOnBackground) pix2 = BACKGROUND;
        else pix2 = pixel;
      }
      
      if ((x1 < realWidth) && (y1 < realHeight)) {
        tile = tiles + (y1 >> TILE_SHIFT) * nbTilesW + (x1 >> TILE_SHIFT);
        status = tile->Read();
        if (status) {       // If error reading on disk and pixels not created
          pixel = BACKGROUND;
//          VISU2 "obj_SubImageIO::ReadMeanInterpolated, read error 7 => BACKGROUND\n" FIN
          return (status);
        }
        pix3 = tile->pixels[(x1 & TILE_MASK) + (y1 & TILE_MASK) * tile->width];
      } else {
        if (WriteOnBackground) pix3 = BACKGROUND;
        else pix3 = pixel;
      }
    }
    
    bx = 256 - ax;  
    by = 256 - ay;  

    c0 = by * bx; 
    c1 = by * ax; 
    c2 = ay * bx; 
    c3 = ay * ax; 
    
    if (useAlpha && PTile::invertLUT) {
      short alpha, beta;
      Pixel srcPix;
      long index;
      
      srcPix.alpha = (unsigned char)((c0 * pix0.alpha + c1 * pix1.alpha + c2 * pix2.alpha + c3 * pix3.alpha) >> 16);
      srcPix.rouge = (unsigned char)((c0 * pix0.rouge + c1 * pix1.rouge + c2 * pix2.rouge + c3 * pix3.rouge) >> 16); 
      srcPix.vert  = (unsigned char)((c0 * pix0.vert  + c1 * pix1.vert  + c2 * pix2.vert  + c3 * pix3.vert)  >> 16); 
      srcPix.bleu  = (unsigned char)((c0 * pix0.bleu  + c1 * pix1.bleu  + c2 * pix2.bleu  + c3 * pix3.bleu)  >> 16);
      
      
      unsigned char* sPix = (unsigned char*)(&srcPix);
      unsigned char* dPix = (unsigned char*)(&pixel);
      alpha      = *(sPix + alphaOffset);
      beta       = PTile::invertLUT[alpha];
      
      if (premultiplied)
        for (index = 4; index; --index, ++dPix, ++sPix)
          *dPix = (unsigned char)((*dPix*beta)/0xFF + *sPix);
      else
        for (index = 4; index; --index, ++dPix, ++sPix)
          *dPix = (unsigned char)((*dPix*beta + *sPix*alpha)/0xFF);
    } else {
      pixel.rouge = (unsigned char)((c0 * pix0.rouge + c1 * pix1.rouge + c2 * pix2.rouge + c3 * pix3.rouge) >> 16); 
      pixel.vert  = (unsigned char)((c0 * pix0.vert  + c1 * pix1.vert  + c2 * pix2.vert  + c3 * pix3.vert)  >> 16); 
      pixel.bleu  = (unsigned char)((c0 * pix0.bleu  + c1 * pix1.bleu  + c2 * pix2.bleu  + c3 * pix3.bleu)  >> 16); 
      if (fatherFile->existAlphaChannel)
        pixel.alpha = (unsigned char)((c0 * pix0.alpha + c1 * pix1.alpha + c2 * pix2.alpha + c3 * pix3.alpha) >> 16);
    }
  }

  return (status);
}


//  ----------------------------------------------------------------------------
// Compute histogram for the 4 channels of the display-modified pixels 
FPXStatus PResolutionLevel::GetHistogram (long* alpha, long* red, long* green, long* blue, long* brightness, const CorrectLut* correctLut)
{
  short i, j;
  PTile*  tile = tiles;

  // Histograms initialization 
  for (i=0; i<256; i++) {
    alpha[i]    = 0;
    red[i]      = 0;
    green[i]    = 0;
    blue[i]     = 0;
    brightness[i] = 0;
  }
  
  // Read pixels if necessary
  FPXStatus status = tile->Read();
  
  if (status == FPX_OK) {
    Pixel*  pt = tile->pixels;
    Pixel interm;
  
    // Compute histograms
    for (j=0; j<tile->height; j++) {
      for (i=0; i<tile->width; i++, pt++) {
        if (correctLut)
          interm = (*correctLut)(*pt);
        else
          interm = *pt;
        alpha[interm.alpha]++;
        red[interm.rouge]++;
        green[interm.vert]++;
        blue[interm.bleu]++;
        brightness[((int32)(interm.rouge)+((int32)(interm.vert)<<1)+(int32)(interm.bleu))>>2]++;
      }
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PResolutionLevel::GetResolutionSizeInfo (long* width, long* height, long* nbTilesWidth, long* nbTilesHeight)
{
  *width      = realWidth;
  *height     = realHeight;
  *nbTilesWidth   = nbTilesW;
  *nbTilesHeight  = nbTilesH;
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Return true if the position is on the border of the crop rectangle 
// CAUTION: coordinates are discrete with 0 bit precision (true pixel position in the level)
Boolean PResolutionLevel::IsOnTheBorder (long xi, long yi)
{
  // Compute cropping for this scale:
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = (fatherFile->cropX1 >> identifier) - 1;
  long cropy1 = (fatherFile->cropY1 >> identifier) - 1;
  
  // Check position on the borders:
  return ((xi >= cropx1) || (yi >= cropy1) || (xi <= cropx0) || (yi <= cropy0));
}


//  ----------------------------------------------------------------------------
// Read a pixel of a sub image. Read it on disk if necessary. 
// If error, return false and pixel is set to 0.
// CAUTION: coordinates are discrete with 0 bit precision (true pixel position in the level)
FPXStatus PResolutionLevel::ReadMean (long xi, long yi, Pixel& pixel)
{
  PTile     *tile;
  FPXStatus     status = FPX_OK;
  Boolean     WriteOnBackground = Toolkit_WriteOnBackground();
  Boolean     useAlpha      = fatherFile->useAlphaChannel | (isAlpha & premultiplied);
  unsigned char   alphaOffset     = fatherFile->alphaOffset;
  
  register long  TILE_SHIFT = fatherFile->log2TileWidth;
  register long  TILE_MASK  = fatherFile->maskTileWidth;
       Pixel BACKGROUND = fatherFile->backgroundUsed;

  // Compute cropping for this scale :
  long cropx0 = fatherFile->cropX0 >> identifier;
  long cropy0 = fatherFile->cropY0 >> identifier;
  long cropx1 = fatherFile->cropX1 >> identifier;
  long cropy1 = fatherFile->cropY1 >> identifier;

  if ((xi >= cropx1) || (yi >= cropy1) || (xi < cropx0) || (yi < cropy0)) {
    if (WriteOnBackground && !useAlpha)
      pixel = BACKGROUND;
  } else {
    tile = tiles + (yi >> TILE_SHIFT) * nbTilesW + (xi >> TILE_SHIFT);
    status = tile->Read();
    if (status) {         // If error reading on disk and pixels not created
      pixel = BACKGROUND;
//      VISU2 "obj_SubImageIO::ReadMean, read error => BACKGROUND\n" FIN
    } else {
      if (useAlpha) {
        short alpha, beta;
        unsigned char* srcPix;
        long index;
      
        srcPix = (unsigned char *)(tile->pixels + ((xi & TILE_MASK) + (yi & TILE_MASK) * tile->width));
        alpha  = *(srcPix + alphaOffset);
        beta   = PTile::invertLUT[alpha];
        unsigned char* desPix = (unsigned char*)(&pixel);
      
        if (premultiplied)
          for (index = 4; index; --index, ++desPix, ++srcPix)
            *desPix = (unsigned char)((*desPix*beta)/0xFF + *srcPix);
        else
          for (index = 4; index; --index, ++desPix, ++srcPix)
            *desPix = (unsigned char)((*desPix*beta + *srcPix*alpha)/0xFF);
      } else
        pixel = tile->pixels[(xi & TILE_MASK) + (yi & TILE_MASK) * tile->width];
    }
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Read a rectangle (x0,y0,x1,y1) in a map
// The correct sub image is read and sampled in order to fill the map with the rectangle 
// CAUTION : map must be allocated and freed by calling routine
//
FPXStatus PResolutionLevel::ReadSampledRectangle(long x0, long y0,long x1, long y1, Pixel* map, short pixelsPerLine, long mapWidth, long mapHeight, Boolean showAlphaChannel, float ratio)
{
  long  i, j, k;
  Pixel*  bufferPtr = map;

  register long  TILE_WIDTH = fatherFile->tileWidth;
  register long  TILE_SHIFT = fatherFile->log2TileWidth;
       Pixel BACKGROUND = fatherFile->backgroundUsed;
  unsigned char alphaOffset = fatherFile->alphaOffset;
  BACKGROUND.rouge  = 0xFF;
  FPXStatus   status;

  if (ratio == 0)
    ratio = MIN(float(float(mapWidth)/float(x1 - x0)) , float(float(mapHeight)/float(y1 - y0)));
      
  // Set use of alpha channel
  if ((showAlphaChannel && fatherFile->existAlphaChannel) || (fatherFile->useAlphaChannel) || (isAlpha & premultiplied))
    showAlphaChannel = true;
  else
    showAlphaChannel = false;

  short *pixToTile;
  long  tileWidth = FLOAT_TO_LONG(TILE_WIDTH * ratio);
  // Reserve an intermediate buffer 
  FastAllocArray(pixToTile, short, tileWidth);
  
  if (pixToTile == NULL) {
    // If not enough space available to Toolkit => fill with background color and quit 
    Pixel* bufferLine = bufferPtr;
    for (j=0; j<mapHeight; ++j, bufferLine += pixelsPerLine) {
      bufferPtr = bufferLine;
      for (i=0; i<mapWidth; ++i, bufferPtr++)
        *bufferPtr = BACKGROUND;
    }
    return FPX_MEMORY_ALLOCATION_FAILED;
  }
  
  // pixToTile will contains the pixels' positions to put into the new rectangle 
  for (i = 0; i < tileWidth; ++i)
    pixToTile[i] = short(float(i) / ratio + 0.01);
  
  PTile* tile = tiles;
  
  long heightOfBuffer, widthOfBuffer;
  long firstInTileW, firstInTileH;
  Boolean first = true;
  long topInBuffer = 0;
  for (short hTile = 0; hTile < nbTilesH; ++hTile) {
  
    long y0Tile = hTile << TILE_SHIFT;
    long y1Tile = (hTile+1) << TILE_SHIFT;
    long leftInBuffer = 0;
    Boolean firstInLine = true;
    
    for (short lTile = 0; lTile < nbTilesW; ++lTile) {
    
      long x0Tile = lTile << TILE_SHIFT;
      long x1Tile = (lTile+1) << TILE_SHIFT;
      
      // compare the tile position with the rectangle to read
      if ((x0 < x1Tile) && (x1 >= x0Tile) && (y0 < y1Tile) && (y1 >= y0Tile) ) {
        // Tile Initialisation
        status = tile->Read();
        if (!tile->pixels || (status != FPX_OK))  { // if error reading data => fill with background color and quit
          Pixel* bufferLine = bufferPtr;
          for (j=0; j<mapHeight; ++j, bufferLine += pixelsPerLine) {
            bufferPtr = bufferLine;
            for (i=0; i<mapWidth; ++i, bufferPtr++)
              *bufferPtr = BACKGROUND;
          }
          FastDeleteArray(pixToTile,short);
          return status;
        }
  
        if (first) {
          topInBuffer = FLOAT_TO_LONG(float(y0Tile - y0) * ratio);
        }
        if (firstInLine) {
          leftInBuffer = FLOAT_TO_LONG(float(x0Tile - x0) * ratio);
          firstInLine = false;
        }
        firstInTileW = 0;
        firstInTileH = 0;
        
        // Search of the height and width of the tile to be read
        if (hTile < nbTilesH-1)
          heightOfBuffer = FLOAT_TO_LONG(ratio * float(y1Tile - y0Tile));
        else 
          heightOfBuffer = FLOAT_TO_LONG(ratio * float(tile->height));
          
        if (topInBuffer < 0) {
          firstInTileH = -topInBuffer;
          topInBuffer = 0;
        }
          
        if (lTile < (nbTilesW-1))
          widthOfBuffer = FLOAT_TO_LONG(ratio * float(x1Tile - x0Tile));
        else 
          widthOfBuffer = FLOAT_TO_LONG(ratio * float(tile->width) );
          
        if (leftInBuffer < 0) {
          firstInTileW = -leftInBuffer;
          leftInBuffer = 0;
        }
           
        if (heightOfBuffer - firstInTileH + topInBuffer > mapHeight)
          heightOfBuffer = mapHeight - topInBuffer + firstInTileH;
        if (widthOfBuffer - firstInTileW + leftInBuffer > mapWidth)
          widthOfBuffer = mapWidth - leftInBuffer + firstInTileW;
          
        
        Pixel* bufferLine = bufferPtr + (topInBuffer * pixelsPerLine + leftInBuffer);
        
        for (j = firstInTileH; j < heightOfBuffer; ++j) {

          Pixel* pixelPtr = tile->pixels + (pixToTile[j] * tile->width);
          
          // Copy pixels with sampling
          if (firstInTileW)
            for (i = firstInTileW, k = 0; i < widthOfBuffer; ++i, ++k)
              bufferLine[k] = pixelPtr[pixToTile[i]];
          else
            for (i = 0; i < widthOfBuffer; ++i)
              bufferLine[i] = pixelPtr[pixToTile[i]];
  
          // Take alpha channel into account
          if (showAlphaChannel) {
            short alpha, beta;
            unsigned char* sPix;
            unsigned char* dPix;
            long index;
            
            for (i = 0; i < widthOfBuffer; ++i) {
              sPix = (unsigned char*)(bufferLine + i);
              alpha = (short)(*(sPix + alphaOffset));
              beta  = (short)(PTile::invertLUT[alpha]);
              if (!alpha)
                bufferLine[i] = BACKGROUND;
              else if (alpha < 0xFF) {
                dPix = (unsigned char*)(&BACKGROUND);
                if (premultiplied)
                  for (index = 4; index; --index, ++dPix, ++sPix)
                    *sPix = (unsigned char)(*dPix*beta/0xFF + *sPix);
                else
                  for (index = 4; index; --index, ++dPix, ++sPix)
                    *sPix = (unsigned char)((*dPix*beta + *sPix*alpha)/0xFF);
              }
              // else if (alpha == 0xFF) bufferLine[i] is unchanged
            }
          }
          
          bufferLine += pixelsPerLine;
        }
        leftInBuffer += widthOfBuffer - firstInTileW;
      }
      ++tile;
    }
    if (!firstInLine) {
      first = false;
      topInBuffer += heightOfBuffer - firstInTileH;
    }
  }
  FastDeleteArray(pixToTile, short);
  
  return status;
}


//  ----------------------------------------------------------------------------
// Search the top and left corner of the screen pixel which contains the given position
// This function is used when 1 image pixel is represented by more than 1 screen pixel
FPXStatus PResolutionLevel::SearchPixelTopLeftCorner(long* x1, long* y1, float ratio)
{
  register long  TILE_WIDTH = fatherFile->tileWidth;
  register long  TILE_SHIFT = fatherFile->log2TileWidth;
      
  short   *pixToTile;
  FPXStatus status;
  
  long  tileWidth = FLOAT_TO_LONG(TILE_WIDTH * ratio);
  
  // Reserve an intermediate buffer 
  FastAllocArray(pixToTile, short, tileWidth);
  if (pixToTile == NULL)
    return FPX_MEMORY_ALLOCATION_FAILED;
  
  long i;
  // pixToTile will contains the screen pixels' positions  
  for (i = 0; i < tileWidth; ++i)
    pixToTile[i] = short(float(i) / ratio + 0.01);
  
  PTile*  tile = tiles;
  Boolean end = false;
  for (short hTile = 0; hTile < nbTilesH; ++hTile) {
  
    long y0Tile = FLOAT_TO_LONG(float(hTile << TILE_SHIFT) * ratio);
    long y1Tile = FLOAT_TO_LONG(float((hTile+1) << TILE_SHIFT) * ratio);
    
    for (short lTile = 0; lTile < nbTilesW; ++lTile) {
    
      long x0Tile = FLOAT_TO_LONG(float( lTile    << TILE_SHIFT) * ratio);
      long x1Tile = FLOAT_TO_LONG(float((lTile+1) << TILE_SHIFT) * ratio);
      
      // compare the tile position with the position to find
      if ((*x1 < x1Tile) && (*x1 >= x0Tile) && (*y1 < y1Tile) && (*y1 >= y0Tile) ) {
        // Tile Initialisation
        status = tile->Read();
        if (status != FPX_OK)         { // if error reading data => quit
          FastDeleteArray(pixToTile,short);
          return status;
        }
  
        long topInBuffer = *y1 - y0Tile;
        short pixelY = pixToTile[topInBuffer];
        long leftInBuffer = *x1 - x0Tile;
        short pixelX = pixToTile[leftInBuffer];
        
        while ((topInBuffer > 0) && (pixToTile[topInBuffer-1] == pixelY)) topInBuffer--;
        while ((leftInBuffer > 0) && (pixToTile[leftInBuffer-1] == pixelX)) leftInBuffer--;
        
        *y1 = topInBuffer + y0Tile;
        *x1 = leftInBuffer + x0Tile;
        end = true;
        break;
      }
      ++tile;
    }
    if ( end ) break;
  }
  FastDeleteArray(pixToTile, short);
  
  return status;
}


//  ----------------------------------------------------------------------------
// Send back the smallest image contained in a rectangle width * height.
// Read it on disk if necessary. 
// If decompressor is missing, send back NULL.
FPXStatus PResolutionLevel::ReadInARectangle(Pixel* bufferOut, short pixelsPerLine, short width, short height, 
                    const CorrectLut* correctLut, Boolean useAlphaChannel, const CombinMat* combinaisonMatrix)
{
  short   i, j;
  Pixel*    bufferPtr;
  float   ratio;
  FPXStatus   status = FPX_OK;

  register long  TILE_WIDTH = fatherFile->tileWidth;
       Pixel BACKGROUND = fatherFile->backgroundUsed;
  unsigned char alphaOffset = fatherFile->alphaOffset;
  BACKGROUND.rouge  = 0xFF;

  // Set use of alpha channel
  if ((useAlphaChannel && fatherFile->existAlphaChannel) ||
      (fatherFile->useAlphaChannel) || (isAlpha & premultiplied))
    useAlphaChannel = true;
  else
    useAlphaChannel = false;

  // Reduce the width or the height in order to have : realHeight * width == realWidth * height
  if (realHeight * width >= realWidth * height) {
    ratio = float(height) / float(realHeight);
    short newWidth = (short)(realWidth * ratio);
    bufferPtr = bufferOut + ((width - newWidth)/2);
    width = newWidth;
  } else {
    ratio = float(width) / float(realWidth);
    short newHeight = (short)(realHeight * ratio);
    bufferPtr = bufferOut + ((height - newHeight)/2) * pixelsPerLine;
    height = newHeight;
  }

  // Reserve an intermediate buffer 
  short* pixToTile;
  long   tileWidth = FLOAT_TO_LONG(float(TILE_WIDTH) * ratio);

  FastAllocArray(pixToTile, short, tileWidth);
    
  if (pixToTile == NULL) {
    // If not enough space available to Toolkit => fill with background color and quit 
    Pixel* bufferLine = bufferPtr;
    for (j = 0; j < height; ++j, bufferLine += pixelsPerLine) {
      bufferPtr = bufferLine;
      for (i = 0; i < width; ++i, ++bufferPtr)
        *bufferPtr = BACKGROUND;
    }
    return FPX_MEMORY_ALLOCATION_FAILED;
  }
  
  // pixToTile will contains the pixels' positions to put into the new rectangle 
  for (i = 0; i < tileWidth; ++i)
    pixToTile[i] = short(float(i) / ratio + 0.01);
      
  // Declare and init some variables before the main loop...
  long heightOfBuffer, widthOfBuffer;   // Width and height to be written for each bloc
  long yBloc = 0;             // Top coordinate in the resolution level
  long topInBuffer = 0;         // Top coordinate in the buffer
  PTile* tile = tiles;          // Pointer to the current bloc of the resolution level

  // Loop on the height 
  for (short hBloc = 0; hBloc < nbTilesH; ++hBloc) {
  
    long xBloc = 0;     // Left coordinate in the resolution level
    long leftInBuffer = 0;  // Left coordinate in the buffer

    // Loop on the width
    for (short lBloc = 0; lBloc < nbTilesW; ++lBloc) {
    
      // Bloc Initialisation
      status = tile->Read();
      if (status != FPX_OK) { // if error reading data => fill with background color and quit
        Pixel* bufferLine = bufferPtr;
        for (j = 0; j < height; ++j, bufferLine += pixelsPerLine) {
          bufferPtr = bufferLine;
          for (i = 0; i < width; ++i, bufferPtr++)
            *bufferPtr = BACKGROUND;
        }
        FastDeleteArray(pixToTile,short);
        return status;
      }

      // Compute the height and width of the tile to be read
      heightOfBuffer = FLOAT_TO_LONG(float(tile->height) * ratio);
      widthOfBuffer  = FLOAT_TO_LONG(float(tile->width)  * ratio);
        
      if (heightOfBuffer + topInBuffer  > height)
        heightOfBuffer = height - topInBuffer;
      if (widthOfBuffer  + leftInBuffer > width)
        widthOfBuffer  = width  - leftInBuffer;

      // Position the pointer to the first pixel to be written
      Pixel* bufferLine = bufferPtr + (topInBuffer * pixelsPerLine + leftInBuffer);
      
      for (j = 0; j < heightOfBuffer; ++j) {
      
        // Position a pointer to the first pixel to be read
        Pixel* pixelPtr = tile->pixels + (pixToTile[j] * tile->width);

        // Copy pixels with sampling
        for (i = 0; i < widthOfBuffer; ++i)
          bufferLine[i] = pixelPtr[pixToTile[i]];

        // Take channel combinaison into account
        if (combinaisonMatrix) {
          for (i = 0; i < widthOfBuffer; ++i)
            bufferLine[i] = (*combinaisonMatrix)(bufferLine[i]);
        }
        
        // Take LUT into account
        if (correctLut) {
          for (i = 0; i < widthOfBuffer; ++i)
            bufferLine[i] = (*correctLut)(bufferLine[i]);
        }

        // Take alpha channel into account
        if (useAlphaChannel && PTile::invertLUT) {
          short alpha, beta;
          unsigned char* sPix;
          unsigned char* dPix;
          long index;
          
          for (i = 0; i < widthOfBuffer; ++i) {
            sPix  = (unsigned char*)(bufferLine + i);
            dPix = (unsigned char*)(&BACKGROUND);
            alpha = (short)(*(sPix + alphaOffset));
            beta  = (short)(PTile::invertLUT[alpha]);
            if (!alpha)
              bufferLine[i] = BACKGROUND;
            else if (alpha < 0xFF) {
              if (premultiplied)
                for (index = 4; index; --index, ++dPix, ++sPix)
                  *sPix = (unsigned char)(*dPix*beta/0xFF + *sPix);
              else
                for (index = 4; index; --index, ++dPix, ++sPix)
                  *sPix = (unsigned char)((*dPix*beta + *sPix*alpha)/0xFF);
            }
            // else if (alpha == 0xFF) bufferLine[i] is unchanged
          }
        }
        
        // Next line to be written
        bufferLine += pixelsPerLine;
      }
      
      xBloc += tile->width;       // Increment left coordinate in the resolution level
      leftInBuffer += widthOfBuffer;    // Increment left coordinate in the buffer
      ++tile;               // Next tile
    }
    
    yBloc += tile->height;      // Increment top coordinate in the resolution level
    topInBuffer += heightOfBuffer;  // Increment top coordinate in the buffer
  }
  
  FastDeleteArray(pixToTile, short);
  return status;
}

//  ----------------------------------------------------------------------------
//  External Functions
//  ----------------------------------------------------------------------------



//  - EOF ----------------------------------------------------------------------
