/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            FFFFF   AAA   X   X                              %
%                            F      A   A   X X                               %
%                            FFF    AAAAA    X                                %
%                            F      A   A   X X                               %
%                            F      A   A  X   X                              %
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
  WriteFAXImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s F A X                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsFAX returns True if the image format type, identified by the
%  magick string, is FAX.
%
%  The format of the IsFAX method is:
%
%      unsigned int IsFAX(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsFAX returns True if the image format type is FAX.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsFAX(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"DFAX",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F A X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFAXImage reads a Group 3 FAX image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFAXImage method is:
%
%      Image *ReadFAXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadFAXImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadFAXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  image->storage_class=PseudoClass;
  if (image->columns == 0)
    image->columns=2592;
  if (image->rows == 0)
    image->rows=3508;
  image->depth=8;
  if (!AllocateImageColormap(image,2))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Monochrome colormap.
  */
  image->colormap[0].red=MaxRGB;
  image->colormap[0].green=MaxRGB;
  image->colormap[0].blue=MaxRGB;
  image->colormap[1].red=0;
  image->colormap[1].green=0;
  image->colormap[1].blue=0;
  status=HuffmanDecodeImage(image);
  if (status == False)
    ThrowReaderException(CorruptImageWarning,"Unable to read image data",image);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r F A X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterFAXImage adds attributes for the FAX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterFAXImage method is:
%
%      RegisterFAXImage(void)
%
*/
ModuleExport void RegisterFAXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("FAX");
  entry->decoder=ReadFAXImage;
  entry->encoder=WriteFAXImage;
  entry->magick=IsFAX;
  entry->description=AllocateString("Group 3 FAX");
  entry->module=AllocateString("FAX");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("G3");
  entry->decoder=ReadFAXImage;
  entry->encoder=WriteFAXImage;
  entry->magick=IsFAX;
  entry->adjoin=False;
  entry->description=AllocateString("Group 3 FAX");
  entry->module=AllocateString("FAX");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r F A X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterFAXImage removes format registrations made by the
%  FAX module from the list of supported formats.
%
%  The format of the UnregisterFAXImage method is:
%
%      UnregisterFAXImage(void)
%
*/
ModuleExport void UnregisterFAXImage(void)
{
  (void) UnregisterMagickInfo("FAX");
  (void) UnregisterMagickInfo("G3");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F A X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteFAXImage writes an image to a file in 1 dimensional Huffman
%  encoded format.
%
%  The format of the WriteFAXImage method is:
%
%      unsigned int WriteFAXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFAXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteFAXImage(const ImageInfo *image_info,Image *image)
{
  ImageInfo
    *clone_info;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->magick,"FAX");
  scene=0;
  do
  {
    /*
      Convert MIFF to monochrome.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    status=HuffmanEncodeImage(clone_info,image);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (clone_info->adjoin);
  DestroyImageInfo(clone_info);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(status);
}
