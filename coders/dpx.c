/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            DDDD   PPPP   X   X                              %
%                            D   D  P   P   X X                               %
%                            D   D  PPPP    XXX                               %
%                            D   D  P       X X                               %
%                            DDDD   P      X   X                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                March 2001                                   %
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
  Forward declaractions.
*/
static unsigned int
  WriteDPXImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D P X                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDPX returns True if the image format type, identified by the
%  magick string, is DPX.
%
%  The format of the IsDPX method is:
%
%      unsigned int IsDPX(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDPX returns True if the image format type is DPX.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsDPX(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"SDPX",4) == 0)
    return(True);
  if (memcmp(magick,"XPDS",4) == 0)
    return(True);
  return(False);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D P X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDPXImage reads an DPX X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadDPXImage method is:
%
%      Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDPXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MonoColorType  6
#define RGBColorType  50

  char
    magick[4];

  Image
    *image;

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    count;    

  unsigned char
    colortype;    

  unsigned int
    status;

  unsigned long
    headersize,
    pixel;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read DPX image.
  */
  count=ReadBlob(image,4,(char *) magick);
  if ((count == 0) || ((LocaleNCompare((char *) magick,"SDPX",4) != 0) &&
      (LocaleNCompare((char *) magick,"XPDS",4) != 0)))
    ThrowReaderException(CorruptImageWarning,"Not a DPX image file",image);
  headersize=ReadBlobMSBLong(image); 
  for (i=0; i < 764; i++)
    (void) ReadBlobByte(image);
  image->columns=(unsigned int) ReadBlobMSBLong(image);
  image->rows=(unsigned int) ReadBlobMSBLong(image);
  for (i=0; i < 20; i++)
    (void) ReadBlobByte(image);
  colortype=ReadBlobByte(image);
  (void) ReadBlobByte(image);
  (void) ReadBlobByte(image);
  image->depth=ReadBlobByte(image) > 8 ? 16  : 8;
  for (i=0; i < (headersize-804); i++)
    (void) ReadBlobByte(image);
  /*
    Convert DPX raster image to pixel packets.
  */
  switch (colortype)
  {
    case (MonoColorType):
    {
      q=SetImagePixels(image,0,0,image->columns,image->rows);
      for (x=0; x < (int) ((image->columns*image->rows)/3); x++)
      {
        pixel=ReadBlobMSBLong(image);
        q->red=MaxRGB*((pixel >> 0) & 0x3ff)/1023;
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=MaxRGB*((pixel >> 10) & 0x3ff)/1023;
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=MaxRGB*((pixel >> 20) & 0x3ff)/1023;
        q->green=q->red;
        q->blue=q->red;
        q++;
      }
      break;
    }
    case (RGBColorType):
    {
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          pixel=ReadBlobMSBLong(image);
          q->red=MaxRGB*((pixel >> 22) & 0x3ff)/1023;
          q->green=MaxRGB*((pixel >> 12) & 0x3ff)/1023;
          q->blue=MaxRGB*((pixel >> 2) & 0x3ff)/1023;     
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    default:
      ThrowReaderException(CorruptImageWarning,"color type not supported",
        image)
  }
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
%   R e g i s t e r D P X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDPXImage adds attributes for the DPX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDPXImage method is:
%
%      RegisterDPXImage(void)
%
*/
ModuleExport void RegisterDPXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DPX");
  entry->decoder=ReadDPXImage;
  entry->encoder=WriteDPXImage;
  entry->magick=IsDPX;
  entry->description=AllocateString("Digital Moving Picture Exchange");
  entry->module=AllocateString("DPX");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D P X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDPXImage removes format registrations made by the
%  DPX module from the list of supported formats.
%
%  The format of the UnregisterDPXImage method is:
%
%      UnregisterDPXImage(void)
%
*/
ModuleExport void UnregisterDPXImage(void)
{
  UnregisterMagickInfo("DPX");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e D P X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteDPXImage writes an image in DPX encoded image format.
%
%  The format of the WriteDPXImage method is:
%
%      unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteDPXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;
  
  unsigned long
    pixel;

  unsigned int
    status;
 
  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  WriteBlobMSBLong(image,0x53445058);
  WriteBlobMSBLong(image,0x2000);
  WriteBlobMSBLong(image,0x56312E30);
  WriteBlobMSBLong(image,0x00000000);
  WriteBlobMSBLong(image,4*image->columns*image->rows+0x2000);
  WriteBlobMSBLong(image,0x00000001);
  WriteBlobMSBLong(image,0x00000680);
  WriteBlobMSBLong(image,0x00000180);
  WriteBlobMSBLong(image,0x00001800);
  for (i=0; i < 124; i++)
    WriteBlobByte(image,0x00);
  WriteBlobMSBLong(image,0x496D6167);
  WriteBlobMSBLong(image,0x654D6167);
  WriteBlobMSBLong(image,0x69636B20);
  for (i=0; i < 600; i++)
    WriteBlobByte(image,0x00);
  WriteBlobMSBLong(image,image->columns);
  WriteBlobMSBLong(image,image->rows);
  for (i=0; i < 20; i++)
    WriteBlobByte(image,0x00);
  WriteBlobByte(image,RGBColorType);
  WriteBlobByte(image,0x00);
  WriteBlobByte(image,0x00);
  WriteBlobByte(image,10);  /* bit depth */
  for (i=0; i < (0x2000-804); i++)
    WriteBlobByte(image,0x00);
  /*
    Convert pixel packets to DPX raster image .
  */
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      pixel=((1023*XUpScale(q->red)/MaxRGB) << 22) |
        ((1023*XUpScale(q->green)/MaxRGB) << 12) |
        ((1023*XUpScale(q->blue)/MaxRGB) << 2);
      WriteBlobMSBLong(image,pixel);
      q++;
    }
  }
  CloseBlob(image);  
  return(status);
}
