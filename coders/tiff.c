/*
% Copyright (C) 2003, 2004, 2005 GraphicsMagick Group
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
%                       Read/Write TIFF Image Format.                         %
%                                                                             %
%                                                                             %
%                        Original Software Design                             %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                     Re-Written For GraphicsMagick                           %
%                             Bob Friesenhahn                                 %
%                                2002-2005                                    %
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
#include "magick/profile.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasTIFF)
#  if defined(HAVE_TIFFCONF_H)
#    include "tiffconf.h"
#  endif /* defined(HAVE_TIFFCONF_H) */
#  include "tiffio.h"
#  if !defined(COMPRESSION_ADOBE_DEFLATE)
#    define COMPRESSION_ADOBE_DEFLATE  8
#  endif  /* !defined(COMPRESSION_ADOBE_DEFLATE) */

/*
  Global declarations.
*/
static ExceptionInfo
  *tiff_exception;

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

static unsigned int ReadNewsProfile(char *text,long int length,Image *image,
  int type)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle IPTC tag.
      */
      length*=4;
      return SetImageProfile(image,"IPTC",p,(size_t) length);
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
  return SetImageProfile(image,"8BIM",p,(size_t) length);
}

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Close BLOB */
static int TIFFCloseBlob(thandle_t image)
{
/*   if (((Image *) image)->logging) */
/*     (void) LogMagickEvent(CoderEvent,GetMagickModule(),"TIFF close blob"); */
  CloseBlob((Image *) image);
  return(0);
}

/* Report errors. Not thread safe!  */
static unsigned int TIFFErrors(const char *module,const char *format,
  va_list warning)
{
  char
    message[MaxTextExtent];

  errno=0;
  (void) vsnprintf(message,MaxTextExtent-2,format,warning);
  message[MaxTextExtent-2]='\0';
  (void) strlcat(message,".",MaxTextExtent);
  ThrowException2(tiff_exception,CoderError,message,module);
  return(True);
}

/* Memory map entire input file in read-only mode. */
static int TIFFMapBlob(thandle_t image,tdata_t *base,toff_t *size)
{
  *base = (tdata_t *) GetBlobStreamData((Image *) image);
  if (*base)
    *size = (toff_t) GetBlobSize((Image *) image);

  if (*base)
    {
/*       if (((Image *) image)->logging) */
/*         (void) LogMagickEvent(CoderEvent,GetMagickModule(), */
/*           "TIFF mapped blob: base=%p size=%ld",*base, *size); */
      return 1;
    }
  return(0);
}

/* Read BLOB data at current offset */
static tsize_t TIFFReadBlob(thandle_t image,tdata_t data,tsize_t size)
{
/*   if (((Image *) image)->logging) */
/*     (void) LogMagickEvent(CoderEvent,GetMagickModule(), */
/*                           "TIFF read blob: data=%p size=%ld", data, size); */
  return((tsize_t) ReadBlob((Image *) image,(size_t) size,data));
}

/* Seek to BLOB offset */
static toff_t TIFFSeekBlob(thandle_t image,toff_t offset,int whence)
{
/*   if (((Image *) image)->logging) */
/*     (void) LogMagickEvent(CoderEvent,GetMagickModule(), */
/*       "TIFF seek blob: offset=%ld whence=%d", offset, whence); */
  return((toff_t) SeekBlob((Image *) image,offset,whence));
}

/* Obtain BLOB size */
static toff_t TIFFGetBlobSize(thandle_t image)
{
/*   if (((Image *) image)->logging) */
/*     (void) LogMagickEvent(CoderEvent,GetMagickModule(), */
/*       "TIFF get blob size"); */
  return((toff_t) GetBlobSize((Image *) image));
}

/* Unmap BLOB memory */
static void TIFFUnmapBlob(thandle_t ARGUNUSED(image),
                          tdata_t ARGUNUSED(base),
                          toff_t ARGUNUSED(size))
{
/*   if (((Image *) image)->logging) */
/*     (void) LogMagickEvent(CoderEvent,GetMagickModule(), */
/*       "TIFF unmap blob: base=%p size=%ld", base, size); */
}

/* Report warnings. Not thread safe! */
static unsigned int TIFFWarnings(const char *module,const char *format,
  va_list warning)
{
  char
    message[MaxTextExtent];

  errno=0;
  (void) vsnprintf(message,MaxTextExtent-2,format,warning);
  message[MaxTextExtent-2]='\0';
  (void) strlcat(message,".",MaxTextExtent);
  ThrowException2(tiff_exception,CoderWarning,message,module);
  return(True);
}

/* Write data a current offset */
static tsize_t TIFFWriteBlob(thandle_t image,tdata_t data,tsize_t size)
{
  if (((Image *) image)->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "TIFF write blob: data=%p size=%u", data, (unsigned int) size);
  return((tsize_t) WriteBlob((Image *) image,(size_t) size,data));
}

/*
  Convert TIFF data from libtiff "native" format to byte-parsable big endian
*/
#if !defined(WORDS_BIGENDIAN)
static void SwabDataToBigEndian(const uint16 bits_per_sample, tdata_t data,
                         const tsize_t size)
{
  if (bits_per_sample == 64U)
    {
      TIFFSwabArrayOfDouble((double*) data,
                            (size+sizeof(double)-1)/sizeof(double));
    }
  else if (bits_per_sample == 32U)
    {
      TIFFSwabArrayOfLong((uint32*) data,
                          (size+sizeof(uint32)-1)/sizeof(uint32));
    }
  else if (bits_per_sample == 16U)
    {
      TIFFSwabArrayOfShort((uint16*) data,
                           (size+sizeof(uint16)-1)/sizeof(uint16));
    }
}
#endif

/*
  Convert TIFF data from byte-parsable big endian to libtiff "native" format.
*/
#if !defined(WORDS_BIGENDIAN)
static void SwabDataToNativeEndian(const uint16 bits_per_sample, tdata_t data,
                            const tsize_t size)
{
  if (bits_per_sample == 64)
    {
      TIFFSwabArrayOfDouble((double*) data,
                            (size+sizeof(double)-1)/sizeof(double));
    }
  else if (bits_per_sample == 32)
    {
      TIFFSwabArrayOfLong((uint32*) data,
                          (size+sizeof(uint32)-1)/sizeof(uint32));
    }
  else if (bits_per_sample == 16)
    {
      TIFFSwabArrayOfShort((uint16*) data,
                           (size+sizeof(uint16)-1)/sizeof(uint16));
    }
}
#endif

/*
  Initialize the image colormap.
*/
static MagickPassFail InitializeImageColormap(Image *image, TIFF *tiff)
{
  uint16
    bits_per_sample,
    photometric;

  register unsigned int
    i;

  unsigned int
    max_sample_value,
    status = MagickFail;

  if (TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample) == -1)
    return status;
  if (TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric) == -1)
    return status;

  /*
    Compute colormap size
  */
  max_sample_value=MaxValueGivenBits(bits_per_sample);

  image->colors=0;
  if (MaxColormapSize > max_sample_value)
    image->colors=max_sample_value+1;
  else if (MaxColormapSize > MaxRGB)
    {
      if (photometric == PHOTOMETRIC_PALETTE)
        return status;
      else
        image->colors=MaxColormapSize;
    }
  
  if (image->colors > 0)
    {
      /*
        Allocate colormap.
      */
      if (AllocateImageColormap(image,image->colors) == MagickFail)
        return status;

      /*
        Create colormap.
      */
      switch (photometric)
        {
        case PHOTOMETRIC_MINISBLACK:
          {
            Quantum
              quantum;
        
            for (i=0; i < image->colors; i++)
              {
                quantum = (Quantum) (i*(MaxRGB/Max(image->colors-1,1)));
                image->colormap[i].red=quantum;
                image->colormap[i].green=quantum;
                image->colormap[i].blue=quantum;
                image->colormap[i].opacity=OpaqueOpacity;
              }
            status = MagickPass;
            break;
          }
        case PHOTOMETRIC_MINISWHITE:
        default:
          {
            Quantum
              quantum;
        
            for (i=0; i < image->colors; i++)
              {
                quantum = (Quantum) (MaxRGB - (i*(MaxRGB/Max(image->colors-1,1))));
                image->colormap[i].red=quantum;
                image->colormap[i].green=quantum;
                image->colormap[i].blue=quantum;
                image->colormap[i].opacity=OpaqueOpacity;
              }
            status = MagickPass;
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
            for (i=0; i < image->colors; i++)
              if ((red_colormap[i] >= 256) || (green_colormap[i] >= 256) ||
                  (blue_colormap[i] >= 256))
                {
                  range=65535L;
                  break;
                }
            for (i=0; i < image->colors; i++)
              {
                image->colormap[i].red=(Quantum)
                  (((double) MaxRGB*red_colormap[i])/range+0.5);
                image->colormap[i].green=(Quantum)
                  (((double) MaxRGB*green_colormap[i])/range+0.5);
                image->colormap[i].blue=(Quantum)
                  (((double) MaxRGB*blue_colormap[i])/range+0.5);
              }
            status = MagickPass;
            break;
          }
        }
      if (status == MagickPass)
        {
          register const PixelPacket
            *p;

          register unsigned int
            scale;

          unsigned int
            depth=1;

          /*
            Evaluate colormap depth.
          */
          p=image->colormap;
          scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
          for (i=image->colors; i > 0; i--)
            {
              if ((p->red != scale*(p->red/scale)) ||
                  (p->green != scale*(p->green/scale)) ||
                  (p->blue != scale*(p->blue/scale)))
                {
                  depth++;
                  if (depth == QuantumDepth)
                    break;
                  scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
                  continue;
                }
              p++;
            }
          if (depth < 8)
            depth=8;
          else
            depth=16;
          image->depth=depth;
        }
    }

  return status;
}

/*
  Determine the quauntum import/export method to use with
  ImportImagePixelArea() and ExportImagePixelArea() based on the
  nature of the image, the TIFF photometric, sample format, the
  desired planar configuration, and the specified plane (0 for
  contiguous planar configuration). Updates quantum_type with the
  import/export method, and quantum_samples with the number of samples
  consumed by one pixel. Returns MagickPass if the photometric is
  supported.
*/
static MagickPassFail QuantumTransferMode(const Image *image,
                                          const uint16 photometric,
                                          const uint16 sample_format,
                                          const uint16 planar_config,
                                          const unsigned int plane,
                                          QuantumType *quantum_type,
                                          int *quantum_samples)
{
  *quantum_type=UndefinedQuantum;
  *quantum_samples=0;
  if ((sample_format == SAMPLEFORMAT_UINT) ||
      (sample_format == SAMPLEFORMAT_VOID))
    {
      switch (photometric)
        {
        case PHOTOMETRIC_MINISBLACK:
        case PHOTOMETRIC_MINISWHITE:
          {
            if (image->matte)
              {
                *quantum_type=GrayAlphaQuantum;
                *quantum_samples=2;
              }
            else
              {
                *quantum_type=GrayQuantum;
                *quantum_samples=1;
              }
            break;
          }
        case PHOTOMETRIC_PALETTE:
          {
            if (image->matte)
              {
                *quantum_type=IndexAlphaQuantum;
                *quantum_samples=2;
              }
            else
              {
                *quantum_type=IndexQuantum;
                *quantum_samples=1;
              }
            break;
          }
        case PHOTOMETRIC_RGB:
          {
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    *quantum_type=RedQuantum;
                    break;
                  case 1:
                    *quantum_type=GreenQuantum;
                    break;
                  case 2:
                    *quantum_type=BlueQuantum;
                    break;
                  case 3:
                    *quantum_type=AlphaQuantum;
                    break;
                  }
                *quantum_samples=1;
              }
            else
              {
                if (image->matte)
                  {
                    *quantum_type=RGBAQuantum;
                    *quantum_samples=4;
                  }
                else
                  {
                    *quantum_type=RGBQuantum;
                    *quantum_samples=3;
                  }
              }
            break;
          }
        case PHOTOMETRIC_SEPARATED:
          {
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    *quantum_type=CyanQuantum;
                    break;
                  case 1:
                    *quantum_type=MagentaQuantum;
                    break;
                  case 2:
                    *quantum_type=YellowQuantum;
                    break;
                  case 3:
                    *quantum_type=BlackQuantum;
                    break;
                  case 4:
                    *quantum_type=AlphaQuantum;
                    break;
                  }
                *quantum_samples=1;
              }
            else
              {
                if (image->matte)
                  {
                    *quantum_type=CMYKAQuantum;
                    *quantum_samples=5;
                  }
                else
                  {
                    *quantum_type=CMYKQuantum;
                    *quantum_samples=4;
                  }
              }
            break;
          }
        }
    }
  
  return (*quantum_samples != 0 ? MagickPass : MagickFail);
}

/*
  Compact samples to only contain raster data. This may seem
  inefficient, but it allows us to easily deal with contiguous images
  which contain extra samples while optimizing performance for images
  without extra samples.
*/
static void CompactSamples( const unsigned long total_pixels,
                            const unsigned int bits_per_sample,
                            const unsigned int samples_per_pixel,
                            const unsigned int quantum_samples,
                            unsigned char *samples)
{
  if (samples_per_pixel > quantum_samples)
    {
      /*
        Compact scanline to only contain raster data.
      */

      BitStreamReadHandle
        read_stream;

      BitStreamWriteHandle
        write_stream;

      unsigned long
        pixels;

      unsigned int
        count,
        quantum_value;

      BitStreamInitializeRead(&read_stream,samples);
      BitStreamInitializeWrite(&write_stream,samples);

      for (pixels = total_pixels; pixels > 0 ; pixels--)
        {
          for (count = quantum_samples; count > 0 ; count--)
            {
              quantum_value=BitStreamMSBRead(&read_stream,bits_per_sample);
              BitStreamMSBWrite(&write_stream,bits_per_sample,quantum_value);
            }
          for (count = samples_per_pixel-quantum_samples; count > 0 ; count--)
            {
              (void) BitStreamMSBRead(&read_stream,bits_per_sample);
            }
        }
    }
}


/*
  Convert selected pixel area to associated alpha representation.
*/
static void AssociateAlphaRegion(Image *image)
{
  register PixelPacket
    *q;

  register long
    x;

  register double
    alpha,
    value;

  long
    number_pixels;

  number_pixels=(long) GetPixelCacheArea(image);
  q=GetPixels(image);

  for (x = number_pixels; x > 0; --x)
    {
      alpha=((double) MaxRGB-q->opacity)/MaxRGB;
      value=(double) q->red*alpha;
      q->red=RoundToQuantum(value);
      value=(double) q->green*alpha;
      q->green=RoundToQuantum(value);
      value=(double) q->blue*alpha;
      q->blue=RoundToQuantum(value);
      q++;
    }
}

/*
  Convert associated alpha to internal representation for selected
  pixel area.
*/
static void DisassociateAlphaRegion(Image *image)
{
  register PixelPacket
    *q;

  register long
    x;

  register double
    alpha,
    value;

  long
    number_pixels;

  number_pixels=(long) GetPixelCacheArea(image);
  q=GetPixels(image);

  for (x = number_pixels; x > 0; --x)
    {
      if (q->opacity != (Quantum) MaxRGB)
        {
          alpha=((double) MaxRGB-q->opacity)/MaxRGB;
          value=(double) q->red/alpha;
          q->red=RoundToQuantum(value);
          value=(double) q->green/alpha;
          q->green=RoundToQuantum(value);
          value=(double) q->blue/alpha;
          q->blue=RoundToQuantum(value);
        }
      q++;
    }
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

typedef enum
{
  ScanLineMethod,            /* Scanline method */
  StrippedMethod,            /* Stripped method */
  TiledMethod,               /* Tiled method */
  RGBAStrippedMethod,        /* RGBA stripped method */
  RGBATiledMethod,           /* RGBA tiled method */
  RGBAPuntMethod             /* RGBA whole-image method (last resort) */
} TIFFMethod;

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

  unsigned int
    y;

  register unsigned int
    x;

  register PixelPacket
    *q;

  register unsigned int
    i;

  TIFF
    *tiff;

  uint16
    compress_tag,
    bits_per_sample,
    extra_samples,
    fill_order,
    max_sample_value,
    min_sample_value,
    pages,
    photometric,
    planar_config,
    *sample_info,
    sample_format,
    samples_per_pixel,
    units,
    value;

  uint32
    height,
    rows_per_strip,
    width;

  TIFFMethod
    method;

  ImportPixelAreaOptions
    import_options;

  AlphaType
    alpha_type=UnspecifiedAlpha;

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
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  tiff_exception=exception;
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  if (BlobIsSeekable(image))
    tiff=TIFFClientOpen(image->filename,"rb",(thandle_t) image,TIFFReadBlob,
                        TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,
                        TIFFGetBlobSize,TIFFMapBlob,TIFFUnmapBlob);
  else
    {
      filename_is_temporary=True;
      if(!AcquireTemporaryFileName(filename))
        ThrowReaderTemporaryFileException(filename);
      (void) ImageToFile(image,filename,exception);
      /*
        Open TIFF file

        'r'          open for read
        'B'          read/write information using MSB2LSB bit order
        'M'          enable use of memory-mapped files when supported
        'C'          enable strip chopping support when reading
      */
      tiff=TIFFOpen(filename,"rBMC");
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Open temporary file %.1024s",filename);
    }
  if (tiff == (TIFF *) NULL)
    {
      if (filename_is_temporary)
        (void) LiberateTemporaryFile(filename);
      ThrowReaderException(FileOpenError,UnableToOpenFile,image);
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
              (void) LiberateTemporaryFile(filename);
            ThrowReaderException(CorruptImageError,UnableToReadSubImageData,
                                 image);
          }
      }
  do
    {
      if (image_info->verbose)
        TIFFPrintDirectory(tiff,stdout,False);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_COMPRESSION,&compress_tag);
      (void) TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width);
      (void) TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_PLANARCONFIG,&planar_config);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLEFORMAT,&sample_format);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fill_order);
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %ux%u",
                                (unsigned int) width,(unsigned int) height);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"PlanarConfiguration: %s",
                                planar_config == PLANARCONFIG_CONTIG ? "contiguous" :
                                planar_config == PLANARCONFIG_SEPARATE ? "separate" :
                                "UNKNOWN");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Samples per pixel: %u", samples_per_pixel);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Sample format: %s",
                                sample_format == SAMPLEFORMAT_UINT ? "Unsigned integer" :
                                sample_format == SAMPLEFORMAT_INT ? "Signed integer" :
                                sample_format == SAMPLEFORMAT_IEEEFP ? "IEEE floating point" :
                                sample_format == SAMPLEFORMAT_VOID ? "Untyped data" :
                                sample_format == SAMPLEFORMAT_COMPLEXINT ? "Complex signed int" :
                                sample_format == SAMPLEFORMAT_COMPLEXIEEEFP ? "Complex IEEE floating point" :
                                "UNKNOWN");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Bits per sample: %u",bits_per_sample);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Min sample value: %u",min_sample_value);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Max sample value: %u",max_sample_value);
          if (sample_format == SAMPLEFORMAT_IEEEFP)
            {
              double
                value;

              if (TIFFGetField(tiff,TIFFTAG_SMINSAMPLEVALUE,&value))
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "Special min sample value: %g", value);
              if (TIFFGetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,&value))
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "Special max sample value: %g", value);
            }
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Photometric: %s", PhotometricTagToString(photometric));
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Compression: %s", CompressionTagToString(compress_tag));
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Byte swapped: %s",TIFFIsByteSwapped(tiff) ?
                                "true" : "false");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Bit fill order: %s",
                                fill_order == FILLORDER_LSB2MSB ? "LSB2MSB" :
                                fill_order == FILLORDER_MSB2LSB ? "MSB2LSB" :
                                "unknown");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Rows per strip: %u",(unsigned int) rows_per_strip);
        }
      ImportPixelAreaOptionsInit(&import_options);
      if (photometric == PHOTOMETRIC_CIELAB)
        {
          TIFFClose(tiff);
          if (filename_is_temporary)
            (void) LiberateTemporaryFile(filename);
          ThrowReaderException(CoderError,UnableToReadCIELABImages,image);
        }
      if (photometric == PHOTOMETRIC_SEPARATED)
        image->colorspace=CMYKColorspace;
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
      {
        /*
          Retrieve embedded profiles.
        */
        uint32
          length;
        /*
          ICC ICM color profile.
        */
#if defined(TIFFTAG_ICCPROFILE)
        if (TIFFGetField(tiff,TIFFTAG_ICCPROFILE,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "ICC ICM embedded profile with length %lu bytes",
                                  (unsigned long)length);
            SetImageProfile(image,"ICM",(unsigned char *) text,(size_t) length);
          }
#endif /* defined(TIFFTAG_ICCPROFILE) */
        /*
          IPTC/Photoshop profile.
        */
#if defined(TIFFTAG_PHOTOSHOP)
        /* Photoshop profile (with embedded IPTC profile) */
        if (TIFFGetField(tiff,TIFFTAG_PHOTOSHOP,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Photoshop embedded profile with length %lu bytes",
                                  (unsigned long) length);
            (void) ReadNewsProfile(text,(long) length,image,TIFFTAG_PHOTOSHOP);
          }
#elif defined(TIFFTAG_RICHTIFFIPTC)
        /* IPTC TAG from RichTIFF specifications */
        if (TIFFGetField(tiff,TIFFTAG_RICHTIFFIPTC,&length,&text) == 1)
          {
            if (TIFFIsByteSwapped(tiff))
              TIFFSwabArrayOfLong((uint32 *) text,length);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "IPTC Newsphoto embedded profile with length %u bytes",length);
            ReadNewsProfile(text,length,image,TIFFTAG_RICHTIFFIPTC);
          }
#endif
        /*
          XML XMP profile.
        */
#if defined(TIFFTAG_XMLPACKET)
        /* %XML packet [Adobe XMP Specification, Janary 2004] */
        if (TIFFGetField(tiff,TIFFTAG_XMLPACKET,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "XMP embedded profile with length %lu bytes",
                                  (unsigned long) length);
            SetImageProfile(image,"XMP",(unsigned char *) text,(size_t) length);
          }
#endif
      }
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
      image->depth=bits_per_sample;

      /*
        Obtain information about any extra samples.
      */
      extra_samples=0;
      if (TIFFGetField(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
                       &sample_info))
        {
          int
            sample_index;

          if (extra_samples > 0)
            {
              const char *
                value;

              alpha_type=AssociatedAlpha;
              image->matte=True;

              if ((value=AccessDefinition(image_info,"tiff","alpha")))
                {
                  if (LocaleCompare(value,"unspecified") == 0)
                    alpha_type=UnspecifiedAlpha;
                  else if (LocaleCompare(value,"associated") == 0)
                    alpha_type=AssociatedAlpha;
                  else if (LocaleCompare(value,"unassociated") == 0)
                    alpha_type=UnassociatedAlpha;
                }
              else if (sample_info[0] == EXTRASAMPLE_UNSPECIFIED)
                alpha_type=UnspecifiedAlpha;
              else if (sample_info[0] == EXTRASAMPLE_UNASSALPHA)
                alpha_type=UnassociatedAlpha;
              else if (sample_info[0] == EXTRASAMPLE_ASSOCALPHA)
                alpha_type=AssociatedAlpha;
            }
          for (sample_index=0 ; sample_index < extra_samples; sample_index++)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Extra sample %u contains %s alpha",sample_index+1,
                                    ((sample_info[sample_index] == EXTRASAMPLE_ASSOCALPHA) ? "ASSOCIATED" :
                                     (sample_info[sample_index] == EXTRASAMPLE_UNASSALPHA) ? "UNASSOCIATED" :
                                     "UNSPECIFIED"));
            }
        }
      /*
        Handle RGBA images which are improperly marked.
      */
      if (extra_samples == 0)
        if ((photometric == PHOTOMETRIC_RGB) && (samples_per_pixel == 4))
          {
            extra_samples=1;
            alpha_type=AssociatedAlpha;
            image->matte=MagickTrue;
          }

      if (image->matte)
        {
          char
            alpha_string[MaxTextExtent];

          switch(alpha_type)
            {
            default:
            case UnspecifiedAlpha:
              strlcpy(alpha_string,"Unspecified",MaxTextExtent);
              break;
            case UnassociatedAlpha:
              strlcpy(alpha_string,"Unassociated",MaxTextExtent);
              break;
            case AssociatedAlpha:
              strlcpy(alpha_string,"Associated",MaxTextExtent);
              break;
            }
          (void) SetImageAttribute(image,"alpha",alpha_string);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Image has a matte channel of type: %s",
                                alpha_string);
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

      if (TIFFGetField(tiff,33432,&text) == 1) /* TIFFTAG_COPYRIGHT */
        (void) SetImageAttribute(image,"copyright",text);

      if (TIFFGetField(tiff,TIFFTAG_DATETIME,&text) == 1)
        (void) SetImageAttribute(image,"timestamp",text);

      if (TIFFGetField(tiff,TIFFTAG_DOCUMENTNAME,&text) == 1)
        (void) SetImageAttribute(image,"document",text);

      if (TIFFGetField(tiff,TIFFTAG_HOSTCOMPUTER,&text) == 1)
        (void) SetImageAttribute(image,"hostcomputer",text);

      if (TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text) == 1)
        (void) SetImageAttribute(image,"comment",text);

      if (TIFFGetField(tiff,32781,&text) == 1) /* TIFFTAG_OPIIMAGEID */
        (void) SetImageAttribute(image,"imageid",text);

      if (TIFFGetField(tiff,TIFFTAG_MAKE,&text) == 1)
        (void) SetImageAttribute(image,"make",text);

      if (TIFFGetField(tiff,TIFFTAG_MODEL,&text) == 1)
        (void) SetImageAttribute(image,"model",text);

      if (TIFFGetField(tiff,TIFFTAG_PAGENAME,&text) == 1)
        (void) SetImageAttribute(image,"label",text);

      if (TIFFGetField(tiff,TIFFTAG_SOFTWARE,&text) == 1)
        (void) SetImageAttribute(image,"software",text);

      if (TIFFGetField(tiff,33423,&text) == 1)
        (void) SetImageAttribute(image,"kodak-33423",text);

      if (TIFFGetField(tiff,36867,&text) == 1)
        (void) SetImageAttribute(image,"kodak-36867",text);

      /*
        Quit if in "ping" mode and we are outside of requested range.
      */
      if (image_info->ping && (image_info->subrange != 0))
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;

      if (photometric == PHOTOMETRIC_PALETTE)
        {
          /*
            Palette image
          */
          if (MaxColormapSize > MaxValueGivenBits(bits_per_sample))
            {
              (void) InitializeImageColormap(image,tiff);
            }
          else if (photometric == PHOTOMETRIC_PALETTE)
            {
              TIFFClose(tiff);
              if (filename_is_temporary)
                (void) LiberateTemporaryFile(filename);
              ThrowReaderException(CoderError,ColormapTooLarge,image);
            }
        }
      /*
        Determine which method to use for reading pixels.
      */
      {
        int
          quantum_samples;

        QuantumType
          quantum_type;

        if ((compress_tag == COMPRESSION_JPEG) ||
            (compress_tag == COMPRESSION_OJPEG))
          {
            /* Following hack avoids the error message "Application
               transferred too many scanlines. (JPEGLib)." caused by
               YCbCr subsampling, but it returns data in RGB rather
               than YCbCr. */
            TIFFSetField( tiff, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB );
            photometric=PHOTOMETRIC_RGB;
          }
        method=RGBAPuntMethod;
        quantum_type=UndefinedQuantum;
        quantum_samples=0;
        if (QuantumTransferMode(image,photometric,sample_format,planar_config,0,
                                &quantum_type,&quantum_samples) == MagickPass)
          {
            method=ScanLineMethod;
            if ((compress_tag == COMPRESSION_JPEG) ||
                (compress_tag == COMPRESSION_OJPEG))
              {
                if (TIFFIsTiled(tiff))
                  method=TiledMethod;
                else
                  method=StrippedMethod;
              }
            else if (TIFFIsTiled(tiff))
              method=TiledMethod;
            else if ((TIFFStripSize(tiff)) < (1024*64))
              method=StrippedMethod;
            if (photometric == PHOTOMETRIC_MINISWHITE)
              import_options.grayscale_inverted=MagickTrue;
          }
        else
          {
            if (TIFFIsTiled(tiff))
              method=RGBATiledMethod;
            else if (TIFFGetField(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip))
              method=RGBAStrippedMethod;
          }
      }

      switch (method)
        {
        case ScanLineMethod:
          {
            /*
              Read TIFF image as scanlines.
            */
            unsigned char
              *scanline;

            int
              max_sample,
              quantum_samples,
              sample;

            tsize_t
              scanline_size;

            QuantumType
              quantum_type;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using scanline %s read method with %u bits per sample",
                                    PhotometricTagToString(photometric),bits_per_sample);
            /*
              Allocate memory for one scanline.
            */
            scanline_size=TIFFScanlineSize(tiff);
            scanline=MagickAllocateMemory(unsigned char *,scanline_size);
            if (scanline == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        PLANARCONFIG_CONTIG,0,&quantum_type,
                                        &quantum_samples)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
            for (y=0; y < image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                for (sample=0; sample < max_sample; sample++)
                  {
                    /*
                      Obtain a scanline
                    */
                    if (TIFFReadScanline(tiff,(char *) scanline,(uint32) y,sample) == -1)
                      {
                        status=MagickFail;
                        break;
                      }
#if !defined(WORDS_BIGENDIAN)
                    SwabDataToBigEndian(bits_per_sample,scanline,scanline_size);
#endif
                    /*
                      Determine quantum parse method.
                    */
                    if (QuantumTransferMode(image,photometric,sample_format,
                                            planar_config,sample,&quantum_type,
                                            &quantum_samples) == MagickFail)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Compact scanline to only contain raster data.
                    */
                    if ((samples_per_pixel > quantum_samples) &&
                        (planar_config == PLANARCONFIG_CONTIG))
                      CompactSamples(image->columns, bits_per_sample,
                                     samples_per_pixel, quantum_samples, scanline);
                    /*
                      Import scanline into image.
                    */
                    if (ImportImagePixelArea(image,quantum_type,bits_per_sample,scanline,
                                             &import_options) == MagickFail)
                      {
                        status=MagickFail;
                        break;
                      }
                  }
                if (status == MagickFail)
                  break;
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);
                /*
                  Save our updates.
                */
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                      break;
              }
            MagickFreeMemory(scanline);
            break;
          }
        case StrippedMethod:
          {
            /*
              Read TIFF image using multi-row strip storage.
            */
            unsigned char
              *p,
              *strip;

            long
              pixels_per_strip,
              stride,
              rows_remaining;

            int
              max_sample,
              quantum_samples,
              sample;

            tsize_t
              strip_size,
              strip_size_max;

            tstrip_t
              strip_id;

            QuantumType
              quantum_type;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using stripped read method with %u bits per sample",
                                    bits_per_sample);
            pixels_per_strip=rows_per_strip*image->columns;
            p=0;
            strip_size=0;
            strip_id=0;
            /*
              Allocate memory for one strip.
            */
            strip_size_max=TIFFStripSize(tiff);
            strip=MagickAllocateMemory(unsigned char *,strip_size_max);
            if (strip == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        PLANARCONFIG_CONTIG,0,&quantum_type,
                                        &quantum_samples) == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Compute per-row stride.
            */
            stride=TIFFVStripSize(tiff,1);
            /*
              Process each plane
            */
            for (sample=0; sample < max_sample; sample++)
              {
                rows_remaining=0;
                /*
                  Determine quantum parse method.
                */
                if (QuantumTransferMode(image,photometric,sample_format,
                                        planar_config,sample,&quantum_type,
                                        &quantum_samples) == MagickFail)
                  {
                    status=MagickFail;
                    break;
                  }
                for (y=0; y < image->rows; y++)
                  {
                    /*
                      Access Magick pixels.
                    */
                    if (sample == 0)
                      q=SetImagePixels(image,0,y,image->columns,1);
                    else
                      q=GetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      {
                        status=MagickFail;
                        break;
                      }
                    if (rows_remaining == 0)
                      {
                        /*
                          Obtain a strip
                        */
                        if (((strip_size=TIFFReadEncodedStrip(tiff,strip_id,strip,
                                                              strip_size_max)) == -1))
                          {
                            status=MagickFail;
                            break;
                          }
#if !defined(WORDS_BIGENDIAN)
                        SwabDataToBigEndian(bits_per_sample,strip,strip_size);
#endif
                        rows_remaining=rows_per_strip;
                        if (y+rows_per_strip > image->rows)
                          rows_remaining=(rows_per_strip-(y+rows_per_strip-image->rows));
                        p=strip;
                        strip_id++;
                      }
                    /*
                      Compact strip row to only contain raster data.
                    */
                    if ((samples_per_pixel > quantum_samples) &&
                        (planar_config == PLANARCONFIG_CONTIG))
                      CompactSamples(image->columns, bits_per_sample,
                                     samples_per_pixel, quantum_samples, p);
                    /*
                      Import strip row into image.
                    */
                    if (ImportImagePixelArea(image,quantum_type,bits_per_sample,p,
                                             &import_options) == MagickFail)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Disassociate alpha from pixels if necessary.
                    */
                    if ((image->matte) && (alpha_type == AssociatedAlpha)
                        && (sample == (max_sample-1)))
                      DisassociateAlphaRegion(image);
                    /*
                      Save our updates.
                    */
                    if (!SyncImagePixels(image))
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Advance to next row
                    */
                    p += stride;
                    rows_remaining--;

                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                          {
                            status=MagickFail;
                            break;
                          }
                  }
                if (status == MagickFail)
                  break;
              }
            MagickFreeMemory(strip);
            break;
          }
        case TiledMethod:
          {
            /*
              Read TIFF using tiled storage.
            */
            unsigned char
              *tile;

            uint32
              tile_columns,
              tile_rows;

            tsize_t
              stride,
              tile_size,
              tile_size_max;

            int
              max_sample,
              quantum_samples,
              sample;

            QuantumType
              quantum_type;

            unsigned long
              tile_total_pixels;
        
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using tiled %s read method with %u bits per sample",
                                    PhotometricTagToString(photometric), bits_per_sample);
            /*
              Obtain tile geometry
            */
            if(!TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&tile_columns) ||
               !TIFFGetField(tiff,TIFFTAG_TILELENGTH,&tile_rows))
              {
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(CoderError,ImageIsNotTiled,image);
              }
            /*
              Obtain the maximum number of bytes required to contain a tile.
            */
            tile_size_max=TIFFTileSize(tiff);
            /*
              Compute the total number of pixels in one tile
            */
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
                                      (unsigned int)tile_rows,
                                      tile_total_pixels);
              }
            /*
              Allocate tile buffer
            */
            tile=MagickAllocateMemory(unsigned char *,(size_t) tile_size_max);
            if (tile == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        PLANARCONFIG_CONTIG,0,&quantum_type,
                                        &quantum_samples) == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Compute per-row stride.
            */
            stride=TIFFTileRowSize(tiff);

            for (y=0; y < image->rows; y+=tile_rows)
              {
                for (x=0; x < image->columns; x+=tile_columns)
                  {
                    long
                      tile_set_columns,
                      tile_set_rows;

                    /*
                      Compute image region corresponding to tile.
                    */
                    if (x+tile_columns > image->columns)
                      tile_set_columns=(tile_columns-(x+tile_columns-image->columns));
                    else
                      tile_set_columns=tile_columns;
                    if (y+tile_rows > image->rows)
                      tile_set_rows=(tile_rows-(y+tile_rows-image->rows));
                    else
                      tile_set_rows=tile_rows;
                    /*
                      Process each plane.
                    */
                    for (sample=0; sample < max_sample; sample++)
                      {
                        unsigned char
                          *p;

                        register long
                          yy;

                        /*
                          Determine quantum parse method.
                        */
                        if (QuantumTransferMode(image,photometric,sample_format,
                                                planar_config,sample,
                                                &quantum_type,&quantum_samples)
                            == MagickFail)
                          {
                            status=MagickFail;
                            break;
                          }
                        /*
                          Read a tile.
                        */
                        if ((tile_size=TIFFReadTile(tiff,tile,x,y,0,sample)) == -1)
                          {
                            status=MagickFail;
                            break;
                          }
#if !defined(WORDS_BIGENDIAN)
                        SwabDataToBigEndian(bits_per_sample,tile,tile_size);
#endif
                        p=tile;
                        for (yy=y; yy < (long) y+tile_set_rows; yy++)
                          {
                            /*
                              Obtain pixel region corresponding to tile row.
                            */
                            if (sample == 0)
                              q=SetImagePixels(image,x,yy,tile_set_columns,1);
                            else
                              q=GetImagePixels(image,x,yy,tile_set_columns,1);
                            if (q == (PixelPacket *) NULL)
                              {
                                status=MagickFail;
                                break;
                              }
                            /*
                              Compact tile row to only contain raster data.
                            */
                            if ((samples_per_pixel > quantum_samples) &&
                                (planar_config == PLANARCONFIG_CONTIG))
                              CompactSamples(tile_set_columns, bits_per_sample,
                                             samples_per_pixel, quantum_samples, p);
                            /*
                              Import tile row
                            */
                            if (ImportImagePixelArea(image,quantum_type,
                                                     bits_per_sample,p,
                                                     &import_options)
                                == MagickFail)
                              {
                                status=MagickFail;
                                break;
                              }
                            /*
                              Disassociate alpha from pixels if necessary.
                            */
                            if ((image->matte) && (alpha_type == AssociatedAlpha)
                                && (sample == (max_sample-1)))
                              DisassociateAlphaRegion(image);
                            /*
                              Save our updates.
                            */
                            if (!SyncImagePixels(image))
                              {
                                status=MagickFail;
                                break;
                              }
                            p += stride;
                          }
                        if (status == MagickFail)
                          break;
                      }
                    if (status == MagickFail)
                      break;
                
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                          {
                            status=MagickFail;
                            break;
                          }
                  }
                if (status == MagickFail)
                  break;
              }

            MagickFreeMemory(tile);
            break;
          }
        case RGBAStrippedMethod:
          {
            unsigned long
              number_pixels;

            uint32
              *strip_pixels;

            register uint32
              *p;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using RGB stripped read method with %u bits per sample",
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
                                     image);
              }
            strip_pixels=MagickAllocateMemory(uint32 *,(size_t) (number_pixels*sizeof(uint32)));
            if (strip_pixels == (uint32 *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Convert image to DirectClass pixel packets.
            */
            i=0;
            p=0;
            for (y=0; y < image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                if (0 == i)
                  {
                    if (!TIFFReadRGBAStrip(tiff,y,strip_pixels))
                      {
                        status=MagickFail;
                        break;
                      }
                    i=(long) Min(rows_per_strip,image->rows-y);
                  }
                i--;
                p=strip_pixels+image->columns*i;
                for (x=0; x < image->columns; x++)
                  {
                    q->red=ScaleCharToQuantum(TIFFGetR(*p));
                    q->green=ScaleCharToQuantum(TIFFGetG(*p));
                    q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                    if (image->matte)
                      q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                    p++;
                    q++;
                  }
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);                
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                      {
                        status=MagickFail;
                        break;
                      }
              }
            MagickFreeMemory(strip_pixels);
            break;
          }
        case RGBATiledMethod:
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
                                    "Using RGB tiled read method with %u bits per sample",
                                    bits_per_sample);
            /*
              Obtain tile geometry
            */
            if (!TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&tile_columns) ||
                !TIFFGetField(tiff,TIFFTAG_TILELENGTH,&tile_rows))
              {
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(CoderError,ImageIsNotTiled,image);
              }
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Reading TIFF tiles ...");
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
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
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            for (y=0; y < image->rows; y+=tile_rows)
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
                /*
                  Obtain a row of pixels
                */
                strip=SetImagePixels(image,0,y,image->columns,tile_rows_remaining);
                if (strip == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
                for (x=0; x < image->columns; x+=tile_columns)
                  {
                    register unsigned int
                      tile_column,
                      tile_row;

                    /*
                      Obtain one tile.  Origin is bottom left of tile.
                    */
                    if (!TIFFReadRGBATile(tiff,x,y,tile_pixels))
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Compute remaining tile columns
                    */
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
                          for (tile_column=tile_columns_remaining; tile_column >0;
                               tile_column--)
                            {
                              q->red=ScaleCharToQuantum(TIFFGetR(*p));
                              q->green=ScaleCharToQuantum(TIFFGetG(*p));
                              q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                              q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                              q++;
                              p++;
                            }
                        else
                          for (tile_column=tile_columns_remaining; tile_column >0;
                               tile_column--)
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
                    if (status == MagickFail)
                      break;
                  }
                if (status == MagickFail)
                  break;
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);                
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                      {
                        status=MagickFail;
                        break;
                      }
              }
            MagickFreeMemory(tile_pixels);
            break;
          }
        case RGBAPuntMethod:
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
                                    "Using RGB punt read method with %u bits per sample",
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
                  (void) LiberateTemporaryFile(filename);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            if (!TIFFReadRGBAImage(tiff,(uint32) image->columns,
                                   (uint32) image->rows,
                                   pixels+image->columns*sizeof(uint32),0))
              {
                MagickFreeMemory(pixels);
                TIFFClose(tiff);
                if (filename_is_temporary)
                  (void) LiberateTemporaryFile(filename);
                return ((Image *) NULL);
              }
            /*
              Convert image to DirectClass pixel packets.
            */
            p=pixels+number_pixels+image->columns*sizeof(uint32)-1;
            for (y=0; y < image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    status=MagickFail;
                    break;
                  }
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
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);
                if (!SyncImagePixels(image))
                  {
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                      {
                        status=MagickFail;
                        break;
                      }
                if (status == MagickFail)
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
          if (status == MagickFail)
            break;
        }
    } while (status == MagickPass);
  TIFFClose(tiff);
  if (filename_is_temporary)
    (void) LiberateTemporaryFile(filename);
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
%      MagickPassFail WritePTIFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WritePTIFImage return MagickPass if the image is written.
%      MagickFail is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static MagickPassFail WritePTIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pyramid_image;

  ImageInfo
    *clone_info;

  FilterTypes
    filter;

  unsigned int
    status;

  /*
    Create pyramid-encoded TIFF image.
  */
  filter=TriangleFilter;
  if (image->is_monochrome)
    filter=PointFilter;
  pyramid_image=CloneImage(image,0,0,True,&image->exception);
  if (pyramid_image == (Image *) NULL)
    ThrowWriterException2(FileOpenError,image->exception.reason,image);
  do
  {
    pyramid_image->next=ResizeImage(image,pyramid_image->columns/2,
      pyramid_image->rows/2,TriangleFilter,1.0,&image->exception);
    if (pyramid_image->next == (Image *) NULL)
      ThrowWriterException2(FileOpenError,image->exception.reason,image);
    if ((!image->is_monochrome) && (image->storage_class == PseudoClass))
      (void) MapImage(pyramid_image->next,image,False);
    pyramid_image->next->x_resolution=pyramid_image->x_resolution/2;
    pyramid_image->next->y_resolution=pyramid_image->y_resolution/2;
    pyramid_image->next->previous=pyramid_image;
    pyramid_image=pyramid_image->next;
  } while ((pyramid_image->columns > 64) && (pyramid_image->rows > 64));
  while (pyramid_image->previous != (Image *) NULL)
    pyramid_image=pyramid_image->previous;
  /*
    Write pyramid-encoded TIFF image.
  */
  clone_info=CloneImageInfo(image_info);
  /* (void) AddDefinitions(clone_info,"tiff:tile",&image->exception); */
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

static void WriteNewsProfile(TIFF *tiff,
                             int profile_tag,
                             const unsigned char *profile_data,
                             const size_t profile_length)
{
  unsigned char
    *profile=0;

  uint32
    length;

  assert(tiff != (TIFF *) NULL);
  assert(profile_tag != 0);
  assert(profile_data != (const unsigned char *) NULL);

  length = (uint32) profile_length;
  if (length == 0)
    return;

  if (profile_tag == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle TIFFTAG_RICHTIFFIPTC tag.
      */
      length += (4-(length & 0x03)); /* Round up for long word alignment */
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,profile_data,profile_length);

      if (TIFFIsByteSwapped(tiff))
        TIFFSwabArrayOfLong((uint32 *) profile,length/4);

      /* Tag is type TIFF_LONG so byte length is divided by four */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "TIFFSetField(tiff=0x%p,tag=%d,length=%lu,data=0x%p)",
                            tiff,profile_tag,(unsigned long) length/4,profile);
      (void) TIFFSetField(tiff,profile_tag,(uint32) length/4,(void *) profile);
    }
  else if (profile_tag == TIFFTAG_PHOTOSHOP)
    {
      /*
        Handle TIFFTAG_PHOTOSHOP tag.
      */
      length += (length & 0x01); /* Round up for Photoshop */
#if defined(GET_ONLY_IPTC_DATA)
      length += 12;              /* Space for 8BIM header */
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,"8BIM\04\04\0\0",8);
      profile[8]=(length >> 24) & 0xff;
      profile[9]=(length >> 16) & 0xff;
      profile[10]=(length >> 8) & 0xff;
      profile[11]=length & 0xff;
      (void) memcpy(profile+12,profile_data,profile_length);
#else
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,profile_data,profile_length);
#endif
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "TIFFSetField(tiff=0x%p,tag=%d,length=%lu,data=0x%p)",
                            tiff,profile_tag,(unsigned long) length,profile);
      (void) TIFFSetField(tiff,profile_tag,(uint32) length,(void *) profile);
    }

  MagickFreeMemory(profile);
}

#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  ((8*1024)/TIFFScanlineSize(tiff))
#endif
static MagickPassFail WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent];

  const ImageAttribute
    *attribute;

  unsigned int
    x,
    y;

  register unsigned int
    i;

  TIFF
    *tiff;

  uint16
    bits_per_sample,
    compress_tag,
    fill_order,
    photometric,
    planar_config,
    sample_format,
    samples_per_pixel;

  AlphaType
    alpha_type;

  TIFFMethod
    method;

  ExportPixelAreaOptions
    export_options;

  MagickBool
    filename_is_temporary=MagickFalse,
    logging;

  MagickPassFail
    status;

  unsigned long
    depth,
    scene,
    strip_size;

  /*
    Open TIFF file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=IsEventLogging();
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  tiff_exception=(&image->exception);
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) TIFFWarnings);
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  if (!(GetBlobFileHandle(image)))
    {
      /*
        If output is not to a stdio file descriptor, then use a
        temporary file for the output so that it is.
      */
      filename_is_temporary=MagickTrue;
      if(!AcquireTemporaryFileName(filename))
        ThrowWriterTemporaryFileException(filename);
    }
  else
    CloseBlob(image);
  /*
    Open TIFF file

    'w'          open for write
  */
  tiff=TIFFOpen(filename,"w");
  if (tiff == (TIFF *) NULL)
    {
      if (filename_is_temporary)
        (void) LiberateTemporaryFile(filename);
      return(MagickFail);
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
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLEFORMAT,&sample_format);
      if (LocaleCompare(image_info->magick,"PTIF") == 0)
        if (image->previous != (Image *) NULL)
          (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
      (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
      (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);

      ExportPixelAreaOptionsInit(&export_options);
      depth=image->depth;
      bits_per_sample=8;
      method=ScanLineMethod;
      if ((AccessDefinition(image_info,"tiff","tile")) ||
          (AccessDefinition(image_info,"tiff","tile-geometry")) ||
          (AccessDefinition(image_info,"tiff","tile-width")) ||
          (AccessDefinition(image_info,"tiff","tile-height")))
        method=TiledMethod;
      compress_tag=COMPRESSION_NONE;
      switch (image->compression)
        {
#ifdef CCITT_SUPPORT
        case FaxCompression:
          {
            (void) SetImageType(image,BilevelType);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Set image type to BilevelType");
            compress_tag=COMPRESSION_CCITTFAX3;
            break;
          }
        case Group4Compression:
          {
            (void) SetImageType(image,BilevelType);
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
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Set image type to DirectClass");
            break;
          }
#endif
        case LZWCompression:
          {
            compress_tag=COMPRESSION_LZW;
            break;
          }
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
#ifdef LZW_SUPPORT
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

      if (image->compression == JPEGCompression)
        {
          /*
            JPEG compression can only use size specified by BITS_IN_JSAMPLE.
            FIXME
          */
          depth=8;
          bits_per_sample=8;
        }
      else
        {
          /*
            Support writing RGB bits per sample of 8, 16, & 32 by default.
          */
          for (bits_per_sample=8; bits_per_sample < depth; )
            bits_per_sample*=2;
        }

      if (((image_info->colorspace == UndefinedColorspace) &&
           (image->colorspace == CMYKColorspace)) ||
          (image_info->colorspace == CMYKColorspace))
        {
          /*
            CMYK Image
          */
          (void) TransformColorspace(image,CMYKColorspace);
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
          (void) TransformColorspace(image,RGBColorspace);
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

      alpha_type=UnspecifiedAlpha;
      if (image->matte)
        {
          /*
            Image has a matte channel.  Mark it correctly.
          */
          uint16
            extra_samples,
            sample_info[1];

          const char *
            value;

          const ImageAttribute
            *attribute;

          if ((value=AccessDefinition(image_info,"tiff","alpha")))
            {
              if (LocaleCompare(value,"unspecified") == 0)
                alpha_type=UnspecifiedAlpha;
              else if (LocaleCompare(value,"associated") == 0)
                alpha_type=AssociatedAlpha;
              else if (LocaleCompare(value,"unassociated") == 0)
                alpha_type=UnassociatedAlpha;
            }
          else if ((attribute=GetImageAttribute(image,"alpha")))
            {
              if (LocaleCompare(attribute->value,"unspecified") == 0)
                alpha_type=UnspecifiedAlpha;
              else if (LocaleCompare(attribute->value,"associated") == 0)
                alpha_type=AssociatedAlpha;
              else if (LocaleCompare(attribute->value,"unassociated") == 0)
                alpha_type=UnassociatedAlpha;
            }
          export_options.alpha_type=alpha_type;

          samples_per_pixel += 1;
          extra_samples=1;
          sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
          switch (alpha_type)
            {
            case UnspecifiedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_UNSPECIFIED;
                break;
              }
            case AssociatedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
                break;
              }
            case UnassociatedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_UNASSALPHA;
                break;
              }
            }
            
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
            /* Clamp maximum bits per sample to 32 bits */
            bits_per_sample=Min(new_value,sizeof(unsigned int)*8);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "User override (bits-per-sample): %u bits per sample (was %u)",
                                    (unsigned int) bits_per_sample, old_value);
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

      /*
        Determine bit ordering in a byte.
      */
      switch (image_info->endian)
        {
        case LSBEndian:
          fill_order=FILLORDER_LSB2MSB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using LSB2MSB bit fillorder");
          break;
        case MSBEndian:
          fill_order=FILLORDER_MSB2LSB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using MSB2LSB bit fillorder");
          break;
        default:
        case UndefinedEndian:
          {
            (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fill_order);
            break;
          }
        }
      (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,fill_order);

      (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
      /*
        Determine planar configuration.
      */
      planar_config=PLANARCONFIG_CONTIG;
      if ((photometric == PHOTOMETRIC_RGB) ||
          (photometric == PHOTOMETRIC_SEPARATED))
        if ((image_info->interlace == PlaneInterlace) ||
            (image_info->interlace == PartitionInterlace))
          planar_config=PLANARCONFIG_SEPARATE;
      (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,planar_config);

      strip_size=Max(TIFFDefaultStripSize(tiff,-1),1);

      switch (compress_tag)
        {
        case COMPRESSION_JPEG:
          {
            /*
              RowsPerStrip must be multiple of 8 for JPEG.
            */
            (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,
                                strip_size+(8-(strip_size % 8)));
            break;
          }
        case COMPRESSION_ADOBE_DEFLATE:
          {
            (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);

            if (((photometric == PHOTOMETRIC_RGB) ||
                 (photometric == PHOTOMETRIC_MINISBLACK)) &&
                ((bits_per_sample == 8) || (bits_per_sample == 16)))
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
            if (((photometric == PHOTOMETRIC_RGB) ||
                 (photometric == PHOTOMETRIC_MINISBLACK)) &&
                ((bits_per_sample == 8) || (bits_per_sample == 16)))
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
      {
        /*
          Copy any embedded profiles to TIFF.
        */
        size_t
          profile_length=0;

        const unsigned char
          *profile_data=0;

#if defined(TIFFTAG_XMLPACKET)
        /*
          XML XMP profile.
        */
        if ((profile_data=GetImageProfile(image,"XMP",&profile_length)) != 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "XMP embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);
            (void) TIFFSetField(tiff,TIFFTAG_XMLPACKET,(uint32) profile_length,
                                profile_data);
          }
#endif /* defined(TIFFTAG_XMLPACKET) */
      
#if defined(TIFFTAG_ICCPROFILE)
        /*
          ICC color profile.
        */
        if ((profile_data=GetImageProfile(image,"ICM",&profile_length)) != 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "ICC ICM embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);
            (void) TIFFSetField(tiff,TIFFTAG_ICCPROFILE,(uint32)profile_length,
                                profile_data);
          }
#endif /* defined(ICC_SUPPORT) */
      
        /*
          IPTC NewsPhoto profile.
        */
        if ((profile_data=GetImageProfile(image,"IPTC",&profile_length)) != 0)
          {
            int
              profile_tag=0;
#if defined(TIFFTAG_PHOTOSHOP)
            /* Photoshop profile (with embedded IPTC profile). */
            profile_tag=TIFFTAG_PHOTOSHOP;
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Photoshop embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);
#else
            /* IPTC TAG from RichTIFF specifications */
            profile_tag=TIFFTAG_RICHTIFFIPTC;
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "IPTC Newsphoto embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);

#endif /* defined(PHOTOSHOP_SUPPORT) */
            WriteNewsProfile(tiff,profile_tag,profile_data,profile_length);
          }
      }
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

      attribute=GetImageAttribute(image,"copyright");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,33432,attribute->value);  /* TIFFTAG_COPYRIGHT */

      attribute=GetImageAttribute(image,"timestamp");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_DATETIME,attribute->value);

      (void) TIFFSetField(tiff,TIFFTAG_DOCUMENTNAME,image->filename);

      attribute=GetImageAttribute(image,"hostcomputer");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_HOSTCOMPUTER,attribute->value);

      attribute=GetImageAttribute(image,"comment");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,attribute->value);
      
      attribute=GetImageAttribute(image,"imageid");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,32781,attribute->value); /* TIFFTAG_OPIIMAGEID */

      attribute=GetImageAttribute(image,"make");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_MAKE,attribute->value);
 
      attribute=GetImageAttribute(image,"model");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_MODEL,attribute->value);

      attribute=GetImageAttribute(image,"label");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_PAGENAME,attribute->value);

      (void) TIFFSetField(tiff,TIFFTAG_SOFTWARE,
                          GetMagickVersion((unsigned long *) NULL));

      attribute=GetImageAttribute(image,"kodak-33423");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,33423,attribute->value);

      attribute=GetImageAttribute(image,"kodak-36867");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,36867,attribute->value);

      if (photometric == PHOTOMETRIC_PALETTE)
        {
          uint16
            *blue,
            *green,
            *red;

          /*
            Initialize TIFF colormap.
          */
          blue=MagickAllocateMemory(unsigned short *,
                                    65536L*sizeof(unsigned short));
          green=MagickAllocateMemory(unsigned short *,
                                     65536L*sizeof(unsigned short));
          red=MagickAllocateMemory(unsigned short *,
                                   65536L*sizeof(unsigned short));
          if ((blue == (unsigned short *) NULL) ||
              (green == (unsigned short *) NULL) ||
              (red == (unsigned short *) NULL))
            ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          (void) memset(red,0,65536L*sizeof(unsigned short));
          (void) memset(green,0,65536L*sizeof(unsigned short));
          (void) memset(blue,0,65536L*sizeof(unsigned short));
          for (i=0; i < image->colors; i++)
            {
              red[i]=ScaleQuantumToShort(image->colormap[i].red);
              green[i]=ScaleQuantumToShort(image->colormap[i].green);
              blue[i]=ScaleQuantumToShort(image->colormap[i].blue);
            }
          (void) TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
          MagickFreeMemory(red);
          MagickFreeMemory(green);
          MagickFreeMemory(blue);
        }

      if (photometric == PHOTOMETRIC_MINISWHITE)
        export_options.grayscale_inverted=MagickTrue;

      switch (method)
        {
        default:
        case ScanLineMethod:
          {
            /*
              Write TIFF image as scanlines
            */
            unsigned char
              *scanline;

            const PixelPacket
              *p;

            int
              max_sample,
              quantum_samples,
              sample;
            
            tsize_t
              scanline_size;
            
            QuantumType
              quantum_type;
            
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using scanline %s write method with %u bits per sample",
                                    PhotometricTagToString(photometric),bits_per_sample);
            /*
              Allocate memory for one scanline.
            */
            scanline_size=TIFFScanlineSize(tiff);
            scanline=MagickAllocateMemory(unsigned char *,scanline_size);
            if (scanline == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        PLANARCONFIG_CONTIG,0,&quantum_type,
                                        &quantum_samples) == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              For each plane
            */
            for (sample=0; sample < max_sample; sample++)
              {
                /*
                  Determine quantum parse method.
                */
                if (QuantumTransferMode(image,photometric,sample_format,
                                        planar_config,sample,&quantum_type,
                                        &quantum_samples) == MagickFail)
                  {
                    status=MagickFail;
                    break;
                  }
                for (y=0; y < image->rows; y++)
                  {
                    if ((image->matte) && (alpha_type == AssociatedAlpha))
                      p=GetImagePixels(image,0,y,image->columns,1);
                    else
                      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
                    if (p == (const PixelPacket *) NULL)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Convert to associated alpha if necessary.
                    */
                    if ((sample == 0) && (image->matte) &&
                        (alpha_type == AssociatedAlpha))
                      AssociateAlphaRegion(image);
                    /*
                      Export pixels to scanline.
                    */
                    if (ExportImagePixelArea(image,quantum_type,bits_per_sample,
                                             scanline,&export_options)
                        == MagickFail)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Write scanline.
                    */
#if !defined(WORDS_BIGENDIAN)
                    SwabDataToNativeEndian(bits_per_sample,scanline,scanline_size);
#endif
                    if (TIFFWriteScanline(tiff, scanline,y,sample) < 0)
                      {
                        status=MagickFail;
                        break;
                      }

                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        if((status &= MagickMonitor(SaveImageText,y,image->rows,
                                                    &image->exception))
                           == MagickFail)
                          break;
                  }
                if (status == MagickFail)
                  break;
              }
            MagickFreeMemory(scanline);
            break;
          }
        case TiledMethod:
          {
            /*
              Write TIFF image as tiles
            */
            unsigned char
              *tile;

            uint32
              tile_columns,
              tile_rows;

            tsize_t
              stride,
              tile_size,
              tile_size_max;

            int
              max_sample,
              quantum_samples,
              sample;

            QuantumType
              quantum_type;

            unsigned long
              tile_total_pixels;
        
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using tiled %s write method with %u bits per sample",
                                    PhotometricTagToString(photometric),bits_per_sample);

            /*
              Determine tile size
            */
            {
              const char *
                value;

              tile_columns=0;
              tile_rows=0;
              
              /*
                Enable tiled output (with default size, or specified separately)
                -define tiff:tile
                
                Use an exact tile size in rows & columns
                -define tiff:tile-geometry=128x128
                
                Use a specific tile width (pixels)
                -define tiff:tile-width=128
                
                Use a specific tile height (pixels)
                -define tiff:tile-height=128
              */
              if ((value=AccessDefinition(image_info,"tiff","tile-geometry")))
                {
                  double
                    width,
                    height;

                  if (GetMagickDimension(value,&width,&height) == 2)
                    {
                      tile_rows=(uint32) height;
                      tile_columns=(uint32) width;
                    }
                }
              if ((value=AccessDefinition(image_info,"tiff","tile-width")))
                {
                  tile_columns=atol(value);
                }
              if ((value=AccessDefinition(image_info,"tiff","tile-height")))
                {
                  tile_rows=atol(value);
                }
              
              TIFFDefaultTileSize(tiff,&tile_columns,&tile_rows);
            }

            if (!TIFFSetField(tiff,TIFFTAG_TILEWIDTH,tile_columns))
              status=MagickFail;
            if (!TIFFSetField(tiff,TIFFTAG_TILELENGTH,tile_rows))
              status=MagickFail;
            /*
              Obtain the maximum number of bytes required to contain a tile.
            */
            tile_size_max=TIFFTileSize(tiff);
            /*
              Compute the total number of pixels in one tile
            */
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
                                      (unsigned int)tile_rows,
                                      tile_total_pixels);
              }
            /*
              Allocate tile buffer
            */
            tile=MagickAllocateMemory(unsigned char *,(size_t) tile_size_max);
            if (tile == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);            
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        PLANARCONFIG_CONTIG,0,&quantum_type,
                                        &quantum_samples) == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Obtain per-row stride.
            */
            stride=TIFFTileRowSize(tiff);

            for (y=0; y < image->rows; y+=tile_rows)
              {
                for (x=0; x < image->columns; x+=tile_columns)
                  {
                    const PixelPacket
                      *p;

                    long
                      tile_set_columns,
                      tile_set_rows;

                    /*
                      Compute image region corresponding to tile.
                    */
                    if (x+tile_columns > image->columns)
                      tile_set_columns=(tile_columns-(x+tile_columns-image->columns));
                    else
                      tile_set_columns=tile_columns;
                    if (y+tile_rows > image->rows)
                      tile_set_rows=(tile_rows-(y+tile_rows-image->rows));
                    else
                      tile_set_rows=tile_rows;
                    /*
                      Process each plane.
                    */
                    for (sample=0; sample < max_sample; sample++)
                      {
                        unsigned char
                          *q;

                        register long
                          yy;

                        /*
                          Determine quantum parse method.
                        */
                        if (QuantumTransferMode(image,photometric,sample_format,
                                                planar_config,sample,&quantum_type,
                                                &quantum_samples) == MagickFail)
                          {
                            status=MagickFail;
                            break;
                          }
                        q=tile;
                        for (yy=y; yy < (long) y+tile_set_rows; yy++)
                          {
                            /*
                              Obtain pixel region corresponding to tile row.
                            */
                            if ((image->matte) && (alpha_type == AssociatedAlpha))
                              p=GetImagePixels(image,x,yy,tile_set_columns,1);
                            else
                              p=AcquireImagePixels(image,x,yy,tile_set_columns,1,&image->exception);
                            if (p == (const PixelPacket *) NULL)
                              {
                                status=MagickFail;
                                break;
                              }
                            /*
                              Convert to associated alpha if necessary.
                            */
                            if ((sample == 0) && (image->matte) &&
                                (alpha_type == AssociatedAlpha))
                              AssociateAlphaRegion(image);
                            /*
                              Export tile row
                            */
                            if (ExportImagePixelArea(image,quantum_type,
                                                     bits_per_sample,q,
                                                     &export_options)
                                == MagickFail)
                              {
                                status=MagickFail;
                                break;
                              }
                            q += stride;
                          } /* for yy */
                        if (status == MagickFail)
                          break;
                        /*
                          Write tile.
                        */
#if !defined(WORDS_BIGENDIAN)
                        SwabDataToNativeEndian(bits_per_sample,tile,tile_size_max);
#endif
                        if ((tile_size=TIFFWriteTile(tiff,tile,x,y,0,sample)) < 0)
                          {
                            status=MagickFail;
                          }
                      } /* for sample */
                    if (status == MagickFail)
                      break;
                  } /* for x */
                if (status == MagickFail)
                  break;
              } /* for y */
            MagickFreeMemory(tile);
            break;
          }
        }
      if (image_info->verbose == MagickTrue)
        TIFFPrintDirectory(tiff,stdout,MagickFalse);
      if(!TIFFWriteDirectory(tiff))
        status=MagickFail;
      if (image->next == (Image *) NULL)
        break;
      image=SyncNextImageInList(image);
      if ((status &= MagickMonitor(SaveImagesText,scene++,
                                   GetImageListLength(image),
                                   &image->exception)) == MagickFail)
        break;
    } while (image_info->adjoin);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  TIFFClose(tiff);

  if (status == MagickFail)
    {
      /*
        Handle Failure
      */
      while (image->previous != (Image *) NULL)
        image=image->previous;
      if (filename_is_temporary)
        LiberateTemporaryFile(filename);
      else
        unlink(filename);
      CloseBlob(image);
      return (MagickFail);
    }
  
  if (filename_is_temporary)
    {
      /*
        Copy temporary file to image blob.
      */
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
      /* st_size has type off_t */
      if ((fstat(file,&attributes) < 0) ||
          (attributes.st_size != (off_t) ((size_t) attributes.st_size)) ||
          (attributes.st_size <= (off_t) ((size_t) 0)))
        {
          (void) close(file);
          ThrowWriterException(FileOpenError,UnableToOpenFile,image);
        }
      length=(size_t) attributes.st_size;
      map=MapBlob(file,ReadMode,0,length);
      if (map != (void *) NULL)
        {
          (void) WriteBlob(image,length,map);
          (void) UnmapBlob(map,length);
        }
      else
        {
          size_t
            count;

          ssize_t
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
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                   image);
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
      (void) LiberateTemporaryFile(filename);
      CloseBlob(image);
    }
  return(MagickPass);
}
#endif
