/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%       TTTTT  RRRR    AAA   N   N  SSSSS  FFFFF   OOO   RRRR   M   M         %
%         T    R   R  A   A  NN  N  SS     F      O   O  R   R  MM MM         %
%         T    RRRR   AAAAA  N N N   SSS   FFF    O   O  RRRR   M M M         %
%         T    R R    A   A  N  NN     SS  F      O   O  R R    M   M         %
%         T    R  R   A   A  N   N  SSSSS  F       OOO   R  R   M   M         %
%                                                                             %
%                                                                             %
%                   ImageMagick Image Transform Methods                       %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#if defined(HasLCMS)
# if !defined(vms) && !defined(macintosh) && !defined(WIN32)
#  include <lcms/lcms.h>
# else
#  include "lcms.h"
# endif
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C h o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ChopImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ChopImage method is:
%
%      Image *ChopImage(Image *image,const RectangleInfo *chop_info)
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o chop_image: Method ChopImage returns a pointer to the chop
%      image.  A null image is returned if there is a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o chop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ChopImage(Image *image,const RectangleInfo *chop_info,
  ExceptionInfo *exception)
{
#define ChopImageText  "  Chop image...  "

  Image
    *chop_image;

  int
    j,
    y;

  RectangleInfo
    clone_info;

  register IndexPacket
    *chop_indexes,
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  /*
    Check chop geometry.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  assert(chop_info != (RectangleInfo *) NULL);
  if (((chop_info->x+(int) chop_info->width) < 0) ||
      ((chop_info->y+(int) chop_info->height) < 0) ||
      (chop_info->x >= (int) image->columns) ||
      (chop_info->y >= (int) image->rows))
    ThrowImageException(OptionWarning,"Unable to chop image",
      "geometry does not contain image");
  clone_info=(*chop_info);
  if ((clone_info.x+(int) clone_info.width) > (int) image->columns)
    clone_info.width=(unsigned int) ((int) image->columns-clone_info.x);
  if ((clone_info.y+(int) clone_info.height) > (int) image->rows)
    clone_info.height=(unsigned int) ((int) image->rows-clone_info.y);
  if (clone_info.x < 0)
    {
      clone_info.width-=(unsigned int) (-clone_info.x);
      clone_info.x=0;
    }
  if (clone_info.y < 0)
    {
      clone_info.height-=(unsigned int) (-clone_info.y);
      clone_info.y=0;
    }
  /*
    Initialize chop image attributes.
  */
  chop_image=CloneImage(image,image->columns-clone_info.width,
    image->rows-clone_info.height,False,exception);
  if (chop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Extract chop image.
  */
  i=0;
  j=0;
  for (y=0; y < clone_info.y; y++)
  {
    p=GetImagePixels(image,0,i++,image->columns,1);
    q=SetImagePixels(chop_image,0,j++,chop_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(image);
    chop_indexes=GetIndexes(chop_image);
    for (x=0; x < (int) image->columns; x++)
    {
      if ((x < clone_info.x) || (x >= (int) (clone_info.x+clone_info.width)))
        {
          if (image->storage_class == PseudoClass)
            chop_indexes[x]=indexes[x];
          *q=(*p);
          q++;
        }
      p++;
    }
    if (!SyncImagePixels(chop_image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(ChopImageText,y,image->rows);
  }
  /*
    Extract chop image.
  */
  i+=clone_info.height;
  for (y=0; y < (int) (image->rows-(clone_info.y+clone_info.height)); y++)
  {
    p=GetImagePixels(image,0,i++,image->columns,1);
    q=SetImagePixels(chop_image,0,j++,chop_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(image);
    chop_indexes=GetIndexes(chop_image);
    for (x=0; x < (int) image->columns; x++)
    {
      if ((x < clone_info.x) || (x >= (int) (clone_info.x+clone_info.width)))
        {
          if (image->storage_class == PseudoClass)
            chop_indexes[x]=indexes[x];
          *q=(*p);
          q++;
        }
      p++;
    }
    if (!SyncImagePixels(chop_image))
      break;
    if (QuantumTick(i,image->rows))
      MagickMonitor(ChopImageText,i,image->rows);
  }
  return(chop_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o a l e s c e I m a g e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CoalesceImages merges a sequence of images.  This is useful for GIF
%  and MNG animation sequences that have page offsets and disposal methods.
%
%  The format of the CoalesceImages method is:
%
%      Image *CoalesceImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.  It points to the first image in the group to be
%      coalesced.
%
%    o exception: return any errors or warnings in this structure.
%
*/
MagickExport Image *CoalesceImages(Image *image,ExceptionInfo *exception)
{
  Image
    *coalesce_image;

  register Image
    *next;

  /*
    Coalesce the next sequence.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to coalesce image",
      "next sequence required");
  /*
    Clone first next in sequence.
  */
  coalesce_image=CloneImage(image,0,0,True,exception);
  if (coalesce_image == (Image *) NULL)
    return((Image *) NULL);
  GetPageInfo(&coalesce_image->page);
  /*
    Coalesce image.
  */
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    coalesce_image->next=CloneImage(coalesce_image,0,0,True,exception);
    if (coalesce_image->next == (Image *) NULL)
      {
        DestroyImages(coalesce_image);
        return((Image *) NULL);
      }
    coalesce_image->next->previous=coalesce_image;
    coalesce_image=coalesce_image->next;
    coalesce_image->delay=next->delay;
    coalesce_image->start_loop=next->start_loop;
    CompositeImage(coalesce_image,next->matte ? OverCompositeOp :
      CopyCompositeOp,next,next->page.x,next->page.y);
    GetPageInfo(&coalesce_image->page);
    CloseImagePixels(next);
    CloseImagePixels(coalesce_image);
  }
  while (coalesce_image->previous != (Image *) NULL)
    coalesce_image=coalesce_image->previous;
  return(coalesce_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CropImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.  This method is optimized to preserve
%  the runlength encoding.  That is, the crop image will always use less
%  memory than the original.
%
%  The format of the CropImage method is:
%
%      Image *CropImage(Image *image,const RectangleInfo *crop_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o crop_image: Method CropImage returns a pointer to the crop
%      image.  A null image is returned if there is a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o crop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CropImage(Image *image,const RectangleInfo *crop_info,
  ExceptionInfo *exception)
{
#define CropImageText  "  Crop image...  "

  Image
    *crop_image;

  int
    y;

  RectangleInfo
    page;

  register IndexPacket
    *crop_indexes,
    *indexes;

  register PixelPacket
    *p,
    *q;

  /*
    Check crop geometry.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(crop_info != (const RectangleInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((crop_info->width != 0) || (crop_info->height != 0))
    {
      if (((crop_info->x+(int) crop_info->width) < 0) ||
          ((crop_info->y+(int) crop_info->height) < 0) ||
          (crop_info->x >= (int) image->columns) ||
          (crop_info->y >= (int) image->rows))
        ThrowImageException(OptionWarning,"Unable to crop image",
          "geometry does not contain any part of the image");
    }
  page=(*crop_info);
  if ((page.width != 0) || (page.height != 0))
    {
      if ((page.x+(int) page.width) > (int) image->columns)
        page.width=image->columns-page.x;
      if ((page.y+(int) page.height) > (int) image->rows)
        page.height=image->rows-page.y;
      if (page.x < 0)
        {
          page.width+=page.x;
          page.x=0;
        }
      if (page.y < 0)
        {
          page.height+=page.y;
          page.y=0;
        }
    }
  else
    {
      /*
        Set bounding box to the image dimensions.
      */
      page=GetImageBoundingBox(image);
      page.width+=crop_info->x*2;
      page.height+=crop_info->y*2;
      page.x-=crop_info->x;
      if (page.x < 0)
        page.x=0;
      page.y-=crop_info->y;
      if (page.y < 0)
        page.y=0;
      if ((((int) page.width+page.x) > (int) image->columns) ||
          (((int) page.height+page.y) > (int) image->rows))
        ThrowImageException(OptionWarning,"Unable to crop image",
          "geometry does not contain image");
    }
  if ((page.width == 0) || (page.height == 0))
    ThrowImageException(OptionWarning,"Unable to crop image",
      "geometry dimensions are zero");
  if ((page.width == image->columns) && (page.height == image->rows) &&
      (page.x == 0) && (page.y == 0))
    return(CloneImage(image,0,0,False,exception));
  /*
    Initialize crop image attributes.
  */
  crop_image=CloneImage(image,page.width,page.height,False,exception);
  if (crop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Extract crop image.
  */
  crop_image->page=page;
  for (y=0; y < (int) crop_image->rows; y++)
  {
    p=GetImagePixels(image,page.x,page.y+y,crop_image->columns,1);
    q=SetImagePixels(crop_image,0,y,crop_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(image);
    crop_indexes=GetIndexes(crop_image);
    if (image->storage_class == PseudoClass)
      memcpy(crop_indexes,indexes,crop_image->columns*sizeof(IndexPacket));
    memcpy(q,p,crop_image->columns*sizeof(PixelPacket));
    if (!SyncImagePixels(crop_image))
      break;
    if (QuantumTick(y,crop_image->rows))
      MagickMonitor(CropImageText,y,crop_image->rows-1);
  }
  if (y < (int) crop_image->rows)
    {
      DestroyImage(crop_image);
      return((Image *) NULL);
    }
  return(crop_image);
} 

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e c o n s t r u c t I m a g e s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DeconstructImages breaks down an image sequence into constituent
%  parts.  This is useful for creating GIF or MNG animation sequences.
%
%  The format of the DeconstructImages method is:
%
%      Image *DeconstructImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.  It points to the first next in the group to be
%      deconstructed.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *DeconstructImages(Image *image,ExceptionInfo *exception)
{
  Image
    *crop_next,
    *deconstruct_image;

  int
    y;

  RectangleInfo
    *bounds;

  register Image
    *next;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to deconstruct image sequence",
      "image sequence required");
  /*
    Ensure the image are the same size.
  */
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if ((next->columns != image->columns) || (next->rows != image->rows))
      ThrowImageException(OptionWarning,"Unable to deconstruct image sequence",
        "image are not the same size");
  }
  /*
    Allocate memory.
  */
  bounds=(RectangleInfo *)
    AcquireMemory(GetNumberScenes(image)*sizeof(RectangleInfo));
  if (bounds == (RectangleInfo *) NULL)
    ThrowImageException(OptionWarning,"Unable to deconstruct image sequence",
      "Memory allocation failed");
  /*
    Compute the bounding box for each next in the sequence.
  */
  i=0;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    /*
      Set bounding box to the next dimensions.
    */
    for (x=0; x < (int) next->columns; x++)
    {
      p=GetImagePixels(next,x,0,1,next->rows);
      q=GetImagePixels(next->previous,x,0,1,next->previous->rows);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (y=0; y < (int) next->rows; y++)
      {
        if (!ColorMatch(*p,*q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (y < (int) next->rows)
        break;
    }
    bounds[i].x=x;
    for (y=0; y < (int) next->rows; y++)
    {
      p=GetImagePixels(next,0,y,next->columns,1);
      q=GetImagePixels(next->previous,0,y,next->previous->columns,1);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (x=0; x < (int) next->columns; x++)
      {
        if (!ColorMatch(*p,*q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (x < (int) next->columns)
        break;
    }
    bounds[i].y=y;
    for (x=next->columns-1; x >= 0; x--)
    {
      p=GetImagePixels(next,x,0,1,next->rows);
      q=GetImagePixels(next->previous,x,0,1,next->previous->rows);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (y=0; y < (int) next->rows; y++)
      {
        if (!ColorMatch(*p,*q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (y < (int) next->rows)
        break;
    }
    bounds[i].width=x-bounds[i].x+1;
    for (y=next->rows-1; y >= 0; y--)
    {
      p=GetImagePixels(next,0,y,next->columns,1);
      q=GetImagePixels(next->previous,0,y,next->previous->columns,1);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      for (x=0; x < (int) next->columns; x++)
      {
        if (!ColorMatch(*p,*q,next->fuzz))
          break;
        p++;
        q++;
      }
      if (x < (int) next->columns)
        break;
    }
    bounds[i].height=y-bounds[i].y+1;
    i++;
    CloseImagePixels(next);
    CloseImagePixels(next->previous);
  }
  /*
    Clone first next in sequence.
  */
  deconstruct_image=CloneImage(image,0,0,True,exception);
  if (deconstruct_image == (Image *) NULL)
    {
      LiberateMemory((void **) &bounds);
      return((Image *) NULL);
    }
  /*
    Deconstruct the next sequence.
  */
  i=0;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    next->orphan=True;
    crop_next=CropImage(next,&bounds[i++],exception);
    if (crop_next == (Image *) NULL)
      break;
    deconstruct_image->next=crop_next;
    crop_next->previous=deconstruct_image;
    deconstruct_image=deconstruct_image->next;
  }
  LiberateMemory((void **) &bounds);
  while (deconstruct_image->previous != (Image *) NULL)
    deconstruct_image=deconstruct_image->previous;
  return(deconstruct_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlipImage creates a new image that reflects each scanline in the
%  vertical direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlipImage method is:
%
%      Image *FlipImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o flip_image: Method FlipImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FlipImage(Image *image,ExceptionInfo *exception)
{
#define FlipImageText  "  Flip image...  "

  Image
    *flip_image;

  int
    y;

  register IndexPacket
    *flip_indexes,
    *indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  unsigned int
    status;

  /*
    Initialize flip image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  flip_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (flip_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Flip each row.
  */
  for (y=0; y < (int) flip_image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(flip_image,0,flip_image->rows-y-1,flip_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(image);
    flip_indexes=GetIndexes(flip_image);
    for (x=0; x < (int) flip_image->columns; x++)
    {
      *q=(*p);
      p++;
      q++;
    }
    if (flip_image->storage_class == PseudoClass)
      memcpy(flip_indexes,indexes,flip_image->columns*sizeof(IndexPacket));
    status=SyncImagePixels(flip_image);
    if (status == False)
      break;
    if (QuantumTick(y,flip_image->rows))
      MagickMonitor(FlipImageText,y,flip_image->rows);
  }
  return(flip_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlopImage creates a new image that reflects each scanline in the
%  horizontal direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlopImage method is:
%
%      Image *FlopImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o flop_image: Method FlopImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *FlopImage(Image *image,ExceptionInfo *exception)
{
#define FlopImageText  "  Flop image...  "

  Image
    *flop_image;

  int
    y;

  register IndexPacket
    *flop_indexes,
    *indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  unsigned int
    status;

  /*
    Initialize flop image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  flop_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (flop_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Flop each row.
  */
  for (y=0; y < (int) flop_image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    q=SetImagePixels(flop_image,0,y,flop_image->columns,1);
    if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    indexes=GetIndexes(image);
    flop_indexes=GetIndexes(flop_image);
    q+=flop_image->columns;
    for (x=0; x < (int) flop_image->columns; x++)
    {
      if (flop_image->storage_class == PseudoClass)
        flop_indexes[flop_image->columns-x-1]=indexes[x];
      q--;
      *q=(*p);
      p++;
    }
    status=SyncImagePixels(flop_image);
    if (status == False)
      break;
    if (QuantumTick(y,flop_image->rows))
      MagickMonitor(FlopImageText,y,flop_image->rows);
  }
  return(flop_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r o f i l e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ProfileImage removes or adds a ICM, IPTC, or generic profile to an
%  image.  If the profile name is specified it is deleted from the image.  If
%  a filename is specified, one or more profiles are read and added to the
%  image.  If the profile name is "*" then all profiles are deleted from
%  the image.
%
%  The format of the ProfileImage method is:
%
%      unsigned int ProfileImage(Image *image,const char *profile_name,
%        const char *filename)
%
%  A description of each parameter follows:
%
%    o status: Method ProfileImage returns True if the profile is successfully
%      added or removed from the image, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o profile_name: Specifies the type of profile to add or remove.
%
%    o filename: Specifies the filename of the ICM or IPTC profile.
%
%
*/
MagickExport unsigned int ProfileImage(Image *image,const char *profile_name,
  const char *filename)
{
  ExceptionInfo
    exception;

  Image
    *profile_image;

  ImageInfo
    *image_info;

  register int
    i,
    j;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (filename == (const char *) NULL)
    {
      /*
        Remove an ICM, IPTC, or generic profile from the image.
      */
      if ((LocaleCompare("icm",profile_name) == 0) || (*profile_name == '*'))
        {
          if (image->color_profile.length != 0)
            LiberateMemory((void **) &image->color_profile.info);
          image->color_profile.length=0;
          image->color_profile.info=(unsigned char *) NULL;
        }
      if ((LocaleCompare("8bim",profile_name) == 0) ||
          (LocaleCompare("iptc",profile_name) == 0) || (*profile_name == '*'))
        {
          if (image->iptc_profile.length != 0)
            LiberateMemory((void **) &image->iptc_profile.info);
          image->iptc_profile.length=0;
          image->iptc_profile.info=(unsigned char *) NULL;
        }
      for (i=0; i < image->generic_profiles; i++)
      {
        if ((LocaleCompare(image->generic_profile[i].name,profile_name) != 0) &&
            (*profile_name != '*'))
          continue;
        if (image->generic_profile[i].name != (char *) NULL)
          LiberateMemory((void **) &image->generic_profile[i].name);
        if (image->iptc_profile.length != 0)
          LiberateMemory((void **) &image->generic_profile[i].info);
        image->generic_profiles--;
        for (j=i; j < image->generic_profiles; j++)
          image->generic_profile[j]=image->generic_profile[j+1];
        i--;
      }
      return(True);
    }
  /*
    Add a ICM, IPTC, or generic profile to the image.
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) strcpy(image_info->filename,filename);
  profile_image=ReadImage(image_info,&exception);
  if (exception.severity != UndefinedException)
    MagickWarning(exception.severity,exception.reason,exception.description);
  DestroyImageInfo(image_info);
  if (profile_image == (Image *) NULL)
    return(False);
  if (profile_image->iptc_profile.length != 0)
    {
      if (image->iptc_profile.length != 0)
        LiberateMemory((void **) &image->iptc_profile.info);
      image->iptc_profile.length=profile_image->iptc_profile.length;
      image->iptc_profile.info=profile_image->iptc_profile.info;
      profile_image->iptc_profile.length=0;
      profile_image->iptc_profile.info=(unsigned char *) NULL;
    }
  if (profile_image->color_profile.length != 0)
    {
      if (image->color_profile.length != 0)
        {
#if defined(HasLCMS)
          typedef struct _ProfilePacket
          {
            unsigned short
              red,
              green,
              blue,
              opacity;
          } ProfilePacket;

          cmsHPROFILE
            image_profile,
            transform_profile;

          cmsHTRANSFORM
            transform;

          int
            intent,
            y;

          ProfilePacket
            alpha,
            beta;

          register int
            x;

          register PixelPacket
            *q;

          /*
            Transform pixel colors as defined by the color profiles.
          */
          image_profile=cmsOpenProfileFromMem(image->color_profile.info,
            image->color_profile.length);
          transform_profile=cmsOpenProfileFromMem(
            profile_image->color_profile.info,
            profile_image->color_profile.length);
          if ((image_profile == (cmsHPROFILE) NULL) ||
              (transform_profile == (cmsHPROFILE) NULL))
            ThrowBinaryException(ResourceLimitWarning,"Unable to manage color",
              "failed to open color profiles");
          switch (image->rendering_intent)
          {
            case AbsoluteIntent: intent=INTENT_ABSOLUTE_COLORIMETRIC; break;
            case PerceptualIntent: intent=INTENT_PERCEPTUAL; break;
            case RelativeIntent: intent=INTENT_RELATIVE_COLORIMETRIC; break;
            case SaturationIntent: intent=INTENT_SATURATION; break;
            default: intent=INTENT_PERCEPTUAL; break;
          }
          if (image->colorspace == CMYKColorspace)
            {
              if (profile_image->colorspace == CMYKColorspace)
                transform=cmsCreateTransform(image_profile,TYPE_CMYK_16,
                  transform_profile,TYPE_CMYK_16,intent,0);
              else
                transform=cmsCreateTransform(image_profile,TYPE_CMYK_16,
                  transform_profile,TYPE_RGBA_16,intent,0);
            }
          else
            {
              if (profile_image->colorspace == CMYKColorspace)
                transform=cmsCreateTransform(image_profile,TYPE_RGBA_16,
                  transform_profile,TYPE_CMYK_16,intent,0);
              else
                transform=cmsCreateTransform(image_profile,TYPE_RGBA_16,
                  transform_profile,TYPE_RGBA_16,intent,0);
            }
          if (transform == (cmsHTRANSFORM) NULL)
            ThrowBinaryException(ResourceLimitWarning,"Unable to manage color",
              "failed to create color transform");
          if (image->colorspace == CMYKColorspace)
            image->matte=1;
          for (y=0; y < (int) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (int) image->columns; x++)
            {
              alpha.red=XUpScale(q->red);
              alpha.green=XUpScale(q->green);
              alpha.blue=XUpScale(q->blue);
              alpha.opacity=OpaqueOpacity;
              if (image->matte || (image->colorspace == CMYKColorspace))
                alpha.opacity=XUpScale(q->opacity);
              cmsDoTransform(transform,&alpha,&beta,1);
              q->red=XDownScale(beta.red);
              q->green=XDownScale(beta.green);
              q->blue=XDownScale(beta.blue);
              q->opacity=XDownScale(beta.opacity);
              q++;
            }
            if (!SyncImagePixels(image))
              break;
          }
          if (image->colorspace == CMYKColorspace)
            image->matte=0;
          image->colorspace=profile_image->colorspace;
          cmsDeleteTransform(transform);
          cmsCloseProfile(image_profile);
          cmsCloseProfile(transform_profile);     
          if (profile_image->colorspace == CMYKColorspace)
            image->colorspace=CMYKColorspace;
          if (profile_image->colorspace == RGBColorspace)
            image->colorspace=RGBColorspace;
#endif
          LiberateMemory((void **) &image->color_profile.info);
        }
      image->color_profile.length=profile_image->color_profile.length;
      image->color_profile.info=profile_image->color_profile.info;
      profile_image->color_profile.length=0;
      profile_image->color_profile.info=(unsigned char *) NULL;
    }
  if (profile_image->generic_profiles != 0)
    {
      unsigned int
        number_profiles;

      number_profiles=image->generic_profiles+profile_image->generic_profiles;
      if (image->generic_profile == (ProfileInfo *) NULL)
        image->generic_profile=(ProfileInfo *)
          AcquireMemory(number_profiles*sizeof(ProfileInfo));
      else
        ReacquireMemory((void **) &image->generic_profile,
          number_profiles*sizeof(ProfileInfo));
      if (image->generic_profile == (ProfileInfo *) NULL)
        {
          image->generic_profiles=0;
          DestroyImage(profile_image);
          ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
            (char *) NULL);
        }
      j=image->generic_profiles;
      for (i=0; i < profile_image->generic_profiles; i++)
      {
        image->generic_profile[j].name=profile_image->generic_profile[i].name;
        image->generic_profile[j].length=
          profile_image->generic_profile[i].length;
        image->generic_profile[j].info=profile_image->generic_profile[i].info;
        profile_image->generic_profile[i].name=(char *) NULL;
        profile_image->generic_profile[i].length=0;
        profile_image->generic_profile[i].info=(unsigned char *) NULL;
        j++;
      }
      image->generic_profiles=number_profiles;
    }
  DestroyImage(profile_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o l l I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RollImage rolls an image vertically and horizontally.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the RollImage method is:
%
%      Image *RollImage(Image *image,const int x_offset,const int y_offset,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o roll_image: Method RollImage returns a pointer to the image after
%      rolling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the number of columns to roll
%      in the horizontal direction.
%
%    o y_offset: An integer that specifies the number of rows to roll in the
%      vertical direction.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *RollImage(Image *image,const int x_offset,
  const int y_offset,ExceptionInfo *exception)
{
#define RollImageText  "  Roll image...  "

  Image
    *roll_image;

  int
    y;

  register IndexPacket
    *indexes,
    *roll_indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  PointInfo
    offset;

  /*
    Initialize roll image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  roll_image=CloneImage(image,image->columns,image->rows,False,exception);
  if (roll_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Roll image.
  */
  offset.x=x_offset % image->columns;
  offset.y=y_offset % image->rows;
  if (offset.x < 0)
    offset.x+=image->columns;
  if (offset.y < 0)
    offset.y+=image->rows;
  for (y=0; y < (int) image->rows; y++)
  {
    /*
      Transfer scanline.
    */
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
    {
      q=SetImagePixels(roll_image,((int) offset.x+x) % image->columns,
        ((int) offset.y+y) % image->rows,1,1);
      if (q == (PixelPacket *) NULL)
        break;
      roll_indexes=GetIndexes(roll_image);
      if (image->storage_class == PseudoClass)
        *roll_indexes=indexes[x];
      *q=(*p);
      p++;
      if (!SyncImagePixels(roll_image))
        break;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(RollImageText,y,image->rows);
  }
  return(roll_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformImage creates a new image that is a transformed size of
%  of existing one as specified by the crop and image geometries.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  If a crop geometry is specified a subregion of the image is obtained.
%  If the specified image size, as defined by the image and scale geometries,
%  is smaller than the actual image size, the image is first minified to an
%  integral of the specified image size with an antialias digital filter.  The
%  image is then scaled to the exact specified image size with pixel
%  replication.  If the specified image size is greater than the actual image
%  size, the image is first enlarged to an integral of the specified image
%  size with bilinear interpolation.  The image is then scaled to the exact
%  specified image size with pixel replication.
%
%  The format of the TransformImage method is:
%
%      void TransformImage(Image **image,const char *crop_geometry,
%        const char *image_geometry)
%
%  A description of each parameter follows:
%
%    o image: The address of an address of a structure of type Image.  The
%      transformed image is returned as this parameter.
%
%    o crop_geometry: Specifies a pointer to a crop geometry string.
%      This geometry defines a subregion of the image.
%
%    o image_geometry: Specifies a pointer to a image geometry string.
%      The specified width and height of this geometry string are absolute.
%
%
*/
MagickExport void TransformImage(Image **image,const char *crop_geometry,
  const char *image_geometry)
{
  Image
    *transform_image;

  int
    flags,
    x,
    y;

  unsigned int
    height,
    width;

  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  transform_image=(*image);
  if (crop_geometry != (const char *) NULL)
    {
      Image
        *crop_image;

      RectangleInfo
        crop_info;

      /*
        Crop image to a user specified size.
      */
      width=transform_image->columns;
      height=transform_image->rows;
      crop_info.x=0;
      crop_info.y=0;
      flags=ParseGeometry((char *) crop_geometry,&crop_info.x,&crop_info.y,
        &width,&height);
      if ((flags & WidthValue) == 0)
        width=(unsigned int) ((int) transform_image->columns-crop_info.x);
      if ((flags & HeightValue) == 0)
        height=(unsigned int) ((int) transform_image->rows-crop_info.y);
      if (strchr(crop_geometry,'%') != (char *) NULL)
        {
          /*
            Crop geometry is relative to image size.
          */
          x=0;
          y=0;
          (void) ParseImageGeometry(crop_geometry,&x,&y,&width,&height);
          crop_info.x=width/2;
          crop_info.y=height/2;
          width=transform_image->columns-2*crop_info.x;
          height=transform_image->rows-2*crop_info.y;
          flags|=XValue | YValue;
        }
      crop_info.width=width;
      crop_info.height=height;
      crop_image=(Image *) NULL;
      if ((width == 0) || (height == 0) ||
          ((flags & XValue) != 0) || ((flags & YValue) != 0))
        crop_image=CropImage(transform_image,&crop_info,
          &(*image)->exception);
      else
        if ((transform_image->columns > width) ||
            (transform_image->rows > height))
          {
            Image
              *next_image;

            /*
              Crop repeatedly to create uniform subimages.
            */
            for (y=0; y < (int) transform_image->rows; y+=height)
            {
              for (x=0; x < (int) transform_image->columns; x+=width)
              {
                crop_info.width=width;
                crop_info.height=height;
                crop_info.x=x;
                crop_info.y=y;
                next_image=
                  CropImage(transform_image,&crop_info,&(*image)->exception);
                if (next_image == (Image *) NULL)
                  break;
                if (crop_image == (Image *) NULL)
                  crop_image=next_image;
                else
                  {
                    next_image->previous=crop_image;
                    crop_image->next=next_image;
                    crop_image=crop_image->next;
                  }
              }
              if (next_image == (Image *) NULL)
                break;
            }
          }
      if (crop_image != (Image *) NULL)
        {
          DestroyImage(transform_image);
          while (crop_image->previous != (Image *) NULL)
            crop_image=crop_image->previous;
          transform_image=crop_image;
        }
    }
  /*
    Scale image to a user specified size.
  */
  width=transform_image->columns;
  height=transform_image->rows;
  x=0;
  y=0;
  (void) ParseImageGeometry(image_geometry,&x,&y,&width,&height);
  if ((transform_image->columns != width) || (transform_image->rows != height))
    {
      Image
        *resize_image;

      /*
        Zoom image.
      */
      resize_image=ZoomImage(transform_image,width,height,&(*image)->exception);
      if (resize_image != (Image *) NULL)
        {
          DestroyImage(transform_image);
          transform_image=resize_image;
        }
    }
  *image=transform_image;
}
