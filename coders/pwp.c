/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   W   W  PPPP                               %
%                            P   P  W   W  P   P                              %
%                            PPPP   W   W  PPPP                               %
%                            P      W W W  P                                  %
%                            P       W W   P                                  %
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
%   I s P W P                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPWP returns True if the image format type, identified by the
%  magick string, is PWP.
%
%  The format of the IsPWP method is:
%
%      unsigned int IsPWP(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPWP returns True if the image format type is PWP.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPWP(const unsigned char *magick,const size_t length)
{
  if (length < 5)
    return(False);
  if (LocaleNCompare((char *) magick,"SFW95",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P W P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPWPImage reads a Seattle Film Works multi-image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPWPImage method is:
%
%      Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPWPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPWPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  FILE
    *file;

  Image
    *image,
    *next_image,
    *pwp_image;

  ImageInfo
    *clone_info;

  int
    c;

  MonitorHandler
    handler;

  register Image
    *p;

  register long
    i;

  size_t
    count;

  unsigned char
    magick[MaxTextExtent];

  unsigned int
    status;

  unsigned long
    filesize;

  /*
    Open image file.
  */
  pwp_image=AllocateImage(image_info);
  status=OpenBlob(image_info,pwp_image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",pwp_image);
  count=ReadBlob(pwp_image,5,(char *) magick);
  if ((count == 0) || (LocaleNCompare((char *) magick,"SFW95",5) != 0))
    ThrowReaderException(CorruptImageWarning,"Not a PWP image file",pwp_image);
  clone_info=CloneImageInfo(image_info);
  RewindBlob(clone_info->blob);
  UniqueImageFilename(pwp_image,clone_info->filename);
  image=(Image *) NULL;
  for ( ; ; )
  {
    for (c=ReadBlobByte(pwp_image); c != EOF; c=ReadBlobByte(pwp_image))
    {
      for (i=0; i < 17; i++)
        magick[i]=magick[i+1];
      magick[17]=(unsigned char) c;
      if (LocaleNCompare((char *) (magick+12),"SFW94A",6) == 0)
        break;
    }
    if (c == EOF)
      break;
    if (LocaleNCompare((char *) (magick+12),"SFW94A",6) != 0)
      ThrowReaderException(CorruptImageWarning,"Not a PWP image file",
        pwp_image);
    /*
      Dump SFW image to a temporary file.
    */
    file=fopen(clone_info->filename,WriteBinaryType);
    if (file == (FILE *) NULL)
      ThrowReaderException(FileOpenWarning,"Unable to write file",image);
    (void) fwrite("SFW94A",1,6,file);
    filesize=65535L*magick[2]+256L*magick[1]+magick[0];
    for (i=0; i < filesize; i++)
    {
      c=ReadBlobByte(pwp_image);
      (void) fputc(c,file);
    }
    (void) fclose(file);
    handler=SetMonitorHandler((MonitorHandler) NULL);
    next_image=ReadImage(clone_info,exception);
    (void) SetMonitorHandler(handler);
    if (next_image == (Image *) NULL)
      break;
    FormatString(next_image->filename,"slide_%02d.sfw",next_image->scene);
    if (image == (Image *) NULL)
      image=next_image;
    else
      {
        /*
          Link image into image list.
        */
        for (p=image; p->next != (Image *) NULL; p=p->next);
        next_image->previous=p;
        next_image->scene=p->scene+1;
        p->next=next_image;
      }
    if (image_info->subrange != 0)
      if (next_image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    MagickMonitor(LoadImagesText,TellBlob(pwp_image),SizeBlob(image));
  }
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  CloseBlob(pwp_image);
  DestroyImage(pwp_image);
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
%   R e g i s t e r P W P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPWPImage adds attributes for the PWP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPWPImage method is:
%
%      RegisterPWPImage(void)
%
*/
ModuleExport void RegisterPWPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PWP");
  entry->decoder=ReadPWPImage;
  entry->magick=IsPWP;
  entry->description=AllocateString("Seattle Film Works");
  entry->module=AllocateString("PWP");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P W P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPWPImage removes format registrations made by the
%  PWP module from the list of supported formats.
%
%  The format of the UnregisterPWPImage method is:
%
%      UnregisterPWPImage(void)
%
*/
ModuleExport void UnregisterPWPImage(void)
{
  UnregisterMagickInfo("PWP");
}
