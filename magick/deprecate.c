/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%        DDDD   EEEEE  PPPP   RRRR   EEEEE   CCCC   AAA   TTTTT  EEEEE        %
%        D   D  E      P   P  R   R  E      C      A   A    T    E            %
%        D   D  EEE    PPPPP  RRRR   EEE    C      AAAAA    T    EEE          %
%        D   D  E      P      R R    E      C      A   A    T    E            %
%        DDDD   EEEEE  P      R  R   EEEEE   CCCC  A   A    T    EEEEE        %
%                                                                             %
%                                                                             %
%                      GraphicsMagick Deprecated Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                October 2002                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/deprecate.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/resource.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e I m a g e L i s t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageList() deletes an image at the specified position in the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the DeleteImageList method is:
%
%      unsigned int DeleteImageList(Image *images,const long offset)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%
*/
MagickExport unsigned int DeleteImageList(Image *images,const long offset)
{
  register long
    i;

  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; i < offset; i++)
  {
    if (images->next == (Image *) NULL)
      return(False);
    images=images->next;
  }
  DeleteImageFromList(&images);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImages() destroys an image list.
%
%  This method is deprecated as of version 5.4.3.
%
%  The format of the DestroyImages method is:
%
%      void DestroyImages(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%
*/
MagickExport void DestroyImages(Image *image)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  DestroyImageList(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageList() returns an image at the specified position in the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetImageList method is:
%
%      Image *GetImageList(const Image *images,const long offset,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *GetImageList(const Image *images,const long offset,
  ExceptionInfo *exception)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(CloneImage(GetImageFromList(images,offset),0,0,True,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t I n d e x                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListIndex() returns the position in the list of the specified
%  image.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetImageListIndex method is:
%
%      long GetImageListIndex(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport long GetImageListIndex(const Image *images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetImageIndexInList(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t S i z e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListSize() returns the number of images in the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetImageListSize method is:
%
%      unsigned long GetImageListSize(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport unsigned long GetImageListSize(const Image *images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetImageListLength(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N e x t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNextImage() returns the next image in a list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetNextImage method is:
%
%      Image *GetNextImage(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetNextImage(const Image *images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetNextImageInList(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N u m b e r S c e n e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNumberScenes() returns the number of images in the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetNumberScenes method is:
%
%      unsigned int GetNumberScenes(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport unsigned int GetNumberScenes(const Image *image)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetImageListLength(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P r e v i o u s I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPreviousImage() returns the previous image in a list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the GetPreviousImage method is:
%
%      Image *GetPreviousImage(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetPreviousImage(const Image *images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetPreviousImageInList(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   P a r s e I m a g e G e o m e t r y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseImageGeometry() is similar to GetGeometry() except the returned
%  geometry is modified as determined by the meta characters:  %, !, <,
%  and >.
%
%  This method is deprecated as of version 5.5.1.
%
%  The format of the ParseImageGeometry method is:
%
%      int ParseImageGeometry(const char *geometry,long *x,long *y,
%        unsigned long *width,unsigned long *height)
%
%  A description of each parameter follows:
%
%    o flags:  Method ParseImageGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  A pointer to an integer.  The x and y offset as determined by
%      the geometry specification is returned here.
%
%    o width,height:  A pointer to an unsigned integer.  The width and height
%      as determined by the geometry specification is returned here.
%
%
*/
MagickExport int ParseImageGeometry(const char *geometry,long *x,long *y,
  unsigned long *width,unsigned long *height)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetMagickGeometry(geometry,x,y,width,height));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P o p I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PopImageList() removes the last image in the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the PopImageList method is:
%
%      Image *PopImageList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *PopImageList(Image **images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(RemoveLastImageFromList(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  P o s t s c r i p t G e o m e t r y                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PostscriptGeometry() replaces any page mneumonic with the equivalent size in
%  picas.
%
%  This method is deprecated as of version 5.5.1.
%
%  The format of the PostscriptGeometry method is:
%
%      char *PostscriptGeometry(const char *page)
%
%  A description of each parameter follows.
%
%   o  page:  Specifies a pointer to an array of characters.
%      The string is either a Postscript page name (e.g. A4) or a postscript
%      page geometry (e.g. 612x792+36+36).
%
%
*/
MagickExport char *PostscriptGeometry(const char *page)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetPageGeometry(page));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P u s h I m a g e L i s t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PushImageList() adds an image to the end of the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the PushImageList method is:
%
%      unsigned int PushImageList(Image *images,const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int PushImageList(Image **images,const Image *image,
  ExceptionInfo *exception)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  AppendImageToList(images,CloneImageList(image,exception));
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t C a c h e T h e s h o l d                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetCacheThreshold() sets the amount of free memory allocated for the
%  pixel cache.  Once this threshold is exceeded, all subsequent pixels cache
%  operations are to/from disk.
%
%  This method is deprecated as of version 5.5.1.
%
%  The format of the SetCacheThreshold() method is:
%
%      void SetCacheThreshold(const size_t threshold)
%
%  A description of each parameter follows:
%
%    o threshold: The number of megabytes of memory available to the pixel
%      cache.
%
%
*/
MagickExport void SetCacheThreshold(const unsigned long size)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  SetMagickResourceLimit(MemoryResource,size);
  SetMagickResourceLimit(MapResource,2*size);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageList() inserts an image into the list at the specified position.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the SetImageList method is:
%
%      unsigned int SetImageList(Image *images,const Image *image,
%        const long offset,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o offset: The position within the list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int SetImageList(Image **images,const Image *image,
  const long offset,ExceptionInfo *exception)
{
  Image
    *clone;

  register long
    i;

  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  clone=CloneImageList(image,exception);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  for (i=0; i < offset; i++)
  {
    if ((*images)->next == (Image *) NULL)
      return(False);
    (*images)=(*images)->next;
  }
  InsertImageInList(images,clone);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S h i f t I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ShiftImageList() removes an image from the beginning of the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the ShiftImageList method is:
%
%      Image *ShiftImageList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *ShiftImageList(Image **images)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(RemoveFirstImageFromList(images));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S i z e B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SizeBlob() returns the current length of the image file or blob.
%
%  This method is deprecated as of version 5.4.3.
%
%  The format of the SizeBlob method is:
%
%      magick_off_t SizeBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o size:  Method SizeBlob returns the current length of the image file
%      or blob.
%
%    o image: The image.
%
%
*/
MagickExport magick_off_t SizeBlob(const Image *image)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  return(GetBlobSize(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i c e I m a g e L i s t                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpliceImageList() removes the images designated by offset and length from
%  the list and replaces them with the specified list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the SpliceImageList method is:
%
%      Image *SpliceImageList(Image *images,const long offset,
%        const unsigned long length,const Image *splices,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%    o length: The length of the image list to remove.
%
%    o splice: Replace the removed image list with this list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SpliceImageList(Image *images,const long offset,
  const unsigned long length,const Image *splices,ExceptionInfo *exception)
{
  Image
    *clone;

  register long
    i;

  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  clone=CloneImageList(splices,exception);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; i < offset; i++)
  {
    if (images->next == (Image *) NULL)
      return(False);
    images=images->next;
  }
  (void) SpliceImageIntoList(&images,length,clone);
  return(images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n s h i f t I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnshiftImageList() adds the image to the beginning of the list.
%
%  This method is deprecated as of version 5.5.2.
%
%  The format of the UnshiftImageList method is:
%
%      unsigned int UnshiftImageList(Image *images,const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int UnshiftImageList(Image **images,const Image *image,
  ExceptionInfo *exception)
{
  (void) LogMagickEvent(DeprecateEvent,GetMagickModule(),
    "Method has been deprecated");
  PrependImageToList(images,CloneImageList(image,exception));
  return(True);
}

