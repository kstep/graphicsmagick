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
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent];

  const char
    *Text = 
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz\n0123456789 "
      "\342\352\356\373\364\344\353\357\366\374\377\340\371\351\350\347\n"
      "&#~\\\"\'(-`_^@)=+\260 $\243^\250*\265\371%!\247:/;.,?<>";

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    y;

  register int
    i;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Open draw file.
  */
  UniqueImageFilename(image,filename);
  file=fopen(filename,"w");
  if (file == (FILE *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  y=20;
  (void) fprintf(file,"font %.1024s\n",image_info->filename);
  (void) fprintf(file,"font-size 18\n");
  (void) fprintf(file,"text +10%+d \"%s\"\n",y,Text);
  y+=20*MultilineCensus(Text)+20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+12;
    (void) fprintf(file,"font-size 18\n");
    (void) fprintf(file,"text +10+%d '%d'\n",y,i);
    (void) fprintf(file,"font-size %d",i);
    (void) fprintf(file,
      "text +50+%d 'That which does not destroy me, only makes me stronger.'\n",
      y);
    if (i >= 24)
      i+=6;
  }
  (void) fclose(file);
  CloseBlob(image);
  DestroyImage(image);
  /*
    Draw image.
  */
  clone_info=CloneImageInfo(image_info);
  RewindBlob(clone_info->blob);
  FormatString(geometry,"800x480");
  CloneString(&clone_info->size,geometry);
  FormatString(clone_info->filename,"mvg:%.1024s",filename);
  image=ReadImage(clone_info,exception);
  if (image != (Image *) NULL)
    (void) strcpy(image->filename,image_info->filename);
  (void) remove(filename);
  DestroyImageInfo(clone_info);
  return(image);
}

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
  entry=SetMagickInfo("AFM");
  entry->decoder=ReadTTFImage;
  entry->adjoin=False;
  entry->description=AllocateString("TrueType font");
  entry->module=AllocateString("TTF");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("PFB");
  entry->decoder=ReadTTFImage;
  entry->adjoin=False;
  entry->description=AllocateString("TrueType font");
  entry->module=AllocateString("TTF");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("PFM");
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
  UnregisterMagickInfo("TTF");
  UnregisterMagickInfo("AFM");
  UnregisterMagickInfo("PFB");
  UnregisterMagickInfo("PFM");
}
