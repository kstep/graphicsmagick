//  ------------------------------------------------------------------------------------------------
//
//                     TOOLKIT GLOBAL MANAGEMENT
//  MODULE    : SystemIVUE.cpp
//  LANGUAGE  : C++
//  AUTHOR    : Philippe BOSSUT
//  DATE    : Wenesday 22 December 1993 
//  DESCRIPTION :  
//  COMMENT   : This package is a part of the IVUE Toolkit I/O and the FlashPix Baseline I/O
//    SCCSID        : @(#)ri_sys.cpp  1.3 12:49:41 29 Apr 1997
//  ------------------------------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ------------------------------------------------------------------------------------------------
  #include "ri_sys.h"
//  ------------------------------------------------------------------------------------------------

//  Includes
//  --------
  #include  <stdlib.h>
  #include  <string.h>
  #include  <math.h>

#ifndef Debug_h
  #include  "debug.h"
#endif
#ifndef Memoire_h
  #include  "b_memory.h"
#endif

#ifndef BufferDesc_h
  #include  "buffdesc.h"
#endif
  
#ifndef Compresseur_h
  #include  "fpxcompress.h"
#endif

#ifndef OLECommun_h
  #include "olecomm.h"
#endif
#ifndef OLECore_h
  #include "olecore.h"
#endif

#ifndef PHierarchicalImage_h
  #include  "ph_image.h"
#endif
#ifndef PTile_h
  #include  "ptile.h"
#endif


//  Constants
//  ---------
  #define NB_MAX_ERRORS       5

//  Variables
//  ---------

  short ErrorsList::nbErr = 0;
  PSystemToolkit* GtheSystemToolkit = NULL;

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Internal Functions
//  ------------------------------------------------------------------------------------------------

//  ------------------------------------------------------------------------------------------------
//  Extern functions
//  ------------------------------------------------------------------------------------------------
#include <stdlib.h>
/************************************************************************/
/*  Purpose..:  Allocate memory                     */
/*  Input....:  size of memory to allocate                */
/*  Return...:  pointer to memory or 0L                 */
/************************************************************************/
void *FitsNew(size_t /* size */,void *p)
{
  return p;
}

/************************************************************************/
/*  Purpose..:  Allocate memory                     */
/*  Input....:  size of memory to allocate                */
/*  Return...:  pointer to memory or 0L                 */
/************************************************************************/
void *FitsNew(size_t size)
{
   return calloc(size, 1);
}

/************************************************************************/
/*  Purpose..:  Dispose memory                      */
/*  Input....:  pointer to memory or 0L (no action if 0L)       */
/*  Return...:  ---                           */
/************************************************************************/
void FitsDelete(void *ptr)
{
  if (ptr) free(ptr);
}

/************************************************************************/
/*  Purpose..:  Check that GtheSystemToolkit is initalized               */
/*  Input....:                                                           */
/*  Return...:  true if initialized (GtheSystemToolkit != NULL)          */
/************************************************************************/
bool GtheSystemToolkitInitialized(void)
{
  return (GtheSystemToolkit != NULL);
}

//  ------------------------------------------------------------------------------------------------
//  Methods of the PToolkitObject class
//  ------------------------------------------------------------------------------------------------


void* PToolkitObject::operator new (size_t size)
{
  void *ptr;
  
  // Call the redefined new to call calloc from Memoire.cp
  ptr = FitsNew (size);
    
  return ptr;
}


void* PToolkitObject::operator new (size_t size, void *p)
{
  // Call the redefined new to call calloc from Memoire.cp
  return FitsNew (size, p);
}


void PToolkitObject::operator delete (void *p)
{
  // Call the redefined delete to call free from Memoire.cp
  FitsDelete (p);
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the PErrorsList class
//  ------------------------------------------------------------------------------------------------
PErrorsList::PErrorsList(OSErr err, FicNom file)

{
  errorCode = err;
  fileName  = file;
  nextError = NULL;
}


PErrorsList::~PErrorsList()
{
  if (nextError != NULL)
    delete nextError;
}


void PErrorsList::AddErrorToList(OSErr err, FicNom file)
{
  PErrorsList*  last;
  PErrorsList*  tmp = new PErrorsList(err, file);
  if (tmp==NULL) {
    return;
  }
  
  last = this;
  while (last->nextError != NULL)
    last = last->nextError;
  last->nextError = tmp;
}


//  ------------------------------------------------------------------------------------------------
//  Methods of the PSystemToolkit class
//  ------------------------------------------------------------------------------------------------

  // Set the default values of all global options :

PSystemToolkit::PSystemToolkit ()
{
  register int i, n;
  
  tousLesCodecs     = new obj_TousLesCodecs;  // Allocate standard compressors

  openRootStorageList = new List;         // Allocate list for opened OLE root storage
  
  fnctWaitFunc    = NULL;           // Pointer to Wait & Break function
  fnctProgFunc    = NULL;           // Pointer to Wait & Break function
  
  unit        = Unit_inch;        // Unit used by the user in commands
  ratio       = (float) 1.0;        // Conversion ratio
          
  interleaving    = Interleaving_Pixel;   // Global interleaving mode
  
  SetAntialias(Antialias_None);         // Global antialiasing quality
  convolution     = Convolution_Gauss;    // Global convolution quality
  filter        = NULL;           // Pointer to convolution function
          
  preserveBackground  = true;           // Transparency on images edges set
  backgroundColor   = 0;            // Color of the background set to black
  backgroundSpace   = SPACE_32_BITS_RGB;    // Default color space is RGB
  
  userColorCode   = ColorCode_RGB;      // User color model
  fileColorCode   = ColorCode_RGB;      // File color model
  
  existAlpha      = false;
  
  for (i = 0; i < 8; i++) {
    fnctModeltoRGB[i]   = NULL;       // Pointer to Model to RGB function
    fnctRGBtoModel[i]   = NULL;       // Pointer to RGB to Model function
  }
          
  compression     = Compression_None;     // Global compression mode
  fnctCompression   = NULL;           // Pointer to compression function
  fnctDecompression = NULL;           // Pointer to decompression function
  compressionHandle = NULL;           // Pointer to compression option handle
  handleSize      = 0;            // Size in bytes of the compression handle
  activeChannel   = ActiveChannel_All;    // All the channels are active

  for (i = 0, n = 0; n < 1024; n++) {       // Compute Log2 table : used everywhere for subimage level 
    if (n >> (i+1))               // computation
      ++i;
    tableLog2[n] = i;
  }

  GtheSystemToolkit = this;

  SetTileSize (DEFAULT_TILE_SIZE);        // Size of the tiles
  lockedImage = NULL;               // Pointer to in memory locked image
  
  errorsList = NULL;                // Pointer to the fatal errors list
  PErrorsList::nbErr = 0;
  
  manageOLE = FALSE;

}

PSystemToolkit::~PSystemToolkit ()
{
  // Delete all the errors list
  if (errorsList)  delete errorsList;

  // Release standard compressor
  delete tousLesCodecs;
  
  // Delete list for opened OLE root storage
  if (openRootStorageList)  delete openRootStorageList;
  
  // Release the luts and the list of locked Block
  PTile::ClearStaticArrays();
}


// Compute the available memory in the Toolkit
void PSystemToolkit::AvailableMemory (long* size)
{
  *size = 2*tileSize;
}


// Purge all memory owned by the toolkit - including locked blocks, etc.
void PSystemToolkit::PurgeSystem ()
{
  long  usedMemory;
  
  usedMemory = 2*tileSize;
  
  LockImage(NULL);
  if (usedMemory)
    PTile::Purge(&usedMemory, true);
}


// This function purges the required amount of memory. In fact, it purges all blocks
// which are not modified (in write mode) nor locked and then, if necessary to reach
// the required amount, purges the modified tiles (after writing them on disk), and
// if necessary the compression buffers.
// If the passed parameter memoryToBePurged is NULL, PurgeSystemWithAmount() returns
// the memory purgeable by the system (standard behavior with PhotoShop or XPress).
long PSystemToolkit::PurgeSystemWithAmount (long memoryToBePurged)
{
  if (memoryToBePurged == 0) {
    return PTile::GetPurgeableMemory();
  } else {
    long memoryRemaining = memoryToBePurged;
    PTile::Purge(&memoryRemaining, false);
    return (memoryToBePurged - memoryRemaining);
  }
}


long PSystemToolkit::SetTileSize(long newTileSize)
{
  register long status = 0;
  
  if (newTileSize < MIN_TILE_SIZE)
    status = 1;
  else
    tileSize = newTileSize;
    
  // tileSize must be of the form (2^(2n))*sizeof(Pixel)
  tileSize /= sizeof(Pixel);
  long i = 0;
  // Compute the log2 of tileSize
  while (tileSize>>=1) i++; 
  tileWidth = 1<<(i>>1);
  tileSize = tileWidth*tileWidth*sizeof(Pixel);
    
  return(status);
}


long PSystemToolkit::SetWaitFunc (Typ_WaitFunc newWaitFunc)
{
  register long status = 0;

  if (newWaitFunc == NULL)
    status = -1;
  else
    fnctWaitFunc = newWaitFunc;
    
  return(status);
}


long PSystemToolkit::SetProgFunc (FPXProgressFunction newProgFunc)
{
  register long status = 0;

  if (newProgFunc == NULL)
    status = -1;
  else
    fnctProgFunc = newProgFunc;
    
  return(status);
}


long PSystemToolkit::SetUnit (Typ_Unit newUnit)
{
  unit = newUnit;
  switch (unit) {
    case Unit_mm:
      ratio = (float) 1.0;
      break;
    case Unit_inch:
      ratio = (float) 25.4;
      break;
    default:
      ratio = (float) 1.0;
      break;
  }
  return(0);
}


long PSystemToolkit::GetUnit (Typ_Unit* newUnit)
{
  *newUnit = unit;
  return(0);
}


long PSystemToolkit::SetConvolution (Typ_Convolution newConvolution, Typ_Filter newFilter)
{
  register long status = 0;
  
  if (newConvolution == Convolution_Other) {
    if (newFilter == NULL)
      status = -1;
    else {
      filter = newFilter;
      convolution = newConvolution;
    }
  }
  else
    convolution = newConvolution;
    
  return(status);
}


long PSystemToolkit::SetConvolution (Typ_Convolution newConvolution)
{
  register long status = 0;
  
  if (newConvolution == Convolution_Other)
    status = 1;
  else
    convolution = newConvolution;
    
  return(status);
}


long PSystemToolkit::SetAntialias(Typ_Antialias newAntialias)
{
  antialias = newAntialias;
  if (antialias == Antialias_None)
    PRIImage::readInterpolated = false;
  else
    PRIImage::readInterpolated = true;
  return 0;
}


long PSystemToolkit::PreserveBackground (Boolean preserve)
{
  // Update flag
  preserveBackground = preserve; 
  
  return(0);
}


long PSystemToolkit::SetBackgroundColor (Pixel color, FPXBaselineColorSpace colorSpace)
{
  // Update color
  backgroundColor = color; 
  // Convert the passed color into the required color space
  ConvertPixelBuffer ((unsigned char*)(&backgroundColor), 1, colorSpace, backgroundSpace);
  return(0);
}


void PSystemToolkit::SetUsedColorSpace (FPXBaselineColorSpace colorSpace)
{
  // Convert the background color into the new used space
  ConvertPixelBuffer ((unsigned char*)(&backgroundColor), 1, backgroundSpace, colorSpace);
  backgroundSpace = colorSpace;
}


void PSystemToolkit::SetToBackground(ptr_Pixel buffer, long width, long height)
{
  register long i = width * height;
  
  // Set a pixel buffer to background color
  while (i--)
    *buffer++ = backgroundColor;
}


long PSystemToolkit::SetColorConversion (Typ_ColorCode colorCode,   Typ_ModeltoRGB modeltoRGB, 
                                Typ_RGBtoModel RGBtoModel)
{
  register long status = 0;
  
  if (colorCode == ColorCode_RGB)
    status = 1;
  else if ((modeltoRGB == NULL) || (RGBtoModel == NULL))
    status = -1;
  else {
    fnctModeltoRGB[colorCode] = modeltoRGB;
    fnctRGBtoModel[colorCode] = RGBtoModel;
  }
    
  return(status);
}


long PSystemToolkit::SetCompression (Typ_Compression newidCompressor, Typ_Compressor newCompressor, Typ_Decompressor newDecompressor)
{
  register long status = 0;
  
  if (newidCompressor == Compression_Other) {
    if ((newCompressor == NULL) || (newDecompressor == NULL))
      status = -1;
    else {
      fnctCompression = newCompressor;
      fnctDecompression = newDecompressor;
      compression = newidCompressor;
    }
  }
  else
    compression = newidCompressor;
    
  return(status);
}


long PSystemToolkit::SetCompressionHandle(void* newCompressionHandle, long newHandleSize)
{
  register long status = 0;
  
  if (newCompressionHandle == NULL)
    status = -1;
  else if (newHandleSize <= 0)
    status = -2;
  else {
    compressionHandle = newCompressionHandle;
    handleSize = newHandleSize;
  }
    
  return(status);
}


void PSystemToolkit::AddErrorToList(short message, OSErr err, FicNom file)
{

  if (PErrorsList::nbErr >= NB_MAX_ERRORS) {
    // Make room for a new error by purging the first/oldest error from the list
    PErrorsList*  oldest = errorsList;
    errorsList = oldest->nextError;
    delete oldest;
  }

  if (PErrorsList::nbErr < NB_MAX_ERRORS) {
    if (errorsList == NULL) {
      errorsList = new PErrorsList(err, file);
      if (errorsList==NULL) {
        return;
      }
      PErrorsList::nbErr = 1;
    }
    if (errorsList) {
      errorsList->AddErrorToList(err, file);
      PErrorsList::nbErr++;
    }
  }
}


void PSystemToolkit::DeleteErrorsList()
{
  if (errorsList != NULL) {
    delete errorsList;
    errorsList = NULL;
    PErrorsList::nbErr = 0;
  }
}


OSErr PSystemToolkit::GetLastErrorFromList( FicNom file)
{
  PErrorsList*  last;
  OSErr     lastError = 0;
  
  if (errorsList != NULL) {
    last = errorsList;
    while (last->nextError != NULL)
      last = last->nextError;
  }
  return lastError;
}


void PSystemToolkit::SetManageOLE(Boolean theManageOLEOption)
{
  // Caution: If manageOLE was already true, that means that the current PSystemToolkit object has
  // already initialized OLE himself and do need to uninit it at the end too.
  if (manageOLE == FALSE)
    manageOLE = theManageOLEOption;
}


Boolean PSystemToolkit::GetManageOLE()
{
  return manageOLE;
}


//  ------------------------------------------------------------------------------------------------
//  Global Methods  
//  ------------------------------------------------------------------------------------------------

float Toolkit_ConvertToMm (float x)
{
  // Convert from user Unit to mm
  return (x * GtheSystemToolkit->Ratio());
}


float Toolkit_ConvertToUnit (float x)
{
  // Convert from mm to user Unit
  return (x / GtheSystemToolkit->Ratio());
}


// Convert a pixel buffer into an interleaved buffer according to the interleaving option
Boolean Toolkit_Interleave (Pixel* source, long width, long height)
{
  unsigned char *src;
  unsigned char *dest, *dst_1, *dst_2, *dst_3, *dst_4;
  unsigned int size;
  long incline, incchannel;
  
  if (GtheSystemToolkit->interleaving == Interleaving_Pixel)    // Nothing to do–
      return false;

  // allocate an intermediate buffer
  size = (unsigned int)(width*height*sizeof(Pixel));
  FastAllocArray(dest,unsigned char,size);
  if (dest == NULL)
    return true;

  // update parameters according to the interleaving option
  switch (GtheSystemToolkit->interleaving) {
    case Interleaving_Line:
      incchannel = width;
      incline    = 3*width;
      break;
    case Interleaving_Channel:
      incchannel = width*height;
      incline    = 0;
      break;
    case Interleaving_Pixel:
      break;
  }
  
  // init pointers
  dst_1 = dest;
  dst_2 = dst_1 + incchannel;
  dst_3 = dst_2 + incchannel;
  dst_4 = dst_3 + incchannel;
  src = (unsigned char*)(source);
  
  // copy loop
  for (long i = 0; i < height; i++) {
    for (long j = 0; j < width; j++) {
      *dst_1++ = *src++;
      *dst_2++ = *src++;
      *dst_3++ = *src++;
      *dst_4++ = *src++;
    }
    dst_1 += incline;
    dst_2 += incline;
    dst_3 += incline;
    dst_4 += incline;
  }
  
  // recopy buffer after interleaving
  memcpy (source, dest, size);
  
  // free intermediate buffer
  FastDeleteArray(dest, unsigned char);
  
  return false;
}


// Convert an interleaved buffer into a pixel buffer according to the interleaving option
Boolean Toolkit_UnInterleave (Pixel* source, Pixel* dest, long sourceWidth, long sourceHeight, long destWidth, long destHeight)
{
  unsigned char *src_1, *src_2, *src_3, *src_4;
  unsigned char *ptrDest;
  long incline, incchannel;
  long i, j;
  
  if (GtheSystemToolkit->interleaving == Interleaving_Pixel) {    // Nothing to do–
    // recopy buffer after interleaving
    for (i = 0; i <destHeight; i++, dest += destWidth, source += sourceWidth)
      memcpy (dest, source, destWidth*sizeof(Pixel));
    return false;
  }

  // update parameters according to the interleaving option
  switch (GtheSystemToolkit->interleaving) {
  case Interleaving_Line:
    incchannel = sourceWidth;
    incline    = 3*sourceWidth + (sourceWidth - destWidth);
    break;
  case Interleaving_Channel:
    incchannel = sourceWidth*sourceHeight;
    incline    = (sourceWidth-destWidth);
    break;
  default:
    {
    }
  }
  
  // init pointers
  src_1 = (unsigned char*)(source);
  src_2 = src_1 + incchannel;
  src_3 = src_2 + incchannel;
  src_4 = src_3 + incchannel;
  
  // copy loop
  ptrDest = (unsigned char*)(dest);
  for (i = 0; i < destHeight; i++) {
    for (j = 0; j < destWidth; j++) {
      *ptrDest++ = *src_1++;
      *ptrDest++ = *src_2++;
      *ptrDest++ = *src_3++;
      *ptrDest++ = *src_4++;
    }
    src_1 += incline;
    src_2 += incline;
    src_3 += incline;
    src_4 += incline;
  }
  return false;
}

// Copy a rectangle in an image according to interleaving option :
// Caution : both buffers must be sorted in the right interleaved mode as issued by
// IVUE_ReadPage(), IVUE_ReadPageLine(), IVUE_ReadRectangle().
//
// "rectangle" of size "recHeight"x"rectWidth" is to be inserted in "image" of size
// "imageHeight"x"imageWidth" starting at location line "j0" & column "i0" in "image"

Boolean Toolkit_CopyInterleaved (Pixel* image,     long imageWidth, long imageHeight,
                Pixel* rectangle, long rectWidth,  long rectHeight, 
                long i0, long j0)
{
  unsigned int size;
  unsigned char *ima[4];
  unsigned char *rect[4];
  long i, j, increct, incima;
  long cropWidth = rectWidth;
  
  // rectangle must be included in image
  if ((i0 < 0) || (j0 < 0))
      return true;
  if ((i0 + rectWidth) > imageWidth)
      cropWidth = imageWidth - i0;
  if (((j0 + rectHeight) > imageHeight))
      rectHeight = imageHeight - j0;
    
  short plan = Toolkit_ActiveChannel();
  switch (GtheSystemToolkit->interleaving) {
  
    case Interleaving_Pixel:
      image += (j0*imageWidth) + i0;
      if (plan == ActiveChannel_All) {
        // copy lines of pixels
        size = (unsigned int)(cropWidth*sizeof(Pixel));
        for (i = 0; i < rectHeight; i++) {
          memcpy (image, rectangle, size);
          image     += imageWidth;
          rectangle += rectWidth;
        }
      } else {
        for (i = 0; i < rectHeight; i++) {
          register unsigned char *src, *dst;
          src = (unsigned char *)(rectangle)  + plan;
          dst = (unsigned char *)(image) + plan; 
          for (j = 0; j < cropWidth; j++, src += sizeof(Pixel), dst += sizeof(Pixel))
            *dst = *src;
          image     += imageWidth;
          rectangle += rectWidth;
        }
      }
      break;
      
    case Interleaving_Line:
      // copy line by line on each channel
      ima[0]  = (unsigned char *)(image);
      ima[0] += (j0*imageWidth*4) + i0;
      rect[0] = (unsigned char *)(rectangle);

      size    = (unsigned int)(cropWidth);
      increct = 4*rectWidth;
      incima  = 4*imageWidth;
      if (plan == ActiveChannel_All) {
        for (j = 1; j < 4; j++) {
          ima[j]  = ima[j-1] + imageWidth;
          rect[j] = rect[j-1] + rectWidth;
        }
        for (i = 0; i < rectHeight; i++) {
          for (j = 0; j < 4; j++) {
            memcpy (ima[j], rect[j], size);
            ima[j]  += incima;
            rect[j] += increct;
          }
        }
      } else {
        ima[plan]  = ima[0] + plan * imageWidth;
        rect[plan] = rect[0] + plan * rectWidth;
        for (i = 0; i < rectHeight; i++) {
          memcpy (ima[plan], rect[plan], size);
          ima[plan]  += incima;
          rect[plan] += increct;
        }
      }
      break;
      
    case Interleaving_Channel: {
      if (plan == ActiveChannel_All) {
        // copy line by line on each channel
        ima[0]  = (unsigned char *)(image);
        ima[0] += (j0*imageWidth) + i0;
        rect[0] = (unsigned char *)(rectangle);
        for (j = 1; j < 4; j++) {
          ima[j]  = ima[j-1] + (imageWidth*imageHeight);
          rect[j] = rect[j-1] + (rectWidth*rectHeight);
        }
        size    = (unsigned int)(cropWidth);
        increct = rectWidth;
        incima  = imageWidth;
        for (i = 0; i < rectHeight; i++) {
          for (j = 0; j < 4; j++) {
            memcpy (ima[j], rect[j], size);
            ima[j]  += incima;
            rect[j] += increct;
          }
        }
      } else {
        // Copy line by line the chosen channel which is unique in rect and image
        ima[0]  = (unsigned char *)(image);
        ima[0] += (j0*imageWidth) + i0;
        rect[0] = (unsigned char *)(rectangle);
        size    = (unsigned int)(cropWidth);
        increct = rectWidth;
        incima  = imageWidth;
        for (i = 0; i < rectHeight; i++) {
          memcpy (ima[0], rect[0], size);
          ima[0]  += incima;
          rect[0] += increct;
        }
      }
      break;
    }
  }
  
  return false;
}

// Fast Log2 computation : using predefined table in PSystemToolkit for fast subimage level computation
short Toolkit_Log2 (long x)
{
  return (GtheSystemToolkit->tableLog2[x]);
}


Typ_Compression Toolkit_Compression()
{
  return (GtheSystemToolkit->compression);
}


Typ_Convolution Toolkit_Convolution()
{
  return (GtheSystemToolkit->convolution);
}

 
Boolean Toolkit_WriteOnBackground()
{
  return (!(GtheSystemToolkit->preserveBackground));
}


Pixel Toolkit_BackgroundColor()
{
  return (GtheSystemToolkit->backgroundColor);
}


Typ_ColorCode Toolkit_FileColorCode()
{
  return (GtheSystemToolkit->fileColorCode);
}


Typ_ColorCode Toolkit_UserColorCode()
{
  return (GtheSystemToolkit->userColorCode);
}


Typ_ModeltoRGB Toolkit_ModeltoRGB(Typ_ColorCode code)
{
  return (GtheSystemToolkit->fnctModeltoRGB[code]);
}


Typ_RGBtoModel Toolkit_RGBtoModel(Typ_ColorCode code)
{
  return (GtheSystemToolkit->fnctRGBtoModel[code]);
}


long Toolkit_TileWidth()
{
  return (GtheSystemToolkit->tileWidth);
}


short Toolkit_ActiveChannel()
{
  return (GtheSystemToolkit->activeChannel);
}


Boolean Toolkit_IsLockedIVUE (IVUE* ivue)
{
  return (GtheSystemToolkit->IsLockedImage(ivue));
}

