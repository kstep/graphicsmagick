/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            U   U  IIIII  L                                  %
%                            U   U    I    L                                  %
%                            U   U    I    L                                  %
%                            U   U    I    L                                  %
%                             UUU   IIIII  LLLLL                              %
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
  WriteUILImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r U I L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterUILImage adds attributes for the UIL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterUILImage method is:
%
%      RegisterUILImage(void)
%
*/
ModuleExport void RegisterUILImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("UIL");
  entry->encoder=WriteUILImage;
  entry->adjoin=False;
  entry->description=AllocateString("X-Motif UIL table");
  entry->module=AllocateString("UIL");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r U I L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterUILImage removes format registrations made by the
%  UIL module from the list of supported formats.
%
%  The format of the UnregisterUILImage method is:
%
%      UnregisterUILImage(void)
%
*/
ModuleExport void UnregisterUILImage(void)
{
  (void) UnregisterMagickInfo("UIL");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U I L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteUILImage writes an image to a file in the X-Motif UIL table
%  format.
%
%  The format of the WriteUILImage method is:
%
%      unsigned int WriteUILImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUILImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteUILImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    basename[MaxTextExtent],
    buffer[MaxTextExtent],
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  int
    j,
    y;

  long
    k;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *p;

  register long
    i;

  unsigned int
    characters_per_pixel,
    status,
    transparent;

  unsigned long
    colors,
    number_pixels;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  transparent=False;
  i=0;
  p=(PixelPacket *) NULL;
  if (image->storage_class == PseudoClass)
    colors=image->colors;
  else
    {
      QuantizeInfo
        quantize_info;

      unsigned char
        *matte_image;

      /*
        Convert DirectClass to PseudoClass image.
      */
      matte_image=(unsigned char *) NULL;
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          number_pixels=image->columns*image->rows;
          matte_image=(unsigned char *) AcquireMemory(number_pixels);
          if (matte_image == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitWarning,
              "Memory allocation failed",image);
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              matte_image[i]=p->opacity == TransparentOpacity;
              if (matte_image[i])
                transparent=True;
              i++;
              p++;
            }
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      colors=image->colors;
      if (transparent)
        {
          colors++;
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (int) image->columns; x++)
            {
              if (matte_image[i])
                indexes[x]=image->colors;
              p++;
            }
          }
        }
      if (matte_image != (unsigned char *) NULL)
        LiberateMemory((void **) &matte_image);
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (long) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    UIL header.
  */
  (void) WriteBlobString(image,"/* UIL */\n");
  GetPathComponent(image->filename,BasePath,basename);
  FormatString(buffer,"value\n  %.1024s_ct : color_table(\n",basename);
  (void) WriteBlobString(image,buffer);
  for (i=0; i < (long) colors; i++)
  {
    /*
      Define UIL color.
    */
    (void) QueryColorname(image,image->colormap+i,X11Compliance,name);
    if (transparent)
      if (i == (long) (colors-1))
        (void) strcpy(name,"None");
    /*
      Write UIL color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    if (LocaleCompare(name,"None") == 0)
      FormatString(buffer,"    background color = '%.1024s'",symbol);
    else
      FormatString(buffer,"    color('%.1024s',%.1024s) = '%.1024s'",
        name,Intensity(*p) < (0.5*(MaxRGB+1)) ? "background" : "foreground",
        symbol);
    (void) WriteBlobString(image,buffer);
    FormatString(buffer,"%.1024s",(i == (long) (colors-1) ? ");\n" : ",\n"));
    (void) WriteBlobString(image,buffer);
  }
  /*
    Define UIL pixels.
  */
  GetPathComponent(image->filename,BasePath,basename);
  FormatString(buffer,
    "  %.1024s_icon : icon(color_table = %.1024s_ct,\n",basename,basename);
  (void) WriteBlobString(image,buffer);
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    (void) WriteBlobString(image,"    \"");
    for (x=0; x < (int) image->columns; x++)
    {
      k=indexes[x] % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) indexes[x]-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      FormatString(buffer,"%.1024s",symbol);
      (void) WriteBlobString(image,buffer);
      p++;
    }
    FormatString(buffer,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? ");" : ","));
    (void) WriteBlobString(image,buffer);
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  CloseBlob(image);
  return(True);
}
