/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        TTTTT  IIIII  FFFFF  FFFFF                           %
%                          T      I    F      F                               %
%                          T      I    FFF    FFF                             %
%                          T      I    F      F                               %
%                          T    IIIII  F      F                               %
%                                                                             %
%                                                                             %
%                   Read/Write GraphicsMagick Image Format.                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/bit_stream.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/resize.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasTIFF)
#if defined(HAVE_TIFFCONF_H)
#include "tiffconf.h"
#endif
#include "tiffio.h"
#if !defined(COMPRESSION_ADOBE_DEFLATE)
#define COMPRESSION_ADOBE_DEFLATE  8
#endif

/*
  Global declarations.
*/
static ExceptionInfo
  *tiff_exception;
#endif

/*
  Forward declarations.
*/
static unsigned int
  WritePTIFImage(const ImageInfo *,Image *),
  WriteTIFFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T I F F                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTIFF returns True if the image format type, identified by the
%  magick string, is TIFF.
%
%  The format of the IsTIFF method is:
%
%      unsigned int IsTIFF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTIFF returns True if the image format type is TIFF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsTIFF(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\115\115\000\052",4) == 0)
    return(True);
  if (memcmp(magick,"\111\111\052\000",4) == 0)
    return(True);
  return(False);
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTIFFImage reads a Tagged image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTIFFImage method is:
%
%      Image *ReadTIFFImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static const char *CompressionTagToString(unsigned int compress_tag)
{
  const char
    *result = "Unknown";

  switch (compress_tag)
    {
    case COMPRESSION_ADOBE_DEFLATE:
      result="ZIP deflate (Adobe)";
      break;
#if defined(COMPRESSION_DEFLATE)
    case COMPRESSION_DEFLATE:
      result="ZIP deflate (Pixar)";
      break;
#endif
    case COMPRESSION_CCITTFAX3:
      result="CCITT Group 3 fax";
      break;
    case COMPRESSION_CCITTFAX4:
      result="CCITT Group 4 fax";
      break;
    case COMPRESSION_CCITTRLE:
      result="CCITT modified Huffman RLE";
      break;
    case COMPRESSION_CCITTRLEW:
      result="CCITT modified Huffman RLE (Word aligned)";
      break;
#if defined(COMPRESSION_OJPEG)
    case COMPRESSION_OJPEG:
      result="JPEG DCT (Old)";
      break;
#endif
    case COMPRESSION_JPEG:
      result="JPEG DCT";
      break;
#if defined(COMPRESSION_JBIG)
    case COMPRESSION_JBIG:
      result="JBIG";
      break;
#endif
    case COMPRESSION_LZW:
      result="LZW";
      break;
#if defined(COMPRESSION_NEXT)
    case COMPRESSION_NEXT:
      result="NeXT 2-bit RLE";
      break;
#endif
    case COMPRESSION_NONE:
      result="not compressed";
      break;
    case COMPRESSION_PACKBITS:
      result="Macintosh RLE (Packbits)";
      break;
#if defined(COMPRESSION_THUNDERSCAN)
    case COMPRESSION_THUNDERSCAN:
      result="ThunderScan RLE";
      break;
#endif
  }
  return result;
}

static const char *PhotometricTagToString(unsigned int photometric)
{
  const char
    *result = "Unknown";
  
  switch (photometric)
    {
    case  PHOTOMETRIC_CIELAB:
      result="CIELAB";
      break;
    case PHOTOMETRIC_LOGL:
      result="CIE Log2(L)";
      break;
    case PHOTOMETRIC_LOGLUV:
      result="LOGLUV";
      break;
#if defined(PHOTOMETRIC_MASK)
    case PHOTOMETRIC_MASK:
      result="MASK";
      break;
#endif
    case PHOTOMETRIC_MINISBLACK:
      result="MINISBLACK";
      break;
    case PHOTOMETRIC_MINISWHITE:
      result="MINISWHITE";
      break;
    case PHOTOMETRIC_PALETTE:
      result="PALETTE";
      break;
    case PHOTOMETRIC_RGB:
      result="RGB";
      break;
    case PHOTOMETRIC_SEPARATED:
      result="SEPARATED";
      break;
    case PHOTOMETRIC_YCBCR:
      result="YCBCR";
      break;
    }

  return result;
}

#if defined(ICC_SUPPORT)
static unsigned int ReadColorProfile(char *text,long int length,Image *image)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (image->color_profile.length != 0)
    {
      MagickFreeMemory(image->color_profile.info);
      image->color_profile.length=0;
    }
  image->color_profile.info=MagickAllocateMemory(unsigned char *,length);
  if (image->color_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      image->filename);
  image->color_profile.length=length;
  (void) memcpy(image->color_profile.info,p,length);
  return(True);
}
#endif

#if defined(IPTC_SUPPORT)
static unsigned int ReadNewsProfile(char *text,long int length,Image *image,
  int type)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (image->iptc_profile.length != 0)
    {
      MagickFreeMemory(image->iptc_profile.info);
      image->iptc_profile.length=0;
      image->iptc_profile.info=(unsigned char *) NULL;
    }
  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle IPTC tag.
      */
      length*=4;
      image->iptc_profile.info=MagickAllocateMemory(unsigned char *,length);
      if (image->iptc_profile.info == (unsigned char *) NULL)
        ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
          image->filename);
      image->iptc_profile.length=length;
      (void) memcpy(image->iptc_profile.info,p,length);
      return(True);
    }
  /*
    Handle PHOTOSHOP tag.
  */
  while (length > 0)
  {
#if defined(GET_ONLY_IPTC_DATA)
    if (LocaleNCompare((char *) p,"8BIM44",6) == 0)
#else
    if (LocaleNCompare((char *) p,"8BIM",4) == 0)
#endif
      break;
    length-=2;
    p+=2;
  }
  if (length <= 0)
    return(False);
  if (image->iptc_profile.length != 0)
    {
      MagickFreeMemory(image->iptc_profile.info);
      image->iptc_profile.length=0;
    }
#if defined(GET_ONLY_IPTC_DATA)
  /*
    Eat OSType, IPTC ID code, and Pascal string length bytes.
  */
  p+=6;
  length=(*p++);
  if (length)
    p+=length;
  if ((length & 0x01) == 0)
    p++;  /* align to an even byte boundary */
  length=(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
  p+=4;
#endif
  image->iptc_profile.info=MagickAllocateMemory(unsigned char *,length);
  if (image->iptc_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
      image->filename);
  image->iptc_profile.length=length;
  (void) memcpy(image->iptc_profile.info,p,length);
  return(True);
}
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int TIFFCloseBlob(thandle_t image)
{
  CloseBlob((Image *) image);
  return(0);
}

static unsigned int TIFFErrors(const char *module,const char *format,
  va_list warning)
{
  char
    message[MaxTextExtent];

  (void) vsprintf(message,format,warning);
  (void) strcat(message,".");
  ThrowException2(tiff_exception,CoderError,message,module);
  return(True);
}

static int TIFFMapBlob(thandle_t image,tdata_t *base,toff_t *size)
{
  return(0);
}

static tsize_t TIFFReadBlob(thandle_t image,tdata_t data,tsize_t size)
{
  return((tsize_t) ReadBlob((Image *) image,(size_t) size,data));
}

static toff_t TIFFSeekBlob(thandle_t image,toff_t offset,int whence)
{
  return((toff_t) SeekBlob((Image *) image,offset,whence));
}

static toff_t TIFFGetBlobSize(thandle_t image)
{
  return((toff_t) GetBlobSize((Image *) image));
}

static void TIFFUnmapBlob(thandle_t image,tdata_t base,toff_t size)
{
}

static unsigned int TIFFWarnings(const char *module,const char *format,
  va_list warning)
{
  char
    message[MaxTextExtent];

  (void) vsprintf(message,format,warning);
  (void) strcat(message,".");
  ThrowException2(tiff_exception,CoderWarning,message,module);
  return(True);
}

static tsize_t TIFFWriteBlob(thandle_t image,tdata_t data,tsize_t size)
{
  return((tsize_t) WriteBlob((Image *) image,(size_t) size,data));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

typedef enum
{
  PseudoClassMethod,         /* PseudoClass method (colormapped & grayscale) */
  DirectClassScanLineMethod, /* Scanline method */
  DirectClassStrippedMethod, /* Stripped RGBA method */
  DirectClassTiledMethod,    /* Tiled RGBA method */
  DirectClassPuntMethod      /* Last resort RGBA method */
} TIFFReadMethod;

static Image *ReadTIFFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    *text;

  float
    *chromaticity,
    x_resolution,
    y_resolution;

  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  TIFF
    *tiff;

  uint16
    compress_tag,
    bits_per_sample,
    extra_samples,
    max_sample_value,
    min_sample_value,
    interlace,
    pages,
    photometric,
    *sample_info,
    samples_per_pixel,
    units,
    value;

  uint32
    height,
#if defined(ICC_SUPPORT) || defined(IPTC_SUPPORT) || defined(PHOTOSHOP_SUPPORT)
    length,
#endif
    rows_per_strip,
    width;

  unsigned char
    *scanline;

  TIFFReadMethod
    method;

  unsigned int
    filename_is_temporary=False,
    logging,
    status;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=IsEventLogging();
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  tiff_exception=exception;
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  if ((image->blob->type == FileStream) || (image->blob->type == BlobStream))
    tiff=TIFFClientOpen(image->filename,"rb",(thandle_t) image,TIFFReadBlob,
      TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
      TIFFUnmapBlob);
  else
    {
      filename_is_temporary=True;
      if(!AcquireTemporaryFileName(filename))
        ThrowReaderTemporaryFileException(filename);
      (void) ImageToFile(image,filename,exception);
      tiff=TIFFOpen(filename,"rb");
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Open temporary file %.1024s",filename);
    }
  if (tiff == (TIFF *) NULL)
    {
      if (filename_is_temporary)
        LiberateTemporaryFile(filename);
      ThrowReaderException(FileOpenError,UnableToOpenFile,image)
    }
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      status=TIFFReadDirectory(tiff);
      if (status == False)
        {
          TIFFClose(tiff);
          if (filename_is_temporary)
            LiberateTemporaryFile(filename);
          ThrowReaderException(CorruptImageError,UnableToReadSubImageData,
            image)
        }
    }
  do
  {
    if (image_info->verbose)
      TIFFPrintDirectory(tiff,stdout,False);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_COMPRESSION,&compress_tag);
    (void) TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width);
    (void) TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_PLANARCONFIG,&interlace);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric);
    if (logging)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %ux%u",
          (unsigned int) width,(unsigned int) height);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Interlace: %u",
          interlace);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Bits per sample: %u",bits_per_sample);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Min sample value: %u",min_sample_value);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Max sample value: %u",max_sample_value);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "Photometric: %s", PhotometricTagToString(photometric));
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "Compression: %s", CompressionTagToString(compress_tag));
      }
    if (photometric == PHOTOMETRIC_CIELAB)
      {
        TIFFClose(tiff);
        if (filename_is_temporary)
          LiberateTemporaryFile(filename);
        ThrowReaderException(CoderError,UnableToReadCIELABImages,image)
      }
    if (photometric == PHOTOMETRIC_SEPARATED)
      image->colorspace=CMYKColorspace;
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
      &samples_per_pixel);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_RESOLUTIONUNIT,&units);
    x_resolution=image->x_resolution;
    y_resolution=image->y_resolution;
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_XRESOLUTION,&x_resolution);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_YRESOLUTION,&y_resolution);
    image->x_resolution=x_resolution;
    image->y_resolution=y_resolution;
    chromaticity=(float *) NULL;
    (void) TIFFGetField(tiff,TIFFTAG_WHITEPOINT,&chromaticity);
    if (chromaticity != (float *) NULL)
      {
        image->chromaticity.white_point.x=chromaticity[0];
        image->chromaticity.white_point.y=chromaticity[1];
      }
    chromaticity=(float *) NULL;
    (void) TIFFGetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,&chromaticity);
    if (chromaticity != (float *) NULL)
      {
        image->chromaticity.red_primary.x=chromaticity[0];
        image->chromaticity.red_primary.y=chromaticity[1];
        image->chromaticity.green_primary.x=chromaticity[2];
        image->chromaticity.green_primary.y=chromaticity[3];
        image->chromaticity.blue_primary.x=chromaticity[4];
        image->chromaticity.blue_primary.y=chromaticity[5];
      }
#if defined(ICC_SUPPORT)
    if (TIFFGetField(tiff,TIFFTAG_ICCPROFILE,&length,&text) == 1)
      (void) ReadColorProfile(text,(long) length,image);
#endif
#if defined(IPTC_SUPPORT)
#if defined(PHOTOSHOP_SUPPORT)
    if (TIFFGetField(tiff,TIFFTAG_PHOTOSHOP,&length,&text) == 1)
      (void) ReadNewsProfile(text,(long) length,image,TIFFTAG_PHOTOSHOP);
#else
    if (TIFFGetField(tiff,TIFFTAG_RICHTIFFIPTC,&length,&text) == 1)
      {
        if (TIFFIsByteSwapped(tiff))
          TIFFSwabArrayOfLong((uint32 *) text,length);
        ReadNewsProfile(text,length,image,TIFFTAG_RICHTIFFIPTC);
      }
#endif
#endif
    /*
      Allocate memory for the image and pixel buffer.
    */
    switch (compress_tag)
    {
      case COMPRESSION_NONE: image->compression=NoCompression; break;
      case COMPRESSION_CCITTFAX3: image->compression=FaxCompression; break;
      case COMPRESSION_CCITTFAX4: image->compression=Group4Compression; break;
      case COMPRESSION_JPEG: image->compression=JPEGCompression; break;
      case COMPRESSION_OJPEG: image->compression=JPEGCompression; break;
      case COMPRESSION_LZW: image->compression=LZWCompression; break;
      case COMPRESSION_DEFLATE: image->compression=ZipCompression; break;
      case COMPRESSION_ADOBE_DEFLATE: image->compression=ZipCompression; break;
      default: image->compression=RLECompression; break;
    }
    image->columns=width;
    image->rows=height;

    if (photometric == PHOTOMETRIC_PALETTE)
      /* Bits per sample has nothing to do with colormapped image
         depth */
      image->depth=8;
    else
      image->depth=bits_per_sample;  /* FIXME */
    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Image depth: %lu",
        image->depth);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
      &sample_info);
    image->matte=((extra_samples == 1) &&
      (*sample_info == EXTRASAMPLE_ASSOCALPHA));
    if (image->colorspace == CMYKColorspace)
      {
        if (samples_per_pixel > 4)
          image->matte=True;
      }
    else
      if (samples_per_pixel > 3)
        image->matte=True;
    if ((samples_per_pixel <= 2) && !TIFFIsTiled(tiff) &&
        ((photometric == PHOTOMETRIC_MINISBLACK) ||
         (photometric == PHOTOMETRIC_MINISWHITE) ||
         (photometric == PHOTOMETRIC_PALETTE)))
      {
        /*
          Palette or grayscale image
        */
        image->colors=1UL << bits_per_sample;
        /* Constrain image colormap to MaxColormapSize */
        image->colors=Min(MaxColormapSize,image->colors);
        /* Allocate colormap */
        if (!AllocateImageColormap(image,image->colors))
          {
            TIFFClose(tiff);
            if ((image->blob->type != FileStream) &&
                (image->blob->type != BlobStream))
              remove(filename);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        /* Extra samples are opacity samples */
        if (samples_per_pixel == 2)
          image->matte=True;
      }
    if (units == RESUNIT_INCH)
      image->units=PixelsPerInchResolution;
    if (units == RESUNIT_CENTIMETER)
      image->units=PixelsPerCentimeterResolution;
    value=(unsigned short) image->scene;
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_PAGENUMBER,&value,&pages);
    image->scene=value;
    if (TIFFGetField(tiff,TIFFTAG_ARTIST,&text) == 1)
      (void) SetImageAttribute(image,"artist",text);
    if (TIFFGetField(tiff,TIFFTAG_DATETIME,&text) == 1)
      (void) SetImageAttribute(image,"timestamp",text);
    if (TIFFGetField(tiff,TIFFTAG_SOFTWARE,&text) == 1)
      (void) SetImageAttribute(image,"software",text);
    if (TIFFGetField(tiff,TIFFTAG_DOCUMENTNAME,&text) == 1)
      (void) SetImageAttribute(image,"document",text);
    if (TIFFGetField(tiff,TIFFTAG_MAKE,&text) == 1)
      (void) SetImageAttribute(image,"make",text);
    if (TIFFGetField(tiff,TIFFTAG_MODEL,&text) == 1)
      (void) SetImageAttribute(image,"model",text);
    if (TIFFGetField(tiff,33432,&text) == 1)
      (void) SetImageAttribute(image,"copyright",text);
    if (TIFFGetField(tiff,33423,&text) == 1)
      (void) SetImageAttribute(image,"kodak-33423",text);
    if (TIFFGetField(tiff,36867,&text) == 1)
      (void) SetImageAttribute(image,"kodak-36867",text);
    if (TIFFGetField(tiff,TIFFTAG_PAGENAME,&text) == 1)
      (void) SetImageAttribute(image,"label",text);
    if (TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text) == 1)
      (void) SetImageAttribute(image,"comment",text);
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    method=PseudoClassMethod;
    if (image->storage_class == DirectClass)
      {
        method=DirectClassPuntMethod;
        if (TIFFGetField(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip))
          method=DirectClassStrippedMethod;
        if (photometric == PHOTOMETRIC_RGB)
          if ((samples_per_pixel >= 2) && (interlace == PLANARCONFIG_CONTIG))
            method=DirectClassScanLineMethod;
        if (image->colorspace == CMYKColorspace)
          method=DirectClassScanLineMethod;
        if (TIFFIsTiled(tiff))
          method=DirectClassTiledMethod;
      }

    switch (method)
    {
      case PseudoClassMethod:
      {
        unsigned long
          packet_size;

        size_t
          scanline_size;

        /*
          Convert TIFF image to PseudoClass image.
        */

        /* Constrain image depth to QuantumDepth */
        image->depth=Min(QuantumDepth,image->depth);
        
        if (logging)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Using PseudoClass read method with %u bits per sample, %lu colors",
                bits_per_sample, image->colors);
          }
        
        packet_size=image->depth > 8 ? 2 : 1;
        if (image->matte)
          packet_size*=2;
        scanline_size=Max((size_t) TIFFScanlineSize(tiff),packet_size*width);
        scanline=MagickAllocateMemory(unsigned char *,scanline_size);
        if (scanline == (unsigned char *) NULL)
          {
            TIFFClose(tiff);
            if (filename_is_temporary)
              LiberateTemporaryFile(filename);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        /*
          Create colormap.
        */
        switch (photometric)
        {
          case PHOTOMETRIC_MINISBLACK:
          {
            Quantum
              quantum;

            for (i=0; i < (long) image->colors; i++)
            {
              quantum = (Quantum) (i*(MaxRGB/Max(image->colors-1,1)));
              image->colormap[i].red=quantum;
              image->colormap[i].green=quantum;
              image->colormap[i].blue=quantum;
              image->colormap[i].opacity=OpaqueOpacity;
            }
            break;
          }
          case PHOTOMETRIC_MINISWHITE:
          default:
          {
            Quantum
              quantum;

            for (i=0; i < (long) image->colors; i++)
            {
              quantum = (Quantum) (MaxRGB - (i*(MaxRGB/Max(image->colors-1,1))));
              image->colormap[i].red=quantum;
              image->colormap[i].green=quantum;
              image->colormap[i].blue=quantum;
              image->colormap[i].opacity=OpaqueOpacity;
            }
            break;
          }
          case PHOTOMETRIC_PALETTE:
          {
            long
              range;

            uint16
              *blue_colormap,
              *green_colormap,
              *red_colormap;

            (void) TIFFGetField(tiff,TIFFTAG_COLORMAP,&red_colormap,
              &green_colormap,&blue_colormap);
            range=256L;  /* might be old style 8-bit colormap */
            for (i=0; i < (long) image->colors; i++)
              if ((red_colormap[i] >= 256) || (green_colormap[i] >= 256) ||
                  (blue_colormap[i] >= 256))
                {
                  range=65535L;
                  break;
                }
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=(Quantum)
                (((double) MaxRGB*red_colormap[i])/range+0.5);
              image->colormap[i].green=(Quantum)
                (((double) MaxRGB*green_colormap[i])/range+0.5);
              image->colormap[i].blue=(Quantum)
                (((double) MaxRGB*blue_colormap[i])/range+0.5);
            }
            break;
          }
        }
        /*
          Convert image to PseudoClass pixel packets.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          register IndexPacket
            *indexes;
          
          BitStreamReadHandle
            stream;

          unsigned int
            quantum;

          unsigned int
            matte_scale=1;
          
#if QuantumDepth < 16
          unsigned int
            index_scale;
          
          index_scale = (1U << bits_per_sample) / image->colors + 1;
#endif

          if (QuantumDepth > bits_per_sample)
            matte_scale=MaxRGB / (MaxRGB >> (QuantumDepth-bits_per_sample));

          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;

          indexes=GetIndexes(image);
          if (indexes == (IndexPacket *) NULL)
            break;

          (void) TIFFReadScanline(tiff,(char *) scanline,(uint32) y,0);
          if (bits_per_sample >= 16)
            {
              unsigned long
                lsb_first;

              /*
                Ensure the header byte-order is most-significant byte first.
              */
              lsb_first=1;
              if (*(char *) &lsb_first)
                MSBOrderShort(scanline, Max(TIFFScanlineSize(tiff),
                  packet_size*width));
            }

          BitStreamInitializeRead(&stream,scanline);

          for (x = (long) width; x > 0 ; x--)
            {
              /* index */
              quantum=BitStreamRead(&stream,bits_per_sample);
#if QuantumDepth < 16
              if (bits_per_sample > QuantumDepth)
                quantum /= index_scale;
#endif
              *indexes++ = quantum;
              *q = image->colormap[quantum];
              if (image->matte)
                {
                  /* matte */
                  quantum = BitStreamRead(&stream,bits_per_sample);
                  if (QuantumDepth > bits_per_sample)
                    quantum = quantum * matte_scale;
                  else
                    quantum >>= (bits_per_sample - QuantumDepth);
                  q->opacity = (Quantum) (MaxRGB - quantum);
                }
              q++;
            }

          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(scanline);
/*         image->depth=GetImageDepth(image,exception); */
        break;
      }
      case DirectClassScanLineMethod:
      {
        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Using DirectClassScanLine read method with %u bits per sample",
               bits_per_sample);
        scanline=MagickAllocateMemory(unsigned char *,8*TIFFScanlineSize(tiff));
        if (scanline == (unsigned char *) NULL)
          {
            TIFFClose(tiff);
            if (filename_is_temporary)
              LiberateTemporaryFile(filename);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) TIFFReadScanline(tiff,(char *) scanline,(uint32) y,0);
          if (bits_per_sample > 8)
            {
              unsigned long
                lsb_first;

              /*
                Ensure the header byte-order is most-significant byte first.
              */
              lsb_first=1;
              if (*(char *) &lsb_first)
                MSBOrderShort(scanline,8*TIFFScanlineSize(tiff));
            }
          if (bits_per_sample == 4)
            {
              register unsigned char
                *r;

              width=TIFFScanlineSize(tiff);
              p=scanline+width-1;
              r=scanline+(width << 1)-1;
              for (x=0; x < (long) width; x++)
              {
                *r--=((*p) & 0xf) << 4;
                *r--=((*p >> 4) & 0xf) << 4;
                p--;
              }
            }
          if (image->colorspace == CMYKColorspace)
            {
              if (!image->matte)
                (void) PushImagePixels(image,CMYKQuantum,scanline);
              else
                (void) PushImagePixels(image,CMYKAQuantum,scanline);
            }
          else
            if (!image->matte)
              (void) PushImagePixels(image,RGBQuantum,scanline);
            else
              (void) PushImagePixels(image,RGBAQuantum,scanline);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(scanline);
        break;
      }
    case DirectClassTiledMethod:
      {
        /*
          Convert tiled TIFF image to DirectClass MIFF image.
        */
        register uint32
          *p;
        
        uint32
          *tile_pixels,
          tile_columns,
          tile_rows;
        
        unsigned long
          tile_total_pixels;
        
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Using DirectClassTiled read method with %u bits per sample",
               bits_per_sample);
        /*
          Obtain tile geometry
        */
        if(!TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&tile_columns)
           || !TIFFGetField(tiff,TIFFTAG_TILELENGTH,&tile_rows))
          {
            TIFFClose(tiff);
            if (filename_is_temporary)
              LiberateTemporaryFile(filename);
            ThrowReaderException(CoderError,ImageIsNotTiled,image)
          }
        tile_total_pixels=tile_columns*tile_rows;
        if (logging)
          {
            LogMagickEvent(CoderEvent,GetMagickModule(),"Reading TIFF tiles ...");
            LogMagickEvent(CoderEvent,GetMagickModule(),
              "TIFF tile geometry %ux%u, %lu pixels",(unsigned int)tile_columns,
              (unsigned int)tile_rows, tile_total_pixels);
          }
        
        /*
          Allocate tile buffer
        */
        tile_pixels=MagickAllocateMemory(uint32*,tile_columns*tile_rows*sizeof (uint32));
        if (tile_pixels == (uint32 *) NULL)
          {
            TIFFClose(tiff);
            if (filename_is_temporary)
              LiberateTemporaryFile(filename);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        for (y=0; y < (long) image->rows; y+=tile_rows)
          {
            /*
              Retrieve a tile height's worth of rows
            */
            PixelPacket
              *strip;

            unsigned int
              tile_columns_remaining,
              tile_rows_remaining;

            /* Compute remaining tile rows */
            if (y+tile_rows < image->rows)
              tile_rows_remaining=tile_rows;
            else
              tile_rows_remaining=image->rows-y;

            strip=SetImagePixels(image,0,y,image->columns,tile_rows_remaining);
            if (strip == (PixelPacket *) NULL)
              break;
            for (x=0; x < image->columns; x+=tile_columns)
              {
                register unsigned int
                  tile_column,
                  tile_row;

                /* Obtain one tile.  Origin is bottom left of tile.  */
                if (!TIFFReadRGBATile(tiff,x,y,tile_pixels))
                  break;

                /* Compute remaining tile columns */
                if (x+tile_columns < image->columns)
                  tile_columns_remaining=tile_columns;
                else
                  tile_columns_remaining=image->columns-x;

                /*
                  Transfer tile to image
                */
                p=tile_pixels+(tile_rows-tile_rows_remaining)*tile_columns;
                q=strip+(x+(tile_rows_remaining-1)*image->columns);
                for ( tile_row=tile_rows_remaining; tile_row > 0; tile_row--)
                  {
                    if (image->matte)
                      for (tile_column=tile_columns_remaining; tile_column >0; tile_column--)
                        {
                          q->red=ScaleCharToQuantum(TIFFGetR(*p));
                          q->green=ScaleCharToQuantum(TIFFGetG(*p));
                          q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                          q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                          q++;
                          p++;
                        }
                    else
                      for (tile_column=tile_columns_remaining; tile_column >0; tile_column--)
                        {
                          q->red=ScaleCharToQuantum(TIFFGetR(*p));
                          q->green=ScaleCharToQuantum(TIFFGetG(*p));
                          q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                          q++;
                          p++;
                        }
                    p+=tile_columns-tile_columns_remaining;
                    q-=(image->columns+tile_columns_remaining);
                  }
              }
            
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                  break;
          }
        MagickFreeMemory(tile_pixels);
        
        break;
      }
      case DirectClassStrippedMethod:
      {
        unsigned long
          number_pixels;

        uint32
          *strip_pixels;

        register uint32
          *p;

        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Using DirectClassStripped read method with %u bits per sample",
               bits_per_sample);
        /*
          Convert stripped TIFF image to DirectClass MIFF image.
        */
        number_pixels=(unsigned long) image->columns*rows_per_strip;
        if ((number_pixels*sizeof(uint32)) != (size_t)
            (number_pixels*sizeof(uint32)))
          {
            TIFFClose(tiff);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        strip_pixels=MagickAllocateMemory(uint32 *,(size_t) (number_pixels*sizeof(uint32)));
        if (strip_pixels == (uint32 *) NULL)
          {
            TIFFClose(tiff);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        /*
          Convert image to DirectClass pixel packets.
        */
        for (y=0; y < (long) image->rows; y+=rows_per_strip)
        {
          i=(long) rows_per_strip;
          if ((y+rows_per_strip) > (long) image->rows)
            i=(long) (image->rows-y);
          q=SetImagePixels(image,0,y,image->columns,i);
          if (q == (PixelPacket *) NULL)
            break;
          q+=image->columns*i-1;
          p=strip_pixels+image->columns*i-1;
          if (!TIFFReadRGBAStrip(tiff,y,strip_pixels))
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=ScaleCharToQuantum(TIFFGetR(*p));
            q->green=ScaleCharToQuantum(TIFFGetG(*p));
            q->blue=ScaleCharToQuantum(TIFFGetB(*p));
            if (image->matte)
              q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
            p--;
            q--;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(strip_pixels);
        break;
      }
      case DirectClassPuntMethod:
      default:
      {
        register uint32
          *p;

        uint32
          *pixels;

        unsigned long
          number_pixels;

        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Using DirectClassPunt (catch all) read method with %u bits per sample",
              bits_per_sample);

        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        number_pixels=(unsigned long) image->columns*image->rows;
        pixels=MagickAllocateMemory(uint32 *,
          (number_pixels+6*image->columns)*sizeof(uint32));
        if (pixels == (uint32 *) NULL)
          {
            TIFFClose(tiff);
            if (filename_is_temporary)
              LiberateTemporaryFile(filename);
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
              image)
          }
        (void) TIFFReadRGBAImage(tiff,(uint32) image->columns,
          (uint32) image->rows,pixels+image->columns*sizeof(uint32),0);
        /*
          Convert image to DirectClass pixel packets.
        */
        p=pixels+number_pixels+image->columns*sizeof(uint32)-1;
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          q+=image->columns-1;
          if (image->matte)
            for (x=(long) image->columns; x > 0; x--)
              {
                q->red=ScaleCharToQuantum(TIFFGetR(*p));
                q->green=ScaleCharToQuantum(TIFFGetG(*p));
                q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                p--;
                q--;
              }
          else
            for (x=(long) image->columns; x > 0; x--)
              {
                q->red=ScaleCharToQuantum(TIFFGetR(*p));
                q->green=ScaleCharToQuantum(TIFFGetG(*p));
                q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                p--;
                q--;
              }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        MagickFreeMemory(pixels);
        break;
      }
    }
    if (image->depth > QuantumDepth)
      image->depth=QuantumDepth;
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    status=TIFFReadDirectory(tiff);
    if (status == True)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        status=MagickMonitor(LoadImageText,image->scene-1,image->scene,
          &image->exception);
        if (status == False)
          break;
      }
  } while (status == True);
  TIFFClose(tiff);
  if (filename_is_temporary)
    LiberateTemporaryFile(filename);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T I F F I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTIFFImage adds attributes for the TIFF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTIFFImage method is:
%
%      RegisterTIFFImage(void)
%
*/
ModuleExport void RegisterTIFFImage(void)
{
#define TIFFDescription  "Tagged Image File Format"
#if defined(HasTIFF)

  char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
#if defined(TIFF_VERSION)
  FormatString(version,"%d",TIFF_VERSION);
#endif

  entry=SetMagickInfo("PTIF");
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WritePTIFImage;
  entry->adjoin=False;
  entry->description=AcquireString("Pyramid encoded TIFF");
  entry->module=AcquireString("TIFF");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("TIF");
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WriteTIFFImage;
  entry->description=AcquireString(TIFFDescription);
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("TIFF");
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("TIFF");
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WriteTIFFImage;
  entry->magick=(MagickHandler) IsTIFF;
  entry->description=AcquireString(TIFFDescription);
  if (*version != '\0')
    entry->version=AcquireString(version);
  entry->module=AcquireString("TIFF");
  (void) RegisterMagickInfo(entry);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T I F F I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTIFFImage removes format registrations made by the
%  TIFF module from the list of supported formats.
%
%  The format of the UnregisterTIFFImage method is:
%
%      UnregisterTIFFImage(void)
%
*/
ModuleExport void UnregisterTIFFImage(void)
{
#if defined(HasTIFF)
  (void) UnregisterMagickInfo("PTIF");
  (void) UnregisterMagickInfo("TIF");
  (void) UnregisterMagickInfo("TIFF");
#endif
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P T I F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WritePTIFImage() writes an image in the pyrimid-encoded Tagged image file
%  format.
%
%  The format of the WritePTIFImage method is:
%
%      unsigned int WritePTIFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WritePTIFImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WritePTIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pyramid_image;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  /*
    Create pyramid-encoded TIFF image.
  */
  pyramid_image=CloneImage(image,0,0,True,&image->exception);
  if (pyramid_image == (Image *) NULL)
    ThrowWriterException2(FileOpenError,image->exception.reason,image);
  do
  {
    pyramid_image->next=ResizeImage(image,pyramid_image->columns/2,
      pyramid_image->rows/2,TriangleFilter,1.0,&image->exception);
    if (pyramid_image->next == (Image *) NULL)
      ThrowWriterException2(FileOpenError,image->exception.reason,image);
    pyramid_image->next->previous=pyramid_image;
    pyramid_image=pyramid_image->next;
  } while ((pyramid_image->columns > 64) && (pyramid_image->rows > 64));
  while (pyramid_image->previous != (Image *) NULL)
    pyramid_image=pyramid_image->previous;
  /*
    Write pyramid-encoded TIFF image.
  */
  clone_info=CloneImageInfo(image_info);
  clone_info->adjoin=True;
  status=WriteTIFFImage(clone_info,pyramid_image);
  DestroyImageList(pyramid_image);
  DestroyImageInfo(clone_info);
  return(status);
}
#endif /* defined(HasTIFF) */

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTIFFImage writes an image in the Tagged image file format.
%
%  The format of the WriteTIFFImage method is:
%
%      unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteTIFFImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

#if defined(IPTC_SUPPORT)
static void WriteNewsProfile(TIFF *tiff,int type,Image *image)
{
  register long
    i;

  unsigned char
    *profile;

  unsigned long
    length,
    roundup;

  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle TIFFTAG_RICHTIFFIPTC tag.
      */
      length=image->iptc_profile.length;
      roundup=4-(length & 0x03); /* round up for long word alignment */
      profile=MagickAllocateMemory(unsigned char *,length+roundup);
      if ((length == 0) || (profile == (unsigned char *) NULL))
        return;
      (void) memcpy(profile,image->iptc_profile.info,length);
      for (i=0; i < (long) roundup; i++)
        profile[length+i] = 0;
      length=(image->iptc_profile.length+roundup)/4;
      if (TIFFIsByteSwapped(tiff))
        TIFFSwabArrayOfLong((uint32 *) profile,length);
      (void) TIFFSetField(tiff,type,(uint32) (length+roundup),(void *) profile);
      MagickFreeMemory(profile);
      return;
    }
  /*
    Handle TIFFTAG_PHOTOSHOP tag.
  */
#if defined(GET_ONLY_IPTC_DATA)
  length=image->iptc_profile.length;
  roundup=(length & 0x01); /* round up for Photoshop */
  profile=MagickAllocateMemory(unsigned char *,length+roundup+12);
  if ((length == 0) || (profile == (unsigned char *) NULL))
    (void) memcpy(profile,"8BIM\04\04\0\0",8);
  profile[8]=(length >> 24) & 0xff;
  profile[9]=(length >> 16) & 0xff;
  profile[10]=(length >> 8) & 0xff;
  profile[11]=length & 0xff;
  for (i=0; i < length; i++)
    profile[i+12]=image->iptc_profile.info[i];
  if (roundup)
    profile[length+roundup+11]=0;
  (void) TIFFSetField(tiff,type,(uint32) length+roundup+12,(void *) profile);
#else
  length=image->iptc_profile.length;
  if (length == 0)
    return;
  roundup=(length & 0x01); /* round up for Photoshop */
  profile=MagickAllocateMemory(unsigned char *,length+roundup);
  if (profile == (unsigned char *) NULL)
    return;
  (void) memcpy(profile,image->iptc_profile.info,length);
  if (roundup)
    profile[length+roundup]=0;
  (void) TIFFSetField(tiff,type,(uint32) length+roundup,(void *) profile);
#endif
  MagickFreeMemory(profile);
}
#endif

static int32 TIFFWritePixels(TIFF *tiff,tdata_t scanline,unsigned long row,
  tsample_t sample,Image *image)
{
  int32
    status;

  long
    bytes_per_pixel,
    j,
    k,
    l;

  register long
    i;

  unsigned long
    number_tiles,
    tile_width;

  static unsigned char
    *scanlines = (unsigned char *) NULL,
    *tile_pixels = (unsigned char *) NULL;

  if (!TIFFIsTiled(tiff))
    return(TIFFWriteScanline(tiff,scanline,(uint32) row,sample));
  if (scanlines == (unsigned char *) NULL)
    scanlines=MagickAllocateMemory(unsigned char *,
      image->tile_info.height*TIFFScanlineSize(tiff));
  if (scanlines == (unsigned char *) NULL)
    return(-1);
  if (tile_pixels == (unsigned char *) NULL)
    tile_pixels=MagickAllocateMemory(unsigned char *,TIFFTileSize(tiff));
  if (tile_pixels == (unsigned char *) NULL)
    return(-1);
  /*
    Fill scanlines to tile height.
  */
  i=(long) (row % image->tile_info.height)*TIFFScanlineSize(tiff);
  (void) memcpy(scanlines+i,(char *) scanline,TIFFScanlineSize(tiff));
  if (((row % image->tile_info.height) != (image->tile_info.height-1)) &&
      (row != image->rows-1))
    return(0);
  /*
    Write tile to TIFF image.
  */
  status=0;
  bytes_per_pixel=TIFFTileSize(tiff)/(long)
    (image->tile_info.height*image->tile_info.width);
  number_tiles=
    (image->columns+image->tile_info.width-1)/image->tile_info.height;
  for (i=0; i < (long) number_tiles; i++)
  {
    tile_width=(i == (long) (number_tiles-1)) ?
      image->columns-(i*image->tile_info.width) : image->tile_info.width;
    for (j=0; j < (long) ((row % image->tile_info.height)+1); j++)
      for (k=0; k < (long) tile_width; k++)
      {
        register unsigned char
          *p,
          *q;

        p=scanlines+(j*TIFFScanlineSize(tiff)+(i*image->tile_info.width+k)*
          bytes_per_pixel);
        q=tile_pixels+(j*(TIFFTileSize(tiff)/image->tile_info.height)+k*
          bytes_per_pixel);
        for (l=0; l < bytes_per_pixel; l++)
          *q++=(*p++);
      }
    status=TIFFWriteTile(tiff,tile_pixels,(uint32) (i*image->tile_info.width),
      (uint32) ((row/image->tile_info.height)*image->tile_info.height),0,
      sample);
    if (status < 0)
      break;
  }
  if (row == (image->rows-1))
    {
      /*
        Free memory resources.
      */
      MagickFreeMemory(scanlines);
      scanlines=(unsigned char *) NULL;
      MagickFreeMemory(tile_pixels);
      tile_pixels=(unsigned char *) NULL;
    }
  return(status);
}

#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  ((8*1024)/TIFFScanlineSize(tiff))
#endif
static unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent];

  const ImageAttribute
    *attribute;

  long
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  TIFF
    *tiff;

  uint16
    bits_per_sample,
    compress_tag,
    photometric,
    samples_per_pixel;

  unsigned char
    *scanline;

  unsigned int
    filename_is_temporary=False,
    logging,
    status;

  unsigned long
    depth,
    scene,
    strip_size;

  StreamType
    btype;

  /*
    Open TIFF file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=IsEventLogging();
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  btype=image->blob->type;
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  tiff_exception=(&image->exception);
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  if (btype != FileStream)
    {
      filename_is_temporary=True;
      if(!AcquireTemporaryFileName(filename))
        ThrowWriterTemporaryFileException(filename);
    }
  else
    CloseBlob(image);
  tiff=TIFFOpen(filename,"wb");
  if (tiff == (TIFF *) NULL)
    {
      if (filename_is_temporary)
        LiberateTemporaryFile(filename);
      return(False);
    }
  scene=0;
  do
  {
    /*
      Initialize TIFF fields.
    */
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
                                 &samples_per_pixel);
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
                                 &bits_per_sample);
    if (LocaleCompare(image_info->magick,"PTIF") == 0)
      if (image->previous != (Image *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);

    depth=image->depth;
    bits_per_sample=8;
    compress_tag=COMPRESSION_NONE;
    switch (image->compression)
    {
#ifdef CCITT_SUPPORT
      case FaxCompression:
      {
        SetImageType(image,BilevelType);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Set image type to BilevelType");
        compress_tag=COMPRESSION_CCITTFAX3;
        break;
      }
      case Group4Compression:
      {
        SetImageType(image,BilevelType);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Set image type to BilevelType");
        compress_tag=COMPRESSION_CCITTFAX4;
        break;
      }
#endif
#ifdef YCBCR_SUPPORT
      case JPEGCompression:
      {
        compress_tag=COMPRESSION_JPEG;
        image->storage_class=DirectClass;
        depth=8;
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Set image type to DirectClass");
        break;
      }
#endif
#ifdef HasLZW
      case LZWCompression:
      {
        compress_tag=COMPRESSION_LZW;
        break;
      }
#endif
      case RLECompression:
      {
        compress_tag=COMPRESSION_PACKBITS;
        break;
      }
#ifdef ZIP_SUPPORT
      case ZipCompression:
      {
        compress_tag=COMPRESSION_ADOBE_DEFLATE;
        break;
      }
#endif
      default:
      {
        compress_tag=COMPRESSION_NONE;
        break;
      }
    }

    switch (image_info->compression)
    {
      case NoCompression: compress_tag=COMPRESSION_NONE; break;
#ifdef CCITT_SUPPORT
      case FaxCompression: compress_tag=COMPRESSION_CCITTFAX3; break;
      case Group4Compression: compress_tag=COMPRESSION_CCITTFAX4; break;
#endif
#ifdef JPEG_SUPPORT
      case JPEGCompression: compress_tag=COMPRESSION_JPEG; break;
#endif
#ifdef HasLZW
      case LZWCompression: compress_tag=COMPRESSION_LZW; break;
#endif
#ifdef PACKBITS_SUPPORT
      case RLECompression:
        compress_tag=COMPRESSION_PACKBITS; break;
#endif
#ifdef ZIP_SUPPORT
      case ZipCompression: compress_tag=COMPRESSION_ADOBE_DEFLATE; break;
#endif
      default: break;
    }
    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Using %s compression", CompressionTagToString(compress_tag));

    if (depth > 8)
      {
        bits_per_sample=16;
      }

    if (((image_info->colorspace == UndefinedColorspace) &&
         (image->colorspace == CMYKColorspace)) ||
         (image_info->colorspace == CMYKColorspace))
      {
        /*
          CMYK Image
        */
        photometric=PHOTOMETRIC_SEPARATED;
        samples_per_pixel=4;
        (void) TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Using INKSET_CMYK");
      }
    else
      {
        /*
          RGB or Gray Image.
        */
        TransformColorspace(image,RGBColorspace);
        photometric=PHOTOMETRIC_RGB;
        samples_per_pixel=3;
        if ((image_info->type != TrueColorType) &&
            (compress_tag != COMPRESSION_JPEG))
          {
            if ((image_info->type != PaletteType) &&
                IsGrayImage(image,&image->exception))
              {
                /*
                  Grayscale Image
                  
                  Produce a compact gray image by testing the
                  image pixels for depth and storing with a
                  portable bits-per-sample value which is the best
                  match for the depth.
                */
                photometric=PHOTOMETRIC_MINISBLACK;
                samples_per_pixel=1;
                depth=GetImageDepth(image,&image->exception);

                if (depth == 1)
                  {
                    if ((compress_tag == COMPRESSION_CCITTFAX3) ||
                        (compress_tag == COMPRESSION_CCITTFAX4))
                      photometric=PHOTOMETRIC_MINISWHITE;
                  }

                for (bits_per_sample=1; bits_per_sample < depth; )
                  bits_per_sample*=2;
              }
            else
              if (image->storage_class == PseudoClass)
                {
                  /*
                    Colormapped Image.
                  */
                  photometric=PHOTOMETRIC_PALETTE;
                  samples_per_pixel=1;
                  bits_per_sample=1;
                  while ((1UL << bits_per_sample) < image->colors)
                    bits_per_sample*=2;
                }
          }
      }

    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Image depth %lu bits",depth);

    if (image->matte)
      {
        /*
          Image has a matte channel.
        */
        uint16
          extra_samples,
          sample_info[1];

        samples_per_pixel += 1;
        extra_samples=1;
        /* sample_info[0]=EXTRASAMPLE_UNASSALPHA; */
        sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
        (void) TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
          &sample_info);
      }

    /*
      Allow the advanced user to over-ride some TIFF write options
    */
    {
      const char *
        value;

      unsigned int
        new_value,
        old_value;

      /*
        Bits per sample
      */
      value=AccessDefinition(image_info,"tiff","bits-per-sample");
      if (value)
        {
          old_value=bits_per_sample;
          new_value=atoi(value);
          if (new_value <= QuantumDepth)
            {
              bits_per_sample=new_value;
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "User override (bits-per-sample): %u bits per sample (was %u)",
                  (unsigned int) bits_per_sample, old_value);
            }
          else
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "User override (bits-per-sample) IGNORED (value %u exceeds limit of %u)",
                  new_value, (unsigned int) QuantumDepth);
            }
        }

      /*
        Samples per pixel
      */
      value=AccessDefinition(image_info,"tiff","samples-per-pixel");
      if (value)
        {
          old_value=samples_per_pixel;
          samples_per_pixel=atoi(value);
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "User override (samples-per-pixel): %u samples per pixel (was %u)",
              (unsigned int) samples_per_pixel, old_value);
        }
    }

    (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,samples_per_pixel);
    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,bits_per_sample);

    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Using %s photometric, %u samples per pixel, %u bits per sample",
        PhotometricTagToString(photometric),
        (unsigned int) samples_per_pixel,
        (unsigned int) bits_per_sample);

    (void) TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
    (void) TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);
    switch (image_info->endian)
    {
      case LSBEndian:
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_LSB2MSB);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "Using LSB2MSB fillorder");
        break;
      case MSBEndian:
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "Using MSB2LSB fillorder");
        break;
      default:
      case UndefinedEndian:
      {
#if defined(HOST_BIGENDIAN)
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "Using MSB2LSB fillorder");
#else
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_LSB2MSB);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "Using LSB2MSB fillorder");
#endif
        break;
      }
    }
    (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    if (photometric == PHOTOMETRIC_RGB)
      if ((image_info->interlace == PlaneInterlace) ||
          (image_info->interlace == PartitionInterlace))
        (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_SEPARATE);
    strip_size=Max(TIFFDefaultStripSize(tiff,-1),1);

    switch (compress_tag)
    {
      case COMPRESSION_JPEG:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,
          strip_size+(8-(strip_size % 8)));
        break;
      }
      case COMPRESSION_ADOBE_DEFLATE:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);
        if((photometric == PHOTOMETRIC_RGB) ||
           ((photometric == PHOTOMETRIC_MINISBLACK) && (bits_per_sample >= 8)))
          (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,2);
        (void) TIFFSetField(tiff,TIFFTAG_ZIPQUALITY,9);
        break;
      }
      case COMPRESSION_CCITTFAX4:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);
        break;
      }
      case COMPRESSION_LZW:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,strip_size);
        /*
          Use horizontal differencing (type 2) for images which are
          likely to be continuous tone.  The TIFF spec says that this
          usually leads to better compression.
        */
        if((photometric == PHOTOMETRIC_RGB) ||
           ((photometric == PHOTOMETRIC_MINISBLACK) && (bits_per_sample >= 8)))
          (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,2);
        break;
      }
      default:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,strip_size);
        break;
      }
    }
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        unsigned short
          units;

        /*
          Set image resolution.
        */
        units=RESUNIT_NONE;
        if (image->units == PixelsPerInchResolution)
          units=RESUNIT_INCH;
        if (image->units == PixelsPerCentimeterResolution)
          units=RESUNIT_CENTIMETER;
        (void) TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(uint16) units);
        (void) TIFFSetField(tiff,TIFFTAG_XRESOLUTION,image->x_resolution);
        (void) TIFFSetField(tiff,TIFFTAG_YRESOLUTION,image->y_resolution);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        float
          chromaticity[6];

        /*
          Set image chromaticity.
        */
        chromaticity[0]=image->chromaticity.red_primary.x;
        chromaticity[1]=image->chromaticity.red_primary.y;
        chromaticity[2]=image->chromaticity.green_primary.x;
        chromaticity[3]=image->chromaticity.green_primary.y;
        chromaticity[4]=image->chromaticity.blue_primary.x;
        chromaticity[5]=image->chromaticity.blue_primary.y;
        (void) TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,chromaticity);
        chromaticity[0]=image->chromaticity.white_point.x;
        chromaticity[1]=image->chromaticity.white_point.y;
        (void) TIFFSetField(tiff,TIFFTAG_WHITEPOINT,chromaticity);
      }
#if defined(ICC_SUPPORT)
    if (image->color_profile.length != 0)
      (void) TIFFSetField(tiff,TIFFTAG_ICCPROFILE,(uint32)
        image->color_profile.length,(void *) image->color_profile.info);
#endif
#if defined(IPTC_SUPPORT)
#if defined(PHOTOSHOP_SUPPORT)
    if (image->iptc_profile.length != 0)
      WriteNewsProfile(tiff,TIFFTAG_PHOTOSHOP,image);
#else
    if (image->iptc_profile.length != 0)
      WriteNewsProfile(tiff,TIFFTAG_RICHTIFFIPTC,image);
#endif
#endif
    if (image_info->adjoin && (GetImageListLength(image) > 1))
      {
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
        if (image->scene != 0)
          (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,(unsigned short)
            image->scene,GetImageListLength(image));
      }
    attribute=GetImageAttribute(image,"artist");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_ARTIST,attribute->value);
    attribute=GetImageAttribute(image,"timeStamp");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_DATETIME,attribute->value);
    attribute=GetImageAttribute(image,"make");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_MAKE,attribute->value);
    attribute=GetImageAttribute(image,"model");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_MODEL,attribute->value);
    (void) TIFFSetField(tiff,TIFFTAG_SOFTWARE,
      GetMagickVersion((unsigned long *) NULL));
    (void) TIFFSetField(tiff,TIFFTAG_DOCUMENTNAME,image->filename);
    attribute=GetImageAttribute(image,"copyright");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,33432,attribute->value);
    attribute=GetImageAttribute(image,"kodak-33423");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,33423,attribute->value);
    attribute=GetImageAttribute(image,"kodak-36867");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,36867,attribute->value);
    attribute=GetImageAttribute(image,"label");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_PAGENAME,attribute->value);
    attribute=GetImageAttribute(image,"comment");
    if (attribute != (const ImageAttribute *) NULL)
      (void) TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,attribute->value);
    /*
      Write image scanlines.
    */
    scanline=MagickAllocateMemory(unsigned char *,8*TIFFScanlineSize(tiff));
    if (scanline == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
    switch (photometric)
    {
      case PHOTOMETRIC_RGB:
      {
        /*
          RGB TIFF image.
        */
        switch (image_info->interlace)
        {
          case NoInterlace:
          default:
          {
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              if (!image->matte)
                (void) PopImagePixels(image,RGBQuantum,scanline);
              else
                (void) PopImagePixels(image,RGBAQuantum,scanline);
              if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  {
                    status=MagickMonitor(SaveImageText,y,image->rows,
                      &image->exception);
                    if (status == False)
                      break;
                  }
            }
            break;
          }
          case PlaneInterlace:
          case PartitionInterlace:
          {
            /*
              Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
            */
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) PopImagePixels(image,RedQuantum,scanline);
              if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                break;
            }
            if (!MagickMonitor(SaveImageText,100,400,&image->exception))
              break;
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) PopImagePixels(image,GreenQuantum,scanline);
              if (TIFFWritePixels(tiff,(char *) scanline,y,1,image) < 0)
                break;
            }
            if (!MagickMonitor(SaveImageText,200,400,&image->exception))
              break;
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              (void) PopImagePixels(image,BlueQuantum,scanline);
              if (TIFFWritePixels(tiff,(char *) scanline,y,2,image) < 0)
                break;
            }
            if (!MagickMonitor(SaveImageText,300,400,&image->exception))
              break;
            if (image->matte)
              for (y=0; y < (long) image->rows; y++)
              {
                p=AcquireImagePixels(image,0,y,image->columns,1,
                  &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                (void) PopImagePixels(image,AlphaQuantum,scanline);
                if (TIFFWritePixels(tiff,(char *) scanline,y,3,image) < 0)
                  break;
              }
            if (!MagickMonitor(SaveImageText,400,400,&image->exception))
              break;
            break;
          }
        }
        break;
      }
      case PHOTOMETRIC_SEPARATED:
      {
        /*
          CMYK TIFF image.
        */
        TransformColorspace(image,CMYKColorspace);
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          if (!image->matte)
            (void) PopImagePixels(image,CMYKQuantum,scanline);
          else
            (void) PopImagePixels(image,CMYKAQuantum,scanline);
          if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        uint16
          *blue,
          *green,
          *red;

        /*
          Colormapped TIFF image.
        */
        blue=MagickAllocateMemory(unsigned short *,65536L*sizeof(unsigned short));
        green=MagickAllocateMemory(unsigned short *,65536L*sizeof(unsigned short));
        red=MagickAllocateMemory(unsigned short *,65536L*sizeof(unsigned short));
        if ((blue == (unsigned short *) NULL) ||
            (green == (unsigned short *) NULL) ||
            (red == (unsigned short *) NULL))
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
            image);
        /*
          Initialize TIFF colormap.
        */
        memset(red,0,65536L*sizeof(unsigned short));
        memset(green,0,65536L*sizeof(unsigned short));
        memset(blue,0,65536L*sizeof(unsigned short));
        for (i=0; i < (long) image->colors; i++)
        {
          red[i]=ScaleQuantumToShort(image->colormap[i].red);
          green[i]=ScaleQuantumToShort(image->colormap[i].green);
          blue[i]=ScaleQuantumToShort(image->colormap[i].blue);
        }
        (void) TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
        MagickFreeMemory(red);
        MagickFreeMemory(green);
        MagickFreeMemory(blue);
        /* Notice that this case falls through to the default case */
      }
      default:
      {
        /*
          Convert PseudoClass packets to contiguous scanlines.
        */
        for (y=0; y < (long) image->rows; y++)
          {
            BitStreamWriteHandle
              bit_stream;
            
            register unsigned long
              scale;
            
            register unsigned int
              quantum;
            
            scale=MaxRGB / (MaxRGB >> (QuantumDepth-bits_per_sample));
            p=AcquireImagePixels(image,0,y,image->columns,1,
                                 &image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            BitStreamInitializeWrite(&bit_stream,scanline);
            if (photometric != PHOTOMETRIC_PALETTE)
              {
                /* Grayscale DirectClass */
                for (x= image->columns; x > 0; --x)
                  {
                    if (photometric == PHOTOMETRIC_MINISWHITE)
                      /* PHOTOMETRIC_MINISWHITE */
                      quantum=(MaxRGB - p->red)/scale;
                    else
                      /* PHOTOMETRIC_MINISBLACK */
                      quantum=p->red/scale;

                    BitStreamWrite(&bit_stream,bits_per_sample,quantum);
                    if (samples_per_pixel == 2)
                      /* with opacity sample */
                      {
                        quantum=(MaxRGB - p->opacity)/scale;
                        BitStreamWrite(&bit_stream,bits_per_sample,quantum);
                      }
                    p++;
                  }
              }
            else
              {
                /* Color/Gray PseudoClass */
                indexes=GetIndexes(image);
                for (x= image->columns; x > 0; --x)
                  {
                    BitStreamWrite(&bit_stream,bits_per_sample,*indexes++);
                    if (samples_per_pixel == 2)
                      /* with opacity sample */
                      {
                        quantum=(MaxRGB - p->opacity)/scale;
                        BitStreamWrite(&bit_stream,bits_per_sample,quantum);
                        p++;
                      }
                  }
              }
            if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                {
                  status=MagickMonitor(SaveImageText,y,image->rows,
                                       &image->exception);
                  if (status == False)
                    break;
                }
          }
        break;
      }
    }
    MagickFreeMemory(scanline);
    if (image_info->verbose == True)
      TIFFPrintDirectory(tiff,stdout,False);
    (void) TIFFWriteDirectory(tiff);
    if (image->next == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=MagickMonitor(SaveImagesText,scene++,GetImageListLength(image),
      &image->exception);
    if (status == False)
      break;
  } while (image_info->adjoin);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  TIFFClose(tiff);
  if (btype != FileStream)
#ifdef SLOW_METHOD
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to image blob.
      */
      file=fopen(filename,"rb");
      if (file == (FILE *) NULL)
        ThrowWriterException(FileOpenError,UnableToOpenFile,image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) WriteBlobByte(image,c);
      (void) fclose(file);
      LiberateTemporaryFile(filename);
      CloseBlob(image);
    }
#else
    {
      int
        file;

      struct stat
        attributes;

      unsigned char
        *buffer;

      void
        *map;

      size_t
        length;

      file=open(filename,O_RDONLY | O_BINARY,0777);
      if (file == -1)
        {
          ThrowWriterException(FileOpenError,UnableToOpenFile,image);
        }
      if ((fstat(file,&attributes) < 0) ||
          (attributes.st_size != (size_t) attributes.st_size) ||
            (attributes.st_size <= (size_t) 0))
        {
          (void) close(file);
          ThrowWriterException(FileOpenError,UnableToOpenFile,image);
        }
      length=(size_t) attributes.st_size;
      map=MapBlob(file,ReadMode,0,length);
      if (map != (void *) NULL)
        {
          (void) WriteBlob(image,length,map);
          UnmapBlob(map,length);
        }
      else
        {
          off_t
            count,
            result;

          register size_t
            i;

          count = 32768;
          if (count > length)
            count = length;
          buffer=MagickAllocateMemory(unsigned char *,count);
          if (buffer == (unsigned char *) NULL)
            {
              (void) close(file);
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            }
          for (i=0; i < length; i+=count)
          {
            result=read(file,buffer,count);
            if (result <= 0)
              break;
            (void) WriteBlob(image,result,buffer);
          }
          MagickFreeMemory(buffer);
        }
      (void) close(file);
      LiberateTemporaryFile(filename);
      CloseBlob(image);
    }
#endif
  return(True);
}
#endif
