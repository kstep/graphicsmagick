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
  if (memcmp(magick,"\033E\033&",4) == 0)
    return(False);
  if (memcmp(magick,"\033E\033",3) == 0)
    return(True);
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
  entry->encoder=WritePCLImage;
  entry->magick=IsPCL;
  entry->adjoin=False;
  entry->description=AllocateString("Page Control Language");
  entry->module=AllocateString("PCL");
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
    buffer[MaxTextExtent],
    geometry[MaxTextExtent];

  ImageAttribute
    *attribute;

  long
    sans_offset,
    x,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  RectangleInfo
    media_info;

  unsigned int
    page_size,
    status,
    text_size;

  unsigned long
    density,
    height,
    width;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Initialize the printer.
  */
  (void) WriteBlobString(image,"\033E");  /* portrait orientation */
  (void) WriteBlobString(image,"\033&l0O");  /* portrait orientation */
  (void) WriteBlobString(image,"\033&l0E");  /* top margin 0 */
  /*
    Center image on PCL page.
  */
  text_size=0;
  attribute=GetImageAttribute(image,"label");
  if (attribute != (ImageAttribute *) NULL)
    text_size=(unsigned int)
      (MultilineCensus(attribute->value)*image_info->pointsize+12);
  width=image->columns;
  height=image->rows;
  x=0;
  y=text_size;
  FormatString(geometry,"%lux%lu",image->columns,image->rows);
  if (image_info->page != (char *) NULL)
    (void) strncpy(geometry,image_info->page,MaxTextExtent-1);
  else
    if ((image->page.width != 0) && (image->page.height != 0))
      (void) FormatString(geometry,"%ux%u%+d%+d",image->page.width,
        image->page.height,image->page.x,image->page.y);
    else
      if (LocaleCompare(image_info->magick,"PCL") == 0)
        (void) strcpy(geometry,PSPageGeometry);
  (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,
    &media_info.width,&media_info.height);
  page_size=2;
  if ((media_info.width == 540) && (media_info.height == 720))
    page_size=1;  /* executive */
  if ((media_info.width == 612) && (media_info.height == 792))
    page_size=2;  /* letter */
  if ((media_info.width == 612) && (media_info.height == 1008))
    page_size=3;  /* legal */
  if ((media_info.width == 1224) && (media_info.height == 792))
    page_size=6;  /* ledger */
  if ((media_info.width == 595) && (media_info.height == 842))
    page_size=26;  /* A4 */
  if ((media_info.width == 842) && (media_info.height == 1191))
    page_size=27;  /* A3 */
  if ((media_info.width == 729) && (media_info.height == 1032))
    page_size=45;  /* B5 */
  if ((media_info.width == 516) && (media_info.height == 729))
    page_size=46;  /* B4 */
  FormatString(buffer,"\033&l%uA",page_size);  /* papersize */
  (void) WriteBlobString(image,buffer);
  density=72;
  if (image_info->density != (char *) NULL)
    (void) ParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &density,&density);
  else
    (void) ParseGeometry("75x75",&sans_offset,&sans_offset,&density,&density);
  FormatString(buffer,"\033*p%ldx%ldY",x,y);
  (void) WriteBlobString(image,buffer);
  attribute=GetImageAttribute(image,"label");
  if (attribute != (ImageAttribute *) NULL)
    {
      /*
        Print label.
      */
      (void) WriteBlobString(image,"\033&k2G");
      FormatString(buffer,"\033(s1p%uv5t3b",(unsigned int)
        image_info->pointsize);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\n%.1024s\n",attribute->value);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033(s0B");
    }
  FormatString(buffer,"\033*t%uR",density);  /* graphic resolution */
  (void) WriteBlobString(image,buffer);
  width=(density*width)/75;
  height=(density*height)/75;
  if ((image->storage_class == DirectClass) ||
      !IsGrayImage(image,&image->exception))
    {
      /*
        Write PCL color image.
      */
      FormatString(buffer,"\033*r%lus%luT",image->columns,image->rows);
      (void) WriteBlobString(image,buffer);
      FormatString(buffer,"\033*t%luh%luV",width,height);
      (void) WriteBlobString(image,buffer);
      (void) WriteBlobString(image,"\033*v6W");
      (void) WriteBlobByte(image,'\000');  /* color model */
      (void) WriteBlobByte(image,'\003');  /* direct pixel encoding */
      (void) WriteBlobByte(image,'\000');  /* bits per index */
      (void) WriteBlobByte(image,'\010');  /* bits red*/
      (void) WriteBlobByte(image,'\010');  /* bits green*/
      (void) WriteBlobByte(image,'\010');  /* bits blue */
      (void) WriteBlobString(image,"\033*r2A");   /* start graphics */
      (void) WriteBlobString(image,"\033*b0M");  /* no compression */
      FormatString(buffer,"\033*b%luW",3*image->columns);
      (void) WriteBlobString(image,buffer);
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          FormatString(buffer,"%c%c%c",(int) DownScale(p->red),
            (int) DownScale(p->green),(int) DownScale(p->blue));
          (void) WriteBlobString(image,buffer);
          p++;
        }
        FormatString(buffer,"\033*b%luW",3*image->columns);
        (void) WriteBlobString(image,buffer);
        if (QuantumTick(y,image->rows))
          MagickMonitor(SaveImageText,y,image->rows);
      }
      (void) WriteBlobString(image,"\033*rC");  /* end graphics */
    }
  else
    {
      Image
        *monochrome_image;

      register unsigned char
        bit,
        byte,
        polarity;

      /*
        Write PCL monochrome image.
      */
      SetImageType(image,GrayscaleType);
      monochrome_image=image;
      if ((width != image->columns) || (height != image->rows))
        {
          Image
            *clone_image;

          /*
            Scale image.
          */
          clone_image=CloneImage(image,0,0,True,&image->exception);
          if (clone_image == (Image *) NULL)
            ThrowWriterException(ResourceLimitWarning,"Unable to scale image",
              image);
          monochrome_image=
            ZoomImage(clone_image,width,height,&image->exception);
          DestroyImage(clone_image);
          if (monochrome_image == (Image *) NULL)
            ThrowWriterException(ResourceLimitWarning,"Unable to scale image",
              image);
        }
      if (!IsMonochromeImage(monochrome_image,&monochrome_image->exception))
        SetImageType(monochrome_image,BilevelType);
      polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
      if (monochrome_image->colors == 2)
        polarity=Intensity(monochrome_image->colormap[0]) >
          Intensity(monochrome_image->colormap[1]);
      FormatString(buffer,"\033*r%lus%luT",monochrome_image->columns,
        monochrome_image->rows);
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
          if (indexes[x] == polarity)
            byte|=0x01;
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
        if (y < (long) monochrome_image->rows)
          {
            FormatString(buffer,"\033*b%luW",(monochrome_image->columns+7)/8);
            (void) WriteBlobString(image,buffer);
          }
        if (QuantumTick(y,monochrome_image->rows))
          MagickMonitor(SaveImageText,y,monochrome_image->rows);
      }
      (void) WriteBlobString(image,"\033*rB");  /* end graphics */
      if (image != monochrome_image)
        DestroyImage(monochrome_image);
    }
  (void) WriteBlobString(image,"\033&l0H");
  (void) WriteBlobString(image,"\033E");  /* portrait orientation */
  CloseBlob(image);
  return(True);
}
