//  ------------------------------------------------------------------------------------------------
//
//                     IMAGE OUTPUT PAGE MANAGEMENT
//
//  This source code is the property of FITS Imaging inc.
//
//  MODULE    : PageIVUE
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Thursday, December 16th 1993
//  DESCRIPTION : 
//  COMMENT   : This package is a part of the IVUE Toolkit I/O and the FlashPix Baseline I/O Toolkit
//      SCCSID        : @(#)ri_page.cpp 1.4 12:41:25 08 Jul 1997
//  ------------------------------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ------------------------------------------------------------------------------------------------
  #include "ri_page.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------

  #include  <stdlib.h>

#ifndef Memoire_h
  #include  "b_memory.h"
#endif
#ifndef Numbers_h
  #include  "numbers.h"
#endif
#ifndef IVUEToolkit_h
  #include "ri_lib.h"
#endif
#ifndef AdvancedIVUE_h
  #include "viewimg.h"
#endif
#ifndef PHierarchicalImage_h
  #include "ph_image.h"
#endif

//  Constants
//  ---------

//  Variables
//  ---------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Internal Functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Methods of the PageImage class
//  ------------------------------------------------------------------------------------------------

PageImage::PageImage (PRIImage* rawImage, long width, long height, float rotation)

{
  // Set the image pointers first
  image          = new ViewImage (rawImage);
  if (image==NULL) {
    return;
  }
  this->rawImage = rawImage;
  
  // Set values
  pixHeight = height; // height of the page in pixels
  pixWidth  = width;  // width of the page in pixels
  numLine   = -1;   // first line number set to an invalid value
  lineSize  = 0;    // length of each line
  buffer    = NULL;   // no buffer allocated...
  line[0]   = NULL;   // ...
  line[1]   = NULL;   // ...
  line[2]   = NULL;   // ...
  line[3]   = NULL;   // ...
  // By default the image covers the page which means that the height of the page is equal to 1.0mm
  // in the image coordinate system. Thus, the resolution of the page is equal to (height)(pixels)/(1.0)(mm)
  resolution  = (float)(height);
  xOrigin   = (float) 0.0;    // the position of the page in the world is on (0,0) by default
  yOrigin   = (float) 0.0;      
  
  // Compute position transformation matrix : Rotate image in page
  TransfoPerspective position;
  ComputeRotationMatrix (&position, rotation);

  // Set the view image position
  image->ApplyTransform(position);
}

PageImage::PageImage (PRIImage* rawImage, long width, long height, float r, float x, float y, TransfoPerspective trans)

{
  // Set the image pointers first
  image          = new ViewImage (rawImage);
  if (image==NULL) {
    return;
  }
  this->rawImage = rawImage;

  // Set values
  pixHeight = height; // height of the page in pixels
  pixWidth  = width;  // width of the page in pixels
  numLine   = -1;   // first line number set to an invalid value
  lineSize  = 0;    // length of each line
  buffer    = NULL;   // no buffer allocated...
  line[0]   = NULL;   // ...
  line[1]   = NULL;   // ...
  line[2]   = NULL;   // ...
  line[3]   = NULL;   // ...
  resolution  = r;    // Set the resolution of the page in pixels per mm
  xOrigin   = x;    // Set the position of the page in the world
  yOrigin   = y;
      
  // Set the view image position
  image->ApplyTransform(trans);
}

PageImage::PageImage (ViewImage* viewImage, long width, long height, float rotation)

{
  // Set the image pointers first
  image     = new ViewImage (viewImage->GetImage());
  if (image==NULL) {
    return;
  }

  rawImage    = viewImage->GetImage();
  
  // Set values
  pixHeight = height; // height of the page in pixels
  pixWidth  = width;  // width of the page in pixels
  numLine   = -1;   // first line number set to an invalid value
  lineSize  = 0;    // length of each line
  buffer    = NULL;   // no buffer allocated...
  line[0]   = NULL;   // ...
  line[1]   = NULL;   // ...
  line[2]   = NULL;   // ...
  line[3]   = NULL;   // ...
  // By default the image covers the page which means that the height of the page is equal to 1.0mm
  // in the image coordinate system. Thus, the resolution of the page is equal to (height)(pixels)/(1.0)(mm)
  resolution  = (float)(height);
  xOrigin   = (float) 0.0;    // the position of the page in the world is on (0,0) by default
  yOrigin   = (float) 0.0;      
  
  // Compute position transformation matrix : Rotate image in page
  TransfoPerspective position;
  ComputeRotationMatrix (&position, rotation);

  // Set the view image position
  image->ApplyTransform(position);
}

PageImage::PageImage (ViewImage* viewImage, long width, long height, float r, float x, float y, TransfoPerspective trans)

{
  // Set the image pointers first
  image     = new ViewImage (viewImage->GetImage());
  if (image==NULL) {
    return;
  }

  rawImage    = viewImage->GetImage();
  
  // Set values
  pixHeight = height; // height of the page in pixels
  pixWidth  = width;  // width of the page in pixels
  numLine   = -1;   // first line number set to an invalid value
  lineSize  = 0;    // length of each line
  buffer    = NULL;   // no buffer allocated...
  line[0]   = NULL;   // ...
  line[1]   = NULL;   // ...
  line[2]   = NULL;   // ...
  line[3]   = NULL;   // ...
  resolution  = r;    // Set the resolution of the page in pixels per mm
  xOrigin   = x;    // Set the position of the page in the world
  yOrigin   = y;

  // Set the view image position
  image->ApplyTransform(trans);
}

PageImage::~PageImage ()

{
  delete image;
  image = NULL;
  
  // Free memory buffer
  if (buffer) {
    FastDeleteArray(buffer,Pixel);
  }
  return;
}

void PageImage::ComputeRotationMatrix (TransfoPerspective* position, float rotation)
{
  PositionMv pointUV;
  PositionMv p[4];

  if (rotation) {
    position->Rotate ((float) 0.0,(float) 0.0, rotation);

    // Inverse position matrix (position is the xy to uv transformation)
    TransfoPerspective uvToxy (*position);
    uvToxy.Inverse();
    
    // Set first corner of image and transform
    pointUV.h = (float) 0.0;
    pointUV.v = (float) 0.0;
    p[0] = uvToxy * pointUV;

    // Set second corner of image and transform
    pointUV.h = (float)(rawImage->width)/rawImage->resolution;
    pointUV.v = (float) 0.0;
    p[1] = uvToxy * pointUV;

    // Set third corner of image and transform
    pointUV.h = (float)(rawImage->width) /rawImage->resolution;
    pointUV.v = (float)(rawImage->height)/rawImage->resolution;
    p[2] = uvToxy * pointUV;

    // Set fourth corner of image and transform
    pointUV.h = (float) 0.0;
    pointUV.v = (float)(rawImage->height)/rawImage->resolution;
    p[3] = uvToxy * pointUV;
    
    // Compute bounding box :
    // init min and max values with the first point
    PositionMv p0 = p[0];
    PositionMv p1 = p[0];
    
    // loop to find min and max values in both direction
    for (short i = 1; i < 4; i++) {
      if (p[i].h < p0.h) p0.h = p[i].h;
      if (p[i].v < p0.v) p0.v = p[i].v;
      if (p[i].h > p1.h) p1.h = p[i].h;
      if (p[i].v > p1.v) p1.v = p[i].v;
    }
    
    // Translate the minimum point of the bounding box to the origin of the page
    position->Translate (-p0.h, -p0.v);
    
    // Scale to fit the page size
    float sh = ((float)(pixWidth)/resolution)/(p1.h - p0.h);
    float sv = ((float)(pixHeight)/resolution)/(p1.v - p0.v);
    float sx = MIN (sh,sv);
    position->Scale ((float) 0.0,(float) 0.0, sx, sx);
  }

  // Note that virtually, the page is 1 mm high, but it still makes
  // sense because we dont't care about the real page size in mm...
  
}


// Read the entire image page       
FPXStatus PageImage::ReadPage (Pixel* image)
{
  Pixel     *pt;
  unsigned char   *dest = NULL;
  FPXStatus     status = FPX_OK;
  
  for (long i = 0; i < pixHeight; i++) {
    // get line pointer from page (read it in file if necessary)
    // PTCH_105 - added following code to implement progress callbacks....
    if ( GtheSystemToolkit->fnctProgFunc)
       if (GtheSystemToolkit->fnctProgFunc( pixHeight, i))
        return FPX_USER_ABORT;
    // PTCH_105 ....end of addition

    if (((pt = ReadLine (i, &status)) == NULL) || (status != FPX_OK)) {
      break;
    } else {
      short plan = GtheSystemToolkit->activeChannel;
      if ((plan != ActiveChannel_All) && (GtheSystemToolkit->interleaving == Interleaving_Channel)) {
        // allocate an intermediate buffer
        if (!dest)
          FastAllocArray(dest,unsigned char,(unsigned int)(pixWidth));
        if (dest == NULL) {
          status = FPX_MEMORY_ALLOCATION_FAILED;
          break;
        }
        // Pixel by pixel if planes specified, the destination contains only the choosed channel
        register long j;
        register unsigned char *src, *dst;
        src = (unsigned char *)(pt)  + plan;
        dst = dest; 
        for (j=0; j<pixWidth;j++,src+=sizeof(Pixel),dst++)
          *dst = *src;
        pt = (Pixel*)(dest);
      } else {
        // convert according to the interleaving option
        if (Toolkit_Interleave (pt, pixWidth, 1)) {
          status = FPX_MEMORY_ALLOCATION_FAILED;
          break;
        }
      }
      if (Toolkit_CopyInterleaved (image, pixWidth, pixHeight, pt, pixWidth, 1, 0, i)) {
        status = FPX_MEMORY_ALLOCATION_FAILED;
        break;
      }
    }
  }
  FastDeleteArray(dest,unsigned char);
  return(status);
}


// Read a line of the page taking into account all global options
FPXStatus PageImage::ReadPageLine (long lineNumber, Pixel* pixLine)
{
  Pixel   *pt;
  FPXStatus   status;
  
  // get line pointer from page (read it in file if necessary)
  pt = ReadLine (lineNumber, &status);

  if (pt && (status == FPX_OK)) {
  
    short plan = GtheSystemToolkit->activeChannel;
    if ((plan != ActiveChannel_All) && (GtheSystemToolkit->interleaving == Interleaving_Channel)) {
      // Pixel by pixel if planes specified, the destination contains only the choosen channel
      register long j;
      register unsigned char *src, *dst;
      src = (unsigned char *)(pt)  + plan;
      dst = (unsigned char *)pixLine; 
      for (j=0; j<pixWidth;j++,src+=sizeof(Pixel),dst++)
        *dst = *src;
    } else {
      // copy pixels in pixLine
      if (plan == ActiveChannel_All) {
        // Copy pixels to the buffer
        BlockMove(pt,pixLine,pixWidth*sizeof(Pixel));
      } else {
        // Pixel by pixel if planes specified
        register long j;
        register unsigned char *src, *dst;
        src = (unsigned char *)(pt)  + plan;
        dst = (unsigned char *)(pixLine) + plan; 
        for (j=0; j<pixWidth;j++,src+=sizeof(Pixel),dst+=sizeof(Pixel))
          *dst = *src;
      }
      // convert according to the interleaving option
      if (Toolkit_Interleave (pixLine, pixWidth, 1))
        status = FPX_MEMORY_ALLOCATION_FAILED;
    }
  }
  
  return(status);
}


// Give a pointer on the pixel line 
// Caution : this function doesn't take interleaving option into account
Pixel* PageImage::ReadLine (long lineNumber, FPXStatus* status)
{
  // test if line has been already read : 
  if ((lineNumber & ~0x3) == numLine) {     // previous multiple by 4 of x is : x & ~0x3
    *status = FPX_OK;
    return (line[lineNumber & 0x3]);      // rest by 4 of x is : x & 0x3
  }
  
  long lineLength = (pixWidth + 3) & ~3;
  // Read 4 lines including the searched one
  // PTCH_105 - removed non-FlashPix call and disabled progress reporting thru this routine
  *status = ReadRectangle(0, lineNumber, lineLength, lineNumber+1, NULL, lineLength, 
        PRIImage::readInterpolated, 0);
  
  // return the buffer pointer to the calling routine
  return (line[lineNumber & 0x3]);            // rest by 4 of x is : x & 0x3
}



// read a rectangle by samples
// bufPix must be allowed before the call 
// PTCH_105 - added 'reportProgress' argument
FPXStatus PageImage::ReadRectangle(long left, long top, long right, long bottom, Pixel* bufPix, 
            long bufWidth, Boolean withAntialias, Boolean reportProgress)
{
  Pixel     pix[16];
  Pixel     pixEmpty[16];
  Pixel   *dest;
  long    i;
  long    j;
  FPXStatus status = FPX_OK;
  const long  left4 = left & ~3;    // 'left' aligned to a multiple of 4
  
  // Allocate buffer if necessary
  long lineLength = ((right+3) & ~3) - left4;
  if (!buffer || (lineSize != lineLength)) {
    if (buffer)
      FastDeleteArray(buffer, Pixel);
    FastAllocArray(buffer,Pixel,4*lineLength);  // size of the buffer : 4 full lines
    if ( buffer == NULL)
      return FPX_MEMORY_ALLOCATION_FAILED;
    line[0] = buffer;
    line[1] = line[0] + lineLength;
    line[2] = line[1] + lineLength;
    line[3] = line[2] + lineLength;
    lineSize = lineLength;
    numLine = -1;
  }
  long    line3 = lineSize*2;
  long    line4 = lineSize*3;
  
  // Set the antialiasing
  Boolean oldMode = PRIImage::readInterpolated;
  if (withAntialias)
    PRIImage::readInterpolated = true;
    
  // Initialize pixEmpty with the backGround
  for (i = 0 ; i < 16 ; i++)
    pixEmpty[i] = GtheSystemToolkit->backgroundColor;
    
  float xOrigin05 = (float) (xOrigin + 0.5/resolution);
  float yOrigin05 = (float) (yOrigin + 0.5/resolution);

  for (j = top; j < bottom; j ++) {
    // PTCH_105 added following to implement progress callbacks....
    if ( reportProgress && GtheSystemToolkit->fnctProgFunc)
       if (GtheSystemToolkit->fnctProgFunc( bottom - top, j - top))
        return FPX_USER_ABORT;
    // PTCH_105 ....end of addition

    if (((j & ~0x3) != numLine) || (j == top)) {
      FPXStatus readStatus;     // CHG_FILE_ERR - used to save intermediate error
      numLine = j & ~0x3;
      
      dest  = buffer;
      for (i = left4 ; i < right; i += 4) {
        // Erase the pix buffer with the background
        memcpy (pix, pixEmpty, (unsigned int)(16*sizeof(Pixel)));

        // Read a sample
        float x0 = (float)(i)  /resolution + xOrigin05;
        float x1 = (float)(i+4)/resolution + xOrigin05;
        float y0 = (float)(numLine)  /resolution + yOrigin05;
        float y1 = (float)(numLine+4)/resolution + yOrigin05;
        if((readStatus = image->Read4x4Points (x0, y0, x1, y1, pix))) // CHG_FILE_ERR - if error
          status = readStatus;                  //  then copy it to 'status'
        BlockMove(pix,dest,4*sizeof(Pixel));
        BlockMove(pix+4,dest+lineSize,4*sizeof(Pixel));
        BlockMove(pix+8,dest+line3,4*sizeof(Pixel));
        BlockMove(pix+12,dest+line4,4*sizeof(Pixel));
        dest+=4;
      }
    }
    if (bufPix) {
      dest  = bufPix + ((j-top)*bufWidth);
      BlockMove(line[j & 0x3]+(left & 0x3),dest,(right-left)*sizeof(Pixel));
    }
  }

  // reset antialiasing
  PRIImage::readInterpolated = oldMode;
  
  return status;
}

