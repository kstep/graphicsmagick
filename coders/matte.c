/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     M   M   AAA   TTTTT  TTTTT  EEEEE                       %
%                     MM MM  A   A    T      T    E                           %
%                     M M M  AAAAA    T      T    EEE                         %
%                     M   M  A   A    T      T    E                           %
%                     M   M  A   A    T      T    EEEEE                       %
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
  WriteMATTEImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M A T T E I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMATTEImage adds attributes for the MATTE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMATTEImage method is:
%
%      RegisterMATTEImage(void)
%
*/
ModuleExport void RegisterMATTEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MATTE");
  entry->encoder=WriteMATTEImage;
  entry->raw=True;
  entry->description=AllocateString("MATTE format");
  entry->module=AllocateString("MATTE");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M A T T E I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMATTEImage removes format registrations made by the
%  MATTE module from the list of supported formats.
%
%  The format of the UnregisterMATTEImage method is:
%
%      UnregisterMATTEImage(void)
%
*/
ModuleExport void UnregisterMATTEImage(void)
{
  (void) UnregisterMagickInfo("MATTE");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A T T E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteMATTEImage writes an image of matte bytes to a file.  It
%  consists of data from the matte component of the image [0..255].
%
%  The format of the WriteMATTEImage method is:
%
%      unsigned int WriteMATTEImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteMATTEImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMATTEImage(const ImageInfo *image_info,Image *image)
{
  Image
    *matte_image;

  long
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned int
    status;

  if (!image->matte)
    ThrowWriterException(ResourceLimitWarning,
      "Image does not have a matte channel",image);
  matte_image=CloneImage(image,image->columns,image->rows,True,
    &image->exception);
  if (matte_image == (Image *) NULL)
    return(False);
  if (!AllocateImageColormap(matte_image,MaxRGB+1))
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Convert image to matte pixels.
  */
  matte_image->matte=False;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    q=SetImagePixels(matte_image,0,y,matte_image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(matte_image);
    for (x=0; x < (long) image->columns; x++)
    {
      indexes[x]=p->opacity;
      *q=matte_image->colormap[p->opacity];
      p++;
      q++;
    }
    if (!SyncImagePixels(matte_image))
      break;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
  }
  (void) FormatString(matte_image->filename,"MIFF:%.1024s",image->filename);
  status=WriteImage(image_info,matte_image);
  DestroyImage(matte_image);
  return(status);
}
