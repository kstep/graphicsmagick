/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%       H   H  IIIII  SSSSS  TTTTT   OOO    GGGG  RRRR    AAA   M   M         %
%       H   H    I    SS       T    O   O  G      R   R  A   A  MM MM         %
%       HHHHH    I     SSS     T    O   O  G  GG  RRRR   AAAAA  M M M         %
%       H   H    I       SS    T    O   O  G   G  R R    A   A  M   M         %
%       H   H  IIIII  SSSSS    T     OOO    GGG   R  R   A   A  M   M         %
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
  WriteHISTOGRAMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r H I S T O G R A M I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterHISTOGRAMImage adds attributes for the Histogram image format
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterHISTOGRAMImage method is:
%
%      RegisterHISTOGRAMImage(void)
%
*/
ModuleExport void RegisterHISTOGRAMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HISTOGRAM");
  entry->encoder=WriteHISTOGRAMImage;
  entry->adjoin=False;
  entry->description=AllocateString("Histogram of the image");
  entry->module=AllocateString("HISTOGRAM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r H I S T O G R A M I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterHISTOGRAMImage removes format registrations made by the
%  HISTOGRAM module from the list of supported formats.
%
%  The format of the UnregisterHISTOGRAMImage method is:
%
%      UnregisterHISTOGRAMImage(void)
%
*/
ModuleExport void UnregisterHISTOGRAMImage(void)
{
  (void) UnregisterMagickInfo("HISTOGRAM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H I S T O G R A M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHISTOGRAMImage writes an image to a file in Histogram format.
%  The image shows a histogram of the color (or gray) values in the image.  The
%  image consists of three overlaid histograms:  a red one for the red channel,
%  a green one for the green channel, and a blue one for the blue channel.  The
%  image comment contains a list of unique pixel values and the number of times
%  each occurs in the image.
%
%  This method is strongly based on a similar one written by
%  muquit@warm.semcor.com which in turn is based on ppmhistmap of netpbm.
%
%  The format of the WriteHISTOGRAMImage method is:
%
%      unsigned int WriteHISTOGRAMImage(const ImageInfo *image_info,
%        Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHISTOGRAMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteHISTOGRAMImage(const ImageInfo *image_info,
  Image *image)
{
#define HistogramDensity  "256x200"

  char
    filename[MaxTextExtent];

  double
    scale;

  FILE
    *file;

  Image
    *histogram_image;

  int
    *blue,
    *green,
    maximum,
    *red;

  long
    sans_offset,
    y;

  register PixelPacket
    *p,
    *q;

  register long
    x;

  unsigned int
    status;

  unsigned long
    height,
    width;

  /*
    Allocate histogram image.
  */
  width=image->columns;
  height=image->rows;
  if (image_info->density != (char *) NULL)
    (void) ParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &width,&height);
  else
    (void) ParseGeometry(HistogramDensity,&sans_offset,&sans_offset,
      &width,&height);
  histogram_image=CloneImage(image,width,height,True,&image->exception);
  if (histogram_image == (Image *) NULL)
      ThrowWriterException(ResourceLimitWarning,
        "Memory allocation failed",image);
  histogram_image->storage_class=DirectClass;
  /*
    Allocate histogram count arrays.
  */
  red=(int *) AcquireMemory (histogram_image->columns*sizeof(int));
  green=(int *) AcquireMemory (histogram_image->columns*sizeof(int));
  blue=(int *) AcquireMemory (histogram_image->columns*sizeof(int));
  if ((red == (int *) NULL) || (green == (int *) NULL) ||
      (blue == (int *) NULL))
    {
      DestroyImage(histogram_image);
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image)
    }
  /*
    Initialize histogram count arrays.
  */
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    red[x]=0;
    green[x]=0;
    blue[x]=0;
  }
  for (y=0; y < (long) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      red[DownScale(p->red)]++;
      green[DownScale(p->green)]++;
      blue[DownScale(p->blue)]++;
      p++;
    }
  }
  maximum=0;
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    if (maximum < red[x])
      maximum=red[x];
    if (maximum < green[x])
      maximum=green[x];
    if (maximum < blue[x])
      maximum=blue[x];
  }
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    if (red[x] > maximum)
      red[x]=maximum;
    if (green[x] > maximum)
      green[x]=maximum;
    if (blue[x] > maximum)
      blue[x]=maximum;
  }
  scale=(double) histogram_image->rows/maximum;
  /*
    Initialize histogram image.
  */
  (void) QueryColorDatabase("black",&histogram_image->background_color);
  SetImage(histogram_image,OpaqueOpacity);
  for (x=0; x < (long) histogram_image->columns; x++)
  {
    q=GetImagePixels(histogram_image,x,0,1,histogram_image->rows);
    if (q == (PixelPacket *) NULL)
      break;
    y=histogram_image->rows-(int) (scale*red[x]);
    p=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      p->red=MaxRGB;
      p++;
    }
    y=histogram_image->rows-(int) (scale*green[x]);
    p=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      p->green=MaxRGB;
      p++;
    }
    y=histogram_image->rows-(int) (scale*blue[x]);
    p=q+y;
    for ( ; y < (long) histogram_image->rows; y++)
    {
      p->blue=MaxRGB;
      p++;
    }
    if (!SyncImagePixels(histogram_image))
      break;
    if (QuantumTick(x,histogram_image->columns))
      MagickMonitor(SaveImageText,x,histogram_image->columns);
  }
  /*
    Free memory resources.
  */
  LiberateMemory((void **) &blue);
  LiberateMemory((void **) &green);
  LiberateMemory((void **) &red);
  TemporaryFilename(filename);
  file=fopen(filename,WriteBinaryType);
  if (file != (FILE *) NULL)
    {
      char
        command[MaxTextExtent];

      /*
        Add a histogram as an image comment.
      */
      (void) GetNumberColors(image,file,&image->exception);
      (void) fclose(file);
      FormatString(command,"@%.1024s",filename);
      (void) SetImageAttribute(histogram_image,"comment",command);
      (void) remove(filename);
    }
  /*
    Write Histogram image as MIFF.
  */
  (void) strncpy(filename,histogram_image->filename,MaxTextExtent-1);
  (void) FormatString(histogram_image->filename,"miff:%.1024s",filename);
  status=WriteImage(image_info,histogram_image);
  DestroyImage(histogram_image);
  return(status);
}
