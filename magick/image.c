/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2003 ImageMagick Studio
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
%                     IIIII  M   M   AAA    GGGG  EEEEE                       %
%                       I    MM MM  A   A  G      E                           %
%                       I    M M M  AAAAA  G  GG  EEE                         %
%                       I    M   M  A   A  G   G  E                           %
%                     IIIII  M   M  A   A   GGGG  EEEEE                       %
%                                                                             %
%                                                                             %
%                        GraphicsMagick Image Methods                         %
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
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/composite.h"
#include "magick/compress.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/map.h"
#include "magick/monitor.h"
#include "magick/module.h"
#include "magick/pixel_iterator.h"
#include "magick/quantize.h"
#include "magick/render.h"
#include "magick/semaphore.h"
#include "magick/signature.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/xwindow.h"

/*
  Constant declaration.
*/
const char
  *BackgroundColor = "#ffffffffffff",  /* white */
  *BorderColor = "#dfdfdfdfdfdf",  /* gray */
  *DefaultTileFrame = "15x15+3+3",
  *DefaultTileGeometry = "120x120+4+3>",
  *DefaultTileLabel = "%f\n%wx%h\n%b",
  *ForegroundColor = "#000000000000",  /* black */
  *LoadImageText = "  Loading image...  ",
  *LoadImagesText = "  Loading images...  ",
  *MatteColor = "#bdbdbdbdbdbd",  /* gray */
  *PSDensityGeometry = "72.0x72.0",
  *PSPageGeometry = "612x792>",
  *SaveImageText = "  Saving image...  ",
  *SaveImagesText = "  Saving images...  ";

const unsigned long
  DefaultCompressionQuality = 75;

/* Round floating value to an integer */
#define RndToInt(value) ((int)((value)+0.5))

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   A c c e s s D e f i n i t i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AccessDefinition() searches the definitions for an entry matching the
%  specified magick and key. NULL is returned if no matching entry is found.
%
%  The format of the AccessDefinition method is:
%
%      const char *AccessDefinition(const ImageInfo *image_info,
%                                    const char *magick, const char *key)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o magick: Format ID. This is usually the same as the coder name.
%
%    o key: Key to search for.
%
*/
MagickExport const char *AccessDefinition(const ImageInfo *image_info,
  const char *magick, const char *key)
{
  const char
    *value = 0;

  char
    search_key[MaxTextExtent];

  if (image_info->definitions)
    {
      FormatString(search_key, "%.60s:%.1024s", magick, key);
      value=MagickMapAccessEntry(image_info->definitions,search_key,0);
    }
  return value;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   A d d D e f i n i t i o n s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddDefinitions() adds definitions from a key/value based string to the current
%  map of definitions in ImageInfo. Definitions may be used by coders/decoders
%  that read and write images.
%
%  The format of the AddDefinitions method is:
%
%      void AddDefinitions(ImageInfo *image_info,const char *options)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o options: List of key/value pairs to put in the definitions map. The
%      format of the string is "key1[=[value1]],key2[=[value2]],...". A missing
%      value argument (with or without the equal sign) inserts an empty, zero
%      length string as value for a key.
%
%    o exception: Errors result in updates to this structure.
%
*/
MagickExport unsigned int
AddDefinitions(ImageInfo *image_info,const char *definitions,
  ExceptionInfo *exception)
{
  char
    key[MaxTextExtent],
    value[MaxTextExtent];

  unsigned int
    status;

  int
    i,
    j;

  size_t
    length;

  status=True;

  if (image_info->definitions == 0)
    image_info->definitions=MagickMapAllocateMap(MagickMapCopyString,
      MagickMapDeallocateString);

  length=strlen(definitions);
  i=0;
  while (i < length)
  {
    unsigned int
      has_value;

    for (j=0; (i < length) && (definitions[i] != '=') && (definitions[i] != ','); i++,j++)
      key[j]=definitions[i];
    key[j]='\0';
    has_value=(i < length) && (definitions[i] == '='); /* Could be 0-length value */
    i++;

    j=0;
    if (has_value)
      {
        for (; (i < length) && (definitions[i] != ','); i++,j++)
          value[j]=definitions[i];
        i++;
      }
    value[j]='\0';
    if (strlen(key) > 0)
      {
        status &= MagickMapAddEntry(image_info->definitions,key,value,0,exception);
      }
    else
      {
        status=False;
        break;
      }
  }
  return(status);
}

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

  /*
    Allocate image structure.
  */
  allocate_image=MagickAllocateMemory(Image *,sizeof(Image));
  if (allocate_image == (Image *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImage);
  (void) memset(allocate_image,0,sizeof(Image));
  /*
    Initialize Image structure.
  */
  (void) strcpy(allocate_image->magick,"MIFF");
  allocate_image->storage_class=DirectClass;
  allocate_image->depth=QuantumDepth;
  allocate_image->interlace=NoInterlace;
  allocate_image->colorspace=RGBColorspace;
  allocate_image->compose=OverCompositeOp;
  allocate_image->blur=1.0;
  GetExceptionInfo(&allocate_image->exception);
  (void) QueryColorDatabase(BackgroundColor,&allocate_image->background_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(BorderColor,&allocate_image->border_color,
    &allocate_image->exception);
  (void) QueryColorDatabase(MatteColor,&allocate_image->matte_color,
    &allocate_image->exception);
  GetTimerInfo(&allocate_image->timer);
  GetCacheInfo(&allocate_image->cache);
  allocate_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  allocate_image->logging=IsEventLogging();
  allocate_image->reference_count=1;
  allocate_image->signature=MagickSignature;
  if (image_info == (ImageInfo *) NULL)
    return(allocate_image);
  /*
    Transfer image info.
  */
  SetBlobClosable(allocate_image,(image_info->file == NULL));
  (void) strncpy(allocate_image->filename,image_info->filename,MaxTextExtent-1);
  (void) strncpy(allocate_image->magick_filename,image_info->filename,
    MaxTextExtent-1);
  (void) strncpy(allocate_image->magick,image_info->magick,MaxTextExtent-1);
  if (image_info->size != (char *) NULL)
    {
      (void) GetGeometry(image_info->size,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
      allocate_image->offset=allocate_image->tile_info.x;
      allocate_image->tile_info.width=allocate_image->columns;
      allocate_image->tile_info.height=allocate_image->rows;
    }
  if (image_info->tile != (char *) NULL)
    if (!IsSubimage(image_info->tile,False))
      (void) GetGeometry(image_info->tile,&allocate_image->tile_info.x,
        &allocate_image->tile_info.y,&allocate_image->columns,
        &allocate_image->rows);
  allocate_image->compression=image_info->compression;
  allocate_image->dither=image_info->dither;
  allocate_image->interlace=image_info->interlace;
  allocate_image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=GetMagickDimension(image_info->density,&allocate_image->x_resolution,
        &allocate_image->y_resolution);
      if (count != 2)
        allocate_image->y_resolution=allocate_image->x_resolution;
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      allocate_image->page=allocate_image->tile_info;
      geometry=GetPageGeometry(image_info->page);
      (void) GetGeometry(geometry,&allocate_image->page.x,
        &allocate_image->page.y,&allocate_image->page.width,
        &allocate_image->page.height);
      MagickFreeMemory(geometry);
    }
  allocate_image->depth=image_info->depth;
  allocate_image->background_color=image_info->background_color;
  allocate_image->border_color=image_info->border_color;
  allocate_image->matte_color=image_info->matte_color;
  allocate_image->client_data=image_info->client_data;
  if (image_info->cache != (void *) NULL)
    ClonePixelCacheMethods(allocate_image->cache,image_info->cache);
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
%  it to a linear gray colorspace with increasing intensity.  If the image
%  already has a colormap, it is replaced.  AllocateImageColormap() returns
%  True if successful, otherwise False if there is not enough memory.
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

  Quantum
    quantum;

  /*
    Allocate image colormap.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  image->storage_class=PseudoClass;
  image->colors=colors;
  length=image->colors*sizeof(PixelPacket);
  if (image->colormap == (PixelPacket *) NULL)
    image->colormap=MagickAllocateMemory(PixelPacket *,length);
  else
    MagickReallocMemory(image->colormap,length);
  if (image->colormap == (PixelPacket *) NULL)
    return(False);
  for (i=0; i < (long) image->colors; i++)
  {
    quantum=(Quantum) (i*(MaxRGB/Max(colors-1,1)));
    image->colormap[i].red=quantum;
    image->colormap[i].green=quantum;
    image->colormap[i].blue=quantum;
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
%  Use AllocateNextImage() to initialize the next image in a sequence to
%  default values.  The next member of image points to the newly allocated
%  image.  If there is a memory shortage, next is assigned NULL.
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
  image->next=AllocateImage(image_info);
  if (image->next == (Image *) NULL)
    return;
  (void) strncpy(image->next->filename,image->filename,MaxTextExtent-1);
  if (image_info != (ImageInfo *) NULL)
    (void) strncpy(image->next->filename,image_info->filename,MaxTextExtent-1);
  DestroyBlob(image->next);
  image->next->blob=ReferenceBlob(image->blob);
  image->next->scene=image->scene+1;
  image->next->previous=image;
}

#if defined(HasX11)
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
  resource.image_info=CloneImageInfo(image_info);
  resource.immutable=True;
  (void) XAnimateImages(display,&resource,&client_name,1,image);
  (void) XCloseDisplay(display);
  DestroyImageInfo(resource.image_info);
  return(image->exception.severity == UndefinedException);
}
#else
MagickExport unsigned int AnimateImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,XWindowLibraryIsNotAvailable,
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
%  The AppendImages() method takes a set of images and appends them to each
%  other top-to-bottom if the stack parameter is true, otherwise left-to-right.
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
%    o stack: A value other than 0 stacks the images top-to-bottom.
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
    x,
    y;

  unsigned int
    status;

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
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError, ImageSequenceIsRequired,
      UnableToAppendImage);
  width=image->columns;
  height=image->rows;
  for (next=image->next; next != (Image *) NULL; next=next->next)
  {
    if (stack)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        continue;
      }
    width+=next->columns;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Initialize append next attributes.
  */
  append_image=CloneImage(image,width,height,True,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  SetImage(append_image,OpaqueOpacity);
  scene=0;
  if (stack)
    {
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
        status=MagickMonitor(AppendImageText,scene,GetImageListLength(image),
          exception);
        if (status == False)
          break;
        scene++;
      }
      return(append_image);
    }
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
    status=MagickMonitor(AppendImageText,scene++,GetImageListLength(image),
      exception);
    if (status == False)
      break;
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
%  The Average() method takes a set of images and averages them together.
%  Each image in the set must have the same width and height.  Average()
%  returns a single image with each corresponding pixel component of
%  each image averaged.   On failure, a NULL image is returned and
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

  DoublePixelPacket
    *pixels_array;

  register DoublePixelPacket
    *pixels;

  Image
    *average_image;

  long
    y;

  register const Image
    *next;

  register const PixelPacket
    *p;

  register long
    x;

  register PixelPacket
    *q;

  unsigned long
    number_scenes,
    number_pixels;

  /*
    Ensure the image are the same size.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if (image->next == (Image *) NULL)
    ThrowImageException3(ImageError,ImageSequenceIsRequired,
      UnableToAverageImage);
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if ((next->columns != image->columns) || (next->rows != image->rows))
      ThrowImageException3(OptionError,UnableToAverageImageSequence,
        ImageWidthsOrHeightsDiffer);
  }
  /*
    Allocate sum accumulation buffer.
  */
  number_pixels=image->columns*image->rows;
  pixels_array=MagickAllocateMemory(DoublePixelPacket *,
    number_pixels*sizeof(DoublePixelPacket));
  if (pixels_array == (DoublePixelPacket *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToAverageImageSequence);
  (void) memset(pixels_array,0,number_pixels*sizeof(DoublePixelPacket));
  /*
    Initialize average next attributes.
  */
  average_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (average_image == (Image *) NULL)
    {
      MagickFreeMemory(pixels_array);
      return((Image *) NULL);
    }
  SetImageType(average_image,TrueColorType);
  /*
    Compute sum over each pixel color component.
  */
  number_scenes=0;
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    pixels=pixels_array;
    for (y=0; y < (long) next->rows; y++)
    {
      p=AcquireImagePixels(next,0,y,next->columns,1,exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=(long) next->columns; x > 0; x--)
      {
        pixels->red+=p->red;
        pixels->green+=p->green;
        pixels->blue+=p->blue;
        pixels->opacity+=p->opacity;
        p++;
        pixels++;
      }
    }
    number_scenes++;
  }
  /*
    Average next pixels.
  */
  pixels=pixels_array;
  for (y=0; y < (long) average_image->rows; y++)
  {
    q=SetImagePixels(average_image,0,y,average_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=(long) average_image->columns; x > 0; x--)
    {
      q->red=(Quantum) (pixels->red/number_scenes+0.5);
      q->green=(Quantum) (pixels->green/number_scenes+0.5);
      q->blue=(Quantum) (pixels->blue/number_scenes+0.5);
      q->opacity=(Quantum) (pixels->opacity/number_scenes+0.5);
      q++;
      pixels++;
    }
    if (!SyncImagePixels(average_image))
      break;
    if (QuantumTick(y,average_image->rows))
      if (!MagickMonitor(AverageImageText,y,average_image->rows,exception))
        break;
  }
  MagickFreeMemory(pixels_array);
  return(average_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C a t c h I m a g e E x c e p t i o n                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CatchImageException() returns if no exceptions are found in the image
%  sequence, otherwise it determines the most severe exception and reports
%  it as a warning or error depending on the severity.
%
%  The format of the CatchImageException method is:
%
%      ExceptionType CatchImageException(Image *image)
%
%  A description of each parameter follows:
%
%    o image: An image sequence.
%
%
*/
MagickExport ExceptionType CatchImageException(Image *image)
{
  ExceptionInfo
    exception;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  GetExceptionInfo(&exception);
  GetImageException(image,&exception);
  CatchException(&exception);
  DestroyExceptionInfo(&exception);
  return(exception.severity);
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
%  Transform an image so that the resulting image is a grayscale image
%  based on a specified image channel. The resulting image is returned in
%  the RGB colorspace. This function does not force or assume an input
%  image colorspace so it may be used to extract channels from images in
%  colorspaces other than RGB or CMYK. For example, if the image is currently
%  transformed to the HWB colorspace, the 'B' channel may be extracted by
%  specifying RedChannel as the ChannelType argument.
%
%  The format of the ChannelImage method is:
%
%      unsigned int ChannelImage(Image *image,const ChannelType channel)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Identify which channel to extract: Red, Cyan, Green, Magenta,
%    Blue, Yellow, or Opacity.
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

  /*
    Channel DirectClass packets.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == PseudoClass)
    {
      SyncImage(image);
      image->storage_class=DirectClass;
    }
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    switch (channel)
      {
      case RedChannel:
      case CyanChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->green=q->red;
              q->blue=q->red;
              q++;
            }
          break;
        }
      case GreenChannel:
      case MagentaChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->green;
              q->blue=q->green;
              q++;
            }
          break;
        }
      case BlueChannel:
      case YellowChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->blue;
              q->green=q->blue;
              q++;
            }
          break;
        }
      case OpacityChannel:
        {
          if (image->colorspace == CMYKColorspace)
            {
              indexes=GetIndexes(image);
              for (x=(long) image->columns; x > 0; x--)
                {
                  q->red=*indexes;
                  q->green=*indexes;
                  q->blue=*indexes;
                  q->opacity=OpaqueOpacity;
                  q++;
                  indexes++;
                }
            }
          else
            {
              for (x=(long) image->columns; x > 0; x--)
                {
                  q->red=q->opacity;
                  q->green=q->opacity;
                  q->blue=q->opacity;
                  q->opacity=OpaqueOpacity;
                  q++;
                }
            }
          image->matte=False;
          break;
        }
      case MatteChannel:
      case BlackChannel:
        {
          for (x=(long) image->columns; x > 0; x--)
            {
              q->red=q->opacity;
              q->green=q->opacity;
              q->blue=q->opacity;
              q->opacity=OpaqueOpacity;
              q++;
            }
          image->matte=False;
          break;
        }
      default:
        {
        }
      }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(ChannelImageText,y,image->rows,&image->exception))
        break;
  }
  image->colorspace=RGBColorspace;
  image->is_grayscale=True;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l i p P a t h I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ClipPathImage() sets the image clip mask based any clipping path information
%  if it exists.
%
%  The format of the ClipPathImage method is:
%
%      unsigned int ClipPathImage(Image *image,const char *pathname,
%        const unsigned int inside)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o pathname: name of clipping path resource. If name is preceded by #, use
%      clipping path numbered by name.
%
%    o inside: if non-zero, later operations take effect inside clipping path.
%      Otherwise later operations take effect outside clipping path.
%
%
*/
MagickExport unsigned int ClipImage(Image *image)
{
  return(ClipPathImage(image,"#1",True));
}

MagickExport unsigned int ClipPathImage(Image *image,const char *pathname,
  const unsigned int inside)
{
#define ClipPathImageTag  "ClipPath/Image"

  char
    key[MaxTextExtent];

  const ImageAttribute
    *attribute;

  Image
    *clip_mask;

  ImageInfo
    *image_info;

  long
    intensity,
    y;

  register long
    x;

  register PixelPacket
    *q;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(pathname != NULL);
  FormatString(key,"8BIM:1999,2998:%s",pathname);
  attribute=GetImageAttribute(image,key);
  if (attribute == (const ImageAttribute *) NULL)
    return(False);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  (void) QueryColorDatabase("#ffffffff",&image_info->background_color,
    &image->exception);
  clip_mask=BlobToImage(image_info,attribute->value,strlen(attribute->value),
    &image->exception);
  DestroyImageInfo(image_info);
  if (clip_mask == (Image *) NULL)
    return(False);
  if (clip_mask->storage_class == PseudoClass)
    {
      SyncImage(clip_mask);
      clip_mask->storage_class=DirectClass;
    }
  clip_mask->matte=True;
  /*
    Force all pixels to be either black or white (opaque or transparent)
    to remove any unintended antialiasing effects created by the SVG
    renderer.
  */
  for (y=0; y < (long) clip_mask->rows; y++)
  {
    q=GetImagePixels(clip_mask,0,y,clip_mask->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) clip_mask->columns; x++)
    {
      intensity=PixelIntensityToQuantum(q);
      if (inside)
        intensity=intensity == TransparentOpacity ? TransparentOpacity :
          OpaqueOpacity;
      else
        intensity=intensity == TransparentOpacity ? OpaqueOpacity :
          TransparentOpacity;
      q->red=intensity;
      q->green=intensity;
      q->blue=intensity;
      q->opacity=intensity;
      q++;
    }
    if (!SyncImagePixels(clip_mask))
      break;
    if (QuantumTick(y,clip_mask->rows))
      if (!MagickMonitor(ClipPathImageTag,y,clip_mask->rows,&image->exception))
        break;
  }
  /*
    Overload magick_filename to keep name of path that created image.
    This is needed so we can get the path as postscript for PS coders
    to create a postscript vector based clipping path.
  */
  FormatString(clip_mask->magick_filename,"8BIM:1999,2998:%s\nPS",pathname);

  clip_mask->is_grayscale=True;
  clip_mask->is_monochrome=True;
  (void) SetImageClipMask(image,clip_mask);
  DestroyImage(clip_mask);
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
%  with the SetImagePixels() and SyncImagePixels() methods.  On failure,
%  a NULL image is returned and exception describes the reason for the
%  failure.
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
%    o columns: The number of columns in the cloned image.
%
%    o rows: The number of rows in the cloned image.
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
  const ImageAttribute
    *attribute;

  Image
    *clone_image;

  register long
    i;

  size_t
    length;

  /*
    Clone the image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_image=MagickAllocateMemory(Image *,sizeof(Image));
  if (clone_image == (Image *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToCloneImage);
  (void) memset(clone_image,0,sizeof(Image));
  clone_image->storage_class=image->storage_class;
  clone_image->colorspace=image->colorspace;
  clone_image->compression=image->compression;
  clone_image->dither=image->dither;
  clone_image->taint=image->taint;
  clone_image->is_grayscale=image->is_grayscale;
  clone_image->is_monochrome=image->is_monochrome;
  clone_image->matte=image->matte;
  clone_image->columns=image->columns;
  clone_image->rows=image->rows;
  clone_image->depth=image->depth;
  if (image->colormap != (PixelPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      clone_image->colors=image->colors;
      length=image->colors*sizeof(PixelPacket);
      clone_image->colormap=MagickAllocateMemory(PixelPacket *,length);
      if (clone_image->colormap == (PixelPacket *) NULL)
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCloneImage);
      length=image->colors*sizeof(PixelPacket);
      (void) memcpy(clone_image->colormap,image->colormap,length);
    }
  clone_image->background_color=image->background_color;
  clone_image->border_color=image->border_color;
  clone_image->matte_color=image->matte_color;
  clone_image->gamma=image->gamma;
  clone_image->chromaticity=image->chromaticity;
  if (image->color_profile.length != 0)
    {
      /*
        Allocate and clone any ICM profile.
      */
      clone_image->color_profile.name=AllocateString(image->color_profile.name);
      clone_image->color_profile.length=image->color_profile.length;
      clone_image->color_profile.info=MagickAllocateMemory(unsigned char *,
        clone_image->color_profile.length);
      if (clone_image->color_profile.info == (unsigned char *) NULL)
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCloneImage);
      (void) memcpy(clone_image->color_profile.info,
        image->color_profile.info,clone_image->color_profile.length);
    }
  if (image->iptc_profile.length != 0)
    {
      /*
        Allocate and clone any IPTC profile.
      */
      clone_image->iptc_profile.name=AllocateString(image->iptc_profile.name);
      clone_image->iptc_profile.length=image->iptc_profile.length;
      clone_image->iptc_profile.info=MagickAllocateMemory(unsigned char *,
        clone_image->iptc_profile.length);
      if (clone_image->iptc_profile.info == (unsigned char *) NULL)
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCloneImage);
      (void) memcpy(clone_image->iptc_profile.info,
        image->iptc_profile.info,clone_image->iptc_profile.length);
    }
  clone_image->generic_profiles=image->generic_profiles;
  if (image->generic_profiles != 0)
    {
      /*
        Allocate and clone any generic profiles.
      */
      clone_image->generic_profile=MagickAllocateMemory(ProfileInfo *,
        image->generic_profiles*sizeof(ProfileInfo));
      if (clone_image->generic_profile == (ProfileInfo *) NULL)
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
          UnableToCloneImage);
      length=image->generic_profiles*sizeof(ProfileInfo);
      (void) memcpy(clone_image->generic_profile,image->generic_profile,
        length);
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        clone_image->generic_profile[i].name=
          AllocateString(image->generic_profile[i].name);
        if (image->generic_profile[i].length == 0)
          continue;
        length=image->generic_profile[i].length;
        clone_image->generic_profile[i].info=
          MagickAllocateMemory(unsigned char *,length);
        if (clone_image->generic_profile[i].info == (unsigned char *) NULL)
          ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
            UnableToCloneImage);
        (void) memcpy(clone_image->generic_profile[i].info,
          image->generic_profile[i].info,length);
      }
    }
  clone_image->rendering_intent=image->rendering_intent;
  clone_image->units=image->units;
  clone_image->montage=(char *) NULL;
  clone_image->directory=(char *) NULL;
  clone_image->geometry=(char *) NULL;
  clone_image->offset=image->offset;
  clone_image->x_resolution=image->x_resolution;
  clone_image->y_resolution=image->y_resolution;
  clone_image->page=image->page;
  clone_image->tile_info=image->tile_info;
  clone_image->blur=image->blur;
  clone_image->fuzz=image->fuzz;
  clone_image->filter=image->filter;
  clone_image->interlace=image->interlace;
  clone_image->endian=image->endian;
  clone_image->gravity=image->gravity;
  clone_image->compose=image->compose;
  clone_image->signature=MagickSignature;
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
    (void) SetImageAttribute(clone_image,attribute->key,attribute->value);
  clone_image->scene=image->scene;
  clone_image->dispose=image->dispose;
  clone_image->delay=image->delay;
  clone_image->iterations=image->iterations;
  clone_image->total_colors=image->total_colors;
  clone_image->error=image->error;
  clone_image->semaphore=0;
  clone_image->logging=image->logging;
  clone_image->timer=image->timer;
  GetExceptionInfo(&clone_image->exception);
  CopyException(&clone_image->exception,&image->exception);
  clone_image->client_data=image->client_data;
  clone_image->start_loop=image->start_loop;
  clone_image->ascii85=0; /* Don't copy  ascii85 huffman support structure */
  clone_image->magick_columns=image->magick_columns;
  clone_image->magick_rows=image->magick_rows;
  (void) strncpy(clone_image->magick_filename,image->magick_filename,
    MaxTextExtent-1);
  (void) strncpy(clone_image->magick,image->magick,MaxTextExtent-1);
  (void) strncpy(clone_image->filename,image->filename,MaxTextExtent-1);
  clone_image->reference_count=1;
  clone_image->previous=(Image *) NULL;
  clone_image->list=(Image *) NULL;
  clone_image->next=(Image *) NULL;
  clone_image->clip_mask=(Image *) NULL;
  if (orphan)
    clone_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  else
    {
      clone_image->blob=ReferenceBlob(image->blob);
      if (image->previous != (Image *) NULL)
        clone_image->previous->next=clone_image;
      if (image->next != (Image *) NULL)
        clone_image->next->previous=clone_image;
    }
  if ((columns == 0) && (rows == 0))
    {
      if (image->montage != (char *) NULL)
        (void) CloneString(&clone_image->montage,image->montage);
      if (image->directory != (char *) NULL)
        (void) CloneString(&clone_image->directory,image->directory);
      if (image->clip_mask != (Image *) NULL)
        clone_image->clip_mask=CloneImage(image->clip_mask,0,0,True,exception);
      clone_image->cache=ReferenceCache(image->cache);
      return(clone_image);
    }
  clone_image->page.width=columns;
  clone_image->page.height=rows;
  clone_image->page.x=(long) columns*image->page.x/(long) clone_image->columns;
  clone_image->page.y=(long) rows*image->page.y/(long) clone_image->rows;
  clone_image->columns=columns;
  clone_image->rows=rows;
  GetCacheInfo(&clone_image->cache);
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

  clone_info=MagickAllocateMemory(ImageInfo *,sizeof(ImageInfo));
  if (clone_info == (ImageInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToCloneImageInfo);
  GetImageInfo(clone_info);
  if (image_info == (ImageInfo *) NULL)
    return(clone_info);
  clone_info->compression=image_info->compression;
  clone_info->temporary=image_info->temporary;
  clone_info->adjoin=image_info->adjoin;
  clone_info->antialias=image_info->antialias;
  clone_info->subimage=image_info->subimage;
  clone_info->subrange=image_info->subrange;
  clone_info->depth=image_info->depth;
  if (image_info->size != (char *) NULL)
    clone_info->size=AllocateString(image_info->size);
  if (image_info->tile != (char *) NULL)
    clone_info->tile=AllocateString(image_info->tile);
  if (image_info->page != (char *) NULL)
    clone_info->page=AllocateString(image_info->page);
  clone_info->interlace=image_info->interlace;
  clone_info->endian=image_info->endian;
  clone_info->units=image_info->units;
  clone_info->quality=image_info->quality;
  if (image_info->sampling_factor != (char *) NULL)
    clone_info->sampling_factor=AllocateString(image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    clone_info->server_name=AllocateString(image_info->server_name);
  if (image_info->font != (char *) NULL)
    clone_info->font=AllocateString(image_info->font);
  if (image_info->texture != (char *) NULL)
    clone_info->texture=AllocateString(image_info->texture);
  if (image_info->density != (char *) NULL)
    clone_info->density=AllocateString(image_info->density);
  clone_info->pointsize=image_info->pointsize;
  clone_info->fuzz=image_info->fuzz;
  clone_info->pen=image_info->pen;
  clone_info->background_color=image_info->background_color;
  clone_info->border_color=image_info->border_color;
  clone_info->matte_color=image_info->matte_color;
  clone_info->dither=image_info->dither;
  clone_info->monochrome=image_info->monochrome;
  clone_info->colorspace=image_info->colorspace;
  clone_info->type=image_info->type;
  clone_info->preview_type=image_info->preview_type;
  clone_info->group=image_info->group;
  clone_info->ping=image_info->ping;
  clone_info->verbose=image_info->verbose;
  if (image_info->view != (char *) NULL)
    clone_info->view=AllocateString(image_info->view);
  if (image_info->authenticate != (char *) NULL)
    clone_info->authenticate=AllocateString(image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    clone_info->attributes=CloneImage(image_info->attributes,0,0,True,
      &image_info->attributes->exception);
  if (image_info->definitions != (MagickMap) NULL)
    clone_info->definitions=MagickMapCloneMap(image_info->definitions,0);
  clone_info->client_data=image_info->client_data;
  clone_info->cache=image_info->cache;
  if (image_info->cache != (void *) NULL)
    clone_info->cache=ReferenceCache(image_info->cache);
  clone_info->file=image_info->file;
  clone_info->stream=image_info->stream;
  clone_info->blob=image_info->blob;
  clone_info->length=image_info->length;
  (void) strncpy(clone_info->magick,image_info->magick,MaxTextExtent-1);
  (void) strncpy(clone_info->unique,image_info->unique,MaxTextExtent-1);
  (void) strncpy(clone_info->zero,image_info->zero,MaxTextExtent-1);
  (void) strncpy(clone_info->filename,image_info->filename,MaxTextExtent-1);
  clone_info->signature=image_info->signature;
  return(clone_info);
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
%  CycleColormap() displaces an image's colormap by a given number of
%  positions.  If you cycle the colormap a number of times you can produce
%  a psychodelic effect.
%
%  The format of the CycleColormapImage method is:
%
%      CycleColormapImage(Image *image,const int amount)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o amount:  Offset the colormap this much.
%
%
*/
MagickExport unsigned int  CycleColormapImage(Image *image,const int amount)
{
  long
    index,
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  unsigned int
    is_grayscale,
    is_monochrome,
    status = True;


  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  is_monochrome=image->is_monochrome;
  if (image->storage_class == DirectClass)
    SetImageType(image,PaletteType);
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      {
        status=False;
        break;
      }
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      index=(long) ((indexes[x]+amount) % image->colors);
      if (index < 0)
        index+=image->colors;
      indexes[x]=(IndexPacket) index;
      q->red=image->colormap[index].red;
      q->green=image->colormap[index].green;
      q->blue=image->colormap[index].blue;
      q++;
    }
    if (!SyncImagePixels(image))
      {
        status=False;
        break;
      }
  }
  image->is_grayscale=is_grayscale;
  image->is_monochrome=is_monochrome;
  return(status);
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
%  DescribeImage() describes an image by printing its attributes to the file.
%  Attributes include the image width, height, size, and others.
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
MagickExport unsigned int DescribeImage(Image *image,FILE *file,
  const unsigned int verbose)
{
  char
    color[MaxTextExtent],
    format[MaxTextExtent];

  const ImageAttribute
    *attribute;

  const MagickInfo
    *magick_info;

  double
    elapsed_time,
    user_time;

  Image
    *p;

  long
    y;

  register long
    i,
    x;

  unsigned long
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
      (void) fprintf(file,"%lux%lu%+ld%+ld ",image->columns,image->rows,
        image->page.x,image->page.y);
      if (image->storage_class == DirectClass)
        {
          (void) fprintf(file,"DirectClass ");
          if (image->total_colors != 0)
            {
              FormatSize(image->total_colors,format);
              (void) fprintf(file,"%.1024s ",format);
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
              (long) image->error.mean_error_per_pixel,
              image->error.normalized_mean_error,
              image->error.normalized_maximum_error);
          }
      (void) fprintf(file,"%lu-bit ",image->depth);
      if (GetBlobSize(image) != 0)
        {
          FormatSize(GetBlobSize(image),format);
          (void) fprintf(file,"%.1024s ",format);
        }
      (void) fprintf(file,"%0.3fu %ld:%02ld\n",user_time,
        (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
      return (ferror(file) ? False : True);
    }
  /*
    Display verbose info about the image.
  */
  (void) SignatureImage(image);
  image->total_colors=GetNumberColors(image,(FILE *) NULL,&image->exception);
  (void) fprintf(file,"Image: %.1024s\n",image->filename);
  magick_info=GetMagickInfo(image->magick,&image->exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
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
      (void) fprintf(file,"  Base geometry: %lux%lu\n",image->magick_columns,
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
  (void) fprintf(file,"  Channel Depths:\n");
  if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:     %u bits\n",
                     GetImageChannelDepth(image,CyanChannel,&image->exception));
      (void) fprintf(file,"    Magenta:  %u bits\n",
                     GetImageChannelDepth(image,MagentaChannel,&image->exception));
      (void) fprintf(file,"    Yellow:   %u bits\n",
                     GetImageChannelDepth(image,YellowChannel,&image->exception));
      (void) fprintf(file,"    Black:    %u bits\n",
                     GetImageChannelDepth(image,BlackChannel,&image->exception));
      if (image->matte)
        (void) fprintf(file,"    Opacity:  %u bits\n",
                       GetImageChannelDepth(image,OpacityChannel,&image->exception));
    }
  else
    {
      (void) fprintf(file,"    Red:      %u bits\n",
                     GetImageChannelDepth(image,RedChannel,&image->exception));
      (void) fprintf(file,"    Green:    %u bits\n",
                     GetImageChannelDepth(image,GreenChannel,&image->exception));
      (void) fprintf(file,"    Blue:     %u bits\n",
                     GetImageChannelDepth(image,BlueChannel,&image->exception));
      if (image->matte)
        (void) fprintf(file,"    Opacity:  %u bits\n",
                       GetImageChannelDepth(image,OpacityChannel,&image->exception));
    }
  (void) fprintf(file,"  Channel Statistics:\n");
  {
    ImageStatistics
      statistics;

    GetImageStatistics(image,&statistics,&image->exception);

    if (image->colorspace == CMYKColorspace)
    {
      (void) fprintf(file,"    Cyan:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.red.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.red.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.red.standard_deviation);
      (void) fprintf(file,"    Magenta:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.green.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.green.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.green.standard_deviation);
      (void) fprintf(file,"    Yellow:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.blue.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.blue.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.blue.standard_deviation);
      (void) fprintf(file,"    Black:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
              (unsigned int) RoundToQuantum(MaxRGB*statistics.opacity.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
              (unsigned int) RoundToQuantum(MaxRGB*statistics.opacity.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
              MaxRGB*statistics.opacity.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
              MaxRGB*statistics.opacity.standard_deviation);
      /*
      if (image->matte)
        (void) fprintf(file,"    Opacity:\n");
      */
    }
  else
    {
      (void) fprintf(file,"    Red:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.red.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.red.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.red.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.red.standard_deviation);
      (void) fprintf(file,"    Green:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.green.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.green.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.green.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.green.standard_deviation);
      (void) fprintf(file,"    Blue:\n");
      (void) fprintf(file,"      Minimum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.blue.minimum));
      (void) fprintf(file,"      Maximum:            %10u\n",
               (unsigned int) RoundToQuantum(MaxRGB*statistics.blue.maximum));
      (void) fprintf(file,"      Mean:               %15.4lf\n",
               MaxRGB*statistics.blue.mean);
      (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
               MaxRGB*statistics.blue.standard_deviation);
      if (image->matte)
        {
          (void) fprintf(file,"    Opacity:\n");
          (void) fprintf(file,"      Minimum:            %10u\n",
                   (unsigned int) RoundToQuantum(MaxRGB*statistics.opacity.minimum));
          (void) fprintf(file,"      Maximum:            %10u\n",
                   (unsigned int) RoundToQuantum(MaxRGB*statistics.opacity.maximum));
          (void) fprintf(file,"      Mean:               %15.4lf\n",
                   MaxRGB*statistics.opacity.mean);
          (void) fprintf(file,"      Standard Deviation: %15.4lf\n",
                   MaxRGB*statistics.opacity.standard_deviation);
        }
    }
  }
  x=0;
  p=(Image *) NULL;
  if ((image->matte && (strcmp(image->magick,"GIF") != 0)) || image->taint)
    {
      char
        tuple[MaxTextExtent];

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
        {
          GetColorTuple(p,image->depth,image->matte,False,tuple);
          (void) fprintf(file,"  Opacity: %.1024s\t",tuple);
          GetColorTuple(p,image->depth,image->matte,True,tuple);
          (void) fprintf(file,"  %.1024s\n",tuple);
        }
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
        char
          tuple[MaxTextExtent];

        GetColorTuple(p,image->depth,image->matte,False,tuple);
        (void) fprintf(file,"    %lu: %.1024s",i,tuple);
        (void) fprintf(file,"\t");
        (void) QueryColorname(image,p,SVGCompliance,name,&image->exception);
        (void) fprintf(file,"  %.1024s",name);
        (void) fprintf(file,"\n");
        p++;
      }
    }
  if (image->error.mean_error_per_pixel != 0.0)
    (void) fprintf(file,"  Mean Exception Per Pixel: %ld\n",
      (long) image->error.mean_error_per_pixel);
  if (image->error.normalized_mean_error != 0.0)
    (void) fprintf(file,"  Normalized Mean Exception: %g\n",
      image->error.normalized_mean_error);
  if (image->error.normalized_maximum_error != 0.0)
    (void) fprintf(file,"  Normalized Maximum Exception: %gn",
      image->error.normalized_maximum_error);
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
  if ((image->chromaticity.red_primary.x != 0.0) ||
      (image->chromaticity.green_primary.x != 0.0) ||
      (image->chromaticity.blue_primary.x != 0.0) ||
      (image->chromaticity.white_point.x != 0.0))
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
  if ((image->tile_info.width*image->tile_info.height) != 0)
    (void) fprintf(file,"  Tile geometry: %lux%lu%+ld%+ld\n",
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
  FormatSize(GetBlobSize(image),format);
  (void) fprintf(file,"  Filesize: %.1024s\n",format);
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
  (void) QueryColorname(image,&image->background_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Background Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->border_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Border Color: %.1024s\n",color);
  (void) QueryColorname(image,&image->matte_color,SVGCompliance,color,
    &image->exception);
  (void) fprintf(file,"  Matte Color: %.1024s\n",color);
  if ((image->page.width != 0) && (image->page.height != 0))
    (void) fprintf(file,"  Page geometry: %lux%lu%+ld%+ld\n",image->page.width,
      image->page.height,image->page.x,image->page.y);
  (void) fprintf(file,"  Dispose: ");
  switch (image->dispose)
  {
    case UndefinedDispose: (void) fprintf(file,"Undefined\n"); break;
    case NoneDispose: (void) fprintf(file,"None\n"); break;
    case BackgroundDispose: (void) fprintf(file,"Background\n"); break;
    case PreviousDispose: (void) fprintf(file,"Previous\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
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
    (void) fprintf(file,"  Scene: %lu of %lu\n",image->scene,count);
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
    case LosslessJPEGCompression: (void) fprintf(file,"Lossless JPEG\n"); break;
    case LZWCompression: (void) fprintf(file,"LZW\n"); break;
    case RLECompression: (void) fprintf(file,"Runlength Encoded\n"); break;
    case ZipCompression: (void) fprintf(file,"Zip\n"); break;
    default: (void) fprintf(file,"\n");  break;
  }
  /*
    Display formatted image attributes. This must happen before we access
    any pseudo attributes like EXIF since doing so causes real attributes
    to be created and we would get duplicates in the output.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  {
#if defined(WRAP_ATTRIBUTES)
    int
      screen_width=79;

    if (getenv("COLUMNS"))
        screen_width=atoi(getenv("COLUMNS"))-1;
#endif /* defined(WRAP_ATTRIBUTES) */

    for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
      {
        (void) fprintf(file,"  %c", toupper((int)attribute->key[0]));
        if (strlen(attribute->key) > 1)
          (void) fprintf(file,"%.1024s",attribute->key+1);
#if defined(WRAP_ATTRIBUTES)
        {
          int
            attribute_length,
            formatted_chars=0,
            length=0,
            start_column=4,
            strip_length;

          char
            *s;

          (void) fprintf(file,":\n");
          attribute_length=strlen(attribute->value);
          for (s=attribute->value; length < attribute_length; s+=formatted_chars)
            {
              fprintf(file,"%*s",start_column,"");
              strip_length=screen_width-start_column;
              if (length+strip_length < attribute_length)
                {
                  char
                    *e;

                  for(e=s+strip_length; (e > s) && ( !isspace((int)(*(e-1)))) ; e--);
                  if ((e > s) && (*(e-1) == ' '))
                    strip_length=e-s;
                }
              formatted_chars=fprintf(file,"%.*s",strip_length,s);
              length+=formatted_chars;
              fprintf(file,"\n");
            }
        }
#else
        (void) fprintf(file,": ");
        (void) fprintf(file,"%s\n",attribute->value);
#endif
      }
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
        text=MagickAllocateMemory(char *,length+1);
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
                  MagickFreeMemory(textlist[j]);
                }
                MagickFreeMemory(textlist);
              }
            MagickFreeMemory(text);
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
    if (LocaleCompare(image->generic_profile[i].name,"EXIF") == 0)
      {
        attribute=GetImageAttribute(image,"EXIF:*");
        if (attribute != (const ImageAttribute *) NULL)
          {
            char
              **values;

            register char
              *p;

            values=StringToList(attribute->value);
            if (values != (char **) NULL)
              {
                for (x=0; values[x] != (char *) NULL; x++)
                {
                  (void) fprintf(file,"    ");
                  for (p=values[x]; *p != '\0'; p++)
                  {
                    if (p > values[x])
                      if ((isupper((int) ((unsigned char) *p)) != False) &&
                          (islower((int) ((unsigned char) *(p+1))) != False))
                        (void) fprintf(file," ");
                    if (*p == '=')
                      {
                        (void) fprintf(file,": ");
                        for (p++; *p != '\0'; p++)
                          (void) fputc(*p,file);
                        break;
                      }
                    (void) fputc(*p,file);
                  }
                  (void) fputc('\n',file);
                  MagickFreeMemory(values[x]);
                }
                MagickFreeMemory(values);
              }
          }
      }
  }
  if (image->montage != (char *) NULL)
    (void) fprintf(file,"  Montage: %.1024s\n",image->montage);
  if (image->directory != (char *) NULL)
    {
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
        tile=ReadImage(image_info,&image->exception);
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
        for ( ; attribute != (const ImageAttribute *) NULL; attribute=attribute->next)
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
  if (image->taint)
    (void) fprintf(file,"  Tainted: True\n");
  else
    (void) fprintf(file,"  Tainted: False\n");
  if (user_time != 0.0)
    (void) fprintf(file,"  User Time: %0.3fu\n",user_time);
  if (elapsed_time != 0.0)
    (void) fprintf(file,"  Elapsed Time: %ld:%02ld\n",
      (long) (elapsed_time/60.0),(long) ceil(fmod(elapsed_time,60.0)));
  (void) fflush(file);
  return (ferror(file) ? False : True);
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
  AcquireSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  image->reference_count--;
  if (image->reference_count == 0)
    destroy=True;
  LiberateSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  if (!destroy)
    return;
  /*
    Ensure that this image is not referenced by another image
  */
#if 0
  if (image->previous)
    assert(image->previous->next != image);
  if (image->next)
    assert(image->next->previous != image);
#endif
  /*
    Destroy image.
  */
  DestroyImagePixels(image);
  if (image->clip_mask != (Image *) NULL)
    DestroyImage(image->clip_mask);
  MagickFreeMemory(image->montage);
  MagickFreeMemory(image->directory);
  MagickFreeMemory(image->colormap);
  MagickFreeMemory(image->color_profile.name);
  if (image->color_profile.length != 0)
    MagickFreeMemory(image->color_profile.info);
  MagickFreeMemory(image->iptc_profile.name);
  if (image->iptc_profile.length != 0)
    MagickFreeMemory(image->iptc_profile.info);
  if (image->generic_profiles != 0)
    {
      for (i=0; i < (long) image->generic_profiles; i++)
      {
        MagickFreeMemory(image->generic_profile[i].name);
        if (image->generic_profile[i].length != 0)
          MagickFreeMemory(image->generic_profile[i].info);
      }
      MagickFreeMemory(image->generic_profile);
    }
  DestroyImageAttributes(image);
  DestroyExceptionInfo(&image->exception);
  MagickFreeMemory(image->ascii85);
  DestroyBlob(image);
  if (image->semaphore)
    DestroySemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  memset((void *)image,0xbf,sizeof(Image));
  MagickFreeMemory(image);
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
%  DestroyImageInfo() deallocates memory associated with a ImageInfo
%  structure.
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
    MagickFreeMemory(image_info->size);
  if (image_info->tile != (char *) NULL)
    MagickFreeMemory(image_info->tile);
  if (image_info->page != (char *) NULL)
    MagickFreeMemory(image_info->page);
  if (image_info->sampling_factor != (char *) NULL)
    MagickFreeMemory(image_info->sampling_factor);
  if (image_info->server_name != (char *) NULL)
    MagickFreeMemory(image_info->server_name);
  if (image_info->font != (char *) NULL)
    MagickFreeMemory(image_info->font);
  if (image_info->texture != (char *) NULL)
    MagickFreeMemory(image_info->texture);
  if (image_info->density != (char *) NULL)
    MagickFreeMemory(image_info->density);
  if (image_info->view != (char *) NULL)
    MagickFreeMemory(image_info->view);
  if (image_info->authenticate != (char *) NULL)
    MagickFreeMemory(image_info->authenticate);
  if (image_info->attributes != (Image *) NULL)
    DestroyImage(image_info->attributes);
  if (image_info->cache != (void *) NULL)
    DestroyCacheInfo(image_info->cache);
  if (image_info->definitions != (MagickMap) NULL)
    MagickMapDeallocateMap(image_info->definitions);
  memset((void *)image_info,0xbf,sizeof(ImageInfo));
  MagickFreeMemory(image_info);
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
%  DisplayImages() displays an image sequence to any X window screen.  It
%  returns a value other than 0 if successful.  Check the exception member
%  of image to determine the reason for any failure.
%
%  The format of the DisplayImages method is:
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
    resource_info;

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
  XGetResourceInfo(resource_database,client_name,&resource_info);
  if (image_info->page != (char *) NULL)
    resource_info.image_geometry=AcquireString(image_info->page);
  resource_info.immutable=True;
  for (next=image; next; next=next->next)
  {
    state=DefaultState;
    (void) XDisplayImage(display,&resource_info,&client_name,1,&next,&state);
    if (state & ExitState)
      break;
  }
  if (resource_database != (XrmDatabase) NULL)
    {
      XrmDestroyDatabase(resource_database);
      resource_database=(XrmDatabase) NULL;
    }
  XDestroyResourceInfo(&resource_info);
  XDestroyX11Resources();
  (void) XCloseDisplay(display);
  return(image->exception.severity != UndefinedException);
}
#else
MagickExport unsigned int DisplayImages(const ImageInfo *image_info,
  Image *image)
{
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  ThrowBinaryException(MissingDelegateError,XWindowLibraryIsNotAvailable,
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
  bounds.x=(long) image->columns;
  bounds.y=(long) image->rows;
  corners[0]=AcquireOnePixel(image,0,0,exception);
  corners[1]=AcquireOnePixel(image,(long) image->columns-1,0,exception);
  corners[2]=AcquireOnePixel(image,0,(long) image->rows-1,exception);
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
        if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
          if (x < bounds.x)
            bounds.x=x;
        if (!FuzzyColorMatch(p,&corners[1],image->fuzz))
          if (x > (long) bounds.width)
            bounds.width=x;
        if (!FuzzyColorMatch(p,&corners[0],image->fuzz))
          if (y < bounds.y)
            bounds.y=y;
        if (!FuzzyColorMatch(p,&corners[2],image->fuzz))
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
%   G e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageChannelDepth() returns the minimum bit depth required to store
%  the specified image channel without actual loss of color resolution.
%  Pixel components are stored in a Quantum, which is 8, 16, or 32 bits
%  depending on the QuantumDepth value set when the software is compiled.
%  GetImageChannelDepth() returns the smallest modulus storage size which
%  supports the scale of the pixel within the range (i.e. no information is
%  lost). As an example, the value one is returned for a bilevel channel
%  since only one bit of resolution is required to represent a bilevel channel.
%
%  The format of the GetImageChannelDepth method is:
%
%      unsigned long GetImageChannelDepth(const Image *image,
%                      const ChannelType channel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to test.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define COMPUTE_CHANNEL_DEPTH(depth,parameter) \
{ \
  long \
    y; \
\
  depth=1; \
  for (y=0; y < (long) image->rows; y++) \
    { \
      register const PixelPacket \
        *p; \
\
      register long \
        x; \
\
      register IndexPacket \
        *indexes; \
\
      register unsigned int \
        scale; \
\
      p=AcquireImagePixels(image,0,y,image->columns,1,exception); \
      if (p == (const PixelPacket *) NULL) \
        break; \
      indexes=GetIndexes(image); \
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth)); \
      x=(long) image->columns; \
      while(x > 0) \
        { \
          if ((parameter) != scale*((parameter)/scale)) \
            { \
              depth++; \
              if (depth == QuantumDepth) \
                break; \
              scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth)); \
              continue; \
            } \
          p++; \
          indexes++; \
          x--; \
        } \
      if (depth == QuantumDepth) \
        break; \
    } \
}

MagickExport unsigned int GetImageChannelDepth(const Image *image,
  const ChannelType channel, ExceptionInfo *exception)
{
  unsigned int
    depth=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  switch (channel)
    {
    case RedChannel:
    case CyanChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->red);
        break;
      }
    case GreenChannel:
    case MagentaChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->green);
        break;
      }
    case BlueChannel:
    case YellowChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->blue);
        break;
      }
    case OpacityChannel:
      {
        if (image->colorspace == CMYKColorspace)
          {
            COMPUTE_CHANNEL_DEPTH(depth,*indexes);
          }
        else
          {
            COMPUTE_CHANNEL_DEPTH(depth,p->opacity);
          }
        break;
      }
    case BlackChannel:
    case MatteChannel:
      {
        COMPUTE_CHANNEL_DEPTH(depth,p->opacity);
        break;
      }
    default:
      {
      }
    }

  return depth;
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
%  GetImageDepth() returns the minimum bit depth of the image required to
%  ensure that data is not lost in the red, green, blue, and opacity, channels.
%  Pixel components are stored in a Quantum, which is 8, 16, or 32 bits
%  depending on the QuantumDepth value set when the software is compiled.
%  GetImageDepth() returns the smallest modulus storage size which supports
%  the scale of the pixel within the range (i.e. no information is lost).
%  As an example, the value one is returned for a black and white image
%  since only one bit of resolution is required to represent a black and white
%  image.
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

  register unsigned int
    scale;

  unsigned int
    depth=1;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (image->is_monochrome)
    return depth;

  if ((image->storage_class == PseudoClass) && !(image->matte))
    {
      p=image->colormap;
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
      for (x=image->colors ; x > 0; x--)
        {
          if ((p->red != scale*(p->red/scale)) ||
              (p->green != scale*(p->green/scale)) ||
              (p->blue != scale*(p->blue/scale)))
            {
              depth++;
              if (depth == QuantumDepth)
                break;
              scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
              continue;
            }
          p++;
        }
    }
  else
    {
      /*
        Directclass
      */
      for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,exception);
          if (p == (const PixelPacket *) NULL)
            break;
          scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
          x=(long) image->columns;
          while(x > 0)
            {
              if ((p->red != scale*(p->red/scale)) ||
                  (p->green != scale*(p->green/scale)) ||
                  (p->blue != scale*(p->blue/scale)) ||
                  (image->matte &&
                   (p->opacity != scale*((p->opacity/scale)))))
                {
                  depth++;
                  if (depth == QuantumDepth)
                    break;
                  scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
                  continue;
                }
              p++;
              x--;
            }
          if (depth == QuantumDepth)
            break;
        }
    }

  return depth;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e E x c e p t i o n                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageException() traverses an image sequence and returns any
%  error more severe than noted by the exception parameter.
%
%  The format of the GetImageException method is:
%
%      void GetImageException(Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to a list of one or more images.
%
%    o exception: return the highest severity exception.
%
%
*/
MagickExport void GetImageException(Image *image,ExceptionInfo *exception)
{
  register Image
    *next;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  for (next=image; next != (Image *) NULL; next=next->next)
  {
    if (next->exception.severity == UndefinedException)
      continue;
    if (next->exception.severity > exception->severity)
      CopyException(exception,&next->exception);
    next->exception.severity=UndefinedException;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e G e o m e t r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageGeometry() returns a region as defined by the geometry string with
%  respect to the image and its gravity.
%
%  The format of the GetImageGeometry method is:
%
%      int GetImageGeometry(const Image *image,const char *geometry,
%        const unsigned int size_to_fit,RectangleInfo *region_info)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetImageGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o geometry:  The geometry (e.g. 100x100+10+10).
%
%    o size_to_fit:  A value other than 0 means to scale the region so it
%      fits within the specified width and height.
%
%    o region_info: The region as defined by the geometry string with
%      respect to the image and its gravity.
%
%
*/
MagickExport int GetImageGeometry(const Image *image,const char *geometry,
  const unsigned int size_to_fit,RectangleInfo *region_info)
{
  char
    region_geometry[MaxTextExtent];

  int
    flags;

  region_info->width=image->columns;
  region_info->height=image->rows;
  region_info->x=0;
  region_info->y=0;
  (void) strncpy(region_geometry,geometry,MaxTextExtent-2);
  if (!size_to_fit)
    (void) strcat(region_geometry,"!");
  flags=GetMagickGeometry(region_geometry,&region_info->x,&region_info->y,
    &region_info->width,&region_info->height);
  switch (image->gravity)
  {
    case ForgetGravity:
    case NorthWestGravity:
      break;
    case NorthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      break;
    }
    case NorthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      break;
    }
    case WestGravity:
    {
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case StaticGravity:
    case CenterGravity:
    default:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case EastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y+=(long) (image->rows/2-region_info->height/2);
      break;
    }
    case SouthWestGravity:
    {
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthGravity:
    {
      region_info->x+=(long) (image->columns/2-region_info->width/2);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
    case SouthEastGravity:
    {
      region_info->x=(long) (image->columns-region_info->width-region_info->x);
      region_info->y=(long) (image->rows-region_info->height-region_info->y);
      break;
    }
  }
  return(flags);
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
%  GetImageInfo() initializes image_info to default values.
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
  ExceptionInfo
    exception;

  /*
    File and image dimension members.
  */
  assert(image_info != (ImageInfo *) NULL);
  (void) memset(image_info,0,sizeof(ImageInfo));
  image_info->adjoin=True;
  image_info->depth=QuantumDepth;
  image_info->interlace=NoInterlace;
  image_info->quality=DefaultCompressionQuality;
  image_info->antialias=True;
  image_info->pointsize=12;
  image_info->dither=True;
  GetExceptionInfo(&exception);
  (void) QueryColorDatabase(BackgroundColor,&image_info->background_color,
    &exception);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color,&exception);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color,&exception);
  DestroyExceptionInfo(&exception);
  image_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e S t a t i s t i c s                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageStatistics computes common statistics (currently maximum, minimum,
%  mean and standard deviation) for the available image channels. The
%  per-channel values are returned in an ImageStatistics structure. Statistics
%  are normalized to the range 0.0 to 1.0.  Multiply values by MaxRGB to obtain
%  the statistics in quantum units. Statistics for non-existent channels are
%  set to zero.
%
%  The format of the GetImageStatistics method is:
%
%      MagickPassFail GetImageStatistics(const Image *image,
%                                        ImageStatistics *statistics
%                                        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o statistics: An ImageStatistics structure to update with statistics.
%
%    o exception: Any errors are reported here.
%
*/
typedef struct _StatisticsContext {
  double samples;
  double variance_divisor;
  ImageStatistics *statistics;
} StatisticsContext;
static MagickPassFail GetImageStatisticsMean(void *user_context,
                                             const long x,const long y,
                                             const Image *image,
                                             const PixelPacket *pixel,
                                             ExceptionInfo *exception)
{
  StatisticsContext
    *context=(StatisticsContext *) user_context;

  ImageStatistics
    *statistics=context->statistics;

  double
    normalized;

  normalized=(double) pixel->red/MaxRGB;
  statistics->red.mean += normalized/context->samples;
  if (normalized > statistics->red.maximum)
    statistics->red.maximum=normalized;
  if (normalized <  statistics->red.minimum)
    statistics->red.minimum=normalized;

  normalized=(double) pixel->green/MaxRGB;
  statistics->green.mean += normalized/context->samples;
  if (normalized > statistics->green.maximum)
    statistics->green.maximum=normalized;
  if (normalized <  statistics->green.minimum)
    statistics->green.minimum=normalized;


  normalized=(double) pixel->blue/MaxRGB;
  statistics->blue.mean += normalized/context->samples;
  if (normalized > statistics->blue.maximum)
    statistics->blue.maximum=normalized;
  if (normalized <  statistics->blue.minimum)
    statistics->blue.minimum=normalized;

  if (image->matte)
    {
      normalized=(double) pixel->opacity/MaxRGB;
      statistics->opacity.mean += normalized/context->samples;
      if (normalized > statistics->opacity.maximum)
        statistics->opacity.maximum=normalized;
      if (normalized <  statistics->opacity.minimum)
        statistics->opacity.minimum=normalized;
    }
  return MagickPass;
}
#define Square(x)  ((x)*(x))
static MagickPassFail GetImageStatisticsVariance(void *user_context,
                                                 const long x,const long y,
                                                 const Image *image,
                                                 const PixelPacket *pixel,
                                                 ExceptionInfo *exception)
{
  StatisticsContext
    *context=(StatisticsContext *) user_context;

  ImageStatistics
    *statistics=context->statistics;

  double
    normalized;

  normalized=(double) pixel->red/MaxRGB;
  statistics->red.variance +=
    Square(normalized-statistics->red.mean)/context->variance_divisor;

  normalized=(double) pixel->green/MaxRGB;
  statistics->green.variance +=
    Square(normalized-statistics->green.mean)/context->variance_divisor;

  normalized=(double) pixel->blue/MaxRGB;
  statistics->blue.variance +=
    Square(normalized-statistics->blue.mean)/context->variance_divisor;

  if (image->matte)
    {
      normalized=(double) pixel->opacity/MaxRGB;
      statistics->opacity.variance +=
        Square(normalized-statistics->opacity.mean)/context->variance_divisor;
    }

  return MagickPass;
}
MagickExport MagickPassFail GetImageStatistics(const Image *image,
                                               ImageStatistics *statistics,
                                               ExceptionInfo *exception)
{
  StatisticsContext
    context;
  
  MagickPassFail
    status=MagickPass;

  double
    samples;
  
  memset((void *) statistics, 0, sizeof(ImageStatistics));
  statistics->red.minimum=1.0;
  statistics->green.minimum=1.0;
  statistics->blue.minimum=1.0;
  if (image->matte)
    statistics->opacity.minimum=1.0;

  samples=(double) image->rows*image->columns;
  context.statistics=statistics;
  context.samples=samples;
  context.variance_divisor=samples-1;
  
  /*
    Compute Mean, Max, and Min
  */
  status &= PixelIterateMonoRead(GetImageStatisticsMean,
                                 &context,0,0,image->columns,
                                 image->rows,image,exception);
  if (status != MagickPass)
    return (status);

  /*
    Compute Variance
  */
  status &= PixelIterateMonoRead(GetImageStatisticsVariance,
                                 &context,0,0,image->columns,
                                 image->rows,image,exception);
  if (status != MagickPass)
    return (status);

  /*
    Compute Standard Deviation
  */
  statistics->red.standard_deviation=sqrt(statistics->red.variance);
  statistics->green.standard_deviation=sqrt(statistics->green.variance);
  statistics->blue.standard_deviation=sqrt(statistics->blue.variance);
  if (image->matte)
    statistics->opacity.standard_deviation=sqrt(statistics->opacity.variance);

  return status;
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
+     G r a d i e n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GradientImage() applies a continuously smooth color transitions along a
%  vector from one color to another.
%
%  Note, the interface of this method will change in the future to support
%  more than one transistion.
%
%  The format of the GradientImage method is:
%
%      unsigned int GradientImage(Image *image,const PixelPacket *start_color,
%        const PixelPacket *stop_color)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o start_color: The start color.
%
%    o stop_color: The stop color.
%
%
*/

static inline PixelPacket BlendComposite(const PixelPacket *p,
  const PixelPacket *q,const double alpha)
{
  double
    color;

  PixelPacket
    composite;

  color=((double) p->red*(MaxRGB-alpha)+q->red*alpha)/MaxRGB;
  composite.red=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  color=((double) p->green*(MaxRGB-alpha)+q->green*alpha)/MaxRGB;
  composite.green=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  color=((double) p->blue*(MaxRGB-alpha)+q->blue*alpha)/MaxRGB;
  composite.blue=(Quantum)
    ((color < 0) ? 0 : (color > MaxRGB) ? MaxRGB : color+0.5);
  composite.opacity=p->opacity;
  return(composite);
}

MagickExport unsigned int GradientImage(Image *image,
  const PixelPacket *start_color,const PixelPacket *stop_color)
{
  long
    y;

  register long
    i,
    x;

  register PixelPacket
    *q;

  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(start_color != (const PixelPacket *) NULL);
  assert(stop_color != (const PixelPacket *) NULL);
  /*
    Generate gradient pixels.
  */
  i=0;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      *q=BlendComposite(start_color,stop_color,(double)
        MaxRGB*i/(image->columns*image->rows));
      q++;
      i++;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(LoadImageText,y,image->rows,&image->exception))
        break;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G r a y s c a l e P s e u d o C l a s s I m a g e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GrayscalePseudoClassImage converts an image to a PseudoClass
%  grayscale representation with an (optionally) compressed and sorted
%  colormap. Colormap is ordered by increasing intensity.
%
%  The format of the GrayscalePseudoClassImage method is:
%
%      void GrayscalePseudoClassImage(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o optimize_colormap: If true, produce an optimimal (compact) colormap.
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int IntensityCompare(const void *x,const void *y)
{
  long
    intensity;

  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  intensity=PixelIntensityToQuantum(color_1)-
    (long) PixelIntensityToQuantum(color_2);
  return(intensity);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport void GrayscalePseudoClassImage(Image *image,
  unsigned int optimize_colormap)
{
  long
    y;

  register long
    x;

  register IndexPacket
    *indexes;

  register const PixelPacket
    *q;

  register int
    i;

  int
    *colormap_index=(int *) NULL;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (!image->is_grayscale)
    TransformColorspace(image,GRAYColorspace);

  if (image->storage_class != PseudoClass)
    {
      /*
        Allocate maximum sized grayscale image colormap
      */
      if (!AllocateImageColormap(image,MaxColormapSize))
        {
          ThrowException3(&image->exception,ResourceLimitError,
            MemoryAllocationFailed,UnableToSortImageColormap);
          return;
        }

      if (optimize_colormap)
        {
          /*
            Use minimal colormap method.
          */

          /*
            Allocate memory for colormap index
          */
          colormap_index=MagickAllocateMemory(int *,MaxColormapSize*sizeof(int));
          if (colormap_index == (int *) NULL)
            {
              ThrowException3(&image->exception,ResourceLimitError,
                MemoryAllocationFailed,UnableToSortImageColormap);
              return;
            }

          /*
            Initial colormap index value is -1 so we can tell if it
            is initialized.
          */
          for (i=0; i < MaxColormapSize; i++)
            colormap_index[i]=-1;

          image->colors=0;
          for (y=0; y < (long) image->rows; y++)
            {
              q=GetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=(long) image->columns; x > 0; x--)
                {
                  register int
                    intensity;
                  
                  /*
                    If index is new, create index to colormap
                  */
                  intensity=ScaleQuantumToMap(q->red);
                  if (colormap_index[intensity] < 0)
                    {
                      colormap_index[intensity]=image->colors;
                      image->colormap[image->colors]=*q;
                      image->colors++;
                    }
                  *indexes++=colormap_index[intensity];
                  q++;
                }
              if (!SyncImagePixels(image))
                return;
            }
        }
      else
        {
          /*
            Use fast-cut linear colormap method.
          */
          for (y=0; y < (long) image->rows; y++)
            {
              q=GetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=(long) image->columns; x > 0; x--)
                {
                  *indexes=ScaleQuantumToIndex(q->red);
                  q++;
                  indexes++;
                } 
              if (!SyncImagePixels(image))
                break;
           }
          image->is_grayscale=True;
          return;
        }
    }

  if (optimize_colormap)
    {
      /*
        Sort and compact the colormap
      */

      /*
        Allocate memory for colormap index
      */
      if (colormap_index == (int *) NULL)
        {
          colormap_index=MagickAllocateMemory(int *,MaxColormapSize*sizeof(int));
          if (colormap_index == (int *) NULL)
            {
              ThrowException3(&image->exception,ResourceLimitError,
                MemoryAllocationFailed,UnableToSortImageColormap);
              return;
            }
        }
      
      /*
        Assign index values to colormap entries.
      */
      for (i=0; i < (long) image->colors; i++)
        image->colormap[i].opacity=(unsigned short) i;
      /*
        Sort image colormap by increasing intensity.
      */
      qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
            IntensityCompare);
      /*
        Create mapping between original indexes and reduced/sorted
        colormap.
      */
      {
        PixelPacket
          *new_colormap;

        int
          j;

        new_colormap=MagickAllocateMemory(PixelPacket *,image->colors*sizeof(PixelPacket));
        if (new_colormap == (PixelPacket *) NULL)
          {
            ThrowException3(&image->exception,ResourceLimitError,
              MemoryAllocationFailed,UnableToSortImageColormap);
            return;
          }

        j=0;
        new_colormap[j]=image->colormap[0];
        for (i=0; i < (long) image->colors; i++)
          {
            if (!ColorMatch(&new_colormap[j],&image->colormap[i]))
              {
                j++;
                new_colormap[j]=image->colormap[i];
              }
            
            colormap_index[image->colormap[i].opacity]=j;
          }
        image->colors=j+1;
        MagickFreeMemory(image->colormap);
        image->colormap=new_colormap;
      }

      /*
        Reassign image colormap indexes
      */
      for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=(long) image->columns; x > 0; x--)
            {
              *indexes=colormap_index[*indexes];
              indexes++;
            }
          if (!SyncImagePixels(image))
            break;
        }
      MagickFreeMemory(colormap_index);
    }
  image->is_grayscale=True;
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
%    o mean_error_per_pixel:  The mean error for any single pixel in
%      the image.
%
%    o normalized_mean_error:  The normalized mean quantization error for
%      any single pixel in the image.  This distance measure is normalized to
%      a range between 0 and 1.  It is independent of the range of red, green,
%      and blue values in the image.
%
%    o normalized_maximum_error:  The normalized maximum quantization
%      error for any single pixel in the image.  This distance measure is
%      normalized to a range between 0 and 1.  It is independent of the range
%      of red, green, and blue values in your image.
%
%  A small normalized mean square error, accessed as
%  image->normalized_mean_error, suggests the images are very similiar in
%  spatial layout and color.
%
%  The format of the IsImagesEqual method is:
%
%      unsigned int IsImagesEqual(Image *image,const Image *reference)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o reference: The reference image.
%
*/
typedef struct _PixelErrorStats {
  double
    maximum_error_per_pixel,
    total_error;
} PixelErrorStats;

static MagickPassFail
ComputePixelError(void *user_data,
                  const Image *first_image,
                  const long first_x,
                  const long first_y,
                  const PixelPacket *first_pixel,
                  const Image *second_image,
                  const long second_x,
                  const long second_y,
                  const PixelPacket *second_pixel,
                  ExceptionInfo *exception)
{
  PixelErrorStats
    *stats = (PixelErrorStats *) user_data;

  double
    difference,
    distance,
    distance_squared;

  difference=(first_pixel->red-(double) second_pixel->red)/MaxRGB;
  distance_squared=(difference*difference);
  difference=(first_pixel->green-(double) second_pixel->green)/MaxRGB;
  distance_squared+=(difference*difference);
  difference=(first_pixel->blue-(double) second_pixel->blue)/MaxRGB;
  distance_squared+=(difference*difference);
  if (first_image->matte)
    {
      difference=(first_pixel->opacity-(double) second_pixel->opacity)/MaxRGB;
      distance_squared+=(difference*difference);
    }
  distance=sqrt(distance_squared);
  stats->total_error+=distance;
  if (distance > stats->maximum_error_per_pixel)
    stats->maximum_error_per_pixel=distance;
  return (MagickPass);
}

MagickExport unsigned int IsImagesEqual(Image *image,const Image *reference)
{
  PixelErrorStats
    stats;

  double
    mean_error_per_pixel,
    normalize,
    number_pixels;

  /*
    Initialize measurement.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(reference != (const Image *) NULL);
  assert(reference->signature == MagickSignature);
  memset(&image->error,0,sizeof(ErrorInfo));
  if ((image->rows != reference->rows) ||
      (image->columns != reference->columns))
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageSizeDiffers);
  if (image->colorspace != reference->colorspace)
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageColorspaceDiffers);
  if(image->matte != reference->matte)
    ThrowBinaryException3(ImageError,UnableToCompareImages,
      ImageOpacityDiffers);

  /*
    For each pixel, collect error statistics.
  */
  number_pixels=(double) image->columns*image->rows;
  stats.maximum_error_per_pixel=0.0;
  stats.total_error=0.0;

  PixelIterateDualRead(ComputePixelError,(void *) &stats,
                       image->columns,image->rows,
                       image,0,0,
                       reference,0,0,
                       &image->exception);

  /*
    Compute final error statistics.
  */
  
  if (image->matte)
    normalize = sqrt(4.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0+1.0*1.0) */
  else
    normalize = sqrt(3.0); /* sqrt(1.0*1.0+1.0*1.0+1.0*1.0) */
  mean_error_per_pixel=stats.total_error/number_pixels;
  image->error.mean_error_per_pixel=mean_error_per_pixel*MaxRGB;
  image->error.normalized_mean_error=mean_error_per_pixel/normalize;
  image->error.normalized_maximum_error=stats.maximum_error_per_pixel/normalize;
  return(image->error.normalized_mean_error == 0.0);
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
  flags=GetGeometry((char *) geometry,&x,&y,&width,&height);
  if (pedantic)
    return((flags != NoValue) && !(flags & HeightValue));
  return(IsGeometry(geometry) && !(flags & HeightValue));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s T a i n t I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsTaintImage() returns a value other than 0 if any pixel in the image
%  has been altered since it was first constituted.
%
%  The format of the IsTaintImage method is:
%
%      unsigned int IsTaintImage(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned int IsTaintImage(const Image *image)
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
MagickExport void ModifyImage(Image **image,ExceptionInfo *exception)
{
  Image
    *clone_image;

  unsigned int
    clone;

  assert(image != (Image **) NULL);
  assert(*image != (Image *) NULL);
  assert((*image)->signature == MagickSignature);
  clone=False;
  AcquireSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  if ((*image)->reference_count > 1)
    clone=True;
  LiberateSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  if (!clone)
    return;
  clone_image=CloneImage(*image,0,0,True,exception);
  AcquireSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  (*image)->reference_count--;
  LiberateSemaphoreInfo((SemaphoreInfo **) &(*image)->semaphore);
  *image=clone_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     P l a s m a I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PlasmaImage() initializes an image with plasma fractal values.  The image
%  must be initialized with a base color and the random number generator
%  seeded before this method is called.
%
%  The format of the PlasmaImage method is:
%
%      unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
%        unsigned long attenuate,unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o segment:   Define the region to apply plasma fractals values.
%
%    o attenuate: Define the plasma attenuation factor.
%
%    o depth: Limit the plasma recursion depth.
%
%
*/

static inline Quantum PlasmaPixel(const double pixel,const double noise)
{
  double
    value;

  value=pixel+noise*rand()/RAND_MAX-noise/2;
  if (value <= 0.0)
    return(0);
  if (value >= MaxRGB)
    return(MaxRGB);
  return((Quantum) (value+0.5));
}

MagickExport unsigned int PlasmaImage(Image *image,const SegmentInfo *segment,
  unsigned long attenuate,unsigned long depth)
{
  double
    plasma;

  long
    x,
    x_mid,
    y,
    y_mid;

  PixelPacket
    u,
    v;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(segment != (SegmentInfo *) NULL);
  if (depth != 0)
    {
      SegmentInfo
        local_info;

      /*
        Divide the area into quadrants and recurse.
      */
      depth--;
      attenuate++;
      x_mid=(long) (segment->x1+segment->x2+0.5)/2;
      y_mid=(long) (segment->y1+segment->y2+0.5)/2;
      local_info=(*segment);
      local_info.x2=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.y1=y_mid;
      local_info.x2=x_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y2=y_mid;
      (void) PlasmaImage(image,&local_info,attenuate,depth);
      local_info=(*segment);
      local_info.x1=x_mid;
      local_info.y1=y_mid;
      return(PlasmaImage(image,&local_info,attenuate,depth));
    }
  if ((image->colorspace != RGBColorspace) ||
      (image->storage_class != DirectClass))
    {
      SetImageType(image,TrueColorType);
    }
  x_mid=(long) (segment->x1+segment->x2+0.5)/2;
  y_mid=(long) (segment->y1+segment->y2+0.5)/2;
  if ((segment->x1 == x_mid) && (segment->x2 == x_mid) &&
      (segment->y1 == y_mid) && (segment->y2 == y_mid))
    return(False);
  /*
    Average pixels and apply plasma.
  */
  plasma=MaxRGB/(2.0*attenuate);
  if ((segment->x1 != x_mid) || (segment->x2 != x_mid))
    {
      /*
        Left pixel.
      */
      x=(long) (segment->x1+0.5);
      u=GetOnePixel(image,x,(long) (segment->y1+0.5));
      v=GetOnePixel(image,x,(long) (segment->y2+0.5));
      q=SetImagePixels(image,x,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
      if (segment->x1 != segment->x2)
        {
          /*
            Right pixel.
          */
          x=(long) (segment->x2+0.5);
          u=GetOnePixel(image,x,(long) (segment->y1+0.5));
          v=GetOnePixel(image,x,(long) (segment->y2+0.5));
          q=SetImagePixels(image,x,y_mid,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->y1 != y_mid) || (segment->y2 != y_mid))
    {
      if ((segment->x1 != x_mid) || (segment->y2 != y_mid))
        {
          /*
            Bottom pixel.
          */
          y=(long) (segment->y2+0.5);
          u=GetOnePixel(image,(long) (segment->x1+0.5),y);
          v=GetOnePixel(image,(long) (segment->x2+0.5),y);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
      if (segment->y1 != segment->y2)
        {
          /*
            Top pixel.
          */
          y=(long) (segment->y1+0.5);
          u=GetOnePixel(image,(long) (segment->x1+0.5),y);
          v=GetOnePixel(image,(long) (segment->x2+0.5),y);
          q=SetImagePixels(image,x_mid,y,1,1);
          if (q == (PixelPacket *) NULL)
            return(True);
          q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
          q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
          q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
          (void) SyncImagePixels(image);
        }
    }
  if ((segment->x1 != segment->x2) || (segment->y1 != segment->y2))
    {
      /*
        Middle pixel.
      */
      x=(long) (segment->x1+0.5);
      y=(long) (segment->y1+0.5);
      u=GetOnePixel(image,x,y);
      x=(long) (segment->x2+0.5);
      y=(long) (segment->y2+0.5);
      v=GetOnePixel(image,x,y);
      q=SetImagePixels(image,x_mid,y_mid,1,1);
      if (q == (PixelPacket *) NULL)
        return(True);
      q->red=PlasmaPixel(((double) u.red+v.red)/2,plasma);
      q->green=PlasmaPixel(((double) u.green+v.green)/2,plasma);
      q->blue=PlasmaPixel(((double) u.blue+v.blue)/2,plasma);
      (void) SyncImagePixels(image);
    }
  if (((segment->x2-segment->x1) < 3.0) && ((segment->y2-segment->y1) < 3.0))
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorImage() performs the requested integer arithmetic
%  operation on the selected channels of the image.
%
%  The format of the QuantumOperatorImage method is:
%
%      MagickPassFail QuantumOperatorImage(Image *image,
%        ChannelType channel, QuantumOperator operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on.
%
%    o quantum_operator: arithmetic or bitwise operator to use (AddQuantumOp,
%                        AndQuantumOp, DivideQuantumOp, LShiftQuantumOp,
%                        MultiplyQuantumOp, OrQuantumOp, RShiftQuantumOp,
%                        SubtractQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/
MagickExport MagickPassFail QuantumOperatorImage(Image *image,
  const ChannelType channel,const QuantumOperator quantum_operator,
  const double rvalue,ExceptionInfo *exception)
{
  return QuantumOperatorRegionImage(image,0,0,image->columns,image->rows,
                                    channel,quantum_operator,rvalue,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u a n t u m O p e r a t o r R e g i o n I m a g e                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QuantumOperatorRegionImage() performs the requested integer arithmetic
%  operation on the selected channels of the image over the specified region.
%
%  The format of the QuantumOperatorRegionImage method is:
%
%      MagickPassFail QuantumOperatorRegionImage(Image *image,
%        long x, long y, unsigned long columns, unsigned long rows,
%        ChannelType channel, QuantumOperator quantum_operator,
%        double rvalue)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: Channel to operate on.
%
%    o x: Ordinate of left row of region.
%
%    o y: Orginate of top column of region.
%
%    o columns: Width of region.
%
%    o rows: Height of region.
%
%    o quantum_operator: arithmetic or bitwise operator to use (AddQuantumOp,
%                        AndQuantumOp, DivideQuantumOp, LShiftQuantumOp,
%                        MultiplyQuantumOp, OrQuantumOp, RShiftQuantumOp,
%                        SubtractQuantumOp, XorQuantumOp).
%
%    o rvalue: Operator argument.
%
%    o exception: Updated with error description.
%
*/

#define ApplyArithmeticOperator(lvalue,op,rvalue)       \
  {                                                     \
    double                                              \
      result;                                           \
                                                        \
    result=(double) lvalue op (double) rvalue;          \
    lvalue=RoundSignedToQuantum(result);                \
}

typedef struct _QuantumContext
{
  ChannelType channel;
  Quantum quantum_value;
  double double_value;
} QuantumContext;

static MagickPassFail
QuantumAdd(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,+,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,+,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,+,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,+,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,+,context->double_value);
      ApplyArithmeticOperator(pixel->green,+,context->double_value);
      ApplyArithmeticOperator(pixel->blue,+,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,+,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumAnd(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red &= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green &= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue &= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity &= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red &= context->quantum_value;
      pixel->green &= context->quantum_value;
      pixel->blue &= context->quantum_value;
      if (const_image->matte)
        pixel->opacity &= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumDivide(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,/,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,/,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,/,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,/,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,/,context->double_value);
      ApplyArithmeticOperator(pixel->green,/,context->double_value);
      ApplyArithmeticOperator(pixel->blue,/,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,/,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumLShift(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red <<= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green <<= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue <<= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity <<= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red <<= context->quantum_value;
      pixel->green <<= context->quantum_value;
      pixel->blue <<= context->quantum_value;
      if (const_image->matte)
        pixel->opacity <<= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumMultiply(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,*,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,*,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,*,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,*,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,*,context->double_value);
      ApplyArithmeticOperator(pixel->green,*,context->double_value);
      ApplyArithmeticOperator(pixel->blue,*,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,*,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumOr(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red |= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green |= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue |= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity |= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red |= context->quantum_value;
      pixel->green |= context->quantum_value;
      pixel->blue |= context->quantum_value;
      if (const_image->matte)
        pixel->opacity |= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumRShift(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red >>= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green >>= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue >>= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity >>= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red >>= context->quantum_value;
      pixel->green >>= context->quantum_value;
      pixel->blue >>= context->quantum_value;
      if (const_image->matte)
        pixel->opacity >>= context->quantum_value;
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumSubtract(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      ApplyArithmeticOperator(pixel->red,-,context->double_value);
      break;
    case GreenChannel:
    case MagentaChannel:
      ApplyArithmeticOperator(pixel->green,-,context->double_value);
      break;
    case BlueChannel:
    case YellowChannel:
      ApplyArithmeticOperator(pixel->blue,-,context->double_value);
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      ApplyArithmeticOperator(pixel->opacity,-,context->double_value);
      break;
    case UndefinedChannel:
    case AllChannels:
      ApplyArithmeticOperator(pixel->red,-,context->double_value);
      ApplyArithmeticOperator(pixel->green,-,context->double_value);
      ApplyArithmeticOperator(pixel->blue,-,context->double_value);
      if (const_image->matte)
        ApplyArithmeticOperator(pixel->opacity,-,context->double_value);
      break;
    }

  return (MagickPass);
}
static MagickPassFail
QuantumXor(void *user_data,const long x,const long y,
  const Image *const_image,PixelPacket *pixel,ExceptionInfo *exception)
{
  QuantumContext
    *context=(QuantumContext *) user_data;

  switch (context->channel)
    {
    case RedChannel:
    case CyanChannel:
      pixel->red ^= context->quantum_value;
      break;
    case GreenChannel:
    case MagentaChannel:
      pixel->green ^= context->quantum_value;
      break;
    case BlueChannel:
    case YellowChannel:
      pixel->blue ^= context->quantum_value;
      break;
    case BlackChannel:
    case MatteChannel:
    case OpacityChannel:
      pixel->opacity ^= context->quantum_value;
      break;
    case UndefinedChannel:
    case AllChannels:
      pixel->red ^= context->quantum_value;
      pixel->green ^= context->quantum_value;
      pixel->blue ^= context->quantum_value;
      if (const_image->matte)
        pixel->opacity ^= context->quantum_value;
      break;
    }
  return (MagickPass);
}
MagickExport MagickPassFail
QuantumOperatorRegionImage(Image *image,
                           const long x,const long y,
                           const unsigned long columns,
                           const unsigned long rows,
                           const ChannelType channel,
                           const QuantumOperator quantum_operator,
                           const double rvalue,
                           ExceptionInfo *exception)
{
  QuantumContext
    context;

  MagickPassFail
    status = MagickFail;

  PixelIteratorMonoModifyCallback
    call_back = 0;

  image->storage_class=DirectClass;

  context.channel=channel;
  context.double_value=rvalue;
  context.quantum_value=RoundSignedToQuantum(rvalue);

  switch (quantum_operator)
    {
    case UndefinedQuantumOp:
      break;
    case AddQuantumOp:
      call_back=QuantumAdd;
      break;
    case AndQuantumOp:
      call_back=QuantumAnd;
      break;
    case DivideQuantumOp:
      call_back=QuantumDivide;
      break;
    case LShiftQuantumOp:
      call_back=QuantumLShift;
      break;
    case MultiplyQuantumOp:
      call_back=QuantumMultiply;
      break;
    case OrQuantumOp:
      call_back=QuantumOr;
      break;
    case RShiftQuantumOp:
      call_back=QuantumRShift;
      break;
    case SubtractQuantumOp:
      call_back=QuantumSubtract;
      break;
    case XorQuantumOp:
      call_back=QuantumXor;
      break;
    }

  if (call_back)
    status=PixelIterateMonoModify(call_back,(void *) &context,x,y,columns,rows,
                                  image,exception);
  return (status);
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
  AcquireSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  image->reference_count++;
  LiberateSemaphoreInfo((SemaphoreInfo **) &image->semaphore);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%   R e m o v e D e f i n i t i o n s                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RemoveDefinitions() removes definitions from the current map of definitions
%  in ImageInfo. Definitions may be used by coders/decoders that read and
%  write images. RemoveDefinitions() returns true only if the specified keys
%  are present in the map and are actually removed.
%
%  The format of the RemoveDefinitions method is:
%
%      void RemoveDefinitions(ImageInfo *image_info,const char *options)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o keys: List of keys to remove from the definitions map. The
%      format of the string is "key1,key2,...". A special key, *, removes
%      all the key/value pairs in the definitions map. This key always
%      succeeds.
%
%    o exception: Errors result in updates to this structure.
%
*/
MagickExport unsigned int
RemoveDefinitions(const ImageInfo *image_info,const char *keys)
{
  char
    key[MaxTextExtent];

  unsigned int
    status;

  int
    i,
    j;

  size_t
    length;

  if (image_info->definitions == 0)
    return(False);

  status=True;

  /*
    TODO: update to accept GlobExpression as argument names list.
    That would require the iterator to know we're manipulating
    the map or a remove method on the map iterator itself.
    Until then we accept a simple "*" to mean clear the whole map.
  */
  length=strlen(keys);
  i=0;
  while (i < length)
  {
    for (j=0; (i < length) && (keys[i] != ','); i++,j++)
      key[j]=keys[i];
    key[j]='\0';
    i++;
    if (strlen(key) > 0)
      {
        if ((key[0] == '*') && (key[1] == '\0'))
          MagickMapClearMap(image_info->definitions);
        else
          status &= MagickMapRemoveEntry(image_info->definitions,key);
      }
    else
      {
        status=False;
        break;
      }
  }
  return(status);
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
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
%
*/

typedef struct _XYZColorTransformPacket
{
  int
    x,
    y,
    z;
} XYZColorTransformPacket;

static const size_t
  XYZMapAllocSize=(MaxMap+1)*sizeof(XYZColorTransformPacket);

static const char *ColorspaceTypeToString(const ColorspaceType colorspace)
{
  const char
    *log_colorspace = "Unknown";
  
  switch (colorspace)
    {
    case UndefinedColorspace:
      log_colorspace="Undefined";
      break;
    case RGBColorspace:
      log_colorspace="RGB";
      break;
    case GRAYColorspace:
      log_colorspace="GRAY";
      break;
    case TransparentColorspace:
      log_colorspace="Transparent";
      break;
    case OHTAColorspace:
      log_colorspace="OHTA";
      break;
    case XYZColorspace:
      log_colorspace="XYZ";
      break;
    case YCbCrColorspace:
      log_colorspace="YCbCr";
      break;
    case YCCColorspace:
      log_colorspace="PhotoCD YCC";
      break;
    case YIQColorspace:
      log_colorspace="YIQ";
      break;
    case YPbPrColorspace:
      log_colorspace="YPbPr";
      break;
    case YUVColorspace:
      log_colorspace="YUV";
      break;
    case CMYKColorspace:
      log_colorspace="CMYK";
      break;
    case sRGBColorspace:
      log_colorspace="PhotoCD sRGB";
      break;
    case HSLColorspace:
      log_colorspace="HSL";
      break;
    case HWBColorspace:
      log_colorspace="HWB";
      break;
    case LABColorspace:
      log_colorspace="LAB";
      break;
    }
  return log_colorspace;
}
static inline void XYZTransformPacket(PixelPacket *pixel,
  const XYZColorTransformPacket *x_map,
  const XYZColorTransformPacket *y_map,
  const XYZColorTransformPacket *z_map,
  const XYZColorTransformPacket *primary_info)
{
  register int
    blue,
    green,
    red;

  register const XYZColorTransformPacket
    *x_p,
    *y_p,
    *z_p;

  x_p = &x_map[ScaleQuantumToMap(pixel->red)];
  y_p = &y_map[ScaleQuantumToMap(pixel->green)];
  z_p = &z_map[ScaleQuantumToMap(pixel->blue)];

  red=(x_p->x+y_p->x+z_p->x+primary_info->x);
  if(red < 0) red = 0;
  if(red > MaxMap) red = MaxMap;
  pixel->red=ScaleMapToQuantum(red);

  green=(x_p->y+y_p->y+z_p->y+primary_info->y);
  if(green < 0) green = 0;
  if(green > MaxMap) green = MaxMap;
  pixel->green=ScaleMapToQuantum(green);

  blue=(x_p->z+y_p->z+z_p->z+primary_info->z);
  if(blue < 0) blue = 0;
  if(blue > MaxMap) blue = MaxMap;
  pixel->blue=ScaleMapToQuantum(blue);
}
MagickExport unsigned int RGBTransformImage(Image *image,
  const ColorspaceType colorspace)
{
#define RGBTransformImageText  "  Transform image colors...  "

  XYZColorTransformPacket
    *x_map,
    *y_map,
    *z_map;

  register XYZColorTransformPacket
    *x_p,
    *y_p,
    *z_p;

  long
    y;

  XYZColorTransformPacket
    primary_info;

  register long
    i,
    x;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /* Detect bogus request to convert to RGB */
  assert(colorspace != RGBColorspace);
  assert(colorspace != TransparentColorspace);
  assert(colorspace != UndefinedColorspace);

  /* if ((colorspace == RGBColorspace) || (colorspace == TransparentColorspace)) */
/*     return(True); */

  /*
    Ensure that image is an RGB-compatable colorspace prior to
    transforming to an alternate colorspace.
  */
  if ((image->colorspace != RGBColorspace) ||
      (image->colorspace != GRAYColorspace) ||
      (image->colorspace != TransparentColorspace))
      TransformRGBImage(image,image->colorspace);

  /*
    Log colorspace transform event
  */
  LogMagickEvent(TransformEvent,GetMagickModule(),
                 "Transform colorspace from RGB to %s",
                 ColorspaceTypeToString(colorspace));

  /*
    Store colorspace in image.
  */
  image->colorspace=colorspace;
  
  if (colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      Quantum
        black,
        cyan,
        magenta,
        yellow;

      register PixelPacket
        *q;

      /*
        Convert RGB to CMYK colorspace.
      */
      if (image->storage_class == PseudoClass)
        {
          SyncImage(image);
          image->storage_class=DirectClass;
        }
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=(long) image->columns; x > 0; x--)
        {
          cyan=(Quantum) (MaxRGB-q->red);
          magenta=(Quantum) (MaxRGB-q->green);
          yellow=(Quantum) (MaxRGB-q->blue);
          black=(cyan < magenta ? Min(cyan,yellow) : Min(magenta,yellow));
          q->red=cyan;
          q->green=magenta;
          q->blue=yellow;
          *indexes++=q->opacity;
          q->opacity=black;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Colorspace transform completed"); 
      return(True);
    }

  if (colorspace == HSLColorspace || colorspace == HWBColorspace)
    {
      void (*transform)(const Quantum,const Quantum,const Quantum,
        double *,double *,double *);

      switch (colorspace)
        {
        case HSLColorspace:
        default:
          transform=TransformHSL;
        case HWBColorspace:
          transform=TransformHWB;
        }

      switch (image->storage_class)
        {
        case DirectClass:
        default:
          {
            ExceptionInfo
              *exception;
            
            register PixelPacket
              *q;
            
            /*
              Convert DirectClass image.
            */
            exception=(&image->exception);
            for (y=0; y < (long) image->rows; y++)
              {
                q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=(long) image->columns; x > 0; x--)
                  {
                    double
                      p1, /* H */
                      p2, /* S or W */
                      p3; /* L or B */

                    (transform)(q->red,q->green,q->blue,&p1,&p2,&p3);
                    q->red=(Quantum) RndToInt(p1*MaxRGB);
                    q->green=(Quantum) RndToInt(p2*MaxRGB);
                    q->blue=(Quantum) RndToInt(p3*MaxRGB);
                    /* printf("Luminosity=%.07lf %u\n", p3, (unsigned int)q->blue); */
                    q++;
                  }
                if (!SyncImagePixels(image))
                  break;
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitor(RGBTransformImageText,y,image->rows,exception))
                    break;
              }
            break;
          }
        case PseudoClass:
          {
            /*
              Convert PseudoClass image.
            */
            register PixelPacket
              *q;
            
            q=image->colormap;
            for (i=(long) image->colors; i > 0; i--)
              {
                double
                  p1, /* H */
                  p2, /* S or W */
                  p3; /* L or B */
                
                (transform)(q->red,q->green,q->blue,&p1,&p2,&p3);
                q->red=(Quantum) RndToInt(p1*MaxRGB);
                q->green=(Quantum) RndToInt(p2*MaxRGB);
                q->blue=(Quantum) RndToInt(p3*MaxRGB);
                q++;
              }
            SyncImage(image);
            break;
          }
        }
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Colorspace transform completed"); 
      return(True);
    }

  if ((colorspace == GRAYColorspace) && IsGrayImage(image,&image->exception))
    {
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Image colorspace was already GRAY");
      return(True);
    }
  /*
    Allocate the tables.
  */
  x_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  y_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  z_map=MagickAllocateMemory(XYZColorTransformPacket *,XYZMapAllocSize);
  if ((x_map == 0) || (y_map == 0) || (z_map == 0))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToTransformColorspace);
  primary_info.x=primary_info.y=primary_info.z=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=x_p->y=x_p->z=RndToInt(0.299*i);
        y_p->x=y_p->y=y_p->z=RndToInt(0.587*i);
        z_p->x=z_p->y=z_p->z=RndToInt(0.114*i);
        ++x_p;
        ++y_p;
        ++z_p;
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
      primary_info.y=RndToInt((MaxMap+1)/2);
      primary_info.z=RndToInt((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.33333*i);
        y_p->x=RndToInt(0.33334*i);
        z_p->x=RndToInt(0.33333*i);
        x_p->y=RndToInt(0.5*i);
        y_p->y=RndToInt(0.0);
        z_p->y=RndToInt((-0.5)*i);
        x_p->z=RndToInt((-0.25)*i);
        y_p->z=RndToInt(0.5*i);
        z_p->z=RndToInt((-0.25)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case sRGBColorspace:
    {
      /*
        Kodak PhotoYCC Color Space

        Initialize sRGB tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        sRGB is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      /* FIXME! The scaling factors for this transform look bizarre,
         and in fact, the results are not correct. */
      primary_info.y=RndToInt(ScaleCharToMap(156));
      primary_info.z=RndToInt(ScaleCharToMap(137));
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) (0.018*MaxMap); i++)
      {
        x_p->x=RndToInt(0.003962014134275617*i);
        y_p->x=RndToInt(0.007778268551236748*i);
        z_p->x=RndToInt(0.001510600706713781*i);
        x_p->y=RndToInt((-0.002426619775463276)*i);
        y_p->y=RndToInt((-0.004763965913702149)*i);
        z_p->y=RndToInt(0.007190585689165425*i);
        x_p->z=RndToInt(0.006927257754597858*i);
        y_p->z=RndToInt((-0.005800713697502058)*i);
        z_p->z=RndToInt((-0.0011265440570958)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      for ( ; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.2201118963486454*(1.099*i-0.099));
        y_p->x=RndToInt(0.4321260306242638*(1.099*i-0.099));
        z_p->x=RndToInt(0.08392226148409894*(1.099*i-0.099));
        x_p->y=RndToInt((-0.1348122097479598)*(1.099*i-0.099));
        y_p->y=RndToInt((-0.2646647729834528)*(1.099*i-0.099));
        z_p->y=RndToInt(0.3994769827314126*(1.099*i-0.099));
        x_p->z=RndToInt(0.3848476530332144*(1.099*i-0.099));
        y_p->z=RndToInt((-0.3222618720834477)*(1.099*i-0.099));
        z_p->z=RndToInt((-0.06258578094976668)*(1.099*i-0.099));
        ++x_p;
        ++y_p;
        ++z_p;
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
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.412453*i);
        y_p->x=RndToInt(0.35758*i);
        z_p->x=RndToInt(0.180423*i);
        x_p->y=RndToInt(0.212671*i);
        y_p->y=RndToInt(0.71516*i);
        z_p->y=RndToInt(0.072169*i);
        x_p->z=RndToInt(0.019334*i);
        y_p->z=RndToInt(0.119193*i);
        z_p->z=RndToInt(0.950227*i);
        ++x_p;
        ++y_p;
        ++z_p;
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
      primary_info.y=RndToInt((MaxMap+1)/2);
      primary_info.z=RndToInt((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.299*i);
        y_p->x=RndToInt(0.587*i);
        z_p->x=RndToInt(0.114*i);
        x_p->y=RndToInt((-0.16873)*i);
        y_p->y=RndToInt((-0.331264)*i);
        z_p->y=RndToInt(0.500000*i);
        x_p->z=RndToInt(0.500000*i);
        y_p->z=RndToInt((-0.418688)*i);
        z_p->z=RndToInt((-0.081316)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Kodak PhotoYCC Color Space.

        Initialize YCC tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          C1= -0.29900*R-0.58700*G+0.88600*B
          C2=  0.70100*R-0.58700*G-0.11400*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      /* FIXME! The scaling factors for this transform look bizarre,
         and in fact, the results are not correct. */
      primary_info.y=RndToInt(ScaleCharToMap(156));
      primary_info.z=RndToInt(ScaleCharToMap(137));
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= (long) (0.018*MaxMap); i++)
      {
        x_p->x=RndToInt(0.003962014134275617*i);
        y_p->x=RndToInt(0.007778268551236748*i);
        z_p->x=RndToInt(0.001510600706713781*i);
        x_p->y=RndToInt((-0.002426619775463276)*i);
        y_p->y=RndToInt((-0.004763965913702149)*i);
        z_p->y=RndToInt(0.007190585689165425*i);
        x_p->z=RndToInt(0.006927257754597858*i);
        y_p->z=RndToInt((-0.005800713697502058)*i);
        z_p->z=RndToInt((-0.0011265440570958)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      for ( ; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.2201118963486454*(1.099*i-0.099));
        y_p->x=RndToInt(0.4321260306242638*(1.099*i-0.099));
        z_p->x=RndToInt(0.08392226148409894*(1.099*i-0.099));
        x_p->y=RndToInt((-0.1348122097479598)*(1.099*i-0.099));
        y_p->y=RndToInt((-0.2646647729834528)*(1.099*i-0.099));
        z_p->y=RndToInt(0.3994769827314126*(1.099*i-0.099));
        x_p->z=RndToInt(0.3848476530332144*(1.099*i-0.099));
        y_p->z=RndToInt((-0.3222618720834477)*(1.099*i-0.099));
        z_p->z=RndToInt((-0.06258578094976668)*(1.099*i-0.099));
        ++x_p;
        ++y_p;
        ++z_p;
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
      primary_info.y=RndToInt((MaxMap+1)/2);
      primary_info.z=RndToInt((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.299*i);
        y_p->x=RndToInt(0.587*i);
        z_p->x=RndToInt(0.114*i);
        x_p->y=RndToInt(0.596*i);
        y_p->y=RndToInt((-0.274)*i);
        z_p->y=RndToInt((-0.322)*i);
        x_p->z=RndToInt(0.211*i);
        y_p->z=RndToInt((-0.523)*i);
        z_p->z=RndToInt(0.312*i);
        ++x_p;
        ++y_p;
        ++z_p;
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
      primary_info.y=RndToInt((MaxMap+1)/2);
      primary_info.z=RndToInt((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.299*i);
        y_p->x=RndToInt(0.587*i);
        z_p->x=RndToInt(0.114*i);
        x_p->y=RndToInt((-0.168736)*i);
        y_p->y=RndToInt((-0.331264)*i);
        z_p->y=RndToInt(0.5*i);
        x_p->z=RndToInt(0.5*i);
        y_p->z=RndToInt((-0.418688)*i);
        z_p->z=RndToInt((-0.081312)*i);
        ++x_p;
        ++y_p;
        ++z_p;
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
      primary_info.y=RndToInt((MaxMap+1)/2);
      primary_info.z=RndToInt((MaxMap+1)/2);
      x_p = x_map;
      y_p = y_map;
      z_p = z_map;
      for (i=0; i <= MaxMap; i++)
      {
        x_p->x=RndToInt(0.299*i);
        y_p->x=RndToInt(0.587*i);
        z_p->x=RndToInt(0.114*i);
        x_p->y=RndToInt((-0.1474)*i);
        y_p->y=RndToInt((-0.2895)*i);
        z_p->y=RndToInt(0.4369*i);
        x_p->z=RndToInt(0.615*i);
        y_p->z=RndToInt((-0.515)*i);
        z_p->z=RndToInt((-0.1)*i);
        ++x_p;
        ++y_p;
        ++z_p;
      }
      break;
    }
  }

#if 0
  /*
    Dump tables
  */
  for (i=0; i <= MaxMap; i++)
    {
      printf("%5ld: x_map(%7d,%7d,%7d) y_map(%7d,%7d,%7d) z_map(%7d,%7d,%7d)\n",
             i,
             ((x_map[i].x)),
             ((x_map[i].y)),
             ((x_map[i].z)),
             
             ((y_map[i].x)),
             ((y_map[i].y)),
             ((y_map[i].z)),
             
             ((z_map[i].x)),
             ((z_map[i].y)),
             ((z_map[i].z)));
    }
#endif

  /*
    Convert from RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      register PixelPacket
        *q;

       /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=(long) image->columns; x > 0; x--)
        {
          XYZTransformPacket(q,x_map,y_map,z_map,&primary_info);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(RGBTransformImageText,y,image->rows,exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      register PixelPacket
        *q;

      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
      {
        XYZTransformPacket(q,x_map,y_map,z_map,&primary_info);
        q++;
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocate memory.
  */
  MagickFreeMemory(z_map);
  MagickFreeMemory(y_map);
  MagickFreeMemory(x_map);

  if (colorspace==GRAYColorspace)
    image->is_grayscale=True;
  LogMagickEvent(TransformEvent,GetMagickModule(),
                 "Colorspace transform completed"); 
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
    {
      image->matte=True;
      image->colorspace=RGBColorspace;
      image->storage_class=DirectClass;
    }

  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;

      /*
        Set DirectClass pixels
      */
      for (x=(long) image->columns; x > 0; x--)
        *q++=background_color;
      
      if ((image->storage_class == PseudoClass) ||
          (image->colorspace == CMYKColorspace))
        {
          /*
            Set PseudoClass pixel indexes.
          */
          indexes=GetIndexes(image);
          for (x=(long) image->columns; x > 0; x--)
            *indexes++=0;
        }
      if (!SyncImagePixels(image))
        break;
    }
  image->is_grayscale=IsGray(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e C h a n n e l D e p t h                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageChannelDepth() translates the pixel quantums in the specified
%  channel so that if they are later divided to fit within the specified bit
%  depth, that no additional information is lost (i.e. no remainder resulting
%  from the division). Note that any subsequent image processing is likely
%  to increase the effective depth of the image channels. A non-zero
%  value is returned if the operation is successful. Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the SetImageChannelDepth method is:
%
%      unsigned int GetImageChannelDepth(const Image *image,
%                    const ChannelType channel,const unsigned int depth)
%
%  A description of each parameter follows:
%
%    o image: The image to update.
%
%    o channel: Channel to modify.
%
%    o depth: Desired channel depth (range 1 to QuantumDepth)
%
%
*/
#define SET_CHANNEL_DEPTH(image,desired_depth,channel,parameter,status) \
{ \
  long \
    y; \
\
  register long \
    x; \
\
  register unsigned long \
    scale; \
\
  register PixelPacket \
    *q; \
\
  status=True; \
\
  scale=MaxRGB / (MaxRGB >> (QuantumDepth-desired_depth)); \
  for (y=0; y < (long) image->rows; y++) \
    { \
      q=GetImagePixels(image,0,y,image->columns,1); \
      if (q == (PixelPacket *) NULL) \
        { \
          status=False; \
          break; \
        } \
      for (x=0; x < (long) image->columns; x++) \
        { \
          parameter=scale*((parameter)/scale); \
          q++; \
        } \
      if (!SyncImagePixels(image)) \
        { \
          status=False; \
          break; \
        } \
    } \
  if (image->storage_class == PseudoClass) \
    { \
      register long \
        i; \
\
      q=image->colormap; \
      for (i=0; i < (long) image->colors; i++) \
        { \
          parameter=scale*((parameter)/scale); \
          q++; \
        } \
    } \
}

MagickExport unsigned int SetImageChannelDepth(Image *image,
  const ChannelType channel, const unsigned int depth)
{
  unsigned long
    current_depth,
    desired_depth;

  unsigned int
    is_grayscale,
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  status=True;
  is_grayscale=image->is_grayscale;

  desired_depth=depth;
  if (desired_depth > QuantumDepth)
    desired_depth=QuantumDepth;

  current_depth=GetImageChannelDepth(image,channel,&image->exception);

  if (current_depth > desired_depth)
    {
      switch (channel)
        {
        case RedChannel:
        case CyanChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->red,status);
            break;
          }
        case GreenChannel:
        case MagentaChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->green,status);
            break;
          }
        case BlueChannel:
        case YellowChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->blue,status);
            break;
          }
        case OpacityChannel:
          {
            if (image->colorspace == CMYKColorspace)
              {
                long
                  y;
                
                register IndexPacket
                  *indexes;
                
                register long
                  x;
                
                register PixelPacket
                  *q;

                register unsigned long
                  scale;

                scale=MaxRGB / (MaxRGB >> (QuantumDepth-desired_depth));
                for (y=0; y < (long) image->rows; y++)
                  {
                    q=GetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      break;
                    indexes=GetIndexes(image);
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        *indexes=scale*((*indexes)/scale);
                        indexes++;
                      }
                    if (!SyncImagePixels(image))
                      break;
                  }
              }
            else
              {
                SET_CHANNEL_DEPTH(image,desired_depth,channel,q->opacity,status);
              }
            break;
          }
        case BlackChannel:
        case MatteChannel:
          {
            SET_CHANNEL_DEPTH(image,desired_depth,channel,q->opacity,status);
            break;
          }
        default:
          {
          }
        }
    }

  image->is_grayscale=is_grayscale;

  return status;
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
%  must be the same dimensions as the image.  Set any pixel component of
%  the clip mask to TransparentOpacity to prevent that corresponding image
%  pixel component from being updated when SyncImagePixels() is applied.
%
%  The clip mask protects the DirectClass pixels and PseudoClass pixel indexes
%  from modification. The clip mask does *not* protect the image colormap since
%  the image colormap is globally shared by all pixels in a PseudoClass image.
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
      ThrowBinaryException3(ImageError,UnableToSetClipMask,ImageSizeDiffers);
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
%  SetImageDepth() translates the pixel quantums across all of the channels
%  so that if they are later divided to fit within the specified bit
%  depth, that no additional information is lost (i.e. no remainder will
%  result from the division).  Note that any subsequent image processing is
%  likely to increase the effective depth of the image channels. A non-zero
%  value is returned if the operation is successful. Check the exception
%  member of image to determine the cause for any failure.
%
%  The format of the SetImageDepth method is:
%
%      unsigned int SetImageDepth(Image *image,const unsigned long depth)
%
%  A description of each parameter follows:
%
%    o image: The image to update.
%
%    o depth: Desired image depth (range 1 to QuantumDepth)
%
%
*/
MagickExport unsigned int SetImageDepth(Image *image,const unsigned long depth)
{
  long
    y;

  unsigned long
    current_depth,
    desired_depth;

  register long
    x;

  register unsigned long
    scale;

  unsigned int
    is_grayscale,
    status;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  status=True;

  desired_depth=depth;
  if (desired_depth > QuantumDepth)
    desired_depth=QuantumDepth;

  current_depth=GetImageDepth(image,&image->exception);

  if (current_depth > desired_depth)
    {
      scale=MaxRGB / (MaxRGB >> (QuantumDepth-desired_depth));

      for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=False;
              break;
            }
          for (x=0; x < (long) image->columns; x++)
            {
              q->red=scale*(q->red/scale);
              q->green=scale*(q->green/scale);
              q->blue=scale*(q->blue/scale);
              q->opacity=scale*(q->opacity/scale);
              q++;
            }
          if (!SyncImagePixels(image))
            {
              status=False;
              break;
            }
        }

      /*
        Note that this approach may lead to multiple occurances of the
        same color in the colormap.
      */
      if (image->storage_class == PseudoClass)
        {
          register long
            i;
      
          q=image->colormap;
          for (i=0; i < (long) image->colors; i++)
            {
              q->red=scale*(q->red/scale);
              q->green=scale*(q->green/scale);
              q->blue=scale*(q->blue/scale);
              q->opacity=scale*(q->opacity/scale);
              q++;
            }
        }
    }

  image->depth=desired_depth;

  /*
    Ensure that image->depth is 8/16/32 until such time that the rest
    of the code handles arbitrary depth.
  */
  if (image->depth < 8)
    image->depth=8;
  else if (image->depth > 8 && image->depth < 16)
    image->depth=16;
  else if (image->depth > 16 && image->depth < 32)
    image->depth=32;

  image->is_grayscale=is_grayscale;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetImageInfo() initializes the `magick' field of the ImageInfo structure.
%  It is set to a type of image format based on the prefix or suffix of the
%  filename.  For example, `ps:image' returns PS indicating a Postscript image.
%  JPEG is returned for this filename: `image.jpg'.  The filename prefix has
%  precendence over the suffix.  Use an optional index enclosed in brackets
%  after a file name to specify a desired subimage of a multi-resolution image
%  format like Photo CD (e.g. img0001.pcd[4]).  A True (non-zero) return value
%  indicates success.
%
%  The format of the SetImageInfo method is:
%
%      unsigned int SetImageInfo(ImageInfo *image_info,
%        const unsigned int rectify,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o rectify: an unsigned value other than zero rectifies the attribute for
%      multi-frame support (user may want multi-frame but image format may not
%      support it).
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static inline unsigned int IsFrame(const char *point)
{
  char
    *p;

  (void) strtol(point,&p,10);
  return(p != point);
}

MagickExport unsigned int SetImageInfo(ImageInfo *image_info,
  const unsigned int rectify,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    magic[MaxTextExtent],
    *q;

  const MagicInfo
    *magic_info;

  Image
    *image;

  register char
    *p;

  size_t
    magick_length;

  unsigned char
    magick[2*MaxTextExtent];

  unsigned int
    status;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  *magic='\0';
  p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
  if (*p == ']')
    {
      /*
        Look for sub-image specification (e.g. img0001.pcd[4]).
      */
      for (q=p; (q > image_info->filename) && (*q != '['); q--)
        ;

      if ((q > image_info->filename) && (*q == '[') && IsFrame(q+1))
        {
          unsigned long
            first,
            last;

          (void) CloneString(&image_info->tile,q+1);
          /* Copy image range spec. to tile spec. w/o brackets */
          image_info->tile[p-q-1]='\0';
          /* Cut image range spec. from filename */
          *q='\0';
          image_info->subimage=atol(image_info->tile);
          image_info->subrange=image_info->subimage;

          /* Parse the image range spec. now placed in tile */
          for (q=image_info->tile; *q != '\0'; )
          {
            while (isspace((int)(unsigned char) *q) || (*q == ','))
              q++;
            first=strtol(q,&q,10);
            last=first;
            while (isspace((int)(unsigned char) *q))
              q++;
            if (*q == '-')
              last=strtol(q+1,&q,10);
            if (first > last)
              Swap(first,last);
            if (first < image_info->subimage)
              image_info->subimage=first;
            if (last > image_info->subrange)
              image_info->subrange=last;
          }
          image_info->subrange-=image_info->subimage-1;
        }

      /* Restore p to end of modified filename */
      p=image_info->filename+Max((long) strlen(image_info->filename)-1,0);
    }
  while ((*p != '.') && (p > (image_info->filename+1)))
    p--;
  if ((LocaleCompare(p,".gz") == 0) || (LocaleCompare(p,".Z") == 0) ||
      (LocaleCompare(p,".bz2") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > (image_info->filename+1)));
  if ((*p == '.') && (strlen(p) < (long) sizeof(magic)))
    {
      /*
        User specified image format.
      */
      (void) strncpy(magic,p+1,MaxTextExtent-1);
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
        (void) strncpy(image_info->magick,magic,MaxTextExtent-1);
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  image_info->affirm=False;
  p=image_info->filename;
  while (isalnum((int) *p))
    p++;
#if defined(vms)
  if (*(p+1) == ':')
    p+=2;  /* skip DECnet node spec */
#endif
  if ((*p == ':') && ((p-image_info->filename) < (long) sizeof(magic)))
    {
      char
        format[MaxTextExtent];

      /*
        User specified image format.
      */
      (void) strncpy(format,image_info->filename,p-image_info->filename);
      format[p-image_info->filename]='\0';

      /*
        Backward compatability and interoperability namimg
      */
      if (LocaleCompare(format,"GRADATION") == 0)
        (void) strcpy(format,"GRADIENT");

      if (LocaleCompare(format,"MAGICK") == 0)
        (void) strcpy(format,"IMAGE");

      LocaleUpper(format);
      if (!IsMagickConflict(format))
        {
          /*
            Strip off image format prefix.
          */
          char base_filename[MaxTextExtent];
          p++;
          (void) strncpy(base_filename,p,MaxTextExtent-1);
          (void) strcpy(image_info->filename,base_filename);
          (void) strncpy(magic,format,MaxTextExtent-1);
          (void) strncpy(image_info->magick,magic,MaxTextExtent-1);
          if (LocaleCompare(magic,"TMP") != 0)
            image_info->affirm=True;
          else
            /* input file will be automatically removed */
            image_info->temporary=True;
        }
    }
  if (rectify)
    {
      const MagickInfo
        *magick_info;

      /*
        Rectify multi-image file support.
      */
      FormatString(filename,image_info->filename,0);
      if ((LocaleCompare(filename,image_info->filename) != 0) &&
          (strchr(filename,'%') == (char *) NULL))
        image_info->adjoin=False;
      magick_info=GetMagickInfo(magic,exception);
      if (magick_info != (const MagickInfo *) NULL)
        image_info->adjoin&=magick_info->adjoin;
      return(True);
    }
  if (image_info->affirm)
    return(True);
  /*
    Determine the image format from the first few bytes of the file.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return(False);
  (void) strncpy(image->filename,image_info->filename,MaxTextExtent-1);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      DestroyImage(image);
      return(False);
    }
  if (!BlobIsSeekable(image))
    {
      /*
        Copy standard input or pipe to temporary file.
      */
      if(!AcquireTemporaryFileName(filename))
        {
          CloseBlob(image);
          DestroyImage(image);
          return(False);
        }
      (void) ImageToFile(image,filename,exception);
      CloseBlob(image);
      (void) strcpy(image->filename,filename);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == False)
        {
          DestroyImage(image);
          return(False);
        }
      (void) strcpy(image_info->filename,filename);
      image_info->temporary=True;
    }
  magick[0]='\0';
  magick_length = ReadBlob(image,2*MaxTextExtent,magick);
  CloseBlob(image);
  DestroyImage(image);
  /*
    Check magic.mgk configuration file.
  */
  magic_info=GetMagicInfo(magick,magick_length,exception);
  if ((magic_info != (const MagicInfo *) NULL) &&
      (magic_info->name != (char *) NULL) &&
      (exception->severity == UndefinedException))
    {
      (void) strncpy(image_info->magick,magic_info->name,MaxTextExtent-1);
      return(True);
    }
  return(False);
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
%  Otherwise, the pixel opacity values are blended with the supplied
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

  unsigned int
    is_grayscale;

  register PixelPacket
    *q;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  is_grayscale=image->is_grayscale;
  image->storage_class=DirectClass;
  if (image->matte && (opacity != OpaqueOpacity) && (opacity != TransparentOpacity))
    {
      /*
        Attenuate existing opacity channel
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);

        if (image->colorspace == CMYKColorspace)
          {
            for (x=(long) image->columns; x > 0; --x)
              {
              *indexes=(IndexPacket)
                ((unsigned long) (opacity*(*indexes))/MaxRGB);
              indexes++;
              }
          }
        else
          {
            for (x=(long) image->columns; x > 0; --x)
              {
                q->opacity=(Quantum)
                  ((unsigned long) (opacity*q->opacity)/MaxRGB);
                q++;
              }
          }
        if (!SyncImagePixels(image))
          break;
      }
      image->is_grayscale=is_grayscale;
      return;
    }
  /*
    Add new opacity channel or make existing opacity channel opaque
  */
  image->matte=True;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if (image->colorspace == CMYKColorspace)
      {
        for (x=(long) image->columns; x > 0; --x)
          *indexes++=opacity;
      }
    else
      {
        for (x=(long) image->columns; x > 0; --x)
          {
            q->opacity=opacity;
            q++;
          }
      }
    if (!SyncImagePixels(image))
      break;
  }
  image->is_grayscale=is_grayscale;
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
%        OptimizeType
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
MagickExport unsigned int SetImageType(Image *image,const ImageType image_type)
{
  QuantizeInfo
    quantize_info;

  unsigned int
    status = True;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  switch (image_type)
  {
    case BilevelType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      if ((image->storage_class != PseudoClass) ||
          !IsMonochromeImage(image,&image->exception))
        {
          if (image->dither == True)
            {
              /*
                Dither image to bilevel
              */
              GetQuantizeInfo(&quantize_info);
              quantize_info.colorspace=GRAYColorspace;
              quantize_info.dither=image->dither;
              quantize_info.tree_depth=8;
              quantize_info.number_colors=2;
              (void) QuantizeImage(&quantize_info,image);
            }
          else
            {
              /*
                Normalize and Threshold image to bilevel
              */
              NormalizeImage(image); 
              ThresholdImage(image,(double)MaxRGB/2);
            }
        }
      image->is_grayscale=True;
      image->is_monochrome=True;
      break;
    }
    case GrayscaleType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      if (!IsGrayImage(image,&image->exception))
        TransformColorspace(image,GRAYColorspace);
      image->is_grayscale=True;
      break;
    }
    case GrayscaleMatteType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      if (!IsGrayImage(image,&image->exception))
          TransformColorspace(image,GRAYColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      image->is_grayscale=True;
      break;
    }
    case PaletteType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      if (image->storage_class != PseudoClass)
        {
          GetQuantizeInfo(&quantize_info);
          quantize_info.dither=image->dither;
          (void) QuantizeImage(&quantize_info,image);
        }
      break;
    }
    case PaletteMatteType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      if (image->storage_class != PseudoClass)
        {
          GetQuantizeInfo(&quantize_info);
          quantize_info.colorspace=TransparentColorspace;
          quantize_info.dither=image->dither;
          (void) QuantizeImage(&quantize_info,image);
        }
      break;
    }
    case TrueColorType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      image->storage_class=DirectClass;
      break;
    }
    case TrueColorMatteType:
    {
      if (image->colorspace != RGBColorspace)
        TransformColorspace(image,RGBColorspace);
      image->storage_class=DirectClass;
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      break;
    }
    case ColorSeparationType:
    {
      TransformColorspace(image,CMYKColorspace);
      break;
    }
    case ColorSeparationMatteType:
    {
      TransformColorspace(image,CMYKColorspace);
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
      break;
    }
    case OptimizeType:
    default:
      break;
  }
  return (status);
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
%    o image: A pointer to an Image structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int InverseIntensityCompare(const void *x,const void *y)
{
  long
    intensity;

  PixelPacket
    *color_1,
    *color_2;

  color_1=(PixelPacket *) x;
  color_2=(PixelPacket *) y;
  /*
    y - x results in decreasing order
  */
  intensity=PixelIntensityToQuantum(color_2)-
    (long) PixelIntensityToQuantum(color_1);
  return(intensity);
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

  unsigned int
    is_grayscale;

  unsigned short
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class != PseudoClass)
    return(True);
  is_grayscale=image->is_grayscale;
  /*
    Allocate memory for pixel indexes.
  */
  pixels=MagickAllocateMemory(unsigned short *,image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToSortImageColormap);
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < (long) image->colors; i++)
    image->colormap[i].opacity=(unsigned short) i;
  /*
    Sort image colormap by decreasing intensity.
  */
  qsort((void *) image->colormap,image->colors,sizeof(PixelPacket),
    InverseIntensityCompare);
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
  MagickFreeMemory(pixels);
  image->is_grayscale=is_grayscale;
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
  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register const PixelPacket
    *p;

  unsigned int
    is_grayscale;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->storage_class == DirectClass)
    return;
  is_grayscale=image->is_grayscale;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=(long) image->columns; x > 0; x--)
    {
      VerifyColormapIndex(image,*indexes);
      p=&image->colormap[*indexes++];
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  image->is_grayscale=is_grayscale;
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
%  canvas. False is returned if an error is encountered.
%
%  The format of the TextureImage method is:
%
%      unsigned int TextureImage(Image *image,const Image *texture)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o texture: This image is the texture to layer on the background.
%
%
*/

MagickExport unsigned int TextureImage(Image *image,const Image *texture)
{
#define TextureImageText  "  Apply image texture...  "

  const PixelPacket
    *pixels;

  long
    x,
    y;

  register long
    z;

  register const PixelPacket
    *p;

  register PixelPacket
    *q;

  unsigned int
    status;

  unsigned long
    width;

  /*
    Tile texture onto the image background.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=False;
  if (texture == (const Image *) NULL)
    return False;
  image->storage_class=DirectClass;
  status=True;
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(texture,0,y % texture->rows,texture->columns,1,
      &image->exception);
    q=GetImagePixels(image,0,y,image->columns,1);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=False;
        break;
      }
    pixels=p;
    for (x=0; x < (long) image->columns; x+=texture->columns)
    {
      width=texture->columns;
      if ((x+width) > image->columns)
        width=image->columns-x;
      p=pixels;
      for (z=(long) width; z != 0; z--)
      {
        *q=(*p);
        p++;
        q++;
      }
    }
    if (!SyncImagePixels(image))
      {
        status=False;
        break;
      }
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(TextureImageText,y,image->rows,&image->exception))
        {
          status=False;
          break;
        }
  }
  image->is_grayscale=texture->is_grayscale;
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     T r a n s f o r m C o l o r s p a c e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TransformColorspace converts the image to a specified colorspace.
%  If the image is already in the requested colorspace, no work is performed.
%  Note that the current colorspace is stored in the image colorspace member.
%  The transformation matrices are not necessarily the standard ones: the
%  weights are rescaled to normalize the range of the transformed values to
%  be [0..MaxRGB].
%
%  The format of the TransformColorspace method is:
%
%      unsigned int TransformColorspace(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image to transform
%
%    o colorspace: the desired colorspace.
%
*/
MagickExport unsigned int TransformColorspace(Image *image,
  const ColorspaceType colorspace)
{
  unsigned int
    status = True;
  assert(image != (Image *) NULL);
  assert(colorspace != UndefinedColorspace);
  assert(image->colorspace != UndefinedColorspace);

  /*
    If the image colorspace is the same as requested, do nothing.
  */
  if (image->colorspace == colorspace)
     return (status);

  /*
    If the requested colorspace is RGB or Transparent, then convert
    via TransformRGBImage.
  */
  if ((colorspace == RGBColorspace) ||
      (colorspace == TransparentColorspace))
      {
        (void) TransformRGBImage(image,image->colorspace);
        return  (status);
      }

  /*
    If the image is not already in an RGB-compatible colorspace, then
    convert it to RGB via TransformRGBImage, and then to the target
    colorspace via RGBTransformImage, otherwise just convert to the
    target colorspace via RGBTransformImage.
  */
  if ((image->colorspace != RGBColorspace) &&
      (image->colorspace != TransparentColorspace) &&
      (image->colorspace != GRAYColorspace))
      (void) TransformRGBImage(image,image->colorspace);

  (void) RGBTransformImage(image,colorspace);
  return  (status);
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
%  colorspace to RGB.  The transformation matrices are not the standard ones:
%  the weights are rescaled to normalize the range of the transformed values
%  to be [0..MaxRGB].
%
%  The format of the TransformRGBImage method is:
%
%      unsigned int TransformRGBImage(Image *image,
%        const ColorspaceType colorspace)
%
%  A description of each parameter follows:
%
%    o image: the image
%
%    o colorspace: the colorspace to transform the image to.
%
*/
typedef struct _RGBColorTransformPacket
{
  int
    red,
    green,
    blue;
} RGBColorTransformPacket;

static void RGBTransformPacket(PixelPacket *pixel,
  const RGBColorTransformPacket *red_map,
  const RGBColorTransformPacket *green_map,
  const RGBColorTransformPacket *blue_map,
  const unsigned char *rgb_map,
  const unsigned int rgb_map_max_index)
{
  register int
    blue,
    green,
    red;

  register const RGBColorTransformPacket
    *red_p,
    *green_p,
    *blue_p;

  red_p = &red_map[ScaleQuantumToMap(pixel->red)];
  green_p = &green_map[ScaleQuantumToMap(pixel->green)];
  blue_p = &blue_map[ScaleQuantumToMap(pixel->blue)];

  red=(red_p->red+green_p->red+blue_p->red);
  if(red < 0) red = 0;

  green=(red_p->green+green_p->green+blue_p->green);
  if(green < 0) green = 0;

  blue=(red_p->blue+green_p->blue+blue_p->blue);
  if(blue < 0) blue = 0;

  if ( rgb_map != 0 )
    {
      unsigned int
        red_index,
        green_index,
        blue_index;

      red_index = ScaleMapToChar(red);
      green_index = ScaleMapToChar(green);
      blue_index = ScaleMapToChar(blue);

      if(red_index > rgb_map_max_index) red_index = rgb_map_max_index;
      if(green_index > rgb_map_max_index) green_index = rgb_map_max_index;
      if(blue_index > rgb_map_max_index) blue_index = rgb_map_max_index;

      pixel->red=ScaleCharToQuantum(rgb_map[red_index]);
      pixel->green=ScaleCharToQuantum(rgb_map[green_index]);
      pixel->blue=ScaleCharToQuantum(rgb_map[blue_index]);
    }
  else
    {
      if(red > MaxMap) red=MaxMap;
      if(green > MaxMap) green=MaxMap;
      if(blue > MaxMap) blue=MaxMap;

      pixel->red=ScaleMapToQuantum((unsigned int) red);
      pixel->green=ScaleMapToQuantum((unsigned int) green);
      pixel->blue=ScaleMapToQuantum((unsigned int) blue);
    }
}
MagickExport unsigned int TransformRGBImage(Image *image,
  const ColorspaceType colorspace)
{
#define TransformRGBImageText  "  Transform image colors...  "


  static const unsigned char
    sRGBMap[351] =
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33,
      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
      50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 65, 66,
      67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82,
      83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99,
      100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
      114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126,
      127, 128, 129, 130, 131, 132, 133, 135, 136, 137, 138, 139, 140,
      141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
      154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166,
      167, 168, 169, 170, 171, 172, 173, 174, 175, 175, 176, 177, 178,
      179, 180, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190,
      191, 192, 193, 194, 194, 195, 196, 197, 198, 199, 199, 200, 201,
      202, 203, 203, 204, 205, 206, 207, 207, 208, 209, 210, 210, 211,
      212, 213, 213, 214, 215, 215, 216, 217, 218, 218, 219, 220, 220,
      221, 222, 222, 223, 223, 224, 225, 225, 226, 227, 227, 228, 228,
      229, 229, 230, 230, 231, 232, 232, 233, 233, 234, 234, 235, 235,
      235, 236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 240,
      241, 241, 242, 242, 242, 243, 243, 243, 243, 244, 244, 244, 245,
      245, 245, 245, 246, 246, 246, 247, 247, 247, 247, 247, 248, 248,
      248, 248, 249, 249, 249, 249, 249, 249, 250, 250, 250, 250, 250,
      250, 251, 251, 251, 251, 251, 251, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255, 255,
      255, 255, 255, 255, 255
    },
    YCCMap[351] =  /* Photo CD information beyond 100% white, Gamma 2.2 */
    {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
      19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35,
      36, 37, 38, 39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 50, 51, 52,
      53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68, 69, 70,
      71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88,
      89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 102, 103, 104,
      105, 106, 107, 108, 110, 111, 112, 113, 114, 115, 116, 117, 118,
      119, 120, 121, 122, 123, 124, 125, 126, 127, 129, 130, 131, 132,
      133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
      146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
      159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171,
      172, 173, 174, 175, 176, 176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 193, 194, 195,
      196, 197, 198, 199, 200, 201, 201, 202, 203, 204, 205, 206, 207,
      207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 215, 216, 217,
      218, 218, 219, 220, 221, 221, 222, 223, 224, 224, 225, 226, 226,
      227, 228, 228, 229, 230, 230, 231, 232, 232, 233, 234, 234, 235,
      236, 236, 237, 237, 238, 238, 239, 240, 240, 241, 241, 242, 242,
      243, 243, 244, 244, 245, 245, 245, 246, 246, 247, 247, 247, 248,
      248, 248, 249, 249, 249, 249, 250, 250, 250, 250, 251, 251, 251,
      251, 251, 252, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253,
      253, 253, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254,
      254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
      255, 255, 255, 255, 255, 255
    };

  RGBColorTransformPacket
    *blue_map,
    *green_map,
    *red_map;

  long
    y;

  register long
    x;

  register long
    i;

  unsigned int
    is_grayscale;

  const unsigned char
    *rgb_map=0;

  unsigned int
    rgb_map_max_index=0;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->colorspace != UndefinedColorspace);

  is_grayscale=(image->is_grayscale ||
                (image->colorspace == GRAYColorspace));

  /*
    If colorspace is already an RGB type then simply return.
  */
  if ((image->colorspace == RGBColorspace) ||
      (image->colorspace == GRAYColorspace) ||
      (image->colorspace == TransparentColorspace))
    {
      return(True);
    }

  {
    /*
      Log colorspace transform event
    */
    const char *
      log_colorspace = ColorspaceTypeToString(image->colorspace);

    if (log_colorspace != NULL)
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Transform colorspace from %s to RGB", log_colorspace);
  }

  if (image->colorspace == CMYKColorspace)
    {
      IndexPacket
        *indexes;

      register PixelPacket
        *q;

      /*
        Transform image from CMYK to RGB.
      */
      if (image->storage_class == PseudoClass)
        {
          SyncImage(image);
          image->storage_class=DirectClass;
        }
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=(Quantum) (((double)(MaxRGB-q->red)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->green=(Quantum) (((double)(MaxRGB-q->green)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->blue=(Quantum) (((double)(MaxRGB-q->blue)*(MaxRGB-q->opacity))/MaxRGB+0.5);
          q->opacity=image->matte ? indexes[x] : OpaqueOpacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
      image->colorspace=RGBColorspace;
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Colorspace transform completed"); 
      return(True);
    }

  if ((image->colorspace == HSLColorspace) ||
      (image->colorspace == HWBColorspace))
    {
      void (*transform)(const double,const double,const double,
        Quantum *,Quantum *,Quantum *);

      switch (colorspace)
        {
        case HSLColorspace:
        default:
          transform=HSLTransform;
        case HWBColorspace:
          transform=HWBTransform;
        }

      switch (image->storage_class)
        {
        case DirectClass:
        default:
          {
            ExceptionInfo
              *exception;
            
            register PixelPacket
              *q;
            
            /*
              Convert DirectClass image.
            */
            exception=(&image->exception);
            for (y=0; y < (long) image->rows; y++)
              {
                q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                for (x=(long) image->columns; x > 0; x--)
                  {
                    (transform)((double)q->red/MaxRGB,(double)q->green/MaxRGB,
                      (double)q->blue/MaxRGB,&q->red,&q->green,&q->blue);
                    q++;
                  }
                if (!SyncImagePixels(image))
                  break;
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitor(RGBTransformImageText,y,image->rows,exception))
                    break;
              }
            break;
          }
        case PseudoClass:
          {
            /*
              Convert PseudoClass image.
            */
            register PixelPacket
              *q;
            
            q=image->colormap;
            for (i=(long) image->colors; i > 0; i--)
              {
                (transform)((double)q->red/MaxRGB,(double)q->green/MaxRGB,
                  (double)q->blue/MaxRGB,&q->red,&q->green,&q->blue);
                q++;
              }
            SyncImage(image);
            break;
          }
        }
      image->colorspace=RGBColorspace;
      LogMagickEvent(TransformEvent,GetMagickModule(),
                     "Colorspace transform completed"); 
      return(True);
    }

  /*
    Allocate the tables.
  */
  red_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  green_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  blue_map=MagickAllocateMemory(RGBColorTransformPacket *,
    (MaxMap+1)*sizeof(RGBColorTransformPacket));
  if ((red_map == (RGBColorTransformPacket *) NULL) ||
      (green_map == (RGBColorTransformPacket *) NULL) ||
      (blue_map == (RGBColorTransformPacket *) NULL))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToTransformColorspace);
  switch (image->colorspace)
  {
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxMap.
      */
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(i);
        green_map[i].red=RndToInt(0.5*(2.0*i-MaxMap));
        blue_map[i].red=RndToInt((-0.33334)*(2.0*i-MaxMap));
        red_map[i].green=RndToInt(i);
        green_map[i].green=0;
        blue_map[i].green=RndToInt(0.666665*(2.0*i-MaxMap));
        red_map[i].blue=RndToInt(i);
        green_map[i].blue=RndToInt((-0.5)*(2.0*i-MaxMap));
        blue_map[i].blue=RndToInt((-0.33334)*(2.0*i-MaxMap));
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
      rgb_map=sRGBMap;
      rgb_map_max_index=350;
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(1.40200*i);
        green_map[i].red=0;
        blue_map[i].red=RndToInt(1.88000*(i-ScaleCharToMap(137)));
        red_map[i].green=RndToInt(1.40200*i);
        green_map[i].green=RndToInt((-0.444066)*(i-ScaleCharToMap(156)));
        blue_map[i].green=RndToInt((-0.95692)*(i-ScaleCharToMap(137)));
        red_map[i].blue=RndToInt(1.40200*i);
        green_map[i].blue=RndToInt(2.28900*(i-ScaleCharToMap(156)));
        blue_map[i].blue=0;
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
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(3.240479*i);
        green_map[i].red=RndToInt((-1.537150)*i);
        blue_map[i].red=RndToInt((-0.498535)*i);
        red_map[i].green=RndToInt((-0.969256)*i);
        green_map[i].green=RndToInt(1.875992*i);
        blue_map[i].green=RndToInt(0.041556*i);
        red_map[i].blue=RndToInt(0.055648*i);
        green_map[i].blue=RndToInt((-0.204043)*i);
        blue_map[i].blue=RndToInt(1.057311*i);
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
        through MaxMap.
      */
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(i);
        green_map[i].red=0;
        blue_map[i].red=RndToInt((1.402000*0.5)*(2.0*i-MaxMap));
        red_map[i].green=RndToInt(i);
        green_map[i].green=RndToInt((-0.344136*0.5)*(2.0*i-MaxMap));
        blue_map[i].green=RndToInt((-0.714136*0.5)*(2.0*i-MaxMap));
        red_map[i].blue=RndToInt(i);
        green_map[i].blue=RndToInt((1.772000*0.5)*(2.0*i-MaxMap));
        blue_map[i].blue=0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Kodak PhotoYCC Color Space.

        Initialize YCC tables:

          R = Y            +1.340762*C2
          G = Y-0.317038*C1-0.682243*C2
          B = Y+1.632639*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      rgb_map=YCCMap;
      rgb_map_max_index=350;
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(1.3584*i);
        green_map[i].red=0;
        blue_map[i].red=RndToInt(1.8215*(i-ScaleCharToMap(137)));
        red_map[i].green=RndToInt(1.3584*i);
        green_map[i].green=RndToInt((-0.4302726)*(i-ScaleCharToMap(156)));
        blue_map[i].green=RndToInt((-0.9271435)*(i-ScaleCharToMap(137)));
        red_map[i].blue=RndToInt(1.3584*i);
        green_map[i].blue=RndToInt(2.2179*(i-ScaleCharToMap(156)));
        blue_map[i].blue=0;
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
        through MaxMap.
      */
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(i);
        green_map[i].red=RndToInt(0.4781*(2.0*i-MaxMap));
        blue_map[i].red=RndToInt(0.3107*(2.0*i-MaxMap));
        red_map[i].green=RndToInt(i);
        green_map[i].green=RndToInt((-0.13635)*(2.0*i-MaxMap));
        blue_map[i].green=RndToInt((-0.3234)*(2.0*i-MaxMap));
        red_map[i].blue=RndToInt(i);
        green_map[i].blue=RndToInt((-0.55185)*(2.0*i-MaxMap));
        blue_map[i].blue=RndToInt(0.8503*(2.0*i-MaxMap));
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
        through MaxMap.
      */
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(i);
        green_map[i].red=0;
        blue_map[i].red=RndToInt(0.701*(2.0*i-MaxMap));
        red_map[i].green=RndToInt(i);
        green_map[i].green=RndToInt((-0.172068)*(2.0*i-MaxMap));
        blue_map[i].green=RndToInt(0.357068*(2.0*i-MaxMap));
        red_map[i].blue=RndToInt(i);
        green_map[i].blue=RndToInt(0.886*(2.0*i-MaxMap));
        blue_map[i].blue=0;
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
        through MaxMap.
      */
      for (i=0; i <= MaxMap; i++)
      {
        red_map[i].red=RndToInt(i);
        green_map[i].red=0;
        blue_map[i].red=RndToInt(0.5699*(2.0*i-MaxMap));
        red_map[i].green=RndToInt(i);
        green_map[i].green=RndToInt((-0.1969)*(2.0*i-MaxMap));
        blue_map[i].green=RndToInt((-0.29025)*(2.0*i-MaxMap));
        red_map[i].blue=RndToInt(i);
        green_map[i].blue=RndToInt(1.01395*(2.0*i-MaxMap));
        blue_map[i].blue=0;
      }
      break;
    }
  }

#if 0
  /*
    Dump tables
  */
  for (i=0; i <= MaxMap; i++)
    {
      printf("%5ld: red_map(%7d,%7d,%7d) green_map(%7d,%7d,%7d) blue_map(%7d,%7d,%7d)\n",
             i,
             ((red_map[i].red)),
             ((red_map[i].green)),
             ((red_map[i].blue)),
             
             ((green_map[i].red)),
             ((green_map[i].green)),
             ((green_map[i].blue)),
             
             ((blue_map[i].red)),
             ((blue_map[i].green)),
             ((blue_map[i].blue)));
    }
#endif

  /*
    Convert to RGB.
  */
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      ExceptionInfo
        *exception;

      register PixelPacket
        *q;

      /*
        Convert DirectClass image.
      */
      exception=(&image->exception);
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;

        for (x=(long) image->columns; x > 0; x--)
        {
          RGBTransformPacket(q,red_map,green_map,blue_map,rgb_map,
                             rgb_map_max_index);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(TransformRGBImageText,y,image->rows,exception))
            break;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      register PixelPacket
        *q;

      q=image->colormap;
      for (i=(long) image->colors; i > 0; i--)
      {
        RGBTransformPacket(q,red_map,green_map,blue_map,rgb_map,
                           rgb_map_max_index);
        q++;
      }
      SyncImagePixels(image);
      break;
    }
  }
  image->is_grayscale=is_grayscale;
  image->colorspace=RGBColorspace;

  /*
    Free allocated memory.
  */
  MagickFreeMemory(blue_map);
  MagickFreeMemory(green_map);
  MagickFreeMemory(red_map);
  LogMagickEvent(TransformEvent,GetMagickModule(),
                 "Colorspace transform completed"); 
  return(True);
}
