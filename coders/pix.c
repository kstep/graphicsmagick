/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   IIIII  X   X                              %
%                            P   P    I     X X                               %
%                            PPPP     I      X                                %
%                            P        I     X X                               %
%                            P      IIIII  X   X                              %
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
%   R e a d P I X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPIXImage reads a Alias/Wavefront RLE image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPIXImage method is:
%
%      Image *ReadPIXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPIXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPIXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  int
    y;

  Quantum
    blue,
    green,
    red;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  unsigned int
    bits_per_pixel,
    status;

  unsigned long
    height,
    length,
    width;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read PIX image.
  */
  width=ReadBlobMSBShort(image);
  height=ReadBlobMSBShort(image);
  (void) ReadBlobMSBShort(image);  /* x-offset */
  (void) ReadBlobMSBShort(image);  /* y-offset */
  bits_per_pixel=ReadBlobMSBShort(image);
  if ((width == (unsigned long) ~0) || (height == (unsigned long) ~0) ||
      ((bits_per_pixel != 8) && (bits_per_pixel != 24)))
    ThrowReaderException(CorruptImageWarning,"Not a PIX image file",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if (bits_per_pixel == 8)
      if (!AllocateImageColormap(image,MaxRGB+1))
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
    /*
      Convert PIX raster image to pixel packets.
    */
    red=0;
    green=0;
    blue=0;
    index=0;
    length=0;
    for (y=0; y < (int) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (int) image->columns; x++)
      {
        if (length == 0)
          {
            length=ReadBlobByte(image);
            if (bits_per_pixel == 8)
              index=UpScale(ReadBlobByte(image));
            else
              {
                blue=UpScale(ReadBlobByte(image));
                green=UpScale(ReadBlobByte(image));
                red=UpScale(ReadBlobByte(image));
              }
          }
        if (image->storage_class == PseudoClass)
          indexes[x]=index;
        q->blue=blue;
        q->green=green;
        q->red=red;
        length--;
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        MagickMonitor(LoadImageText,y,image->rows);
    }
    if (image->storage_class == PseudoClass)
      SyncImage(image);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    width=ReadBlobMSBLong(image);
    height=ReadBlobMSBLong(image);
    (void) ReadBlobMSBShort(image);
    (void) ReadBlobMSBShort(image);
    bits_per_pixel=ReadBlobMSBShort(image);
    status=(width != (unsigned long) ~0) && (height == (unsigned long) ~0) &&
      ((bits_per_pixel == 8) || (bits_per_pixel == 24));
    if (status == True)
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image=image->next;
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
      }
  } while (status == True);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P I X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPIXImage adds attributes for the PIX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPIXImage method is:
%
%      RegisterPIXImage(void)
%
*/
ModuleExport void RegisterPIXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PIX");
  entry->decoder=ReadPIXImage;
  entry->description=AllocateString("Alias/Wavefront RLE image format");
  entry->module=AllocateString("PIX");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P I X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPIXImage removes format registrations made by the
%  PIX module from the list of supported formats.
%
%  The format of the UnregisterPIXImage method is:
%
%      UnregisterPIXImage(void)
%
*/
ModuleExport void UnregisterPIXImage(void)
{
  UnregisterMagickInfo("PIX");
}
