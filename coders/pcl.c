/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP    CCCC  L                                  %
%                            P   P  C      L                                  %
%                            PPPP   C      L                                  %
%                            P      C      L                                  %
%                            P       CCCC  LLLLL                              %
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
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated	      %
%  to making software imaging solutions freely available.		      %
%									      %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("Open	      %
%  ImageMagick"), to deal in GraphicsMagick without restriction,	      %
%  including without limitation the rights to use, copy, modify, merge,	      %
%  publish, distribute, sublicense, and/or sell copies of ImageMagick, and    %
%  to permit persons to whom GraphicsMagick is furnished to do so,	      %
%  subject to the following conditions:					      %
%									      %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.		      %
%									      %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of		      %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,	      %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick	      %
%  or the use or other dealings in GraphicsMagick.			      %
%									      %
%  Except as contained in this notice, the name of the GraphicsMagick	      %
%  Group shall not be used in advertising or otherwise to promote the	      %
%  sale, use or other dealings in GraphicsMagick without prior written	      %
%  authorization from the GraphicsMagick Group.				      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "cache.h"
#include "color.h"
#include "magick.h"
#include "monitor.h"
#include "static.h"
#include "utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WritePCLImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P C L                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPCL returns True if the image format type, identified by the
%  magick string, is PCL.
%
%  The format of the IsPCL method is:
%
%      unsigned int IsPCL(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPCL returns True if the image format type is PCL.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPCL(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\033E\033",3) == 0)
    return(True);
  if (memcmp(magick,"\033E\033&",4) == 0)
    return(False);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P C L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPCLImage adds attributes for the PCL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPCLImage method is:
%
%      RegisterPCLImage(void)
%
*/
ModuleExport void RegisterPCLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PCL");
  entry->encoder=(EncoderHandler) WritePCLImage;
  entry->magick=(MagickHandler) IsPCL;
  entry->adjoin=False;
  entry->description=AcquireString("Page Control Language");
  entry->module=AcquireString("PCL");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P C L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPCLImage removes format registrations made by the
%  PCL module from the list of supported formats.
%
%  The format of the UnregisterPCLImage method is:
%
%      UnregisterPCLImage(void)
%
*/
ModuleExport void UnregisterPCLImage(void)
{
  (void) UnregisterMagickInfo("PCL");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCLImage writes an image in the Page Control Language encoded
%  image format.
%
%  The format of the WritePCLImage method is:
%
%      unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
%
*/
static unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[MaxTextExtent];

  long
    sans,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register unsigned char
    *q;

  unsigned int
    status;

  unsigned long
    density;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  (void) TransformRGBImage(image,image->colorspace);
  /*
    Initialize the printer.
  */
  (void) WriteBlobString(image,"\033E");  /* printer reset */
  (void) WriteBlobString(image,"\033&l0E");  /* top margin 0 */
  (void) WriteBlobString(image,buffer);
  (void) GetGeometry("75x75",&sans,&sans,&density,&density);
  if (image_info->density != (char *) NULL)
    (void) GetGeometry(image_info->density,&sans,&sans,&density,&density);
  if (IsMonochromeImage(image,&image->exception))
    {
      register unsigned char
        bit,
        byte;

      /*
        Write PCL monochrome image.
      */
      FormatString(buffer,"\033*t%ldR",density);  /* set resolution */
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033*r1A");  /* start graphics */
      (void) WriteBlobString(image,"\033*b0M");  /* no compression */
      FormatString(buffer,"\033*b%luW",(image->columns+7)/8);
      (void) WriteBlobString(image,buffer);
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
          byte<<=1;
          byte|=indexes[x] ? 0x01 : 0x00;
          bit++;
          if (bit == 8)
            {
              (void) WriteBlobByte(image,byte);
              bit=0;
              byte=0;
            }
        }
        if (bit != 0)
          (void) WriteBlobByte(image,byte << (8-bit));
        if (y < (long) image->rows)
          {
            FormatString(buffer,"\033*b%luW",(image->columns+7)/8);
            (void) WriteBlobString(image,buffer);
          }
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
            break;
      }
      (void) WriteBlobString(image,"\033*rB");  /* end graphics */
    }
  else
    {
      static char
        color_mode[6] = { 0, 3, 0, 8, 8, 8 };

      unsigned char
        *pixels;

      /*
        Allocate pixel buffers.
      */
      pixels=(unsigned char *)
        AcquireMemory(3*image->columns*sizeof(unsigned char));
      if (pixels == (unsigned char *) NULL)
        ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image)
      /*
        Write PCL color image.
      */
      (void) WriteBlobString(image,"\033*r3F");  /* set presentation mode */
      FormatString(buffer,"\033*t%luR",density);  /* set resolution */
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*r%luT",image->rows);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*r%luS",image->columns);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033*v6W");  /* set color mode */
      (void) WriteBlob(image,6,color_mode);
      (void) WriteBlobString(image,"\033*r1A");  /* start raster graphics */
      (void) WriteBlobString(image,"\033*b0Y");  /* set y offset */
      (void) WriteBlobString(image,"\033*b0M");  /* no compression */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        q=pixels;
        for (x=0; x < (long) image->columns; x++)
        {
          *q++=ScaleQuantumToChar(p->red);
          *q++=ScaleQuantumToChar(p->green);
          *q++=ScaleQuantumToChar(p->blue);
          p++;
        }
        FormatString(buffer,"\033*b%luW",3*image->columns);  /* send row */
        (void) WriteBlobString(image,buffer);
        (void) WriteBlob(image,3*image->columns,pixels);
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
            break;
      }
      (void) WriteBlobString(image,"\033*r0C");  /* end graphics */
      LiberateMemory((void **) &pixels);
    }
  (void) WriteBlobString(image,"\033E");
  CloseBlob(image);
  return(True);
}
