/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            AAA   PPPP   PPPP   1                            %
%                           A   A  P   P  P   P  1                            %
%                           AAAAA  PPPP   PPPP   1                            %
%                           A   A  P      P      1                            %
%                           A   A  P      P      1                            %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             William Radcliffe                               %
%                                 Sept 2001                                   %
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
  WriteAPP1Image(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s A P P 1                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsAPP1 returns True if the image format type, identified by the
%  magick string, is APP1.
%
%  The format of the IsAPP1 method is:
%
%      unsigned int IsAPP1(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsAPP1 returns True if the image format type is APP1.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsAPP1(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"APP1",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A P P 1 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadAPP1Image reads an binary file in the APP1 format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the Photoshop resource (APP1) information is useful in
%  the returned image.
%
%  The format of the ReadAPP1Image method is:
%
%      Image *ReadAPP1Image(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadAPP1Image returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadAPP1Image(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    c,
    i;

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
  i=image->generic_profiles;
  if (image->generic_profile == (ProfileInfo *) NULL)
    image->generic_profile=(ProfileInfo *) AcquireMemory(sizeof(ProfileInfo));
  else
    ReacquireMemory((void **) &image->generic_profile,
      (i+1)*sizeof(ProfileInfo));
  if (image->generic_profile == (ProfileInfo *) NULL)
    {
      image->generic_profiles=0;
      ThrowReaderException(FileOpenWarning,"Memory allocation failed",image);
    }
  image->generic_profiles++;
  image->generic_profile[i].name=AllocateString((char *) NULL);
  FormatString(image->generic_profile[i].name,"APP%d",1);
  /*
    Read APP1 image.
  */
  length=MaxTextExtent;
  image->generic_profile[i].info=(unsigned char *) AcquireMemory(length+2);
  for (q=image->generic_profile[i].info; image->generic_profile[i].info; q++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      break;
    if ((q-image->generic_profile[i].info+1) >= (int) length)
      {
        image->generic_profile[i].length=q-image->generic_profile[i].info;
        length<<=1;
        ReacquireMemory((void **) &image->generic_profile[i].info,length+2);
        if (image->generic_profile[i].info == (unsigned char *) NULL)
          break;
        q=image->generic_profile[i].info+image->generic_profile[i].length;
      }
    *q=(unsigned char) c;
  }
  if (image->generic_profile[i].info == (unsigned char *) NULL)
    ThrowReaderException(FileOpenWarning,"Memory allocation failed",image);
  image->generic_profile[i].length=q-image->generic_profile[i].info;
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r A P P 1 I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterAPP1Image adds attributes for the APP1 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterAPP1Image method is:
%
%      RegisterAPP1Image(void)
%
*/
ModuleExport void RegisterAPP1Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("APP1");
  entry->decoder=ReadAPP1Image;
  entry->encoder=WriteAPP1Image;
  entry->magick=IsAPP1;
  entry->adjoin=False;
  entry->description=AllocateString("Photoshop resource format");
  entry->module=AllocateString("APP1");
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
%  Method UnregisterBIMImage removes format registrations made by the
%  BIM module from the list of supported formats.
%
%  The format of the UnregisterBIMImage method is:
%
%      UnregisterAPP1Image(void)
%
*/
ModuleExport void UnregisterAPP1Image(void)
{
  (void) UnregisterMagickInfo("APP1");
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
%  Method WriteAPP1Image writes an image in the APP1 format.
%
%  The format of the WriteAPP1Image method is:
%
%      unsigned int WriteAPP1Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteAPP1Image return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteAPP1Image(const ImageInfo *image_info,Image *image)
{
  int
    i;

  unsigned int
    status;

  for (i=0; i < (int) image->generic_profiles; i++)
  {
    if ((LocaleCompare(image->generic_profile[i].name,"APP1") == 0) &&
        (image->generic_profile[i].length != 0))
      {
        /*
          Open image file.
        */
        status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
        if (status == False)
          ThrowWriterException(FileOpenWarning,"Unable to open file",image);
        (void) WriteBlob(image,(int) image->generic_profile[i].length,
          (char *) image->generic_profile[i].info);
        CloseBlob(image);
        return(True);
      }
  }
  ThrowWriterException(FileOpenWarning,"No APP1 data is available",image);
  return(True);
}
