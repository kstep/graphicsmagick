/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     L       AAA   BBBB   EEEEE  L                           %
%                     L      A   A  B   B  E      L                           %
%                     L      AAAAA  BBBB   EEE    L                           %
%                     L      A   A  B   B  E      L                           %
%                     LLLLL  A   A  BBBB   EEEEE  LLLLL                       %
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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
%   R e a d L A B E L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLABELImage reads a LABEL image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadLABELImage method is:
%
%      Image *ReadLABELImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLABELImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadLABELImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *image;

  TypeMetric
    metrics;

  unsigned int
    status;

  unsigned long
    height,
    width;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->text=AllocateString(image_info->filename);
  if ((image->columns != 0) || (image->rows != 0))
    {
      /*
        Fit label to canvas size.
      */
      /* first size text up until it no longer fits into the canvas */
      for ( ; GetTypeMetrics(image,draw_info,&metrics); draw_info->pointsize*=2)
      {
        width=(unsigned long) floor(metrics.width+metrics.max_advance+0.5);
        height=(unsigned long) floor(metrics.height+0.5);
        if (((image->columns != 0) && (width >= image->columns)) ||
            ((image->rows != 0) && (height >= image->rows)))
          break;
      }
      for ( ; GetTypeMetrics(image,draw_info,&metrics); draw_info->pointsize--)
      {
        width=(unsigned long) floor(metrics.width+metrics.max_advance+0.5);
        height=(unsigned long) floor(metrics.height+0.5);
        if ((image->columns != 0) && (width <= image->columns) &&
           ((image->rows == 0) || (height <= image->rows)))
          break;
        if ((image->rows != 0) && (height <= image->rows) &&
           ((image->columns == 0) || (width <= image->columns)))
          break;
        if (draw_info->pointsize < 2.0)
          break;
      }
    }
  status=GetTypeMetrics(image,draw_info,&metrics);
  if (status == False)
    ThrowReaderException(DelegateWarning,"Unable to get type metrics",image);
  FormatString(geometry,"+%g+%g",0.5*metrics.max_advance,metrics.ascent);
  if (image->columns == 0)
    image->columns=(unsigned long) floor(metrics.width+metrics.max_advance+0.5);
  if (image->rows == 0)
    {
      image->rows=(unsigned long) floor(metrics.height+0.5);
      FormatString(geometry,"+%g+%g",image->columns/2.0-metrics.width/2.0,
        metrics.ascent);
    }
  draw_info->geometry=AllocateString(geometry);
  SetImage(image,OpaqueOpacity);
  (void) AnnotateImage(image,draw_info);
  DestroyDrawInfo(draw_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r L A B E L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterLABELImage adds attributes for the LABEL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterLABELImage method is:
%
%      RegisterLABELImage(void)
%
*/
ModuleExport void RegisterLABELImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("LABEL");
  entry->decoder=ReadLABELImage;
  entry->adjoin=False;
  entry->description=AllocateString("Text image format");
  entry->module=AllocateString("LABEL");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r L A B E L I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterLABELImage removes format registrations made by the
%  LABEL module from the list of supported formats.
%
%  The format of the UnregisterLABELImage method is:
%
%      UnregisterLABELImage(void)
%
*/
ModuleExport void UnregisterLABELImage(void)
{
  (void) UnregisterMagickInfo("LABEL");
}
