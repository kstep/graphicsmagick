/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              SSSSS  TTTTT  EEEEE   GGGG   AAA   N   N   OOO                 %
%              SS       T    E      G      A   A  NN  N  O   O                %
%               SSS     T    EEE    G  GG  AAAAA  N N N  O   O                %
%                 SS    T    E      G   G  A   A  N  NN  O   O                %
%              SSSSS    T    EEEEE   GGG   A   A  N   N   OOO                 %
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
%   R e a d S T E G A N O I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSTEGANOImage reads a steganographic image hidden within another
%  image type.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadSTEGANOImage method is:
%
%      Image *ReadSTEGANOImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSTEGANOImage returns a pointer to the image
%      after reading.  A null image is returned if there is a memory shortage
%      of if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadSTEGANOImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define GetBit(a,i) (((a) >> (i)) & 1L)
#define SetBit(a,i,set) a=(set) ? (a) | (1L << (i)) : (a) & ~(1L << (i))

  Image
    *clone_image,
    *image,
    *watermark;

  ImageInfo
    *clone_info;

  long
    c,
    i,
    j,
    k,
    y;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes,
    *stegano_indexes;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize Image structure.
  */
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  clone_info=CloneImageInfo(image_info);
  DetachBlob(clone_info->blob);
  *clone_info->magick='\0';
  watermark=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (watermark == (Image *) NULL)
    return((Image *) NULL);
  watermark->depth=QuantumDepth;
  if (!AllocateImageColormap(image,MaxRGB+1))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Get hidden watermark from low-order bits of image.
  */
  c=0;
  i=0;
  j=0;
  k=image->offset;
  for (i=QuantumDepth-1; (i >= 0) && (j < QuantumDepth); i--)
  {
    for (y=0; (y < (long) image->rows) && (j < QuantumDepth); y++)
    {
      for (x=0; (x < (long) image->columns) && (j < QuantumDepth); x++)
      {
        pixel=AcquireOnePixel(watermark,k % (long) watermark->columns,
          k/(long) watermark->columns,exception);
        q=GetImagePixels(image,x,y,1,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        switch (c)
        {
          case 0:
	  {
            SetBit(*indexes,i,GetBit(pixel.red,j));
            break;
          }
          case 1:
	  {
            SetBit(*indexes,i,GetBit(pixel.green,j));
            break;
          }
          case 2:
	  {
            SetBit(*indexes,i,GetBit(pixel.blue,j));
            break;
          }
        }
        (void) SyncImage(image);
        c++;
        if (c == 3)
          c=0;
        k++;
        if (k == (long) (watermark->columns*watermark->columns))
          k=0;
        if (k == image->offset)
          j++;
      }
    }
    MagickMonitor(LoadImagesText,i,QuantumDepth);
  }
  DestroyImage(watermark);
  SyncImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r S T E G A N O I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSTEGANOImage adds attributes for the STEGANO image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSTEGANOImage method is:
%
%      RegisterSTEGANOImage(void)
%
*/
ModuleExport void RegisterSTEGANOImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("STEGANO");
  entry->decoder=ReadSTEGANOImage;
  entry->description=AllocateString("Steganographic image");
  entry->module=AllocateString("STEGANO");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S T E G A N O I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSTEGANOImage removes format registrations made by the
%  STEGANO module from the list of supported formats.
%
%  The format of the UnregisterSTEGANOImage method is:
%
%      UnregisterSTEGANOImage(void)
%
*/
ModuleExport void UnregisterSTEGANOImage(void)
{
  (void) UnregisterMagickInfo("STEGANO");
}
