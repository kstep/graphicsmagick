/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            U   U  RRRR   L                                  %
%                            U   U  R   R  L                                  %
%                            U   U  RRRR   L                                  %
%                            U   U  R R    L                                  %
%                             UUU   R  R   LLLLL                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                              Bill Radcliffe                                 %
%                                March 2000                                   %
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
#if defined(HasXML)
#ifdef WIN32
#include <win32config.h>
#endif
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/nanoftp.h>
#include <libxml/nanohttp.h>
#endif

#if defined(HasXML)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d U R L I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadURLImage reads a Scalable Vector Gaphics file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadURLImage method is:
%
%      Image *ReadURLImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadURLImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static void GetFTPData(void *userdata,const char *data,int length)
{
  FILE
    *file;

  file=(FILE *) userdata;
  if (file == (FILE *) NULL)
    return;
  if (length <= 0)
    return;
  (void) fwrite(data,length,1,file);
}

static Image *ReadURLImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent],
    filename[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *clone_info;

  int
    length;

  void
    *context;

  image=(Image *) NULL;
  clone_info=CloneImageInfo(image_info);
  TemporaryFilename(clone_info->filename);
  file=fopen(clone_info->filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    {
      DestroyImageInfo(clone_info);
      ThrowReaderException(FileOpenWarning,"Unable to open file",image);
    }
  (void) strcpy(filename,image_info->magick);
  (void) strcat(filename,":");
  LocaleLower(filename);
  (void) strcat(filename,image_info->filename);
  if (LocaleCompare(clone_info->magick,"ftp") != 0)
    {
      char
        *type;

      type=(char *) NULL;
      context=xmlNanoHTTPOpen(filename,&type);
      if (context != (void *) NULL)
        {
          while ((length=xmlNanoHTTPRead(context,buffer,MaxTextExtent)) > 0)
            (void) fwrite(buffer,MaxTextExtent,1,file);
          xmlNanoHTTPClose(context);
          xmlFree(type);
          xmlNanoHTTPCleanup();
        }
    }
  else
    {
      xmlNanoFTPInit();
      context=xmlNanoFTPNewCtxt(filename);
      if (context != (void *) NULL)
        {
          if (xmlNanoFTPConnect(context) >= 0)
            xmlNanoFTPGet(context,GetFTPData,(void *) file,(char *) NULL);
          xmlNanoFTPClose(context);
        }
    }
  (void) fclose(file);
  *clone_info->magick='\0';
  image=ReadImage(clone_info,exception);
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  return(image);
}
#else
static Image *ReadURLImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "XML library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r U R L I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterURLImage adds attributes for the URL image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterURLImage method is:
%
%      RegisterURLImage(void)
%
*/
ModuleExport void RegisterURLImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("HTTP");
  entry->decoder=ReadURLImage;
  entry->description=AllocateString("Uniform Resource Locator");
  entry->module=AllocateString("URL");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("FTP");
  entry->decoder=ReadURLImage;
  entry->description=AllocateString("Uniform Resource Locator");
  entry->module=AllocateString("URL");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("FILE");
  entry->decoder=ReadURLImage;
  entry->description=AllocateString("Uniform Resource Locator");
  entry->module=AllocateString("URL");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r U R L I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterURLImage removes format registrations made by the
%  URL module from the list of supported formats.
%
%  The format of the UnregisterURLImage method is:
%
%      UnregisterURLImage(void)
%
*/
ModuleExport void UnregisterURLImage(void)
{
  UnregisterMagickInfo("HTTP");
  UnregisterMagickInfo("FTP");
  UnregisterMagickInfo("FILE");
}
