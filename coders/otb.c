/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                              OOO   TTTTT  BBBB                              %
%                             O   O    T    B   B                             %
%                             O   O    T    BBBB                              %
%                             O   O    T    B   B                             %
%                              OOO     T    BBBB                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               January 2000                                  %
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
  WriteOTBImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d O T B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadOTBImage reads a on-the-air (level 0) bitmap and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadOTBImage method is:
%
%      Image *ReadOTBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadOTBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadOTBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    byte;

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

  unsigned char
    bit,
    depth;

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
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  (void) ReadBlobByte(image);
  image->columns=ReadBlobByte(image);
  image->rows=ReadBlobByte(image);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageWarning,"Not a OTB image file",image);
  depth=ReadBlobByte(image);
  if (depth != 1)
    ThrowReaderException(CorruptImageWarning,"Only OTB level 0 files supported",
      image);
  if (!AllocateImageColormap(image,2))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Convert bi-level image to pixel packets.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (bit == 0)
        {
          byte=ReadBlobByte(image);
          if (byte == EOF)
            ThrowReaderException(CorruptImageWarning,"Corrupt OTB image",
              image);
        }
      indexes[x]=(byte & (0x01 << (7-bit))) ? 1 : 0;
      bit++;
      if (bit == 8)
        bit=0;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  SyncImage(image);
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
%   R e g i s t e r O T B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterOTBImage adds attributes for the OTB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterOTBImage method is:
%
%      RegisterOTBImage(void)
%
*/
ModuleExport void RegisterOTBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("OTB");
  entry->decoder=ReadOTBImage;
  entry->encoder=WriteOTBImage;
  entry->adjoin=False;
  entry->description=AllocateString("On-the-air bitmap");
  entry->module=AllocateString("OTB");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r O T B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterOTBImage removes format registrations made by the
%  OTB module from the list of supported formats.
%
%  The format of the UnregisterOTBImage method is:
%
%      UnregisterOTBImage(void)
%
*/
ModuleExport void UnregisterOTBImage(void)
{
  (void) UnregisterMagickInfo("OTB");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e O T B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteOTBImage writes an image to a file in the On-the-air Bitmap
%  (level 0) image format.
%
%  The format of the WriteOTBImage method is:
%
%      unsigned int WriteOTBImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteOTBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteOTBImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  unsigned char
    bit,
    byte,
    polarity;

  unsigned int
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
    Convert image to a bi-level image.
  */
  if (!IsMonochromeImage(image,&image->exception))
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=2;
      quantize_info.dither=image_info->dither;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
    }
  polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
  if (image->colors == 2)
    polarity=Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
  (void) WriteBlobMSBShort(image,0);
  (void) WriteBlobMSBShort(image,image->columns);
  (void) WriteBlobMSBShort(image,image->rows);
  (void) WriteBlobMSBShort(image,1);  /* depth */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (indexes[x] == polarity)
        byte|=0x1 << (7-bit);
      bit++;
      if (bit == 8)
        {
          (void) WriteBlobByte(image,byte);
          bit=0;
          byte=0;
        }
    }
    if (bit != 0)
      (void) WriteBlobByte(image,byte);
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  CloseBlob(image);
  return(True);
}
