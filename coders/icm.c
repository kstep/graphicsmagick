/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            IIIII   CCCC  M   M                              %
%                              I    C      MM MM                              %
%                              I    C      M M M                              %
%                              I    C      M   M                              %
%                            IIIII   CCCC  M   M                              %
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
  WriteICMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadICMImage reads an image file in the ICM format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the color profile information is useful in the
%  returned image.
%
%  The format of the ReadICMImage method is:
%
%      Image *ReadICMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadICMImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadICMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    c;

  register unsigned char
    *q;

  unsigned int
    length,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  image->columns=1;
  image->rows=1;
  SetImage(image,OpaqueOpacity);
  /*
    Read ICM image.
  */
  length=MaxTextExtent;
  image->color_profile.info=(unsigned char *) AcquireMemory(length);
  for (q=image->color_profile.info; ; q++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    if ((q-image->color_profile.info+1) >= (int) length)
      {
        image->color_profile.length=q-image->color_profile.info;
        length<<=1;
        ReacquireMemory((void **) &image->color_profile.info,length);
        if (image->color_profile.info == (unsigned char *) NULL)
          break;
        q=image->color_profile.info+image->color_profile.length;
      }
    *q=(unsigned char) c;
  }
  image->color_profile.length=0;
  if (image->color_profile.info != (unsigned char *) NULL)
    image->color_profile.length=q-image->color_profile.info;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r I C M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterICMImage adds attributes for the ICM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterICMImage method is:
%
%      RegisterICMImage(void)
%
*/
ModuleExport void RegisterICMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ICM");
  entry->decoder=ReadICMImage;
  entry->encoder=WriteICMImage;
  entry->adjoin=False;
  entry->description=AllocateString("ICC Color Profile");
  entry->module=AllocateString("ICM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I C M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterICMImage removes format registrations made by the
%  ICM module from the list of supported formats.
%
%  The format of the UnregisterICMImage method is:
%
%      UnregisterICMImage(void)
%
*/
ModuleExport void UnregisterICMImage(void)
{
  (void) UnregisterMagickInfo("ICM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I C M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteICMImage writes an image in the ICM format.
%
%  The format of the WriteICMImage method is:
%
%      unsigned int WriteICMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteICMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteICMImage(const ImageInfo *image_info,Image *image)
{
  unsigned int
    status;

  /*
    Open image file.
  */
  if (image->color_profile.length == 0)
    ThrowWriterException(FileOpenWarning,"No color profile available",image);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) WriteBlob(image,image->color_profile.length,
    (char *) image->color_profile.info);
  CloseBlob(image);
  return(True);
}
