/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            TTTTT  TTTTT  FFFFF                              %
%                              T      T    F                                  %
%                              T      T    FFF                                %
%                              T      T    F                                  %
%                              T      T    F                                  %
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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#include "defines.h"

#if defined(HasTTF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T T F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTTFImage reads a TrueType font file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTTFImage method is:
%
%      Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTTFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  AnnotateInfo
    *annotate_info;

  char
    geometry[MaxTextExtent],
    text[MaxTextExtent];

  Image
    *annotate_image,
    *image;

  int
    y;

  long
    magick;

  register int
    i;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  magick=MSBFirstReadLong(image);
  if ((magick != 256) && (magick != 65536))
    ThrowReaderException(CorruptImageWarning,"Not a TTF font file",image);
  DestroyImage(image);
  /*
    Start with a white canvas.
  */
  y=0;
  clone_info=CloneImageInfo(image_info);
  if (clone_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  (void) CloneString(&clone_info->font,"");
  FormatString(clone_info->font,"@%.1024s",image_info->filename);
  annotate_info=CloneAnnotateInfo(clone_info,(AnnotateInfo *) NULL);
  (void) CloneString(&clone_info->size,"800x480");
  (void) strcpy(clone_info->filename,"xc:white");
  image=ReadImage(clone_info,exception);
  if (image == (Image *) NULL)
    {
      DestroyAnnotateInfo(annotate_info);
      return((Image *) NULL);
    }
  (void) strcpy(image->filename,image_info->filename);
  if (image_info->ping)
    {
      DestroyAnnotateInfo(annotate_info);
      DestroyImageInfo(clone_info);
      CloseBlob(image);
      return(image);
    }
  clone_info->pointsize=18;
  /*
    Annotate canvas with text rendered with font at different point sizes.
  */
  y=30;
  FormatString(clone_info->filename,"label:white");
  annotate_image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (annotate_image != (Image *) NULL)
    {
      ImageAttribute
        *attribute;

      annotate_info->pointsize=30;
      FormatString(geometry,"+10%+d",y);
      (void) CloneString(&annotate_info->geometry,geometry);
      (void) CloneString(&annotate_info->text,"Unknown family");
      attribute=GetImageAttribute(annotate_image,"Label");
      if (attribute != (ImageAttribute *) NULL)
        (void) CloneString(&annotate_info->text,attribute->value);
      AnnotateImage(image,annotate_info);
      DestroyImage(annotate_image);
      y+=42;
    }
  annotate_info->pointsize=18;
  FormatString(geometry,"+10%+d",y);
  (void) CloneString(&annotate_info->geometry,geometry);
  (void) CloneString(&annotate_info->text,"abcdefghijklmnopqrstuvwxyz");
  AnnotateImage(image,annotate_info);
  y+=20;
  FormatString(geometry,"+10%+d",y);
  (void) CloneString(&annotate_info->geometry,geometry);
  (void) CloneString(&annotate_info->text,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  AnnotateImage(image,annotate_info);
  y+=20;
  FormatString(geometry,"+10%+d",y);
  (void) CloneString(&annotate_info->geometry,geometry);
  (void) CloneString(&annotate_info->text,"1234567890.:,;(:*!?')");
  AnnotateImage(image,annotate_info);
  y+=20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+6;
    annotate_info->pointsize=18;
    FormatString(geometry,"+10%+d",y);
    (void) CloneString(&annotate_info->geometry,geometry);
    FormatString(text,"%d",i);
    (void) CloneString(&annotate_info->text,text);
    AnnotateImage(image,annotate_info);
    annotate_info->pointsize=i;
    FormatString(geometry,"+50%+d",y);
    (void) CloneString(&annotate_info->geometry,geometry);
    (void) CloneString(&annotate_info->text,
      "That which does not destroy me, only makes me stronger");
    AnnotateImage(image,annotate_info);
    if (i >= 24)
      i+=6;
  }
  DestroyAnnotateInfo(annotate_info);
  return(image);
}
#else
static Image *ReadTTFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,"Cannot read TTF images",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T T F I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTTFImage adds attributes for the TTF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTTFImage method is:
%
%      RegisterTTFImage(void)
%
*/
ModuleExport void RegisterTTFImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TTF");
  entry->decoder=ReadTTFImage;
  entry->adjoin=False;
  entry->description=AllocateString("TrueType font");
  entry->module=AllocateString("TTF");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T T F I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTTFImage removes format registrations made by the
%  TTF module from the list of supported formats.
%
%  The format of the UnregisterTTFImage method is:
%
%      UnregisterTTFImage(void)
%
*/
ModuleExport void UnregisterTTFImage(void)
{
#if defined(HasTTF)
  UnregisterMagickInfo("TTF");
#endif
}
