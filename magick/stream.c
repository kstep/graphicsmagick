/*
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
%                      ImageMagick Pixel Stream Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 March 2000                                  %
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

/*
  Typedef declaractions.
*/
typedef struct _StreamInfo
{
  unsigned int
    columns,
    rows;

  off_t
    length;

  void
    *stash;

  PixelPacket
    *pixels;

  IndexPacket
    *indexes;
} StreamInfo;

/*
  Declare pixel stream interfaces.
*/
static IndexPacket
  *GetIndexesFromStream(const Image *);

static PixelPacket
  GetOnePixelFromStream(Image *,const int,const int),
  *GetPixelsFromStream(const Image *),
  *GetPixelStream(Image *,const int,const int,const unsigned int,
    const unsigned int),
  *SetPixelStream(Image *,const int,const int,const unsigned int,
    const unsigned int);

static unsigned int
  SyncPixelStream(Image *);

static void
  ClosePixelStream(Image *),
  DestroyPixelStream(Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e P i x e l S t r e a m                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ClosePixelStream closes the pixel stream.  For streams, this method
%  is a no-op.
%
%  The format of the ClosePixelStream method is:
%
%      void ClosePixelStream(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void ClosePixelStream(Image *image)
{
  assert(image != (Image *) NULL);
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
%  Method DestroyPixelStream deallocates memory associated with the pixel
%  stream.
%
%  The format of the DestroyPixelStream method is:
%
%      void DestroyPixelStream(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
static void DestroyPixelStream(Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  if (image->cache == (void *) NULL)
    return;
  stream_info=(StreamInfo *) image->cache;
  FreeMemory((void **) &stream_info->stash);
  FreeMemory((void **) &stream_info);
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
%  Method GetIndexesFromStream returns the colormap indexes associated with
%  the last call to the SetPixelStream() or GetPixelStream() methods.
%
%  The format of the GetIndexesFromStream method is:
%
%      IndexPacket *GetIndexesFromStream(const Image *image)
%
%  A description of each parameter follows:
%
%    o indexes: Method GetIndexesFromStream returns the colormap indexes
%      associated with the last call to the SetPixelStream() or GetPixelStream()
%      methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static IndexPacket *GetIndexesFromStream(const Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  stream_info=(StreamInfo *) image->cache;
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
%  Method GetOnePixelFromStream returns a single pixel at the specified (x,y)
%  location.  The image background color is returned if an error occurs.  For
%  streams we just return the background color.
%
%  The format of the GetOnePixelFromStream method is:
%
%      PixelPacket *GetOnePixelFromStream(const Image image,const int x,
%        const int y)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetOnePixelFromStream returns a pixel at the specified
%      (x,y) location.
%
%    o image: The address of a structure of type Image.
%
%    o x,y:  These values define the location of the pixel to return.
%
*/
static PixelPacket GetOnePixelFromStream(Image *image,const int x,const int y)
{
  register PixelPacket
    *pixel;

  assert(image != (Image *) NULL);
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
%  Method GetPixelStream gets pixels from the in-memory or disk pixel cache as
%  defined by the geometry parameters.   A pointer to the pixels is returned if
%  the pixels are transferred, otherwise a NULL is returned.  For streams
%  this method is a no-op.
%
%  The format of the GetPixelStream method is:
%
%      PixelPacket *GetPixelStream(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o status: Method GetPixelStream returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *GetPixelStream(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  PixelPacket
    *pixels;

  assert(image != (Image *) NULL);
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
%  Method GetPixelsFromStream returns the pixels associated with the last call
%  to the SetPixelStream() or GetPixelStream() methods.
%
%  The format of the GetPixelsFromStream method is:
%
%      PixelPacket *GetPixelsFromStream(const Image image)
%
%  A description of each parameter follows:
%
%    o pixels: Method GetPixelsFromStream returns the pixels associated with
%      the last call to the SetPixelStream() or GetPixelStream() methods.
%
%    o image: The address of a structure of type Image.
%
%
*/
static PixelPacket *GetPixelsFromStream(const Image *image)
{
  StreamInfo
    *stream_info;

  assert(image != (Image *) NULL);
  stream_info=(StreamInfo *) image->cache;
  return(stream_info->pixels);
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
%  Method SetPixelStream allocates an area to store image pixels as defined
%  by the region rectangle and returns a pointer to the area.  This area is
%  subsequently transferred from the pixel cache with method SyncPixelStream.
%  A pointer to the pixels is returned if the pixels are transferred,
%  otherwise a NULL is returned.
%
%  The format of the SetPixelStream method is:
%
%      PixelPacket *SetPixelStream(Image *image,const int x,const int y,
%        const unsigned int columns,const unsigned int rows)
%
%  A description of each parameter follows:
%
%    o pixels: Method SetPixelStream returns a pointer to the pixels is
%      returned if the pixels are transferred, otherwise a NULL is returned.
%
%    o image: The address of a structure of type Image.
%
%    o x,y,columns,rows:  These values define the perimeter of a region of
%      pixels.
%
%
*/
static PixelPacket *SetPixelStream(Image *image,const int x,const int y,
  const unsigned int columns,const unsigned int rows)
{
  StreamInfo
    *stream_info;

  off_t
    length,
    number_pixels;

  /*
    Validate pixel cache geometry.
  */
  assert(image != (Image *) NULL);
  if ((x < 0) || (y < 0) || ((x+columns) > (int) image->columns) ||
      ((y+rows) > (int) image->rows) || (columns == 0) || (rows == 0))
    {
      ThrowException(&image->exception,StreamWarning,
        "Unable to set pixel stream",
        "image does not contain the stream geometry");
      return((PixelPacket *) NULL);
    }
  if (image->cache == (void *) NULL)
    {
      stream_info=(StreamInfo *) AllocateMemory(sizeof(StreamInfo));
      if (stream_info == (StreamInfo *) NULL)
        MagickError(ResourceLimitError,"Memory allocation failed",
          "unable to allocate cache info");
      stream_info->columns=image->columns;
      stream_info->rows=image->rows;
      stream_info->stash=(void *) NULL;
      stream_info->pixels=(PixelPacket *) NULL;
      stream_info->indexes=(IndexPacket *) NULL;
      image->cache=stream_info;
    }
  /*
    Pixels are stored in a temporary buffer until they are synced to the cache.
  */
  stream_info=(StreamInfo *) image->cache;
  number_pixels=stream_info->columns*stream_info->rows;
  length=number_pixels*sizeof(PixelPacket);
  if (image->storage_class == PseudoClass)
    length+=number_pixels*sizeof(IndexPacket);
  if (stream_info->stash == (void *) NULL)
    stream_info->stash=AllocateMemory(length);
  else
    if (stream_info->length != length)
      ReallocateMemory((void **) &stream_info->stash,length);
  if (stream_info->stash == (void *) NULL)
    MagickError(ResourceLimitError,"Memory allocation failed",
      "unable to allocate cache info");
  stream_info->length=length;
  stream_info->pixels=(PixelPacket *) stream_info->stash;
  stream_info->indexes=(IndexPacket *)
    (stream_info->pixels+stream_info->columns*stream_info->rows);
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
%  Method SyncPixelStream calls the user supplied callback method with the
%  latest stream of pixels.
%
%  The format of the SyncPixelStream method is:
%
%      unsigned int SyncPixelStream(Image *image)
%
%  A description of each parameter follows:
%
%    o status: Method SyncPixelStream returns True if the image pixels are
%      transferred to the in-memory or disk cache otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
static unsigned int SyncPixelStream(Image *image)
{
  assert(image != (Image *) NULL);
  image->fifo(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r e a m I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StreamImage makes the image pixels available to a user supplied
%  callback method immediately upon reading a scanline with the ReadImage()
%  method.
%
%  The format of the StreamImage method is:
%
%      unsigned int StreamImage(const ImageInfo *image_info,
%        void (*Stream)(const Image *),ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method StreamImage returns True if the image pixels are
%      streamed to the user supplied callback method otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o stream: a callback method.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int StreamImage(const ImageInfo *image_info,
  void (*fifo)(Image *),ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  /*
    Replace image pixel methods with streaming methods.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  CloseImagePixels=ClosePixelStream;
  DestroyImagePixels=DestroyPixelStream;
  GetImagePixels=GetPixelStream;
  GetIndexes=GetIndexesFromStream;
  GetOnePixel=GetOnePixelFromStream;
  GetPixels=GetPixelsFromStream;
  SetImagePixels=SetPixelStream;
  SyncImagePixels=SyncPixelStream;
  /*
    Stream image pixels.
  */
  clone_info=CloneImageInfo(image_info);
  clone_info->fifo=fifo;
  image=ReadImage(clone_info,exception);
  DestroyImageInfo(clone_info);
  DestroyImage(image);
  /*
    Restore pixel cache methods.
  */
  SetPixelCacheMethods();
  return(exception->severity == UndefinedException);
}
