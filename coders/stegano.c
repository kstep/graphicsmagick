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
#define UnembedBit(byte) \
{ \
  q=GetImagePixels(image,(int) (j % image->columns),(int) (j/image->columns),1,1); \
  if (q == (PixelPacket *) NULL) \
    break; \
  indexes=GetIndexes(image); \
  (*indexes)|=((byte) & 0x01) << shift; \
  (void) SyncImagePixels(image); \
  j++; \
  if (j == (long) (image->columns*image->rows)) \
    { \
      j=0; \
      shift--; \
      if (shift < 0) \
        break; \
    } \
}

  Image
    *clone_image,
    *image,
    *stegano_image;

  ImageInfo
    *clone_info;

  int
    shift,
    y;

  long
    i,
    j;

  register IndexPacket
    *indexes,
    *stegano_indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Initialize Image structure.
  */
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  clone_info=CloneImageInfo(image_info);
  RewindBlob(clone_info->blob);
  *clone_info->magick='\0';
  stegano_image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (stegano_image == (Image *) NULL)
    return((Image *) NULL);
  clone_image=CloneImage(stegano_image,image->columns,image->rows,True,
    exception);
  DestroyImage(image);
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  image=clone_image;
  if (!AllocateImageColormap(image,1 << image->depth))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image,OpaqueOpacity);
  /*
    Grab embedded watermark.
  */
  i=image->offset;
  j=0;
  shift=image->depth-1;
  for (y=0; y < (int) stegano_image->rows; y++)
  {
    for (x=0; x < (int) stegano_image->columns; x++)
    {
      if (i == (long) (stegano_image->columns*stegano_image->rows))
        i=0;
      p=GetImagePixels(stegano_image,(int) (i % stegano_image->columns),
        (int) (i/stegano_image->columns),1,1);
      if (p == (PixelPacket *) NULL)
        break;
      stegano_indexes=GetIndexes(stegano_image);
      if (stegano_image->storage_class == PseudoClass)
        UnembedBit(*stegano_indexes)
      else
        {
          UnembedBit(p->red);
          UnembedBit(p->green);
          UnembedBit(p->blue);
        }
      i++;
    }
    if (shift < 0)
      break;
    if (QuantumTick(y,stegano_image->rows))
      MagickMonitor(LoadImageText,y,stegano_image->rows);
  }
  SyncImage(image);
  image->storage_class=DirectClass;
  DestroyImage(stegano_image);
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
  RegisterMagickInfo(entry);
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
  UnregisterMagickInfo("STEGANO");
}
