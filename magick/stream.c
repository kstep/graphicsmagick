/*
% Copyright (C) 2003, 2004 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  SSSSS  TTTTT  RRRR   EEEEE   AAA   M   M                   %
%                  SS       T    R   R  E      A   A  MM MM                   %
%                   SSS     T    RRRR   EEE    AAAAA  M M M                   %
%                     SS    T    R R    E      A   A  M   M                   %
%                  SSSSS    T    R  R   EEEEE  A   A  M   M                   %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Pixel Stream Methods                      %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
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
#include "magick/cache.h"
#include "magick/constitute.h"
#include "magick/semaphore.h"
#include "magick/stream.h"
#include "magick/utility.h"

/*
  Typedef declaractions.
*/
typedef CacheInfo StreamInfo;

/*
  Declare pixel cache interfaces.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static const PixelPacket
  *AcquirePixelStream(const Image *,const long,const long,const unsigned long,
    const unsigned long,ExceptionInfo *);

static IndexPacket
  *GetIndexesFromStream(const Image *);

static PixelPacket
  AcquireOnePixelFromStream(const Image *,const long,const long,
    ExceptionInfo *),
  GetOnePixelFromStream(Image *,const long,const long),
  *GetPixelStream(Image *,const long,const long,const unsigned long,
    const unsigned long),
  *GetPixelsFromStream(const Image *),
  *SetPixelStream(Image *,const long,const long,const unsigned long,
    const unsigned long);

static unsigned int
  SyncPixelStream(Image *);

static void
  DestroyPixelStream(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e O n e P i x e l F r o m S t r e a m                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquireOnePixelFromStream() returns a single pixel at the specified
%  (x,y) location.  The image background color is returned if an error occurs.
%
%  The format of the AcquireOnePixelFromStream() method is:
%
%      PixelPacket *AcquireOnePixelFromStream(const Image image,const long x,
%        const long y,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromStream returns a pixel at the specified
%      (x,y) location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static PixelPacket AcquireOnePixelFromStream(const Image *image,const long x,
  const long y,ExceptionInfo *exception)
{
  register const PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixel=AcquirePixelStream(image,x,y,1,1,exception);
  if (pixel != (PixelPacket *) NULL)
    return(*pixel);
  return(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   A c q u i r e P i x e l S t r e a m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method AcquirePixelStream() gets pixels from the in-memory or disk pixel
%  cache as defined by the geometry parameters.   A pointer to the pixels is
%  returned if the pixels are transferred, otherwise a NULL is returned.  For
%  streams this method is a no-op.
%
%  The format of the AcquirePixelStream() method is:
%
%      const PixelPacket *AcquirePixelStream(const Image *image,const long x,
%        const long y,const unsigned long columns,const unsigned long rows,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelStream() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static const PixelPacket *AcquirePixelStream(const Image *image,const long x,
  const long y,const unsigned long columns,const unsigned long rows,
  ExceptionInfo *exception)
{
  size_t
    length;

  StreamInfo
    *stream_info;

  magick_uint64_t
    number_pixels;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (const Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+(long) columns) > (long) image->columns) ||
      ((y+(long) rows) > (long) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException3(exception,StreamError,UnableToAcquirePixelStream,
        ImageDoesNotContainTheStreamGeometry);
      return((PixelPacket *) NULL);
    }
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  if (stream_info->type == UndefinedCache)
    {
      ThrowException(exception,StreamError,PixelCacheIsNotOpen,
        image->filename);
      return((PixelPacket *) NULL);
    }
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  number_pixels=(magick_uint64_t)columns*rows;
  length=number_pixels*sizeof(PixelPacket);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  if (stream_info->pixels == (PixelPacket *) NULL)
    stream_info->pixels=MagickAllocateMemory(PixelPacket *,length);
  else
    if (length != stream_info->length)
      MagickReallocMemory(stream_info->pixels,length);
  if (stream_info->pixels == (void *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateCacheInfo);
  stream_info->length=length;
  stream_info->indexes=(IndexPacket *) NULL;
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    stream_info->indexes=(IndexPacket *) (stream_info->pixels+number_pixels);
  return(stream_info->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y P i x e l S t r e a m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyPixelStream() deallocates memory associated with the pixel
%  stream.
%
%  The format of the DestroyPixelStream() method is:
%
%      void DestroyPixelStream(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
static void DestroyPixelStream(Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  AcquireSemaphoreInfo((SemaphoreInfo **) &stream_info->semaphore);
  stream_info->reference_count--;
  if (stream_info->reference_count > 0)
    {
      LiberateSemaphoreInfo((SemaphoreInfo **) &stream_info->semaphore);
      return;
    }
  LiberateSemaphoreInfo((SemaphoreInfo **) &stream_info->semaphore);
  if (stream_info->pixels != (PixelPacket *) NULL)
    MagickFreeMemory(stream_info->pixels);
  if (stream_info->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo((SemaphoreInfo **) &stream_info->semaphore);
  MagickFreeMemory(stream_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t I n d e x e s F r o m S t r e a m                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetIndexesFromStream() returns the indexes associated with the last
%  call to SetPixelStream() or GetPixelStream().
%
%  The format of the GetIndexesFromStream() method is:
%
%      IndexPacket *GetIndexesFromStream(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexesFromStream() returns the indexes associated
%      with the last call to SetPixelStream() or GetPixelStream().
%
%    o image: The image.
%
%
*/
static IndexPacket *GetIndexesFromStream(const Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  return(stream_info->indexes);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t O n e P i x e l F r o m S t r e a m                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetOnePixelFromStream() returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.
%
%  The format of the GetOnePixelFromStream() method is:
%
%      PixelPacket *GetOnePixelFromStream(const Image image,const long x,
%        const long y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromStream returns a pixel at the specified
%      (x,y) location.
%
%    o image: The image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
static PixelPacket GetOnePixelFromStream(Image *image,const long x,const long y)
{
  register PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixel=GetPixelStream(image,x,y,1,1);
  if (pixel != (PixelPacket *) NULL)
    return(*pixel);
  return(image->background_color);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t P i x e l S t r e a m                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelStream() gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.  For streams
%  this method is a no-op.
%
%  The format of the GetPixelStream() method is:
%
%      PixelPacket *GetPixelStream(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelStream() returns a pointer to the pixels if they
%      are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *GetPixelStream(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  PixelPacket
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  pixels=SetPixelStream(image,x,y,columns,rows);
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t P i x e l F r o m S t e a m                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPixelsFromStream() returns the pixels associated with the last
%  call to SetPixelStream() or GetPixelStream().
%
%  The format of the GetPixelsFromStream() method is:
%
%      PixelPacket *GetPixelsFromStream(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixelsFromStream returns the pixels associated with
%      the last call to SetPixelStream() or GetPixelStream().
%
%    o image: The image.
%
%
*/
static PixelPacket *GetPixelsFromStream(const Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  return(stream_info->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S t r e a m                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadStream() makes the image pixels available to a user supplied
%  callback method immediately upon reading a scanline with the ReadImage()
%  method.
%
%  The format of the ReadStream() method is:
%
%      Image *ReadStream(const ImageInfo *image_info,StreamHandler stream,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o stream: a callback method.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *ReadStream(const ImageInfo *image_info,StreamHandler stream,
  ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  /*
    Stream image pixels.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  clone_info=CloneImageInfo(image_info);
  GetCacheInfo(&clone_info->cache);
  SetPixelCacheMethods(clone_info->cache,AcquirePixelStream,GetPixelStream,
    SetPixelStream,SyncPixelStream,GetPixelsFromStream,GetIndexesFromStream,
    AcquireOnePixelFromStream,GetOnePixelFromStream,DestroyPixelStream);
  clone_info->stream=stream;
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S e t P i x e l S t r e a m                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetPixelStream() allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with method SyncPixelStream().
%  A pointer to the pixels is returned if the pixels are transferred,
%  otherwise a NULL is returned.
%
%  The format of the SetPixelStream() method is:
%
%      PixelPacket *SetPixelStream(Image *image,const long x,const long y,
%        const unsigned long columns,const unsigned long rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetPixelStream returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *SetPixelStream(Image *image,const long x,const long y,
  const unsigned long columns,const unsigned long rows)
{
  size_t
    length;

  StreamInfo
    *stream_info;

  StreamHandler
    stream;

  magick_uint64_t
    number_pixels;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+(long) columns) > (long) image->columns) ||
      ((y+(long) rows) > (long) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException3(&image->exception,StreamError,UnableToSetPixelStream,
        ImageDoesNotContainTheStreamGeometry);
      return((PixelPacket *) NULL);
    }
  stream=GetBlobStreamHandler(image);
  if (stream == (const StreamHandler) NULL)
    {
      ThrowException3(&image->exception,StreamError,UnableToSetPixelStream,
        NoStreamHandlerIsDefined);
      return((PixelPacket *) NULL);
    }
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  if ((image->storage_class != GetCacheClass(image->cache)) ||
      (image->colorspace != GetCacheColorspace(image->cache)))
    {
      if (GetCacheClass(image->cache) == UndefinedClass)
        (void) stream(image,(const void *) NULL,stream_info->columns);
      stream_info->storage_class=image->storage_class;
      stream_info->colorspace=image->colorspace;
      stream_info->columns=image->columns;
      stream_info->rows=image->rows;
      image->cache=stream_info;
    }
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  stream_info->columns=columns;
  stream_info->rows=rows;
  number_pixels=(magick_uint64_t)columns*rows;
  length=number_pixels*sizeof(PixelPacket);
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    length+=number_pixels*sizeof(IndexPacket);
  if (stream_info->pixels == (PixelPacket *) NULL)
    {
      stream_info->pixels=MagickAllocateMemory(PixelPacket *,length);
      stream_info->length=length;
    }
  else
    if (stream_info->length < length)
      {
        MagickReallocMemory(stream_info->pixels,length);
        stream_info->length=length;
      }
  if (stream_info->pixels == (void *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateImagePixels);
  stream_info->indexes=(IndexPacket *) NULL;
  if ((image->storage_class == PseudoClass) ||
      (image->colorspace == CMYKColorspace))
    stream_info->indexes=(IndexPacket *) (stream_info->pixels+number_pixels);
  return(stream_info->pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S y n c P i x e l S t r e a m                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SyncPixelStream() calls the user supplied callback method with the
%  latest stream of pixels.
%
%  The format of the SyncPixelStream method is:
%
%      unsigned int SyncPixelStream(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelStream() returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The image.
%
%
*/
static unsigned int SyncPixelStream(Image *image)
{
  StreamInfo
    *stream_info;

  StreamHandler
    stream;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  stream_info=(StreamInfo *) image->cache;
  assert(stream_info->signature == MagickSignature);
  stream=GetBlobStreamHandler(image);
  if (stream == (StreamHandler) NULL)
    {
      ThrowException3(&image->exception,StreamError,UnableToSyncPixelStream,
        NoStreamHandlerIsDefined);
      return(False);
    }
  return(stream(image,stream_info->pixels,stream_info->columns));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S t r e a m                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteStream() makes the image pixels available to a user supplied
%  callback method immediately upon writing pixel data with the WriteImage()
%  method.
%
%  The format of the WriteStream() method is:
%
%      unsigned int WriteStream(const ImageInfo *image_info,Image *,
%        StreamHandler stream)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o stream: A callback method.
%
%
*/
MagickExport unsigned int WriteStream(const ImageInfo *image_info,Image *image,
  StreamHandler stream)
{
  ImageInfo
    *clone_info;

  unsigned int
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  clone_info=CloneImageInfo(image_info);
  clone_info->stream=stream;
  status=WriteImage(clone_info,image);
  DestroyImageInfo(clone_info);
  return(status);
}
