/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     IIIII  M   M   AAA    GGGG  EEEEE                       %
%                       I    MM MM  A   A  G      E                           %
%                       I    M M M  AAAAA  G  GG  EEE                         %
%                       I    M   M  A   A  G   G  E                           %
%                     IIIII  M   M  A   A   GGGG  EEEEE                       %
%                                                                             %
%                                                                             %
%                          ImageMagick Image Methods                          %
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
#include "define.h"

/*
  Constant declaration.
*/
const char
  *AppendBinaryType = "ab",
  *BackgroundColor = "#dfdfdf",  /* gray */
  *BorderColor = "#dfdfdf",  /* gray */
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileGeometry = "120x120+4+3>",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *ForegroundColor = "#000000",  /* black */
  *LoadImageText = "  Loading image...  ",
  *LoadImagesText = "  Loading images...  ",
  *MatteColor = "#bdbdbd",  /* gray */
  *PSDensityGeometry = "72.0x72.0",
  *PSPageGeometry = "612x792>",
  *ReadBinaryType = "rb",
  *ReadBinaryUnbufferedType = "rbu",
  *SaveImageText = "  Saving image...  ",
  *SaveImagesText = "  Saving images...  ",
  *WriteBinaryType = "wb";

const unsigned long
  DefaultCompressionQuality = 75;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% AllocateImage() returns a pointer to an image structure initialized to
% default values.
%
%  The format of the AllocateImage method is:
%
%      Image *AllocateImage(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%
*/
MagickExport Image *AllocateImage(const ImageInfo *image_info)
{
  Image
    *allocate_image;

  int
    flags;

  /*
    Allocate image structure.
  */
  allocate_image=(Image *) AcquireMemory(sizeof(Image));
  if (allocate_image == (Image *) NULL)
    MagickError(ResourceLimitError,"Unable to allocate image",
      "Memory allocation failed");
  (void) memset(allocate_image,0,sizeof(Image));
  /*
    Initialize Image structure.
  */
  (void) strcpy(allocate_image->magick,"MIFF");
  allocate_image->storage_class=DirectClass;
  allocate_image->depth=QuantumDepth;
  allocate_image->interlace=NoInterlace;
  allocate_image->colorspace=RGBColorspace;
  (void) QueryColorDatabase(BackgroundColor,&allocate_image->background_color);
  (void) QueryColorDatabase(BorderColor,&allocate_image->border_color);
  (void) QueryColorDatabase(MatteColor,&allocate_image->matte_color);
  allocate_image->filter=LanczosFilter;
  allocate_image->blur=1.0;
  allocate_image->reference_count=1;
  GetExceptionInfo(&allocate_image->exception);
  GetTimerInfo(&allocate_image->timer);
  GetCacheInfo(&allocate_image->cache);
  allocate_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  allocate_image->signature=MagickSignature;
  if (image_info == (ImageInfo *) NULL)
    return(allocate_image);
  /*
    Transfer image info.
  */
  allocate_image->exempt=image_info->file != (FILE *) NULL;
  (void) strncpy(allocate_image->filename,image_info->filename,MaxTextExtent-1);
  (void) strncpy(allocate_image->magick_filename,image_info->filename,
    MaxTextExtent-1);
  (void) strncpy(allocate_image->magick,image_info->magick,MaxTextExtent-1);
  if (image_info->size != (char *) NULL)
    {
      long
        offset;

      (void) sscanf(image_info->size,"%lux%lu",&allocate_image->columns,
        &allocate_image->rows);
      offset=0;
      flags=ParseGeometry(image_info->size,&offset,&offset,
        &allocate_image->columns,&allocate_image->rows);
      if ((flags & HeightValue) == 0)
        allocate_image->rows=allocate_image->columns;
      allocate_image->offset=offset;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      {
        (void) sscanf(image_info->tile,"%lux%lu",&allocate_image->columns,
          &allocate_image->rows);
        flags=ParseGeometry(image_info->tile,&allocate_image->tile_info.x,
          &allocate_image->tile_info.y,&allocate_image->columns,
          &allocate_image->rows);
        if ((flags & HeightValue) == 0)
          allocate_image->rows=allocate_image->columns;
      }
  allocate_image->compression=image_info->compression;
  allocate_image->interlace=image_info->interlace;
  allocate_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%lfx%lf",&allocate_image->x_resolution,
        &allocate_image->y_resolution);
      if (count != 2)
        allocate_image->y_resolution=allocate_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      geometry=PostscriptGeometry(image_info->page);
      (void) ParseImageGeometry(geometry,&allocate_image->page.x,
        &allocate_image->page.y,&allocate_image->page.width,
        &allocate_image->page.height);
      LiberateMemory((void **) &geometry);
    }
  allocate_image->depth=image_info->depth;
  allocate_image->background_color=image_info->background_color;
  allocate_image->border_color=image_info->border_color;
  allocate_image->matte_color=image_info->matte_color;
  allocate_image->fifo=image_info->fifo;
  allocate_image->client_data=image_info->client_data;
  *allocate_image->blob=(*image_info->blob);
  return(allocate_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateImageColormap() allocates an image colormap and initializes
%  it to a linear gray colorspace.  If the image already has a colormap,
%  it is replaced.  AllocateImageColormap() returns True if successful,
%  otherwise False if there is not enough memory.
%
%  The format of the AllocateImageColormap method is:
%
%      unsigned int AllocateImageColormap(Image *image,
%        const unsigned long colors)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o colors: The number of colors in the image colormap.
%
%
*/
MagickExport unsigned int AllocateImageColormap(Image *image,
  const unsigned long colors)
{
  register long
    i;

  size_t
    length;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(colors != 0);
  image->storage_class=PseudoClass;
  image->colors=colors;
  length=Max(image->colors,MaxRGB+1)*sizeof(PixelPacket);
  if (image->colormap == (PixelPacket *) NULL)
    image->colormap=(PixelPacket *) AcquireMemory(length);
  else
    ReacquireMemory((void **) &image->colormap,length);
  if (image->colormap == (PixelPacket *) NULL)
    return(False);
  for (i=0; i < (long) image->colors; i++)
  {
    image->colormap[i].red=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
    image->colormap[i].green=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
    image->colormap[i].blue=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
    image->colormap[i].opacity=OpaqueOpacity;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e N e x t I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Use AllocateNextImage() to initialize the next image in a sequence.  The
%  next member of image points to the newly allocated image.  If there is a
%  memory shortage, next is assigned NULL.
%
%  The format of the AllocateNextImage method is:
%
%      void AllocateNextImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: Many of the image default values are set from this
%      structure.  For example, filename, compression, depth, background color,
%      and others.
%
%    o image: The image.
%
%
*/
MagickExport void AllocateNextImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  CloseImagePixels(image);
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strncpy(image->next->filename,image->filename,MaxTextExtent-1);
  if (image_info != (ImageInfo *) NULL)
    (void) strncpy(image->next->filename,image_info->filename,MaxTextExtent-1);
  *image->next->blob=(*image->blob);
  image->next->file=image->file;
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

#if defined(HasX11)
#include "xwindow.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnimateImages() repeatedly displays an image sequence to any X window
%  screen.  It returns a value other than 0 if successful.  Check the
%  exception member of image to determine the reason for any failure.
%
%  The format of the AnimateImages method is:
%
%      unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  (void) XAnimateImages(display,&resource,&client_name,1,image);
  (void) XCloseDisplay(display);
  return(image->exception.severity == UndefinedException);
}
#else
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateWarning,"X11 library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A p p e n d I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The Append() method takes a set of images and appends them to each
%  other.  Each image in the set must have the equal width or equal height
%  (or both).  Append() returns a single image where each image in the original
%  set is side-by-side if all the heights are equal or stacked on top of
%  each other if all widths are equal.   On failure, a NULL image is returned
%  and exception describes the reason for the failure.
%
%  The format of the AppendImage method is:
%
%      Image *AppendImages(const Image *image,const unsigned int stack,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o stack: An unsigned value other than stacks rectangular image
%      top-to-bottom otherwise left-to-right.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AppendImages(const Image *image,const unsigned int stack,
  ExceptionInfo *exception)
{
#define AppendImageText  "  Append image sequence...  "

  Image
    *append_image;

  register const Image
    *next;

  register long
    i;

  unsigned long
    height,
    scene,
    width;

  /*
    Ensure the image have the same column width.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  next=image;
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to append image sequence",
      "image sequence required");
  for (next=image->next; next != (Image *) NULL; next=next->next)
    if ((next->columns != image->columns) && (next->rows != image->rows))
      ThrowImageException(OptionWarning,"Unable to append image sequence",
        "image widths or heights differ");
  width=image->columns;
  height=image->rows;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    width+=next->columns;
    height+=next->rows;
  }
  /*
    Initialize append next attributes.
  */
  if ((image->columns != image->next->columns) || !stack)
    append_image=CloneImage(image,width,image->rows,True,exception);
  else
    append_image=CloneImage(image,image->columns,height,True,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  scene=0;
  if ((image->columns != image->next->columns) || !stack)
    {
      register long
        x;

      /*
        Stack left-to-right.
      */
      x=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          SetImageType(append_image,TrueColorType);
        (void) CompositeImage(append_image,CopyCompositeOp,next,x,0);
        x+=next->columns;
        MagickMonitor(AppendImageText,scene++,GetNumberScenes(image));
      }
    }
  else
    {
      register long
        y;

      /*
        Stack top-to-bottom.
      */
      y=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          SetImageType(append_image,TrueColorType);
        (void) CompositeImage(append_image,CopyCompositeOp,next,0,y);
        y+=next->rows;
        MagickMonitor(AppendImageText,scene,GetNumberScenes(image));
        scene++;
      }
    }
  if (append_image->storage_class == PseudoClass)
    {
      unsigned int
        global_colormap;

      /*
        Determine if the sequence of image has the identical colormap.
      */
      global_colormap=True;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if ((next->storage_class == DirectClass) ||
            (next->colors != image->colors))
          {
            global_colormap=False;
            break;
          }
        for (i=0; i < (long) image->colors; i++)
          if (!ColorMatch(next->colormap[i],image->colormap[i],next->fuzz))
            {
              global_colormap=False;
              break;
            }
      }
      if (!global_colormap)
        SetImageType(append_image,TrueColorType);
    }
  return(append_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A v e r a g e I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The AverageImages() method takes a set of images and averages them together.
%  Each image in the set must have the same width and the same height.
%  AverageImages() returns a single image with each corresponding pixel
%  component of each image averaged.   On failure, a NULL image is returned and
%  exception describes the reason for the failure.
%
%  The format of the AverageImage method is:
%
%      Image *AverageImages(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image sequence.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AverageImages(const Image *image,ExceptionInfo *exception)
{
#define AverageImageText  "  Average image sequence...  "

  typedef struct _SumPacket
  {
    double
      red,
      green,
      blue,
      opacity;
  } SumPacket;

  Image
    *average_image;

  long
    y;

  register const PixelPacket
    *p;

  register const Image
    *next;

  register long
    i,
    x;

  register PixelPacket
    *q;

  SumPacket
    *sum;

  unsigned int
    number_scenes;

  unsigned long
    number_pixels;

  /*
    Ensure the image are the same size.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to average image sequence",
      "image sequence required");
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if ((next->columns != image->columns) || (next->rows != image->rows))
      ThrowImageException(OptionWarning,"Unable to average image sequence",
        "image widths or heights differ");
  }
  /*
    Allocate sum accumulation buffer.
  */
  number_pixels=image->columns*image->rows;
  sum=(SumPacket *) AcquireMemory(number_pixels*sizeof(SumPacket));
  if (sum == (SumPacket *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to average image sequence",
      "Memory allocation failed");
  (void) memset(sum,0,number_pixels*sizeof(SumPacket));
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      LiberateMemory((void **) &sum);
      return((Image *) NULL);
    }
  SetImageType(average_image,TrueColorType);
  /*
    Compute sum over each pixel color component.
  */
  number_scenes=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    i=0;
    for (y=0; y < (long) next->rows; y++)
    {
      p=AcquireImagePixels(next,0,y,next->columns,1,exception);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (long) next->columns; x++)
      {
        sum[i].red+=p->red;
        sum[i].green+=p->green;
        sum[i].blue+=p->blue;
        sum[i].opacity+=p->opacity;
        p++;
        i++;
      }
    }
    number_scenes++;
  }
  /*
    Average next pixels.
  */
  i=0;
  for (y=0; y < (long) average_image->rows; y++)
  {
    q=SetImagePixels(average_image,0,y,average_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) average_image->columns; x++)
    {
      q->red=(Quantum) ((sum[i].red+number_scenes/2.0)/number_scenes);
      q->green=(Quantum) ((sum[i].green+number_scenes/2.0)/number_scenes);
      q->blue=(Quantum) ((sum[i].blue+number_scenes/2.0)/number_scenes);
      q->opacity=(Quantum) ((sum[i].opacity+number_scenes/2.0)/number_scenes);
      q++;
      i++;
    }
    if (!SyncImagePixels(average_image))
      break;
    if (QuantumTick(y,average_image->rows))
      MagickMonitor(AverageImageText,y,average_image->rows);
  }
  LiberateMemory((void **) &sum);
  return(average_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a n n e l I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Extract a channel from the image.  A channel is a particular color component
%  of each pixel in the image.
%
%  The format of the ChannelImage method is:
%
%      unsigned int ChannelImage(Image *image,const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: A value of type ChannelType that identifies which channel to
%      extract: Red, Green, Blue, or Opacity.
%
%
*/
MagickExport unsigned int ChannelImage(Image *image,const ChannelType channel)
{
#define ChannelImageText  "  Extract a channel from the image...  "

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  /*
    Channel DirectClass packets.
  */
  SetImageType(image,TrueColorMatteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      switch (channel)
      {
        case RedChannel:
        case CyanChannel:
        {
          q->green=q->red;
          q->blue=q->red;
          break;
        }
        case GreenChannel:
        case MagentaChannel:
        {
          q->red=q->green;
          q->blue=q->green;
          break;
        }
        case BlueChannel:
        case YellowChannel:
        {
          q->red=q->blue;
          q->green=q->blue;
          break;
        }
        case OpacityChannel:
        {
          if (image->colorspace == CMYKColorspace)
            {
              q->red=indexes[x];
              q->green=indexes[x];
              q->blue=indexes[x];
              break;
            }
        }
        case BlackChannel:
        case MatteChannel:
        default:
        {
          q->red=q->opacity;
          q->green=q->opacity;
          q->blue=q->opacity;
          break;
        }
      }
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(ChannelImageText,y,image->rows);
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImage() copies an image and returns the copy as a new image object.
%  If the specified columns and rows is 0, an exact copy of the image is
%  returned, otherwise the pixel data is undefined and must be initialized
%  with the SetImagePixels() and SyncImagePixels() methods.  A NULL image
%  is returned if the image cannot be cloned; check exception for the reason
%  why.
%
%  The format of the CloneImage method is:
%
%      Image *CloneImage(const Image *image,const unsigned long columns,
%        const unsigned long rows,const unsigned int orphan,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o columns: The number of columns in the copied
%
%    o rows: The number of rows in the copied image.
%
%    o orphan:  With a value other than 0, the cloned image is an orphan.  An
%      orphan is a stand-alone image that is not assocated with an image list.
%      In effect, the next and previous members of the cloned image is set to
%      NULL.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImage(const Image *image,const unsigned long columns,
  const unsigned long rows,const unsigned int orphan,ExceptionInfo *exception)
{
  Image
    *clone_image;

  ImageAttribute
    *attribute;

  register long
    i;

  size_t
    length;

  /*
    Clone the image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_image=(Image *) AcquireMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    MagickError(ResourceLimitError,"Unable to clone image",
      "Memory allocation failed");
  *clone_image=(*image);
  clone_image->reference_count=1;
  clone_image->montage=(char *) NULL;
  clone_image->directory=(char *) NULL;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      length=Max(image->colors,MaxRGB+1)*sizeof(PixelPacket);
      clone_image->colormap=(PixelPacket *) AcquireMemory(length);
      if (clone_image->colormap == (PixelPacket *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      length=image->colors*sizeof(PixelPacket);
      (void) memcpy(clone_image->colormap,image->colormap,length);
    }
  clone_image->color_profile.name=AllocateString(image->color_profile.name);
  if (image->color_profile.length != 0)
    {
      /*
        Allocate and clone any ICM profile.
      */
      length=image->color_profile.length;
      clone_image->color_profile.info=(unsigned char *) AcquireMemory(length);
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      (void) memcpy(clone_image->color_profile.info,image->color_profile.info,
        length);
    }
  clone_image->iptc_profile.name=AllocateString(image->iptc_profile.name);
  if (image->iptc_profile.length != 0)
    {
      /*
        Allocate and clone any IPTC profile.
      */
      length=image->iptc_profile.length;
      clone_image->iptc_profile.info=(unsigned char *) AcquireMemory(length);
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      (void) memcpy(clone_image->iptc_profile.info,image->iptc_profile.info,
        length);
    }
  if (image->generic_profiles != 0)
    {
      /*
        Allocate and clone any generic profiles.
      */
      clone_image->generic_profile=(ProfileInfo *)
        AcquireMemory(image->generic_profiles*sizeof(ProfileInfo));
      if (clone_image->generic_profile == (ProfileInfo *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      length=image->generic_profiles*sizeof(ProfileInfo);
      (void) memcpy(clone_image->generic_profile,image->generic_profile,length);
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        clone_image->generic_profile[i].name=
          AllocateString(image->generic_profile[i].name);
        if (image->generic_profile[i].length == 0)
          continue;
        length=image->generic_profile[i].length;
        clone_image->generic_profile[i].info=
          (unsigned char *) AcquireMemory(length);
        if (clone_image->generic_profile[i].info == (unsigned char *) NULL)
          ThrowImageException(ResourceLimitWarning,"Unable to clone image",
            "Memory allocation failed");
        (void) memcpy(clone_image->generic_profile[i].info,
          image->generic_profile[i].info,length);
      }
    }
  clone_image->attributes=(ImageAttribute *) NULL;
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (ImageAttribute *) NULL; attribute=attribute->next)
    (void) SetImageAttribute(clone_image,attribute->key,attribute->value);
  GetExceptionInfo(&clone_image->exception);
  if ((columns != 0) || (rows != 0))
    {
      clone_image->page.width=columns;
      clone_image->page.height=rows;
      clone_image->page.x*=(int) ((double) columns/clone_image->columns);
      clone_image->page.y*=(int) ((double) rows/clone_image->rows);
      clone_image->clip_mask=(Image *) NULL;
      GetCacheInfo(&clone_image->cache);
      clone_image->columns=columns;
      clone_image->rows=rows;
    }
  else
    {
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
      if (clone_image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,True,exception);
      clone_image->cache=ReferenceCache(image->cache);
    }
  clone_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  if (clone_image->orphan || orphan)
    {
      clone_image->orphan=False;
      clone_image->exempt=True;
      clone_image->previous=(Image *) NULL;
      clone_image->next=(Image *) NULL;
    }
  else
    {
      /*
        Link image into image list.
      */
      if (clone_image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (clone_image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  return(clone_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e I m a g e I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneImageInfo() makes a copy of the given image info structure.  If
%  NULL is specified, a new image info structure is created initialized to
%  default values.
%
%  The format of the CloneImageInfo method is:
%
%      ImageInfo *CloneImageInfo(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport ImageInfo *CloneImageInfo(const ImageInfo *image_info)
{
  ImageInfo
    *clone_info;

  clone_info=(ImageInfo *) AcquireMemory(sizeof(ImageInfo));
  if (clone_info == (ImageInfo *) NULL)
    MagickError(ResourceLimitError,"Unable to clone image info",
      "Memory allocation failed");
  if (image_info == (ImageInfo *) NULL)
    {
      GetImageInfo(clone_info);
      return(clone_info);
    }
  *clone_info=(*image_info);
  if (image_info->size != (char *) NULL)
    clone_info->size=AllocateString(image_info->size);
  if (image_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(image_info->tile);
  if (image_info->page != (char *) NULL)
    clone_info->page=AllocateString(image_info->page);
  if (image_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(image_info->server_name);
  if (image_info->font != (char *) NULL)
    clone_info->font=AllocateString(image_info->font);
  if (image_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(image_info->texture);
  if (image_info->density != (char *) NULL)
    clone_info->density=AllocateString(image_info->density);
  if (image_info->view != (char *) NULL)
    clone_info->view=AllocateString(image_info->view);
  clone_info->blob=CloneBlobInfo(image_info->blob);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeImage() returns the second image composited onto the first at the
%  specified offsets.
%
%  The format of the CompositeImage method is:
%
%      unsigned int CompositeImage(Image *image,const CompositeOperator compose,
%        const Image *composite_image,const long x_offset,const long y_offset)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o compose: This operator affects how the composite is applied to
%      the image.  The default is Over.  Choose from these operators:
%
%        OverCompositeOp       InCompositeOp         OutCompositeOP
%        AtopCompositeOP       XorCompositeOP        PlusCompositeOP
%        MinusCompositeOP      AddCompositeOP        SubtractCompositeOP
%        DifferenceCompositeOP BumpmapCompositeOP    CopyCompositeOP
%        DisplaceCompositeOP
%
%    o composite_image: The composite image.
%
%    o x_offset: The column offset of the composited image.
%
%    o y_offset: The row offset of the composited image.
%
%
*/
MagickExport unsigned int CompositeImage(Image *image,
  const CompositeOperator compose,const Image *composite_image,
  const long x_offset,const long y_offset)
{
  double
    amount,
    blue,
    brightness,
    green,
    hue,
    midpoint,
    opacity,
    percent_brightness,
    percent_saturation,
    red,
    saturation,
    threshold;

  IndexPacket
    *composite_indexes,
    *indexes;

  long
    y;

  PixelPacket
    pixel;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Prepare composite image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(composite_image != (Image *) NULL);
  assert(composite_image->signature == MagickSignature);
  SetImageType(image,TrueColorType);
  switch (compose)
  {
    case DisplaceCompositeOp:
    {
      double
        x_displace,
        y_displace;

      double
        horizontal_scale,
        vertical_scale;

      Image
        *displace_image;

      register PixelPacket
        *r;

      /*
        Allocate the displace image.
      */
      displace_image=CloneImage(composite_image,0,0,True,&image->exception);
      if (displace_image == (Image *) NULL)
        return(False);
      horizontal_scale=20.0;
      vertical_scale=20.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the horizontal and vertical displacement scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &horizontal_scale,&vertical_scale);
          if (count == 1)
            vertical_scale=horizontal_scale;
        }
      /*
        Shift image pixels as defined by a displacement map.
      */
      for (y=0; y < (long) composite_image->rows; y++)
      {
        if (((y+y_offset) < 0) || ((y+y_offset) >= (long) image->rows))
          continue;
        p=AcquireImagePixels(composite_image,0,y,composite_image->columns,1,
          &image->exception);
        q=GetImagePixels(image,0,y+y_offset,image->columns,1);
        r=GetImagePixels(displace_image,0,y,displace_image->columns,1);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
            (r == (PixelPacket *) NULL))
          break;
        q+=x_offset;
        for (x=0; x < (long) composite_image->columns; x++)
        {
          if (((x_offset+x) < 0) || ((x_offset+x) >= (long) image->columns))
            {
              p++;
              q++;
              continue;
            }
          x_displace=(horizontal_scale*(Intensity(*p)-
            (0.5*(MaxRGB+1))))/(0.5*(MaxRGB+1));
          y_displace=x_displace;
          if (composite_image->matte)
            y_displace=(vertical_scale*((double) p->opacity-
              ((MaxRGB+1)/2)))/((MaxRGB+1)/2);
          *r=InterpolateColor(image,x_offset+x+x_displace,y_offset+y+y_displace,
            &image->exception);
          p++;
          q++;
          r++;
        }
        if (!SyncImagePixels(displace_image))
          break;
      }
      composite_image=displace_image;
      break;
    }
    case ModulateCompositeOp:
    {
      percent_saturation=50.0;
      percent_brightness=50.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the brightness and saturation scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &percent_brightness,&percent_saturation);
          if (count == 1)
            percent_saturation=percent_brightness;
        }
      percent_brightness/=100.0;
      percent_saturation/=100.0;
      break;
    }
    case ThresholdCompositeOp:
    {
      /*
        Determine the amount and threshold.
      */
      amount=0.5;
      threshold=0.05;
      if (composite_image->geometry != (char *) NULL)
        (void) sscanf(composite_image->geometry,"%lfx%lf\n",
          &amount,&threshold);
      threshold*=MaxRGB;
      break;
    }
    default:
      break;
  }
  /*
    Composite image.
  */
  midpoint=0x80;
  for (y=0; y < (long) image->rows; y++)
  {
    if (y < y_offset)
      continue;
    if ((y-y_offset) >= (long) composite_image->rows)
      break;
    p=AcquireImagePixels(composite_image,0,y-y_offset,composite_image->columns,
      1,&image->exception);
    q=GetImagePixels(image,0,y,image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    composite_indexes=GetIndexes(composite_image);
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (x < x_offset)
        {
          q++;
          continue;
        }
      if ((x-x_offset) >= (long) composite_image->columns)
        break;
      pixel=(*p);
      if (!composite_image->matte)
        pixel.opacity=OpaqueOpacity;
      if (!image->matte)
        q->opacity=OpaqueOpacity;
      switch (compose)
      {
        case ThresholdCompositeOp:
        {
          double
            blue,
            green,
            opacity,
            red;

          red=q->red-(double) pixel.red;
          if (fabs(2.0*red) < threshold)
            red=q->red;
          else
            red=q->red+(red*amount);
          green=q->green-(double) pixel.green;
          if (fabs(2.0*green) < threshold)
            green=q->green;
          else
            green=q->green+(green*amount);
          blue=q->blue-(double) pixel.blue;
          if (fabs(2.0*blue) < threshold)
            blue=q->blue;
          else
            blue=q->blue+(blue*amount);
          opacity=q->opacity-(double) pixel.opacity;
          if (fabs(2.0*opacity) < threshold)
            opacity=q->opacity;
          else
            opacity=q->opacity+(opacity*amount);
          q->red=(Quantum)
            ((red < 0.0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
          q->green=(Quantum)
            ((green < 0.0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
          q->blue=(Quantum)
            ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
          q->opacity=(Quantum)
            ((opacity < 0.0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5);
          break;
        }
        case ModulateCompositeOp:
        {
          int
            offset;

          offset=(int) (Intensity(pixel)-midpoint);
          if (offset == 0)
            break;
          TransformHSL(q->red,q->green,q->blue,&hue,&saturation,
            &brightness);
          brightness+=(percent_brightness*offset)/midpoint;
          if (brightness < 0.0)
            brightness=0.0;
          else
            if (brightness > 1.0)
              brightness=1.0;
          HSLTransform(hue,saturation,brightness,&q->red,&q->green,&q->blue);
          break;
        }
        default:
        {
          switch (compose)
          {
            case OverCompositeOp:
            default:
            {
              AlphaComposite(p,pixel.opacity,q,q->opacity);
              break;
            }
            case InCompositeOp:
            {
              q->red=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                (MaxRGB-q->opacity)*pixel.red/MaxRGB/MaxRGB+0.5);
              q->green=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                (MaxRGB-q->opacity)*pixel.green/MaxRGB/MaxRGB+0.5);
              q->blue=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                (MaxRGB-q->opacity)*pixel.blue/MaxRGB/MaxRGB+0.5);
              q->opacity=(Quantum) (MaxRGB-(double) (MaxRGB-pixel.opacity)*
                (MaxRGB-q->opacity)/MaxRGB+0.5);
              break;
            }
            case OutCompositeOp:
            {
              q->red=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                q->opacity*pixel.red/MaxRGB/MaxRGB+0.5);
              q->green=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                q->opacity*pixel.green/MaxRGB/MaxRGB+0.5);
              q->blue=(Quantum) ((double) (MaxRGB-pixel.opacity)*
                q->opacity*pixel.blue/MaxRGB/MaxRGB+0.5);
              q->opacity=(Quantum) (MaxRGB-(double) (MaxRGB-pixel.opacity)*
                q->opacity/MaxRGB+0.5);
              break;
            }
            case AtopCompositeOp:
            {
              red=((double) (MaxRGB-pixel.opacity)*(MaxRGB-q->opacity)*
                pixel.red/MaxRGB+(double) pixel.opacity*(MaxRGB-q->opacity)*
                q->red/MaxRGB)/MaxRGB;
              q->red=(Quantum) (red > MaxRGB ? MaxRGB : red+0.5);
              green=((double) (MaxRGB-pixel.opacity)*(MaxRGB-q->opacity)*
                pixel.green/MaxRGB+(double) pixel.opacity*(MaxRGB-q->opacity)*
                q->green/MaxRGB)/MaxRGB;
              q->green=(Quantum) (green > MaxRGB ? MaxRGB : green+0.5);
              blue=((double) (MaxRGB-pixel.opacity)*(MaxRGB-q->opacity)*
                pixel.blue/MaxRGB+(double) pixel.opacity*(MaxRGB-q->opacity)*
                q->blue/MaxRGB)/MaxRGB;
              q->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue+0.5);
              opacity=MaxRGB-((double) (MaxRGB-pixel.opacity)*
                (MaxRGB-q->opacity)+(double) pixel.opacity*
                (MaxRGB-q->opacity))/MaxRGB;
              q->opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity+0.5);
              break;
            }
            case XorCompositeOp:
            {
              red=((double) (MaxRGB-pixel.opacity)*q->opacity*pixel.red/MaxRGB+
                (double) pixel.opacity*(MaxRGB-q->opacity)*
                q->red/MaxRGB)/MaxRGB;
              q->red=(Quantum) (red > MaxRGB ? MaxRGB : red+0.5);
              green=((double) (MaxRGB-pixel.opacity)*q->opacity*pixel.green/
                MaxRGB+(double) pixel.opacity*(MaxRGB-q->opacity)*q->green/
                MaxRGB)/MaxRGB;
              q->green=(Quantum) (green > MaxRGB ? MaxRGB : green+0.5);
              blue=((double) (MaxRGB-pixel.opacity)*q->opacity*pixel.blue/
                MaxRGB+(double) pixel.opacity*(MaxRGB-q->opacity)*
                q->blue/MaxRGB)/MaxRGB;
              q->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue+0.5);
              opacity=MaxRGB-((double) (MaxRGB-pixel.opacity)*q->opacity+
                (double) pixel.opacity*(MaxRGB-q->opacity))/MaxRGB;
              q->opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity+0.5);
              break;
            }
            case PlusCompositeOp:
            {
              red=((double) (MaxRGB-pixel.opacity)*pixel.red+
                (double) (MaxRGB-q->opacity)*q->red)/MaxRGB;
              q->red=(Quantum) (red > MaxRGB ? MaxRGB : red+0.5);
              green=((double) (MaxRGB-pixel.opacity)*pixel.green+
                (double) (MaxRGB-q->opacity)*q->green)/MaxRGB;
              q->green=(Quantum) (green > MaxRGB ? MaxRGB : green+0.5);
              blue=((double) (MaxRGB-pixel.opacity)*pixel.blue+
                (double) (MaxRGB-q->opacity)*q->blue)/MaxRGB;
              q->blue=(Quantum) (blue > MaxRGB ? MaxRGB : blue+0.5);
              opacity=MaxRGB-((double) (MaxRGB-pixel.opacity)+
                (MaxRGB-q->opacity));
              q->opacity=(Quantum) (opacity > MaxRGB ? MaxRGB : opacity+0.5);
              break;
            }
            case MinusCompositeOp:
            {
              red=((double) (MaxRGB-pixel.opacity)*pixel.red-
                (double) (MaxRGB-q->opacity)*q->red)/MaxRGB;
              q->red=(Quantum) (red < 0 ? 0 : red+0.5);
              green=((double) (MaxRGB-pixel.opacity)*pixel.green-
                (double) (MaxRGB-q->opacity)*q->green)/MaxRGB;
              q->green=(Quantum) (green < 0 ? 0 : green+0.5);
              blue=((double) (MaxRGB-pixel.opacity)*pixel.blue-
                (double) (MaxRGB-q->opacity)*q->blue)/MaxRGB;
              q->blue=(Quantum) (blue < 0 ? 0 : blue+0.5);
              opacity=MaxRGB-((double) (MaxRGB-pixel.opacity)-
                (MaxRGB-q->opacity));
              q->opacity=(Quantum) (opacity < 0 ? 0 : opacity+0.5);
              break;
            }
            case AddCompositeOp:
            {
              red=(double) pixel.red+q->red;
              q->red=(Quantum) (red > MaxRGB ? red-=MaxRGB : red+0.5);
              green=(double) pixel.green+q->green;
              q->green=(Quantum) (green > MaxRGB ? green-=MaxRGB : green+0.5);
              blue=(double) pixel.blue+q->blue;
              q->blue=(Quantum) (blue > MaxRGB ? blue-=MaxRGB : blue+0.5);
              q->opacity=OpaqueOpacity;
              break;
            }
            case SubtractCompositeOp:
            {
              red=(double) pixel.red-q->red;
              q->red=(Quantum) (red < 0 ? red+=MaxRGB : red+0.5);
              green=(double) pixel.green-q->green;
              q->green=(Quantum) (green < 0 ? green+=MaxRGB : green+0.5);
              blue=(double) pixel.blue-q->blue;
              q->blue=(Quantum) (blue < 0 ? blue+=MaxRGB : blue+0.5);
              q->opacity=OpaqueOpacity;
              break;
            }
            case MultiplyCompositeOp:
            {
              q->red=(unsigned long) (pixel.red*q->red/MaxRGB);
              q->green=(unsigned long) (pixel.green*q->green/MaxRGB);
              q->blue=(unsigned long) (pixel.blue*q->blue/MaxRGB);
              q->opacity=(unsigned long) (pixel.opacity*q->opacity/MaxRGB+0.5);
              break;
            }
            case DifferenceCompositeOp:
            {
              q->red=(Quantum) AbsoluteValue((double) pixel.red-q->red);
              q->green=(Quantum) AbsoluteValue((double) pixel.green-q->green);
              q->blue=(Quantum) AbsoluteValue((double) pixel.blue-q->blue);
              q->opacity=(Quantum)
                AbsoluteValue((double) pixel.opacity-q->opacity);
              break;
            }
            case BumpmapCompositeOp:
            {
              q->red=(unsigned long) ((Intensity(pixel)*q->red)/MaxRGB);
              q->green=(unsigned long) ((Intensity(pixel)*q->green)/MaxRGB);
              q->blue=(unsigned long) ((Intensity(pixel)*q->blue)/MaxRGB);
              q->opacity=(unsigned long) (Intensity(pixel)*q->opacity/MaxRGB);
              break;
            }
            case CopyCompositeOp:
            {
              *q=pixel;
              break;
            }
            case CopyRedCompositeOp:
            {
              q->red=pixel.red;
              break;
            }
            case CopyGreenCompositeOp:
            {
              q->green=pixel.green;
              break;
            }
            case CopyBlueCompositeOp:
            {
              q->blue=pixel.blue;
              break;
            }
            case CopyOpacityCompositeOp:
            {
              q->opacity=pixel.opacity;
              break;
            }
            case ClearCompositeOp:
            {
              q->opacity=TransparentOpacity;
              break;
            }
            case DissolveCompositeOp:
            {
              q->red=(unsigned long) ((pixel.opacity*pixel.red+
                (MaxRGB-q->opacity)*q->red)/MaxRGB);
              q->green=(unsigned long) ((pixel.opacity*pixel.green+
                (MaxRGB-q->opacity)*q->green)/MaxRGB);
              q->blue=(unsigned long) ((pixel.opacity*pixel.blue+
                (MaxRGB-q->opacity)*q->blue)/MaxRGB);
              q->opacity=OpaqueOpacity;
              break;
            }
            case DisplaceCompositeOp:
            {
              *q=pixel;
              break;
            }
          }
          break;
        }
      }
      if ((indexes != (IndexPacket *) NULL) &&
          (composite_indexes != (IndexPacket *) NULL))
        indexes[x]=composite_indexes[x-x_offset];
      p++;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C y c l e C o l o r m a p I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CycleColormap() displaces an image's colormap by a given number of positions.
%  If you cycle the colormap a number of times you can produce a psychodelic
%  effect.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(Image *image,const int amount)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amount:  Offset of the colormap this much.
%
%
*/
MagickExport void CycleColormapImage(Image *image,const int amount)
{
#define CycleColormapImageText  "  Cycle image colormap...  "

  long
    index,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    SetImageType(image,PaletteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(indexes[x]+amount) % image->colors;
      if (index < 0)
        index+=image->colors;
      indexes[x]=(IndexPacket) index;
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DescribeImage() describes an image by printing its attributes to the
%  file.  Attributes include the image width, height, size, and others.
%
%  The format of the DescribeImage method is:
%
%      void DescribeImage(Image *image,FILE *file,const unsigned int verbose)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o file: The file, typically stdout.
%
%    o verbose: A value other than zero prints more detailed information
%      about the image.
%
%
*/
MagickExport void DescribeImage(Image *image,FILE *file,
  const unsigned int verbose)
{
  char
    color[MaxTextExtent];

  double
    elapsed_time,
    user_time;

  ExceptionInfo
    exception;

  Image
    *p;

  ImageAttribute
    *attribute;

  long
    y;

  MagickInfo
    *magick_info;

  register long
    i,
    x;

  unsigned int
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(file != (FILE *) NULL);
  elapsed_time=GetElapsedTime(&image->timer);
  user_time=GetUserTime(&image->timer);
  (void) ContinueTimer(&image->timer);
  if (!verbose)
    {
      /*
        Display detailed info about the image.
      */
      if (*image->magick_filename != '\0')
        if (LocaleCompare(image->magick_filename,image->filename) != 0)
          (void) fprintf(file,"%.1024s=>",image->magick_filename);
       if ((image->previous == (Image *) NULL) &&
           (image->next == (Image *) NULL) && (image->scene == 0))
        (void) fprintf(file,"%.1024s ",image->filename);
      else
        (void) fprintf(file,"%.1024s[%lu] ",image->filename,image->scene);
      (void) fprintf(file,"%.1024s ",image->magick);
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          (void) fprintf(file,"%lux%lu=>",image->magick_columns,
            image->magick_rows);
      if ((image->page.width <= 1) || (image->page.height <= 1))
        (void) fprintf(file,"%lux%lu ",image->columns,image->rows);
      else
        (void) fprintf(file,"%lux%lu%+ld%+ld ",image->page.width,
          image->page.height,image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            {
              if (image->total_colors >= (1 << 24))
                (void) fprintf(file,"%lumc ",image->total_colors/1024/1024);
              else
                if (image->total_colors >= (1 << 16))
                  (void) fprintf(file,"%lukc ",image->total_colors/1024);
                else
                  (void) fprintf(file,"%luc ",image->total_colors);
            }
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %luc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%luc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%ld/%.6f/%.6fe ",
              (long) image->mean_error_per_pixel,image->normalized_mean_error,
              image->normalized_maximum_error);
          }
      (void) fprintf(file,"%lu-bit ",image->depth);
      if (SizeBlob(image) != 0)
        {
          if (SizeBlob(image) >= (1 << 24))
            (void) fprintf(file,"%lumb ",
              (unsigned long) (SizeBlob(image)/1024/1024));
          else
            if (SizeBlob(image) >= (1 << 16))
              (void) fprintf(file,"%lukb ",
                (unsigned long) (SizeBlob(image)/1024));
            else
              (void) fprintf(file,"%lub ",(unsigned long) SizeBlob(image));
        }
      (void) fprintf(file,"%.1fu %d:%02d\n",user_time,(int) (elapsed_time/60.0),
        (int) ceil(fmod(elapsed_time,60.0)));
      return;
    }
  /*
    Display verbose info about the image.
  */
  (void) SignatureImage(image);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  GetExceptionInfo(&exception);
  magick_info=(MagickInfo *) GetMagickInfo(image->magick,&exception);
  if ((magick_info == (MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  Format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  Format: %.1024s (%.1024s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  Geometry: %lux%lu\n",image->columns,image->rows);
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Class: DirectClass\n");
  else
    (void) fprintf(file,"  Class: PseudoClass\n");
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  Base Geometry: %lux%lu\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  Type: ");
  switch (GetImageType(image,&image->exception))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case GrayscaleMatteType:
      (void) fprintf(file,"grayscale with transparency"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case PaletteMatteType:
      (void) fprintf(file,"palette with transparency"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case TrueColorMatteType:
      (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    case ColorSeparationMatteType:
      (void) fprintf(file,"color separated with transparency"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  (void) fprintf(file,"  Depth: %lu bits-per-pixel component\n",
    GetImageDepth(image,&image->exception));
  x=0;
  p=(Image *) NULL;
  if ((image->matte && (strcmp(image->magick,"GIF") != 0)) || image->taint)
    {
      register const PixelPacket
        *p;

      p=(PixelPacket *) NULL;
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if (p->opacity == TransparentOpacity)
            break;
          p++;
        }
        if (x < (long) image->columns)
          break;
      }
      if ((x < (long) image->columns) || (y < (long) image->rows))
        (void) fprintf(file,"  Opacity: (%5d,%5d,%5d)\t#%04x%04x%04x\n",
          p->red,p->green,p->blue,p->red,p->green,p->blue);
    }
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Colors: %lu\n",image->total_colors);
  else
    if (image->total_colors <= image->colors)
      (void) fprintf(file,"  Colors: %lu\n",image->colors);
    else
      (void) fprintf(file,"  Colors: %lu=>%lu\n",image->total_colors,
        image->colors);
  if (image->storage_class == DirectClass)
    {
      if (image->total_colors < 1024)
        (void) GetNumberColors(image,file,&image->exception);
    }
  else
    {
      char
        name[MaxTextExtent];

      register PixelPacket
        *p;

      /*
        Display image colormap.
      */
      p=image->colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        (void) fprintf(file,"    %lu: (%5d,%5d,%5d)",i,p->red,p->green,p->blue);
        (void) fprintf(file,"\t");
        (void) QueryColorname(image,p,AllCompliance,name);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->mean_error_per_pixel != 0.0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %ld\n",
      (long) image->mean_error_per_pixel);
  if (image->normalized_mean_error != 0.0)
    (void) fprintf(file,"  Normalized Mean Exception: %g\n",
      image->normalized_mean_error);
  if (image->normalized_maximum_error != 0.0)
    (void) fprintf(file,"  Normalized Maximum Exception: %gn",
      image->normalized_maximum_error);
  if (image->rendering_intent == SaturationIntent)
    (void) fprintf(file,"  Rendering-Intent: saturation\n");
  else
    if (image->rendering_intent == PerceptualIntent)
      (void) fprintf(file,"  Rendering-Intent: perceptual\n");
    else
      if (image->rendering_intent == AbsoluteIntent)
        (void) fprintf(file,"  Rendering-Intent: absolute\n");
    else
      if (image->rendering_intent == RelativeIntent)
        (void) fprintf(file,"  Rendering-Intent: relative\n");
  if (image->gamma != 0.0)
    (void) fprintf(file,"  Gamma: %g\n",image->gamma);
  if (image->chromaticity.white_point.x != 0.0)
    {
      /*
        Display image chromaticity.
      */
      (void) fprintf(file,"  Chromaticity:\n");
      (void) fprintf(file,"    red primary: (%g,%g)\n",
        image->chromaticity.red_primary.x,image->chromaticity.red_primary.y);
      (void) fprintf(file,"    green primary: (%g,%g)\n",
        image->chromaticity.green_primary.x,
        image->chromaticity.green_primary.y);
      (void) fprintf(file,"    blue primary: (%g,%g)\n",
        image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y);
      (void) fprintf(file,"    white point: (%g,%g)\n",
        image->chromaticity.white_point.x,image->chromaticity.white_point.y);
    }
  if (image->color_profile.length != 0)
    (void) fprintf(file,"  Profile-color: %lu bytes\n",(unsigned long)
      image->color_profile.length);
  if (image->iptc_profile.length != 0)
    {
      char
        *tag,
        *text;

      size_t
        length;

      /*
        Describe IPTC data.
      */
      (void) fprintf(file,"  Profile-iptc: %lu bytes\n",(unsigned long)
        image->iptc_profile.length);
      for (i=0; i < (long) image->iptc_profile.length; )
      {
        if (image->iptc_profile.info[i] != 0x1c)
          {
            i++;
            continue;
          }
        i++;  /* skip file separator */
        i++;  /* skip record number */
        switch (image->iptc_profile.info[i])
        {
          case 5: tag=(char *) "Image Name"; break;
          case 7: tag=(char *) "Edit Status"; break;
          case 10: tag=(char *) "Priority"; break;
          case 15: tag=(char *) "Category"; break;
          case 20: tag=(char *) "Supplemental Category"; break;
          case 22: tag=(char *) "Fixture Identifier"; break;
          case 25: tag=(char *) "Keyword"; break;
          case 30: tag=(char *) "Release Date"; break;
          case 35: tag=(char *) "Release Time"; break;
          case 40: tag=(char *) "Special Instructions"; break;
          case 45: tag=(char *) "Reference Service"; break;
          case 47: tag=(char *) "Reference Date"; break;
          case 50: tag=(char *) "Reference Number"; break;
          case 55: tag=(char *) "Created Date"; break;
          case 60: tag=(char *) "Created Time"; break;
          case 65: tag=(char *) "Originating Program"; break;
          case 70: tag=(char *) "Program Version"; break;
          case 75: tag=(char *) "Object Cycle"; break;
          case 80: tag=(char *) "Byline"; break;
          case 85: tag=(char *) "Byline Title"; break;
          case 90: tag=(char *) "City"; break;
          case 95: tag=(char *) "Province State"; break;
          case 100: tag=(char *) "Country Code"; break;
          case 101: tag=(char *) "Country"; break;
          case 103: tag=(char *) "Original Transmission Reference"; break;
          case 105: tag=(char *) "Headline"; break;
          case 110: tag=(char *) "Credit"; break;
          case 115: tag=(char *) "Source"; break;
          case 116: tag=(char *) "Copyright String"; break;
          case 120: tag=(char *) "Caption"; break;
          case 121: tag=(char *) "Local Caption"; break;
          case 122: tag=(char *) "Caption Writer"; break;
          case 200: tag=(char *) "Custom Field 1"; break;
          case 201: tag=(char *) "Custom Field 2"; break;
          case 202: tag=(char *) "Custom Field 3"; break;
          case 203: tag=(char *) "Custom Field 4"; break;
          case 204: tag=(char *) "Custom Field 5"; break;
          case 205: tag=(char *) "Custom Field 6"; break;
          case 206: tag=(char *) "Custom Field 7"; break;
          case 207: tag=(char *) "Custom Field 8"; break;
          case 208: tag=(char *) "Custom Field 9"; break;
          case 209: tag=(char *) "Custom Field 10"; break;
          case 210: tag=(char *) "Custom Field 11"; break;
          case 211: tag=(char *) "Custom Field 12"; break;
          case 212: tag=(char *) "Custom Field 13"; break;
          case 213: tag=(char *) "Custom Field 14"; break;
          case 214: tag=(char *) "Custom Field 15"; break;
          case 215: tag=(char *) "Custom Field 16"; break;
          case 216: tag=(char *) "Custom Field 17"; break;
          case 217: tag=(char *) "Custom Field 18"; break;
          case 218: tag=(char *) "Custom Field 19"; break;
          case 219: tag=(char *) "Custom Field 20"; break;
          default: tag=(char *) "unknown"; break;
        }
        i++;
        (void) fprintf(file,"    %.1024s:\n",tag);
        length=image->iptc_profile.info[i++] << 8;
        length|=image->iptc_profile.info[i++];
        text=(char *) AcquireMemory(length+1);
        if (text != (char *) NULL)
          {
            char
              **textlist;

            register long
              j;

            (void) strncpy(text,(char *) image->iptc_profile.info+i,length);
            text[length]='\0';
            textlist=StringToList(text);
            if (textlist != (char **) NULL)
              {
                for (j=0; textlist[j] != (char *) NULL; j++)
                {
                  (void) fprintf(file,"  %s\n",textlist[j]);
                  LiberateMemory((void **) &textlist[j]);
                }
                LiberateMemory((void **) &textlist);
              }
            LiberateMemory((void **) &text);
          }
        i+=length;
      }
    }
  for (i=0; i < (long) image->generic_profiles; i++)
  {
    if (image->generic_profile[i].length == 0)
      continue;
    (void) fprintf(file,"  Profile-%.1024s: %lu bytes\n",
      image->generic_profile[i].name == (char *) NULL ? "generic" :
      image->generic_profile[i].name,(unsigned long)
      image->generic_profile[i].length);
  }
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile Geometry: %lux%lu%+ld%+ld\n",
      image->tile_info.width,image->tile_info.height,image->tile_info.x,
      image->tile_info.y);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    {
      /*
        Display image resolution.
      */
      (void) fprintf(file,"  Resolution: %gx%g",image->x_resolution,
        image->y_resolution);
      if (image->units == UndefinedResolution)
        (void) fprintf(file," pixels\n");
      else
        if (image->units == PixelsPerInchResolution)
          (void) fprintf(file," pixels/inch\n");
        else
          if (image->units == PixelsPerCentimeterResolution)
            (void) fprintf(file," pixels/centimeter\n");
          else
            (void) fprintf(file,"\n");
    }
  if (SizeBlob(image) >= (1 << 24))
    (void) fprintf(file,"  Filesize: %lumb\n",
      (unsigned long) (SizeBlob(image)/1024/1024));
  else
    if (SizeBlob(image) >= (1 << 16))
      (void) fprintf(file,"  Filesize: %lukb\n",
        (unsigned long) (SizeBlob(image)/1024));
    else
      (void) fprintf(file,"  Filesize: %lub\n",(unsigned long) SizeBlob(image));
  if (image->interlace == NoInterlace)
    (void) fprintf(file,"  Interlace: None\n");
  else
    if (image->interlace == LineInterlace)
      (void) fprintf(file,"  Interlace: Line\n");
    else
      if (image->interlace == PlaneInterlace)
        (void) fprintf(file,"  Interlace: Plane\n");
    else
      if (image->interlace == PartitionInterlace)
        (void) fprintf(file,"  Interlace: Partition\n");
  (void) QueryColorname(image,&image->background_color,AllCompliance,color);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->border_color,AllCompliance,color);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->matte_color,AllCompliance,color);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page Geometry: %lux%lu%+ld%+ld\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  if (image->dispose != 0)
    (void) fprintf(file,"  Dispose Method: %lu\n",image->dispose);
  if (image->delay != 0)
    (void) fprintf(file,"  Delay: %lu\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  Iterations: %lu\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  Scene: %lu of %u\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  Scene: %lu\n",image->scene);
  (void) fprintf(file,"  Compression: ");
  switch (image->compression)
  {
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group 4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
    case LosslessJPEGCompression: (void) fprintf(file,"Lossless JPEG\n");
         break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RunlengthEncodedCompression:
      (void) fprintf(file,"Runlength Encoded\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  Montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
      ExceptionInfo
        exception;

      Image
        *tile;

      ImageInfo
        *image_info;

      register char
        *p,
        *q;

      WarningHandler
        handler;

      /*
        Display visual image directory.
      */
      image_info=CloneImageInfo((ImageInfo *) NULL);
      (void) CloneString(&image_info->size,"64x64");
      (void) fprintf(file,"  Directory:\n");
      for (p=image->directory; *p != '\0'; p++)
      {
        q=p;
        while ((*q != '\n') && (*q != '\0'))
          q++;
        (void) strncpy(image_info->filename,p,q-p);
        image_info->filename[q-p]='\0';
        p=q;
        (void) fprintf(file,"    %.1024s",image_info->filename);
        handler=SetWarningHandler((WarningHandler) NULL);
        tile=ReadImage(image_info,&exception);
        if (exception.severity != UndefinedException)
          MagickWarning(exception.severity,exception.reason,
            exception.description);
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %lux%lu %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        (void) SignatureImage(tile);
        attribute=GetImageAttribute(tile,(char *) NULL);
        for ( ; attribute != (ImageAttribute *) NULL; attribute=attribute->next)
        {
          if (*attribute->key == '[')
            continue;
          (void) fprintf(file,"  %.1024s:\n",attribute->key);
          (void) fprintf(file,"%s\n",attribute->value);
        }
        DestroyImage(tile);
      }
      DestroyImageInfo(image_info);
    }
  /*
    Display image attributes.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (ImageAttribute *) NULL; attribute=attribute->next)
  {
    if (*attribute->key == '[')
      continue;
    (void) fprintf(file,"  %.1024s: ",attribute->key);
    (void) fprintf(file,"%s\n",attribute->value);
  }
  if (image->taint)
    (void) fprintf(file,"  Tainted: True\n");
  else
    (void) fprintf(file,"  Tainted: False\n");
  if (user_time != 0.0)
    (void) fprintf(file,"  User Time: %.1fu\n",user_time);
  if (elapsed_time != 0.0)
    (void) fprintf(file,"  Elapsed Time: %d:%02d\n",
      (int) (elapsed_time/60.0),(int) ceil(fmod(elapsed_time,60.0)));
  (void) fflush(file);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImage() dereferences an image, deallocating memory associated with
%  the image if the reference count becomes zero.
%
%  The format of the DestroyImage method is:
%
%      void DestroyImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyImage(Image *image)
{
  int
    destroy;

  register long
    i;

  /*
    Dereference image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  destroy=False;
  AcquireSemaphoreInfo(&image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=True;
  LiberateSemaphoreInfo(&image->semaphore);
  if (!destroy)
    return;
  /*
    Destroy image.
  */
  CloseBlob(image);
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  if (image->montage != (char *) NULL)
    LiberateMemory((void **) &image->montage);
  if (image->directory != (char *) NULL)
    LiberateMemory((void **) &image->directory);
  if (image->colormap != (PixelPacket *) NULL)
    LiberateMemory((void **) &image->colormap);
  if (image->color_profile.name != (char *) NULL)
    LiberateMemory((void **) &image->color_profile.name);
  if (image->color_profile.length != 0)
    LiberateMemory((void **) &image->color_profile.info);
  if (image->iptc_profile.name != (char *) NULL)
    LiberateMemory((void **) &image->iptc_profile.name);
  if (image->iptc_profile.length != 0)
    LiberateMemory((void **) &image->iptc_profile.info);
  if (image->generic_profiles != 0)
    {
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        if (image->generic_profile[i].name != (char *) NULL)
          LiberateMemory((void **) &image->generic_profile[i].name);
        if (image->generic_profile[i].length != 0)
          LiberateMemory((void **) &image->generic_profile[i].info);
      }
      LiberateMemory((void **) &image->generic_profile);
    }
  DestroyImageAttributes(image);
  DestroyImagePixels(image);
  DestroyExceptionInfo(&image->exception);
  DestroyBlobInfo(image->blob);
  if (image->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(image->semaphore);
  if (!image->orphan)
    {
      /*
        Unlink from linked list.
      */
      if (image->previous != (Image *) NULL)
        {
          if (image->next != (Image *) NULL)
            image->previous->next=image->next;
          else
            image->previous->next=(Image *) NULL;
        }
      if (image->next != (Image *) NULL)
        {
          if (image->previous != (Image *) NULL)
            image->next->previous=image->previous;
          else
            image->next->previous=(Image *) NULL;
        }
    }
  LiberateMemory((void **) &image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyImageInfo() deallocates memory associated with an ImageInfo structure.
%
%  The format of the DestroyImageInfo method is:
%
%      void DestroyImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->size != (char *) NULL)
    LiberateMemory((void **) &image_info->size);
  if (image_info->tile != (char *) NULL)
    LiberateMemory((void **) &image_info->tile);
  if (image_info->page != (char *) NULL)
    LiberateMemory((void **) &image_info->page);
  if (image_info->server_name != (char *) NULL)
    LiberateMemory((void **) &image_info->server_name);
  if (image_info->font != (char *) NULL)
    LiberateMemory((void **) &image_info->font);
  if (image_info->texture != (char *) NULL)
    LiberateMemory((void **) &image_info->texture);
  if (image_info->density != (char *) NULL)
    LiberateMemory((void **) &image_info->density);
  if (image_info->view != (char *) NULL)
    LiberateMemory((void **) &image_info->view);
  DestroyBlobInfo(image_info->blob);
  LiberateMemory((void **) &image_info);
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
%  DestroyImages() deallocates memory associated with all the images in a
%  sequence.
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
  Image
    *next;

  /*
    Proceed to the top of the image list.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  do
  {
    /*
      Destroy this image.
    */
    next=image->next;
    if (next != (Image *)NULL)
      next->previous=(Image *) NULL;
    DestroyImage(image);
    image=next;
  } while (image != (Image *) NULL);
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p l a y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% DisplayImages() displays an image sequence to any X window
% screen.  It returns a value other than 0 if successful.  Check the
% exception member of image to determine the reason for any failure.
%
%  The format of the AllocateNextImage method is:
%
%      unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%
*/
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;

  Image
    *next;

  unsigned long
    state;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  (void) XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) XDisplayImage(display,&resource,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  (void) XCloseDisplay(display);
  return(image->exception.severity != UndefinedException);
}
#else
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateWarning,"X11 library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I m a g e B o u n d i n g B o x                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageBoundingBox returns the bounding box of an image canvas.
%
%  The format of the GetImageBoundingBox method is:
%
%      RectangleInfo GetImageBoundingBox(const Image *image,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o bounds: Method GetImageBoundingBox returns the bounding box of an
%      image canvas.
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport RectangleInfo GetImageBoundingBox(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  PixelPacket
    corners[3];

  RectangleInfo
    bounds;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  bounds.width=0;
  bounds.height=0;
  bounds.x=image->columns;
  bounds.y=image->rows;
  corners[0]=AcquireOnePixel(image,0,0,exception);
  corners[1]=AcquireOnePixel(image,image->columns-1,0,exception);
  corners[2]=AcquireOnePixel(image,0,image->rows-1,exception);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    if (image->matte)
      for (x=0; x < (long) image->columns; x++)
      {
        if (p->opacity != corners[0].opacity)
          if (x < bounds.x)
            bounds.x=x;
        if (p->opacity != corners[1].opacity)
          if (x > (long) bounds.width)
            bounds.width=x;
        if (p->opacity != corners[0].opacity)
          if (y < bounds.y)
            bounds.y=y;
        if (p->opacity != corners[2].opacity)
          if (y > (long) bounds.height)
            bounds.height=y;
        p++;
      }
    else
      for (x=0; x < (long) image->columns; x++)
      {
        if (!ColorMatch(*p,corners[0],image->fuzz))
          if (x < bounds.x)
            bounds.x=x;
        if (!ColorMatch(*p,corners[1],image->fuzz))
          if (x > (long) bounds.width)
            bounds.width=x;
        if (!ColorMatch(*p,corners[0],image->fuzz))
          if (y < bounds.y)
            bounds.y=y;
        if (!ColorMatch(*p,corners[2],image->fuzz))
          if (y > (long) bounds.height)
            bounds.height=y;
        p++;
      }
  }
  if ((bounds.width != 0) || (bounds.height != 0))
    {
      bounds.width-=(bounds.x-1);
      bounds.height-=(bounds.y-1);
    }
  if (bounds.x < 0)
    bounds.x=0;
  if (bounds.y < 0)
    bounds.y=0;
  return(bounds);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageDepth() returns the depth of the image, either 8 or 16 bits.  By
%  default, pixels components are stored as 16-bit two byte unsigned short
%  integers that range in value from 0 to 65535.  However, if all the pixels
%  have lower-order bytes of zero, the image is considered to have a depth of
%  8-bit.
%
%  The format of the GetImageDepth method is:
%
%      unsigned long GetImageDepth(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned long GetImageDepth(const Image *image,
  ExceptionInfo *exception)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (QuantumDepth == 8)
    return(QuantumDepth);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      if (p->red != UpScale(DownScale(p->red)))
        return(QuantumDepth);
      if (p->green != UpScale(DownScale(p->green)))
        return(QuantumDepth);
      if (p->blue != UpScale(DownScale(p->blue)))
        return(QuantumDepth);
      if (image->matte)
        if (p->opacity != UpScale(DownScale(p->opacity)))
          return(QuantumDepth);
      p++;
    }
  }
  return(8);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageInfo() initializes the ImageInfo structure to default values.
%
%  The format of the GetImageInfo method is:
%
%      void GetImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%
*/
MagickExport void GetImageInfo(ImageInfo *image_info)
{
  /*
    File and image dimension members.
  */
  assert(image_info != (ImageInfo *) NULL);
  (void) memset(image_info,0,sizeof(ImageInfo));
  image_info->blob=CloneBlobInfo((BlobInfo *) NULL);
  image_info->adjoin=True;
  image_info->depth=QuantumDepth;
  image_info->interlace=NoInterlace;
  image_info->quality=DefaultCompressionQuality;
  image_info->antialias=True;
  image_info->pointsize=12;
  (void) QueryColorDatabase("#000000ff",&image_info->pen);
  (void) QueryColorDatabase("#ffffff",&image_info->background_color);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color);
  image_info->dither=True;
  image_info->preview_type=JPEGPreview;
  image_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageType() returns the type of image:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%
%
%  The format of the GetImageType method is:
%
%      ImageType GetImageType(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport ImageType GetImageType(const Image *image,ExceptionInfo *exception)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == CMYKColorspace)
    {
      if (!image->matte)
        return(ColorSeparationType);
      return(ColorSeparationMatteType);
    }
  if (IsGrayImage(image,exception))
    {
      if (IsMonochromeImage(image,exception))
        return(BilevelType);
      if (image->matte)
        return(GrayscaleMatteType);
      return(GrayscaleType);
    }
  if (IsPaletteImage(image,exception))
    {
      if (image->matte)
        return(PaletteMatteType);
      return(PaletteType);
    }
  if (!IsOpaqueImage(image,exception))
    return(TrueColorMatteType);
  return(TrueColorType);
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
%  GetNextImage() returns the next image in an image sequence.
%
%  The format of the GetNextImage method is:
%
%      Image *GetNextImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport Image *GetNextImage(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  CloseImagePixels(image);
  if (image->next == (Image *) NULL)
    return((Image *) NULL);
  *image->next->blob=(*image->blob);
  image->next->file=image->file;
  return(image->next);
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
%  Method GetNumberScenes returns the number of images in an image sequence.
%
%  The format of the GetNumberScenes method is:
%
%      unsigned int GetNumberScenes(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int GetNumberScenes(const Image *image)
{
  const Image
    *next;

  unsigned int
    number_scenes;

  /*
    Compute the number of scenes in the image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  next=image;
  for (number_scenes=0; next != (Image *) NULL; number_scenes++)
    next=next->next;
  return(number_scenes);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGeometry returns True if the geometry specification is valid
%  as determined by ParseGeometry.
%
%  The format of the IsGeometry method is:
%
%      unsigned int IsGeometry(const char *geometry)
%
%  A description of each parameter follows:
%
%    o status: Method IsGeometry returns True if the geometry specification
%      is valid otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%
*/
MagickExport unsigned int IsGeometry(const char *geometry)
{
  double
    value;

  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=ParseGeometry((char *) geometry,&x,&y,&width,&height);
  return((flags != NoValue) || (sscanf(geometry,"%lf",&value) > 0));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s I m a g e s E q u a l                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsImagesEqual() measures the difference between colors at each pixel
%  location of two images.  A value other than 0 means the colors match
%  exactly.  Otherwise an error measure is computed by summing over all
%  pixels in an image the distance squared in RGB space between each image
%  pixel and its corresponding pixel in the reference image.  The error
%  measure is assigned to these image members:
%
%    o mean_error_per_pixel:  This value is the mean error for any single
%      pixel in the image.
%
%    o normalized_mean_error:  This value is the normalized mean
%      quantization error for any single pixel in the image.  This distance
%      measure is normalized to a range between 0 and 1.  It is independent
%      of the range of red, green, and blue values in the image.
%
%    o normalized_maximum_error:  This value is the normalized
%      maximum quantization error for any single pixel in the image.  This
%      distance measure is normalized to a range between 0 and 1.  It is
%      independent of the range of red, green, and blue values in your image.
%
%  A small normalized mean square error, accessed as
%  image->normalized_mean_error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the IsImagesEqual method is:
%
%      unsigned int IsImagesEqual(Image *image,Image *reference)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o reference: The reference image.
%
%
*/
MagickExport unsigned int IsImagesEqual(Image *image,Image *reference)
{
  double
    distance,
    maximum_error_per_pixel,
    normalize,
    total_error;

  long
    y;

  register const PixelPacket
    *p;

  register double
    blue,
    green,
    opacity,
    red;

  register long
    x;

  register PixelPacket
    *q;

  /*
    Initialize measurement.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(reference != (Image *) NULL);
  assert(reference->signature == MagickSignature);
  (void) GetNumberColors(image,(FILE *) NULL,&image->exception);
  image->mean_error_per_pixel=0.0;
  image->normalized_mean_error=0.0;
  image->normalized_maximum_error=0.0;
  if ((image->rows != reference->rows) ||
      (image->columns != reference->columns) ||
      (image->interlace != reference->interlace) ||
      (image->colorspace != reference->colorspace) ||
      (image->matte != reference->matte))
    return(False);
  /*
    For each pixel, collect error statistics.
  */
  maximum_error_per_pixel=0;
  total_error=0;
  opacity=0;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    q=GetImagePixels(reference,0,y,reference->columns,1);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      red=(double) (p->red-q->red);
      green=(double) (p->green-q->green);
      blue=(double) (p->blue-q->blue);
      if (image->matte)
        opacity=(double) (p->opacity-q->opacity);
      distance=red*red+green*green+blue*blue+opacity*opacity;
      total_error+=distance;
      if (distance > maximum_error_per_pixel)
        maximum_error_per_pixel=distance;
      p++;
      q++;
    }
  }
  /*
    Compute final error statistics.
  */
  normalize=3.0*(MaxRGB+1)*(MaxRGB+1);
  if (image->matte)
    normalize=4.0*(MaxRGB+1)*(MaxRGB+1);
  image->mean_error_per_pixel=total_error/image->columns/image->rows;
  image->normalized_mean_error=image->mean_error_per_pixel/normalize;
  image->normalized_maximum_error=maximum_error_per_pixel/normalize;
  return(image->normalized_mean_error == 0.0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s I m a g e T a i n t e d                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsImageTainted() returns a value other than 0 if any pixel in an image
%  has been altered since it was first constituted.
%
%  The format of the IsImageTainted method is:
%
%      unsigned int IsImageTainted(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int IsImageTainted(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) strncpy(magick,image->magick,MaxTextExtent-1);
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  for (p=image; p != (Image *) NULL; p=p->next)
  {
    if (p->taint)
      return(True);
    if (LocaleCompare(p->magick,magick) != 0)
      return(True);
    if (LocaleCompare(p->filename,filename) != 0)
      return(True);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s S u b i m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSubimage returns True if the geometry is a valid subimage
%  specification (e.g. [1], [1-9], [1,7,4]).
%
%  The format of the IsSubimage method is:
%
%      unsigned int IsSubimage(const char *geometry,const unsigned int pedantic)
%
%  A description of each parameter follows:
%
%    o status: Method IsSubimage returns True if the geometry is a valid
%      subimage specification otherwise False is returned.
%
%    o geometry: This string is the geometry specification.
%
%    o pedantic: A value other than 0 invokes a more restriction set of
%      conditions for a valid specification (e.g. [1], [1-4], [4-1]).
%
%
*/
MagickExport unsigned int IsSubimage(const char *geometry,
  const unsigned int pedantic)
{
  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=ParseGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     L i s t T o G r o u p I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListToGroupImage() is a convenience method that converts a linked list of
%  images to a sequential array.  For example,
%
%    images = ListToGroupImage(image_list,&n);
%    for (i=0; i < n; i++)
%      puts(images[i]->filename);
%
%  The format of the ListToGroupImage method is:
%
%      Image **ListToGroupImage(Image *image,unsigned long *number_images)
%
%  A description of each parameter follows:
%
%    o image: The image list.
%
%    o number_images:  The length of the image array is returned here.
%
%
*/
MagickExport Image **ListToGroupImage(const Image *image,unsigned long *number_images)
{
  Image
    **images,
    *next;

  register long
    i;

  /*
    Determine the number of images in the list.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(number_images != (unsigned long *) NULL);
  next=(Image *) image;
  for (i=0; next != (Image *) NULL; i++)
    next=next->next;
  images=(Image **) AcquireMemory(i*sizeof(Image *));
  if (images == (Image **) NULL)
    MagickError(ResourceLimitWarning,"Unable to convert image list",
      "Memory allocation failed");
  *number_images=i;
  /*
    Add each image in the linked list to the group.
  */
  next=(Image *) image;
  for (i=0; next != (Image *) NULL; i++)
  {
    images[i]=next;
    next=next->next;
  }
  return(images);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o d i f y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ModifyImage() ensures that there is only a single reference to the image
%  to be modified, updating the provided image pointer to point to a clone of
%  the original image if necessary.
%
%  The format of the ModifyImage method is:
%
%      ModifyImage(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void ModifyImage(Image** image,ExceptionInfo *exception)
{
  Image
    *clone_image;

  unsigned int
    clone;

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  assert((*image)->signature == MagickSignature);
  clone=False;
  AcquireSemaphoreInfo(&(*image)->semaphore);
  if ((*image)->reference_count > 1)
    clone=True;
  LiberateSemaphoreInfo(&(*image)->semaphore);
  if (!clone)
    return;
  clone_image=CloneImage(*image,0,0,True,exception);
  AcquireSemaphoreInfo(&(*image)->semaphore);
  (*image)->reference_count--;
  LiberateSemaphoreInfo(&(*image)->semaphore);
  *image=clone_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImage applies image processing options to an image as
%  prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImage(const ImageInfo *image_info,const int argc,
%        char **argv,Image **image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImage(const ImageInfo *image_info,
  const int argc,char **argv,Image **image)
{
  char
    *geometry,
    *option;

  DrawInfo
    *draw_info;

  ExceptionInfo
    exception;

  Image
    *map_image,
    *region_image;

  ImageInfo
    *clone_info;

  int
    count,
    flags;

  long
    x,
    y;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    region_info;

  register int
    i;

  unsigned int
    matte;

  unsigned long
    height,
    width;

  /*
    Verify option length.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  for (i=1; i < argc; i++)
    if (strlen(argv[i]) > (MaxTextExtent/2-1))
      MagickError(OptionWarning,"Option length exceeds limit",argv[i]);
  /*
    Initialize method variables.
  */
  clone_info=CloneImageInfo(image_info);
  draw_info=CloneDrawInfo(clone_info,(DrawInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  geometry=(char *) NULL;
  map_image=(Image *) NULL;
  quantize_info.number_colors=0;
  quantize_info.tree_depth=0;
  quantize_info.dither=True;
  if (clone_info->monochrome)
    if (!IsMonochromeImage(*image,&(*image)->exception))
      {
        quantize_info.number_colors=2;
        quantize_info.tree_depth=8;
        quantize_info.colorspace=GRAYColorspace;
      }
  region_image=(Image *) NULL;
  region_info.width=(*image)->columns;
  region_info.height=(*image)->rows;
  region_info.x=0;
  region_info.y=0;
  /*
    Transmogrify the image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleNCompare("-affine",option,4) == 0)
          {
            char
              *p;

            /*
              Draw affine matrix.
            */
            p=argv[++i];
            draw_info->affine.sx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.rx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ry=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.sy=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.tx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ty=strtod(p,&p);
            break;
          }
        if (LocaleNCompare("antialias",option+1,3) == 0)
          {
            clone_info->antialias=(*option == '-');
            draw_info->stroke_antialias=(*option == '-');
            draw_info->text_antialias=(*option == '-');
            break;
          }
        break;
      }
      case 'b':
      {
        if (LocaleNCompare("-background",option,6) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->background_color);
            (*image)->background_color=clone_info->background_color;
            continue;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            blur_image=BlurImage(*image,radius,sigma,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleCompare("-border",option) == 0)
          {
            Image
              *border_image;

            RectangleInfo
              border_info;

            /*
              Surround image with a border of solid color.
            */
            border_info.width=0;
            border_info.height=0;
            border_info.x=0;
            border_info.y=0;
            flags=ParseGeometry(argv[++i],&border_info.x,&border_info.y,
              &border_info.width,&border_info.height);
            if ((flags & HeightValue) == 0)
              border_info.height=border_info.width;
            border_image=BorderImage(*image,&border_info,&(*image)->exception);
            if (border_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            SetImageType(border_image,TrueColorType);
            *image=border_image;
            continue;
          }
        if (LocaleNCompare("-bordercolor",option,8) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->border_color);
            draw_info->border_color=clone_info->border_color;
            (*image)->border_color=clone_info->border_color;
            continue;
          }
        if (LocaleCompare("-box",option) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->box);
            continue;
          }
        break;
      }
      case 'c':
      {
        if (LocaleNCompare("channel",option+1,4) == 0)
          {
            ChannelType
              channel;

            channel=UndefinedChannel;
            if (*option == '-')
              {
                option=argv[++i];
                if (LocaleCompare("Red",option) == 0)
                  channel=RedChannel;
                if (LocaleCompare("Cyan",option) == 0)
                  channel=CyanChannel;
                if (LocaleCompare("Green",option) == 0)
                  channel=GreenChannel;
                if (LocaleCompare("Magenta",option) == 0)
                  channel=MagentaChannel;
                if (LocaleCompare("Blue",option) == 0)
                  channel=BlueChannel;
                if (LocaleCompare("Yellow",option) == 0)
                  channel=YellowChannel;
                if (LocaleCompare("Opacity",option) == 0)
                  channel=OpacityChannel;
                if (LocaleCompare("Black",option) == 0)
                  channel=BlackChannel;
                if (LocaleCompare("Matte",option) == 0)
                  channel=MatteChannel;
              }
            if ((*image)->colorspace != clone_info->colorspace)
              (void) RGBTransformImage(*image,clone_info->colorspace);
            (void) ChannelImage(*image,channel);
            continue;
          }
        if (LocaleNCompare("-charcoal",option,4) == 0)
          {
            double
              radius,
              sigma;

            Image
              *charcoal_image;

            /*
              Charcoal image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            charcoal_image=CharcoalImage(*image,radius,sigma,
              &(*image)->exception);
            if (charcoal_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=charcoal_image;
            continue;
          }
        if (LocaleNCompare("-colorize",option,8) == 0)
          {
            Image
              *colorize_image;

            /*
              Colorize the image.
            */
            colorize_image=ColorizeImage(*image,argv[++i],draw_info->fill,
              &(*image)->exception);
            if (colorize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=colorize_image;
            continue;
          }
        if (LocaleNCompare("-compress",option,6) == 0)
          {
            if (*option == '-')
              {
                CompressionType
                  compression;

                option=argv[++i];
                compression=UndefinedCompression;
                if (LocaleCompare("None",option) == 0)
                  compression=NoCompression;
                if (LocaleCompare("BZip",option) == 0)
                  compression=BZipCompression;
                if (LocaleCompare("Fax",option) == 0)
                  compression=FaxCompression;
                if (LocaleCompare("Group4",option) == 0)
                  compression=Group4Compression;
                if (LocaleCompare("JPEG",option) == 0)
                  compression=JPEGCompression;
                if (LocaleCompare("Lossless",option) == 0)
                  compression=LosslessJPEGCompression;
                if (LocaleCompare("LZW",option) == 0)
                  compression=LZWCompression;
                if (LocaleCompare("RLE",option) == 0)
                  compression=RunlengthEncodedCompression;
                if (LocaleCompare("Zip",option) == 0)
                  compression=ZipCompression;
                (*image)->compression=compression;
              }
            continue;
          }
        if (LocaleCompare("-colors",option) == 0)
          {
            quantize_info.number_colors=atol(argv[++i]);
            continue;
          }
        if (LocaleNCompare("colorspace",option+1,7) == 0)
          {
            char
              type;

            type=(*option);
            option=argv[++i];
            if (LocaleCompare("cmyk",option) == 0)
              {
                (void) RGBTransformImage(*image,CMYKColorspace);
                quantize_info.colorspace=CMYKColorspace;
              }
            if (LocaleCompare("gray",option) == 0)
              {
                quantize_info.colorspace=GRAYColorspace;
                if (quantize_info.number_colors == 0)
                  quantize_info.number_colors=256;
                quantize_info.tree_depth=8;
              }
            if (LocaleCompare("ohta",option) == 0)
              quantize_info.colorspace=OHTAColorspace;
            if (LocaleCompare("rgb",option) == 0)
              {
                (void) TransformRGBImage(*image,RGBColorspace);
                quantize_info.colorspace=RGBColorspace;
              }
            if (LocaleCompare("srgb",option) == 0)
              quantize_info.colorspace=sRGBColorspace;
            if (LocaleCompare("transparent",option) == 0)
              quantize_info.colorspace=TransparentColorspace;
            if (LocaleCompare("xyz",option) == 0)
              quantize_info.colorspace=XYZColorspace;
            if (LocaleCompare("ycbcr",option) == 0)
              quantize_info.colorspace=YCbCrColorspace;
            if (LocaleCompare("ycc",option) == 0)
              quantize_info.colorspace=YCCColorspace;
            if (LocaleCompare("yiq",option) == 0)
              quantize_info.colorspace=YIQColorspace;
            if (LocaleCompare("ypbpr",option) == 0)
              quantize_info.colorspace=YPbPrColorspace;
            if (LocaleCompare("yuv",option) == 0)
              quantize_info.colorspace=YUVColorspace;
            clone_info->colorspace=quantize_info.colorspace;
            if (type == '+')
              (*image)->colorspace=clone_info->colorspace;
            continue;
          }
        if (LocaleNCompare("comment",option+1,4) == 0)
          {
            if (*option == '-')
              (void) SetImageAttribute(*image,"comment",argv[++i]);
            else
              (void) SetImageAttribute(*image,"comment",(char *) NULL);
            continue;
          }
        if (LocaleNCompare("contrast",option+1,3) == 0)
          {
            (void) ContrastImage(*image,(unsigned int) (*option == '-'));
            continue;
          }
        if (LocaleNCompare("-crop",option,3) == 0)
          {
            TransformImage(image,argv[++i],(char *) NULL);
            continue;
          }
        if (LocaleNCompare("-cycle",option,3) == 0)
          {
            /*
              Cycle an image colormap.
            */
            CycleColormapImage(*image,atoi(argv[++i]));
            continue;
          }
        break;
      }
      case 'd':
      {
        if (LocaleNCompare("debug",option+1,3) == 0)
          {
            draw_info->debug=(*option == '-');
            continue;
          }
        if (LocaleNCompare("-delay",option,4) == 0)
          {
            double
              maximum_delay,
              minimum_delay;

            /*
              Set image delay.
            */
            count=sscanf(argv[++i],"%lf-%lf",&minimum_delay,&maximum_delay);
            if (count == 1)
              (*image)->delay=(unsigned int) minimum_delay;
            else
              {
                if ((*image)->delay < minimum_delay)
                  (*image)->delay=(unsigned int) minimum_delay;
                if ((*image)->delay > maximum_delay)
                  (*image)->delay=(unsigned int) maximum_delay;
              }
            continue;
          }
        if (LocaleNCompare("-density",option,4) == 0)
          {
            /*
              Set image density.
            */
            (void) CloneString(&clone_info->density,argv[++i]);
            (void) CloneString(&draw_info->density,clone_info->density);
            count=sscanf(clone_info->density,"%lfx%lf",
              &(*image)->x_resolution,&(*image)->y_resolution);
            if (count != 2)
              (*image)->y_resolution=(*image)->x_resolution;
            continue;
          }
        if (LocaleNCompare("-depth",option,4) == 0)
          {
            (void) SetImageDepth(*image,atol(argv[++i]));
            continue;
          }
        if (LocaleNCompare("-despeckle",option,4) == 0)
          {
            Image
              *despeckle_image;

            /*
              Reduce the speckles within an image.
            */
            despeckle_image=DespeckleImage(*image,&(*image)->exception);
            if (despeckle_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=despeckle_image;
            continue;
          }
        if (LocaleNCompare("-display",option,6) == 0)
          {
            (void) CloneString(&clone_info->server_name,argv[++i]);
            (void) CloneString(&draw_info->server_name,clone_info->server_name);
            continue;
          }
        if (LocaleNCompare("-dispose",option,6) == 0)
          {
            /*
              Set image dispose.
            */
            (*image)->dispose=atol(argv[++i]);
            continue;
          }
        if (LocaleNCompare("dither",option+1,3) == 0)
          {
            clone_info->dither=(*option == '-');
            quantize_info.dither=(*option == '-');
            continue;
          }
        if (LocaleNCompare("-draw",option,3) == 0)
          {
            /*
              Draw image.
            */
            (void) CloneString(&draw_info->primitive,argv[++i]);
            (void) DrawImage(*image,draw_info);
            continue;
          }
        break;
      }
      case 'e':
      {
        if (LocaleNCompare("-edge",option,3) == 0)
          {
            double
              radius;

            Image
              *edge_image;

            /*
              Enhance edges in the image.
            */
            radius=atof(argv[++i]);
            edge_image=EdgeImage(*image,radius,&(*image)->exception);
            if (edge_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=edge_image;
            continue;
          }
        if (LocaleNCompare("-emboss",option,3) == 0)
          {
            double
              radius,
              sigma;

            Image
              *emboss_image;

            /*
              Emboss the image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            emboss_image=EmbossImage(*image,radius,sigma,&(*image)->exception);
            if (emboss_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=emboss_image;
            continue;
          }
        if (LocaleNCompare("-enhance",option,3) == 0)
          {
            Image
              *enhance_image;

            /*
              Enhance image.
            */
            enhance_image=EnhanceImage(*image,&(*image)->exception);
            if (enhance_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=enhance_image;
            continue;
          }
        if (LocaleNCompare("-equalize",option,3) == 0)
          {
            /*
              Equalize image.
            */
            (void) EqualizeImage(*image);
            continue;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("-fill",option) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->fill);
            continue;
          }
        if (LocaleNCompare("filter",option+1,4) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                option=argv[++i];
                filter=LanczosFilter;
                if (LocaleCompare("Point",option) == 0)
                  filter=PointFilter;
                if (LocaleCompare("Box",option) == 0)
                  filter=BoxFilter;
                if (LocaleCompare("Triangle",option) == 0)
                  filter=TriangleFilter;
                if (LocaleCompare("Hermite",option) == 0)
                  filter=HermiteFilter;
                if (LocaleCompare("Hanning",option) == 0)
                  filter=HanningFilter;
                if (LocaleCompare("Hamming",option) == 0)
                  filter=HammingFilter;
                if (LocaleCompare("Blackman",option) == 0)
                  filter=BlackmanFilter;
                if (LocaleCompare("Gaussian",option) == 0)
                  filter=GaussianFilter;
                if (LocaleCompare("Quadratic",option) == 0)
                  filter=QuadraticFilter;
                if (LocaleCompare("Cubic",option) == 0)
                  filter=CubicFilter;
                if (LocaleCompare("Catrom",option) == 0)
                  filter=CatromFilter;
                if (LocaleCompare("Mitchell",option) == 0)
                  filter=MitchellFilter;
                if (LocaleCompare("Lanczos",option) == 0)
                  filter=LanczosFilter;
                if (LocaleCompare("Bessel",option) == 0)
                  filter=BesselFilter;
                if (LocaleCompare("Sinc",option) == 0)
                  filter=SincFilter;
                (*image)->filter=filter;
              }
            continue;
          }
        if (LocaleNCompare("-flip",option,4) == 0)
          {
            Image
              *flip_image;

            /*
              Flip image scanlines.
            */
            flip_image=FlipImage(*image,&(*image)->exception);
            if (flip_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flip_image;
            continue;
          }
        if (LocaleNCompare("-flop",option,4) == 0)
          {
            Image
              *flop_image;

            /*
              Flop image scanlines.
            */
            flop_image=FlopImage(*image,&(*image)->exception);
            if (flop_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flop_image;
            continue;
          }
        if (LocaleCompare("-frame",option) == 0)
          {
            Image
              *frame_image;

            FrameInfo
              frame_info;

            /*
              Surround image with an ornamental border.
            */
            frame_info.width=0;
            frame_info.height=0;
            frame_info.outer_bevel=0;
            frame_info.inner_bevel=0;
            flags=ParseGeometry(argv[++i],&frame_info.outer_bevel,
              &frame_info.inner_bevel,&frame_info.width,&frame_info.height);
            if ((flags & HeightValue) == 0)
              frame_info.height=frame_info.width;
            if ((flags & XValue) == 0)
              frame_info.outer_bevel=(frame_info.width >> 2)+1;
            if ((flags & YValue) == 0)
              frame_info.inner_bevel=frame_info.outer_bevel;
            frame_info.x=frame_info.width;
            frame_info.y=frame_info.height;
            frame_info.width=(*image)->columns+(frame_info.width << 1);
            frame_info.height=(*image)->rows+(frame_info.height << 1);
            frame_image=FrameImage(*image,&frame_info,&(*image)->exception);
            if (frame_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            SetImageType(frame_image,TrueColorType);
            *image=frame_image;
            continue;
          }
        if (LocaleNCompare("-fuzz",option,3) == 0)
          {
            (*image)->fuzz=StringToDouble(argv[++i],MaxRGB);
            continue;
          }
        if (LocaleCompare("-font",option) == 0)
          {
            (void) CloneString(&clone_info->font,argv[++i]);
            (void) CloneString(&draw_info->font,clone_info->font);
            continue;
          }
        break;
      }
      case 'g':
      {
        if (LocaleNCompare("gamma",option+1,3) == 0)
          {
            if (*option == '+')
              (*image)->gamma=atof(argv[++i]);
            else
              (void) GammaImage(*image,argv[++i]);
            continue;
          }
        if (LocaleCompare("gaussian",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            blur_image=GaussianBlurImage(*image,radius,sigma,
              &(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleNCompare("-geometry",option,4) == 0)
          {
            Image
              *resize_image;

            /*
              Resize image.
            */
            width=(*image)->columns;
            height=(*image)->rows;
            x=0;
            y=0;
            (void) CloneString(&geometry,argv[++i]);
            (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
            if ((width == (*image)->columns) && (height == (*image)->rows))
              break;
            resize_image=ZoomImage(*image,width,height,&(*image)->exception);
            if (resize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=resize_image;
            continue;
          }
        if (LocaleNCompare("gravity",option+1,2) == 0)
          {
            draw_info->gravity=(GravityType) NorthWestGravity;
            if (*option == '-')
              {
                option=argv[++i];
                if (LocaleCompare("NorthWest",option) == 0)
                  draw_info->gravity=(GravityType) NorthWestGravity;
                if (LocaleCompare("North",option) == 0)
                  draw_info->gravity=(GravityType) NorthGravity;
                if (LocaleCompare("NorthEast",option) == 0)
                  draw_info->gravity=(GravityType) NorthEastGravity;
                if (LocaleCompare("West",option) == 0)
                  draw_info->gravity=(GravityType) WestGravity;
                if (LocaleCompare("Center",option) == 0)
                  draw_info->gravity=(GravityType) CenterGravity;
                if (LocaleCompare("East",option) == 0)
                  draw_info->gravity=(GravityType) EastGravity;
                if (LocaleCompare("SouthWest",option) == 0)
                  draw_info->gravity=(GravityType) SouthWestGravity;
                if (LocaleCompare("South",option) == 0)
                  draw_info->gravity=(GravityType) SouthGravity;
                if (LocaleCompare("SouthEast",option) == 0)
                  draw_info->gravity=(GravityType) SouthEastGravity;
              }
            continue;
          }
        break;
      }
      case 'i':
      {
        if (LocaleNCompare("-implode",option,4) == 0)
          {
            double
              amount;

            Image
              *implode_image;

            /*
              Implode image.
            */
            amount=atof(argv[++i]);
            implode_image=ImplodeImage(*image,amount,&(*image)->exception);
            if (implode_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=implode_image;
            continue;
          }
        if (LocaleNCompare("intent",option+1,5) == 0)
          {
            if (*option == '-')
              {
                RenderingIntent
                  rendering_intent;

                option=argv[++i];
                rendering_intent=UndefinedIntent;
                if (LocaleCompare("Absolute",option) == 0)
                  rendering_intent=AbsoluteIntent;
                if (LocaleCompare("Perceptual",option) == 0)
                  rendering_intent=PerceptualIntent;
                if (LocaleCompare("Relative",option) == 0)
                  rendering_intent=RelativeIntent;
                if (LocaleCompare("Saturation",option) == 0)
                  rendering_intent=SaturationIntent;
                (*image)->rendering_intent=rendering_intent;
              }
            continue;
          }
        break;
      }
      case 'l':
      {
        if (LocaleNCompare("label",option+1,3) == 0)
          {
            if (*option == '-')
              (void) SetImageAttribute(*image,"label",argv[++i]);
            else
              (void) SetImageAttribute(*image,"label",(char *) NULL);
            continue;
          }
        if (LocaleCompare("-linewidth",option) == 0)
          {
            draw_info->stroke_width=atof(argv[++i]);
            continue;
          }
        if (LocaleNCompare("-loop",option,3) == 0)
          {
            /*
              Set image iterations.
            */
            (*image)->iterations=atol(argv[++i]);
            continue;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("-map",option) == 0)
          {
            /*
              Transform image colors to match this set of colors.
            */
            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            map_image=ReadImage(clone_info,&exception);
            if (exception.severity != UndefinedException)
              MagickWarning(exception.severity,exception.reason,
                exception.description);
            continue;
          }
        if (LocaleCompare("matte",option+1) == 0)
          {
            if (*option == '-')
              if (!(*image)->matte)
                SetImageOpacity(*image,OpaqueOpacity);
            (*image)->matte=(*option == '-');
            continue;
          }
        if (LocaleNCompare("-mattecolor",option,7) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->matte_color);
            (*image)->matte_color=clone_info->matte_color;
            continue;
          }
        if (LocaleNCompare("-median",option,4) == 0)
          {
            Image
              *median_image;

            /*
              Median filter image.
            */
            median_image=
              MedianFilterImage(*image,atol(argv[++i]),&(*image)->exception);
            if (median_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=median_image;
            continue;
          }
        if (LocaleNCompare("-modulate",option,4) == 0)
          {
            (void) ModulateImage(*image,argv[++i]);
            continue;
          }
        if (LocaleNCompare("-monochrome",option,4) == 0)
          {
            clone_info->monochrome=True;
            quantize_info.number_colors=2;
            quantize_info.tree_depth=8;
            quantize_info.colorspace=GRAYColorspace;
            continue;
          }
        break;
      }
      case 'n':
      {
        if (LocaleNCompare("negate",option+1,3) == 0)
          {
            (void) NegateImage(*image,*option == '+');
            continue;
          }
        if (LocaleNCompare("noise",option+1,4) == 0)
          {
            Image
              *noisy_image;

            option=argv[++i];
            if (*option == '-')
              noisy_image=
                ReduceNoiseImage(*image,atol(option),&(*image)->exception);
            else
              {
                NoiseType
                  noise_type;

                /*
                  Add noise to image.
                */
                noise_type=UniformNoise;
                if (LocaleCompare("Gaussian",option) == 0)
                  noise_type=GaussianNoise;
                if (LocaleCompare("multiplicative",option) == 0)
                  noise_type=MultiplicativeGaussianNoise;
                if (LocaleCompare("impulse",option) == 0)
                  noise_type=ImpulseNoise;
                if (LocaleCompare("laplacian",option) == 0)
                  noise_type=LaplacianNoise;
                if (LocaleCompare("Poisson",option) == 0)
                  noise_type=PoissonNoise;
                noisy_image=AddNoiseImage(*image,noise_type,
                  &(*image)->exception);
              }
            if (noisy_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=noisy_image;
            continue;
          }
        if (LocaleNCompare("-normalize",option,4) == 0)
          {
            (void) NormalizeImage(*image);
            continue;
          }
        break;
      }
      case 'o':
      {
        if (LocaleNCompare("-opaque",option,3) == 0)
          {
            PixelPacket
              target;

            target=GetOnePixel(*image,0,0);
            (void) QueryColorDatabase(argv[++i],&target);
            (void) OpaqueImage(*image,target,draw_info->fill);
            continue;
          }
        break;
      }
      case 'p':
      {
        if (LocaleNCompare("-paint",option,4) == 0)
          {
            Image
              *paint_image;

            /*
              Oil paint image.
            */
            paint_image=OilPaintImage(*image,atol(argv[++i]),
              &(*image)->exception);
            if (paint_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=paint_image;
            continue;
          }
        if (LocaleCompare("-pen",option) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->pen);
            continue;
          }
        if (LocaleNCompare("pointsize",option+1,2) == 0)
          {
            clone_info->pointsize=atof(argv[++i]);
            draw_info->pointsize=clone_info->pointsize;
            continue;
          }
        if (LocaleNCompare("-process",option,5) == 0)
          {
            char
              *arguments,
              breaker,
              quote,
              *token;

            int
              next,
              status;

            size_t
              length;

            TokenInfo
              token_info;

            length=strlen(argv[++i]);
            token=(char *) AcquireMemory(length+1);
            if (token == (char *) NULL)
              continue;
            next=0;
            arguments=argv[i];
            status=Tokenizer(&token_info,0,token,length,arguments,
              (char *) "",(char *) "=",(char *) "\"",0,&breaker,&next,&quote);
            if (status == 0)
              {
                char
                  *argv;

                argv=&(arguments[next]);
                (void) ExecuteModuleProcess((const char *) token,*image,1,
                  &argv);
              }
            LiberateMemory((void **) &token);
            continue;
          }
        if (LocaleNCompare("profile",option+1,4) == 0)
          {
            if (*option == '+')
              {
                /*
                  Remove a ICM, IPTC, or generic profile from the image.
                */
                (void) ProfileImage(*image,argv[++i],(char *) NULL);
                continue;
              }
            /*
              Add a ICM, IPTC, or generic profile to the image.
            */
            (void) ProfileImage(*image,(char *) NULL,argv[++i]);
            continue;
          }
        break;
      }
      case 'r':
      {
        if (LocaleNCompare("raise",option+1,2) == 0)
          {
            RectangleInfo
              raise_info;

            /*
              Surround image with a raise of solid color.
            */
            raise_info.width=0;
            raise_info.height=0;
            raise_info.x=0;
            raise_info.y=0;
            flags=ParseGeometry(argv[++i],&raise_info.x,&raise_info.y,
              &raise_info.width,&raise_info.height);
            if ((flags & HeightValue) == 0)
              raise_info.height=raise_info.width;
            (void) RaiseImage(*image,&raise_info,*option == '-');
            continue;
          }
        if (LocaleNCompare("region",option+1,3) == 0)
          {
            Image
              *crop_image;

            if (region_image != (Image *) NULL)
              {
                /*
                  Composite region.
                */
                (void) CompositeImage(region_image,
                  (*image)->matte ?  OverCompositeOp : CopyCompositeOp,*image,
                  region_info.x,region_info.y);
                DestroyImage(*image);
                *image=region_image;
              }
            if (*option == '+')
              continue;
            /*
              Apply transformations to a selected region of the image.
            */
            width=(*image)->columns;
            height=(*image)->rows;
            region_info.x=0;
            region_info.y=0;
            flags=ParseGeometry(argv[++i],&region_info.x,&region_info.y,
              &width,&height);
            if ((flags & WidthValue) == 0)
              width=(unsigned long) ((long) (*image)->columns-region_info.x);
            if ((flags & HeightValue) == 0)
              height=(unsigned long) ((long) (*image)->rows-region_info.y);
            if ((width != 0) || (height != 0))
              {
                if ((flags & XNegative) != 0)
                  region_info.x+=(*image)->columns-width;
                if ((flags & YNegative) != 0)
                  region_info.y+=(*image)->rows-height;
              }
            if (strchr(argv[i],'%') != (char *) NULL)
              {
                /*
                  Region geometry is relative to image size.
                */
                x=0;
                y=0;
                (void) ParseImageGeometry(argv[i],&x,&y,&width,&height);
                if (width > (*image)->columns)
                  width=(*image)->columns;
                if (height > (*image)->rows)
                  height=(*image)->rows;
                region_info.x=width >> 1;
                region_info.y=height >> 1;
                width=(*image)->columns-width;
                height=(*image)->rows-height;
                flags|=XValue | YValue;
              }
            region_info.width=width;
            region_info.height=height;
            crop_image=CropImage(*image,&region_info,&(*image)->exception);
            if (crop_image == (Image *) NULL)
              break;
            region_image=(*image);
            *image=crop_image;
            continue;
          }
        if (LocaleNCompare("-roll",option,4) == 0)
          {
            Image
              *roll_image;

            /*
              Roll image.
            */
            x=0;
            y=0;
            flags=ParseGeometry(argv[++i],&x,&y,&width,&height);
            roll_image=RollImage(*image,x,y,&(*image)->exception);
            if (roll_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=roll_image;
            continue;
          }
        if (LocaleNCompare("-rotate",option,4) == 0)
          {
            double
              degrees;

            Image
              *rotate_image;

            /*
              Check for conditional image rotation.
            */
            i++;
            if (strchr(argv[i],'>') != (char *) NULL)
              if ((*image)->columns <= (*image)->rows)
                break;
            if (strchr(argv[i],'<') != (char *) NULL)
              if ((*image)->columns >= (*image)->rows)
                break;
            /*
              Rotate image.
            */
            degrees=atof(argv[i]);
            rotate_image=RotateImage(*image,degrees,&(*image)->exception);
            if (rotate_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=rotate_image;
            continue;
          }
        break;
      }
      case 's':
      {
        if (LocaleNCompare("-sample",option,3) == 0)
          {
            Image
              *sample_image;

            /*
              Sample image with pixel replication.
            */
            width=(*image)->columns;
            height=(*image)->rows;
            x=0;
            y=0;
            (void) ParseImageGeometry(argv[++i],&x,&y,&width,&height);
            sample_image=SampleImage(*image,width,height,&(*image)->exception);
            if (sample_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sample_image;
            continue;
          }
        if (LocaleNCompare("sans",option+1,2) == 0)
          if (*option == '-')
            i++;
        if (LocaleNCompare("-scale",option,4) == 0)
          {
            Image
              *scale_image;

            /*
              Resize image.
            */
            width=(*image)->columns;
            height=(*image)->rows;
            x=0;
            y=0;
            (void) CloneString(&geometry,argv[++i]);
            (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
            scale_image=ScaleImage(*image,width,height,&(*image)->exception);
            if (scale_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=scale_image;
            continue;
          }
        if (LocaleCompare("-scene",option) == 0)
          {
            (*image)->scene=atol(argv[++i]);
            continue;
          }
        if (LocaleNCompare("-segment",option,4) == 0)
          {
            double
              cluster_threshold,
              smoothing_threshold;

            /*
              Segment image.
            */
            cluster_threshold=1.0;
            smoothing_threshold=1.5;
            (void) sscanf(argv[++i],"%lfx%lf",&cluster_threshold,
              &smoothing_threshold);
            (void) SegmentImage(*image,quantize_info.colorspace,
              clone_info->verbose,cluster_threshold,smoothing_threshold);
            continue;
          }
        if (LocaleNCompare("shade",option+1,5) == 0)
          {
            double
              azimuth,
              elevation;

            Image
              *shade_image;

            /*
              Shade image.
            */
            azimuth=30.0;
            elevation=30.0;
            (void) sscanf(argv[++i],"%lfx%lf",&azimuth,&elevation);
            shade_image=ShadeImage(*image,*option == '-',azimuth,
              elevation,&(*image)->exception);
            if (shade_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shade_image;
            continue;
          }
        if (LocaleNCompare("-sharpen",option,6) == 0)
          {
            double
              radius,
              sigma;

            Image
              *sharp_image;

            /*
              Gaussian sharpen image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            sharp_image=SharpenImage(*image,radius,sigma,&(*image)->exception);
            if (sharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sharp_image;
            continue;
          }
        if (LocaleCompare("-shave",option) == 0)
          {
            Image
              *shave_image;

            RectangleInfo
              shave_info;

            /*
              Shave the image edges.
            */
            shave_info.width=0;
            shave_info.height=0;
            shave_info.x=0;
            shave_info.y=0;
            flags=ParseGeometry(argv[++i],&shave_info.x,&shave_info.y,
              &shave_info.width,&shave_info.height);
            if ((flags & HeightValue) == 0)
              shave_info.height=shave_info.width;
            shave_image=ShaveImage(*image,&shave_info,&(*image)->exception);
            if (shave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shave_image;
            continue;
          }
        if (LocaleNCompare("-shear",option,4) == 0)
          {
            double
              x_shear,
              y_shear;

            Image
              *shear_image;

            /*
              Shear image.
            */
            x_shear=0.0;
            y_shear=0.0;
            (void) sscanf(argv[++i],"%lfx%lf",&x_shear,&y_shear);
            shear_image=ShearImage(*image,x_shear,y_shear,&(*image)->exception);
            if (shear_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            SetImageType(shear_image,TrueColorType);
            *image=shear_image;
            continue;
          }
        if (LocaleNCompare("-solarize",option,3) == 0)
          {
            SolarizeImage(*image,StringToDouble(argv[++i],MaxRGB));
            continue;
          }
        if (LocaleNCompare("-spread",option,3) == 0)
          {
            unsigned int
              amount;

            Image
              *spread_image;

            /*
              Spread an image.
            */
            amount=atoi(argv[++i]);
            spread_image=SpreadImage(*image,amount,&(*image)->exception);
            if (spread_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=spread_image;
            continue;
          }
        if (LocaleCompare("-stroke",option) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->stroke);
            continue;
          }
        if (LocaleNCompare("-strokewidth",option,8) == 0)
          {
            draw_info->stroke_width=atof(argv[++i]);
            continue;
          }
        if (LocaleNCompare("-swirl",option,3) == 0)
          {
            double
              degrees;

            Image
              *swirl_image;

            /*
              Swirl image.
            */
            degrees=atof(argv[++i]);
            swirl_image=SwirlImage(*image,degrees,&(*image)->exception);
            if (swirl_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=swirl_image;
            continue;
          }
        break;
      }
      case 't':
      {
        if (LocaleNCompare("-threshold",option,3) == 0)
          {
            double
              threshold;

            /*
              Threshold image.
            */
            threshold=StringToDouble(argv[++i],MaxRGB);
            (void) ThresholdImage(*image,threshold);
            continue;
          }
        if (LocaleCompare("-tile",option) == 0)
          {
            Image
              *tile_image;

            (void) strncpy(clone_info->filename,argv[++i],MaxTextExtent-1);
            tile_image=ReadImage(clone_info,&exception);
            if (tile_image == (Image *) NULL)
              continue;
            draw_info->tile=
              CloneImage(tile_image,0,0,True,&tile_image->exception);
            DestroyImage(tile_image);
            continue;
          }
        if (LocaleNCompare("-transparent",option,8) == 0)
          {
            PixelPacket
              target;

            target=GetOnePixel(*image,0,0);
            (void) QueryColorDatabase(argv[++i],&target);
            (void) TransparentImage(*image,target,TransparentOpacity);
            continue;
          }
        if (LocaleNCompare("-treedepth",option,4) == 0)
          {
            quantize_info.tree_depth=atoi(argv[++i]);
            continue;
          }
        if (LocaleCompare("-type",option) == 0)
          {
            if (*option == '-')
              {
                ImageType
                  image_type;

                option=argv[++i];
                image_type=GetImageType(*image,&(*image)->exception);
                if (LocaleCompare("Bilevel",option) == 0)
                  image_type=BilevelType;
                if (LocaleCompare("Grayscale",option) == 0)
                  image_type=GrayscaleType;
                if (LocaleCompare("GrayscaleMatte",option) == 0)
                  image_type=GrayscaleMatteType;
                if (LocaleCompare("Palette",option) == 0)
                  image_type=PaletteType;
                if (LocaleCompare("PaletteMatte",option) == 0)
                  image_type=PaletteMatteType;
                if (LocaleCompare("TrueColor",option) == 0)
                  image_type=TrueColorType;
                if (LocaleCompare("TrueColorMatte",option) == 0)
                  image_type=TrueColorMatteType;
                if (LocaleCompare("ColorSeparation",option) == 0)
                  image_type=ColorSeparationType;
                if (LocaleCompare("ColorSeparationMatte",option) == 0)
                  image_type=ColorSeparationMatteType;
                SetImageType(*image,image_type);
              }
            continue;
          }
        break;
      }
      case 'u':
      {
        if (LocaleNCompare("units",option+1,3) == 0)
          {
            (*image)->units=UndefinedResolution;
            if (*option == '-')
              {
                option=argv[++i];
                if (LocaleCompare("PixelsPerInch",option) == 0)
                  (*image)->units=PixelsPerInchResolution;
                if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                  (*image)->units=PixelsPerCentimeterResolution;
              }
            continue;
          }
        if (LocaleNCompare("-unsharp",option,6) == 0)
          {
            double
              radius,
              sigma;

            Image
              *unsharp_image;

            /*
              Gaussian unsharpen image.
            */
            radius=0.0;
            sigma=1.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&radius,&sigma);
            unsharp_image=UnsharpMaskImage(*image,radius,sigma,1.0,0.05,
              &(*image)->exception);
            if (unsharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=unsharp_image;
            continue;
          }
        break;
      }
      case 'v':
      {
        if (LocaleNCompare("verbose",option+1,4) == 0)
          {
            clone_info->verbose=(*option == '-');
            quantize_info.measure_error=(*option == '-');
            continue;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            double
              amplitude,
              wavelength;

            Image
              *wave_image;

            /*
              Wave image.
            */
            amplitude=25.0;
            wavelength=150.0;
            if (*option == '-')
              (void) sscanf(argv[++i],"%lfx%lf",&amplitude,&wavelength);
            wave_image=WaveImage(*image,amplitude,wavelength,
              &(*image)->exception);
            if (wave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=wave_image;
            continue;
          }
        break;
      }
      default:
        break;
    }
  }
  if (region_image != (Image *) NULL)
    {
      /*
        Composite transformed region onto image.
      */
      matte=region_image->matte;
      (void) CompositeImage(region_image,
        (*image)->matte ? OverCompositeOp : CopyCompositeOp,*image,
        region_info.x,region_info.y);
      DestroyImage(*image);
      *image=region_image;
      (*image)->matte=matte;
    }
  if ((quantize_info.number_colors != 0) ||
      (quantize_info.colorspace == GRAYColorspace))
    {
      /*
        Reduce the number of colors in the image.
      */
      if (((*image)->storage_class == DirectClass) ||
          ((*image)->colors > quantize_info.number_colors) ||
          (quantize_info.colorspace == GRAYColorspace))
        (void) QuantizeImage(&quantize_info,*image);
      else
        CompressColormap(*image);
    }
  if (map_image != (Image *) NULL)
    {
      (void) MapImage(*image,map_image,quantize_info.dither);
      DestroyImage(map_image);
    }
  /*
    Free resources.
  */
  if (geometry != (char *) NULL)
    LiberateMemory((void **) &geometry);
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(clone_info);
  return((*image)->exception.severity == UndefinedException);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImages applies next processing options to a sequence of
%  images as prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImages(const ImageInfo *image_info,const int argc,
%        char **argv,Image **images)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o images: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImages(const ImageInfo *image_info,
  const int argc,char **argv,Image **images)
{
#define MogrifyImageText  "  Transform image...  "

  Image
    *image,
    *mogrify_images;

  int
    scene;

  MonitorHandler
    handler;

  unsigned int
    number_images,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(images != (Image **) NULL);
  assert((*images)->signature == MagickSignature);
  number_images=GetNumberScenes(*images);
  if (number_images == 1)
    return(MogrifyImage(image_info,argc,argv,images));
  MagickMonitor(MogrifyImageText,0,number_images);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  status=MogrifyImage(image_info,argc,argv,images);
  (void) SetMonitorHandler(handler);
  if (status == False)
    return(False);
  if (image_info->verbose)
    DescribeImage(*images,stdout,False);
  mogrify_images=(*images);
  image=(*images)->next;
  for (scene=1; scene < (int) number_images; scene++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    status=MogrifyImage(image_info,argc,argv,&image);
    (void) SetMonitorHandler(handler);
    if (status == False)
      break;
    mogrify_images->next=image;
    mogrify_images->next->previous=mogrify_images;
    mogrify_images=mogrify_images->next;
    if (image_info->verbose)
      DescribeImage(image,stdout,False);
    image=image->next;
    MagickMonitor(MogrifyImageText,scene,number_images);
  }
  return(status);
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
%  Method ParseImageGeometry parses a geometry specification and returns the
%  width, height, x, and y values.  It also returns flags that indicates
%  which of the four values (width, height, xoffset, yoffset) were located
%  in the string, and whether the x and y values are negative.  In addition,
%  there are flags to report any meta characters (%, !, <, and >).
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
  int
    count,
    flags;

  long
    delta;

  RectangleInfo
    media_info;

  unsigned long
    former_height,
    former_width;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(NoValue);
  /*
    Parse geometry using ParseGeometry.
  */
  former_width=(*width);
  former_height=(*height);
  flags=GetGeometry(geometry,x,y,width,height);
  if (flags & PercentValue)
    {
      double
        x_scale,
        y_scale;

      /*
        Geometry is a percentage of the image size.
      */
      x_scale=(*width);
      y_scale=(*height);
      count=sscanf(geometry,"%lf%%x%lf",&x_scale,&y_scale);
      if (count != 2)
        count=sscanf(geometry,"%lfx%lf",&x_scale,&y_scale);
      if (count == 1)
        y_scale=x_scale;
      *width=Max((unsigned int) ((x_scale*former_width)/100.0),1);
      *height=Max((unsigned int) ((y_scale*former_height)/100.0),1);
      former_width=(*width);
      former_height=(*height);
    }
  if (flags & AreaValue)
    {
      double
        distance,
        x_area,
        y_area;

      /*
        Geometry is a maximum area in pixels.
      */
      x_area=(*width);
      y_area=(*height);
      count=sscanf(geometry,"%lf%%x%lf",&x_area,&y_area);
      if (count != 2)
        count=sscanf(geometry,"%lfx%lf",&x_area,&y_area);
      if (count == 1)
        y_area=x_area;
      distance=sqrt((double) former_width*former_height);
      *width=(unsigned int) (former_width/(distance/x_area));
      *height=(unsigned int) (former_height/(distance/y_area));
      former_width=(*width);
      former_height=(*height);
    }
  if (!(flags & AspectValue))
    {
      double
        scale_factor;

      /*
        Respect aspect ratio of the image.
      */
      scale_factor=1.0;
      if ((former_width*former_height) != 0)
        {
          if (((flags & WidthValue) != 0) && (flags & HeightValue) != 0)
            {
              scale_factor=(double) *width/former_width;
              if (scale_factor > ((double) *height/former_height))
                scale_factor=(double) *height/former_height;
            }
          else
            if ((flags & WidthValue) != 0)
              scale_factor=(double) *width/former_width;
            else
              scale_factor=(double) *height/former_height;
        }
      *width=(unsigned int) Max(scale_factor*former_width,1);
      *height=(unsigned int) Max(scale_factor*former_height,1);
    }
  if ((flags & XValue) == 0)
    *width-=(*x) << 1;
  if ((flags & YValue) == 0)
    *height-=(*y) << 1;
  if (flags & GreaterValue)
    {
      if (former_width < *width)
        *width=former_width;
      if (former_height < *height)
        *height=former_height;
    }
  if (flags & LessValue)
    {
      if (former_width > *width)
        *width=former_width;
      if (former_height > *height)
        *height=former_height;
    }
  media_info.width=(*width);
  media_info.height=(*height);
  media_info.x=(*x);
  media_info.y=(*y);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,&media_info.width,
    &media_info.height);
  if ((flags & XValue) == 0)
    {
      /*
        Center image in the X direction.
      */
      delta=media_info.width-(*width);
      if (delta >= 0)
        *x=delta >> 1;
    }
  else
    if ((flags & XNegative) != 0)
      *x+=media_info.width-(*width);
  if ((flags & YValue) == 0)
    {
      /*
        Center image in the Y direction.
      */
      delta=media_info.height-(*height);
      if (delta >= 0)
        *y=delta >> 1;
    }
  else
    if ((flags & YNegative) != 0)
      *y+=media_info.height-(*height);
  if (flags & GreaterValue)
    {
      if ((*width+((*x) << 1)) > media_info.width)
        {
          if ((long) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((long) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
      if ((*height+((*y) << 1)) > media_info.height)
        {
          if ((long) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((long) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
    }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceImage() increments the reference count associated with an image
%  returning a pointer to the image.
%
%  The format of the ReferenceImage method is:
%
%      Image *ReferenceImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport Image *ReferenceImage(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  AcquireSemaphoreInfo(&image->semaphore);
  image->reference_count++;
  LiberateSemaphoreInfo(&image->semaphore);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalized the range of the transformed
%  values to be [0..MaxRGB].
%
%  The format of the RGBTransformImage method is:
%
%      unsigned int RGBTransformImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates which colorspace
%      to transform the image.
%
%
*/
MagickExport unsigned int RGBTransformImage(Image *image,
  const ColorspaceType colorspace)
{
#define RGBTransformImageText  "  Transform image colors...  "
#define X 0
#define Y (MaxRGB+1)
#define Z (MaxRGB+1)*2

  double
    blue,
    green,
    red,
    tx,
    ty,
    tz,
    *x_map,
    *y_map,
    *z_map;

  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == colorspace)
    return(True);
  if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace))
    return(True);
  if (colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      long
        black,
        cyan,
        magenta,
        yellow;

      /*
        Convert RGB to CMYK colorspace.
      */
      SetImageType(image,TrueColorType);
      image->colorspace=CMYKColorspace;
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          cyan=MaxRGB-q->red;
          magenta=MaxRGB-q->green;
          yellow=MaxRGB-q->blue;
          black=cyan < magenta ? Min(cyan,yellow) : Min(magenta,yellow);
          q->red=(Quantum) cyan;
          q->green=(Quantum) magenta;
          q->blue=(Quantum) yellow;
          indexes[x]=q->opacity;
          q->opacity=(Quantum) black;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      return(True);
    }
  x=0;
  if (colorspace == GRAYColorspace)
    {
      /*
        Return if the image is already grayscale.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            break;
          p++;
        }
        if (x < (long) image->columns)
          break;
      }
      if ((x == (long) image->columns) && (y == (long) image->rows))
        return(True);
    }
  /*
    Allocate the tables.
  */
  x_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  y_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  z_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  if ((x_map == (double *) NULL) || (y_map == (double *) NULL) ||
      (z_map == (double *) NULL))
    ThrowBinaryException(ResourceLimitWarning,
      "Unable to transform color space","Memory allocation failed");
  tx=0;
  ty=0;
  tz=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=0.299*i;
        y_map[i+Y]=0.587*i;
        z_map[i+Y]=0.114*i;
        x_map[i+Z]=0.299*i;
        y_map[i+Z]=0.587*i;
        z_map[i+Z]=0.114*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1)/2;
      tz=(MaxRGB+1)/2;
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.33333*i;
        y_map[i+X]=0.33334*i;
        z_map[i+X]=0.33333*i;
        x_map[i+Y]=0.5*i;
        y_map[i+Y]=0.0;
        z_map[i+Y]=(-0.5)*i;
        x_map[i+Z]=(-0.25)*i;
        y_map[i+Z]=0.5*i;
        z_map[i+Z]=(-0.25)*i;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpScale(156);
      tz=UpScale(137);
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x_map[i+X]=0.003962014134275617*MaxRGB*i;
        y_map[i+X]=0.007778268551236748*MaxRGB*i;
        z_map[i+X]=0.001510600706713781*MaxRGB*i;
        x_map[i+Y]=(-0.002426619775463276)*MaxRGB*i;
        y_map[i+Y]=(-0.004763965913702149)*MaxRGB*i;
        z_map[i+Y]=0.007190585689165425*MaxRGB*i;
        x_map[i+Z]=0.006927257754597858*MaxRGB*i;
        y_map[i+Z]=(-0.005800713697502058)*MaxRGB*i;
        z_map[i+Z]=(-0.0011265440570958)*MaxRGB*i;
      }
      for ( ; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.2201118963486454*(1.099*i-0.099);
        y_map[i+X]=0.4321260306242638*(1.099*i-0.099);
        z_map[i+X]=0.08392226148409894*(1.099*i-0.099);
        x_map[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y_map[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z_map[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x_map[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y_map[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z_map[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          X = 0.412453*X+0.357580*Y+0.180423*Z
          Y = 0.212671*X+0.715160*Y+0.072169*Z
          Z = 0.019334*X+0.119193*Y+0.950227*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.412453*i;
        y_map[i+X]=0.35758*i;
        z_map[i+X]=0.180423*i;
        x_map[i+Y]=0.212671*i;
        y_map[i+Y]=0.71516*i;
        z_map[i+Y]=0.072169*i;
        x_map[i+Z]=0.019334*i;
        y_map[i+Z]=0.119193*i;
        z_map[i+Z]=0.950227*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Cb= -0.168736*R-0.331264*G+0.500000*B
          Cr=  0.500000*R-0.418688*G-0.081316*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1)/2;
      tz=(MaxRGB+1)/2;
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.16873)*i;
        y_map[i+Y]=(-0.331264)*i;
        z_map[i+Y]=0.500000*i;
        x_map[i+Z]=0.500000*i;
        y_map[i+Z]=(-0.418688)*i;
        z_map[i+Z]=(-0.081316)*i;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpScale(156);
      tz=UpScale(137);
      for (i=0; i <= (int) (0.018*MaxRGB); i++)
      {
        x_map[i+X]=0.003962014134275617*MaxRGB*i;
        y_map[i+X]=0.007778268551236748*MaxRGB*i;
        z_map[i+X]=0.001510600706713781*MaxRGB*i;
        x_map[i+Y]=(-0.002426619775463276)*MaxRGB*i;
        y_map[i+Y]=(-0.004763965913702149)*MaxRGB*i;
        z_map[i+Y]=0.007190585689165425*MaxRGB*i;
        x_map[i+Z]=0.006927257754597858*MaxRGB*i;
        y_map[i+Z]=(-0.005800713697502058)*MaxRGB*i;
        z_map[i+Z]=(-0.0011265440570958)*MaxRGB*i;
      }
      for ( ; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.2201118963486454*(1.099*i-0.099);
        y_map[i+X]=0.4321260306242638*(1.099*i-0.099);
        z_map[i+X]=0.08392226148409894*(1.099*i-0.099);
        x_map[i+Y]=(-0.1348122097479598)*(1.099*i-0.099);
        y_map[i+Y]=(-0.2646647729834528)*(1.099*i-0.099);
        z_map[i+Y]=0.3994769827314126*(1.099*i-0.099);
        x_map[i+Z]=0.3848476530332144*(1.099*i-0.099);
        y_map[i+Z]=(-0.3222618720834477)*(1.099*i-0.099);
        z_map[i+Z]=(-0.06258578094976668)*(1.099*i-0.099);
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1)/2;
      tz=(MaxRGB+1)/2;
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=0.596*i;
        y_map[i+Y]=(-0.274)*i;
        z_map[i+Y]=(-0.322)*i;
        x_map[i+Z]=0.211*i;
        y_map[i+Z]=(-0.523)*i;
        z_map[i+Z]=0.312*i;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          Y =  0.299000*R+0.587000*G+0.114000*B
          Pb= -0.168736*R-0.331264*G+0.500000*B
          Pr=  0.500000*R-0.418688*G-0.081312*B

        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=(MaxRGB+1)/2;
      tz=(MaxRGB+1)/2;
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.168736)*i;
        y_map[i+Y]=(-0.331264)*i;
        z_map[i+Y]=0.5*i;
        x_map[i+Z]=0.5*i;
        y_map[i+Z]=(-0.418688)*i;
        z_map[i+Z]=(-0.081312)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      ty=(MaxRGB+1)/2;
      tz=(MaxRGB+1)/2;
      for (i=0; i <= MaxRGB; i++)
      {
        x_map[i+X]=0.299*i;
        y_map[i+X]=0.587*i;
        z_map[i+X]=0.114*i;
        x_map[i+Y]=(-0.1474)*i;
        y_map[i+Y]=(-0.2895)*i;
        z_map[i+Y]=0.4369*i;
        x_map[i+Z]=0.615*i;
        y_map[i+Z]=(-0.515)*i;
        z_map[i+Z]=(-0.1)*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          red=x_map[q->red+X]+y_map[q->green+X]+z_map[q->blue+X]+tx;
          green=x_map[q->red+Y]+y_map[q->green+Y]+z_map[q->blue+Y]+ty;
          blue=x_map[q->red+Z]+y_map[q->green+Z]+z_map[q->blue+Z]+tz;
          q->red=(Quantum)
            ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
          q->green=(Quantum)
            ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
          q->blue=(Quantum)
            ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(RGBTransformImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        red=x_map[image->colormap[i].red+X]+y_map[image->colormap[i].green+X]+
          z_map[image->colormap[i].blue+X]+tx;
        green=x_map[image->colormap[i].red+Y]+y_map[image->colormap[i].green+Y]+
          z_map[image->colormap[i].blue+Y]+ty;
        blue=x_map[image->colormap[i].red+Z]+y_map[image->colormap[i].green+Z]+
          z_map[image->colormap[i].blue+Z]+tz;
        image->colormap[i].red=(Quantum)
          ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
        image->colormap[i].green=(Quantum)
          ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
        image->colormap[i].blue=(Quantum)
          ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocate memory.
  */
  LiberateMemory((void **) &z_map);
  LiberateMemory((void **) &y_map);
  LiberateMemory((void **) &x_map);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   S e t I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImage() sets the red, green, and blue components of each pixel to
%  the image background color and the opacity component to the specified
%  level of transparency.  The background color is defined by the
%  background_color member of the image.
%
%  The format of the SetImage method is:
%
%      void SetImage(Image *image,const Quantum opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: Set each pixel to this level of transparency.
%
%
*/
MagickExport void SetImage(Image *image,const Quantum opacity)
{
  long
    y;

  PixelPacket
    background_color;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  background_color=image->background_color;
  if (opacity != OpaqueOpacity)
    background_color.opacity=opacity;
  if (background_color.opacity != OpaqueOpacity)
    SetImageType(image,TrueColorMatteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (image->storage_class == PseudoClass)
        indexes[x]=0;
      *q++=background_color;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C l i p M a s k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageClipMask() associates a clip mask with the image.  The clip mask
%  must be the same dimensions as the image.
%
%  The format of the SetImageClipMask method is:
%
%      unsigned int SetImageClipMask(Image *image,Image *clip_mask)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o clip_mask: The image clip mask.
%
%
*/
MagickExport unsigned int SetImageClipMask(Image *image,Image *clip_mask)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (clip_mask != (const Image *) NULL)
    if ((clip_mask->columns != image->columns) ||
        (clip_mask->rows != image->rows))
      ThrowBinaryException(OptionWarning,"Unable to set clip mask",
        "image widths or heights differ");
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  if (clip_mask == (Image *) NULL)
    {
      image->clip_mask=(Image *) NULL;
      return(True);
    }
  image->clip_mask=CloneImage(clip_mask,0,0,True,&clip_mask->exception);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e D e p t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageDepth() sets the depth of the image, either 8 or 16.  Some image
%  formats support both 8 and 16-bits per color component (e.g. PNG).  Use
%  SetImageDepth() to specify your preference.  A value other than 0 is
%  returned if the depth is set.
%
%  The format of the SetImageDepth method is:
%
%      unsigned int SetImageDepth(Image *image,const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o depth: The image depth.
%
%
*/
MagickExport unsigned int SetImageDepth(Image *image,const unsigned long depth)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->depth=depth;
  if (image->depth == QuantumDepth)
    return(True);
  if (GetImageDepth(image,&image->exception) == depth)
    return(True);
  image->depth=8;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=UpScale(DownScale(q->red));
      q->green=UpScale(DownScale(q->green));
      q->blue=UpScale(DownScale(q->blue));
      q->opacity=UpScale(DownScale(q->opacity));
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if (image->storage_class == PseudoClass)
    {
      register long
        i;

      q=image->colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        q->red=UpScale(DownScale(q->red));
        q->green=UpScale(DownScale(q->green));
        q->blue=UpScale(DownScale(q->blue));
        q->opacity=UpScale(DownScale(q->opacity));
        q++;
      }
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S e t I m a g e O p a c i t y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageOpacity() attenuates the opacity channel of an image.  If the
%  image pixels are opaque, they are set to the specified opacity level.
%  Otherwise, the pixel oapcity values are blended with the supplied
%  transparency value.
%
%  The format of the SetImageOpacity method is:
%
%      void SetImageOpacity(Image *image,const unsigned int opacity)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o opacity: The level of transparency: 0 is fully opaque and MaxRGB is
%      fully transparent.
%
%
*/
MagickExport void SetImageOpacity(Image *image,const unsigned int opacity)
{
  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->matte)
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          if (image->colorspace == CMYKColorspace)
            {
              indexes[x]=((unsigned long) (opacity*indexes[x])/MaxRGB);
              continue;
            }
          q->opacity=((unsigned long) (opacity*q->opacity)/MaxRGB);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      return;
    }
  image->matte=True;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      if (image->colorspace == CMYKColorspace)
        {
          indexes[x]=opacity;
          continue;
        }
      q->opacity=opacity;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e T y p e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageType() sets the type of image.  Choose from these types:
%
%        Bilevel        Grayscale       GrayscaleMatte
%        Palette        PaletteMatte    TrueColor
%        TrueColorMatte ColorSeparation ColorSeparationMatte
%
%  The format of the SetImageType method is:
%
%      SetImageType(Image *image,const ImageType image_type)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o image_type: Image type.
%
%
*/
MagickExport void SetImageType(Image *image,const ImageType image_type)
{
  QuantizeInfo
    quantize_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image_type)
  {
    case BilevelType:
    {
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=2;
      quantize_info.tree_depth=8;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case GrayscaleType:
    {
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=256;
      quantize_info.tree_depth=8;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case GrayscaleMatteType:
    {
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=256;
      quantize_info.tree_depth=8;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      image->matte=True;
      break;
    }
    case PaletteType:
    {
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=256;
      quantize_info.tree_depth=8;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case PaletteMatteType:
    {
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      image->matte=True;
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=256;
      quantize_info.colorspace=TransparentColorspace;
      (void) QuantizeImage(&quantize_info,image);
      break;
    }
    case TrueColorType:
    {
      image->storage_class=DirectClass;
      break;
    }
    case TrueColorMatteType:
    {
      image->storage_class=DirectClass;
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      image->matte=True;
      break;
    }
    case ColorSeparationType:
    {
      if (image->colorspace != CMYKColorspace)
        (void) RGBTransformImage(image,CMYKColorspace);
      break;
    }
    case ColorSeparationMatteType:
    {
      if (image->colorspace != CMYKColorspace)
        (void) RGBTransformImage(image,CMYKColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      image->matte=True;
      break;
    }
    default:
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SortColormapByIntensity() sorts the colormap of a PseudoClass image by
%  decreasing color intensity.
%
%  The format of the SortColormapByIntensity method is:
%
%      unsigned int SortColormapByIntensity(Image *image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to a Image structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int IntensityCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) (Intensity(*color_2)-Intensity(*color_1)));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport unsigned int SortColormapByIntensity(Image *image)
{
  IndexPacket
    index;

  long
    y;

  register long
    x;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  register long
    i;

  unsigned short
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(True);
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned short *) AcquireMemory(image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    ThrowBinaryException(MissingDelegateWarning,"Unable to sort colormap",
      "Memory allocation failed");
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (long) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing color popularity.
  */
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
    IntensityCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < (long) image->colors; i++)
    pixels[image->colormap[i].opacity]=(unsigned short) i;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=pixels[indexes[x]];
      indexes[x]=index;
      *q++=image->colormap[index];
    }
  }
  LiberateMemory((void **) &pixels);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage method is:
%
%      void SyncImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void SyncImage(Image *image)
{
  IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    return;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=indexes[x];
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T e x t u r e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TextureImage() repeatedly tiles the texture image across and down the image
%  canvas.
%
%  The format of the TextureImage method is:
%
%      void TextureImage(Image *image,const Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o texture: This image is the texture to layer on the background.
%
%
*/
MagickExport void TextureImage(Image *image,const Image *texture)
{
#define TextureImageText  "  Apply image texture...  "

  long
    x,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (texture == (const Image *) NULL)
    return;
  /*
    Tile texture onto the image background.
  */
  for (y=0; y < (long) image->rows; y+=texture->rows)
  {
    for (x=0; x < (long) image->columns; x+=texture->columns)
      (void) CompositeImage(image,CopyCompositeOp,texture,x,y);
    if (QuantumTick(y,image->rows))
      MagickMonitor(TextureImageText,y,image->rows);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformRGBImage converts the reference image from an alternate
%  colorspace.  The transformation matrices are not the standard ones:  the
%  weights are rescaled to normalize the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the TransformRGBImage method is:
%
%      unsigned int TransformRGBImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value defines which colorspace to
%      transform the image to.
%
%
*/
MagickExport unsigned int TransformRGBImage(Image *image,
  const ColorspaceType colorspace)
{
#define B (MaxRGB+1)*2
#define G (MaxRGB+1)
#define R 0
#define TransformRGBImageText  "  Transform image colors...  "

  static const Quantum
    sRGBMap[351] =
    {
      UpScale(0), UpScale(1), UpScale(2), UpScale(3), UpScale(4),
      UpScale(5), UpScale(6), UpScale(7), UpScale(8), UpScale(9),
      UpScale(10), UpScale(11), UpScale(12), UpScale(13), UpScale(14),
      UpScale(15), UpScale(16), UpScale(17), UpScale(18), UpScale(19),
      UpScale(20), UpScale(21), UpScale(22), UpScale(23), UpScale(24),
      UpScale(25), UpScale(26), UpScale(27), UpScale(28), UpScale(29),
      UpScale(29), UpScale(30), UpScale(31), UpScale(32), UpScale(33),
      UpScale(34), UpScale(35), UpScale(36), UpScale(37), UpScale(38),
      UpScale(39), UpScale(40), UpScale(41), UpScale(42), UpScale(43),
      UpScale(44), UpScale(45), UpScale(46), UpScale(47), UpScale(48),
      UpScale(49), UpScale(50), UpScale(51), UpScale(52), UpScale(53),
      UpScale(54), UpScale(55), UpScale(56), UpScale(57), UpScale(58),
      UpScale(59), UpScale(60), UpScale(61), UpScale(62), UpScale(63),
      UpScale(65), UpScale(66), UpScale(67), UpScale(68), UpScale(69),
      UpScale(70), UpScale(71), UpScale(72), UpScale(73), UpScale(74),
      UpScale(75), UpScale(76), UpScale(77), UpScale(78), UpScale(79),
      UpScale(80), UpScale(81), UpScale(82), UpScale(83), UpScale(84),
      UpScale(85), UpScale(86), UpScale(87), UpScale(88), UpScale(89),
      UpScale(90), UpScale(91), UpScale(92), UpScale(93), UpScale(95),
      UpScale(96), UpScale(97), UpScale(98), UpScale(99), UpScale(100),
      UpScale(101), UpScale(102), UpScale(103), UpScale(104), UpScale(105),
      UpScale(106), UpScale(107), UpScale(108), UpScale(109), UpScale(110),
      UpScale(111), UpScale(112), UpScale(114), UpScale(115), UpScale(116),
      UpScale(117), UpScale(118), UpScale(119), UpScale(120), UpScale(121),
      UpScale(122), UpScale(123), UpScale(124), UpScale(125), UpScale(126),
      UpScale(127), UpScale(128), UpScale(129), UpScale(130), UpScale(131),
      UpScale(132), UpScale(133), UpScale(135), UpScale(136), UpScale(137),
      UpScale(138), UpScale(139), UpScale(140), UpScale(141), UpScale(142),
      UpScale(143), UpScale(144), UpScale(145), UpScale(146), UpScale(147),
      UpScale(148), UpScale(149), UpScale(150), UpScale(151), UpScale(152),
      UpScale(153), UpScale(154), UpScale(155), UpScale(156), UpScale(157),
      UpScale(158), UpScale(159), UpScale(160), UpScale(161), UpScale(162),
      UpScale(163), UpScale(164), UpScale(165), UpScale(166), UpScale(167),
      UpScale(168), UpScale(169), UpScale(170), UpScale(171), UpScale(172),
      UpScale(173), UpScale(174), UpScale(175), UpScale(175), UpScale(176),
      UpScale(177), UpScale(178), UpScale(179), UpScale(180), UpScale(181),
      UpScale(182), UpScale(183), UpScale(184), UpScale(185), UpScale(186),
      UpScale(187), UpScale(187), UpScale(188), UpScale(189), UpScale(190),
      UpScale(191), UpScale(192), UpScale(193), UpScale(194), UpScale(194),
      UpScale(195), UpScale(196), UpScale(197), UpScale(198), UpScale(199),
      UpScale(199), UpScale(200), UpScale(201), UpScale(202), UpScale(203),
      UpScale(203), UpScale(204), UpScale(205), UpScale(206), UpScale(207),
      UpScale(207), UpScale(208), UpScale(209), UpScale(210), UpScale(210),
      UpScale(211), UpScale(212), UpScale(213), UpScale(213), UpScale(214),
      UpScale(215), UpScale(215), UpScale(216), UpScale(217), UpScale(218),
      UpScale(218), UpScale(219), UpScale(220), UpScale(220), UpScale(221),
      UpScale(222), UpScale(222), UpScale(223), UpScale(223), UpScale(224),
      UpScale(225), UpScale(225), UpScale(226), UpScale(227), UpScale(227),
      UpScale(228), UpScale(228), UpScale(229), UpScale(229), UpScale(230),
      UpScale(230), UpScale(231), UpScale(232), UpScale(232), UpScale(233),
      UpScale(233), UpScale(234), UpScale(234), UpScale(235), UpScale(235),
      UpScale(235), UpScale(236), UpScale(236), UpScale(237), UpScale(237),
      UpScale(238), UpScale(238), UpScale(238), UpScale(239), UpScale(239),
      UpScale(240), UpScale(240), UpScale(240), UpScale(241), UpScale(241),
      UpScale(242), UpScale(242), UpScale(242), UpScale(243), UpScale(243),
      UpScale(243), UpScale(243), UpScale(244), UpScale(244), UpScale(244),
      UpScale(245), UpScale(245), UpScale(245), UpScale(245), UpScale(246),
      UpScale(246), UpScale(246), UpScale(247), UpScale(247), UpScale(247),
      UpScale(247), UpScale(247), UpScale(248), UpScale(248), UpScale(248),
      UpScale(248), UpScale(249), UpScale(249), UpScale(249), UpScale(249),
      UpScale(249), UpScale(249), UpScale(250), UpScale(250), UpScale(250),
      UpScale(250), UpScale(250), UpScale(250), UpScale(251), UpScale(251),
      UpScale(251), UpScale(251), UpScale(251), UpScale(251), UpScale(252),
      UpScale(252), UpScale(252), UpScale(252), UpScale(252), UpScale(252),
      UpScale(252), UpScale(252), UpScale(252), UpScale(253), UpScale(253),
      UpScale(253), UpScale(253), UpScale(253), UpScale(253), UpScale(253),
      UpScale(253), UpScale(253), UpScale(254), UpScale(254), UpScale(254),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(254),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255)
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
      UpScale(0), UpScale(1), UpScale(2), UpScale(3), UpScale(4),
      UpScale(5), UpScale(6), UpScale(7), UpScale(8), UpScale(9),
      UpScale(10), UpScale(11), UpScale(12), UpScale(13), UpScale(14),
      UpScale(15), UpScale(16), UpScale(17), UpScale(18), UpScale(19),
      UpScale(20), UpScale(21), UpScale(22), UpScale(23), UpScale(24),
      UpScale(25), UpScale(26), UpScale(27), UpScale(28), UpScale(29),
      UpScale(30), UpScale(32), UpScale(33), UpScale(34), UpScale(35),
      UpScale(36), UpScale(37), UpScale(38), UpScale(39), UpScale(40),
      UpScale(41), UpScale(42), UpScale(43), UpScale(45), UpScale(46),
      UpScale(47), UpScale(48), UpScale(49), UpScale(50), UpScale(51),
      UpScale(52), UpScale(53), UpScale(54), UpScale(56), UpScale(57),
      UpScale(58), UpScale(59), UpScale(60), UpScale(61), UpScale(62),
      UpScale(63), UpScale(64), UpScale(66), UpScale(67), UpScale(68),
      UpScale(69), UpScale(70), UpScale(71), UpScale(72), UpScale(73),
      UpScale(74), UpScale(76), UpScale(77), UpScale(78), UpScale(79),
      UpScale(80), UpScale(81), UpScale(82), UpScale(83), UpScale(84),
      UpScale(86), UpScale(87), UpScale(88), UpScale(89), UpScale(90),
      UpScale(91), UpScale(92), UpScale(93), UpScale(94), UpScale(95),
      UpScale(97), UpScale(98), UpScale(99), UpScale(100), UpScale(101),
      UpScale(102), UpScale(103), UpScale(104), UpScale(105), UpScale(106),
      UpScale(107), UpScale(108), UpScale(110), UpScale(111), UpScale(112),
      UpScale(113), UpScale(114), UpScale(115), UpScale(116), UpScale(117),
      UpScale(118), UpScale(119), UpScale(120), UpScale(121), UpScale(122),
      UpScale(123), UpScale(124), UpScale(125), UpScale(126), UpScale(127),
      UpScale(129), UpScale(130), UpScale(131), UpScale(132), UpScale(133),
      UpScale(134), UpScale(135), UpScale(136), UpScale(137), UpScale(138),
      UpScale(139), UpScale(140), UpScale(141), UpScale(142), UpScale(143),
      UpScale(144), UpScale(145), UpScale(146), UpScale(147), UpScale(148),
      UpScale(149), UpScale(150), UpScale(151), UpScale(152), UpScale(153),
      UpScale(154), UpScale(155), UpScale(156), UpScale(157), UpScale(158),
      UpScale(159), UpScale(160), UpScale(161), UpScale(162), UpScale(163),
      UpScale(164), UpScale(165), UpScale(166), UpScale(167), UpScale(168),
      UpScale(169), UpScale(170), UpScale(171), UpScale(172), UpScale(173),
      UpScale(174), UpScale(175), UpScale(176), UpScale(176), UpScale(177),
      UpScale(178), UpScale(179), UpScale(180), UpScale(181), UpScale(182),
      UpScale(183), UpScale(184), UpScale(185), UpScale(186), UpScale(187),
      UpScale(188), UpScale(189), UpScale(190), UpScale(191), UpScale(192),
      UpScale(193), UpScale(193), UpScale(194), UpScale(195), UpScale(196),
      UpScale(197), UpScale(198), UpScale(199), UpScale(200), UpScale(201),
      UpScale(201), UpScale(202), UpScale(203), UpScale(204), UpScale(205),
      UpScale(206), UpScale(207), UpScale(207), UpScale(208), UpScale(209),
      UpScale(210), UpScale(211), UpScale(211), UpScale(212), UpScale(213),
      UpScale(214), UpScale(215), UpScale(215), UpScale(216), UpScale(217),
      UpScale(218), UpScale(218), UpScale(219), UpScale(220), UpScale(221),
      UpScale(221), UpScale(222), UpScale(223), UpScale(224), UpScale(224),
      UpScale(225), UpScale(226), UpScale(226), UpScale(227), UpScale(228),
      UpScale(228), UpScale(229), UpScale(230), UpScale(230), UpScale(231),
      UpScale(232), UpScale(232), UpScale(233), UpScale(234), UpScale(234),
      UpScale(235), UpScale(236), UpScale(236), UpScale(237), UpScale(237),
      UpScale(238), UpScale(238), UpScale(239), UpScale(240), UpScale(240),
      UpScale(241), UpScale(241), UpScale(242), UpScale(242), UpScale(243),
      UpScale(243), UpScale(244), UpScale(244), UpScale(245), UpScale(245),
      UpScale(245), UpScale(246), UpScale(246), UpScale(247), UpScale(247),
      UpScale(247), UpScale(248), UpScale(248), UpScale(248), UpScale(249),
      UpScale(249), UpScale(249), UpScale(249), UpScale(250), UpScale(250),
      UpScale(250), UpScale(250), UpScale(251), UpScale(251), UpScale(251),
      UpScale(251), UpScale(251), UpScale(252), UpScale(252), UpScale(252),
      UpScale(252), UpScale(252), UpScale(253), UpScale(253), UpScale(253),
      UpScale(253), UpScale(253), UpScale(253), UpScale(253), UpScale(253),
      UpScale(253), UpScale(253), UpScale(253), UpScale(253), UpScale(253),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(254),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(254),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(254),
      UpScale(254), UpScale(254), UpScale(254), UpScale(254), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255), UpScale(255), UpScale(255), UpScale(255), UpScale(255),
      UpScale(255)
    };

  double
    *blue_map,
    *green_map,
    *red_map;

  long
    y;

  register double
    blue,
    green,
    red;

  register long
    x;

  register long
    i;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == colorspace)
    return(True);
  if ((image->colorspace == CMYKColorspace) && (colorspace == RGBColorspace))
    {
      IndexPacket
        *indexes;

      /*
        Transform image from CMYK to RGB.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=(unsigned long) ((MaxRGB-q->red)*(MaxRGB-q->opacity))/MaxRGB;
          q->green=(unsigned long)
            ((MaxRGB-q->green)*(MaxRGB-q->opacity))/MaxRGB;
          q->blue=(unsigned long) ((MaxRGB-q->blue)*(MaxRGB-q->opacity))/MaxRGB;
          q->opacity=image->matte ? indexes[x] : OpaqueOpacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      image->colorspace=RGBColorspace;
      return(True);
    }
  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace) ||
      (colorspace == TransparentColorspace))
    return(True);
  /*
    Allocate the tables.
  */
  red_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  green_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  blue_map=(double *) AcquireMemory(3*(MaxRGB+1)*sizeof(double));
  if ((red_map == (double *) NULL) || (green_map == (double *) NULL) ||
      (blue_map == (double *) NULL))
    ThrowBinaryException(MissingDelegateWarning,
      "Unable to transform colorspace","Memory allocation failed");
  switch (colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.5*(2.0*i-MaxRGB);
        blue_map[i+R]=(-0.33334)*(2.0*i-MaxRGB);
        red_map[i+G]=i;
        green_map[i+G]=0.0;
        blue_map[i+G]=0.666665*(2.0*i-MaxRGB);
        red_map[i+B]=i;
        green_map[i+B]=(-0.5)*(2.0*i-MaxRGB);
        blue_map[i+B]=(-0.33334)*(2.0*i-MaxRGB);
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Initialize sRGB tables:

          R = Y            +1.032096*C2
          G = Y-0.326904*C1-0.704445*C2
          B = Y+1.685070*C1

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=1.40200*i;
        green_map[i+R]=0.0;
        blue_map[i+R]=1.88000*(i-UpScale(137.0));
        red_map[i+G]=1.40200*i;
        green_map[i+G]=(-0.444066)*(i-UpScale(156.0));
        blue_map[i+G]=(-0.95692)*(i-UpScale(137.0));
        red_map[i+B]=1.40200*i;
        green_map[i+B]=2.28900*(i-UpScale(156.0));
        blue_map[i+B]=0.0;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize CIE XYZ tables:

          R =  3.240479*R-1.537150*G-0.498535*B
          G = -0.969256*R+1.875992*G+0.041556*B
          B =  0.055648*R-0.204043*G+1.057311*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=3.240479*i;
        green_map[i+R]=(-1.537150)*i;
        blue_map[i+R]=(-0.498535)*i;
        red_map[i+G]=(-0.969256)*i;
        green_map[i+G]=1.875992*i;
        blue_map[i+G]=0.041556*i;
        red_map[i+B]=0.055648*i;
        green_map[i+B]=(-0.204043)*i;
        blue_map[i+B]=1.057311*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y            +1.402000*Cr
          G = Y-0.344136*Cb-0.714136*Cr
          B = Y+1.772000*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=(1.402000*0.5)*(2.0*i-MaxRGB);
        red_map[i+G]=i;
        green_map[i+G]=(-0.344136*0.5)*(2.0*i-MaxRGB);
        blue_map[i+G]=(-0.714136*0.5)*(2.0*i-MaxRGB);
        red_map[i+B]=i;
        green_map[i+B]=(1.772000*0.5)*(2.0*i-MaxRGB);
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=1.3584*i;
        green_map[i+R]=0.0;
        blue_map[i+R]=1.8215*(i-UpScale(137.0));
        red_map[i+G]=1.3584*i;
        green_map[i+G]=(-0.4302726)*(i-UpScale(156.0));
        blue_map[i+G]=(-0.9271435)*(i-UpScale(137.0));
        red_map[i+B]=1.3584*i;
        green_map[i+B]=2.2179*(i-UpScale(156.0));
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95620*I+0.62140*Q
          G = Y-0.27270*I-0.64680*Q
          B = Y-1.10370*I+1.70060*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.4781*(2.0*i-MaxRGB);
        blue_map[i+R]=0.3107*(2.0*i-MaxRGB);
        red_map[i+G]=i;
        green_map[i+G]=(-0.13635)*(2.0*i-MaxRGB);
        blue_map[i+G]=(-0.3234)*(2.0*i-MaxRGB);
        red_map[i+B]=i;
        green_map[i+B]=(-0.55185)*(2.0*i-MaxRGB);
        blue_map[i+B]=0.8503*(2.0*i-MaxRGB);
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          R = Y            +1.402000*C2
          G = Y-0.344136*C1+0.714136*C2
          B = Y+1.772000*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=0.701*(2.0*i-MaxRGB);
        red_map[i+G]=i;
        green_map[i+G]=(-0.172068)*(2.0*i-MaxRGB);
        blue_map[i+G]=0.357068*(2.0*i-MaxRGB);
        red_map[i+B]=i;
        green_map[i+B]=0.886*(2.0*i-MaxRGB);
        blue_map[i+B]=0.0;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red_map[i+R]=i;
        green_map[i+R]=0.0;
        blue_map[i+R]=0.5699*(2.0*i-MaxRGB);
        red_map[i+G]=i;
        green_map[i+G]=(-0.1969)*(2.0*i-MaxRGB);
        blue_map[i+G]=(-0.29025)*(2.0*i-MaxRGB);
        red_map[i+B]=i;
        green_map[i+B]=1.01395*(2.0*i-MaxRGB);
        blue_map[i+B]=0;
      }
      break;
    }
  }
  /*
    Convert to RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      /*
        Convert DirectClass image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          red=red_map[q->red+R]+green_map[q->green+R]+blue_map[q->blue+R];
          green=red_map[q->red+G]+green_map[q->green+G]+blue_map[q->blue+G];
          blue=red_map[q->red+B]+green_map[q->green+B]+blue_map[q->blue+B];
          switch (colorspace)
          {
            case sRGBColorspace:
            case YCCColorspace:
            {
              red=(red < 0) ? 0 : (red > UpScale(350)) ? UpScale(350) : red+0.5;
              green=(green < 0) ? 0 : (green > UpScale(350)) ? UpScale(350) :
                green+0.5;
              blue=(blue < 0) ? 0 : (blue > UpScale(350)) ? UpScale(350) :
                blue+0.5;
              if (colorspace == sRGBColorspace)
                {
                  q->red=sRGBMap[(int) DownScale(red)];
                  q->green=sRGBMap[(int) DownScale(green)];
                  q->blue=sRGBMap[(int) DownScale(blue)];
                  break;
                }
              q->red=(Quantum) YCCMap[(int) DownScale(red)];
              q->green=(Quantum) YCCMap[(int) DownScale(green)];
              q->blue=(Quantum) YCCMap[(int) DownScale(blue)];
              break;
            }
            default:
            {
              q->red=(Quantum)
                ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
              q->green=(Quantum)
                ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
              q->blue=(Quantum)
                ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
              break;
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(TransformRGBImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (long) image->colors; i++)
      {
        red=red_map[image->colormap[i].red+R]+
          green_map[image->colormap[i].green+R]+
          blue_map[image->colormap[i].blue+R];
        green=red_map[image->colormap[i].red+G]+
          green_map[image->colormap[i].green+G]+
          blue_map[image->colormap[i].blue+G];
        blue=red_map[image->colormap[i].red+B]+
          green_map[image->colormap[i].green+B]+
          blue_map[image->colormap[i].blue+B];
        switch (colorspace)
        {
          case sRGBColorspace:
          case YCCColorspace:
          {
            red=(red < 0) ? 0 : (red > UpScale(350)) ? UpScale(350) : red+0.5;
            green=(green < 0) ? 0 : (green > UpScale(350)) ? UpScale(350) :
              green+0.5;
            blue=(blue < 0) ? 0 : (blue > UpScale(350)) ? UpScale(350) :
              blue+0.5;
            if (colorspace == sRGBColorspace)
              {
                image->colormap[i].red=sRGBMap[(int) DownScale(red)];
                image->colormap[i].green=sRGBMap[(int) DownScale(green)];
                image->colormap[i].blue=sRGBMap[(int) DownScale(blue)];
                break;
              }
            image->colormap[i].red=YCCMap[(int) DownScale(red)];
            image->colormap[i].green=YCCMap[(int) DownScale(green)];
            image->colormap[i].blue=YCCMap[(int) DownScale(blue)];
            break;
          }
          default:
          {
            image->colormap[i].red=(Quantum)
              ((red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5);
            image->colormap[i].green=(Quantum)
              ((green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5);
            image->colormap[i].blue=(Quantum)
              ((blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5);
            break;
          }
        }
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocate memory.
  */
  LiberateMemory((void **) &blue_map);
  LiberateMemory((void **) &green_map);
  LiberateMemory((void **) &red_map);
  return(True);
}
