/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M   OOO   N   N   OOO                            %
%                        MM MM  O   O  NN  N  O   O                           %
%                        M M M  O   O  N N N  O   O                           %
%                        M   M  O   O  N  NN  O   O                           %
%                        M   M   OOO   N   N   OOO                            %
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
  WriteMONOImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M O N O I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMONOImage reads an image of raw bites in LSB order and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadMONOImage method is:
%
%      Image *ReadMONOImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMONOImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadMONOImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

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
    byte;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < image->offset; i++)
    (void) ReadBlobByte(image);
  /*
    Initialize image colormap.
  */
  if (!AllocateImageColormap(image,2))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
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
        byte=ReadBlobByte(image);
      indexes[x]=(byte & 0x01) ? 0 : 1;
      bit++;
      if (bit == 8)
        bit=0;
      byte>>=1;
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
%   R e g i s t e r M O N O I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMONOImage adds attributes for the MONO image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMONOImage method is:
%
%      RegisterMONOImage(void)
%
*/
ModuleExport void RegisterMONOImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MONO");
  entry->decoder=ReadMONOImage;
  entry->encoder=WriteMONOImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Bi-level bitmap in least-significant-byte first order");
  entry->module=AllocateString("MONO");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M O N O I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMONOImage removes format registrations made by the
%  MONO module from the list of supported formats.
%
%  The format of the UnregisterMONOImage method is:
%
%      UnregisterMONOImage(void)
%
*/
ModuleExport void UnregisterMONOImage(void)
{
  (void) UnregisterMagickInfo("MONO");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M O N O I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMONOImage writes an image of raw bits in LSB order to a file.
%
%  The format of the WriteMONOImage method is:
%
%      unsigned int WriteMONOImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMONOImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMONOImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *p;

  unsigned char
    bit,
    byte,
    polarity;

  unsigned int
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Convert image to a bi-level image.
  */
  if (!IsMonochromeImage(image))
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
    polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
  for (y=0; y < (long) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      byte>>=1;
      if (indexes[x] == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          (void) WriteBlobByte(image,byte);
          bit=0;
          byte=0;
        }
    }
    if (bit != 0)
      (void) WriteBlobByte(image,byte >> (8-bit));
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  CloseBlob(image);
  return(True);
}
