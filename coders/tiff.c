/*
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
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "cache.h"
#include "color.h"
#include "constitute.h"
#include "log.h"
#include "magick.h"
#include "monitor.h"
#include "resize.h"
#include "utility.h"
#include "version.h"
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

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
      LiberateMemory((void **) &image->color_profile.info);
      image->color_profile.length=0;
    }
  image->color_profile.info=(unsigned char *) AcquireMemory(length);
  if (image->color_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
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
      LiberateMemory((void **) &image->iptc_profile.info);
      image->iptc_profile.length=0;
      image->iptc_profile.info=(unsigned char *) NULL;
    }
  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle IPTC tag.
      */
      length*=4;
      image->iptc_profile.info=(unsigned char *) AcquireMemory(length);
      if (image->iptc_profile.info == (unsigned char *) NULL)
        ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
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
      LiberateMemory((void **) &image->iptc_profile.info);
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
  image->iptc_profile.info=(unsigned char *) AcquireMemory(length);
  if (image->iptc_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
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
  ThrowException(tiff_exception,CoderError,message,module);
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
  ThrowException(tiff_exception,DelegateWarning,message,module);
  return(True);
}

static tsize_t TIFFWriteBlob(thandle_t image,tdata_t data,tsize_t size)
{
  return((tsize_t) WriteBlob((Image *) image,(size_t) size,data));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

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
    length,
    width;

  unsigned char
    *scanline;

  unsigned int
    method,
    logging,
    status;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"UnableToOpenFile",image);
  tiff_exception=exception;
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  if ((image->blob->type == FileStream) || (image->blob->type == BlobStream))
    tiff=TIFFClientOpen(image->filename,"rb",(thandle_t) image,TIFFReadBlob,
      TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
      TIFFUnmapBlob);
  else
    {
      TemporaryFilename(filename);
      (void) ImageToFile(image,filename,exception);
      tiff=TIFFOpen(filename,"rb");
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Open temporary file %.1024s",filename);
    }
  if (tiff == (TIFF *) NULL)
    {
      if ((image->blob->type != FileStream) &&
          (image->blob->type != BlobStream))
        remove(filename);
      ThrowReaderException(FileOpenError,"UnableToOpenFile",image)
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
          if ((image->blob->type != FileStream) &&
              (image->blob->type != BlobStream))
            remove(filename);
          ThrowReaderException(CorruptImageError,"UnableToReadSubimage",
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
        switch (photometric)
        {
          case PHOTOMETRIC_MINISBLACK:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: MINISBLACK");
            break;
          }
          case PHOTOMETRIC_MINISWHITE:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: MINISBLACK");
            break;
          }
          case PHOTOMETRIC_PALETTE:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: PALETTE");
            break;
          }
          case PHOTOMETRIC_RGB:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: RGB");
            break;
          }
          case PHOTOMETRIC_CIELAB:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: CIELAB");
            break;
          }
          case PHOTOMETRIC_SEPARATED:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric: SEPARATED");
            break;
          }
          default:
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "Photometric interpretation: %u",photometric);
            break;
          }
        }
      }
    if (photometric == PHOTOMETRIC_CIELAB)
      {
        TIFFClose(tiff);
        ThrowReaderException(CoderError,"UnableToReadCIELABImages",image)
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
      (void) ReadColorProfile(text,length,image);
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
    image->depth=bits_per_sample <= 8 ? 8 : bits_per_sample;
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
        image->colors=1 << bits_per_sample;
        if (!AllocateImageColormap(image,image->colors))
          {
            TIFFClose(tiff);
            if ((image->blob->type != FileStream) &&
                (image->blob->type != BlobStream))
              remove(filename);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
              image)
          }
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
    method=0;
    if ((image->storage_class == DirectClass) || image->matte)
      {
        method=2;
        if (photometric == PHOTOMETRIC_RGB)
          if ((samples_per_pixel >= 2) && (interlace == PLANARCONFIG_CONTIG))
            method=1;
        if (image->colorspace == CMYKColorspace)
          method=1;
      }
    if (TIFFIsTiled(tiff))
      method=2;
    switch (method)
    {
      case 0:
      {
        register unsigned char
          *r;

        unsigned char
          *quantum_scanline;

        unsigned long
          packet_size;

        /*
          Convert TIFF image to PseudoClass MIFF image.
        */
        packet_size=bits_per_sample > 8 ? 2 : 1;
        if (image->matte)
          packet_size*=2;
        quantum_scanline=(unsigned char *) AcquireMemory(packet_size*width);
        scanline=(unsigned char *) AcquireMemory(8*TIFFScanlineSize(tiff));
        if ((quantum_scanline == (unsigned char *) NULL) ||
            (scanline == (unsigned char *) NULL))
          {
            TIFFClose(tiff);
            if ((image->blob->type != FileStream) &&
                (image->blob->type != BlobStream))
              remove(filename);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
              image)
          }
        /*
          Create colormap.
        */
        switch (photometric)
        {
          case PHOTOMETRIC_MINISBLACK:
          {
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=(Quantum)
                (((double) MaxRGB*i)/Max(image->colors-1,1)+0.5);
              image->colormap[i].green=image->colormap[i].red;
              image->colormap[i].blue=image->colormap[i].red;
            }
            break;
          }
          case PHOTOMETRIC_MINISWHITE:
          default:
          {
            for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=(Quantum) (MaxRGB-
                (((double) MaxRGB*i)/Max(image->colors-1,1))+0.5);
              image->colormap[i].green=image->colormap[i].red;
              image->colormap[i].blue=image->colormap[i].red;
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
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) TIFFReadScanline(tiff,(char *) scanline,(uint32) y,0);
          if (bits_per_sample == 16)
            {
              unsigned long
                lsb_first;

              /*
                Ensure the header byte-order is most-significant byte first.
              */
              lsb_first=1;
              if (*(char *) &lsb_first)
                MSBOrderShort(scanline,(TIFFScanlineSize(tiff) << 1)+4);
            }
          p=scanline;
          r=quantum_scanline;
          switch (bits_per_sample)
          {
            case 1:
            {
              register long
                bit;

              register unsigned char
                c;

              for (x=((long) width-7); x > 0; x-=8)
              {
                c=*p++;
                *r++=(c & 0x80) >> 7;
                *r++=(c & 0x40) >> 6;
                *r++=(c & 0x20) >> 5;
                *r++=(c & 0x10) >> 4;
                *r++=(c & 0x08) >> 3;
                *r++=(c & 0x04) >> 2;
                *r++=(c & 0x02) >> 1;
                *r++=(c & 0x01);
              }
              if ((width % 8) != 0)
                {
                  c=*p;
                  for (bit=7; bit >= (long) (8-(width % 8)); bit--)
                    *r++=(c >> bit) & 0x01;
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((long) width-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((width % 4) != 0)
                {
                  for (i=3; i >= (long) (4-(width % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((long) width-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((width % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              for (x=(long) width; x > 0; x--)
                *r++=(*p++);
              break;
            }
            case 16:
            {
              if (image->depth <= 8)
                {
                  for (x=0; x < (long) width; x++)
                  {
                    *r=(*p++) << 8;
                    *r|=(*p++);
                    r++;
                  }
                  break;
                }
              for (x=0; x < (long) image->columns; x++)
              {
                *r++=(*p++);
                *r++=(*p++);
              }
              break;
            }
            default:
              break;
          }
          /*
            Transfer image scanline.
          */
          if (image->matte)
            (void) PushImagePixels(image,GrayAlphaQuantum,quantum_scanline);
          else
            if (photometric != PHOTOMETRIC_PALETTE)
              (void) PushImagePixels(image,GrayQuantum,quantum_scanline);
            else
              (void) PushImagePixels(image,IndexQuantum,quantum_scanline);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                break;
        }
        LiberateMemory((void **) &scanline);
        LiberateMemory((void **) &quantum_scanline);
        break;
      }
      case 1:
      {
        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        scanline=(unsigned char *) AcquireMemory(8*TIFFScanlineSize(tiff));
        if (scanline == (unsigned char *) NULL)
          {
            TIFFClose(tiff);
            if ((image->blob->type != FileStream) &&
                (image->blob->type != BlobStream))
              remove(filename);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
              image)
          }
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          (void) TIFFReadScanline(tiff,(char *) scanline,(uint32) y,0);
          if (bits_per_sample == 16)
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
        LiberateMemory((void **) &scanline);
        break;
      }
      case 2:
      default:
      {
        register uint32
          *p;

        uint32
          *pixels;

        unsigned long
          number_pixels;

        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        number_pixels=image->columns*image->rows;
        pixels=(uint32 *)
          AcquireMemory((number_pixels+6*image->columns)*sizeof(uint32));
        if (pixels == (uint32 *) NULL)
          {
            TIFFClose(tiff);
            if ((image->blob->type != FileStream) &&
                (image->blob->type != BlobStream))
              remove(filename);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",
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
        LiberateMemory((void **) &pixels);
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
  if ((image->blob->type != FileStream) && (image->blob->type != BlobStream))
    remove(filename);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return(image);
}
#else
static Image *ReadTIFFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ThrowException(exception,CoderError,"TIFFLibraryIsNotAvailable",
    image_info->filename);
  return((Image *) NULL);
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
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
    ThrowWriterException(FileOpenError,image->exception.reason,image);
  do
  {
    pyramid_image->next=ResizeImage(image,pyramid_image->columns/2,
      pyramid_image->rows/2,TriangleFilter,1.0,&image->exception);
    if (pyramid_image->next == (Image *) NULL)
      ThrowWriterException(FileOpenError,image->exception.reason,image);
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
#else
static unsigned int WritePTIFImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(CoderError,"TIFFLibraryIsNotAvailable",
    image->filename);
}
#endif

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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
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
      profile=(unsigned char *) AcquireMemory(length+roundup);
      if ((length == 0) || (profile == (unsigned char *) NULL))
        return;
      (void) memcpy(profile,image->iptc_profile.info,length);
      for (i=0; i < (long) roundup; i++)
        profile[length+i] = 0;
      length=(image->iptc_profile.length+roundup)/4;
      if (TIFFIsByteSwapped(tiff))
        TIFFSwabArrayOfLong((uint32 *) profile,length);
      (void) TIFFSetField(tiff,type,(uint32) (length+roundup),(void *) profile);
      LiberateMemory((void **) &profile);
      return;
    }
  /*
    Handle TIFFTAG_PHOTOSHOP tag.
  */
#if defined(GET_ONLY_IPTC_DATA)
  length=image->iptc_profile.length;
  roundup=(length & 0x01); /* round up for Photoshop */
  profile=(unsigned char *) AcquireMemory(length+roundup+12);
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
  profile=(unsigned char *) AcquireMemory(length+roundup);
  if (profile == (unsigned char *) NULL)
    return;
  (void) memcpy(profile,image->iptc_profile.info,length);
  if (roundup)
    profile[length+roundup]=0;
  (void) TIFFSetField(tiff,type,(uint32) length+roundup,(void *) profile);
#endif
  LiberateMemory((void **) &profile);
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
    scanlines=(unsigned char *)
      AcquireMemory(image->tile_info.height*TIFFScanlineSize(tiff));
  if (scanlines == (unsigned char *) NULL)
    return(-1);
  if (tile_pixels == (unsigned char *) NULL)
    tile_pixels=(unsigned char *)AcquireMemory(TIFFTileSize(tiff));
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
      LiberateMemory((void **) &scanlines);
      scanlines=(unsigned char *) NULL;
      LiberateMemory((void **) &tile_pixels);
      tile_pixels=(unsigned char *) NULL;
    }
  return(status);
}

static unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  ((8*1024)/TIFFScanlineSize(tiff))
#endif

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

  register unsigned char
    *q;

  TIFF
    *tiff;

  uint16
    compress_tag,
    photometric;

  unsigned char
    *scanline;

  unsigned int
    logging,
    status;

  unsigned long
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
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  btype=image->blob->type;
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  tiff_exception=(&image->exception);
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  if (btype != FileStream)
    TemporaryFilename(filename);
  else
    CloseBlob(image);
  tiff=TIFFOpen(filename,"wb");
  if (tiff == (TIFF *) NULL)
    return(False);
  scene=0;
  do
  {
    /*
      Initialize TIFF fields.
    */
    if (LocaleCompare(image_info->magick,"PTIF") == 0)
      if (image->previous != (Image *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);
    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,8);
    compress_tag=COMPRESSION_NONE;
    switch (image->compression)
    {
#ifdef CCITT_SUPPORT
      case FaxCompression:
      {
        SetImageType(image,BilevelType);
        compress_tag=COMPRESSION_CCITTFAX3;
        break;
      }
      case Group4Compression:
      {
        SetImageType(image,BilevelType);
        compress_tag=COMPRESSION_CCITTFAX4;
        break;
      }
#endif
#ifdef JPEG_SUPPORT
      case JPEGCompression:
      {
        compress_tag=COMPRESSION_JPEG;
        image->storage_class=DirectClass;
        image->depth=8;
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
    if (image->depth > 8)
      (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,16);
    if (((image_info->colorspace == UndefinedColorspace) &&
         (image->colorspace == CMYKColorspace)) ||
         (image_info->colorspace == CMYKColorspace))
      {
        photometric=PHOTOMETRIC_SEPARATED;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,4);
        (void) TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
      }
    else
      {
        /*
          Full color TIFF raster.
        */
        (void) TransformRGBImage(image,image->colorspace);
        photometric=PHOTOMETRIC_RGB;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,3);
        if (image_info->type != TrueColorType)
          {
            if ((image_info->type != PaletteType) &&
                IsGrayImage(image,&image->exception))
              {
                (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                photometric=PHOTOMETRIC_MINISBLACK;
                if ((image->storage_class == PseudoClass) &&
                    IsMonochromeImage(image,&image->exception))
                  {
                    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,1);
                    photometric=PHOTOMETRIC_MINISWHITE;
                    compress_tag=COMPRESSION_CCITTFAX4;
                  }
              }
            else
              if (image->storage_class == PseudoClass)
                {
                  /*
                    Colormapped TIFF raster.
                  */
                  (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                  if (image->colors <= 256)
                    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,8);
                  photometric=PHOTOMETRIC_PALETTE;
                }
          }
      }
    if (image->matte)
      {
        uint16
          extra_samples,
          sample_info[1],
          samples_per_pixel;

        /*
          TIFF has a matte channel.
        */
        extra_samples=1;
        //sample_info[0]=EXTRASAMPLE_UNASSALPHA;
        sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
          &samples_per_pixel);
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,samples_per_pixel+1);
        (void) TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
          &sample_info);
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
    (void) TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
    (void) TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);
    switch (image_info->endian)
    {
      case LSBEndian:
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_LSB2MSB);
        break;
      case MSBEndian:
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
        break;
      default:
      case UndefinedEndian:
      {
#if defined(HOST_BIGENDIAN)
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
#else
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_LSB2MSB);
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
        (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,2);
        (void) TIFFSetField(tiff,TIFFTAG_ZIPQUALITY,9);
        break;
      }
      case COMPRESSION_CCITTFAX4:
      {
        (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);
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
    scanline=(unsigned char *) AcquireMemory(8*TIFFScanlineSize(tiff));
    if (scanline == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image);
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
        if (image->colorspace != CMYKColorspace)
          (void) RGBTransformImage(image,CMYKColorspace);
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
        blue=(unsigned short *) AcquireMemory(65536L*sizeof(unsigned short));
        green=(unsigned short *) AcquireMemory(65536L*sizeof(unsigned short));
        red=(unsigned short *) AcquireMemory(65536L*sizeof(unsigned short));
        if ((blue == (unsigned short *) NULL) ||
            (green == (unsigned short *) NULL) ||
            (red == (unsigned short *) NULL))
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",
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
        LiberateMemory((void **) &red);
        LiberateMemory((void **) &green);
        LiberateMemory((void **) &blue);
      }
      default:
      {
        register unsigned char
          bit,
          byte,
          polarity;

        if ((image->storage_class == DirectClass) ||
            (image_info->type == PaletteType) ||
            !IsMonochromeImage(image,&image->exception))
          {
            /*
              Convert PseudoClass packets to contiguous grayscale scanlines.
            */
            for (y=0; y < (long) image->rows; y++)
            {
              p=AcquireImagePixels(image,0,y,image->columns,1,
                &image->exception);
              if (p == (const PixelPacket *) NULL)
                break;
              if (image->matte)
                (void) PopImagePixels(image,GrayAlphaQuantum,scanline);
              else
                if (photometric != PHOTOMETRIC_PALETTE)
                  (void) PopImagePixels(image,GrayQuantum,scanline);
                else
                  (void) PopImagePixels(image,IndexQuantum,scanline);
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
        /*
          Convert PseudoClass packets to contiguous monochrome scanlines.
        */
        SetImageType(image,BilevelType);
        polarity=PixelIntensityToQuantum(&image->colormap[0]) < (MaxRGB/2);
        if (photometric == PHOTOMETRIC_PALETTE)
          polarity=1;
        else
          if (image->colors == 2)
            {
              polarity=PixelIntensityToQuantum(&image->colormap[0]) <
                PixelIntensityToQuantum(&image->colormap[1]);
              if (photometric == PHOTOMETRIC_MINISBLACK)
                polarity=!polarity;
            }
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          bit=0;
          byte=0;
          q=scanline;
          for (x=0; x < (long) image->columns; x++)
          {
            byte<<=1;
            if (indexes[x] != polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
          }
          if (bit != 0)
            *q++=byte << (8-bit);
          if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
        }
        break;
      }
    }
    LiberateMemory((void **) &scanline);
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
        ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) WriteBlobByte(image,c);
      (void) fclose(file);
      (void) remove(filename);
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
          ThrowWriterException(FileOpenError,"Unable to open file",image);
        }
      if ((fstat(file,&attributes) < 0) ||
          (attributes.st_size != (size_t) attributes.st_size) ||
            (attributes.st_size <= (size_t) 0))
        {
          (void) close(file);
          ThrowWriterException(FileOpenError,"Unable to open file",image);
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
          buffer=(unsigned char *) AcquireMemory(count);
          if (buffer == (unsigned char *) NULL)
            {
              (void) close(file);
              ThrowWriterException(FileOpenError,"Memory allocation failed",image);
            }
          for (i=0; i < length; i+=count)
          {
            result=read(file,buffer,count);
            if (result <= 0)
              break;
            (void) WriteBlob(image,result,buffer);
          }
          LiberateMemory((void **) &buffer);
        }
      (void) close(file);
      (void) remove(filename);
      CloseBlob(image);
    }
#endif
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return(True);
}
#else
static unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(CoderError,"TIFFLibraryIsNotAvailable",
    image->filename);
}
#endif
