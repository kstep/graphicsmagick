/*
% Copyright (C) 2003 GraphicsMagick Group
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
%                            DDDD   PPPP   X   X                              %
%                            D   D  P   P   X X                               %
%                            D   D  PPPP    XXX                               %
%                            D   D  P       X X                               %
%                            DDDD   P      X   X                              %
%                                                                             %
%                                                                             %
%                     Read/Write SMTPE DPX Image Format.                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                March 2001                                   %
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
#include "magick/cache.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

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
%    o image_info: Specifies a pointer to a ImageInfo structure.
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

  long
    y;

  register long
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
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read DPX image.
  */
  count=ReadBlob(image,4,(char *) magick);
  if ((count == 0) || ((LocaleNCompare((char *) magick,"SDPX",4) != 0) &&
      (LocaleNCompare((char *) magick,"XPDS",4) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  headersize=ReadBlobMSBLong(image); 
  for (i=0; i < 764; i++)
    (void) ReadBlobByte(image);
  image->columns= ReadBlobMSBLong(image);
  image->rows= ReadBlobMSBLong(image);
  for (i=0; i < 20; i++)
    (void) ReadBlobByte(image);
  colortype=ReadBlobByte(image);
  (void) ReadBlobByte(image);
  (void) ReadBlobByte(image);
  image->depth=ReadBlobByte(image) > 8 ? 16  : 8;
  for (i=0; i < (long) (headersize-804); i++)
    (void) ReadBlobByte(image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  /*
    Convert DPX raster image to pixel packets.
  */
  switch ((int) colortype)
  {
    case (MonoColorType):
    {
      q=SetImagePixels(image,0,0,image->columns,image->rows);
      for (x=0; x < (long) ((image->columns*image->rows)/3); x++)
      {
        pixel=ReadBlobMSBLong(image);
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 0) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 10) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 20) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
      }
      break;
    }
    case (RGBColorType):
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          pixel=ReadBlobMSBLong(image);
          q->red=(Quantum) ((double) MaxRGB*((pixel >> 22) & 0x3ff)/1023+0.5);
          q->green=(Quantum) ((double) MaxRGB*((pixel >> 12) & 0x3ff)/1023+0.5);
          q->blue=(Quantum) ((double) MaxRGB*((pixel >> 2) & 0x3ff)/1023+0.5);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(LoadImageText,y,image->rows,exception))
              break;
      }
      break;
    }
    default:
      ThrowReaderException(CoderError,ColorTypeNotSupported,image)
  }
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
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
  entry->decoder=(DecoderHandler) ReadDPXImage;
  entry->encoder=(EncoderHandler) WriteDPXImage;
  entry->magick=(MagickHandler) IsDPX;
  entry->description=AcquireString("Digital Moving Picture Exchange");
  entry->module=AcquireString("DPX");
  (void) RegisterMagickInfo(entry);
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
  (void) UnregisterMagickInfo("DPX");
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
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;
  
  register long
    i,
    x;

  unsigned int
    status;
 
  unsigned long
    pixel;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  TransformColorspace(image,RGBColorspace);
  (void) WriteBlobMSBLong(image,0x53445058);
  (void) WriteBlobMSBLong(image,0x2000);
  (void) WriteBlobMSBLong(image,0x56312E30);
  (void) WriteBlobMSBLong(image,0x00000000);
  (void) WriteBlobMSBLong(image,4*image->columns*image->rows+0x2000);
  (void) WriteBlobMSBLong(image,0x00000001);
  (void) WriteBlobMSBLong(image,0x00000680);
  (void) WriteBlobMSBLong(image,0x00000180);
  (void) WriteBlobMSBLong(image,0x00001800);
  for (i=0; i < 124; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobMSBLong(image,0x496D6167);
  (void) WriteBlobMSBLong(image,0x654D6167);
  (void) WriteBlobMSBLong(image,0x69636B20);
  for (i=0; i < 599; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x01);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows);
  for (i=0; i < 20; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,RGBColorType);
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,10);  /* bit depth */
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x01);
  for (i=0; i < (0x2000-806); i++)
    (void) WriteBlobByte(image,0x00);
  /*
    Convert pixel packets to DPX raster image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      pixel=(((long) ((1023L*p->red+MaxRGB/2)/MaxRGB) & 0x3ff) << 22) |
        (((long) ((1023L*p->green+MaxRGB/2)/MaxRGB) &0x3ff) << 12) |
        (((long) ((1023L*p->blue+MaxRGB/2)/MaxRGB) &0x3ff) << 2);
      (void) WriteBlobMSBLong(image,pixel);
      p++;
    }
  }
  CloseBlob(image);  
  return(status);
}
