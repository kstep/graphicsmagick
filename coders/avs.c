/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             AAA   V   V  SSSSS                              %
%                            A   A  V   V  SS                                 %
%                            AAAAA  V   V   SSS                               %
%                            A   A   V V      SS                              %
%                            A   A    V    SSSSS                              %
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
  WriteAVSImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A V S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadAVSImage reads an AVS X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadAVSImage method is:
%
%      Image *ReadAVSImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadAVSImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadAVSImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned long
    height,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read AVS image.
  */
  width=ReadBlobMSBLong(image);
  height=ReadBlobMSBLong(image);
  if ((width == (unsigned long) ~0) || (height == (unsigned long) ~0))
    ThrowReaderException(CorruptImageWarning,"Not a AVS image file",image);
  do
  {
    /*
      Convert AVS raster image to pixel packets.
    */
    image->columns=(unsigned int) width;
    image->rows=(unsigned int) height;
    image->depth=8;
    pixels=(unsigned char *) AcquireMemory(4*image->columns);
    if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
        image);
    for (y=0; y < (int) image->rows; y++)
    {
      count=ReadBlob(image,4*image->columns,pixels);
      if (count == 0)
        ThrowReaderException(CorruptImageWarning,"Unable to read image data",
          image);
      p=pixels;
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        q->opacity=MaxRGB-UpScale(*p++);
        q->red=UpScale(*p++);
        q->green=UpScale(*p++);
        q->blue=UpScale(*p++);
        image->matte|=(q->opacity != OpaqueOpacity);
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
    }
    LiberateMemory((void **) &pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    width=ReadBlobMSBLong(image);
    height=ReadBlobMSBLong(image);
    if ((width != (unsigned long) ~0) && (height != (unsigned long) ~0))
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
  } while ((width != (unsigned long) ~0) && (height != (unsigned long) ~0));
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
%   R e g i s t e r A V S I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterAVSImage adds attributes for the AVS image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterAVSImage method is:
%
%      RegisterAVSImage(void)
%
*/
ModuleExport void RegisterAVSImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AVS");
  entry->decoder=ReadAVSImage;
  entry->encoder=WriteAVSImage;
  entry->description=AllocateString("AVS X image");
  entry->module=AllocateString("AVS");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r A V S I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterAVSImage removes format registrations made by the
%  AVS module from the list of supported formats.
%
%  The format of the UnregisterAVSImage method is:
%
%      UnregisterAVSImage(void)
%
*/
ModuleExport void UnregisterAVSImage(void)
{
  (void) UnregisterMagickInfo("AVS");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e A V S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteAVSImage writes an image to a file in AVS X image format.
%
%  The format of the WriteAVSImage method is:
%
%      unsigned int WriteAVSImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteAVSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteAVSImage(const ImageInfo *image_info,Image *image)
{
  register int
    x,
    y;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Write AVS header.
    */
    TransformRGBImage(image,RGBColorspace);
    WriteBlobMSBLong(image,image->columns);
    WriteBlobMSBLong(image,image->rows);
    /*
      Allocate memory for pixels.
    */
    pixels=(unsigned char *) AcquireMemory(image->columns*sizeof(PixelPacket));
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    /*
      Convert MIFF to AVS raster pixels.
    */
    for (y=0; y < (int) image->rows; y++)
    {
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      q=pixels;
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=MaxRGB-DownScale((image->matte ? p->opacity : OpaqueOpacity));
        *q++=DownScale(p->red);
        *q++=DownScale(p->green);
        *q++=DownScale(p->blue);
        p++;
      }
      (void) WriteBlob(image,q-pixels,(char *) pixels);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          MagickMonitor(SaveImageText,y,image->rows);
    }
    LiberateMemory((void **) &pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
