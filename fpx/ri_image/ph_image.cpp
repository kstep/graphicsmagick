//  ----------------------------------------------------------------------------
//  MODULE    : PHierarchicalImage
//  LANGUAGE  : C++ 
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Tuesday, March 12, 1996 
//  DESCRIPTION : 
//  COMMENTS  : 
//    SCCSID          : @(#)ph_image.cpp  1.2 12:48:36 22 Jul 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
#include "ph_image.h"
#include "f_fpxio.h"
//  ----------------------------------------------------------------------------

//  Includes
//  --------

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#ifndef _WINDOWS
#  include  <strings.h>
#endif
#include  <math.h>
#ifdef macintosh
#  include  <Errors.h>
#endif  

  
#ifndef Debug_h
#  include  "debug.h"
#endif
#ifndef Memoire_h
#  include  "b_memory.h"
#endif
#ifndef Fichier_h
#  include  "a_file.h"
#endif

#ifndef PResolutionLevel_h
#  include "pr_level.h"
#endif
#ifndef PTile_h
#  include "ptile.h"
#endif
#ifndef PImageFile_h
#  include "pimgfile.h"
#endif

#ifndef Numbers_h
#  include  "numbers.h"
#endif

#ifndef BufferDesc_h
#  include  "buffdesc.h"
#endif


//  Constants
//  ---------

//  Variables
//  ---------

//  ----------------------------------------------------------------------------
//  #pragma segment PHierarchicalImage
//  ----------------------------------------------------------------------------

//  ----------------------------------------------------------------------------
//  Internal Functions
//  ----------------------------------------------------------------------------

inline float MIN(float a, float b, float c)
{
  return MIN(a, MIN(b,c));
}
  
//  ----------------------------------------------------------------------------
//  Member Functions
//  ----------------------------------------------------------------------------

// Open a file in Create mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (FicNom& refName, long width, long height, float resolution) : PRIImage (width, height, resolution)
{
  Init ();        // Init the object

  fileName = refName;     // Name of the image file
  mode   = mode_Ecrasement;   // Opening mode: overwrite image

  if (EnoughDiskSpace() == false)
    mode = mode_Lecture;    // If not enough disk space, invalid overwrite mode
}


//  ----------------------------------------------------------------------------
// Open a file in Read mode only (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (FicNom& refName) : PRIImage ()
{
  Init ();        // Init the object
  fileName = refName;     // Name of the image file
}


//  ----------------------------------------------------------------------------
// Open a file in Read or Write mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (FicNom& refName, mode_Ouverture openMode) : PRIImage ()

{
  Init ();        // Init the object
  fileName = refName;     // Name of the image file
  mode   = openMode;      // Opening mode: overwrite image
}


//  ----------------------------------------------------------------------------
// Open a file in Create mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (int theFd, long width, long height, float resolution) 
  : PRIImage (width, height, resolution)
{
  Init ();        // Init the object

  fd  = theFd;      // The caller opened the image
  mode  = mode_Ecrasement;    // Opening mode: overwrite image

  if (EnoughDiskSpace() == false)
    mode = mode_Lecture;    // If not enough disk space, invalid overwrite mode
}


//  ----------------------------------------------------------------------------
// Open a file in Read mode only (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (int theFd) : PRIImage ()
{
  Init ();    // Init the object
  fd = theFd;   // The caller opened the image
}


//  ----------------------------------------------------------------------------
// Open a file in Read or Write mode (compressed or uncompressed) :
// CAUTION : the file is opened as long as necessary
PHierarchicalImage::PHierarchicalImage (int theFd, mode_Ouverture openMode) : PRIImage ()

{
  Init ();      // Init the object
  fd    = theFd;  // The caller opened the image
  mode  = openMode;   // Opening mode: overwrite image
}


//  ----------------------------------------------------------------------------
// Clean memory
PHierarchicalImage::~PHierarchicalImage ()
{
  if (filePtr) {
    delete filePtr;
    filePtr = NULL;
  }
  if (firstSubImage) {
    delete firstSubImage;
    firstSubImage = NULL;
  }
  if (subImages) {
    delete [] subImages;
    subImages = NULL;
  }
}


//  ----------------------------------------------------------------------------
// Give default values to image information before starting reading or using them
void PHierarchicalImage::Init ()
{
  // PHierarchicalImage values
  baseSpace     = SPACE_32_BITS_RGB;
  baseUncalibrated  = FALSE;
  usedSpace     = SPACE_32_BITS_RGB;
  isFlatFile    =  FALSE;
  alphaOffset     = (unsigned char) GetAlphaOffsetBaseline(usedSpace);
  backgroundBase  = Toolkit_BackgroundColor();
  backgroundUsed  = Toolkit_BackgroundColor();

  convolution   = Toolkit_Convolution();  // Default convolution mode
  filter    = 0;        // No custom convolution function for the moment
  automaticDecimation = true;       // Default is: yes, do decimation automatically
  
  compression   = Toolkit_Compression();  // Default compression mode
  compressionHandle = NULL;       // Start with no compression handleÐ
  handleSize    = 0;        // Ðwhich size is then null

  SetTileParameter (Toolkit_TileWidth(), Toolkit_TileWidth()); // Default tile size

                                                                  firstSubImage   = NULL;
  nbSubImages   = 0;
  subImages   = NULL;

  //  fileName  = "";       // Name of the image file

  filePtr   = NULL;       // File not opened yetÐ
  fd      = 0;        // Ðor may be by the caller.
  mode      = mode_Lecture;     // Opening mode: read only by default
  posFic    = 0;        // Position at the begining os the file
  version   = 0;        // Version of the file (we've no idea at that level...)

  posPath   = 0;        // No paths
}


//  ----------------------------------------------------------------------------
FPXStatus PHierarchicalImage::SetTileParameter (long width, long height)
{
  // Test parameters consistency
  assert((width>0) && (height>0));
  
  tileWidth = MAX(width,height);
  tileSize = width * height * sizeof(Pixel);
  tileLineSize = width * sizeof(Pixel);
  log2TileWidth = (long)(Toolkit_Log2(tileWidth));  
  maskTileWidth = tileWidth - 1;
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Open the file, create it if necessary (if mode is write), write first information on the first block
FPXStatus PHierarchicalImage::OpenFile ()
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}


//  ----------------------------------------------------------------------------
// Store the header of the file before closing
FPXStatus PHierarchicalImage::CloseFile()
{
  return FPX_UNIMPLEMENTED_FUNCTION;
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::DoNotCloseFileWhenDelete()
{
  filePtr->DoNotCloseFileWhenDelete();
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::CloseFileWhenDelete()
{
  filePtr->CloseFileWhenDelete();
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::UpdateErrorCount()
{
  PRIImage::UpdateErrorCount();
  if (!Status()) {      // If error raised
    nbSubImages = 0;    // Invalid the image
    mode = mode_Lecture;  // Force mode to be read only
  }   
}


//  ----------------------------------------------------------------------------
// Open an image file and create the resolution level list "firstSubImage" and the "subImages" array of pointers
FPXStatus PHierarchicalImage::OpenImage()
{
  FPXStatus status = FPX_OK;
  
  // Open the file if it hasn't been done yet
  if (filePtr == NULL) {
    status = OpenFile();
    // Init the size parameters according to the infos read into the file
    SetImageSize(width,height,resolution);
  }
    
  if (firstSubImage == NULL && !Status()) {
    if (nbSubImages != 0)                     // The file is open en read or write mode
      CreateInitResolutionLevelList();            // Create and initialize the list of sub image reading the file "filePtr"
    else if ((nbSubImages == 0) && (mode == mode_Ecrasement)) // The file is open in create mode
      CreateEmptyResolutionLevelList();           // Create the list of empty sub images
    else {
      RaiseError( FPX_FILE_READ_ERROR);
      status = FPX_FILE_READ_ERROR;
    }
    // Initialize the array of pointers "subImages"
    // LS 1/11/96: We initialize the field 'subImages' even on error; 
    //         else, LP crashes when reading a compressed image without QuickTime installed
    InitResolutionLevelsTable();
  }
  if (status == FPX_OK)
    status = Status();
  return status;
}


//  ----------------------------------------------------------------------------
FPXStatus PHierarchicalImage::GetFileName (FicNom& refName)
{
  refName = fileName;
  return(FPX_OK);
}
      

//  ----------------------------------------------------------------------------
// Store "pix" in a buffer, write it if it's full and compute sub-image buffer if necessary
FPXStatus PHierarchicalImage::WriteLine (Pixel* pix, short plan)
{
  Pixel   *source;
  FPXStatus status = FPX_OK;
  
  if (GtheSystemToolkit->interleaving == Interleaving_Pixel) 
    status = firstSubImage->WriteLine(pix, plan);
  else {
    FastAllocArray(source,Pixel,(unsigned int)(width));
    if (source == NULL)
      return FPX_MEMORY_ALLOCATION_FAILED;
      
    if ((GtheSystemToolkit->interleaving != Interleaving_Channel) || (plan == ActiveChannel_All)) {
      if (Toolkit_UnInterleave (pix, source, width, 1, width, 1)) {
        FastDeleteArray(source,Pixel);
        return FPX_COLOR_CONVERSION_ERROR;  // error converting pixel buffer according to user's options
      }
  
    } else {
      // The buffer contain only the active channel
      // we must create a temporary buffer to send to the image file
      register long j;
      register unsigned char *src, *dst;
      
      src = (unsigned char *)(pix); 
      dst = (unsigned char *)(source)  + plan;
      for (j = 0; j < width; j++, src++, dst+=sizeof(Pixel))
        *dst = *src;
    }
    status = firstSubImage->WriteLine(source, plan);
    FastDeleteArray(source,Pixel);
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Store "pix" in a buffer, write it if it's full and compute sub-image buffer if necessary
FPXStatus PHierarchicalImage::WriteRectangle (long x0, long y0, long x1, long y1, Pixel* pix, short plan, short resolution)
{
  FPXStatus status = FPX_OK;
  
  if ( GtheSystemToolkit->interleaving == Interleaving_Pixel) {
    status = subImages[resolution]->WriteRectangle (x0, y0, x1, y1, pix, plan);
  } else {
    long pixWidth = (x1 - x0 + 1);    // width of the buffer 'pix'
    long pixHeight = (y1 - y0 + 1);   // height of the buffer 'pix'
    Pixel*  dest = NULL;        // temporary buffer
    Pixel*  ptrDest = NULL;
    long subX0, subY0, subX1, subY1;  // corners of the temporary buffer
    long bufSize = 0;         // size of temporary buffer
    long bufWidth, bufHeight;     // width ans height of temporary buffer
    register long i, j;
    register unsigned char *src, *dst, *ptrPix;
    
    src = dst = ptrPix = 0;
    // cut the rectangle in sub-rectangles to avoid to create a big buffer 
    // in memory when converting the interleaving.
    // These sub-rectangles have the size of a tile because writing tiles is optimum.
    long incrRow = tileWidth;
    long incrCol = tileWidth;
    for (subY0 = y0; subY0 <= y1; subY0+=incrRow) {
      subY1 = subY0 + incrRow-1;
      if (subY1 > y1) subY1 = y1;
      bufHeight = subY1 - subY0 +1;
      for (subX0 = x0; subX0 <= x1; subX0+=incrCol) {
        subX1 = subX0 + incrCol-1;
        if (subX1 > x1) subX1 = x1;
        bufWidth = subX1 - subX0 +1;
        if (bufSize != (bufWidth*bufHeight)) {
          bufSize = bufWidth*bufHeight;
          FastDeleteArray(dest, Pixel);
          FastAllocArray(dest, Pixel, bufSize);
          if (!dest)
            return FPX_MEMORY_ALLOCATION_FAILED;
        }
        assert(dest);
        // search the position of the first pixel in the rectangle to copy in the sub_rectangle
        if ((GtheSystemToolkit->interleaving != Interleaving_Channel) || (plan == ActiveChannel_All)) {
          switch (GtheSystemToolkit->interleaving) {
          case Interleaving_Line:
            ptrPix = (unsigned char *)(pix);
            ptrPix += (subY0-y0)*pixWidth*4 + (subX0-x0);
            break;
          case Interleaving_Channel:
            ptrPix = (unsigned char *)(pix);
            ptrPix += (subY0-y0)*pixWidth + (subX0-x0);
            break;
          case Interleaving_Pixel:
            break;
          }
          // compute the Uninterleaving in the sub-rectangle
          if (Toolkit_UnInterleave ((Pixel *)(ptrPix), dest, pixWidth, pixHeight, bufWidth, bufHeight)) {
            FastDeleteArray(dest,Pixel);
            return FPX_COLOR_CONVERSION_ERROR;  // error converting pixel buffer according to user's options
          }
        } else {
          // The buffer contain only the active channel
          // we must copy only this channel to the temporary buffer to send to the image file
          
          ptrPix = (unsigned char *)(pix); 
          ptrPix += (subY0-y0)*pixWidth + (subX0-x0);
          ptrDest = dest; 
          for (i=subY0;i<=subY1;i++,ptrDest+=bufWidth,ptrPix+=pixWidth ) {
            src = ptrPix; 
            dst = (unsigned char *)(ptrDest)  + plan;
            for (j=subX0; j<=subX1;j++,src++,dst+=sizeof(Pixel))
              *dst = *src;
          }
        }
        // write the temporary buffer in the image file
        status = subImages[resolution]->WriteRectangle (subX0, subY0, subX1, subY1, dest, plan);
        if (status) {
          FastDeleteArray(dest,Pixel);
          return status;
        }
      }
    }
  
    FastDeleteArray(dest,Pixel);
    dest = NULL;
  }
  return status;
  
}


//  ----------------------------------------------------------------------------
// To generate the hierarchy, we just have to generate the high res: because of the recursive
// nature of the DecimateTile() function, this will automatically decimate all the sub resolutions.
// Note that this routine does not ensure that modified tiles are written to the file.
// After this call returns, there may still be cached, modified tiles in memory.
FPXStatus PHierarchicalImage::RegenerateHierarchy ()
{
  FPXStatus err = FPX_OK;

  // Required to have the recursion working
  SetAutomaticDecimation(true);
  
  // Save any changes to the full resolution's tiles and then decimate it's tiles.
  if (firstSubImage) {
    err = firstSubImage->FlushModifiedTiles();
    if (err == FPX_OK)
      err = firstSubImage->DecimateLevel();
  }
  else
    err = FPX_ERROR;

  return err;
}


//  ----------------------------------------------------------------------------
// Read a rectangle in the correct resolution level and fill 'pix' with it
FPXStatus PHierarchicalImage::ReadRectangle (long x0, long y0, long x1, long y1, Pixel* pix, long resolution)
{
  FPXStatus status = FPX_OK;
  FPXStatus   currentStatus;
  
  if (resolution == -1)
    resolution = 0;
  if (GtheSystemToolkit->interleaving == Interleaving_Pixel)
    status = subImages[resolution]->ReadRectangle (x0, y0, x1, y1, pix);
  else {
    // Cut the rectangle in sub-rectangles to avoid to create a big buffer in memory
    // when converting the interleaving.
    // These sub-rectangles have the size of a tile because it's optimum.
    long pixWidth = (x1 - x0 + 1);    // width of the buffer 'pix'
    long pixHeight = (y1 - y0 + 1);   // height of the buffer 'pix'
    Pixel*  source = NULL;      // temporary buffer
    Pixel*  ptrSource = NULL;
    long subX0, subY0, subX1, subY1;  // corners of the temporary buffer
    long bufSize = 0;         // size of temporary buffer
    long bufWidth, bufHeight;     // width ans height of temporary buffer
    register long i, j;
    register unsigned char *src, *dst, *ptrPix;
    
    short plan = Toolkit_ActiveChannel();
    long incrRow = tileWidth;
    long incrCol = tileWidth;
    for (subY0 = y0; subY0 <= y1; subY0+=incrRow) {
      subY1 = subY0 + incrRow-1;
      if (subY1 > y1) subY1 = y1;
      bufHeight = subY1 - subY0 +1;
      for (subX0 = x0; subX0 <= x1; subX0+=incrCol) {
        subX1 = subX0 + incrCol-1;
        if (subX1 > x1) subX1 = x1;
        bufWidth = subX1 - subX0 +1;
        if (bufSize != (bufWidth*bufHeight)) {
          bufSize = bufWidth*bufHeight;
          FastDeleteArray(source, Pixel);
          FastAllocArray(source, Pixel, bufSize);
          if (!source)
            return FPX_MEMORY_ALLOCATION_FAILED;
        }
        assert(source);
        // read a sub rectangle in Pixel
        currentStatus = subImages[resolution]->ReadRectangle (subX0, subY0, subX1, subY1, source);
        if (currentStatus)
          status = currentStatus;
        if (status != FPX_MEMORY_ALLOCATION_FAILED) {
          if ((GtheSystemToolkit->interleaving != Interleaving_Channel) || (plan == ActiveChannel_All)) {
            if (Toolkit_Interleave (source, bufWidth, bufHeight))
              status = FPX_MEMORY_ALLOCATION_FAILED;
            else if (Toolkit_CopyInterleaved (pix, pixWidth, pixHeight,
                                              source, bufWidth, bufHeight, subX0-x0, subY0-y0))
              status = FPX_MEMORY_ALLOCATION_FAILED;
          } else {
            // Pixel by pixel if planes specified, the destination contains only the choosen channel
            ptrPix  = (unsigned char *)(pix); 
            ptrPix += (subY0-y0)*pixWidth + (subX0-x0);
            ptrSource = source; 
            for (i = subY0; i <= subY1; i++, ptrSource += bufWidth, ptrPix += pixWidth ) {
              src = (unsigned char *)(ptrSource)  + plan;
              dst = ptrPix; 
              for (j = subX0; j <= subX1; j++, src += sizeof(Pixel), dst++)
                *dst = *src;
            }
          }
        }
        if (status == FPX_MEMORY_ALLOCATION_FAILED) {
          FastDeleteArray(source,Pixel);
          return status;
        }

      }
    }
    FastDeleteArray(source,Pixel);
    source = NULL;
  }
  return status;
}


//  ----------------------------------------------------------------------------
// Read the path from the Image file: this function has to be derived to really write the things in
// the file. The following code handle only the memory management part.
OSErr PHierarchicalImage::ReadPaths (data_Record** thePath, long* nbRecords, long* theClipPathNum)
{
  return PRIImage::ReadPaths (thePath, nbRecords, theClipPathNum);
}


//  ----------------------------------------------------------------------------
// Write the path in the Image file: this function has to be derived to really write the things in
// the file. The following code handle only the memory management part.
OSErr PHierarchicalImage::WritePaths (data_Record*  thePath, long  nbRecords, long  theClipPathNum)
{
  OSErr  err = noErr;

  // Release the old path if any
  ReleasePaths ();
  
  // Store the new path info
  nbPaths   = nbRecords;
  clipPathNum = theClipPathNum;
  
  // Allocate the intermediate path buffer
  if ((path = new data_Record [nbPaths]) == NULL) {
    err = memFullErr;   // error : can't reserve buffer
    goto end;
  }
  
  // Copy the given paths into the allocated path buffer
  BlockMove(thePath,path,nbPaths*sizeof(data_Record));
  
 end:
  if (err != noErr)
    // If error, clean everything before exit
    ReleasePaths ();
  return err;
}


//  ----------------------------------------------------------------------------
FPXStatus PHierarchicalImage::Read4Points (long x0, long y0, long x1, long y1, long x2, long y2, long x3, long y3, Pixel* table, long levelSubImage)
{
  FPXStatus status = FPX_OK;        // Returned value
  long    dx, dy;
  long    n;
  long    px[16], py[16];


  dx = x1 - x0;
  dy = y1 - y0;
  
  // Compute sub-image level
  if (levelSubImage != -1)
    n = levelSubImage;
  else
    n = Toolkit_Log2((((dx < 0 ? -dx:dx) + (dy < 0 ? -dy:dy)) >> 14) & 0x3ff); // (14 = 12 + 2) PARAM_ECHELLE_VUE;
  // CHG_FIX_FLAT_TNAIL - if a non-hierarchical image is being created, then isFlatFile has
  //    not been initialized, so it is misleading. However, if we're creating, then the
  //    nbCreatedResolutions is 1. nbCreatedResolutions is set properly for reads and
  //    creates, so it was added to the following test to ensure that we render using a
  //    valid resoltion.
  if(isFlatFile || (((PFileFlashPixIO*)this)->GetNumCreatedResolutions() == 1))
    n = 0;
  if (nbSubImages) {          // If image has been successfully opened (else, file is missing)
    if (PRIImage::readInterpolated) {
      if (n >= nbSubImages)
        n = nbSubImages - 1;
      if (n) {
        x3 >>= n; y3 >>= n;
        x2 >>= n; y2 >>= n;
        x1 >>= n; y1 >>= n;
        x0 >>= n; y0 >>= n;
      }
    
      // Shift by half a pixel toward top left corner (origin): necessary to avoid misregistration when switching
      // between with and without interpolation (classic difference between discrete and continuous coordinates)
      x0 -= 2048; y0 -= 2048;   // Coordinates are with 12 bits precision, so 1 pixel = 4096 units...
      x1 -= 2048; y1 -= 2048; 
      x2 -= 2048; y2 -= 2048; 
      x3 -= 2048; y3 -= 2048; 
    
      px[P00] = x0 >> 4;          py[P00] = y0 >> 4;
      dx = (x0+x3) >>1;         dy = (y0+y3) >>1;
      px[P22] = dx >>4;         py[P22] = dy >>4;
      px[P11] = (x0+dx)>>5;       py[P11] = (y0+dy)>>5;
      px[P33] = (x3+dx)>>5;       py[P33] = (y3+dy)>>5;
      px[P31] = (x1+dx)>>5;       py[P31] = (y1+dy)>>5;
      px[P13] = (x2+dx)>>5;       py[P13] = (y2+dy)>>5;
    
      px[P32] = ((x3 += (x0 + x3)) + x1) >> 6;  py[P32] = ((y3 += (y0 + y3)) + y1) >> 6;    
      px[P23] = (x3 + x2) >> 6;   py[P23] = (y3 + y2) >> 6;
        
      x3 = (x0 + x1) >> 1;        y3 = (y0 + y1) >> 1;
      px[P20] = x3 >> 4;          py[P20] = y3 >> 4;
      px[P21] = (x3+dx)>>5;       py[P21] = (y3+dy)>>5;
      px[P10] = (x0+x3)>>5;       py[P10] = (y0+y3)>>5;
      px[P30] = (x1+x3)>>5;       py[P30] = (y1+y3)>>5;
    
      x3 = (x0 + x2) >> 1;        y3 = (y0 + y2) >> 1;
      px[P02] = x3 >> 4;          py[P02] = y3 >> 4;
      px[P12] = (x3+dx)>>5;       py[P12] = (y3+dy)>>5;
      px[P01] = (x0+x3)>>5;       py[P01] = (y0+y3)>>5;
      px[P03] = (x2+x3)>>5;       py[P03] = (y2+y3)>>5;
      
      status = subImages[n]->ReadInterpolated (px, py, table);
    }
    else {
      if (n >= nbSubImages)
        n = nbSubImages - 1;
      if (n) {
        x3 >>= n; y3 >>= n;
        x2 >>= n; y2 >>= n;
        x1 >>= n; y1 >>= n;
        x0 >>= n; y0 >>= n;
      }

      px[P00] = x0 >> 12;         py[P00] = y0 >> 12;
      dx = (x0+x3) >>1;         dy = (y0+y3) >>1;
      px[P22] = dx >>12;          py[P22] = dy >>12;
      px[P11] = (x0+dx)>>13;        py[P11] = (y0+dy)>>13;
      px[P33] = (x3+dx)>>13;        py[P33] = (y3+dy)>>13;
      px[P31] = (x1+dx)>>13;        py[P31] = (y1+dy)>>13;
      px[P13] = (x2+dx)>>13;        py[P13] = (y2+dy)>>13;
    
      px[P32] = ((x3 += (x0 + x3)) + x1) >> 14; py[P32] = ((y3 += (y0 + y3)) + y1) >> 14;   
      px[P23] = (x3 + x2) >> 14;    py[P23] = (y3 + y2) >> 14;
        
      x3 = (x0 + x1) >> 1;        y3 = (y0 + y1) >> 1;
      px[P20] = x3 >> 12;         py[P20] = y3 >> 12;
      px[P21] = (x3+dx)>>13;        py[P21] = (y3+dy)>>13;
      px[P10] = (x0+x3)>>13;        py[P10] = (y0+y3)>>13;
      px[P30] = (x1+x3)>>13;        py[P30] = (y1+y3)>>13;
    
      x3 = (x0 + x2) >> 1;        y3 = (y0 + y2) >> 1;
      px[P02] = x3 >> 12;         py[P02] = y3 >> 12;
      px[P12] = (x3+dx)>>13;        py[P12] = (y3+dy)>>13;
      px[P01] = (x0+x3)>>13;        py[P01] = (y0+y3)>>13;
      px[P03] = (x2+x3)>>13;        py[P03] = (y2+y3)>>13;
      status = subImages[n]->Read(px,py,table);
    }
  } else
    status = FPX_FILE_READ_ERROR;
    
  if (status != FPX_OK) {
    // If the tile is missing, fill "table[]" with a chessboard pattern
    Pixel* ptp = table;

    *ptp++ = 0x00000000; *ptp++ = 0x00000000; *ptp++ = 0xFFFFFFFF; *ptp++ = 0xFFFFFFFF;
    *ptp++ = 0x00000000; *ptp++ = 0x00000000; *ptp++ = 0xFFFFFFFF; *ptp++ = 0xFFFFFFFF;
    *ptp++ = 0xFFFFFFFF; *ptp++ = 0xFFFFFFFF; *ptp++ = 0x00000000; *ptp++ = 0x00000000;
    *ptp++ = 0xFFFFFFFF; *ptp++ = 0xFFFFFFFF; *ptp++ = 0x00000000; *ptp++ = 0x00000000;
  }
  
  return status;
}


//  ----------------------------------------------------------------------------
// Send back the smallest image contained in a rectangle width * height.
// Read it on disk if necessary. 
// If decompressor is missing, send back NULL.
// CAUTION : bufferOut must be allocated and freed by calling routine
// CAUTION : we allocate a temporary buffer with pixelsPerLine*height pixels
FPXStatus PHierarchicalImage::ReadInARectangle (Pixel* bufferOut, short pixelsPerLine, short rectWidth, short rectHeight, 
                  const CorrectLut* correctLut, Boolean useAlphaChannel, const CombinMat* combinaisonMatrix)
{
  Pixel *bufferTemp, *ptrBufferTemp, *ptrBufferOut;
  register long i;
  
  // allocate an intermediate buffer to read the rectangle
  unsigned int size = (unsigned int)(pixelsPerLine * rectHeight);
  FastAllocArray(bufferTemp,Pixel,size);
  if (bufferTemp == NULL)
    return FPX_MEMORY_ALLOCATION_FAILED;
  ptrBufferTemp = bufferTemp;
  ptrBufferOut = bufferOut;
  for (i = 0; i < rectHeight; i++, ptrBufferTemp += pixelsPerLine, ptrBufferOut += pixelsPerLine )
    memcpy(ptrBufferTemp, ptrBufferOut, pixelsPerLine*sizeof(Pixel));
    
  // search the corresponding sub-image and call 'ReadInARectangle()' on this sub-image
  FPXStatus status = FPX_OK;
  if (nbSubImages) {
    i = 0;          // Start from the high resolution
    long iWidth  = width; // Width  of the high res in pixels
    long iHeight = height;  // Height of the high res in pixels
    while ((iHeight >= rectHeight) && (iWidth >= rectWidth) && (i <= (nbSubImages-1))) {
      iHeight = (iHeight + 1)/2;
      iWidth  = (iWidth  + 1)/2;
      i++;
    }
    if (i != 0)
      i -= 1;
    subImages[i]->ReadInARectangle(bufferTemp, pixelsPerLine, rectWidth, rectHeight, correctLut, useAlphaChannel, combinaisonMatrix);
  }
  
  short plan = Toolkit_ActiveChannel();
  if ((plan != ActiveChannel_All) && (GtheSystemToolkit->interleaving == Interleaving_Channel)) {
    // Copy Pixel by pixel if planes specified, the destination contains only the choosen channel
    register long j;
    register unsigned char *src, *dst, *ptrCharBufferOut;
    ptrCharBufferOut = (unsigned char *)(bufferOut);
    for (i = 0; i < rectHeight; i++, bufferTemp += pixelsPerLine, ptrCharBufferOut += pixelsPerLine ) {
      src = (unsigned char *)(bufferTemp)  + plan;
      dst = ptrCharBufferOut; 
      for (j = 0; j < pixelsPerLine; j++, src += sizeof(Pixel), dst++)
        *dst = *src;
    }
  } else {
    // copy the buffer line by line
    ptrBufferTemp = bufferTemp;
    ptrBufferOut = bufferOut;
    for (i = 0; i < rectHeight; i++, ptrBufferTemp += pixelsPerLine, ptrBufferOut += pixelsPerLine )
      memcpy(ptrBufferOut, ptrBufferTemp, pixelsPerLine*sizeof(Pixel));
    // interleave according to user's option
    if (GtheSystemToolkit->interleaving != Interleaving_Pixel)
      if (Toolkit_Interleave (bufferOut, pixelsPerLine, rectHeight))
        status = FPX_MEMORY_ALLOCATION_FAILED;
  }
  FastDeleteArray(bufferTemp,Pixel);
  return status;
}


//  ----------------------------------------------------------------------------
// Read size of full resolution image and size of tiles
FPXStatus PHierarchicalImage::GetInfo (long* width, long* height, long* tileWidth, long* tileHeight, Typ_Compression* compr)
{
  *width  = this->width;
  *height = this->height;
  *tileWidth  = this->tileWidth;
  *tileHeight = this->tileWidth;
  *compr  = this->compression;
      
  return FPX_OK;
}

//  ----------------------------------------------------------------------------
FPXStatus PHierarchicalImage::GetTileSize (long* tileSize)
{
  *tileSize = this->tileSize;
  return FPX_OK;
}

//  ----------------------------------------------------------------------------
void* PHierarchicalImage::GetCurrentFile()
{
  return filePtr;
}


//  ----------------------------------------------------------------------------
// Test if there is enough space on disk to create the file
// Prompt the user if not
Boolean PHierarchicalImage::EnoughDiskSpace ()
{
  Boolean enoughSpace = true;
  long channelNumber;
  float compressorRatio;
  return true;
  
  channelNumber = GetNbChannel(baseSpace);
  
  switch (compression) {
  case Compression_None:
    compressorRatio = (float) 1.0;
    break;
  case Compression_NeoTech5:
  case Compression_NeoTech15:
  case Compression_NeoTech25:
    compressorRatio =(float) 0.33;
    break;
  case Compression_QuickTime:
    compressorRatio = (float) 0.33;
    break;
  case Compression_32to24:
    if (existAlphaChannel)
      compressorRatio =(float) 0.75;
    else
      compressorRatio = (float)1.0;
    break;
  case Compression_Other:
    compressorRatio =(float) 0.33;
    break;
  default:
    compressorRatio = (float) 1.0;
    break;
  }
  
  long size = (long) (((float)(width * height * channelNumber * compressorRatio * 4 / 3)/1024.0));

  enoughSpace = fileName.EnoughFreeSpace(size);
  
  return enoughSpace;
}


//  ----------------------------------------------------------------------------
// Set the Boolean existAlphaChannel and test if there is enougth space to use it
Boolean PHierarchicalImage::SetAlphaChannel(Boolean exist)
{
  PRIImage::SetAlphaChannel(exist);
  return (ExistAlphaChannel() ? EnoughDiskSpace() : true);
}
  
//  ----------------------------------------------------------------------------
// Set the decimation type when computing a subresolution
void PHierarchicalImage::SetConvolution (Typ_Convolution newConvolution)
{
  convolution = newConvolution;
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::SetAutomaticDecimation (Boolean automaticDecimationOn)
{
  automaticDecimation = automaticDecimationOn;
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::SetBackgroundColor (FPXBaselineColorSpace colorSpace, Pixel backgroundColor)
{
  // Set the background color to the color passed in parameter
  backgroundBase = backgroundColor;
  backgroundUsed = backgroundColor;

  // Convert the passed color into the required color space
  ConvertPixelBuffer ((unsigned char*)(&backgroundBase), 1, colorSpace, baseSpace);
  ConvertPixelBuffer ((unsigned char*)(&backgroundUsed), 1, colorSpace, usedSpace);
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::SetUsedColorSpace (FPXBaselineColorSpace colorSpace)
{
  // Convert the background color into the new used space
  ConvertPixelBuffer ((unsigned char*)(&backgroundUsed), 1, usedSpace, colorSpace);
  alphaOffset = (unsigned char) GetAlphaOffsetBaseline(colorSpace);
  usedSpace   = colorSpace;
}

//  ----------------------------------------------------------------------------
void PHierarchicalImage::SetBaseColorSpace (FPXBaselineColorSpace colorSpace)
{
  // Convert the background color into the new base space
  ConvertPixelBuffer ((unsigned char*)(&backgroundBase), 1, baseSpace, colorSpace);
  baseSpace = colorSpace;
}

//  ----------------------------------------------------------------------------
FPXStatus PHierarchicalImage::InverseAlpha ()
{
  for (long i = 0; i < nbSubImages; ++i)
    subImages[i]->InverseAlpha();
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Read a pixel of a sub image. Read it on disk if necessary. 
// If error, return false and pixel is set to 0.
// CAUTION : coordinates must be discrete with 12 bits precision (1 << 12).
FPXStatus PHierarchicalImage::ReadMean (long xi, long yi, Pixel& pix, long levelSubImage)
{
  FPXStatus status = FPX_OK;

  if ((Status() == 0) && nbSubImages) {

    if (PRIImage::readInterpolated) {
      // Conversion for ReadMeanInterpolated(): requires continuous coordinates with 8 bits precision
      xi = (xi - 2048) >> 4;
      yi = (yi - 2048) >> 4;
      // Go to the next level for precision of interpolation
      levelSubImage = (levelSubImage >= nbSubImages ? nbSubImages - 1 : levelSubImage - 1);
      if (levelSubImage <= 0)
        levelSubImage = 0;
      // Read the pixel
      status = subImages[levelSubImage]->ReadMeanInterpolated (xi >> levelSubImage, yi >> levelSubImage, pix);
    } else {
      // Conversion for ReadMean(): requires discrete coordinates with 0 bits precision
      xi = xi >> 12;
      yi = yi >> 12;
      // Check level
      if (levelSubImage >= nbSubImages)
        levelSubImage = nbSubImages - 1;
      if (levelSubImage <= 0)
        levelSubImage = 0;
      // Read the pixel
      status = subImages[levelSubImage]->ReadMean (xi >> levelSubImage, yi >> levelSubImage, pix);
    }
    
  } else
    pix = backgroundUsed;
    
  return status;
}


//  ----------------------------------------------------------------------------
// Determine the dispersion of the alpha channel in a neighborhood: the idea is to compare the local value
// to the value of the same pixel on the next resolution level. Because of the convolution from a level to
// another, one can say that if the pixels are equal, there is no dispersion of the alpha channel, otherwise
// there is some. That way, we avoid effective dispersion computation.
// CAUTION : coordinates must be discrete with 12 bits precision (1 << 12).
Boolean PHierarchicalImage::DispersionAlphaChannel (long xNW,
                                                    long yNW,
                                                    long /*xNE*/,
                                                    long /*yNE*/,
                                                    long /*xSW*/,
                                                    long /*ySW*/,
                                                    long xSE,
                                                    long ySE,
                                                    long levelSubImage)
{
  Boolean dispersion = false;
  
  if ((Status() == 0) && nbSubImages) {
  
    /*    The following is the full dispersion algorithm. More computation but less I/O...
 
    Pixel pix[16];
    // Read the sample
    Boolean status = Read4Points(xNW, yNW, xNE, yNE, xSW, ySW, xSE, ySE, pix, levelSubImage);
    
    if (status) {
                // Compute the dispersion with the usual algorithm (see CommandeQ2.cp for explanation)
                unsigned char alphaMax = pix[0].alpha;
                unsigned char alphaMin = pix[0].alpha;
                for (long i = 1; i < 16; ++i) {
                alphaMax &= pix[i].alpha;
                alphaMin |= pix[i].alpha;
                }
                dispersion = (alphaMin ^ alphaMax);
      
                if (!dispersion) {
        // Check if we are on the border to avoid interpolation with the outside of the image
        Boolean onBorder = false;
        if (levelSubImage < 0)
                                levelSubImage = 0;
        if (levelSubImage >= nbSubImages)
                                levelSubImage = nbSubImages - 1;
        long shift = levelSubImage + 12;
        onBorder |= subImages[levelSubImage]->IsOnTheBorder(xNW >> shift, yNW >> shift);
        onBorder |= subImages[levelSubImage]->IsOnTheBorder(xNE >> shift, yNE >> shift);
        onBorder |= subImages[levelSubImage]->IsOnTheBorder(xSW >> shift, ySW >> shift);
        onBorder |= subImages[levelSubImage]->IsOnTheBorder(xSE >> shift, ySE >> shift);
        if (onBorder)
                                dispersion = !(alphaMax == 0);
                                }
                                }
    */
    // Compute central point (>>1) and convert for ReadMean() (>>12)
    // ReadMean() requires discrete coordinates with 0 bits precision
    long xi = (xNW + xSE) >> 13;
    long yi = (yNW + ySE) >> 13;

    // Determine the levels to be compared: the two levels are 2 levels apart to check the
    // stability on a 9x9 neighborhood.
    long nextLevel;

    levelSubImage = levelSubImage - 1;
    if (levelSubImage >= (nbSubImages - 2))
      levelSubImage = nbSubImages - 3;
    if (levelSubImage < 0)
      levelSubImage = 0;
    nextLevel = levelSubImage + 2;
    if (nextLevel >= nbSubImages)
      nextLevel = nbSubImages - 1;
    
    if (levelSubImage != nextLevel) {
      // Read the pixels in each level
      Pixel pix0, pix1;
      Boolean status  = subImages[levelSubImage]->ReadMean (xi >> levelSubImage, yi >> levelSubImage, pix0);
      status &= subImages[nextLevel]->ReadMean (xi >> nextLevel, yi >> nextLevel, pix1);
      
      // Compare them and estimate the dispersion value
      dispersion = status && !(pix0.alpha == pix1.alpha);

      // Comparision for the border pixels: the alpha channel outside the image is supposed to be null, so
      // if the pixel is on the border and is not null, there is a dispersion with the outside of the image.
      dispersion |= ((subImages[nextLevel]->IsOnTheBorder(xi >> nextLevel, yi >> nextLevel)) && pix1.alpha);
    }   
  }
  
  return dispersion;
}


//  ----------------------------------------------------------------------------
// Read a rectangle (x0,y0,x1,y1: coordinates at full resolution) in a map
// The correct sub image is read and sampled in order to fill the map with the rectangle 
// CAUTION : map must be allocated and freed by calling routine
FPXStatus PHierarchicalImage::ReadSampledRectangle(long x0, long y0,long x1, long y1, Pixel* map, short pixelsPerLine, long mapWidth, long mapHeight, Boolean showAlphaChannel, float ratio)
{
  
  if (ratio == 0.0){
    if (mapWidth <= MIN((float) mapHeight,(float) (x1-x0),(float) (y1-y0)) ) {
      if ( ABS(float(float(mapWidth) - float(x1-x0)*float(mapHeight)/float(y1-y0))) > 1.0) {
        assert (false);
        return FPX_BAD_COORDINATES;
      }
    } else if (mapHeight <= MIN((float) mapWidth,(float) (x1-x0),(float) (y1-y0)) ) {
      if ( ABS(float(float(mapHeight) - float(y1-y0)*float(mapWidth)/float(x1-x0))) > 1.0) {
        assert (false);
        return FPX_BAD_COORDINATES;
      }
    } else if (x1-x0 <= MIN((float) mapWidth,(float) mapHeight,(float) (y1-y0)) ) {
      if ( ABS(float(float(x1-x0) - float(mapWidth)*float(y1-y0)/float(mapHeight))) > 1.0) {
        assert (false);
        return FPX_BAD_COORDINATES;
      }
    } else if (y1-y0 <= MIN((float) mapWidth,(float) (x1-x0),(float) mapHeight) ) {
      if ( ABS(float(float(y1-y0) - float(mapHeight)*float(x1-x0)/float(mapWidth))) > 1.0 ) {
        assert (false);
        return FPX_BAD_COORDINATES;
      }
    }
    ratio = MIN(float(float(mapWidth)/float(x1 - x0)) , float(float(mapHeight)/float(y1 - y0)));
  }
    
  if (nbSubImages) {
    long i;
    
    for (i = nbSubImages-1; i>0; --i)
      if ((subImages[i]->realHeight >= FLOAT_TO_LONG(float(height) * ratio)) && (subImages[i]->realWidth >= FLOAT_TO_LONG(float(width) * ratio)))
        break;
        
    // compute the coordinates in the choosed sub image
    if (i) {
      x1 >>= i; y1 >>= i;
      x0 >>= i; y0 >>= i;
      ratio *= (float) (pow(2,(double) i));
    }
    
    // read the rectangle in the correct sub image
    return subImages[i]->ReadSampledRectangle(x0, y0,x1, y1, map, pixelsPerLine, mapWidth, mapHeight, showAlphaChannel, ratio);
  }
  return FPX_OK;
}


//  ----------------------------------------------------------------------------
// Search the top and left corner of the screen pixel which contains the given position
// This function is used when 1 image pixel is represented by more than 1 screen pixel
FPXStatus PHierarchicalImage::SearchPixelTopLeftCorner(long* x1, long* y1, float ratio)
{
  if (nbSubImages) {
    long i;
    for (i = nbSubImages-1; i>0; --i)
      if ((subImages[i]->realHeight >= FLOAT_TO_LONG(float(height) * ratio)) && (subImages[i]->realWidth >= FLOAT_TO_LONG(float(width) * ratio)))
        break;
        
    // compute the coordinates in the choosed sub image
    if (i) 
      ratio *= (float) (pow(2,(double) i));
      
    // read the rectangle in the correct sub image
    return subImages[i]->SearchPixelTopLeftCorner(x1, y1, ratio);
  }
  return FPX_ERROR;
}


//  ----------------------------------------------------------------------------
// Compute histogram for the 4 channels 
FPXStatus PHierarchicalImage::GetHistogram (long* alpha, long* red, long* green, long* blue, long* brightness, const CorrectLut* correctLut)
{
  FPXStatus status = FPX_OK;
  
  if (Status() == 0 && nbSubImages)
    // Compute only on the last sub-imageÐ
    status = subImages[nbSubImages-1]->GetHistogram (alpha, red, green, blue, brightness, correctLut);
  else
    status = FPX_ERROR;
  
  return status;
}

//  ----------------------------------------------------------------------------
// Make a sub resolution level in create mode
PResolutionLevel* PHierarchicalImage::CreateEmptyResolutionLevel(long , long , long* )
{
  return NULL;
} 

//  ----------------------------------------------------------------------------
// Make a sub resolution level in read or write mode
PResolutionLevel* PHierarchicalImage::CreateInitResolutionLevel(long* , long )
{
  return NULL;
} 

//  ----------------------------------------------------------------------------
// Make a resolution list in read or write mode and initialize it
// Set the field 'error' and return it
FPXStatus PHierarchicalImage::CreateInitResolutionLevelList()
{
  return FPX_OK;
} 

//  ----------------------------------------------------------------------------
// Make a resolution list in create mode
FPXStatus PHierarchicalImage::CreateEmptyResolutionLevelList()
{
  return FPX_OK;
} 

//  ----------------------------------------------------------------------------
// Create the "subImages" array of pointers according to the "firstSubImage" list
FPXStatus PHierarchicalImage::InitResolutionLevelsTable()
{ 
  PResolutionLevel* img;
  long i;
  
  // if (nbSubImages == 0) -> the PHierarchicalImage is invalid or impossible to open 
  if (nbSubImages) {
    subImages = new PResolutionLevel*[nbSubImages];
    if (subImages)
      for (i = 0, img = firstSubImage; i < nbSubImages; i++, img = img->next)
        subImages[i] = img;
    else {
      nbSubImages = 0;
    }
  } else {
    // Invalid the PHierarchicalImage
    subImages   = NULL;
    nbSubImages = 0;
  }

  return FPX_OK;
}


//  ----------------------------------------------------------------------------
//  External functions used by C code : 
//  This is the interface of the Standard Toolkit
//  ----------------------------------------------------------------------------
static Boolean DeleteGlobalArrays ()
{
  return tousLesCodecs->Purge();
}


//  ----------------------------------------------------------------------------
Boolean DemandeMemoireUrgente (long , long size)
{ 
#ifdef Memoire_Debug
  // FOR DEBUG PURPOSE
  VISU2 "ENTER IN DEMANDEMEMOIREURGENTE()\n" FIN
    VISU2 "Memory requested : %d\n", size FIN
    TraceMemoire();
  // END DEBUG PURPOSE
#endif
  Boolean aPurge        = false;
  Boolean aPurgeImage     = false;
  Boolean aPurgeLockedImage = false;
  Boolean aPurgeCompress    = false;
  long  amountNowFree;
  long  remain = size;
  
  // Free any tiles not accessed within the last 5 minutes. The hope is that this will
  //  free-up clumps of blocks (i.e., tiles) from a resolution no longer being displayed, etc.
  PTile::FreeAncientBuffers( 2);
  GtheSystemToolkit->AvailableMemory (&amountNowFree);
  
  // For each iteration of the folowing loop, free the oldest, unlocked buffer used to hold
  //  a tile's 'pixels' or 'rawPixels'
  PTile *foundTile;
  long  isRawPixelsBuffer;
  
  while ((amountNowFree < size)
   &&   (PTile::FindOldestTileBuffer( &foundTile, &isRawPixelsBuffer) == 0)) {
    if (isRawPixelsBuffer)
      foundTile->FreeRawPixelsBuffer( );
    else 
      foundTile->FreePixelsBuffer( );
      
    GtheSystemToolkit->AvailableMemory (&amountNowFree);
  }

  if (amountNowFree > size)
    return true;
  
  // Still need more. Purge unlocked image blocks
  PTile::Purge(&remain, 0);
  aPurgeImage = (remain != size ? true : false);
  aPurge = aPurgeImage || aPurge;

  // Purge locked Image if necessary
  if (!aPurge) {
    GtheSystemToolkit->LockImage(NULL);
    PTile::Purge(&remain);
    aPurgeLockedImage = (remain != size ? true : false);
  }
  aPurge = aPurge || aPurgeLockedImage;
  
  // Delete global arrays if nothing else to purge
  if (!aPurge)
    aPurgeCompress = DeleteGlobalArrays ();
  aPurge = aPurgeCompress || aPurge;
  
#ifdef Memoire_Debug
  // FOR DEBUG PURPOSE 
  if (aPurgeImage) {
    VISU2 "Image purged\n" FIN
      } else {
        VISU2 "Image NOT PURGED\n" FIN
          }
  if (aPurgeLockedImage) {
    VISU2 "Locked Image purged\n" FIN
      } else {
        VISU2 "Locked Image NOT PURGED\n" FIN
          }
  if (aPurgeCompress) {
    VISU2 "Global arrays purged\n" FIN
      } else {
        VISU2 "GLOBAL ARRAYS NOT PURGED\n" FIN
          }
  TraceMemoire();
  PrintAllocatedMemoryStat();
  VISU2 "Exit status = " FIN
    if (!aPurge) {
      VISU2 "Error, DemandeMemoireUrgente FAILED\n" FIN
  } else {
          VISU2 "OK\n" FIN
            }
  VISU2 "Exit of DemandeMemoireUrgente()\n\n\n" FIN
    // END DEBUG PURPOSE
#endif

    return aPurge;
}


//  - EOF ----------------------------------------------------------------------
