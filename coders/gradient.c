/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%            GGGG  RRRR    AAA   DDDD   IIIII  EEEEE  N   N  TTTTT            %
%           G      R   R  A   A  D   D    I    E      NN  N    T              %
%           G  GG  RRRR   AAAAA  D   D    I    EEE    N N N    T              %
%           G   G  R R    A   A  D   D    I    E      N  NN    T              %
%            GGG   R  R   A   A  DDDD   IIIII  EEEEE  N   N    T              %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A D I E N T I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRADIENTImage creates a gradient image and initializes it to
%  the X server color range as specified by the filename.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadGRADIENTImage method is:
%
%      Image *ReadGRADIENTImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRADIENTImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadGRADIENTImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  PixelPacket
    color;

  double
    brightness,
    brightness_step,
    hue,
    hue_step,
    saturation,
    saturation_step;

  Image
    *image;

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  unsigned long
    number_pixels;

  /*
    Initialize Image structure.
  */
  image=AllocateImage(image_info);
  (void) strcpy(image->filename,image_info->filename);
  if (image->columns == 0)
    image->columns=512;
  if (image->rows == 0)
    image->rows=512;
  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  (void) strcpy(colorname,image_info->filename);
  (void) sscanf(image_info->filename,"%[^-]",colorname);
  (void) QueryColorDatabase(colorname,&color);
  (void) TransformHSL(color.red,color.green,color.blue,&hue,&saturation,&brightness);
  (void) strcpy(colorname,"white");
  if (Intensity(color) > (0.5*MaxRGB))
    (void) strcpy(colorname,"black");
  (void) sscanf(image_info->filename,"%*[^-]-%s",colorname);
  (void) QueryColorDatabase(colorname,&color);
  (void) TransformHSL(color.red,color.green,color.blue,&hue_step,&saturation_step,
    &brightness_step);
  number_pixels=image->columns*image->rows;
  hue_step=(hue_step-hue)/number_pixels;
  saturation_step=(saturation_step-saturation)/number_pixels;
  brightness_step=(brightness_step-brightness)/number_pixels;
  /*
    Initialize image pixels.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      HSLTransform(hue,saturation,brightness,&q->red,&q->green,&q->blue);
      q++;
      hue+=hue_step;
      saturation+=saturation_step;
      brightness+=brightness_step;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r G R A D I E N T I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterGRADIENTImage adds attributes for the GRADIENT image format
%  to the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterGRADIENTImage method is:
%
%      RegisterGRADIENTImage(void)
%
*/
ModuleExport void RegisterGRADIENTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("GRADIENT");
  entry->decoder=ReadGRADIENTImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Gradual passing from one shade to another");
  entry->module=AllocateString("GRADIENT");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r G R A D I E N T I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterGRADIENTImage removes format registrations made by the
%  GRADIENT module from the list of supported formats.
%
%  The format of the UnregisterGRADIENTImage method is:
%
%      UnregisterGRADIENTImage(void)
%
*/
ModuleExport void UnregisterGRADIENTImage(void)
{
  (void) UnregisterMagickInfo("GRADIENT");
}
