/*
% Copyright (C) 2003-2015 GraphicsMagick Group
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
%                            M   M   AAA   PPPP                               %
%                            MM MM  A   A  P   P                              %
%                            M M M  AAAAA  PPPP                               %
%                            M   M  A   A  P                                  %
%                            M   M  A   A  P                                  %
%                                                                             %
%                                                                             %
%                 Read/Write Image Colormaps As An Image File                 %
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
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/colormap.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteMAPImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMAPImage reads an image of raw RGB colormap and colormap index
%  bytes and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadMAPImage method is:
%
%      Image *ReadMAPImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMAPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define ThrowMAPReaderException(code_,reason_,image_) \
do { \
  MagickFreeMemory(colormap); \
  MagickFreeMemory(pixels);                   \
  ThrowReaderException(code_,reason_,image_); \
} while (0);
static Image *ReadMAPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  unsigned int
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  unsigned char
    *colormap = (unsigned char *) NULL,
    *pixels = (unsigned char *) NULL;

  unsigned int
    packet_size,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowMAPReaderException(OptionError,MustSpecifyImageSize,image);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowMAPReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Initialize image structure.
  */
  image->storage_class=PseudoClass;
  if (!AllocateImageColormap(image,image->offset ? image->offset : 256))
    ThrowMAPReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=MagickAllocateArray(unsigned char *,packet_size,image->colors);
  if (colormap == (unsigned char *) NULL)
    ThrowMAPReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Read image colormap.
  */
  if (ReadBlob(image,packet_size*image->colors,(char *) colormap) !=
      packet_size*image->colors)
    ThrowMAPReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  p=colormap;
  if (image->colors <= 256)
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=ScaleCharToQuantum(*p++);
      image->colormap[i].green=ScaleCharToQuantum(*p++);
      image->colormap[i].blue=ScaleCharToQuantum(*p++);
    }
  else
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=(*p++ << 8U);
      image->colormap[i].red|=(*p++);
      image->colormap[i].green=(*p++ << 8U);
      image->colormap[i].green|=(*p++);
      image->colormap[i].blue=(*p++ << 8U);
      image->colormap[i].blue|=(*p++);
    }
  MagickFreeMemory(colormap);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=MagickAllocateArray(unsigned char *,packet_size,image->columns);
  if (pixels == (unsigned char *) NULL)
    ThrowMAPReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  for (y=0; y < (long) image->rows; y++)
  {
    p=pixels;
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=AccessMutableIndexes(image);
    if (ReadBlob(image,packet_size*image->columns,(char *) pixels) !=
        packet_size*image->columns)
      ThrowMAPReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(*p++);
      if (image->colors > 256)
        index=((index << 8)+(*p++));
      VerifyColormapIndex(image,index);
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixelsEx(image,exception))
      break;
  }
  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMAPImage adds attributes for the MAP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMAPImage method is:
%
%      RegisterMAPImage(void)
%
*/
ModuleExport void RegisterMAPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MAP");
  entry->decoder=(DecoderHandler) ReadMAPImage;
  entry->encoder=(EncoderHandler) WriteMAPImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description="Colormap intensities and indices";
  entry->module="MAP";
  entry->coder_class=PrimaryCoderClass;
  entry->extension_treatment=ObeyExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMAPImage removes format registrations made by the
%  MAP module from the list of supported formats.
%
%  The format of the UnregisterMAPImage method is:
%
%      UnregisterMAPImage(void)
%
*/
ModuleExport void UnregisterMAPImage(void)
{
  (void) UnregisterMagickInfo("MAP");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMAPImage writes an image to a file as red, green, and blue
%  colormap bytes followed by the colormap indexes.
%
%  The format of the WriteMAPImage method is:
%
%      unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMAPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
#define ThrowMAPWriterException(code_,reason_,image_) \
do { \
  MagickFreeMemory(colormap); \
  MagickFreeMemory(pixels);                   \
  ThrowWriterException(code_,reason_,image_); \
} while (0);
static unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *colormap = (unsigned char *) NULL,
    *pixels = (unsigned char *) NULL;

  unsigned int
    status;

  unsigned long
    packet_size;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowMAPWriterException(FileOpenError,UnableToOpenFile,image);
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Allocate colormap.
  */
  if (!IsPaletteImage(image,&image->exception))
    (void) SetImageType(image,PaletteType);
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=MagickAllocateMemory(unsigned char *,image->columns*packet_size);
  if (pixels == (unsigned char *) NULL)
    ThrowMAPWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=MagickAllocateMemory(unsigned char *,packet_size*image->colors);
  if (colormap == (unsigned char *) NULL)
    ThrowMAPWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Write colormap to file.
  */
  q=colormap;
  if (image->colors <= 256)
    for (i=0; i < (long) image->colors; i++)
    {
      *q++=image->colormap[i].red;
      *q++=image->colormap[i].green;
      *q++=image->colormap[i].blue;
    }
#if QuantumDepth > 8
  else
    for (i=0; i < (long) image->colors; i++)
    {
      *q++=image->colormap[i].red >> 8;
      *q++=image->colormap[i].red & 0xff;
      *q++=image->colormap[i].green >> 8;
      *q++=image->colormap[i].green & 0xff;
      *q++=image->colormap[i].blue >> 8;
      *q++=image->colormap[i].blue & 0xff;
    }
#endif /* QuantumDepth > 8 */
  if (WriteBlob(image,packet_size*image->colors,(char *) colormap) !=
      packet_size*image->colors)
    ThrowMAPWriterException(FileOpenError,UnableToWriteFile,image);
  MagickFreeMemory(colormap);
  /*
    Write image pixels to file.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=AccessImmutableIndexes(image);
    q=pixels;
    for (x=0; x < (long) image->columns; x++)
    {
#if QuantumDepth > 8
      if (image->colors > 256)
        *q++=indexes[x] >> 8;
#endif /* QuantumDepth > 8 */
      *q++=indexes[x];
    }
    if (WriteBlob(image,q-pixels,(char *) pixels) != (size_t) (q-pixels))
      ThrowMAPWriterException(FileOpenError,UnableToWriteFile,image);
  }
  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(status);
}
