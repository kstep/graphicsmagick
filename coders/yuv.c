/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            Y   Y  U   U  V   V                              %
%                             Y Y   U   U  V   V                              %
%                              Y    U   U  V   V                              %
%                              Y    U   U   V V                               %
%                              Y     UUU     V                                %
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
  WriteYUVImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d Y U V I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadYUVImage reads an image with digital YUV (CCIR 601 4:1:1) bytes
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadYUVImage method is:
%
%      Image *ReadYUVImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadYUVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadYUVImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *chroma_image,
    *image,
    *resize_image;

  int
    count,
    y;

  register int
    i,
    x;

  register PixelPacket
    *q,
    *r;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  unsigned int
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  image->depth=8;
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryType);
      if (status == False)
        ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) ReadBlobByte(image);
    }
  /*
    Allocate memory for a scanline.
  */
  scanline=(unsigned char *) AcquireMemory(image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  do
  {
    /*
      Convert raster image to pixel packets.
    */
    if (image_info->interlace == PartitionInterlace)
      {
        AppendImageFormat("Y",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryType);
        if (status == False)
          ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      }
    for (y=0; y < (int) image->rows; y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadBlob(image,image->columns,scanline);
      p=scanline;
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        q->red=UpScale(*p++);
        q->green=0;
        q->blue=0;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        MagickMonitor(LoadImageText,y,image->rows);
    }
    if (image_info->interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("U",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryType);
        if (status == False)
          ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      }
    chroma_image=CloneImage(image,image->columns/2,image->rows/2,True,
      exception);
    if (chroma_image == (Image *) NULL)
      return((Image *) NULL);
    for (y=0; y < (int) chroma_image->rows; y++)
    {
      (void) ReadBlob(image,chroma_image->columns,scanline);
      p=scanline;
      q=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) chroma_image->columns; x++)
      {
        q->red=0;
        q->green=UpScale(*p++);
        q->blue=0;
        q++;
      }
      if (!SyncImagePixels(chroma_image))
        break;
    }
    if (image_info->interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("V",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryType);
        if (status == False)
          ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      }
    for (y=0; y < (int) chroma_image->rows; y++)
    {
      (void) ReadBlob(image,chroma_image->columns,scanline);
      p=scanline;
      q=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) chroma_image->columns; x++)
      {
        q->blue=UpScale(*p++);
        q++;
      }
      if (!SyncImagePixels(chroma_image))
        break;
    }
    /*
      Scale image.
    */
    chroma_image->orphan=True;
    resize_image=ResizeImage(chroma_image,image->columns,image->rows,
      TriangleFilter,1.0,exception);
    DestroyImage(chroma_image);
    if (resize_image == (Image *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    for (y=0; y < (int) image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      r=GetImagePixels(resize_image,0,y,resize_image->columns,1);
      if ((q == (PixelPacket *) NULL) || (r == (PixelPacket *) NULL))
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        q->green=r->green;
        q->blue=r->blue;
        r++;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
    }
    DestroyImage(resize_image);
    TransformRGBImage(image,YCbCrColorspace);
    if (image_info->interlace == PartitionInterlace)
      (void) strcpy(image->filename,image_info->filename);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,image->columns,(char *) scanline);
    if (count > 0)
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
        MagickMonitor(LoadImagesText,TellBlob(image),image->blob.filesize);
      }
  } while (count > 0);
  LiberateMemory((void **) &scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
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
%   R e g i s t e r Y U V I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterYUVImage adds attributes for the YUV image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterYUVImage method is:
%
%      RegisterYUVImage(void)
%
*/
ModuleExport void RegisterYUVImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("YUV");
  entry->decoder=ReadYUVImage;
  entry->encoder=WriteYUVImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description=AllocateString("CCIR 601 4:1:1");
  entry->module=AllocateString("YUV");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r Y U V I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterYUVImage removes format registrations made by the
%  YUV module from the list of supported formats.
%
%  The format of the UnregisterYUVImage method is:
%
%      UnregisterYUVImage(void)
%
*/
ModuleExport void UnregisterYUVImage(void)
{
  UnregisterMagickInfo("YUV");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e Y U V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteYUVImage writes an image to a file in the digital YUV
%  (CCIR 601 4:1:1) format.
%
%  The format of the WriteYUVImage method is:
%
%      unsigned int WriteYUVImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteYUVImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteYUVImage(const ImageInfo *image_info,Image *image)
{
  Image
    *chroma_image,
    *yuv_image;

  int
    y;

  register int
    x;

  register PixelPacket
    *p;

  unsigned int
    height,
    scene,
    status,
    width;

  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryType);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
    }
  if (image_info->interlace == PartitionInterlace)
    {
      AppendImageFormat("Y",image->filename);
      status=OpenBlob(image_info,image,WriteBinaryType);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
    }
  scene=0;
  do
  {
    /*
      Sample image to an even width and height.
    */
    TransformRGBImage(image,RGBColorspace);
    width=image->columns+(image->columns & 0x01);
    height=image->rows+(image->rows & 0x01);
    image->orphan=True;
    yuv_image=ResizeImage(image,width,height,TriangleFilter,1.0,
      &image->exception);
    if (yuv_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Unable to resize image",image);
    RGBTransformImage(yuv_image,YCbCrColorspace);
    /*
      Initialize Y channel.
    */
    for (y=0; y < (int) yuv_image->rows; y++)
    {
      p=GetImagePixels(yuv_image,0,y,yuv_image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) yuv_image->columns; x++)
      {
        (void) WriteBlobByte(image,DownScale(p->red));
        p++;
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          MagickMonitor(SaveImageText,y,image->rows);
    }
    DestroyImage(yuv_image);
    /*
      Downsample image.
    */
    image->orphan=True;
    chroma_image=ResizeImage(image,width/2,height/2,TriangleFilter,1.0,
      &image->exception);
    if (chroma_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Unable to resize image",image);
    RGBTransformImage(chroma_image,YCbCrColorspace);
    /*
      Initialize U channel.
    */
    if (image_info->interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("U",image->filename);
        status=OpenBlob(image_info,image,WriteBinaryType);
        if (status == False)
          ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      }
    for (y=0; y < (int) chroma_image->rows; y++)
    {
      p=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) chroma_image->columns; x++)
      {
        (void) WriteBlobByte(image,DownScale(p->green));
        p++;
      }
    }
    /*
      Initialize V channel.
    */
    if (image_info->interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("V",image->filename);
        status=OpenBlob(image_info,image,WriteBinaryType);
        if (status == False)
          ThrowWriterException(FileOpenWarning,"Unable to open file",image);
      }
    for (y=0; y < (int) chroma_image->rows; y++)
    {
      p=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) chroma_image->columns; x++)
      {
        (void) WriteBlobByte(image,DownScale(p->blue));
        p++;
      }
    }
    DestroyImage(chroma_image);
    if (image_info->interlace == PartitionInterlace)
      (void) strcpy(image->filename,image_info->filename);
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
