/*
% Copyright (C) 2003-2012 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%         IIIII  M   M       CCCC    OOO   M   M  PPPP    AAA   TTTTT         %
%           I    MM MM      C       O   O  MM MM  P   P  A   A    T           %
%           I    M M M      C       O   O  M M M  PPPP   AAAAA    T           %
%           I    M   M      C       O   O  M   M  P      A   A    T           %
%         IIIII  M   M       CCCC    OOO   M   M  P      A   A    T           %
%                                                                             %
%                                                                             %
%                     ImageMagick Compatability Methods                       %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 2003                                   %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
% THIS MODULE IS SCHEDULED FOR DELETION. IT CONTAINS DEAD CODE WHICH REMAINS
% ONLY TO SUPPORT THE PREVIOUSLY EXISTING ABI.
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/constitute.h"
#include "magick/error.h"
#include "magick/utility.h"
#include "wand/wand_api.h"
#include "wand/wand_private.h"

/*
  Legacy definitions to compile code below with same exported symbols as
  before.
*/
#define AcquireMagickMemory(memory) malloc(memory)
#define AcquireUniqueFileResource(char_pointer) AcquireTemporaryFileName(char_pointer)
  /* #define CopyMagickString GMPrivateCopyMagickString */
  /* #define FormatMagickString GMPrivateFormatMagickString */
#define GetAffineMatrix(affine_matrix) IdentityAffine(affine_matrix)
#define InheritException(copy,original) CopyException(copy,original)
#define RelinquishUniqueFileResource LiberateTemporaryFile
#define ThrowMagickException ThrowException

#define PsiNegative YNegative
#define PsiValue XValue
#define RhoValue WidthValue
#define SigmaValue HeightValue
#define XiNegative XNegative
#define XiValue YValue

#define ConcatenateMagickString GMPrivateConcatenateMagickString
#define GeometryInfo GMPrivateGeometryInfo
#define ImportImagePixels GMPrivateImportImagePixels
#define ParseAbsoluteGeometry GMPrivateParseAbsoluteGeometry
#define ParseGeometry GMPrivateParseGeometry
#define RelinquishMagickMemory GMPrivateRelinquishMagickMemory
#define ResizeMagickMemory GMPrivateResizeMagickMemory
#define SetGeometryInfo GMPrivateSetGeometryInfo
#define _GeometryInfo _GMPrivateGeometryInfo

#define ExportImagePixels DispatchImage

typedef struct _GeometryInfo
{
  double
    rho,
    sigma,
    xi,
    psi;
} GeometryInfo;

typedef struct _MagickPixelPacket
{
  ColorspaceType
    colorspace;

  unsigned int
    matte;

  Quantum
    red,
    green,
    blue,
    opacity;

  IndexPacket
    index;
} MagickPixelPacket;


extern WandExport void
  *RelinquishMagickMemory(void *),
  *ResizeMagickMemory(void *memory,const size_t size),
   SetGeometryInfo(GeometryInfo *geometry_info);

extern WandExport int
  FormatMagickString(char *,const size_t,const char *,...)
    MAGICK_ATTRIBUTE((format (printf,3,4))),
  FormatMagickStringList(char *string,const size_t length,
    const char *format,va_list operands);

extern WandExport unsigned int
  ImportImagePixels(Image *image,const long x_offset,
    const long y_offset,const unsigned long columns,const unsigned long rows,
    const char *map,const StorageType type,const void *pixels),
  ParseAbsoluteGeometry(const char *geometry,RectangleInfo *region_info),
  ParseGeometry(const char *geometry,GeometryInfo *geometry_info),
  QueryMagickColor(const char *,MagickPixelPacket *,ExceptionInfo *);

extern WandExport size_t
  ConcatenateMagickString(char *,const char *,const size_t),
  CopyMagickString(char *,const char *,const size_t);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o p y M a g i c k S t r i n g                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CopyMagickString() copies the source string to the destination string.  The
%  destination buffer is always null-terminated even if the string must be
%  truncated.
%
%  The format of the CopyMagickString method is:
%
%      size_t CopyMagickString(const char *destination,char *source,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o destination: The destination string.
%
%    o source: The source string.
%
%    o length: The length of the destination string.
%
%
*/
WandExport size_t CopyMagickString(char *destination,const char *source,
  const size_t length)
{
  return(strlcpy(destination,source,length));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t M a g i c k S t r i n g                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FormatMagickString() prints formatted output of a variable argument list.
%
%  The format of the FormatMagickString method is:
%
%      void FormatMagickString(char *string,const size_t length,
%        const char *format,...)
%
%  A description of each parameter follows.
%
%   o string:  FormatMagickString() returns the formatted string in this
%     character buffer.
%
%   o length: The maximum length of the string.
%
%   o format:  A string describing the format to use to write the remaining
%     arguments.
%
%
*/
WandExport int FormatMagickStringList(char *string,const size_t length,
                                      const char *format,va_list operands)
{
  int
    count;

#if defined(HAVE_VSNPRINTF)
  count=vsnprintf(string,length,format,operands);
#else
  count=vsprintf(string,format,operands);
#endif

  return(count);
}
WandExport int FormatMagickString(char *string,const size_t length,
  const char *format,...)
{
  int
    count;

  va_list
    operands;

  va_start(operands, format);
  count=FormatMagickStringList(string,length,format,operands);
  va_end(operands);
  return (count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e M a g i c k S t r i n g                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConcatenateMagickString() concatenates the source string to the destination
%  string.  The destination buffer is always null-terminated even if the
%  string must be truncated.
%
%  The format of the ConcatenateMagickString method is:
%
%      size_t ConcatenateMagickString(char *destination,const char *source,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o destination: The destination string.
%
%    o source: The source string.
%
%    o length: The length of the destination string.
%
%
*/
WandExport size_t ConcatenateMagickString(char *destination,
  const char *source,const size_t length)
{
  return(strlcat(destination,source,length));
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t I m a g e P i x e l s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImagePixels() accepts pixel data and stores in the image at the
%  location you specify.  The method returns True on success otherwise False
%  if an error is encountered.  The pixel data can be either char, short int,
%  int, long, float, or double in the order specified by map.
%
%  Suppose your want want to upload the first scanline of a 640x480 image from
%  character data in red-green-blue order:
%
%      ImportImagePixels(image,0,0,640,1,"RGB",CharPixel,pixels);
%
%  The format of the ImportImagePixels method is:
%
%      unsigned int ImportImagePixels(Image *image,const long x_offset,
%        const long y_offset,const unsigned long columns,
%        const unsigned long rows,const char *map,const StorageType type,
%        const void *pixels)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o x_offset, y_offset: Offset (from top left) on base image on which
%      to composite image data.
%
%    o map:  This string reflects the expected ordering of the pixel array.
%      It can be any combination or order of R = red, G = green, B = blue,
%      A = alpha (same as Transparency), O = Opacity, T = Transparency,
%      C = cyan, Y = yellow, M = magenta, K = black, or I = intensity
%      (for grayscale). Specify "P" = pad, to skip over a quantum which is
%      intentionally ignored. Creation of an alpha channel for CMYK images
%      is currently not supported.
%
%    o type: Define the data type of the pixels.  Float and double types are
%      normalized to [0..1] otherwise [0..MaxRGB].  Choose from these types:
%      CharPixel, ShortPixel, IntegerPixel, LongPixel, FloatPixel, or
%      DoublePixel.
%
%    o pixels: This array of values contain the pixel components as defined by
%      map and type.  You must preallocate this array where the expected
%      length varies depending on the values of width, height, map, and type.
%
%
*/
WandExport unsigned int ImportImagePixels(Image *image,const long x_offset,
  const long y_offset,const unsigned long columns,const unsigned long rows,
  const char *map,const StorageType type,const void *pixels)
{
  Image
    *constitute_image;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  constitute_image=
    ConstituteImage(columns,rows,map,type,pixels,&image->exception);
  if (constitute_image)
    {
      (void) CompositeImage(image,CopyCompositeOp,constitute_image,x_offset,
                            y_offset);
      DestroyImage(constitute_image);
      return (image->exception.severity == UndefinedException);
    }
  return (False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e A b s o l u t e G e o m e t r y                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseAbsoluteGeometry() returns a region as defined by the geometry string.
%
%  The format of the ParseAbsoluteGeometry method is:
%
%      unsigned int ParseAbsoluteGeometry(const char *geometry,
%        RectangeInfo *region_info)
%
%  A description of each parameter follows:
%
%    o geometry:  The geometry (e.g. 100x100+10+10).
%
%    o region_info: The region as defined by the geometry string.
%
%
*/
WandExport unsigned int ParseAbsoluteGeometry(const char *geometry,
  RectangleInfo *region_info)
{
  unsigned int
    flags;

  flags=GetGeometry(geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e G e o m e t r y                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseGeometry() parses a geometry specification and returns the sigma,
%  rho, xi, and psi values.  It also returns flags that indicates which
%  of the four values (sigma, rho, xi, psi) were located in the string, and
%  whether the xi or pi values are negative.  In addition, there are flags to
%  report any meta characters (%, !, <, or >).
%
%  The format of the ParseGeometry method is:
%
%      unsigned int ParseGeometry(const char *geometry,
%        GeometryInfo *geometry_info)
%
%  A description of each parameter follows:
%
%    o geometry:  The geometry.
%
%    o geometry_info:  returns the parsed width/height/x/y in this structure.
%
%
*/
static inline char *MoveStringForward(char *dst,const char *src,size_t dstlen)
{
  const size_t srclen = strlen(src);
  const size_t movelen = Min(dstlen,srclen+1);
  (void) memmove(dst,src,movelen);
  dst[movelen-1]='\0';
  return dst;
}
WandExport unsigned int ParseGeometry(const char *geometry,
  GeometryInfo *geometry_info)
{
  char
    *p,
    pedantic_geometry[MaxTextExtent],
    *q;

  unsigned int
    flags;

  double
    double_val;

  /*
    Remove whitespaces meta characters from geometry specification.
  */
  assert(geometry_info != (GeometryInfo *) NULL);
  flags=NoValue;
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(flags);
  if (strlcpy(pedantic_geometry,geometry,sizeof(pedantic_geometry))
      >= sizeof(pedantic_geometry))
    return(flags);
  for (p=pedantic_geometry; *p != '\0'; )
  {
    if (isspace((int) (*p)))
      {
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        continue;
      }
    switch (*p)
    {
      case '%':
      {
        flags|=PercentValue;
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        break;
      }
      case '!':
      {
        flags|=AspectValue;
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        break;
      }
      case '<':
      {
        flags|=LessValue;
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        break;
      }
      case '>':
      {
        flags|=GreaterValue;
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        break;
      }
      case '@':
      {
        flags|=AreaValue;
        (void) MoveStringForward(p,p+1,sizeof(pedantic_geometry));
        break;
      }
      case '-':
      case '.':
      case '+':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'x':
      case 'X':
      case ',':
      case '/':
      {
        p++;
        break;
      }
      default:
      {
        ExceptionInfo
          exception;

        Image
          *image;

        ImageInfo
          *image_info;

        /*
          See if we can grab geometry from an image.
        */
        GetExceptionInfo(&exception);
        image_info=CloneImageInfo((ImageInfo *) NULL);
	(void) strlcpy(image_info->filename,geometry,sizeof(image_info->filename));
        image=PingImage(image_info,&exception);
        if (image != (Image *) NULL)
          {
            geometry_info->rho=image->columns;
            geometry_info->sigma=image->rows;
            flags|=RhoValue | SigmaValue;
            DestroyImage(image);
          }
        DestroyImageInfo(image_info);
        DestroyExceptionInfo(&exception);
        return(flags);
      }
    }
  }
  /*
    Parse rho, sigma, xi, and psi.
  */
  p=pedantic_geometry;
  if (*p == '\0')
    return(flags);
  q=p;
  double_val=strtod(p,&q);
  (void) double_val;
  if ((*q == 'x') || (*q == 'X') || (*q == '/') || (*q == ',') || (*q =='\0'))
    {
      /*
        Parse rho.
      */
      q=p;
      if (LocaleNCompare(p,"0x",2) == 0)
        geometry_info->rho=strtol(p,&p,10);
      else
        geometry_info->rho=strtod(p,&p);
      if (p != q)
        flags|=RhoValue;
    }
  if ((*p == 'x') || (*p == 'X') || (*p == '/') || (*p == ','))
    {
      /*
        Parse sigma.
      */
      p++;
      q=p;
      geometry_info->sigma=strtod(p,&p);
      if (p != q)
        flags|=SigmaValue;
    }
  if ((*p == '+') || (*p == '-') || (*p == ',') || (*p == '/'))
    {
      /*
        Parse xi value.
      */
      if ((*p == ',') || (*p == '/'))
        p++;
      q=p;
      geometry_info->xi=strtod(p,&p);
      if (p != q)
        {
          flags|=XiValue;
          if (*q == '-')
            flags|=XiNegative;
        }
      if ((*p == '+') || (*p == '-') || (*p == ',') || (*p == '/'))
        {
          /*
            Parse psi value.
          */
          if ((*p == ',') || (*p == '/'))
            p++;
          q=p;
          geometry_info->psi=strtod(p,&p);
          if (p != q)
            {
              flags|=PsiValue;
              if (*q == '-')
                flags|=PsiNegative;
            }
        }
    }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e l i n q u i s h M a g i c k M e m o r y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RelinquishMagickMemory() frees memory that has already been allocated with
%  AcquireMagickMemory().
%
%  The format of the RelinquishMagickMemory method is:
%
%      void *RelinquishMagickMemory(void *memory)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a block of memory to free for reuse.
%
%
*/
WandExport void *RelinquishMagickMemory(void *memory)
{
  if (memory == (void *) NULL)
    return((void *) NULL);
  free(memory);
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e s i z e M a g i c k M e m o r y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ResizeMagickMemory() changes the size of the memory and returns a pointer to
%  the (possibly moved) block.  The contents will be unchanged up to the
%  lesser of the new and old sizes.
%
%  The format of the ResizeMagickMemory method is:
%
%      void *ResizeMagickMemory(void *memory,const size_t size)
%
%  A description of each parameter follows:
%
%    o memory: A pointer to a memory allocation.  On return the pointer
%      may change but the contents of the original allocation will not.
%
%    o size: The new size of the allocated memory.
%
%
*/
WandExport void *ResizeMagickMemory(void *memory,const size_t size)
{
  void
    *allocation;

  if (memory == (void *) NULL)
    return(AcquireMagickMemory(size));
  allocation=realloc(memory,size);
  if (allocation == (void *) NULL)
    (void) RelinquishMagickMemory(memory);
  return(allocation);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y M a g i c k C o l o r                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryMagickColor() returns the red, green, blue, and opacity intensities
%  for a given color name.
%
%  The format of the QueryMagickColor method is:
%
%      unsigned int QueryMagickColor(const char *name,MagickPixelPacket *color,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The color name (e.g. white, blue, yellow).
%
%    o color: The red, green, blue, and opacity intensities values of the
%      named color in this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
WandExport unsigned int QueryMagickColor(const char *name,
  MagickPixelPacket *color,ExceptionInfo *exception)
{
  PixelPacket
    pixel;

  unsigned int
    status;

  status=QueryColorDatabase(name,&pixel,exception);
  color->colorspace=RGBColorspace;
  color->matte=0;
  color->red=pixel.red;
  color->green=pixel.green;
  color->blue=pixel.blue;
  color->opacity=pixel.opacity;
  color->index=0;
  return status;
}
