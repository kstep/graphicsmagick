/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        U   U  Y   Y  V   V  Y   Y                           %
%                        U   U   Y Y   V   V   Y Y                            %
%                        U   U    Y    V   V    Y                             %
%                        U   U    Y     V V     Y                             %
%                         UUU     Y      V      Y                             %
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
  WriteUYVYImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d U Y V Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadUYVYImage reads an image in the UYVY format and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadUYVYImage method is:
%
%      Image *ReadUYVYImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadUYVYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadUYVYImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
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

  unsigned char
    u,
    v,
    y1,
    y2;

  unsigned int
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
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image)
  for (i=0; i < image->offset; i++)
    (void) ReadBlobByte(image);
  image->depth=8;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Accumulate UYVY, then unpack into two pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) (image->columns >> 1); x++)
    {
      u=ReadBlobByte(image);
      y1=ReadBlobByte(image);
      v=ReadBlobByte(image);
      y2=ReadBlobByte(image);
      q->red=UpScale(y1);
      q->green=UpScale(u);
      q->blue=UpScale(v);
      q++;
      q->red=UpScale(y2);
      q->green=UpScale(u);
      q->blue=UpScale(v);
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  (void) TransformRGBImage(image,YCbCrColorspace);
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
%   R e g i s t e r U Y V Y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterUYVYImage adds attributes for the UYVY image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterUYVYImage method is:
%
%      RegisterUYVYImage(void)
%
*/
ModuleExport void RegisterUYVYImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PAL");
  entry->decoder=ReadUYVYImage;
  entry->encoder=WriteUYVYImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description=AllocateString("16bit/pixel interleaved YUV");
  entry->module=AllocateString("UYVY");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("UYVY");
  entry->decoder=ReadUYVYImage;
  entry->encoder=WriteUYVYImage;
  entry->adjoin=False;
  entry->raw=True;
  entry->description=AllocateString("16bit/pixel interleaved YUV");
  entry->module=AllocateString("UYVY");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r U Y V Y I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterUYVYImage removes format registrations made by the
%  UYVY module from the list of supported formats.
%
%  The format of the UnregisterUYVYImage method is:
%
%      UnregisterUYVYImage(void)
%
*/
ModuleExport void UnregisterUYVYImage(void)
{
  (void) UnregisterMagickInfo("PAL");
  (void) UnregisterMagickInfo("UYVY");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U Y V Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteUYVYImage writes an image to a file in the digital UYVY
%  format.  This format, used by AccomWSD, is not dramatically higher quality
%  than the 12bit/pixel YUV format, but has better locality.
%
%  The format of the WriteUYVYImage method is:
%
%      unsigned int WriteUYVYImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUYVYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%      Implicit assumption: number of columns is even.
%
*/
static unsigned int WriteUYVYImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  unsigned int
    full,
    status,
    u,
    v,
    y1;

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
    Convert to YUV, at full resolution.
  */
  (void) RGBTransformImage(image,YCbCrColorspace);
  /*
    Accumulate two pixels, then output.
  */
  full=False;
  u=0;
  v=0;
  y1=0;
  full=False;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (full)
        {
          (void) WriteBlobByte(image,DownScale((u+p->green) >> 1));
          (void) WriteBlobByte(image,DownScale(y1));
          (void) WriteBlobByte(image,DownScale((v+p->blue) >> 1));
          (void) WriteBlobByte(image,DownScale(p->red));
          full=False;
        }
      else
        {
          y1=p->red;
          u=p->green;
          v=p->blue;
          full=True;
        }
      p++;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  (void) TransformRGBImage(image,YCbCrColorspace);
  CloseBlob(image);
  return(True);
}
