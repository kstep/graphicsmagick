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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
#include "magic.h"
#if defined(HasLTDL) || defined(_MAGICKMOD_)
#include "modules.h"
#endif

/*
  Constant declaration.
*/
const char
  *AppendBinaryType = "ab",
  *BackgroundColor = "#dfdfdf",  /* gray */
  *BorderColor = "#dfdfdf",  /* gray */
  *DefaultPointSize = "12",
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileGeometry = "106x106+4+3>",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *ForegroundColor = "#000000",  /* black */
  *LoadImageText = "  Loading image...  ",
  *LoadImagesText = "  Loading images...  ",
  *MatteColor = "#bdbdbd",  /* gray */
  *PSDensityGeometry = "72x72",
  *PSPageGeometry = "612x792>",
  *ReadBinaryType = "rb",
  *ReadBinaryUnbufferedType = "rbu",
  *SaveImageText = "  Saving image...  ",
  *SaveImagesText = "  Saving images...  ",
  *WriteBinaryType = "wb";

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
%  Method AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage method is:
%
%      Image *AllocateImage(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o allocate_image: Method AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
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
  /*
    Initialize Image structure.
  */
  allocate_image->file=(FILE *) NULL;
  GetBlobInfo(&allocate_image->blob);
  allocate_image->exempt=False;
  allocate_image->status=False;
  allocate_image->temporary=False;
  *allocate_image->filename='\0';
  allocate_image->filesize=0;
  allocate_image->pipe=False;
  (void) strcpy(allocate_image->magick,"MIFF");
  allocate_image->attributes=(ImageAttribute *) NULL;
  allocate_image->storage_class=DirectClass;
  allocate_image->matte=False;
  allocate_image->compression=UndefinedCompression;
  allocate_image->columns=0;
  allocate_image->rows=0;
  allocate_image->depth=QuantumDepth;
  allocate_image->tile_info.width=0;
  allocate_image->tile_info.height=0;
  allocate_image->tile_info.x=0;
  allocate_image->tile_info.y=0;
  allocate_image->offset=0;
  allocate_image->interlace=NoInterlace;
  allocate_image->scene=0;
  allocate_image->units=UndefinedResolution;
  allocate_image->x_resolution=0.0;
  allocate_image->y_resolution=0.0;
  allocate_image->montage=(char *) NULL;
  allocate_image->directory=(char *) NULL;
  allocate_image->colormap=(PixelPacket *) NULL;
  allocate_image->colors=0;
  allocate_image->colorspace=RGBColorspace;
  allocate_image->rendering_intent=UndefinedIntent;
  allocate_image->gamma=0.0;
  allocate_image->chromaticity.red_primary.x=0.0;
  allocate_image->chromaticity.red_primary.y=0.0;
  allocate_image->chromaticity.green_primary.x=0.0;
  allocate_image->chromaticity.green_primary.y=0.0;
  allocate_image->chromaticity.blue_primary.x=0.0;
  allocate_image->chromaticity.blue_primary.y=0.0;
  allocate_image->chromaticity.white_point.x=0.0;
  allocate_image->chromaticity.white_point.y=0.0;
  allocate_image->color_profile.name=(char *) NULL;
  allocate_image->color_profile.length=0;
  allocate_image->color_profile.info=(unsigned char *) NULL;
  allocate_image->iptc_profile.name=(char *) NULL;
  allocate_image->iptc_profile.length=0;
  allocate_image->iptc_profile.info=(unsigned char *) NULL;
  (void) QueryColorDatabase(BackgroundColor,&allocate_image->background_color);
  (void) QueryColorDatabase(BorderColor,&allocate_image->border_color);
  (void) QueryColorDatabase(MatteColor,&allocate_image->matte_color);
  allocate_image->geometry=(char *) NULL;
  GetPageInfo(&allocate_image->page);
  allocate_image->dispose=0;
  allocate_image->delay=0;
  allocate_image->iterations=1;
  allocate_image->fuzz=0;
  allocate_image->filter=LanczosFilter;
  allocate_image->blur=1.0;
  allocate_image->total_colors=0;
  allocate_image->normalized_mean_error=0.0;
  allocate_image->normalized_maximum_error=0.0;
  allocate_image->mean_error_per_pixel=0;
  *allocate_image->magick_filename='\0';
  allocate_image->magick_columns=0;
  allocate_image->magick_rows=0;
  allocate_image->restart_animation_here=False;
  allocate_image->taint=False;
  allocate_image->bounding_box.x1=0.0;
  allocate_image->bounding_box.y1=0.0;
  allocate_image->bounding_box.x2=0.0;
  allocate_image->bounding_box.y2=0.0;
  allocate_image->ascii85.offset=0;
  allocate_image->ascii85.line_break=0;
  GetExceptionInfo(&allocate_image->exception);
  GetTimerInfo(&allocate_image->timer);
  allocate_image->cache=(void *) NULL;
  allocate_image->fifo=(int (*)(const Image *,const void *,const size_t)) NULL;
  allocate_image->orphan=False;
  allocate_image->signature=MagickSignature;
  allocate_image->previous=(Image *) NULL;
  allocate_image->list=(Image *) NULL;
  allocate_image->next=(Image *) NULL;
  if (image_info == (ImageInfo *) NULL)
    return(allocate_image);
  /*
    Transfer image info.
  */
  allocate_image->blob=image_info->blob;
  allocate_image->exempt=image_info->file != (FILE *) NULL;
  (void) strcpy(allocate_image->filename,image_info->filename);
  (void) strcpy(allocate_image->magick_filename,image_info->filename);
  (void) strcpy(allocate_image->magick,image_info->magick);
  if (image_info->size != (char *) NULL)
    {
      int
        y;

      (void) sscanf(image_info->size,"%ux%u",
        &allocate_image->columns,&allocate_image->rows);
      flags=ParseGeometry(image_info->size,&allocate_image->offset,&y,
        &allocate_image->columns,&allocate_image->rows);
      if ((flags & HeightValue) == 0)
        allocate_image->rows=allocate_image->columns;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      {
        (void) sscanf(image_info->tile,"%ux%u",
          &allocate_image->columns,&allocate_image->rows);
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
    ParseImageGeometry(PostscriptGeometry(image_info->page),
      &allocate_image->page.x,&allocate_image->page.y,
      &allocate_image->page.width,&allocate_image->page.height);
  if (image_info->dispose != (char *) NULL)
    allocate_image->dispose=atoi(image_info->dispose);
  if (image_info->delay != (char *) NULL)
    allocate_image->delay=atoi(image_info->delay);
  if (image_info->iterations != (char *) NULL)
    allocate_image->iterations=atoi(image_info->iterations);
  allocate_image->depth=image_info->depth;
  allocate_image->background_color=image_info->background_color;
  allocate_image->border_color=image_info->border_color;
  allocate_image->matte_color=image_info->matte_color;
  allocate_image->fifo=image_info->fifo;
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
%  Method AllocateImageColormap allocates an Image colormap and initializes it.
%  The minimum number of colormap cells allocated is 256.
%
%  The format of the AllocateNextImage method is:
%
%      unsigned int AllocateImageColormap(Image *image,
%        const unsigned int colors)
%
%  A description of each parameter follows:
%
%    o status: Method AllocateImageColormap returns True if the colormap is
%      successfully allocated and initialized, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o colors: The number of colors in the image colormap.
%
%
*/
MagickExport unsigned int AllocateImageColormap(Image *image,
  const unsigned int colors)
{
  register int
    i;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(colors != 0);
  image->storage_class=PseudoClass;
  image->colors=colors;
  image->colormap=(PixelPacket *)
    AcquireMemory(Max(colors,256)*sizeof(PixelPacket));
  if (image->colormap == (PixelPacket *) NULL)
    return(False);
  for (i=0; i < (int) colors; i++)
  {
    image->colormap[i].red=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
    image->colormap[i].green=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
    image->colormap[i].blue=((unsigned long) (MaxRGB*i)/Max(colors-1,1));
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
%  Method AllocateNextImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateNextImage method is:
%
%      void AllocateNextImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
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
  (void) strcpy(image->next->filename,image->filename);
  if (image_info != (ImageInfo *) NULL)
    (void) strcpy(image->next->filename,image_info->filename);
  image->next->blob=image->blob;
  image->next->filesize=image->filesize;
  image->next->file=image->file;
  image->next->filesize=image->filesize;
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

#if defined(HasX11)
#include "xwindows.h"
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
%  Method AnimateImages displays one or more images to an X window.
%
%  The format of the AnimateImages method is:
%
%      unsigned int AnimateImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method AnimateImages returns True if the images are displayed
%      in an X window, otherwise False is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  char
    *client_name;

  Display
    *display;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  if (image_info->delay != (char *) NULL)
    resource.delay=atoi(image_info->delay);
  (void) XAnimateImages(display,&resource,&client_name,1,image);
  XCloseDisplay(display);
  return(image->exception.severity == UndefinedException);
}
#else
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (ImageInfo *) NULL);
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
%  Method AppendImages appends a sequence of images.  All the input images must
%  have the same width or height.  Images of the same width are stacked
%  top-to-bottom.  Images of the same height are stacked left-to-right.
%  If stack is false, rectangular images are stacked left-to-right otherwise
%  top-to-bottom.
%
%  The format of the AppendImage method is:
%
%      Image *AppendImages(Image *image,const unsigned int stack,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o stack: An unsigned value other than stacks rectangular image
%      top-to-bottom otherwise left-to-right.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AppendImages(Image *image,const unsigned int stack,
  ExceptionInfo *exception)
{
#define AppendImageText  "  Appending image sequence...  "

  Image
    *append_image;

  register Image
    *next;

  register int
    i;

  unsigned int
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
      register int
        x;

      /*
        Stack left-to-right.
      */
      x=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          append_image->storage_class=DirectClass;
        CompositeImage(append_image,ReplaceCompositeOp,next,x,0);
        x+=next->columns;
        ProgressMonitor(AppendImageText,scene++,GetNumberScenes(image));
      }
    }
  else
    {
      register int
        y;

      /*
        Stack top-to-bottom.
      */
      y=0;
      for (next=image; next != (Image *) NULL; next=next->next)
      {
        if (next->storage_class == DirectClass)
          append_image->storage_class=DirectClass;
        CompositeImage(append_image,ReplaceCompositeOp,next,0,y);
        y+=next->rows;
        ProgressMonitor(AppendImageText,scene,GetNumberScenes(image));
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
        for (i=0; i < (int) image->colors; i++)
          if (!ColorMatch(next->colormap[i],image->colormap[i],next->fuzz))
            {
              global_colormap=False;
              break;
            }
      }
      if (!global_colormap)
        append_image->storage_class=DirectClass;
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
%  Method AverageImages averages a sequence of images.  All the input image
%  must be the same size in pixels.
%
%  The format of the AverageImage method is:
%
%      Image *AverageImages(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o average_image: Method AverageImages returns the mean pixel value
%      for an image sequence.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *AverageImages(Image *image,ExceptionInfo *exception)
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

  int
    y;

  register Image
    *next;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  SumPacket
    *sum;

  unsigned int
    number_scenes;

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
        "images are not the same size");
  }
  /*
    Allocate sum accumulation buffer.
  */
  sum=(SumPacket *)
    AcquireMemory(image->columns*image->rows*sizeof(SumPacket));
  if (sum == (SumPacket *) NULL)
    ThrowImageException(ResourceLimitWarning,"Unable to average image sequence",
      "Memory allocation failed");
  for (i=0; i < (int) (image->columns*image->rows); i++)
  {
    sum[i].red=0.0;
    sum[i].green=0.0;
    sum[i].blue=0.0;
    sum[i].opacity=0.0;
  }
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      LiberateMemory((void **) &sum);
      return((Image *) NULL);
    }
  average_image->storage_class=DirectClass;
  /*
    Compute sum over each pixel color component.
  */
  number_scenes=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    i=0;
    for (y=0; y < (int) next->rows; y++)
    {
      p=GetImagePixels(next,0,y,next->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) next->columns; x++)
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
  for (y=0; y < (int) average_image->rows; y++)
  {
    q=SetImagePixels(average_image,0,y,average_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) average_image->columns; x++)
    {
      q->red=(sum[i].red+number_scenes/2.0)/number_scenes;
      q->green=(sum[i].green+number_scenes/2.0)/number_scenes;
      q->blue=(sum[i].blue+number_scenes/2.0)/number_scenes;
      q->opacity=(sum[i].opacity+number_scenes/2.0)/number_scenes;
      q++;
      i++;
    }
    if (!SyncImagePixels(average_image))
      break;
    if (QuantumTick(y,average_image->rows))
      ProgressMonitor(AverageImageText,y,average_image->rows);
  }
  LiberateMemory((void **) &sum);
  return(average_image);
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
%  Method CloneImage returns a copy of all fields of the input image.  The
%  image pixels and indexes are copied only if the columns and rows of the
%  cloned image are the same as the original otherwise the pixel data is
%  undefined and must be initialized with SetImagePixels() and SyncImagePixels()
%  methods.
%
%  The format of the CloneImage method is:
%
%      Image *CloneImage(Image *image,const unsigned int columns,
%        const unsigned int rows,const unsigned int orphan,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o clone_image: Method CloneImage returns a pointer to the image after
%      copying.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the copied
%      image.
%
%    o rows: An integer that specifies the number of rows in the copied
%      image.
%
%    o orphan:  if true, consider this image an orphan.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *CloneImage(Image *image,const unsigned int columns,
  const unsigned int rows,const unsigned int orphan,ExceptionInfo *exception)
{
  Image
    *clone_image;

  ImageAttribute
    *attribute;

  unsigned long
    length;

  /*
    Allocate image structure.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_image=(Image *) AcquireMemory(sizeof(Image));
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Clone the image.
  */
  *clone_image=(*image);
  clone_image->montage=(char *) NULL;
  clone_image->directory=(char *) NULL;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      length=image->colors*sizeof(PixelPacket);
      clone_image->colormap=(PixelPacket *) AcquireMemory(length);
      if (clone_image->colormap == (PixelPacket *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      memcpy(clone_image->colormap,image->colormap,length);
    }
  if (image->color_profile.length > 0)
    {
      /*
        Allocate and copy the image ICC profile.
      */
      length=image->color_profile.length;
      clone_image->color_profile.info=(unsigned char *) AcquireMemory(length);
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      memcpy(clone_image->color_profile.info,image->color_profile.info,length);
    }
  if (image->iptc_profile.length > 0)
    {
      /*
        Allocate and copy the image IPTC profile.
      */
      length=image->iptc_profile.length;
      clone_image->iptc_profile.info=(unsigned char *) AcquireMemory(length);
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        ThrowImageException(ResourceLimitWarning,"Unable to clone image",
          "Memory allocation failed");
      memcpy(clone_image->iptc_profile.info,image->iptc_profile.info,length);
    }
  GetBlobInfo(&clone_image->blob);
  clone_image->cache=(void *) NULL;
  if ((image->columns != columns) || (image->rows != rows))
    {
      clone_image->columns=columns;
      clone_image->rows=rows;
      GetPageInfo(&clone_image->page);
    }
  else
    {
      int
        y;

      register IndexPacket
        *clone_indexes,
        *indexes;

      register PixelPacket
        *p,
        *q;

      /*
        Clone pixel cache.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        p=GetImagePixels(image,0,y,image->columns,1);
        q=SetImagePixels(clone_image,0,y,clone_image->columns,1);
        if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          break;
        memcpy(q,p,image->columns*sizeof(PixelPacket));
        indexes=GetIndexes(image);
        clone_indexes=GetIndexes(clone_image);
        if (image->storage_class == PseudoClass)
          memcpy(clone_indexes,indexes,image->columns*sizeof(IndexPacket));
        if (!SyncImagePixels(clone_image))
          break;
      }
      if (y < image->rows)
        ThrowImageException(CacheWarning,"Unable to clone image",
          "could not get image cache");
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
    }
  clone_image->attributes=(ImageAttribute *) NULL;
  attribute=GetImageAttribute(image,(char *) NULL);
  while (attribute != (ImageAttribute *) NULL)
  {
    (void) SetImageAttribute(clone_image,attribute->key,attribute->value);
    attribute=attribute->next;
  }
  GetExceptionInfo(&clone_image->exception);
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
%  Method CloneImageInfo makes a duplicate of the given image info, or if
%  image info is NULL, a new one.
%
%  The format of the CloneImageInfo method is:
%
%      ImageInfo *CloneImageInfo(const ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneImageInfo returns a duplicate of the given
%      image info, or if image info is NULL a new one.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
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
  if (image_info->dispose != (char *) NULL)
    clone_info->dispose=AllocateString(image_info->dispose);
  if (image_info->delay != (char *) NULL)
    clone_info->delay=AllocateString(image_info->delay);
  if (image_info->iterations != (char *) NULL)
    clone_info->iterations=AllocateString(image_info->iterations);
  if (image_info->view != (char *) NULL)
    clone_info->view=AllocateString(image_info->view);
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
%  Method CompositeImage returns the second image composited onto the
%  first at the specified offsets.
%
%  The format of the CompositeImage method is:
%
%      unsigned int CompositeImage(Image *image,const CompositeOperator compose,
%        Image *composite_image,const int x_offset,const int y_offset)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o compose: Specifies an image composite operator.
%
%    o composite_image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the column offset of the composited
%      image.
%
%    o y_offset: An integer that specifies the row offset of the composited
%      image.
%
%
*/
MagickExport unsigned int CompositeImage(Image *image,
  const CompositeOperator compose,Image *composite_image,const int x_offset,
  const int y_offset)
{
  int
    y;

  double
    alpha,
    blue,
    green,
    midpoint,
    opacity,
    red;

  PixelPacket
    color;

  Quantum
    shade;

  register IndexPacket
    *composite_indexes,
    *indexes;

  register int
    x;

  register PixelPacket
    *p,
    *q;

  double
    saturation_scale,
    brightness_scale,
    brightness,
    hue,
    saturation;

  /*
    Prepare composite image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(composite_image != (Image *) NULL);
  assert(composite_image->signature == MagickSignature);
  alpha=1.0/MaxRGB;
  red=0.0;
  green=0.0;
  blue=0.0;
  midpoint=0.0;
  switch (compose)
  {
    case XorCompositeOp:
    case PlusCompositeOp:
    case MinusCompositeOp:
    case AddCompositeOp:
    case SubtractCompositeOp:
    case DifferenceCompositeOp:
    case BumpmapCompositeOp:
    case BlendCompositeOp:
    case ReplaceRedCompositeOp:
    case ReplaceGreenCompositeOp:
    case ReplaceBlueCompositeOp:
    {
      image->storage_class=DirectClass;
      break;
    }
    case ReplaceMatteCompositeOp:
    {
      if (!image->matte)
        MatteImage(image,OpaqueOpacity);
      break;
    }
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
      displace_image=CloneImage(composite_image,composite_image->columns,
        composite_image->rows,True,&image->exception);
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
      for (y=0; y < (int) composite_image->rows; y++)
      {
        if (((y+y_offset) < 0) || ((y+y_offset) >= (int) image->rows))
          continue;
        p=GetImagePixels(composite_image,0,y,composite_image->columns,1);
        q=GetImagePixels(image,0,y+y_offset,image->columns,1);
        r=GetImagePixels(displace_image,0,y,displace_image->columns,1);
        if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL) ||
            (r == (PixelPacket *) NULL))
          break;
        q+=x_offset;
        for (x=0; x < (int) composite_image->columns; x++)
        {
          if (((x_offset+x) < 0) || ((x_offset+x) >= (int) image->columns))
            {
              p++;
              q++;
              continue;
            }
          x_displace=(horizontal_scale*((double) Intensity(*p)-
            ((MaxRGB+1)/2)))/((MaxRGB+1)/2);
          y_displace=x_displace;
          if (composite_image->matte)
            y_displace=(vertical_scale*((double) p->opacity-
              ((MaxRGB+1)/2)))/((MaxRGB+1)/2);
          *r=InterpolateColor(image,x_offset+x+x_displace,
            y_offset+y+y_displace);
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
      midpoint=MaxRGB/2.0;
      saturation_scale=50.0;
      brightness_scale=50.0;
      if (composite_image->geometry != (char *) NULL)
        {
          int
            count;

          /*
            Determine the brightness and saturation scale.
          */
          count=sscanf(composite_image->geometry,"%lfx%lf\n",
            &brightness_scale,&saturation_scale);
          if (count == 1)
            saturation_scale=brightness_scale;
        }
      brightness_scale/=100.0;
      saturation_scale/=100.0;
      break;
    }
    case ReplaceCompositeOp:
    {
      /*
        Promote image to DirectClass if colormaps differ.
      */
      if (image->storage_class == PseudoClass)
        {
          if ((composite_image->storage_class == DirectClass) ||
              (composite_image->colors != image->colors))
            image->storage_class=DirectClass;
          else
            {
              for (x=0; x < (int) image->colors; x++)
                if (!ColorMatch(image->colormap[x],
                     composite_image->colormap[x],0))
                  {
                    image->storage_class=DirectClass;
                    break;
                  }
            }
        }
      if (image->colorspace == RGBColorspace)
        if (image->matte && !composite_image->matte)
          MatteImage(composite_image,OpaqueOpacity);
      break;
    }
    default:
    {
      /*
        Initialize image matte data.
      */
      image->storage_class=DirectClass;
      if (!image->matte)
        MatteImage(image,OpaqueOpacity);
      if (!composite_image->matte)
        {
          (void) IsMonochromeImage(composite_image);
          for (y=0; y < (int) composite_image->rows; y++)
          {
            p=GetImagePixels(composite_image,0,y,composite_image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            if (y == 0)
              {
                red=p->red;
                green=p->green;
                blue=p->blue;
                if (IsMonochromeImage(composite_image))
                  {
                    red=composite_image->background_color.red;
                    green=composite_image->background_color.green;
                    blue=composite_image->background_color.blue;
                  }
              }
            for (x=0; x < (int) composite_image->columns; x++)
            {
              p->opacity=OpaqueOpacity;
              if ((p->red == red) && (p->green == green) &&
                  (p->blue == blue))
                p->opacity=TransparentOpacity;
              p++;
            }
            if (!SyncImagePixels(composite_image))
              break;
          }
          composite_image->storage_class=DirectClass;
          composite_image->matte=True;
        }
      break;
    }
  }
  /*
    Composite image.
  */
  for (y=0; y < image->rows; y++)
  {
    if (y < y_offset)
      continue;
    if ((y-y_offset) >= composite_image->rows)
      break;
    q=GetImagePixels(image,0,y,image->columns,1);
    indexes=GetIndexes(image);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < image->columns; x++)
    {
      if (x < x_offset)
        {
          q++;
          continue;
        }
      if ((x-x_offset) >= composite_image->columns)
        break;
      p=GetImagePixels(composite_image,x-x_offset,y-y_offset,1,1);
      if ((p == (PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        break;
      composite_indexes=GetIndexes(composite_image);
      opacity=q->opacity;
      switch (compose)
      {
        case AnnotateCompositeOp:
        case OverCompositeOp:
        default:
        {
          if (composite_image->matte && (p->opacity == TransparentOpacity))
            {
              red=q->red;
              green=q->green;
              blue=q->blue;
              opacity=q->opacity;
            }
          else
            if (composite_image->matte && (p->opacity == OpaqueOpacity))
              {
                red=p->red;
                green=p->green;
                blue=p->blue;
                opacity=p->opacity;
              }
            else
              {
                red=alpha*(p->red*(MaxRGB-p->opacity)+q->red*p->opacity);
                green=alpha*(p->green*(MaxRGB-p->opacity)+q->green*p->opacity);
                blue=alpha*(p->blue*(MaxRGB-p->opacity)+q->blue*p->opacity);
                if (composite_image->matte)
                  opacity=alpha*(p->opacity*(MaxRGB-p->opacity)+
                    q->opacity*p->opacity);
              }
          break;
        }
        case InCompositeOp:
        {
          red=alpha*(p->red*(MaxRGB-q->opacity));
          green=alpha*(p->green*(MaxRGB-q->opacity));
          blue=alpha*(p->blue*(MaxRGB-q->opacity));
          if (composite_image->matte)
            opacity=alpha*(p->opacity*(MaxRGB-q->opacity));
          break;
        }
        case OutCompositeOp:
        {
          red=alpha*(p->red*q->opacity);
          green=alpha*(p->green*q->opacity);
          blue=alpha*(p->blue*q->opacity);
          if (composite_image->matte)
            opacity=alpha*(p->opacity*q->opacity);
          break;
        }
        case AtopCompositeOp:
        {
          red=alpha*(p->red*(MaxRGB-q->opacity)+q->red*p->opacity);
          green=alpha*(p->green*(MaxRGB-q->opacity)+q->green*p->opacity);
          blue=alpha*(p->blue*(MaxRGB-q->opacity)+q->blue*p->opacity);
          if (composite_image->matte)
            opacity=alpha*(p->opacity*(MaxRGB-q->opacity)+
              q->opacity*p->opacity);
          break;
        }
        case XorCompositeOp:
        {
          red=alpha*(p->red*q->opacity+q->red*p->opacity);
          green=alpha*(p->green*q->opacity+q->green*p->opacity);
          blue=alpha*(p->blue*q->opacity+q->blue*p->opacity);
          if (composite_image->matte)
            opacity=alpha*(p->opacity*q->opacity+q->opacity*p->opacity);
          break;
        }
        case PlusCompositeOp:
        {
          red=p->red+q->red;
          green=p->green+q->green;
          blue=p->blue+q->blue;
          if (composite_image->matte)
            opacity=p->opacity+q->opacity;
          break;
        }
        case MinusCompositeOp:
        {
          red=p->red-(int) q->red;
          green=p->green-(int) q->green;
          blue=p->blue-(int) q->blue;
          opacity=OpaqueOpacity;
          break;
        }
        case AddCompositeOp:
        {
          red=p->red+q->red;
          if (red > MaxRGB)
            red-=(MaxRGB+1);
          green=p->green+q->green;
          if (green > MaxRGB)
            green-=(MaxRGB+1);
          blue=p->blue+q->blue;
          if (blue > MaxRGB)
            blue-=(MaxRGB+1);
          if (composite_image->matte)
            {
              opacity=p->opacity+q->opacity;
              if (opacity > MaxRGB)
                opacity-=(MaxRGB+1);
            }
          break;
        }
        case SubtractCompositeOp:
        {
          red=p->red-(int) q->red;
          if (red < 0)
            red+=(MaxRGB+1);
          green=p->green-(int) q->green;
          if (green < 0)
            green+=(MaxRGB+1);
          blue=p->blue-(int) q->blue;
          if (blue < 0)
            blue+=(MaxRGB+1);
          if (composite_image->matte)
            {
              opacity=p->opacity-(int) q->opacity;
              if (opacity < 0)
                opacity+=(MaxRGB+1);
            }
          break;
        }
        case DifferenceCompositeOp:
        {
          red=AbsoluteValue(p->red-(int) q->red);
          green=AbsoluteValue(p->green-(int) q->green);
          blue=AbsoluteValue(p->blue-(int) q->blue);
          if (composite_image->matte)
            opacity=AbsoluteValue(p->opacity-(int) q->opacity);
          break;
        }
        case BumpmapCompositeOp:
        {
          shade=Intensity(*p);
          red=alpha*(q->red*shade);
          green=alpha*(q->green*shade);
          blue=alpha*(q->blue*shade);
          opacity=alpha*(q->opacity*shade);
          break;
        }
        case ReplaceCompositeOp:
        {
          red=p->red;
          green=p->green;
          blue=p->blue;
          if ((image->colorspace != RGBColorspace) || composite_image->matte)
            opacity=p->opacity;
          break;
        }
        case ReplaceRedCompositeOp:
        {
          red=DownScale(Intensity(*p));
          green=q->green;
          blue=q->blue;
          break;
        }
        case ReplaceGreenCompositeOp:
        {
          red=q->red;
          green=DownScale(Intensity(*p));
          blue=q->blue;
          break;
        }
        case ReplaceBlueCompositeOp:
        {
          red=q->red;
          green=q->green;
          blue=DownScale(Intensity(*p));
          break;
        }
        case ReplaceMatteCompositeOp:
        {
          red=q->red;
          green=q->green;
          blue=q->blue;
          opacity=DownScale(Intensity(*p));
          break;
        }
        case BlendCompositeOp:
        {
          red=alpha*(p->red*p->opacity+q->red*(MaxRGB-p->opacity));
          green=alpha*(p->green*p->opacity+q->green*(MaxRGB-p->opacity));
          blue=alpha*(p->blue*p->opacity+q->blue*(MaxRGB-p->opacity));
          opacity=OpaqueOpacity;
          break;
        }
        case DisplaceCompositeOp:
        {
          red=p->red;
          green=p->green;
          blue=p->blue;
          if (composite_image->matte)
            opacity=p->opacity;
          break;
        }
        case ModulateCompositeOp:
        {
          if (Intensity(*p) != midpoint)
            {
              double
                percent_brightness;

              /*
                Pixel is not at 0 point.
              */
              color=(*q);
              TransformHSL(color.red,color.green,color.blue,&hue,&saturation,
                &brightness);
              percent_brightness=(brightness_scale*
                ((double) Intensity(*p)-midpoint))/midpoint;
              brightness*=percent_brightness/100.0;
              if (brightness < 0.0)
                brightness=0.0;
              else
                if (brightness > 1.0)
                  brightness=1.0;
              HSLTransform(hue,saturation,brightness,&color.red,&color.green,
                &color.blue);
              red=color.red;
              green=color.green;
              blue=color.blue;
              break;
            }
          red=q->red;
          green=q->green;
          blue=q->blue;
          break;
        }
      }
      if (image->storage_class == PseudoClass)
        if (image->storage_class == composite_image->storage_class)
          indexes[x]=composite_indexes[x-x_offset];
      q->red=(red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
      q->green=(green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
      q->blue=(blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
      q->opacity=(opacity < 0) ? 0 : (opacity > MaxRGB) ? MaxRGB : opacity+0.5;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  if (compose == DisplaceCompositeOp)
    DestroyImage(composite_image);
  (void) IsMatteImage(image);
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
%  Method CycleColormapImage cycles the image colormap by a specified
%  amount.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(image,amount)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o amount:  An unsigned value that specifies the offset of the colormap.
%
%
*/
MagickExport void CycleColormapImage(Image *image,const int amount)
{
#define CycleColormapImageText  "  Cycling image...  "

  int
    index,
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=MaxRGB+1;
      (void) QuantizeImage(&quantize_info,image);
    }
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
    {
      index=((int) indexes[x]+amount) % image->colors;
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
%  Method DescribeImage describes an image by printing its attributes to
%  stdout.
%
%  The format of the DescribeImage method is:
%
%      void DescribeImage(Image *image,FILE *file,const unsigned int verbose)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o file: send the image attributes to this file.
%
%    o verbose: an unsigned value other than zero prints detailed information
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

  Image
    *p;

  ImageAttribute
    *attribute;

  int
    y;

  MagickInfo
    *magick_info;

  register int
    i,
    x;

  unsigned int
    count;

  unsigned long
    number_colors;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(file != (FILE *) NULL);
  elapsed_time=GetElapsedTime(&image->timer);
  user_time=GetUserTime(&image->timer);
  ContinueTimer(&image->timer);
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
        (void) fprintf(file,"%.1024s[%u] ",image->filename,image->scene);
      if ((image->magick_columns != 0) || (image->magick_rows != 0))
        if ((image->magick_columns != image->columns) ||
            (image->magick_rows != image->rows))
          (void) fprintf(file,"%ux%u=>",image->magick_columns,
            image->magick_rows);
      if ((image->page.width <= 1) || (image->page.height <= 1))
        (void) fprintf(file,"%ux%u ",image->columns,image->rows);
      else
        (void) fprintf(file,"%ux%u%+d%+d ",image->page.width,
          image->page.height,image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            (void) fprintf(file,"%luc ",image->total_colors);
        }
      else
        if (image->total_colors <= image->colors)
          (void) fprintf(file,"PseudoClass %uc ",image->colors);
        else
          {
            (void) fprintf(file,"PseudoClass %lu=>%uc ",image->total_colors,
              image->colors);
            (void) fprintf(file,"%u/%.6f/%.6fe ",image->mean_error_per_pixel,
              image->normalized_mean_error,image->normalized_maximum_error);
          }
      if (image->filesize != 0)
        {
          if (image->filesize >= (1 << 24))
            (void) fprintf(file,"%dmb ",image->filesize/1024/1024);
          else
            if (image->filesize >= (1 << 14))
              (void) fprintf(file,"%dkb ",image->filesize/1024);
            else
              (void) fprintf(file,"%db ",image->filesize);
        }
      (void) fprintf(file,"%.1024s %.1fu %d:%02d\n",image->magick,user_time,
        (int) (elapsed_time/60.0),(int) ceil(fmod(elapsed_time,60.0)));
      return;
    }
  /*
    Display verbose info about the image.
  */
  SignatureImage(image);
  number_colors=GetNumberColors(image,(FILE *) NULL);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=(MagickInfo *) GetMagickInfo(image->magick);
  if ((magick_info == (MagickInfo *) NULL) ||
      (*magick_info->description == '\0'))
    (void) fprintf(file,"  Format: %.1024s\n",image->magick);
  else
    (void) fprintf(file,"  Format: %.1024s (%s)\n",image->magick,
      magick_info->description);
  (void) fprintf(file,"  Type: ");
  switch (GetImageType(image))
  {
    case BilevelType: (void) fprintf(file,"bilevel"); break;
    case GrayscaleType: (void) fprintf(file,"grayscale"); break;
    case PaletteType: (void) fprintf(file,"palette"); break;
    case PaletteMatteType:
      (void) fprintf(file,"palette with transparency"); break;
    case TrueColorType: (void) fprintf(file,"true color"); break;
    case TrueColorMatteType:
      (void) fprintf(file,"true color with transparency"); break;
    case ColorSeparationType: (void) fprintf(file,"color separated"); break;
    default: (void) fprintf(file,"undefined"); break;
  }
  (void) fprintf(file,"\n");
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Class: DirectClass\n");
  else
    (void) fprintf(file,"  Class: PseudoClass\n");
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(file,"  Base Geometry: %ux%u\n",image->magick_columns,
        image->magick_rows);
  (void) fprintf(file,"  Geometry: %ux%u\n",image->columns,image->rows);
  (void) fprintf(file,"  Depth: %u\n",image->depth);
  x=0;
  p=(Image *) NULL;
  if (!image->matte)
    (void) fprintf(file,"  Matte: False\n");
  else
    if ((strcmp(image->magick,"GIF") != 0) || image->taint)
      (void) fprintf(file,"  Matte: True\n");
    else
      {
        PixelPacket
          *p;

        p=(PixelPacket *) NULL;
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            if (p->opacity == TransparentOpacity)
              break;
            p++;
          }
          if (x < (int) image->columns)
            break;
        }
        if ((x < (int) image->columns) || (y < (int) image->rows))
          {
            if (image->depth == 8)
              (void) fprintf(file,"  Matte: (%3d,%3d,%3d) #%02x%02x%02x\n",
                p->red,p->green,p->blue,p->red,p->green,p->blue);
            else
              (void) fprintf(file,"  Matte: (%5d,%5d,%5d) #%04x%04x%04x\n",
                p->red,p->green,p->blue,p->red,p->green,p->blue);
          }
      }
  if (image->storage_class == DirectClass)
    (void) fprintf(file,"  Colors: %lu\n",number_colors);
  else
    if (number_colors <= image->colors)
      (void) fprintf(file,"  Colors: %u\n",image->colors);
    else
      (void) fprintf(file,"  Colors: %lu=>%u\n",number_colors,image->colors);
  if (image->storage_class == DirectClass)
    {
      if (number_colors < 1024)
        (void) GetNumberColors(image,file);
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
      for (i=0; i < (int) image->colors; i++)
      {
        (void) fprintf(file,"    %d: (%3d,%3d,%3d)",i,p->red,p->green,p->blue);
        (void) fprintf(file,"  ");
        (void) QueryColorName(p,name);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->mean_error_per_pixel != 0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %d\n",
      image->mean_error_per_pixel);
  if (image->normalized_mean_error != 0)
    (void) fprintf(file,"  Normalized Mean Exception: %.6f\n",
      image->normalized_mean_error);
  if (image->normalized_maximum_error != 0)
    (void) fprintf(file,"  Normalized Maximum Exception: %.6f\n",
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
  if (image->color_profile.length > 0)
    (void) fprintf(file,"  Color Profile: %u bytes\n",
      image->color_profile.length);
  if (image->iptc_profile.length > 0)
    {
      char
        *tag,
        *text;

      unsigned short
        length;

      /*
        Describe IPTC data.
      */
      (void) fprintf(file,"  IPTC Profile: %u bytes\n",
        image->iptc_profile.length);
      for (i=0; i < image->iptc_profile.length; i++)
      {
        if (image->iptc_profile.info[i] != 0x1c)
          continue;
        i++;  /* skip dataset byte */
        switch (image->iptc_profile.info[++i])
        {
          case 5: tag="Image Name"; break;
          case 7: tag="Edit Status"; break;
          case 10: tag="Priority"; break;
          case 15: tag="Category"; break;
          case 20: tag="Supplemental Category"; break;
          case 22: tag="Fixture Identifier"; break;
          case 25: tag="Keyword"; break;
          case 30: tag="Release Date"; break;
          case 35: tag="Release Time"; break;
          case 40: tag="Special Instructions"; break;
          case 45: tag="Reference Service"; break;
          case 47: tag="Reference Date"; break;
          case 50: tag="Reference Number"; break;
          case 55: tag="Created Date"; break;
          case 60: tag="Created Time"; break;
          case 65: tag="Originating Program"; break;
          case 70: tag="Program Version"; break;
          case 75: tag="Object Cycle"; break;
          case 80: tag="Byline"; break;
          case 85: tag="Byline Title"; break;
          case 90: tag="City"; break;
          case 95: tag="Province State"; break;
          case 100: tag="Country Code"; break;
          case 101: tag="Country"; break;
          case 103: tag="Original Transmission Reference"; break;
          case 105: tag="Headline"; break;
          case 110: tag="Credit"; break;
          case 115: tag="Source"; break;
          case 116: tag="Copyright String"; break;
          case 120: tag="Caption"; break;
          case 121: tag="Local Caption"; break;
          case 122: tag="Caption Writer"; break;
          case 200: tag="Custom Field 1"; break;
          case 201: tag="Custom Field 2"; break;
          case 202: tag="Custom Field 3"; break;
          case 203: tag="Custom Field 4"; break;
          case 204: tag="Custom Field 5"; break;
          case 205: tag="Custom Field 6"; break;
          case 206: tag="Custom Field 7"; break;
          case 207: tag="Custom Field 8"; break;
          case 208: tag="Custom Field 9"; break;
          case 209: tag="Custom Field 10"; break;
          case 210: tag="Custom Field 11"; break;
          case 211: tag="Custom Field 12"; break;
          case 212: tag="Custom Field 13"; break;
          case 213: tag="Custom Field 14"; break;
          case 214: tag="Custom Field 15"; break;
          case 215: tag="Custom Field 16"; break;
          case 216: tag="Custom Field 17"; break;
          case 217: tag="Custom Field 18"; break;
          case 218: tag="Custom Field 19"; break;
          case 219: tag="Custom Field 20"; break;
          default: tag="unknown"; break;
        }
        (void) fprintf(file,"    %s:\n",tag);
        length=image->iptc_profile.info[++i] << 8;
        length|=image->iptc_profile.info[++i];
        text=(char *) AcquireMemory(length+1);
        if (text != (char *) NULL)
          {
            char
              **textlist;

            register int
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
      }
    }
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile Geometry: %ux%u%+d%+d\n",image->tile_info.width,
      image->tile_info.height,image->tile_info.x,image->tile_info.y);
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
  if (image->filesize != 0)
    {
      if (image->filesize >= (1 << 24))
        (void) fprintf(file,"  Filesize: %dmb\n",image->filesize/1024/1024);
      else
        if (image->filesize >= (1 << 14))
          (void) fprintf(file,"  Filesize: %dkb\n",image->filesize/1024);
        else
          (void) fprintf(file,"  Filesize: %db\n",image->filesize);
    }
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
  (void) QueryColorName(&image->background_color,color);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorName(&image->border_color,color);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorName(&image->matte_color,color);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page Geometry: %ux%u%+d%+d\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  if (image->dispose != 0)
    (void) fprintf(file,"  Dispose Method: %d\n",image->dispose);
  if (image->delay != 0)
    (void) fprintf(file,"  Delay: %d\n",image->delay);
  if (image->iterations != 1)
    (void) fprintf(file,"  Iterations: %d\n",image->iterations);
  p=image;
  while (p->previous != (Image *) NULL)
    p=p->previous;
  for (count=1; p->next != (Image *) NULL; count++)
    p=p->next;
  if (count > 1)
    (void) fprintf(file,"  Scene: %u of %u\n",image->scene,count);
  else
    if (image->scene != 0)
      (void) fprintf(file,"  Scene: %u\n",image->scene);
  (void) fprintf(file,"  Compression: ");
  switch (image->compression)
  {
    case NoCompression: (void) fprintf(file,"None\n"); break;
    case BZipCompression: (void) fprintf(file,"BZip\n"); break;
    case FaxCompression: (void) fprintf(file,"Fax\n"); break;
    case Group4Compression: (void) fprintf(file,"Group 4\n"); break;
    case JPEGCompression: (void) fprintf(file,"JPEG\n"); break;
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
        (void) SetWarningHandler(handler);
        if (tile == (Image *) NULL)
          {
            (void) fprintf(file,"\n");
            continue;
          }
        (void) fprintf(file," %ux%u %.1024s\n",tile->magick_columns,
          tile->magick_rows,tile->magick);
        SignatureImage(tile);
        attribute=GetImageAttribute(tile,(char *) NULL);
        while (attribute != (ImageAttribute *) NULL)
        {
          (void) fprintf(file,"  %s:\n",attribute->key);
          (void) fprintf(file,"%s\n",attribute->value);
          attribute=attribute->next;
        }
        DestroyImage(tile);
      }
      DestroyImageInfo(image_info);
    }
  /*
    Display image attributes.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  while (attribute != (ImageAttribute *) NULL)
  {
    (void) fprintf(file,"  %s: ",attribute->key);
    (void) fprintf(file,"%s\n",attribute->value);
    attribute=attribute->next;
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
%  Method DestroyImage deallocates memory associated with an image.
%
%  The format of the DestroyImage method is:
%
%      void DestroyImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport void DestroyImage(Image *image)
{
  /*
    Close image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  DestroyBlobInfo(&image->blob);
  if (image->file != (FILE *) NULL)
    {
      CloseBlob(image);
      if (image->temporary)
        (void) remove(image->filename);
    }
  DestroyImagePixels(image);
  /*
    Deallocate the image montage directory.
  */
  if (image->montage != (char *) NULL)
    LiberateMemory((void **) &image->montage);
  if (image->directory != (char *) NULL)
    LiberateMemory((void **) &image->directory);
  /*
    Deallocate the image colormap.
  */
  if (image->colormap != (PixelPacket *) NULL)
    LiberateMemory((void **) &image->colormap);
  /*
    Deallocate the image ICC profile.
  */
  if (image->color_profile.length > 0)
    LiberateMemory((void **) &image->color_profile.info);
  /*
    Deallocate the image IPTC profile.
  */
  if (image->iptc_profile.length > 0)
    LiberateMemory((void **) &image->iptc_profile.info);
  /*
    Deallocate the image text attributes.
  */
  DestroyImageAttributes(image);
  /*
    Deallocate the exception info structure.
  */
  DestroyExceptionInfo(&image->exception);
  /*
    Deallocate the image pixels.
  */
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
%  Method DestroyImageInfo deallocates memory associated with an ImageInfo
%  structure.
%
%  The format of the DestroyImageInfo method is:
%
%      void DestroyImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
MagickExport void DestroyImageInfo(ImageInfo *image_info)
{
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  if (image_info->server_name != (char *) NULL)
    LiberateMemory((void **) &image_info->server_name);
  if (image_info->size != (char *) NULL)
    LiberateMemory((void **) &image_info->size);
  if (image_info->tile != (char *) NULL)
    LiberateMemory((void **) &image_info->tile);
  if (image_info->page != (char *) NULL)
    LiberateMemory((void **) &image_info->page);
  if (image_info->density != (char *) NULL)
    LiberateMemory((void **) &image_info->density);
  if (image_info->dispose != (char *) NULL)
    LiberateMemory((void **) &image_info->dispose);
  if (image_info->delay != (char *) NULL)
    LiberateMemory((void **) &image_info->delay);
  if (image_info->iterations != (char *) NULL)
    LiberateMemory((void **) &image_info->iterations);
  if (image_info->texture != (char *) NULL)
    LiberateMemory((void **) &image_info->texture);
  if (image_info->font != (char *) NULL)
    LiberateMemory((void **) &image_info->font);
  if (image_info->view != (char *) NULL)
    LiberateMemory((void **) &image_info->view);
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
%  Method DestroyImages deallocates memory associated with a linked list
%  of images.
%
%  The format of the DestroyImages method is:
%
%      void DestroyImages(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
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
%  Method DisplayImages displays one or more images to an X window.
%
%  The format of the AllocateNextImage method is:
%
%      unsigned int DisplayImages(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method DisplayImages returns True if the images are displayed
%      in an X window, otherwise False is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
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

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    return(False);
  XSetErrorHandler(XError);
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource);
  *resource.image_info=(*image_info);
  resource.immutable=True;
  if (image_info->delay != (char *) NULL)
    resource.delay=atoi(image_info->delay);
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) XDisplayImage(display,&resource,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  XCloseDisplay(display);
  return(image->exception.severity != UndefinedException);
}
#else
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (ImageInfo *) NULL);
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
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetImageInfo initializes the ImageInfo structure.
%
%  The format of the GetImageInfo method is:
%
%      void GetImageInfo(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
MagickExport void GetImageInfo(ImageInfo *image_info)
{
  register int
    i;

  /*
    File and image dimension members.
  */
  assert(image_info != (ImageInfo *) NULL);
  GetBlobInfo(&(image_info->blob));
  image_info->file=(FILE *) NULL;
  *image_info->filename='\0';
  *image_info->magick='\0';
  TemporaryFilename(image_info->unique);
  (void) strcat(image_info->unique,"u");
  TemporaryFilename(image_info->zero);
  image_info->temporary=False;
  image_info->adjoin=True;
  image_info->subimage=0;
  image_info->subrange=0;
  image_info->depth=QuantumDepth;
  image_info->size=(char *) NULL;
  image_info->tile=(char *) NULL;
  image_info->page=(char *) NULL;
  image_info->interlace=NoInterlace;
  image_info->units=UndefinedResolution;
  /*
    Compression members.
  */
  image_info->compression=UndefinedCompression;
  image_info->quality=75;
  /*
    Annotation members.
  */
  image_info->server_name=(char *) NULL;
  image_info->font=(char *) NULL;
  image_info->texture=(char *) NULL;
  image_info->density=(char *) NULL;
  image_info->antialias=True;
  image_info->pointsize=atof(DefaultPointSize);
  for (i=0; i < 6; i++)
    image_info->affine[i]=(i == 0) || (i == 3) ? 1.0 : 0.0;
  image_info->fuzz=0;
  (void) QueryColorDatabase("none",&image_info->stroke);
  (void) QueryColorDatabase("none",&image_info->fill);
  (void) QueryColorDatabase("#ffffff",&image_info->background_color);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color);
  /*
    Color reduction members.
  */
  image_info->dither=True;
  image_info->monochrome=False;
  image_info->colorspace=UndefinedColorspace;
  /*
    Animation members.
  */
  image_info->dispose=(char *) NULL;
  image_info->delay=(char *) NULL;
  image_info->iterations=(char *) NULL;
  image_info->decode_all_MNG_objects=False;
  image_info->coalesce_frames=False;
  image_info->insert_backdrops=False;
  /*
    Miscellaneous members.
  */
  image_info->verbose=False;
  image_info->preview_type=JPEGPreview;
  image_info->view=(char *) NULL;
  image_info->group=0L;
  image_info->ping=False;
  image_info->fifo=(int (*)(const Image *,const void *,const size_t)) NULL;
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
%  Method GetImageType returns the type of image (e.g. bilevel, palette, etc).
%
%  The format of the GetImageType method is:
%
%      ImageType GetImageType(Image *image)
%
%  A description of each parameter follows:
%
%    o type: Method GetImageType returns a ImageType enum that specifies the
%      type of the specified image (e.g. bilevel, palette, etc).
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport ImageType GetImageType(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->colorspace == CMYKColorspace)
    return(ColorSeparationType);
  if (IsMonochromeImage(image))
    return(BilevelType);
  if (IsGrayImage(image))
    return(GrayscaleType);
  if (IsPseudoClass(image) && image->matte)
    return(PaletteMatteType);
  if (IsPseudoClass(image))
    return(PaletteType);
  if (IsMatteImage(image))
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
%  Method GetNextImage returns the next image in an image sequence.
%
%  The format of the GetNextImage method is:
%
%      Image *GetNextImage(Image *image)
%
%  A description of each parameter follows:
%
%    o next: Method GetNextImage returns the next image in an image sequence.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport Image *GetNextImage(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->next->blob=image->blob;
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
%  Method GetNumberScenes returns the number of scenes in an image sequence.
%
%  The format of the GetNumberScenes method is:
%
%      unsigned int GetNumberScenes(const Image *image)
%
%  A description of each parameter follows:
%
%    o scenes:  Method GetNumberScenes returns the number of scenes in an
%      image sequence.
%
%    o image: The address of a structure of type Image.
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
%                                                                             %
%   G e t P a g e I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPageInfo initializes the image page structure.
%
%  The format of the GetPageInfo method is:
%
%      void GetPageInfo(RectangleInfo *page)
%
%  A description of each parameter follows:
%
%    o page: Specifies a pointer to a RectangleInfo structure.
%
%
*/
MagickExport void GetPageInfo(RectangleInfo *page)
{
  assert(page != (RectangleInfo *) NULL);
  page->width=0;
  page->height=0;
  page->x=0;
  page->y=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P i x e l P a c k e t                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelPacket initializes the PixelPacket structure.
%
%  The format of the GetPixelPacket method is:
%
%      void GetPixelPacket(PixelPacket *pixel)
%
%  A description of each parameter follows:
%
%    o pixel: Specifies a pointer to a PixelPacket structure.
%
%
*/
MagickExport void GetPixelPacket(PixelPacket *pixel)
{
  assert(pixel != (PixelPacket *) NULL);
  pixel->red=0;
  pixel->green=0;
  pixel->blue=0;
  pixel->opacity=TransparentOpacity;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G e o m e t r y                                                     %
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

  int
    x,
    y;

  unsigned int
    flags,
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=ParseGeometry((char *) geometry,&x,&y,&width,&height);
  return((flags != NoValue) || sscanf(geometry,"%lf",&value));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s S u b i m a g e                                                     %
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
  int
    x,
    y;

  unsigned int
    flags,
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
%     I s T a i n t e d                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTainted returns True if the image has been altered since it
%  was first read or if any image in the sequence has a difference magic or
%  filename.
%
%  The format of the IsTainted method is:
%
%      unsigned int IsTainted(const Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method IsTainted returns True if the image has been altered
%      since it was first read.
%
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport unsigned int IsTainted(const Image *image)
{
  char
    magick[MaxTextExtent],
    filename[MaxTextExtent];

  register const Image
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  (void) strcpy(magick,image->magick);
  (void) strcpy(filename,image->filename);
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
%     L a y e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LayerImage extracts the specified layer from the references image.
%
%  The format of the LayerImage method is:
%
%      unsigned int LayerImage(Image *image,const LayerType layer)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o layer: A value of type LayerType that identifies which layer to extract.
%
%
*/
MagickExport unsigned int LayerImage(Image *image,const LayerType layer)
{
#define LayerImageText  "  Extracting the layer from the image...  "

  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((layer == MatteLayer) && !image->matte)
    ThrowBinaryException(OptionWarning,"Unable to extract layer",
      "image does not have a matte layer");
  /*
    Layer DirectClass packets.
  */
  image->storage_class=DirectClass;
  image->matte=False;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      switch (layer)
      {
        case RedLayer:
        {
          q->green=q->red;
          q->blue=q->red;
          break;
        }
        case GreenLayer:
        {
          q->red=q->green;
          q->blue=q->green;
          break;
        }
        case BlueLayer:
        {
          q->red=q->blue;
          q->green=q->blue;
          break;
        }
        case MatteLayer:
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
      ProgressMonitor(LayerImageText,y,image->rows);
  }
  (void) IsGrayImage(image);
  return(True);
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
%  Method ListToGroupImage converts a linked list of images to a sequential
%  array.
%
%  The format of the ListToGroupImage method is:
%
%      Image **ListToGroupImage(Image *image,unsigned int *number_images)
%
%  A description of each parameter follows:
%
%    o images: Method ListToGroupImage converts a linked list of images to
%      a sequential array and returns the array..
%
%    o images: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o number_images:  A pointer to an unsigned integer.  The number of images
%      in the image array is returned here.
%
%
*/
MagickExport Image **ListToGroupImage(Image *image,unsigned int *number_images)
{
  Image
    **images,
    *next;

  register int
    i;

  /*
    Determine the number of images in the list.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(number_images != (unsigned int *) NULL);
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
%     M a t t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MatteImage initializes the matte channel of the reference image to
%  opaque.
%
%  The format of the MatteImage method is:
%
%      void MatteImage(Image *image,Quantum opacity)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o opacity: The level of transparency.
%
%
*/
MagickExport void MatteImage(Image *image,Quantum opacity)
{
  int
    y;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->storage_class=DirectClass;
  image->matte=True;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
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
%     M o g r i f y I m a g e                                                 %
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
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The address of a structure of type Image;  returned from
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

  Image
    *map_image,
    *region_image;

  ImageInfo
    *clone_info;

  int
    flags,
    x,
    y;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    region_info;

  register int
    i;

  unsigned int
    matte,
    height,
    width;

  /*
    Verify option length.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  for (i=1; i < argc; i++)
    if (Extent(argv[i]) > (MaxTextExtent/2-1))
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
    if (!IsMonochromeImage(*image))
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
    if ((Extent(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleNCompare("-affine",option,4) == 0)
      {
        char
          *p;

        /*
          Draw affine matrix.
        */
        p=argv[++i];
        for (x=0; x < 6; x++)
        {
          draw_info->affine[x]=strtod(p,&p);
          if (*p ==',')
            p++;
        }
        continue;
      }
    if (LocaleNCompare("antialias",option+1,3) == 0)
      {
        clone_info->antialias=(*option == '-');
        draw_info->antialias=(*option == '-');
        continue;
      }
    if (LocaleNCompare("-background",option,6) == 0)
      {
        (void) QueryColorDatabase(argv[++i],&clone_info->background_color);
        (*image)->background_color=clone_info->background_color;
        continue;
      }
    if (LocaleNCompare("-blur",option,4) == 0)
      {
        Image
          *blur_image;

        unsigned int
          order;

        /*
          Blur an image.
        */
        order=atoi(argv[++i]);
        blur_image=BlurImage(*image,order,&(*image)->exception);
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
        border_image->storage_class=DirectClass;
        *image=border_image;
        continue;
      }
    if (LocaleNCompare("-bordercolor",option,8) == 0)
      {
        (void) QueryColorDatabase(argv[++i],&clone_info->border_color);
        (*image)->border_color=clone_info->border_color;
        continue;
      }
    if (LocaleCompare("-box",option) == 0)
      {
        (void) QueryColorDatabase(argv[++i],&draw_info->box);
        continue;
      }
    if (LocaleNCompare("-charcoal",option,3) == 0)
      {
        char
          *commands[7];

        QuantizeInfo
          quantize_info;

        /*
          Charcoal drawing.
        */
        i++;
        GetQuantizeInfo(&quantize_info);
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,*image);
        commands[0]=SetClientName((char *) NULL);
        commands[1]="-edge";
        commands[2]=argv[i];
        commands[3]="-blur";
        commands[4]=argv[i];
        commands[5]="-normalize";
        commands[6]="-negate";
        MogrifyImage(clone_info,7,commands,image);
        (void) QuantizeImage(&quantize_info,*image);
        continue;
      }
    if (LocaleNCompare("-colorize",option,8) == 0)
      {
        Image
          *colorize_image;

        /*
          Colorize the image.
        */
        colorize_image=
          ColorizeImage(*image,argv[++i],clone_info->fill,&(*image)->exception);
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
            if (LocaleCompare("LZW",option) == 0)
              compression=LZWCompression;
            if (LocaleCompare("RunlengthEncoded",option) == 0)
              compression=RunlengthEncodedCompression;
            if (LocaleCompare("Zip",option) == 0)
              compression=ZipCompression;
            (*image)->compression=compression;
          }
        continue;
      }
    if (LocaleCompare("-colors",option) == 0)
      {
        quantize_info.number_colors=atoi(argv[++i]);
        continue;
      }
    if (LocaleNCompare("-colorspace",option,8) == 0)
      {
        option=argv[++i];
        if (LocaleCompare("cmyk",option) == 0)
          {
            RGBTransformImage(*image,CMYKColorspace);
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
            TransformRGBImage(*image,RGBColorspace);
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
        continue;
      }
    if (LocaleNCompare("comment",option+1,4) == 0)
      {
        if (*option == '-')
          (void) SetImageAttribute(*image,"Comment",argv[++i]);
        else
          (void) SetImageAttribute(*image,"Comment",(char *) NULL);
        continue;
      }
    if (LocaleNCompare("contrast",option+1,3) == 0)
      {
        ContrastImage(*image,(unsigned int) (*option == '-'));
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
    if (LocaleNCompare("-density",option,4) == 0)
      {
        int
          count;

        /*
          Set image density.
        */
        (void) CloneString(&clone_info->density,argv[++i]);
        count=sscanf(clone_info->density,"%lfx%lf",
          &(*image)->x_resolution,&(*image)->y_resolution);
        if (count != 2)
          (*image)->y_resolution=(*image)->x_resolution;
      }
    if (LocaleNCompare("-depth",option,4) == 0)
      {
        (*image)->depth=atoi(argv[++i]) <= 8 ? 8 : 16;
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
        DrawImage(*image,draw_info);
        continue;
      }
    if (LocaleNCompare("-edge",option,3) == 0)
      {
        Image
          *edge_image;

        unsigned int
          order;

        /*
          Detect edges in the image.
        */
        order=atoi(argv[++i]);
        edge_image=EdgeImage(*image,order,&(*image)->exception);
        if (edge_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=edge_image;
        continue;
      }
    if (LocaleNCompare("-emboss",option,3) == 0)
      {
        Image
          *emboss_image;

        unsigned int
          order;

        /*
          Emboss the image.
        */
        order=atoi(argv[++i]);
        emboss_image=EmbossImage(*image,order,&(*image)->exception);
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
        EqualizeImage(*image);
        continue;
      }
    if (LocaleCompare("-fill",option) == 0)
      {
        (void) QueryColorDatabase(argv[++i],&clone_info->fill);
        draw_info->fill=clone_info->fill;
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
        frame_image->storage_class=DirectClass;
        *image=frame_image;
        continue;
      }
    if (LocaleNCompare("-fuzz",option,3) == 0)
      {
        (*image)->fuzz=atoi(argv[++i]);
        continue;
      }
    if (LocaleCompare("-font",option) == 0)
      {
        i++;
        (void) CloneString(&clone_info->font,argv[i]);
        (void) CloneString(&draw_info->font,argv[i]);
        continue;
      }
    if (LocaleNCompare("gamma",option+1,3) == 0)
      {
        if (*option == '+')
          (*image)->gamma=atof(argv[++i]);
        else
          GammaImage(*image,argv[++i]);
        continue;
      }
    if (LocaleCompare("gaussian",option+1) == 0)
      {
        double
          sigma,
          width;

        Image
          *blur_image;

        /*
          Gaussian blur image.
        */
        width=3.0;
        sigma=1.0;
        if (*option == '-')
          (void) sscanf(argv[++i],"%lfx%lf",&width,&sigma);
        blur_image=GaussianBlurImage(*image,width,sigma,&(*image)->exception);
        if (blur_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=blur_image;
        continue;
      }
    if (LocaleNCompare("-geometry",option,4) == 0)
      {
        Image
          *zoom_image;

        /*
          Resize image.
        */
        width=(*image)->columns;
        height=(*image)->rows;
        x=0;
        y=0;
        (void) CloneString(&geometry,argv[++i]);
        (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
        zoom_image=ZoomImage(*image,width,height,&(*image)->exception);
        if (zoom_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=zoom_image;
        continue;
      }
    if (LocaleNCompare("gravity",option+1,2) == 0)
      {
        draw_info->gravity=NorthWestGravity;
        if (*option == '-')
          {
            option=argv[++i];
            if (LocaleCompare("NorthWest",option) == 0)
              draw_info->gravity=NorthWestGravity;
            if (LocaleCompare("North",option) == 0)
              draw_info->gravity=NorthGravity;
            if (LocaleCompare("NorthEast",option) == 0)
              draw_info->gravity=NorthEastGravity;
            if (LocaleCompare("West",option) == 0)
              draw_info->gravity=WestGravity;
            if (LocaleCompare("Center",option) == 0)
              draw_info->gravity=CenterGravity;
            if (LocaleCompare("East",option) == 0)
              draw_info->gravity=EastGravity;
            if (LocaleCompare("SouthWest",option) == 0)
              draw_info->gravity=SouthWestGravity;
            if (LocaleCompare("South",option) == 0)
              draw_info->gravity=SouthGravity;
            if (LocaleCompare("SouthEast",option) == 0)
              draw_info->gravity=SouthEastGravity;
          }
        continue;
      }
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
    if (LocaleNCompare("label",option+1,3) == 0)
      {
        if (*option == '-')
          (void) SetImageAttribute(*image,"Label",argv[++i]);
        else
          (void) SetImageAttribute(*image,"Label",(char *) NULL);
        continue;
      }
    if (LocaleNCompare("layer",option+1,3) == 0)
      {
        LayerType
          layer;

        layer=UndefinedLayer;
        if (*option == '-')
          {
            option=argv[++i];
            if (LocaleCompare("Red",option) == 0)
              layer=RedLayer;
            if (LocaleCompare("Green",option) == 0)
              layer=GreenLayer;
            if (LocaleCompare("Blue",option) == 0)
              layer=BlueLayer;
            if (LocaleCompare("Matte",option) == 0)
              layer=MatteLayer;
          }
        LayerImage(*image,layer);
        continue;
      }
    if (LocaleNCompare("-linewidth",option,3) == 0)
      {
        draw_info->linewidth=atof(argv[++i]);
        continue;
      }
    if (LocaleCompare("-map",option) == 0)
      {
        /*
          Transform image colors to match this set of colors.
        */
        (void) strcpy(clone_info->filename,argv[++i]);
        map_image=ReadImage(clone_info,&(*image)->exception);
        continue;
      }
    if (LocaleCompare("matte",option+1) == 0)
      {
        if (*option == '-')
          if (!(*image)->matte)
            MatteImage(*image,OpaqueOpacity);
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
          MedianFilterImage(*image,atoi(argv[++i]),&(*image)->exception);
        if (median_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=median_image;
        continue;
      }
    if (LocaleNCompare("-modulate",option,4) == 0)
      {
        ModulateImage(*image,argv[++i]);
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
    if (LocaleNCompare("negate",option+1,3) == 0)
      {
        NegateImage(*image,*option == '+');
        continue;
      }
    if (LocaleNCompare("noise",option+1,4) == 0)
      {
        Image
          *noisy_image;

        option=argv[++i];
        if (*option == '-')
          noisy_image=
            ReduceNoiseImage(*image,atoi(option),&(*image)->exception);
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
            noisy_image=AddNoiseImage(*image,noise_type,&(*image)->exception);
          }
        if (noisy_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=noisy_image;
        continue;
      }
    if (LocaleNCompare("-normalize",option,4) == 0)
      {
        NormalizeImage(*image);
        continue;
      }
    if (LocaleNCompare("-opaque",option,3) == 0)
      {
        PixelPacket
          target;

        target=GetOnePixel(*image,0,0);
        (void) QueryColorDatabase(argv[++i],&target);
        OpaqueImage(*image,target,draw_info->fill);
        continue;
      }
    if (LocaleNCompare("-paint",option,4) == 0)
      {
        Image
          *paint_image;

        /*
          Oil paint image.
        */
        paint_image=OilPaintImage(*image,atoi(argv[++i]),&(*image)->exception);
        if (paint_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=paint_image;
        continue;
      }
    if (LocaleCompare("-pen",option) == 0)  /* anachronism */
      {
        (void) QueryColorDatabase(argv[++i],&clone_info->stroke);
        draw_info->stroke=clone_info->stroke;
        clone_info->fill=clone_info->stroke;
        draw_info->fill=clone_info->fill;
        continue;
      }
    if (LocaleNCompare("pointsize",option+1,2) == 0)
      {
        clone_info->pointsize=atof(argv[++i]);
        continue;
      }
#if defined(HasLTDL) || defined(_MAGICKMOD_)
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

        unsigned int
          length;

        TokenInfo
          token_info;

        length=Extent(argv[++i]);
        token=(char *) AcquireMemory(length+1);
        if (token == (char *) NULL)
          continue;
        next=0;
        arguments=argv[i];
        status=Tokenizer(&token_info,0,token,length,arguments,"","=","\"",0,
          &breaker,&next,&quote);
        if (status == 0)
          {
            char
              *argv;

            argv=&(arguments[next]);
            ExecuteModuleProcess((const char *) token,*image,1,&argv);
          }
        LiberateMemory((void **) &token);
        continue;
      }
#endif
    if (LocaleNCompare("profile",option+1,4) == 0)
      {
        if (*option == '+')
          {
            /*
              Remove a ICC or IPTC profile from the image.
            */
            option=argv[++i];
            if (LocaleCompare("icm",option) == 0)
              ProfileImage(*image,ICMProfile,(const char *) NULL);
            if (LocaleCompare("iptc",option) == 0)
              ProfileImage(*image,IPTCProfile,(const char *) NULL);
            if (LocaleCompare("8bim",option) == 0)
              ProfileImage(*image,IPTCProfile,(const char *) NULL);
            continue;
          }
        /*
          Add a ICC or IPTC profile to the image.
        */
        ProfileImage(*image,UndefinedProfile,argv[++i]);
        continue;
      }
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
        RaiseImage(*image,&raise_info,*option == '-');
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
            matte=region_image->matte;
            CompositeImage(region_image,
              (*image)->matte ? OverCompositeOp : ReplaceCompositeOp,*image,
              region_info.x,region_info.y);
            DestroyImage(*image);
            *image=region_image;
            (*image)->matte=matte;
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
          width=(unsigned int) ((int) (*image)->columns-region_info.x);
        if ((flags & HeightValue) == 0)
          height=(unsigned int) ((int) (*image)->rows-region_info.y);
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
        (*image)->scene=atoi(argv[++i]);
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
        (void) SegmentImage(*image,quantize_info.colorspace,clone_info->verbose,
          cluster_threshold,smoothing_threshold);
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
    if (LocaleNCompare("-sharpen",option,5) == 0)
      {
        Image
          *sharpen_image;

        unsigned int
          order;

        /*
          Sharpen an image.
        */
        order=atoi(argv[++i]);
        sharpen_image=SharpenImage(*image,order,&(*image)->exception);
        if (sharpen_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=sharpen_image;
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
        shear_image->storage_class=DirectClass;
        *image=shear_image;
        continue;
      }
    if (LocaleNCompare("-solarize",option,3) == 0)
      {
        SolarizeImage(*image,atof(argv[++i]));
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
        (void) QueryColorDatabase(argv[++i],&clone_info->stroke);
        draw_info->stroke=clone_info->stroke;
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
    if (LocaleNCompare("-threshold",option,3) == 0)
      {
        double
          threshold;

        /*
          Threshold image.
        */
        threshold=atof(argv[++i]);
        ThresholdImage(*image,threshold);
        continue;
      }
    if (LocaleNCompare("-transparent",option,8) == 0)
      {
        PixelPacket
          target;

        target=GetOnePixel(*image,0,0);
        (void) QueryColorDatabase(argv[++i],&target);
        TransparentImage(*image,target);
        continue;
      }
    if (LocaleNCompare("-treedepth",option,4) == 0)
      {
        quantize_info.tree_depth=atoi(argv[++i]);
        continue;
      }
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
    if (LocaleNCompare("verbose",option+1,3) == 0)
      {
        clone_info->verbose=(*option == '-');
        quantize_info.measure_error=(*option == '-');
        continue;
      }
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
        wave_image=WaveImage(*image,amplitude,wavelength,&(*image)->exception);
        if (wave_image == (Image *) NULL)
          break;
        DestroyImage(*image);
        *image=wave_image;
        continue;
      }
  }
  if (region_image != (Image *) NULL)
    {
      /*
        Composite transformed region onto image.
      */
      matte=region_image->matte;
      CompositeImage(region_image,
        (*image)->matte ? OverCompositeOp : ReplaceCompositeOp,*image,
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
  CloseImagePixels(*image);
  return((*image)->exception.severity == UndefinedException);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o g r i f y I m a g e s                                               %
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
%      unsigned int MogrifyImages(const ImageInfo *next_info,const int argc,
%        char **argv,Image **image)
%
%  A description of each parameter follows:
%
%    o next_info: Specifies a pointer to an ImageInfo structure.
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImages(const ImageInfo *next_info,
  const int argc,char **argv,Image **image)
{
#define MogrifyImageText  "  Transforming image...  "

  Image
    *mogrify_image;

  register int
    i;

  MonitorHandler
    handler;

  register Image
    *next;

  unsigned int
    number_images,
    status;

  assert(next_info != (ImageInfo *) NULL);
  assert(next_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  next=(*image);
  for (number_images=1; next->next != (Image *) NULL; number_images++)
    next=next->next;
  ProgressMonitor(MogrifyImageText,0,number_images);
  handler=SetMonitorHandler((MonitorHandler) NULL);
  status=MogrifyImage(next_info,argc,argv,image);
  (void) SetMonitorHandler(handler);
  if (status == False)
    return(False);
  next=(*image);
  mogrify_image=(*image)->next;
  if (next_info->verbose)
    DescribeImage(next,stdout,False);
  for (i=1; mogrify_image != (Image *) NULL; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    status=MogrifyImage(next_info,argc,argv,&mogrify_image);
    if (status == False)
      break;
    next->next=mogrify_image;
    next->next->previous=next;
    next=next->next;
    if (next_info->verbose)
      DescribeImage(mogrify_image,stdout,False);
    mogrify_image=mogrify_image->next;
    (void) SetMonitorHandler(handler);
    ProgressMonitor(MogrifyImageText,i,number_images);
  }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o s a i c I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MosaicImages inlays a number of image to form a single coherent
%  picture.
%
%  The format of the MosaicImage method is:
%
%      Image *MosaicImages(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport Image *MosaicImages(Image *image,ExceptionInfo *exception)
{
#define MosaicImageText  "  Creating an image mosaic...  "

  Image
    *mosaic_image;

  int
    y;

  RectangleInfo
    page;

  register Image
    *next;

  register int
    x;

  register PixelPacket
    *q;

  unsigned int
    scene;

  /*
    Determine next bounding box.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException(OptionWarning,"Unable to create image mosaic",
      "image sequence required");
  page.width=image->columns;
  page.height=image->rows;
  page.x=0;
  page.y=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    page.x=next->page.x;
    page.y=next->page.y;
    if ((next->columns+page.x) > page.width)
      page.width=next->columns+page.x;
    if ((next->rows+page.y) > page.height)
      page.height=next->rows+page.y;
  }
  /*
    Allocate next structure.
  */
  mosaic_image=AllocateImage((ImageInfo *) NULL);
  if (mosaic_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Initialize colormap.
  */
  mosaic_image->columns=page.width;
  mosaic_image->rows=page.height;
  for (y=0; y < (int) mosaic_image->rows; y++)
  {
    q=SetImagePixels(mosaic_image,0,y,mosaic_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) mosaic_image->columns; x++)
    {
      *q=mosaic_image->background_color;
      q++;
    }
    if (!SyncImagePixels(mosaic_image))
      break;
  }
  scene=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    CompositeImage(mosaic_image,ReplaceCompositeOp,next,next->page.x,
      next->page.y);
    ProgressMonitor(MosaicImageText,scene++,GetNumberScenes(image));
  }
  return(mosaic_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e I m a g e G e o m e t r y                                       %
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
%      int ParseImageGeometry(const char *geometry,int *x,int *y,
%        unsigned int *width,unsigned int *height)
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
MagickExport int ParseImageGeometry(const char *geometry,int *x,int *y,
  unsigned int *width,unsigned int *height)
{
  int
    delta,
    flags;

  RectangleInfo
    media_info;

  unsigned int
    former_height,
    former_width;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (int *) NULL);
  assert(y != (int *) NULL);
  assert(width != (unsigned int *) NULL);
  assert(height != (unsigned int *) NULL);
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
      int
        count;

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
      *width=Max(scale_factor*former_width,1);
      *height=Max(scale_factor*former_height,1);
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
          if ((int) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((int) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
      if ((*height+((*y) << 1)) > media_info.height)
        {
          if ((int) *width > ((*x) << 1))
            *width-=(*x) << 1;
          if ((int) *height > ((*y) << 1))
            *height-=(*y) << 1;
        }
    }
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R G B T r a n s f o r m I m a g e                                       %
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
%    o image: The address of a structure of type Image;  returned from
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
#define RGBTransformImageText  "  Transforming image colors...  "
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

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *p,
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace))
    return(True);
  if (colorspace == CMYKColorspace)
    {
      Quantum
        black,
        cyan,
        magenta,
        yellow;

      /*
        Convert RGB to CMYK colorspace.
      */
      image->colorspace=CMYKColorspace;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          cyan=MaxRGB-q->red;
          magenta=MaxRGB-q->green;
          yellow=MaxRGB-q->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          q->red=cyan;
          q->green=magenta;
          q->blue=yellow;
          q->opacity=black;
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
      for (y=0; y < (int) image->rows; y++)
      {
        p=GetImagePixels(image,0,y,image->columns,1);
        if (p == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          if ((p->red != p->green) || (p->green != p->blue))
            break;
          p++;
        }
        if (x < (int) image->columns)
          break;
      }
      if ((x == (int) image->columns) && (y == (int) image->rows))
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
          Cr=  0.500000*R-0.418688*G-0.083168*B

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
        z_map[i+Z]=(-0.081312)*i;
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          red=x_map[q->red+X]+y_map[q->green+X]+z_map[q->blue+X]+tx;
          green=x_map[q->red+Y]+y_map[q->green+Y]+z_map[q->blue+Y]+ty;
          blue=x_map[q->red+Z]+y_map[q->green+Z]+z_map[q->blue+Z]+tz;
          q->red=(red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
          q->green=(green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
          q->blue=(blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(RGBTransformImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
      {
        red=x_map[image->colormap[i].red+X]+y_map[image->colormap[i].green+X]+
          z_map[image->colormap[i].blue+X]+tx;
        green=x_map[image->colormap[i].red+Y]+y_map[image->colormap[i].green+Y]+
          z_map[image->colormap[i].blue+Y]+ty;
        blue=x_map[image->colormap[i].red+Z]+y_map[image->colormap[i].green+Z]+
          z_map[image->colormap[i].blue+Z]+tz;
        image->colormap[i].red=
          (red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
        image->colormap[i].green=
          (green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
        image->colormap[i].blue=
          (blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
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
%  Method SetImage initializes the reference image to the background color.
%
%  The format of the SetImage method is:
%
%      void SetImage(Image *image,opacity)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o opacity: The transparency of the background color.
%
%
*/
MagickExport void SetImage(Image *image,Quantum opacity)
{
  int
    y;

  PixelPacket
    background_color;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  background_color=image->background_color;
  background_color.opacity=opacity;
  if (opacity != OpaqueOpacity)
    image->matte=True;
  for (y=0; y < (int) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
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
%   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetImageInfo initializes the `magick' field of the ImageInfo
%  structure.  It is set to a type of image format based on the prefix or
%  suffix of the filename.  For example, `ps:image' returns PS indicating
%  a Postscript image.  JPEG is returned for this filename: `image.jpg'.
%  The filename prefix has precendence over the suffix.  Use an optional index
%  enclosed in brackets after a file name to specify a desired subimage of a
%  multi-resolution image format like Photo CD (e.g. img0001.pcd[4]).
%
%  The format of the SetImageInfo method is:
%
%      unsigned int SetImageInfo(ImageInfo *image_info,
%        const unsigned int rectify)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%
*/
MagickExport unsigned int SetImageInfo(ImageInfo *image_info,
  const unsigned int rectify)
{
  char
    magic[MaxTextExtent];

  Image
    *image;

  register char
    *p,
    *q;

  unsigned char
    magick[MaxTextExtent];

  unsigned int
    affirm,
    status;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  *magic='\0';
  p=image_info->filename+Max(Extent(image_info->filename)-1,0);
  if (*p == ']')
    for (q=p-1; q > image_info->filename; q--)
    {
      char
        *tile;

      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      if (*q != '[')
        continue;
      if (!IsGeometry(q+1))
        break;
      tile=(char *) AcquireMemory(p-q);
      if (tile == (char *) NULL)
        break;
      (void) strncpy(tile,q+1,p-q-1);
      tile[p-q-1]='\0';
      *q='\0';
      p=q;
      (void) CloneString(&image_info->tile,tile);
      LiberateMemory((void **) &tile);
      if (!IsSubimage(image_info->tile,True))
        break;
      /*
        Determine sub-image range.
      */
      image_info->subimage=atoi(image_info->tile);
      image_info->subrange=atoi(image_info->tile);
      (void) sscanf(image_info->tile,"%u-%u",&image_info->subimage,
        &image_info->subrange);
      if (image_info->subrange < image_info->subimage)
        Swap(image_info->subimage,image_info->subrange);
      else
        {
          LiberateMemory((void **) &image_info->tile);
          image_info->tile=(char *) NULL;
        }
      image_info->subrange-=image_info->subimage-1;
      break;
    }
  while ((*p != '.') && (p > (image_info->filename+1)))
    p--;
  if ((LocaleCompare(p,".gz") == 0) || (LocaleCompare(p,".Z") == 0) ||
      (LocaleCompare(p,".bz2") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > (image_info->filename+1)));
  if ((*p == '.') && (Extent(p) < (int) sizeof(magic)))
    {
      /*
        User specified image format.
      */
      (void) strcpy(magic,p+1);
      for (q=magic; *q != '\0'; q++)
        if (*q == '.')
          {
            *q='\0';
            break;
          }
      LocaleUpper(magic);
      /*
        SGI and RGB are ambiguous;  TMP must be set explicitly.
      */
      if (((LocaleNCompare(image_info->magick,"SGI",3) != 0) ||
          (LocaleCompare(magic,"RGB") != 0)) &&
          (LocaleCompare(magic,"TMP") != 0))
        (void) strcpy(image_info->magick,magic);
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  affirm=False;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
#if defined(vms)
  if (*(p+1) == '[')
    p++;
#endif
  if ((*p == ':') && ((p-image_info->filename) < (int) sizeof(magic)))
    {
      /*
        User specified image format.
      */
      (void) strncpy(magic,image_info->filename,p-image_info->filename);
      magic[p-image_info->filename]='\0';
      LocaleUpper(magic);
#if defined(macintosh) || defined(WIN32)
      if (!ImageFormatConflict(magic))
#endif
        {
          /*
            Strip off image format prefix.
          */
          p++;
          (void) strcpy(image_info->filename,p);
          if (LocaleCompare(magic,"IMPLICIT") != 0)
            {
              (void) strcpy(image_info->magick,magic);
              if (LocaleCompare(magic,"TMP") != 0)
                affirm=True;
              else
                image_info->temporary=True;
            }
        }
    }
  if (rectify)
    {
      char
        filename[MaxTextExtent];

      MagickInfo
        *magick_info;

      /*
        Rectify multi-image file support.
      */
      FormatString(filename,image_info->filename,0);
      if ((LocaleCompare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=False;
      magick_info=(MagickInfo *) GetMagickInfo(magic);
      if (magick_info != (MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return(True);
    }
  if (affirm)
    return(True);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return(True);
  /*
    Determine the image format from the first few bytes of the file.
  */
  (void) strcpy(image->filename,image_info->filename);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    {
      DestroyImage(image);
      return(True);
    }
  if ((image->blob.data != (char *) NULL)  || !image->exempt)
    (void) ReadBlob(image,MaxTextExtent,magick);
  else
    {
      FILE
        *file;

      register int
        c,
        i;

      /*
        Copy standard input or pipe to temporary file.
      */
      image_info->file=(FILE *) NULL;
      TemporaryFilename(image->filename);
      image_info->temporary=True;
      FormatString(image_info->filename,"%.1024s",image->filename);
      file=fopen(image->filename,WriteBinaryType);
      if (file == (FILE *) NULL)
        ThrowBinaryException(MissingDelegateWarning,"Unable to write file",
          image->filename);
      i=0;
      for (c=fgetc(image->file); c != EOF; c=fgetc(image->file))
      {
        if (i < MaxTextExtent)
          magick[i++]=(unsigned char) c;
        (void) fputc(c,file);
      }
      (void) fclose(file);
    }
  DestroyImage(image);
  if (SetImageMagic(magick,MaxTextExtent,magic) == True)
    (void) strcpy(image_info->magick,magic);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SortColormapByIntensity sorts the colormap of a PseudoClass image
%  by decreasing color intensity.
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

static int IntensityCompare(const void *x,const void *y)
{
  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  return((int) Intensity(*color_2)-(int) Intensity(*color_1));
}

MagickExport unsigned int SortColormapByIntensity(Image *image)
{
  IndexPacket
    index;

  int
    y;

  register int
    i,
    x;

  register IndexPacket
    *indexes;

  register PixelPacket
    *q;

  unsigned short
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(True);
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned short *)
    AcquireMemory(image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    ThrowBinaryException(MissingDelegateWarning,"Unable to sort colormap",
      "Memory allocation failed");
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (int) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing color popularity.
  */
  qsort((void *) image->colormap,(int) image->colors,sizeof(PixelPacket),
    (int (*)(const void *, const void *)) IntensityCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < (int) image->colors; i++)
    pixels[image->colormap[i].opacity]=(unsigned short) i;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
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
%   S y n c I m a g e                                                         %
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
%    o image: The address of a structure of type Image.
%
%
*/
MagickExport void SyncImage(Image *image)
{
  IndexPacket
    index;

  int
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    return;
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (int) image->columns; x++)
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
%  Method TextureImage layers a texture onto the background of an image.
%
%  The format of the TextureImage method is:
%
%      void TextureImage(Image *image,Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o texture: This image contains the texture to layer on the background.
%
%
*/
MagickExport void TextureImage(Image *image,Image *texture)
{
#define TextureImageText  "  Appling image texture...  "

  int
    x,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (texture == (const Image *) NULL)
    return;
  /*
    Tile texture onto the image background.
  */
  for (y=0; y < (int) image->rows; y+=texture->rows)
  {
    for (x=0; x < (int) image->columns; x+=texture->columns)
      CompositeImage(image,ReplaceCompositeOp,texture,x,y);
    if (QuantumTick(y,image->rows))
      ProgressMonitor(TextureImageText,y,image->rows);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s f o r m R G B I m a g e                                       %
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
%    o image: The address of a structure of type Image;  returned from
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
#define TransformRGBImageText  "  Transforming image colors...  "

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
    blue,
    *blue_map,
    green,
    *green_map,
    red,
    *red_map;

  int
    y;

  register int
    i,
    x;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if ((image->colorspace == CMYKColorspace) && (colorspace == RGBColorspace))
    {
      unsigned int
        black,
        cyan,
        magenta,
        yellow;

      /*
        Transform image from CMYK to RGB.
      */
      image->colorspace=RGBColorspace;
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
        {
          cyan=q->red;
          magenta=q->green;
          yellow=q->blue;
          black=q->opacity;
          if ((cyan+black) > MaxRGB)
            q->red=0;
          else
            q->red=MaxRGB-(cyan+black);
          if ((magenta+black) > MaxRGB)
            q->green=0;
          else
            q->green=MaxRGB-(magenta+black);
          if ((yellow+black) > MaxRGB)
            q->blue=0;
          else
            q->blue=MaxRGB-(yellow+black);
          q->opacity=0;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
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
      for (y=0; y < (int) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (int) image->columns; x++)
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
              q->red=YCCMap[(int) DownScale(red)];
              q->green=YCCMap[(int) DownScale(green)];
              q->blue=YCCMap[(int) DownScale(blue)];
              break;
            }
            default:
            {
              q->red=(red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
              q->green=(green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
              q->blue=(blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
              break;
            }
          }
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          ProgressMonitor(TransformRGBImageText,y,image->rows);
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < (int) image->colors; i++)
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
            image->colormap[i].red=
              (red < 0) ? 0 : (red > MaxRGB) ? MaxRGB : red+0.5;
            image->colormap[i].green=
              (green < 0) ? 0 : (green > MaxRGB) ? MaxRGB : green+0.5;
            image->colormap[i].blue=
              (blue < 0) ? 0 : (blue > MaxRGB) ? MaxRGB : blue+0.5;
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s m i t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransmitImage transmit an image to a variety of destinations. It
%  is used primarily in CGI and ISAPI programs to send an image to a client
%  using either disk, blobs, streaming, or via the normal in memory image
%  structure.
%
%  The format of the TransmitImage method is:
%
%      unsigned int TransmitImage(Image *image,ImageInfo *image_info,
%        const TransmitType sendmode, void *param1, void *param2)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image; returned from
%      ReadImage.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o sendmode: An unsigned integer value defines which transmit mode
%      to send the image with.
%
%    o param1: An void pointer whose meaning depends on the transmit mode.
%
%    o param2: An void pointer whose meaning depends on the transmit mode.
%
%
*/
MagickExport unsigned int TransmitImage(Image *image,ImageInfo *image_info,
  const TransmitType mode,void *param1,void *param2)
{
  unsigned int
    status;

  status=True;
  switch (mode)
  {
    case BlobTransmitType:
    {
      char
        **blob_data;

      ExceptionInfo
        exception;

      size_t
        *blob_length;

      blob_data=(char **) param1;
      blob_length=(size_t *) param2;
      (void) strcpy(image->magick,image_info->magick);
      if (*blob_length == 0)
        *blob_length=8192;
      *blob_data=ImageToBlob(image_info,image,blob_length,&exception);
      if (*blob_data == NULL)
        status=False;
      break;
    }
    case StreamTransmitType:
    {
      int
        (*fifo)(const Image *,const void *,const size_t);

      fifo=(int (*)(const Image *,const void *,const size_t)) param1;
      image->client_data=param2;
      status=WriteStream(image_info,image,fifo);
      break;
    }
    case ImageTransmitType:
    {
      Image
        **image_ptr;

      ImageInfo
        **image_info_ptr;

      image_ptr=(Image **) param1;
      image_info_ptr=(ImageInfo **) param2;
      *image_ptr=image;
      *image_info_ptr=image_info;
      break;
    }
    case FileTransmitType:
    default:
    {
      status=WriteImage(image_info,image);
      break;
    }
  }
  return(status);
}
