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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include "studio.h"

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
%  Method ReadYUVImage reads an image with digital YUV (CCIR 601 4:1:1, plane
%  or partition interlaced, or 4:2:2 plane, partition interlaced or
%  noninterlaced) bytes and returns it.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
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
    *clone_image,
    *image,
    *resize_image;

  long
    horizontal_factor,
    vertical_factor,
    y;

  register const PixelPacket
    *r;

  register long
    x;

  register PixelPacket
    *q,
    *s;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    *scanline;

  unsigned int
    status;

  InterlaceType
    interlace;

  /*
    Allocate image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"Must specify image size",image);
  image->depth=8;
  interlace=image_info->interlace;
  horizontal_factor=2;
  vertical_factor=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      long
        factors;

      factors=sscanf(image_info->sampling_factor,"%ldx%ld",&horizontal_factor,
        &vertical_factor);
      if (factors != 2)
        vertical_factor=horizontal_factor;
      if ((horizontal_factor != 1) && (horizontal_factor != 2) &&
          (vertical_factor != 1) && (vertical_factor != 2))
        ThrowReaderException(CorruptImageError,"Unexpected sampling factor",
          image);
    }
  if ((interlace == UndefinedInterlace) ||
      ((interlace == NoInterlace) && (vertical_factor == 2)))
    {
      interlace=NoInterlace;    /* CCIR 4:2:2 */
      if (vertical_factor == 2)
        interlace=PlaneInterlace; /* CCIR 4:1:1 */
    }
  if (interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryType,exception);
      if (status == False)
        ThrowReaderException(FileOpenError,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) ReadBlobByte(image);
    }
  /*
    Allocate memory for a scanline.
  */
  if (interlace == NoInterlace)
    scanline=(unsigned char *) AcquireMemory(2*image->columns+2);
  else
    scanline=(unsigned char *) AcquireMemory(image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  do
  {
    chroma_image=CloneImage(image,image->columns/horizontal_factor,
      image->rows/vertical_factor,True,exception);
    if (chroma_image == (Image *) NULL)
      ThrowReaderException(ResourceLimitError,"Memory allocation failed",
        image);
    /*
      Convert raster image to pixel packets.
    */
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (interlace == PartitionInterlace)
      {
        AppendImageFormat("Y",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryType,exception);
        if (status == False)
          ThrowReaderException(FileOpenError,"Unable to open file",image);
      }
    for (y=0; y < (long) image->rows; y++)
    {
      if (interlace == NoInterlace)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,2*image->columns,scanline);
          p=scanline;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          s=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
          if (s == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x+=2)
          {
            s->red=0;
            s->green=Upscale(*p++);
            q->red=Upscale(*p++);
            q->green=0;
            q->blue=0;
            q++;
            q->green=0;
            q->blue=0;
            s->blue=Upscale(*p++);
            q->red=Upscale(*p++);
            s++;
            q++;
          }
        }
      else
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,image->columns,scanline);
          p=scanline;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=Upscale(*p++);
            q->green=0;
            q->blue=0;
            q++;
          }
        }
      if (!SyncImagePixels(image))
        break;
      if (interlace == NoInterlace)
        if (!SyncImagePixels(chroma_image))
          break;
      if (image->previous == (Image *) NULL)
        MagickMonitor(LoadImageText,y,image->rows);
    }
    if (interlace == PartitionInterlace)
      {
        CloseBlob(image);
        AppendImageFormat("U",image->filename);
        status=OpenBlob(image_info,image,ReadBinaryType,exception);
        if (status == False)
          ThrowReaderException(FileOpenError,"Unable to open file",image);
      }
    if (interlace != NoInterlace)
      {
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          (void) ReadBlob(image,chroma_image->columns,scanline);
          p=scanline;
          q=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            q->red=0;
            q->green=Upscale(*p++);
            q->blue=0;
            q++;
          }
          if (!SyncImagePixels(chroma_image))
            break;
        }
      if (interlace == PartitionInterlace)
        {
          CloseBlob(image);
          AppendImageFormat("V",image->filename);
          status=OpenBlob(image_info,image,ReadBinaryType,exception);
          if (status == False)
            ThrowReaderException(FileOpenError,"Unable to open file",image);
        }
      for (y=0; y < (long) chroma_image->rows; y++)
      {
        (void) ReadBlob(image,chroma_image->columns,scanline);
        p=scanline;
        q=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) chroma_image->columns; x++)
        {
          q->blue=Upscale(*p++);
          q++;
        }
        if (!SyncImagePixels(chroma_image))
          break;
      }
    }
    /*
      Scale image.
    */
    clone_image=CloneImage(chroma_image,0,0,True,&image->exception);
    if (clone_image == (Image *) NULL)
      return(False);
    resize_image=ResizeImage(clone_image,image->columns,image->rows,
      TriangleFilter,1.0,exception);
    DestroyImage(clone_image);
    DestroyImage(chroma_image);
    if (resize_image == (Image *) NULL)
      ThrowReaderException(ResourceLimitError,"Memory allocation failed",
        image);
    for (y=0; y < (long) image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      r=AcquireImagePixels(resize_image,0,y,resize_image->columns,1,
        &resize_image->exception);
      if ((q == (PixelPacket *) NULL) || (r == (const PixelPacket *) NULL))
        break;
      for (x=0; x < (long) image->columns; x++)
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
    (void) TransformRGBImage(image,YCbCrColorspace);
    if (interlace == PartitionInterlace)
      (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageError,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (interlace == NoInterlace)
      count=ReadBlob(image,2*image->columns,(char *) scanline);
    else
      count=ReadBlob(image,image->columns,(char *) scanline);
    if (count != 0)
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
        image=image->next;
        MagickMonitor(LoadImagesText,TellBlob(image),GetBlobSize(image));
      }
  } while (count != 0);
  LiberateMemory((void **) &scanline);
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
  entry->description=AcquireString("CCIR 601 4:1:1 or 4:2:2");
  entry->module=AcquireString("YUV");
  (void) RegisterMagickInfo(entry);
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
  (void) UnregisterMagickInfo("YUV");
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
%  (CCIR 601 4:1:1, plane or partition interlaced, or 4:2:2 plane, partition
%  interlaced or noninterlaced) bytes and returns it.
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
    *clone_image,
    *yuv_image;

  long
    horizontal_factor,
    vertical_factor,
    y;

  register const PixelPacket
    *p,
    *s;

  register long
    x;

  unsigned int
    status;

  unsigned long
    scene,
    height,
    width;

  InterlaceType
    interlace;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  interlace=image_info->interlace;
  horizontal_factor=2;
  vertical_factor=2;
  if (image_info->sampling_factor != (char *) NULL)
    {
      long
        factors;

      factors=sscanf(image_info->sampling_factor,"%ldx%ld",&horizontal_factor,
        &vertical_factor);
      if (factors != 2)
        vertical_factor=horizontal_factor;
      if ((horizontal_factor != 1) && (horizontal_factor != 2) &&
          (vertical_factor != 1) && (vertical_factor != 2))
        ThrowWriterException(ResourceLimitError,"Unexpected sampling factor",
          image);
    }
  if ((interlace == UndefinedInterlace) ||
      ((interlace == NoInterlace) && (vertical_factor == 2)))
    {
      interlace=NoInterlace;    /* CCIR 4:2:2 */
      if (vertical_factor == 2)
        interlace=PlaneInterlace; /* CCIR 4:1:1 */
    }
  if (interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"Unable to open file",image);
    }
  else
    {
      AppendImageFormat("Y",image->filename);
      status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
      if (status == False)
        ThrowWriterException(FileOpenError,"Unable to open file",image);
    }
  scene=0;
  do
  {
    /*
      Sample image to an even width and height, if necessary.
    */
    image->depth=8;
    (void) TransformRGBImage(image,RGBColorspace);
    width=image->columns+(image->columns & (horizontal_factor-1));
    height=image->rows+(image->rows & (vertical_factor-1));
    clone_image=CloneImage(image,0,0,True,&image->exception);
    if (clone_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitError,"Unable to resize image",image);
    if (clone_image->storage_class == PseudoClass)
      clone_image->filter=PointFilter;
    yuv_image=ZoomImage(clone_image,width,height,&image->exception);
    DestroyImage(clone_image);
    if (yuv_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitError,"Unable to resize image",image);
    (void) RGBTransformImage(yuv_image,YCbCrColorspace);
    /*
      Downsample image.
    */
    clone_image=CloneImage(image,0,0,True,&image->exception);
    if (clone_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitError,"Unable to resize image",image);
    if (clone_image->storage_class == PseudoClass)
      clone_image->filter=PointFilter;
    chroma_image=ZoomImage(clone_image,width/horizontal_factor,
      height/vertical_factor,&image->exception);
    DestroyImage(clone_image);
    if (chroma_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitError,"Unable to resize image",image);
    (void) RGBTransformImage(chroma_image,YCbCrColorspace);
    if (interlace == NoInterlace)
      {
        /* Write noninterlaced YUV. */
        for (y=0; y < (long) yuv_image->rows; y++)
        {
          p=AcquireImagePixels(yuv_image,0,y,yuv_image->columns,1,
            &yuv_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          s=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (s == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) yuv_image->columns; x++)
          {
            (void) WriteBlobByte(image,Downscale(s->green));
            (void) WriteBlobByte(image,Downscale(p->red));
            p++;
            (void) WriteBlobByte(image,Downscale(s->blue));
            (void) WriteBlobByte(image,Downscale(p->red));
            p++;
            s++;
            x++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        DestroyImage(yuv_image);
      }
    else
      {
        /*
          Initialize Y channel.
        */
        for (y=0; y < (long) yuv_image->rows; y++)
        {
          p=AcquireImagePixels(yuv_image,0,y,yuv_image->columns,1,
            &yuv_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) yuv_image->columns; x++)
          {
            (void) WriteBlobByte(image,Downscale(p->red));
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        DestroyImage(yuv_image);
        /*
          Initialize U channel.
        */
        if (interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("U",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,"Unable to open file",image);
          }
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          p=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            (void) WriteBlobByte(image,Downscale(p->green));
            p++;
          }
        }
        /*
          Initialize V channel.
        */
        if (interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("V",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
            if (status == False)
              ThrowWriterException(FileOpenError,"Unable to open file",image);
          }
        for (y=0; y < (long) chroma_image->rows; y++)
        {
          p=AcquireImagePixels(chroma_image,0,y,chroma_image->columns,1,
            &chroma_image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          for (x=0; x < (long) chroma_image->columns; x++)
          {
            (void) WriteBlobByte(image,Downscale(p->blue));
            p++;
          }
        }
      }
    DestroyImage(chroma_image);
    if (interlace == PartitionInterlace)
      (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetImageListSize(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
