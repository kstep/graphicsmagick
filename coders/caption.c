/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%               CCCC   AAA   PPPP   TTTTT  IIIII   OOO   N   N                %
%              C      A   A  P   P    T      I    O   O  NN  N                %
%              C      AAAAA  PPPP     T      I    O   O  N N N                %
%              C      A   A  P        T      I    O   O  N  NN                %
%               CCCC  A   A  P        T    IIIII   OOO   N   N                %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               February 2002                                 %
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
%   R e a d C A P T I O N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCAPTIONImage reads a CAPTION image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadCAPTIONImage method is:
%
%      Image *ReadCAPTIONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCAPTIONImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadCAPTIONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  char
    *caption,
    geometry[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *image;

  register char
    *p,
    *q;

  register int
    i;

  TypeMetric
    metrics;

  unsigned int
    status;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  if (image->columns == 0)
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  if (*image_info->filename != '@')
    caption=AllocateString(image_info->filename);
  else
    {
      unsigned long
        length;

      /*
        Read caption.
      */
      (void) strncpy(image->filename,image_info->filename+1,MaxTextExtent-1);
      status=OpenBlob(image_info,image,ReadBinaryType,exception);
      if (status == False)
        ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      length=MaxTextExtent;
      caption=(char *) AcquireMemory(length);
      p=caption;
      if (caption != (char *) NULL)
        while (ReadBlobString(image,p) != (char *) NULL)
        {
          p+=strlen(p);
          if ((p-caption+MaxTextExtent+1) < (long) length)
            continue;
          length<<=1;
          ReacquireMemory((void **) &caption,length);
          if (caption == (char *) NULL)
            break;
          p=caption+strlen(caption);
        }
      if (caption == (char *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      CloseBlob(image);
    }
  /*
    Format caption.
  */
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  draw_info->text=AllocateString(caption);
  p=caption;
  q=draw_info->text;
  for (i=0; *p != '\0'; p++)
  {
    *q++=(*p);
    *q='\0';
    status=GetTypeMetrics(image,draw_info,&metrics);
    if (status == False)
      ThrowReaderException(DelegateWarning,"Unable to get type metrics",image);
    if ((metrics.width+metrics.max_advance/2) < image->columns)
      continue;
    for (p--; !isspace(*p) && (p > caption); p--);
    *p++='\n';
    q=draw_info->text;
    i++;
  }
  if (image->rows == 0)
    image->rows=(i+1)*metrics.height;
  SetImage(image,OpaqueOpacity);
  /*
    Draw caption.
  */
  (void) strcpy(draw_info->text,caption);
  FormatString(geometry,"+%g+%g",metrics.max_advance/4,metrics.ascent);
  draw_info->geometry=AllocateString(geometry);
  (void) AnnotateImage(image,draw_info);
  DestroyDrawInfo(draw_info);
  LiberateMemory((void **) &caption);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C A P T I O N I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCAPTIONImage adds attributes for the CAPTION image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCAPTIONImage method is:
%
%      RegisterCAPTIONImage(void)
%
*/
ModuleExport void RegisterCAPTIONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CAPTION");
  entry->decoder=ReadCAPTIONImage;
  entry->adjoin=False;
  entry->description=AllocateString("Image caption");
  entry->module=AllocateString("CAPTION");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C A P T I O N I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCAPTIONImage removes format registrations made by the
%  CAPTION module from the list of supported formats.
%
%  The format of the UnregisterCAPTIONImage method is:
%
%      UnregisterCAPTIONImage(void)
%
*/
ModuleExport void UnregisterCAPTIONImage(void)
{
  (void) UnregisterMagickInfo("CAPTION");
}
