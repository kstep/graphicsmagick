/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         L      IIIII  SSSSS  TTTTT                          %
%                         L        I    SS       T                            %
%                         L        I     SSS     T                            %
%                         L        I       SS    T                            %
%                         LLLLL  IIIII  SSSSS    T                            %
%                                                                             %
%                                                                             %
%                       ImageMagick Image List Methods                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                January 2003                                 %
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
#include "list.h"
#include "blob.h"
#include "utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A p p e n d I m a g e T o L i s t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AppendImageToList() appends an image to the end of the list.
%
%  The format of the AppendImageToList method is:
%
%      unsigned int AppendImageToList(Image *images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int AppendImageToList(Image **images,Image *image)
{
  register Image
    *p;

  assert(images != (Image **) NULL);
  if (image == (Image *) NULL)
    return(False);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      *images=image;
      return(True);
    }
  assert((*images)->signature == MagickSignature);
  for (p=(*images); p->next != (Image *) NULL; p=p->next);
  p->next=image;
  p->next->previous=p;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageList() returns a duplicate of the image list.
%
%  The format of the CloneImageList method is:
%
%      Image *CloneImageList(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImageList(const Image *images,ExceptionInfo *exception)
{
  Image
    *clone;

  register Image
    *p;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  p=(Image *) NULL;
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for ( ; images != (Image *) NULL; images=images->next)
  {
    clone=CloneImage(images,0,0,True,exception);
    if (clone == (Image *) NULL)
      {
        if (p != (Image *) NULL)
          DestroyImageList(p);
        return((Image *) NULL);
      }
    if (p == (Image *) NULL)
      {
        p=clone;
        continue;
      }
    clone->previous=p;
    p->next=clone;
    p=p->next;
  }
  while (p->previous != (Image *) NULL)
    p=p->previous;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e l e t e I m a g e F r o m L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DeleteImageFromList() deletes an image at the specified position from the
%  list.
%
%  The format of the DeleteImageFromList method is:
%
%      unsigned int DeleteImageFromList(Image **images,const long offset)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%
*/
MagickExport unsigned int DeleteImageFromList(Image **images,const long offset)
{
  register Image
    *p;

  register long
    i;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return(False);
  assert((*images)->signature == MagickSignature);
  p=(*images);
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (i=0; p != (Image *) NULL; p=p->next)
    if (i++ == offset)
      break;
  if (p == (Image *) NULL)
    return(False);
  if ((p->previous == (Image *) NULL) && (p->next == (Image *) NULL))
    *images=(Image *) NULL;
  else
    {
      if (p->previous != (Image *) NULL)
        {
          p->previous->next=p->next;
          if (p == *images)
            *images=p->previous;
        }
      if (p->next != (Image *) NULL)
        {
          p->next->previous=p->previous;
          if (p == *images)
            *images=p->next;
        }
    }
  DestroyImage(p);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageList() destroys an image list.
%
%  The format of the DestroyImageList method is:
%
%      void DestroyImageList(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%
*/
MagickExport void DestroyImageList(Image *images)
{
  register Image
    *p;

  if (images == (Image *) NULL)
    return;
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (p=images; images != (Image *) NULL; p=images)
  {
    images=images->next;
    DestroyImage(p);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e F r o m L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageFromList() returns an image at the specified position from the list.
%
%  The format of the GetImageFromList method is:
%
%      Image *GetImageFromList(const Image *images,const long offset,
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
MagickExport Image *GetImageFromList(const Image *images,const long offset,
  ExceptionInfo *exception)
{
  register long
    i;

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
  return(CloneImage(images,0,0,True,exception));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n d e x I n L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageIndexInList() returns the position in the list of the specified
%  image.
%
%  The format of the GetImageIndexInList method is:
%
%      long GetImageIndexInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport long GetImageIndexInList(const Image *images)
{
  register long
    i;

  if (images == (const Image *) NULL)
    return(-1);
  assert(images->signature == MagickSignature);
  for (i=0; images->previous != (Image *) NULL; i++)
    images=images->previous;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e L i s t L e n g t h                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageListLength() returns the length of the list (the number of images in
%  the list).
%
%  The format of the GetImageListLength method is:
%
%      unsigned long GetImageListLength(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport unsigned long GetImageListLength(const Image *images)
{
  register long
    i;

  if (images == (Image *) NULL)
    return(0);
  assert(images->signature == MagickSignature);
  while (images->previous != (Image *) NULL)
    images=images->previous;
  for (i=0; images != (Image *) NULL; images=images->next)
    i++;
  return(i);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t N e x t I m a g e I n L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetNextImageInList() returns the next image in the list.
%
%  The format of the GetNextImageInList method is:
%
%      Image *GetNextImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetNextImageInList(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->next == (Image *) NULL)
    return((Image *) NULL);
  if (images->blob != images->next->blob)
    {
      DestroyBlobInfo(images->next->blob);
      images->next->blob=ReferenceBlob(images->blob);
    }
  return(images->next);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P r e v i o u s I m a g e I n L i s t                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPreviousImageInList() returns the previous image in the list.
%
%  The format of the GetPreviousImageInList method is:
%
%      Image *GetPreviousImageInList(const Image *images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *GetPreviousImageInList(const Image *images)
{
  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  if (images->previous == (Image *) NULL)
    return((Image *) NULL);
  if (images->blob != images->previous->blob)
    {
      DestroyBlobInfo(images->previous->blob);
      images->previous->blob=ReferenceBlob(images->blob);
    }
  return(images->previous);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I m a g e L i s t T o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageListToArray() is a convenience method that converts an image list to
%  a sequential array.  For example,
%
%    group = ImageListToArray(images, exception);
%    for (i = 0; i < n; i++)
%      puts(group[i]->filename);
%
%  The format of the ImageListToArray method is:
%
%      Image **ImageListToArray(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image **ImageListToArray(const Image *images,
  ExceptionInfo *exception)
{
  Image
    **group;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image **) NULL);
  assert(images->signature == MagickSignature);
  group=(Image **) AcquireMemory(GetImageListLength(images)*sizeof(Image *));
  if (group == (Image **) NULL)
    {
      ThrowException(exception,ResourceLimitError,"MemoryAllocationFailed",
        "UnableToCreateImageGroup");
      return((Image **) NULL);
    }
  for (i=0; i < (long) GetImageListLength(images); i++)
    group[i]=GetImageFromList(images,i,exception);
  return(group);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n s e r t I m a g e I n L i s t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InsertImageInList() inserts an image in the list at the specified position.
%
%  The format of the InsertImageInList method is:
%
%      unsigned int InsertImageInList(Image *images,const Image *image,
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
MagickExport unsigned int InsertImageInList(Image **images,const Image *image,
  const long offset,ExceptionInfo *exception)
{
  register Image
    *p;

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
  for (p=(*images); p->previous != (Image *) NULL; p=p->previous);
  for (i=0; p != (Image *) NULL; p=p->next)
    if (i++ == offset)
      break;
  if (p == (Image *) NULL)
    return(False);
  p->next=CloneImageList(image,exception);
  if (p->next == (Image *) NULL)
    return(False);
  p->next->previous=p;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N e w I m a g e L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NewImageList() creates an empty image list.
%
%  The format of the NewImageList method is:
%
%      Image *NewImageList(void)
%
*/
MagickExport Image *NewImageList(void)
{
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r e p e n d I m a g e T o L i s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PrependImageToList() prepends the image to the beginning of the list.
%
%  The format of the PrependImageToList method is:
%
%      unsigned int PrependImageToList(Image *images,Image *image)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int PrependImageToList(Image **images,Image *image)
{
  register Image
    *p;

  assert(images != (Image **) NULL);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    {
      *images=image;
      return(True);
    }
  assert((*images)->signature == MagickSignature);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  p=image;
  while (p->next != (Image *) NULL)
    p=p->next;
  (*images)->previous=p;
  (*images)->previous->next=(*images);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e F i r s t I m a g e F r o m L i s t                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveFirstImageFromList() removes an image from the beginning of the list.
%
%  The format of the RemoveFirstImageFromList method is:
%
%      Image *RemoveFirstImageFromList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *RemoveFirstImageFromList(Image **images)
{
  register Image
    *p;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  while ((*images)->previous != (Image *) NULL)
    (*images)=(*images)->previous;
  p=(*images);
  *images=(*images)->next;
  if ((*images) != (Image *) NULL)
    (*images)->previous=(Image *) NULL;
  p->next=(Image *) NULL;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e m o v e L a s t I m a g e F r o m L i s t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveLastImageFromList() removes the last image from the list.
%
%  The format of the RemoveLastImageFromList method is:
%
%      Image *RemoveLastImageFromList(Image **images)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%
*/
MagickExport Image *RemoveLastImageFromList(Image **images)
{
  Image
    *p;

  assert(images != (Image **) NULL);
  if ((*images) == (Image *) NULL)
    return((Image *) NULL);
  assert((*images)->signature == MagickSignature);
  while ((*images)->next != (Image *) NULL)
    (*images)=(*images)->next;
  p=(*images);
  *images=(*images)->previous;
  if ((*images) != (Image *) NULL)
    (*images)->next=(Image *) NULL;
  p->previous=(Image *) NULL;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e v e r s e I m a g e L i s t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReverseImageList() reverses the image list.
%
%  The format of the ReverseImageList method is:
%
%      Image *ReverseImageList(const Image *images,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReverseImageList(const Image *images,
  ExceptionInfo *exception)
{
  Image
    *clone;

  register Image
    *p;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  assert(images->signature == MagickSignature);
  p=(Image *) NULL;
  while (images->next != (Image *) NULL)
    images=images->next;
  for ( ; images != (Image *) NULL; images=images->previous)
  {
    clone=CloneImage(images,0,0,True,exception);
    if (clone == (Image *) NULL)
      {
        if (p != (Image *) NULL)
          DestroyImageList(p);
        return((Image *) NULL);
      }
    if (p == (Image *) NULL)
      {
        p=clone;
        continue;
      }
    clone->previous=p;
    p->next=clone;
    p=p->next;
  }
  while (p->previous != (Image *) NULL)
    p=p->previous;
  return(p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i c e I m a g e I n t o L i s t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpliceImageIntoList() removes the images designated by offset and length
%  from the list and replaces them with the specified list.
%
%  The format of the SpliceImageIntoList method is:
%
%      unsigned int SpliceImageIntoList(Image **images,const long offset,
%        const unsigned long length,Image *splices)
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
MagickExport unsigned int SpliceImageIntoList(Image **images,const long offset,
  const unsigned long length,Image *splices)
{
  Image
    *split;

  register long
    i;

  unsigned int
    status;

  assert(images != (Image **) NULL);
  assert(splices != (Image *) NULL);
  assert(splices->signature == MagickSignature);
  if ((*images) == (Image *) NULL)
    return(False);
  assert((*images)->signature == MagickSignature);
  split=SplitImageList(*images,offset);
  if (split == False)
    return(False);
  status=False;
  for (i=0; i < length; i++)
    status|=DeleteImageFromList(&split,0);
  AppendImageToList(images,splices);
  AppendImageToList(images,split);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S p l i t I m a g e L i s t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SplitImageList() splits an image into two lists.
%
%  The format of the SplitImageList method is:
%
%      Image *SplitImageList(Image *images,const long offset)
%
%  A description of each parameter follows:
%
%    o images: The image list.
%
%    o offset: The position within the list.
%
%
*/
MagickExport Image *SplitImageList(Image *images,const long offset)
{
  register Image
    *p;

  register long
    i;

  if (images == (Image *) NULL)
    return((Image *) NULL);
  p=images;
  for ( ; p->previous != (Image *) NULL; p=p->previous);
  for (i=0; p != (Image *) NULL; p=p->next)
    if (i++ == offset)
      break;
  if (p == (Image *) NULL)
    return((Image *) NULL);
  if (p->next == (Image *) NULL)
    return((Image *) NULL);
  p=p->next;
  p->previous->next=(Image *) NULL;
  p->previous=(Image *) NULL;
  return(p);
}
