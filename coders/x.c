/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                   X   X                                     %
%                                    X X                                      %
%                                     X                                       %
%                                    X X                                      %
%                                   X   X                                     %
%                                                                             %
%                                                                             %
%                   Read/Write GraphicsMagick Image Format.                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "magick.h"
#include "utility.h"
#include "xwindow.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteXImage(const ImageInfo *,Image *);

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ReadXImage reads an image from an X window.
%
%  The format of the ReadXImage method is:
%
%      Image *ReadXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  XImportInfo
    ximage_info;

  XGetImportInfo(&ximage_info);
  return(XImportImage(image_info,&ximage_info));
}
#else
static Image *ReadXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateError,"XWindowLibraryIsNotAvailable",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXImage adds attributes for the X image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXImage method is:
%
%      RegisterXImage(void)
%
*/
ModuleExport void RegisterXImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("X");
  entry->decoder=(DecoderHandler) ReadXImage;
  entry->encoder=(EncoderHandler) WriteXImage;
  entry->adjoin=False;
  entry->description=AcquireString("X Image");
  entry->module=AcquireString("X");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXImage removes format registrations made by the
%  X module from the list of supported formats.
%
%  The format of the UnregisterXImage method is:
%
%      UnregisterXImage(void)
%
*/
ModuleExport void UnregisterXImage(void)
{
  (void) UnregisterMagickInfo("X");
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteXImage writes an image to an X server.
%
%  The format of the WriteXImage method is:
%
%      unsigned int WriteXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXImage return True if the image is displayed on
%      the X server.  False is returned is there is a memory shortage or if
%      the image file fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteXImage(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;

  unsigned long
    state;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Open X server connection.
  */
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    ThrowWriterException(ResourceLimitError,"UnableToOpenXServer",image);
  /*
    Set our forgiving error handler.
  */
  (void) XSetErrorHandler(XError);
  /*
    Get user defaults from X resource database.
  */
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  resource_info.immutable=True;
  /*
    Display image.
  */
  state=DefaultState;
  (void) XDisplayImage(display,&resource_info,&client_name,1,&image,&state);
  (void) XCloseDisplay(display);
  return(True);
}
#else
static unsigned int WriteXImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateError,"XWindowLibraryIsNotAvailable",
    image->filename);
}
#endif
