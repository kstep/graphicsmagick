/*
% Copyright (C) 2013 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         W   W  EEEEE  BBBB   PPPP                           %
%                         W   W  E      B   B  P   P                          %
%                         W W W  EEE    BBBB   PPPP                           %
%                         WW WW  E      B   B  P                              %
%                         W   W  EEEEE  BBBB   P                              %
%                                                                             %
%                                                                             %
%                     Read/Write Google WEBP Image Format.                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                  TIMEBUG                                    %
%                                January 2013                                 %
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
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/log.h"
#include "magick/constitute.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
#if defined(HasWEBP)
static unsigned int WriteWEBPImage(const ImageInfo *,Image *);
#endif

#if defined(HasWEBP)
#include <webp/decode.h>
#include <webp/encode.h>

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d W E B P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadWEBPImage() reads an image in the WebP image format.
%
%  The format of the ReadWEBPImage method is:
%
%      Image *ReadWEBPImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *ReadWEBPImage(const ImageInfo *image_info,
                            ExceptionInfo *exception)
{
  int
    height,
    width;

  Image
    *image;

  int
    status;

  size_t
    length;

  unsigned long
    count,
    y;

  register PixelPacket
    *q;

  register size_t
    x;

  register unsigned char
    *p;

  unsigned char
    *stream,
    *pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  image->depth=8;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read WEBP file.
  */
  length = (size_t) GetBlobSize(image);
  stream=MagickAllocateArray(unsigned char *,
                             length,sizeof(*stream));
  if (stream == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);

  count=(long) ReadBlob(image,length,(char *) stream);
  if (count != (size_t) length)
    {
      MagickFreeMemory(stream);
      ThrowReaderException(CorruptImageError,InsufficientImageDataInFile,image);
    }

  pixels=(unsigned char *) WebPDecodeRGBA(stream,length,&width,&height);
  if (pixels == (unsigned char *) NULL)
    {
      MagickFreeMemory(stream);
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }
  image->columns=(size_t) width;
  image->rows=(size_t) height;

  p=pixels;

  for (y=0; y < (size_t) image->rows; y++)
    {
      q=GetImagePixelsEx(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;

      for (x=0; x < (size_t) image->columns; x++)
        {
          SetRedSample(q,ScaleCharToQuantum(*p++));
          SetGreenSample(q,ScaleCharToQuantum(*p++));
          SetBlueSample(q,ScaleCharToQuantum(*p++));
          SetOpacitySample(q,MaxRGB-ScaleCharToQuantum(*p++));
          q++;
        }

      if(!SyncImagePixels(image))
        break;
    }
  /*
    Free scale resource.
  */
  free(pixels);
  pixels=(unsigned char *) NULL;
  MagickFreeMemory(stream);
  CloseBlob(image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r W E B P I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterWEBPImage adds attributes for the WEBP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterWEBPImage method is:
%
%      RegisterWEBPImage(void)
%
*/
ModuleExport void RegisterWEBPImage(void)
{
  static const char
    *description = "WebP Image Format";

  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  *version='\0';
  entry=SetMagickInfo("WEBP");
#if defined(HasWEBP)
  entry->decoder=(DecoderHandler) ReadWEBPImage;
  entry->encoder=(EncoderHandler) WriteWEBPImage;
#endif
  entry->description=description;
  entry->adjoin=False;
  entry->seekable_stream=MagickTrue;
  if (*version != '\0')
    entry->version=version;
  entry->module="WEBP";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r W E B P I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterWEBPImage removes format registrations made by the
%  WEBP module from the list of supported formats.
%
%  The format of the UnregisterWEBPImage method is:
%
%      UnregisterWEBPImage(void)
%
*/
ModuleExport void UnregisterWEBPImage(void)
{
  (void) UnregisterMagickInfo("WEBP");
}

#if defined(HasWEBP)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e W E B P I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteWEBPImage() writes an image in the WebP image format.
%
%  The format of the WriteWEBPImage method is:
%
%      MagickPassFail WriteWEBPImage(const ImageInfo *image_info, Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: the image info.
%
%    o image:  The image.
%
*/

static int WebPWriter(const unsigned char *stream,size_t length,
                      const WebPPicture *const picture)
{
  Image
    *image;

  image=(Image *) picture->custom_ptr;
  return (length != 0 ? (int) WriteBlob(image,length,stream) : 1);
}


static unsigned int WriteWEBPImage(const ImageInfo *image_info,Image *image)
{
  int
    webp_status;

  unsigned int
    status;

  register PixelPacket
    *p;

  register size_t
    x;

  register unsigned char
    *q;

  unsigned long
    y;

  size_t
    per_column;

  unsigned char
    *pixels;

  WebPConfig
    configure;

  WebPPicture
    picture;

  WebPAuxStats
    statistics;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->columns > 16383) || (image->rows > 16383))
    ThrowWriterException(ImageError,WidthOrHeightExceedsLimit,image);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  if (WebPPictureInit(&picture) == 0)
    ThrowWriterException(ResourceLimitError, MemoryAllocationFailed, image);
  picture.writer=WebPWriter;
  picture.custom_ptr=(void *) image;
  picture.stats=(&statistics);
  picture.width=(int) image->columns;
  picture.height=(int) image->rows;
  if (WebPConfigInit(&configure) == 0)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  if (image_info->quality != DefaultCompressionQuality)
    configure.quality = (float) image_info->quality;
  if (WebPValidateConfig(&configure) == 0)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  /*
    Allocate memory for pixels.
  */
  per_column = MagickArraySize(MagickArraySize(4,image->rows),sizeof(*pixels));
  pixels=MagickAllocateArray(unsigned char *,image->columns,per_column);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  /*
    Convert image to WebP raster pixels.
  */
  q=pixels;
  for (y=0; y < (size_t) image->rows; y++)
    {
      p=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (size_t) image->columns; x++)
        {
          *q++=ScaleQuantumToChar(GetRedSample(p));
          *q++=ScaleQuantumToChar(GetGreenSample(p));
          *q++=ScaleQuantumToChar(GetBlueSample(p));
          if (image->matte == MagickTrue)
            *q++=ScaleQuantumToChar(MaxRGB-GetOpacitySample(p));
          p++;
        }
    }

  if (image->matte != MagickTrue)
    webp_status=WebPPictureImportRGB(&picture,pixels,3*picture.width);
  else
    webp_status=WebPPictureImportRGBA(&picture,pixels,4*picture.width);
  MagickFreeMemory(pixels);
  webp_status=WebPEncode(&configure, &picture);
  WebPPictureFree(&picture);
  CloseBlob(image);

  return(webp_status == 0 ? MagickPass : MagickFail);
}
#endif
