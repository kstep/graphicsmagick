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
%                        TTTTT  IIIII  L      EEEEE                           %
%                          T      I    L      E                               %
%                          T      I    L      EEE                             %
%                          T      I    L      E                               %
%                          T    IIIII  LLLLL  EEEEE                           %
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
#include "composite.h"
#include "constitute.h"
#include "magick.h"
#include "monitor.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I L E I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTILEImage tiles a texture on an image.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadTILEImage method is:
%
%      Image *ReadTILEImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTILEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTILEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image,
    *tile_image;

  ImageInfo
    *clone_info;

  long
    y;

  register long
    x;

  /*
    Initialize Image structure.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  *clone_info->magick='\0';
  tile_image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  if (tile_image == (Image *) NULL)
    return((Image *) NULL);
  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionError,"MustSpecifyImageSize",image);
  if (*image_info->filename == '\0')
    ThrowReaderException(OptionError,"MustSpecifyAnImageName",image);
  /*
    Tile texture onto image.
  */
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  for (y=0; y < (long) image->rows; y+=tile_image->rows)
  {
    for (x=0; x < (long) image->columns; x+=tile_image->columns)
      (void) CompositeImage(image,CopyCompositeOp,tile_image,x,y);
    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
      break;
  }
  DestroyImage(tile_image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T I L E I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTILEImage adds attributes for the TILE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTILEImage method is:
%
%      RegisterTILEImage(void)
%
*/
ModuleExport void RegisterTILEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("TILE");
  entry->decoder=(DecoderHandler) ReadTILEImage;
  entry->raw=True;
  entry->description=AcquireString("Tile image with a texture");
  entry->module=AcquireString("TILE");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T I L E I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTILEImage removes format registrations made by the
%  TILE module from the list of supported formats.
%
%  The format of the UnregisterTILEImage method is:
%
%      UnregisterTILEImage(void)
%
*/
ModuleExport void UnregisterTILEImage(void)
{
  (void) UnregisterMagickInfo("TILE");
}
