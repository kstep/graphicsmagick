/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            TTTTT  IIIII  M   M                              %
%                              T      I    MM MM                              %
%                              T      I    M M M                              %
%                              T      I    M   M                              %
%                              T    IIIII  M   M                              %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d T I M I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTIMImage reads a PSX TIM image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  Contributed by os@scee.sony.co.uk.
%
%  The format of the ReadTIMImage method is:
%
%      Image *ReadTIMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTIMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTIMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  typedef struct _TIMInfo
  {
    unsigned long
      id,
      flag;
  } TIMInfo;

  TIMInfo
    tim_info;

  Image
    *image;

  int
    bits_per_pixel,
    has_clut;

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
    *tim_data,
    *tim_pixels;

  unsigned short
    word;

  unsigned int
    bytes_per_line,
    height,
    image_size,
    status,
    width;

  unsigned long
    pixel_mode;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a TIM file.
  */
  tim_info.id=ReadBlobLSBLong(image);
  do
  {
    /*
      Verify TIM identifier.
    */
    if (tim_info.id != 0x00000010)
      ThrowReaderException(CorruptImageWarning,"Not a TIM image file",image);
    tim_info.flag=ReadBlobLSBLong(image);
    has_clut=!!(tim_info.flag & (1 << 3));
    pixel_mode=tim_info.flag & 0x07;
    switch (pixel_mode)
    {
      case 0: bits_per_pixel=4; break;
      case 1: bits_per_pixel=8; break;
      case 2: bits_per_pixel=16; break;
      case 3: bits_per_pixel=24; break;
      default: bits_per_pixel=4; break;
    }
    if (has_clut)
      {
        unsigned char
          *tim_colormap;

        /*
          Read TIM raster colormap.
        */
        (void)ReadBlobLSBLong(image);
        (void)ReadBlobLSBShort(image);
        (void)ReadBlobLSBShort(image);
        width=ReadBlobLSBShort(image);
        height=ReadBlobLSBShort(image);
        if (!AllocateImageColormap(image,pixel_mode == 1 ? 256 : 16))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        tim_colormap=(unsigned char *) AcquireMemory(image->colors*2);
        if (tim_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) ReadBlob(image,2*image->colors,(char *) tim_colormap);
        p=tim_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          word=(*p++);
          word|=(*p++ << 8);
          image->colormap[i].blue=UpScale(ScaleColor5to8((word >> 10) & 0x1f));
          image->colormap[i].green=UpScale(ScaleColor5to8((word >> 5) & 0x1f));
          image->colormap[i].red=UpScale(ScaleColor5to8(word & 0x1f));
        }
        LiberateMemory((void **) &tim_colormap);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Read image data.
    */
    (void) ReadBlobLSBLong(image);
    (void) ReadBlobLSBShort(image);
    (void) ReadBlobLSBShort(image);
    width=ReadBlobLSBShort(image);
    height=ReadBlobLSBShort(image);
    image_size=2*width*height;
    bytes_per_line=width*2;
    width=(width*16)/bits_per_pixel;
    tim_data=(unsigned char *) AcquireMemory(image_size);
    if (tim_data == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",            image);
    (void) ReadBlob(image,image_size,(char *) tim_data);
    tim_pixels=tim_data;
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    /*
      Convert TIM raster image to pixel packets.
    */
    switch (bits_per_pixel)
    {
      case 4:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          p=tim_pixels+y*bytes_per_line;
          for (x=0; x < ((long) image->columns-1); x+=2)
          {
            indexes[x]=(*p) & 0xf;
            indexes[x+1]=(*p >> 4) & 0xf;
            p++;
          }
          if ((image->columns % 2) != 0)
            {
              indexes[x]=(*p >> 4) & 0xf;
              p++;
            }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          p=tim_pixels+y*bytes_per_line;
          for (x=0; x < (long) image->columns; x++)
            indexes[x]=(*p++);
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 16:
      {
        /*
          Convert DirectColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            word=(*p++);
            word|=(*p++ << 8);
            q->blue=UpScale(ScaleColor5to8((word >> 10) & 0x1f));
            q->green=UpScale(ScaleColor5to8((word >> 5) & 0x1f));
            q->red=UpScale(ScaleColor5to8(word & 0x1f));
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 24:
      {
        /*
          Convert DirectColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      default:
        ThrowReaderException(CorruptImageWarning,"Not a TIM image file",image)
    }
    if (image->storage_class == PseudoClass)
      SyncImage(image);
    LiberateMemory((void **) &tim_pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    tim_info.id=ReadBlobLSBLong(image);
    if (tim_info.id == 0x00000010)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image=image->next;
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
      }
  } while (tim_info.id == 0x00000010);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T I M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTIMImage adds attributes for the TIM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTIMImage method is:
%
%      RegisterTIMImage(void)
%
*/
ModuleExport void RegisterTIMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TIM");
  entry->decoder=ReadTIMImage;
  entry->description=AllocateString("PSX TIM");
  entry->module=AllocateString("TIM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T I M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTIMImage removes format registrations made by the
%  TIM module from the list of supported formats.
%
%  The format of the UnregisterTIMImage method is:
%
%      UnregisterTIMImage(void)
%
*/
ModuleExport void UnregisterTIMImage(void)
{
  (void) UnregisterMagickInfo("TIM");
}
