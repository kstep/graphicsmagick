//  ----------------------------------------------------------------------------
//  MODULE    : PTileFlashPix 
//  LANGUAGE  : C++
//  CREATOR   : Philippe BOSSUT 
//  CREAT. DATE : Wednesday, March 20, 1996 
//  DESCRIPTION :  
//  COMMENTS  : 
//  SCCSID      : @(#)ptil_fpx.h  1.2 14:44:16 06 Jan 1997
//  ----------------------------------------------------------------------------
//  Copyright (c) 1999 Digital Imaging Group, Inc.
//  For conditions of distribution and use, see copyright notice
//  in Flashpix.h
//  ----------------------------------------------------------------------------
  #ifndef PTileFlashPix_h
  #define PTileFlashPix_h
  #ifndef Commun_h
    #include  "common.h"
  #endif
//  ----------------------------------------------------------------------------

//  Includes
//  --------
#ifndef PTile_h
  #include  "ptile.h"
#endif

#ifndef FPXBaselineIO_h
  #include  "fpxlibio.h"
#endif

//  Constants
//  ---------

//  Types Declarations
//  ------------------
typedef struct {
  double  capture;          // capture MTF, MTFc(s)
  double  prefilter;          // prefilter MTF, MTFd(s)
  double  printer;          // printer and media MTF, MTFd(s)
  double  fraction;         // relative size of viewed picture, w
  long  level;            // resolution level to which filtering is applied.
                      // level = 0 is the top (full) resolution layer. 
                      // Note that this is referred as k in the FPX Spec
  long  height_o;         // height in pixels of the full-resolution image in 
                      // the hierarchy (No)
  long  height_p;         // height in pixels of the image requested by the 
                      // application ie. the displayed or printed image (Np)
  long  height_r;         // height in pixels of image level, FiltP.level (Nk)
  double  delta_A;          // change in acutance
  double  total;            // Total system MTF, MTFt(s)
  FPXBaselineColorSpace colorSpace; // defines color space for filtering.
} filtParmS, *filtParmP;


typedef struct {
  double  width;            // Gaussian filter width
  long  elements;         // number of elements, n, in filter
  double  kernel[9];          // n elements specifies a (2n-1) point kernel
} firS, *firP;


typedef struct {                  // packed lut structure
  long lut0[256];               // stage 0 for 5-pt kernel
  long lut1[256];               // stage 1 for 11-pt kernel
  long lut2[256];               // stage 2 for 17-pt kernel
  long bias;                    // accumulated stage biases
  long stages;                  // # of stages used: 1,2,3
} lutS, *lutP;



//  Types Definitions
//  -----------------

//  Classes Declarations
//  --------------------
  class PResolutionFlashPix;
  class PFlashPixFile;
  class PTileFlashPix;

//  Classes Definitions
//  -------------------
  // Block class. Block keep memory of the pixels themselves.
  class PTileFlashPix : public PTile {
  
    friend class PHierarchicalImage;
    friend class PResolutionLevel;
    friend class PResolutionFlashPix;

  public:
                 PTileFlashPix();
                ~PTileFlashPix();
        
        // Public interface: Methods implementing the file specific tile description:
        // --------------------------------------------------------------------------

        // Read and write a tile:
        virtual FPXStatus Read();     // Read display-modified pixels from file
        
            FPXStatus makePaddedRawPixels(long padLen, Pixel **paddedRawPixels);
        
        virtual FPXStatus Write();    // Write pixels in file

        // Set information tools
        virtual void    InitializeRead (PResolutionLevel* father, long offset, 
                      long sizetile, long id, 
                      long compression = 0, 
                      long theCompressionSubtype = 0);  // Init tile in read mode

        // Read jpeg header from image content property set
            Boolean   ReadHeader(PFlashPixFile* filePtr,
                       unsigned char** pJpegHeader, 
                       unsigned long* headerSize); 
          
        // Write jpeg table to image content property set
            Boolean   WriteHeader(PFlashPixFile* filePtr, 
                      unsigned char* jpegHeader, unsigned long headerSize); 
                    
        // Convert the FPX compression code to index of compressor 
        virtual TLC_IdCodec ConvertCompressionOption (); 
            FPXStatus   Contrast(double k,FPXBaselineColorSpace colorSpace,
                      Pixel *pixels,long count);

            FPXStatus ApplyFilter(FPXBaselineColorSpace colorSpace); // in filter.cpp

        // Raw data read and write functions
            FPXStatus ReadRawPixels();

        // Raw tile read and write functions
            FPXStatus ReadRawTile (FPXCompressionOption*  compressOption, 
                         unsigned char*     compressQuality,
                         long*          compressSubtype,
                         unsigned long*     dataLength,
                         void**         data);
            FPXStatus WriteRawTile (FPXCompressionOption  compressOption, 
                          unsigned char     compressQuality,
                          long          compressSubtype,
                          unsigned long     dataLength,
                          void*         data);
    protected:
        FPXStatus   UnsharpMask(filtParmS *FiltP);  // in filter.cpp
        FPXStatus   BlurFilter(filtParmS *FiltP); // in filter.cpp
        FPXStatus   Convolve(unsigned char *in, long pad, lutS *luts, 
                    unsigned char * out); // in filter.cpp
        FPXStatus   InitPackedLuts(double *kernel, long n);   // in filter.cpp
        void      Fastconv( unsigned char *src, long len, long padLen, 
                    long stride, lutS *luts, unsigned char *dst);
        long      GetFilterKernels(firS *Gauss_array, long *NumKernels);  // in filter.cpp

  };
  
  

//  'inline' Functions
//  ------------------


//  'extern' Functions
//  ------------------

//  'extern' Variables
//  ------------------

//  ----------------------------------------------------------------------------
  #endif // PTileFlashPix_h
//  ----------------------------------------------------------------------------

