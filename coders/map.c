/*
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
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMAPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
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
    *colormap,
    *pixels;

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
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  image->storage_class=PseudoClass;
  if (!AllocateImageColormap(image,image->offset ? image->offset : 256))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=(unsigned char *) AcquireMemory(packet_size*image->columns);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=(unsigned char *) AcquireMemory(packet_size*image->colors);
  if ((pixels == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read image colormap.
  */
  (void) ReadBlob(image,packet_size*image->colors,(char *) colormap);
  p=colormap;
  if (image->colors <= 256)
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=UpScale(*p++);
      image->colormap[i].green=UpScale(*p++);
      image->colormap[i].blue=UpScale(*p++);
    }
  else
    for (i=0; i < (long) image->colors; i++)
    {
      image->colormap[i].red=(*p++ << 8);
      image->colormap[i].red|=(*p++);
      image->colormap[i].green=(*p++ << 8);
      image->colormap[i].green|=(*p++);
      image->colormap[i].blue=(*p++ << 8);
      image->colormap[i].blue|=(*p++);
    }
  LiberateMemory((void **) &colormap);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  packet_size=image->depth > 8 ? 2 : 1;
  for (y=0; y < (long) image->rows; y++)
  {
    p=pixels;
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) ReadBlob(image,packet_size*image->columns,(char *) pixels);
    for (x=0; x < (long) image->columns; x++)
    {
      index=ValidateColormapIndex(image,*p++);
      if (image->colors > 256)
        index=ValidateColormapIndex(image,(index << 8)+(*p++));
      indexes[x]=index;
      *q++=image->colormap[index];
    }
    if (!SyncImagePixels(image))
      break;
  }
  LiberateMemory((void **) &pixels);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
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
  entry->decoder=ReadMAPImage;
  entry->encoder=WriteMAPImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description=AllocateString("Colormap intensities and indices");
  entry->module=AllocateString("MAP");
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *colormap,
    *pixels;

  unsigned int
    packet_size,
    status;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Allocate colormap.
  */
  if (!IsPaletteImage(image,&image->exception))
    SetImageType(image,PaletteType);
  packet_size=image->depth > 8 ? 2 : 1;
  pixels=(unsigned char *) AcquireMemory(image->columns*packet_size);
  packet_size=image->colors > 256 ? 6 : 3;
  colormap=(unsigned char *) AcquireMemory(packet_size*image->colors);
  if ((pixels == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
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
  (void) WriteBlob(image,packet_size*image->colors,(char *) colormap);
  LiberateMemory((void **) &colormap);
  /*
    Write image pixels to file.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    q=pixels;
    for (x=0; x < (long) image->columns; x++)
    {
      if (image->colors > 256)
        *q++=indexes[x] >> 8;
      *q++=indexes[x];
    }
    (void) WriteBlob(image,q-pixels,(char *) pixels);
  }
  LiberateMemory((void **) &pixels);
  CloseBlob(image);
  return(status);
}
