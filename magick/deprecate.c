/*
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
%                        ImageMagick Deprecated Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                October 2002                                 %
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
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "deprecate.h"
#include "list.h"
#include "resource.h"
#include "utility.h"

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
  Image
    *image;

  register long
    i;

  if (images == (Image *) NULL)
    return(False);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    if (i++ == offset)
      break;
  if (images == (Image *) NULL)
    return(False);
  image=images;
  if (images->previous != (Image *) NULL)
    images->previous->next=images->next;
  if (images->next != (Image *) NULL)
    images->next->previous=images->previous;
  DestroyImage(image);
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
  return(CloneImage(GetImageFromList(images,offset,exception),0,0,True,exception));
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
  return(AppendImageToList(images,CloneImageList(image,exception)));
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
    *next;

  register long
    i;

  assert(images != (Image **) NULL);
  if (image == (Image *) NULL)
    return(False);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      if (offset > 0)
        return(False);
      *images=CloneImageList(image,exception);
      return(*images != (Image *) NULL);
    }
  assert((*images)->signature == MagickSignature);
  for (next=(*images); next->previous != (Image *) NULL; next=next->previous);
  for (i=0; next != (Image *) NULL; next=next->next)
    if (i++ == offset)
      break;
  if (next == (Image *) NULL)
    return(False);
  next->next=CloneImageList(image,exception);
  if (next->next == (Image *) NULL)
    return(False);
  next->next->previous=next;
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
%      off_t SizeBlob(const Image *image)
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
MagickExport ExtendedSignedIntegralType SizeBlob(const Image *image)
{
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
    *splice;

  register long
    i;

  register Image
    *p,
    *q;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    if (i++ == offset)
      break;
  if (images == (Image *) NULL)
    return((Image *) NULL);
  p=images;
  for (i=1; images != (Image *) NULL; images=images->next)
    if (i++ == (long) length)
      break;
  if (images == (Image *) NULL)
    return((Image *) NULL);
  q=images;
  splice=CloneImageList(splices,exception);
  if (splice == (Image *) NULL)
    return(q);
  if (p->previous != (Image *) NULL)
    {
      p->previous->next=splice;
      if (splice != (Image *) NULL)
        splice->previous=p->previous;
    }
  p->previous=(Image *) NULL;
  if (q->next != (Image *) NULL)
    {
      q->next->previous=splice;
      if (splice != (Image *) NULL)
        {
          while (splice->next != (Image *) NULL)
            splice=splice->next;
          splice->next=q->next;
        }
    }
  q->next=(Image *) NULL;
  return(p);
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
  return(PrependImageToList(images,CloneImageList(image,exception)));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   V a l i d a t e C o l o r m a p I n d e x                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ValidateColormapIndex() validates the colormap index.  If the index does
%  not range from 0 to the number of colors in the colormap an exception
%  is issued and 0 is returned.
%
%  This method is deprecated as of version 5.4.4.
%
%  The format of the ValidateColormapIndex method is:
%
%      IndexPacket ValidateColormapIndex(Image *image,const unsigned int index)
%
%  A description of each parameter follows:
%
%    o index: Method ValidateColormapIndex returns colormap index if it is
%      valid other an exception is issued and 0 is returned.
%
%    o image: The image.
%
%    o index: This integer is the colormap index.
%
%
*/

MagickExport IndexPacket ValidateColormapIndex(Image *image,
  const unsigned long index)
{
  return(ConstrainColormapIndex(image,index));
}
