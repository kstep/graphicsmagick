/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        88888  BBBB   IIIII  M   M                           %
%                        88 88  B   B    I    MM MM                           %
%                         888   BBBB     I    M M M                           %
%                        88 88  B   B    I    M   M                           %
%                        88888  BBBB   IIIII  M   M                           %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
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
  Write8BIMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s 8 B I M                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Is8BIM returns True if the image format type, identified by the
%  magick string, is 8BIM.
%
%  The format of the Is8BIM method is:
%
%      unsigned int Is8BIM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method Is8BIM returns True if the image format type is 8BIM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int Is8BIM(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"8BIM",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d 8 B I M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Read8BIMImage reads an binary file in the 8BIM format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the Photoshop resource (8BIM) information is useful in
%  the returned image.
%
%  The format of the Read8BIMImage method is:
%
%      Image *Read8BIMImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method Read8BIMImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *Read8BIMImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    c;

  register unsigned char
    *q;

  size_t
    length;

  unsigned int
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
    Read 8BIM image.
  */
  length=MaxTextExtent;
  image->iptc_profile.info=(unsigned char *) AcquireMemory(length+2);
  for (q=image->iptc_profile.info; q != (unsigned char *) NULL; q++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    if ((size_t) (q-image->iptc_profile.info+1) >= length)
      {
        image->iptc_profile.length=q-image->iptc_profile.info;
        length<<=1;
        ReacquireMemory((void **) &image->iptc_profile.info,length+2);
        if (image->iptc_profile.info == (unsigned char *) NULL)
          break;
        q=image->iptc_profile.info+image->iptc_profile.length;
      }
    *q=(unsigned char) c;
  }
  if (image->iptc_profile.info == (unsigned char *) NULL)
    ThrowReaderException(FileOpenWarning,"Memory allocation failed",image);
  image->iptc_profile.length=q-image->iptc_profile.info;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r 8 B I M I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Register8BIMImage adds attributes for the 8BIM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the Register8BIMImage method is:
%
%      Register8BIMImage(void)
%
*/
ModuleExport void Register8BIMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("8BIM");
  entry->decoder=Read8BIMImage;
  entry->encoder=Write8BIMImage;
  entry->magick=Is8BIM;
  entry->adjoin=False;
  entry->description=AllocateString("Photoshop resource format");
  entry->module=AllocateString("8BIM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r 8 B I M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Unregister8BIMImage removes format registrations made by the
%  BIM module from the list of supported formats.
%
%  The format of the Unregister8BIMImage method is:
%
%      Unregister8BIMImage(void)
%
*/
ModuleExport void Unregister8BIMImage(void)
{
  (void) UnregisterMagickInfo("8BIM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e 8 B I M I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Write8BIMImage writes an image in the 8BIM format.
%
%  The format of the Write8BIMImage method is:
%
%      unsigned int Write8BIMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method Write8BIMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int Write8BIMImage(const ImageInfo *image_info,Image *image)
{
  unsigned int
    status;

  /*
    Open image file.
  */
  if (image->iptc_profile.length == 0)
    ThrowWriterException(FileOpenWarning,"No 8BIM data is available",image);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) WriteBlob(image,image->iptc_profile.length,
    (char *) image->iptc_profile.info);
  CloseBlob(image);
  return(True);
}
