/*************************************************************************
 *
 *     Program: FPX Filtering example and C code.
 *
 *     Purpose: This listing contains a description of a calling application,
 *    and blurring and sharpening C code. The filtering code includes 
 *    a convolution routine adapted from fast convolution code by 
 *    Wolberg and Massalin.
 *
 *
 *
 *************************************************************************/
//  SCCSID      : @(#)filter.cpp  1.2 15:03:03 27 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "ptile.h"
#include "ptil_fpx.h"

#ifndef Memoire_h
  #include  "b_memory.h"
#endif

#ifndef AdvancedIVUE_h
  #include "viewimg.h"
#endif

#ifndef PHierarchicalImage_h
  #include "ph_image.h"
#endif

#ifndef PResolutionLevel_h
  #include "pr_level.h"
#endif

#ifndef PResolutionFlashPix_h
  #include "pres_fpx.h"
#endif

/* Defines.
 */
#define MAXKERNNUM  50      // Maximum number of kernels which can be defined.
#define MAXRESLEV 10      // Maximum number of levels allowed in resolution hierarchy

#define NOKERNEL  -2
   
#define RESCALE_MTF_HEIGHT 1000 // height of image to start MTF adjustment



/*************************************************************************
 *
 *  Program: Includes all the resolution independent filtering routines
 *    including convolution code.
 *
 *  List of Functions:
 *    applyFilter()
 *    unsharpMask()
 *    blurFilter()
 *    convolve()
 *    initPackedLut()
 *    fastconv()
 *
 *************************************************************************/
/* Macro definitions.
 */
#define CLAMP(A,L,H)  ((A)<=(L) ? (L) : (A)<=(H) ? (A) : (H))
#define ABS(A)          ((A) >= 0 ? (A) : -(A))
#define Z(k)            ( (k == 0) ? 0 : 1 )
#define MASK            0x3FF
#define ROUNDD          1
#define PACK(A,B,C)     (((A)<<20) + ((B)<<10) + (C))
#define INTC(A)          ((long) ((A)*262144+32768) >> 16)

const double log_ratio = 1.097610796626;    // log(2.14)/log(2.0)


/* Constants.
 */
#define B_AMT   40.0




// The algorithm is based upon creating a lut(s) which contain pre-multiplied values. These
//  values are based upon the weighting values in the filter kernel. Typically, the filter
//  kernel does not change from one tile to the next in the same image/resolution. So. as a 
//  simple optimization, the most recently generated lut(s) is saved statically. The kernel
//  values are also saved. If the filter (i.e., the kernel values) are the same as those 
//  used for the current lut(s), then the saved lut(s) can be used again. Otherwise, a new
//  set of lut values is generated in initPackedLut().
//
static lutS   gLuts;        // Saved LUTs from a previous filtering operation
static double gKernelValue[9];  // A 9-point filter is the largest being used presently
static long   gKernelSize;    // Number of values stored in 'gKernelValue'



 /* -----------------------------------------------------------------------
 * applyFilter
 *
 * Apply image filtering using acutance. This is the main filtering
 * routine. The image hierarchy, filter parameters amd gaussian kernels
 * are passed as arguments to the function and the appropriate level is
 * either sharpened or blurred as specified in the change of acutance. The
 * output image should be resized to the required output height by the
 * calling application.
 *
 * Return -1 on error.
 *
 */
FPXStatus PTileFlashPix::ApplyFilter(FPXBaselineColorSpace colorSpace)
{
  double    total;
  filtParmS   FiltP;
  ViewImage   *imageParam = fatherSubImage->fatherFile->imageParam;
  float     delta_A;
  long    h, w, nbTilesWidth, nbTilesHeight;
  FPXStatus status;


  /*  Set the following filter parameters:
    FiltP.capture   --  default  7.0
    FiltP.prefilter   --  default  10.0 for a good filter.
              for Livepicture's reference implementation kernels:
              Kernel: (1/6 1/3 1/3 1/6) use  24.0
              Kernel: (1/2 1/2)   use  7.0
    FiltP.printer   --  If the destination output is:
            xls8600 thermal printer   use  12.0
            Sun Trinitron 19in        use   6.0
    FiltP.fraction    --  default 1.0
    FiltP.height_p    --  height in pixels of the image requested by
                the application. eg. height of display
                window in pixels.
    FiltP.height_o    --  height in pixels of the full resolution image
            in the hierarchy eg. in[0].lines.
    FiltP.delta_A   --  change in acutance specified by the user
    FiltP.color_space --  RGB = filter all three channels using same filter.
              YCC = filter luma channel only.
   */
  FiltP.capture   = 7.0;
  FiltP.prefilter = 10.0;
  FiltP.printer   = 6.0;
  FiltP.fraction  = 1.0;

  FiltP.level = fatherSubImage->identifier;

  fatherSubImage->GetResolutionSizeInfo (&w, &h, &nbTilesWidth, &nbTilesHeight);
  FiltP.height_r = ( w < h ? w : h);

  // We consider that the tile size is the display resolution (otherwise, we won't take that tile...)
  FiltP.height_p = FiltP.height_r;

  // Get size of highest resolution image
  fatherSubImage->fatherFile->firstSubImage->GetResolutionSizeInfo (&w, &h, &nbTilesWidth, &nbTilesHeight);
  FiltP.height_o = ( w < h ? w : h); 
  imageParam->GetFiltering(&delta_A);
  FiltP.delta_A = (double)delta_A;
  FiltP.colorSpace = colorSpace;

  // Set total system MTF. Resolution level to filter has been set in FiltP.level,
  // original image height set in FiltP.height_o and picture height at resolution
  // level is set in FiltP.height_r. Note that all these were set by the calling
  // application.
  //
  total = FiltP.capture / pow(4.0, (double)FiltP.level) 
    + Z(FiltP.level)*FiltP.prefilter/4.0
    + FiltP.printer*(double)(FiltP.height_r*FiltP.height_r)
       / (double)(FiltP.height_p*FiltP.height_p) 
    + 5.1e-6 * pow((double)FiltP.height_r/FiltP.fraction, 2.0);
   
    // Adjust the total MTF to prevent lower resolution images from being
    // over blurred or sharpened
    if (FiltP.height_p < RESCALE_MTF_HEIGHT) {  // apply rescaling
      double rescale = 0.94 * pow((double)FiltP.height_p/RESCALE_MTF_HEIGHT,log_ratio) + 0.06;
      total = total*rescale;
    }

  FiltP.total = total;

  if (FiltP.delta_A > 0) {    /* Apply unsharp masking */
    if ((status = UnsharpMask(&FiltP)) != FPX_OK) {
//      fprintf(stderr, "Error in unsharpMask()\n");
      return status;
    }
  } else if (FiltP.delta_A < 0) { /* Apply blur filter */
    if ((status = BlurFilter(&FiltP))  != FPX_OK) {
//      fprintf(stderr, "Error in blurFilter()\n");
      return status;
    }
  }
  return status;
}



/* -----------------------------------------------------------------------
 * unsharpMask
 *
 * Apply unsharp masking to input image. The following equation is used:
 *
 *  f'(x) = f(x) + beta*(f(x) - f(x)@G(g,x))  @ == convolved with
 *
 * where f(x) is the input image, G(g,x) is a Gaussian function of width g,
 * beta is a scaling factor and f'(x) the sharpened image. beta is chosen as the
 * value, lying between 0 and 5 which allows the smallest width Gaussian filter 
 * to give the required sharpening defined by the change in acutance.
 * 
 * Returns: FPX_COLOR_CONVERSION_ERROR
 *      FPX_MEMORY_ALLOCATION_FAILED
 */
FPXStatus PTileFlashPix::UnsharpMask(filtParmS *FiltP)
{
  // ====================< Declarations >==========================
  long  n, k;         // Counters
  double  g_width, beta;      // selected Gaussian width and boost factor
  double  delta_A;        // Change in acutance
  double  maxBeta, total;     // intermediate values
  long  kernelNum;        // Index of selected Gaussian kernel

  long  sum, pixVal;      // holders
  firS  Gauss[MAXKERNNUM];    // Table of available gaussian kernels

  long  paramSet = 0;     // boolean to determine if a valid kernel was found
  long  nc;           // Number of components in tile
  long  NumKernels;       // The number of kernels in the database

  unsigned char* tempcomp[4];   // Pointer to the image components in temp, i.e RGBA
  unsigned char* srcComp[4];    // Pointers to components in paddedTile
  unsigned char* dstComp[4];    // Pointers to components in output tile (tile->pixels)

  Pixel*  paddedTile = NULL;    // Holds the minimum necessary pixels to filter
  long  pad;          // number of pixels to pad each edge of the tile
  FPXStatus status = FPX_OK;    // Return status


  // Initialise Gaussian filter database.
  if (GetFilterKernels(Gauss, &NumKernels) == -1)
    return FPX_COLOR_CONVERSION_ERROR;

  // Set maximum Gaussian width which has been defined
  g_width = 0;
  for (n = 0; n < NumKernels; n++) {
    if (g_width < (Gauss+n)->width) {
      g_width = (Gauss+n)->width;
    }
  }
  
  // Set total system MTF
  total = FiltP->total;

  // For all Gaussian widths which give suitable beta's (between 1 and 5)
  // choose the filter with the smallest width.
  delta_A = FiltP->delta_A;
  double  tmpDelta = pow(10.0, delta_A/B_AMT) - 1.0;
  for (n = 0; n < NumKernels; n++) {
    beta = tmpDelta / (1.0 - sqrt(total)/sqrt(total + (Gauss+n)->width));
    if (beta <= 5.0 && beta >= 0) {       // Valid range of beta
      if (g_width >= (Gauss+n)->width) {
        g_width = (Gauss+n)->width;
        kernelNum = n;
        maxBeta = beta;
        if (!paramSet) paramSet = 1;
      }
    }
  }

  if (!paramSet) {          // Couldn't find a suitable Gaussian
    status = FPX_COLOR_CONVERSION_ERROR;
    goto RETURN;
  }
  beta = maxBeta;

  // init packed luts
  if (InitPackedLuts( (Gauss+kernelNum)->kernel, (Gauss+kernelNum)->elements) != 0)
    return FPX_COLOR_CONVERSION_ERROR;            // Probably an overflow condition

  // Need to allocate at least 1/2 the filter width on each edge
  pad = (short)((Gauss+kernelNum)->elements) - 1;

  if ((status = makePaddedRawPixels( pad, &paddedTile)) != FPX_OK)
    goto RETURN; 

  // Determine channel #'s in color space 
  // Alpha channel is not filtered
  switch(FiltP->colorSpace) {
    case  SPACE_32_BITS_RGB:      // The 24 bits are stored in the LSB part of the long
    case  SPACE_32_BITS_ARGB:
    case  SPACE_32_BITS_YCC:      // DAG added following for YCC
    case  SPACE_32_BITS_AYCC:
      {
      srcComp[0] = ((unsigned char*)paddedTile) + 1; 
      srcComp[1] = ((unsigned char*)paddedTile) + 2;  
      srcComp[2] = ((unsigned char*)paddedTile) + 3;
  
      tempcomp[0] = ((unsigned char*)rawPixels) + 1;
      tempcomp[1] = ((unsigned char*)rawPixels) + 2;
      tempcomp[2] = ((unsigned char*)rawPixels) + 3;
      
      dstComp[0] = ((unsigned char*)pixels) + 1; 
      dstComp[1] = ((unsigned char*)pixels) + 2;  
      dstComp[2] = ((unsigned char*)pixels) + 3;
  
      nc=3;
      } break;
    case  SPACE_32_BITS_RGBA: 
    case  SPACE_32_BITS_YCCA:     // DAG added following for YCC
      srcComp[0] = ((unsigned char*)paddedTile); 
      srcComp[1] = ((unsigned char*)paddedTile) + 1;  
      srcComp[2] = ((unsigned char*)paddedTile) + 2;
  
      tempcomp[0] = ((unsigned char*)rawPixels);
      tempcomp[1] = ((unsigned char*)rawPixels) + 1;
      tempcomp[2] = ((unsigned char*)rawPixels) + 2;
      
      dstComp[0] = ((unsigned char*)pixels); 
      dstComp[1] = ((unsigned char*)pixels) + 1;  
      dstComp[2] = ((unsigned char*)pixels) + 2;
      nc=3;
      break;
    case  SPACE_32_BITS_M:    
    case  SPACE_32_BITS_AM:
      srcComp[0]  = ((unsigned char*)paddedTile) + 3;
      tempcomp[0] = ((unsigned char*)rawPixels) + 3;
      dstComp[0]  = ((unsigned char*)pixels) + 3;
      nc=1;
      break;
    case  SPACE_32_BITS_MA:
      srcComp[0]  = ((unsigned char*)paddedTile) + 2;
      tempcomp[0] = ((unsigned char*)rawPixels) + 2;
      dstComp[0]  = ((unsigned char*)pixels) + 2;
      nc=1;
      break;
    default:
      // invalid colorspace
      status = FPX_COLOR_CONVERSION_ERROR;
      goto RETURN;
  }
  
  // Sharpen image. Each pass does one of the components
  // We Convolve() the padded tile into a normal-sized temp tile, then sum the
  //  convolved output back into the original, raw data and put the result into
  //  'pixels'
  for (k = 0; k < nc; k++) {
    if ((status = Convolve( srcComp[k], pad, &gLuts, dstComp[k])) != FPX_OK)
      goto RETURN;
    
    unsigned char *padP, *tmpP, *dstP;
    long      x, y,
            padWid = (width + (2 * pad));     // Width of padded line in Pixels

    for (y = 0; y < height; y++) {
      padP = srcComp[k]  +(((y + pad) * padWid) * sizeof( Pixel)) + (pad * sizeof( Pixel));
      tmpP = tempcomp[k] + ((y * width)         * sizeof( Pixel));
      dstP = dstComp[k]  + ((y * width)         * sizeof( Pixel));
  
      for (x = 0; x < width; x++) {
        pixVal = (long)*tmpP;
        sum = pixVal + (long)(beta*(double)(pixVal - (long)*dstP) + 0.5);
        *dstP = (unsigned char)CLAMP(sum, 0, 255);
        padP += sizeof( Pixel);
        tmpP += sizeof( Pixel);
        dstP += sizeof( Pixel);
      }
    }
  }

 RETURN: 
  // Added this to make sure all memory is recovered on return
  // It's easier to have one return point than to maintain
  // multiple returns

  if (paddedTile) 
    FastDeleteArray( paddedTile, Pixel);
  return status;
}

/* -----------------------------------------------------------------------
 * blurFilter
 *
 *
 * Apply blurring to input image. The following equation is used:
 *
 *  f'(x) = f(x)@((1 - beta)*G(g1, x) + beta*G(g2, x)]
 *
 *
 * where f(x) is the input image, G(g1, x) and G(g2, x) are gaussian functions of
 * width g1 and g2 respectively, beta is a scaling factor and f'(x) the blurred
 * image. Beta is chosen as the value, lying between 0 and 1 which allows the
 * smallest width Gaussian filters to give the required blurring effect as
 * defined by the change in acutance.
 * 
 * The Gaussian widths g1 and g2 should be of the order of 1 to 10 magnitudes
 * apart. For example the following combinations are allowed:
 *  g1: 0.1 8 20  50
 *  g2: 10  25  60  150
 *
 * Returns: FPX_COLOR_CONVERSION_ERROR
 *      FPX_MEMORY_ALLOCATION_FAILED
 *
 *
 */
FPXStatus PTileFlashPix::BlurFilter(filtParmS *FiltP)
{
  long  n, n1, n2, k;
  double  g1_width, g2_width, beta, oneMbeta;
  double  maxBeta, delta_A;
  double  widthg1, widthg2, rootg1, rootg2, sumwidth;
  double  total;
  long  kernelNum1, kernelNum2;

  firS  Gauss[MAXKERNNUM];

  double  blurKernel[9];
  long  blurElements;

  long  paramSet = 0;
  long  nc;
  long  NumKernels;

  unsigned char* srcComp[4];  // Table of pointers to components
  unsigned char* dstComp[4];
    Pixel*  paddedTile = NULL;    // Holds the minimum necessary pixels to filter
  long  pad;          // number of pixels to pad each edge of the tile

  FPXStatus status = FPX_OK;    // Return status


  if (GetFilterKernels( Gauss, &NumKernels) == -1)
    return FPX_COLOR_CONVERSION_ERROR;

  /* Set maximum Gaussian width which has been defined */
  g1_width = 0;
  for (n = 0; n < NumKernels; n++) {
    if (g1_width < (Gauss+n)->width) {
      g1_width = (Gauss+n)->width;
    }
  }
  g2_width = g1_width;
  sumwidth = g2_width + g1_width;

  /* Set total system MTF */
  total = FiltP->total;

  /* For all Gaussian widths which give suitable beta's (between 0 and 1)
   * choose the set of filter pairs with the smallest sum of widths.
   */
  delta_A = FiltP->delta_A;
  double  tmpDelta = pow(10.0, delta_A/B_AMT);
  
  for (n1 = 0; n1 < NumKernels; n1++) {
    for (n2 = 0; n2 < NumKernels; n2++) {
    widthg1 = (Gauss+n1)->width;
    widthg2 = (Gauss+n2)->width;
    if (widthg1 != widthg2 && widthg2 > widthg1 && widthg2 < 10.0*widthg1) {
      rootg1 = sqrt(total + widthg1);
      rootg2 = sqrt(total + widthg2);

      beta = (tmpDelta * rootg1 * rootg2 /
          sqrt(total) - rootg2)/(rootg1 - rootg2);

      if (beta <= 1.0 && beta >= 0) {   /* Valid range of beta */
        if (sumwidth > widthg2 + widthg1) {
          sumwidth = widthg2 + widthg1;
          g1_width = widthg1;
          g2_width = widthg2;
          kernelNum1 = n1;
          kernelNum2 = n2;
          maxBeta = beta;
          if (!paramSet) paramSet = 1;
        }
      }
    }
     }
  }

  if (!paramSet) {
    // Failed to find a suitable filter, so simply return the data unchanged (in this
    //  case, copy the data from 'rawPixels' to the 'pixels' buffer.
    memcpy( pixels, rawPixels, width * height * sizeof(Pixel) );
    return FPX_OK;
  }
  
  beta = maxBeta;
  oneMbeta = 1.0 - beta;

  // Set up blurring kernel and number of elements
  for (n=0; n < 9; n++) blurKernel[n] = (double)beta*(Gauss+kernelNum2)->kernel[n] + 
    (double)(oneMbeta)*(Gauss+kernelNum1)->kernel[n];

  if ((Gauss+kernelNum2)->elements > (Gauss+kernelNum1)->elements) {
    blurElements = (Gauss+kernelNum2)->elements;
  } else {
    blurElements = (Gauss+kernelNum1)->elements;
  }

  // init packed luts for filter kernel
  if (InitPackedLuts( blurKernel, blurElements) != 0)
    return FPX_COLOR_CONVERSION_ERROR;            // Probably an overflow condition

  pad = (3 * (gLuts.stages)) - 1;

  if ((status = makePaddedRawPixels( pad, &paddedTile)) != FPX_OK)
    goto RETURN; 
    
  // Determine channel #'s in color space 
  // Alpha channel is not filtered
  switch(FiltP->colorSpace) {
    case  SPACE_32_BITS_RGB:  // The 24 bits are stored in the LSB part of the long
    case  SPACE_32_BITS_ARGB:
    case  SPACE_32_BITS_YCC:  // DAG added following for YCC
    case  SPACE_32_BITS_AYCC:
      srcComp[0] = ((unsigned char*)paddedTile) + 1; 
      srcComp[1] = ((unsigned char*)paddedTile) + 2;  
      srcComp[2] = ((unsigned char*)paddedTile) + 3;
  
      dstComp[0] = ((unsigned char*)pixels) + 1;
      dstComp[1] = ((unsigned char*)pixels) + 2;
      dstComp[2] = ((unsigned char*)pixels) + 3;
      nc=3;
      break;
    case  SPACE_32_BITS_RGBA: 
    case  SPACE_32_BITS_YCCA:   // DAG added following for YCC
      srcComp[0] = ((unsigned char*)paddedTile); 
      srcComp[1] = ((unsigned char*)paddedTile) + 1;  
      srcComp[2] = ((unsigned char*)paddedTile) + 2;
  
      dstComp[0] = ((unsigned char*)pixels);
      dstComp[1] = ((unsigned char*)pixels) + 1;
      dstComp[2] = ((unsigned char*)pixels) + 2;
      nc=3;
      break;
    case  SPACE_32_BITS_M:    
    case  SPACE_32_BITS_AM:
      srcComp[0] = ((unsigned char*)paddedTile) + 3;
      dstComp[0] = ((unsigned char*)pixels) + 3;
      nc=1;
      break;
    case  SPACE_32_BITS_MA:
      srcComp[0] = ((unsigned char*)paddedTile) + 2;
      dstComp[0] = ((unsigned char*)pixels) + 2;
      nc=1;
      break;
    default:
      // invalid colorspace
      status = FPX_COLOR_CONVERSION_ERROR;
      goto RETURN;
  }

  // Blur image
  for (k = 0; k < nc; k++) {
    if ((status = Convolve( srcComp[k], pad, &gLuts, dstComp[k])) != FPX_OK)
      goto RETURN;
  }


  RETURN:
  if (paddedTile) 
    FastDeleteArray( paddedTile, Pixel);
  return status;
}




/* ======================================================================
 *
 *  Program: Fast Convolution 
 *
 *  adapted from:
 *  "Fast Convolution with Packed Lookup Tables" Wolberg G. and
 *  Massalin H., in Heckbert P (editor), "Graphics Gems IV", 
 *  Academic Press, 1994, pages 447-464
 *
 *
 * ======================================================================
*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * convolve:
 *
 * Convolve input image I1 with kernel, a (2n-1) -point symmetric filter
 *  kernel containing n entries: h[i] = kernel[ |i| ] for -n < i < n.
 *  Output is stored in I2.
 * It is assumed that the 'in' buffer has been padded on all sides by the amount
 *  indicated by 'pad'. The 'out' buffer does not contain any padding, thus the 
 *  'in' and 'out' buffers are different sizes.
 */
FPXStatus PTileFlashPix::Convolve( unsigned char *in, long pad, lutS *luts, unsigned char *out)
{
  long      x, y, paddedWidth, paddedHeight;
  unsigned char *src, *dst;
  long      srcInc, dstInc;
  unsigned char *temp;

  paddedWidth  = width  + (2 * pad);
  paddedHeight = height + (2 * pad);
  
  // Allocate the temporary buffer to hold the 1st pass
  temp = new unsigned char [width * sizeof( Pixel) * paddedHeight]; 
  if (temp == NULL)
    return FPX_MEMORY_ALLOCATION_FAILED;
  
  src = in;
  dst = temp;
  srcInc = paddedWidth * sizeof( Pixel);
  dstInc =    width    * sizeof( Pixel);
  
  for (y = 0; y < paddedHeight; y++) {          // process all rows 
    Fastconv( src, width, pad, sizeof( Pixel), luts, dst);
    src += srcInc;
    dst += dstInc;
  }

  src = temp;
  dst = out;
  dstInc = width * sizeof( Pixel);

  for (x = 0; x < width; x++) {             // process all columns
    Fastconv(src, height, pad, dstInc, luts, dst);
    src += sizeof( Pixel);
    dst += sizeof( Pixel);
  }

  /* free temporary image */
  delete [] temp;
  
  return FPX_OK;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * initPackedLuts:
 *
 * Initialize scaled and packed lookup tables in lut.
 * Permit up to 3 cascaded stages for the following kernel sizes:
 *  stage 0:   5-point kernel
 *  stage 1:  11-point kernel
 *  stage 2:  17-point kernel
 * lut->lut0 <== packed entries (i*k2, i*k1, .5*i*k0), for i in [0, 255]
 * lut->lut1 <== packed entries (i*k5, i*k4,    i*k3), for i in [0, 255]
 * lut->lut2 <== packed entries (i*k8, i*k7,    i*k6), for i in [0, 255]
 * where k0,...,k8 are taken in sequence from kernel[].
 *
 * note that in lut[0], k0 is halved since it corresponds to the center
 * pixel's kernel value and it appears in both fwd0 and rev0 (see text).
 *
 * The lut(s) generated are in the static 'gLuts'. This lut is saved between calls along
 *  with the kernel used to generate it. If the kernel values passed-in match those used
 *  for the current lut, then the old lut is used; otherwise, new lut values are generated.
 */
FPXStatus PTileFlashPix::InitPackedLuts(double *kernel, long n)
{
  long  i, k, s, *lut;
  long  b1, b2, b3;
  double  k1, k2, k3;
  double  sum;
  lutS  *luts = &gLuts;

  // If the kernel passed-in is the same as that used to generate the current luts,
  //  then just return.
  for (i=0; i < n; i++)
    if (kernel[i] != gKernelValue[i])
      break;
  if (i >= n)
    return FPX_OK;



  /* enforce flat-field response constraint: sum of kernel values = 1*/
  sum = kernel[0];
  for (i=1; i<n; i++) sum += 2*kernel[i];    /* account for symmetry */
  if (ABS(sum - 1) > 0.001) {
//    fprintf(stderr, "Warning: filter sum != 1 (=%f)\n", sum);
    }

  /* init bias added to fields to avoid negative numbers (underflow) */
  luts->bias = 0;

  /* set up lut stages, 3 kernel values at a time */
  for (k = s = 0; k < n; s++) {     /* init lut (stage s) */
    k1 = (k < n) ? kernel[k++] : 0;
    k2 = (k < n) ? kernel[k++] : 0;
    k3 = (k < n) ? kernel[k++] : 0;
    if (k <= 3) k1 *= 0.5;        /* kernel[0]: halve k0  */

    /* select proper array in lut structure based on stage s */
    switch (s) {
      case 0: lut = luts->lut0; break;
      case 1: lut = luts->lut1; break;
      case 2: lut = luts->lut2; break;
    }

    /* check k1, k2, k3 to avoid overflow in 10-bit fields */
    if (ABS(k1) + ABS(k2) + ABS(k3) > 1) {
//      fprintf(stderr, "|%f|+|%f|+|%f| > 1\n", k1, k2, k3);
      return FPX_COLOR_CONVERSION_ERROR;
    }

    /* compute bias for each field to avoid underflow */
    b1 = b2 = b3 = 0;
    if (k1 < 0) b1 = (long)(-k1*1024.0);
    if (k2 < 0) b2 = (long)(-k2*1024.0);
    if (k3 < 0) b3 = (long)(-k3*1024.0);

    /* luts->bias will be subtracted in fastconv() after adding
     * stages; multiply by 2 because of combined effect of fwd
     * and rev terms
     */
    luts->bias += 2*(b1 + b2 + b3);

    /* scale and pack kernel values in lut */
    for (i=0; i < 256; i++) {
      /*
       * INTC(A) forms fixed point field:
       * (A*(1<<18)+(1<<15)) >> 16
       */
      lut[i] = PACK(  INTC(i*k3) + b3,
          INTC(i*k2) + b2 + ROUNDD,
          INTC(i*k1) + b1 );
    }
  }
  luts->stages = s;
  
  // Save the kernel for testing next time the routine is called
  for (i=0; i < n; i++)
    gKernelValue[i] = kernel[i];
  gKernelSize = n;

  return FPX_OK;
}
 



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fast 1D convolver.
 * Convolve len input samples in src with a symmetric kernel packed in luts,
 * a lookup table that is created by initPackedLuts() from kernel values.
 * It is assumed that 'src' has been padded at both ends by 'padLen', so it's
 *  width is 'len' + (2 * 'padLen'). 'padLen' is based upon the number of stages
 *  in 'luts', not the kernel size.
 * It is assumed that 'dst' does not have room for padding.
 * 'inStride' is the stride/offset from one value to be convolved to the next in 
 *  the input buffer. Note that only one component is convolved by this routine.
 * 'outStride' is the stride/offset from one value to be convolved to the next in 
 *  the output buffer, 'dst'.
 */
void PTileFlashPix::Fastconv(unsigned char *src, long len, long padLen, long stride,
            lutS *luts, unsigned char *dst)
{
  long      x, val, bias;
  long      fwd0, fwd1, fwd2;
  long      rev0, rev1, rev2;
  long      *lut0, *lut1, *lut2;
  unsigned char *p1, *p2, *ip, *op;
  unsigned char buf[4096];


  // copy src into buf 
  p1 = src;               // pointer to row (or column) of input
  p2 = buf;               // pointer to row of padded buffer
  for (x=0; x< (len + (2 * padLen)); x++) {
    *p2++ = *p1;
    p1 += stride;
  }

  // Initialize input and output pointers, ip and op, respectively
  ip = buf;
  op = dst;

  // bias was added to lut entries to deal with negative kernel values
  bias = luts->bias;

  switch(luts->stages) {
  case 1: // 5-pt kernel
    lut0 = luts->lut0;

    ip += 2;                  // ip[0] is center pixel
    fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
    rev0 = (lut0[ip[0]] << 10) + lut0[ip[1]];

    while(len--) {
      fwd0 = (fwd0 >> 10) + lut0[ip[0]];
      rev0 = (rev0 << 10) + lut0[ip[2]];
      val = ((fwd0 & MASK) + ((rev0 >> 20) & MASK) - bias)
        >> 2;
      *op = CLAMP(val, 0, 255);
      ip++;
      op += stride;
    }
    break;
  case 2: // 11-pt kernel
    lut0 = luts->lut0;
    lut1 = luts->lut1;

    ip += 5;                // ip[0] is center pixel 
    fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
    rev0 = (lut0[ip[0]] << 10) + lut0[ip[1]];

    fwd1 = (lut1[ip[-5]] >> 10) + lut1[ip[-4]];
    rev1 = (lut1[ip[3]] << 10) + lut1[ip[4]];

    while(len--) {
      fwd0 = (fwd0 >> 10) + lut0[ip[0]];
      rev0 = (rev0 << 10) + lut0[ip[2]];

      fwd1 = (fwd1 >> 10) + lut1[ip[-3]];
      rev1 = (rev1 << 10) + lut1[ip[5]];

      val = ((fwd0 & MASK) + ((rev0 >> 20) & MASK)
            +(fwd1 & MASK) + ((rev1 >> 20) & MASK) - bias)
        >> 2;
      *op = CLAMP(val, 0, 255);

      ip++;
      op += stride;
    }
    break;
  case 3: // 17-pt kernel
    lut0 = luts->lut0;
    lut1 = luts->lut1;
    lut2 = luts->lut2;

    ip += 8;                    // ip[0] is center pixel
    fwd0 = (lut0[ip[-2]] >> 10) + lut0[ip[-1]];
    rev0 = (lut0[ip[ 0]] << 10) + lut0[ip[1]];

    fwd1 = (lut1[ip[-5]] >> 10) + lut1[ip[-4]];
    rev1 = (lut1[ip[ 3]] << 10) + lut1[ip[4]];

    fwd2 = (lut2[ip[-8]] >> 10) + lut2[ip[-7]];
    rev2 = (lut2[ip[ 6]] << 10) + lut2[ip[7]];

    while(len--) {
      fwd0 = (fwd0 >> 10) + lut0[ip[0]];
      rev0 = (rev0 << 10) + lut0[ip[2]];

      fwd1 = (fwd1 >> 10) + lut1[ip[-3]];
      rev1 = (rev1 << 10) + lut1[ip[5]];

      fwd2 = (fwd2 >> 10) + lut2[ip[-6]];
      rev2 = (rev2 << 10) + lut2[ip[8]];

      val = ((fwd0 & MASK) + ((rev0 >> 20) & MASK)
            +(fwd1 & MASK) + ((rev1 >> 20) & MASK)
            +(fwd2 & MASK) + ((rev2 >> 20) & MASK) - bias)
        >> 2;
      *op = CLAMP(val, 0, 255);

      ip++;
      op += stride;
    }
    break;
  }
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * getFilterKernels:
 *
 * Gaussian FIR kernel array of structures is defined and set.
 *
 * Format of database FIR kernel entry is:
 *
 *   g,  n,  k0,  k1,  k2,   k3,... kn-1
 *
 * where n is less than or equal to 9. This defines a (2n-1) point
 * kernel which is symmetric and corresponds to an FIR filter of:
 *
 * kn-1, ..., k3, k2, k1, k0, k1, k2, k3, ..., kn-1
 *
 * The frequency response of the filter should approximate a Gaussian
 * function of width g. All the filters defined are normalised. That is:
 *
 *  k0 + 2*(k1 + k2 + k3 + ...+ kn-1) = 1.0
 *
 * The FirS struct contans: filter width, # elements and cooef[9]
 *
 */
 long PTileFlashPix::GetFilterKernels(firS *Gauss_array, long *NumKernels)
{
  long  i;
  firS Gauss[] = {
        1, 3, 0.927947, 0.046292, -0.010266, 0, 0, 0, 0, 0, 0,
        2, 3, 0.863510, 0.084621, -0.016376, 0, 0, 0, 0, 0, 0,
        5, 3, 0.709951, 0.163083, -0.018058, 0, 0, 0, 0, 0, 0,
        10, 3, 0.549109, 0.221605, 0.003841, 0, 0, 0, 0, 0, 0,
        15, 3, 0.456657, 0.240525, 0.031146, 0, 0, 0, 0, 0, 0,
        20, 3, 0.399725, 0.245012, 0.055126, 0, 0, 0, 0, 0, 0,
        25, 3, 0.362202, 0.244322, 0.074576, 0, 0, 0, 0, 0, 0,
        30, 3, 0.336000, 0.241897, 0.090102, 0, 0, 0, 0, 0, 0,
        40, 4, 0.283671, 0.221641, 0.105732, 0.030791, 0, 0, 0, 0, 0,
        50, 4, 0.257185, 0.211101, 0.116785, 0.043522, 0, 0, 0, 0, 0,
        60, 4, 0.238807, 0.202564, 0.123695, 0.054338, 0, 0, 0, 0, 0,
        80, 5, 0.203005, 0.179459, 0.123936, 0.066898, 0.028204, 0, 0, 0, 0,
        100, 5, 0.185341, 0.167947, 0.124891, 0.076274, 0.038218, 0, 0, 0, 0,
        120, 5, 0.173222, 0.159578, 0.124665, 0.082670, 0.046476, 0, 0, 0, 0,
        135, 6, 0.157970, 0.146819, 0.117923, 0.081809, 0.049060, 0.025405, 0, 0, 0,
        150, 6, 0.151509, 0.141843, 0.116457, 0.083798, 0.052894, 0.029252, 0, 0, 0,
        165, 7, 0.141342, 0.133147, 0.111267, 0.082517, 0.054283, 0.031700, 0.016415, 0, 0,
        180, 7, 0.136262, 0.129005, 0.109428, 0.083204, 0.056678, 0.034618, 0.018937, 0, 0,
        210, 7, 0.128108, 0.122244, 0.106156, 0.083945, 0.060403, 0.039592, 0.023606, 0, 0,
        240, 7, 0.121869, 0.116980, 0.103387, 0.084197, 0.063127, 0.043627, 0.027747, 0, 0,
        280, 7, 0.115536, 0.111561, 0.100347, 0.084163, 0.065748, 0.047909, 0.032504, 0, 0,
        320, 7, 0.110736, 0.107404, 0.097889, 0.083935, 0.067621, 0.051271, 0.036512, 0, 0,
        360, 7, 0.106978, 0.104120, 0.095873, 0.083632, 0.069011, 0.053967, 0.039907, 0, 0,
        400, 7, 0.103961, 0.101465, 0.094197, 0.083308, 0.070074, 0.056169, 0.042807, 0, 0
  };

  *NumKernels = sizeof(Gauss)/sizeof(Gauss[0]);

  if (*NumKernels > MAXKERNNUM) {
//    fprintf(stderr, "Too many kernels defined.\n");
    return -1;
  }

  for (i = 0; i < *NumKernels; i++) 
    Gauss_array[i] = Gauss[i];

  return 0;
}

