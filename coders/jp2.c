/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                              JJJ  PPPP    222                               %
%                               J   P   P  2   2                              %
%                               J   PPPP     22                               %
%                            J  J   P       2                                 %
%                             JJ    P      22222                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                                John Cristy                                  %
%                                Nathan Brown                                 %
%                                 June 2001                                   %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"
#if defined(HasJP2)
#if !defined(uchar)
#define uchar  unsigned char
#endif
#include "jasper/jasper.h"
#endif

/*
  Forward declarations.
*/
static unsigned int
  WriteJP2Image(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P 2                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJP2 returns True if the image format type, identified by the
%  magick string, is JP2.
%
%  The format of the IsJP2 method is:
%
%      unsigned int IsJP2(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJP2 returns True if the image format type is JP2.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJP2(const unsigned char *magick,const unsigned int length)
{
  if (length < 9)
    return(False);
  if (memcmp(magick+4,"\152\120\040\040\015",5) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P C                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJPC returns True if the image format type, identified by the
%  magick string, is JPC.
%
%  The format of the IsJPC method is:
%
%      unsigned int IsJPC(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJPC returns True if the image format type is JPC.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJPC(const unsigned char *magick,const unsigned int length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\377\117",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P 2 I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJP2Image reads a JPEG 2000 Image file (JP2) or JPEG 2000
%  codestream (JPC) image file and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image or set of images.
%
%  JP2 support written by Nathan Brown, nathanbrown@letu.edu.
%
%  The format of the ReadJP2Image method is:
%
%      Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJP2Image returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
#if defined(HasJP2)

typedef struct _StreamManager
{
  jas_stream_t
    *stream;

  Image
    *image;
} StreamManager;

static int BlobRead(jas_stream_obj_t *object,char *buffer,int count)
{
  return(ReadBlob(((StreamManager *) object)->image,count,(void *) buffer));
}

static int BlobWrite(jas_stream_obj_t *object,char *buffer,int count)
{
  return(WriteBlob(((StreamManager *) object)->image,count,(void *) buffer));
}

static long BlobSeek(jas_stream_obj_t *object,long offset,int origin)
{
  return(SeekBlob(((StreamManager *) object)->image,offset,origin));
}

static int BlobClose(jas_stream_obj_t *object)
{
  CloseBlob(((StreamManager *) object)->image);
  return(0);
}

static jas_stream_ops_t
  StreamOperators =
  {
    BlobRead,
    BlobWrite,
    BlobSeek,
    BlobClose
  };

static jas_stream_t *JP2StreamManager(Image *image)
{
  jas_stream_t 
    *stream;

  StreamManager
    *source;

  stream=(jas_stream_t *) AcquireMemory(sizeof(jas_stream_t));
  if (stream == (jas_stream_t *) NULL)
    return((jas_stream_t *) NULL);
  memset(stream,0,sizeof(jas_stream_t));
  stream->rwlimit_=(-1);
  stream->obj_=(jas_stream_obj_t *) AcquireMemory(sizeof(StreamManager));
  if (stream->obj_ == (jas_stream_obj_t *) NULL)
    return((jas_stream_t *) NULL);
  stream->ops_=(&StreamOperators);
  stream->openmode_=JAS_STREAM_READ | JAS_STREAM_WRITE | JAS_STREAM_BINARY;
  stream->bufbase_=stream->tinybuf_;
  stream->bufsize_=1;
  stream->bufstart_=(&stream->bufbase_[JAS_STREAM_MAXPUTBACK]);
  stream->ptr_=stream->bufstart_;
  stream->bufmode_|=JAS_STREAM_UNBUF & JAS_STREAM_BUFMODEMASK;
  source=(StreamManager *) stream->obj_;
  source->image=image;
  return(stream);
}

static Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    y;

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register int
    i,
    x;

  register PixelPacket
    *q;

  unsigned int
    number_components,
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize JPEG 2000 API.
  */
  jas_init();
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to manage JP2 stream",image);
  jp2_image=jas_image_decode(jp2_stream,-1,0);
  if (jp2_image == (jas_image_t *) NULL)
    ThrowReaderException(FileOpenWarning,"Unable to decode image file",image);
  (void) jas_stream_close(jp2_stream);
  /*
    Convert JPEG 2000 pixels.
  */
  image->columns=jas_image_width(jp2_image);
  image->rows=jas_image_height(jp2_image);
  number_components=Min(jas_image_numcmpts(jp2_image),4);
  for (i=0; i < (int) number_components; i++)
  {
    if (jas_image_cmptprec(jp2_image,i) <= 8)
      image->depth=jas_image_cmptprec(jp2_image,i);
    pixels[i]=jas_matrix_create(1,image->columns);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        jas_image_destroy(jp2_image);
        ThrowReaderException(ResourceLimitWarning,"Unable to allocate memory",
          image);
      }
  }
  for (y=0; y < (int) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (i=0; i < (int) number_components; i++)
      (void) jas_image_readcmpt(jp2_image,(short) i,0,y,image->columns,1,
	pixels[i]);
    for (x=0; x < (int) image->columns; x++)
    {
      q->red=UpScale(jas_matrix_getv(pixels[0],x));
      q->green=q->red;
      q->blue=q->red;
      if (number_components > 1)
        q->green=UpScale(jas_matrix_getv(pixels[1],x));
      if (number_components > 2)
        q->blue=UpScale(jas_matrix_getv(pixels[2],x));
      if (number_components > 3)
        q->opacity=UpScale(jas_matrix_getv(pixels[3],x));
      q++;
    }
    if (!SyncImagePixels(image))
      break;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        MagickMonitor(LoadImageText,y,image->rows);
  }
  for (i=0; i < (int) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  jas_image_destroy(jp2_image);
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "JP2 library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J P 2 I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJP2Image adds attributes for the JP2 image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJP2Image method is:
%
%      RegisterJP2Image(void)
%
*/
ModuleExport void RegisterJP2Image(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("JP2");
  entry->description=AllocateString("JPEG-2000 JP2 File Format Syntax");
  entry->module=AllocateString("JP2");
  entry->magick=IsJP2;
  entry->adjoin=False;
  entry->thread_support=False;
  entry->decoder=ReadJP2Image;
  entry->encoder=WriteJP2Image;
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPC");
  entry->description=AllocateString("JPEG-2000 Code Stream Syntax");
  entry->module=AllocateString("JPC");
  entry->magick=IsJPC;
  entry->adjoin=False;
  entry->thread_support=False;
  entry->decoder=ReadJP2Image;
  entry->encoder=WriteJP2Image;
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J P 2 I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJP2Image removes format registrations made by the
%  PNG module from the list of supported formats.
%
%  The format of the UnregisterJP2Image method is:
%
%      UnregisterJP2Image(void)
%
*/
ModuleExport void UnregisterJP2Image(void)
{
  UnregisterMagickInfo("JP2");
  UnregisterMagickInfo("JPC");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J P 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJP2Image writes an image in the JPEG 2000 image format.
%
%  JP2 support written by Nathan Brown, nathanbrown@letu.edu
%
%  The format of the WriteJP2Image method is:
%
%      unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteJP2Image return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
#if defined(HasJP2)
static unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
{
  char
    magick[MaxTextExtent],
    options[MaxTextExtent];

  int
    format,
    y;

  jas_image_cmptparm_t
    component_info[4];

  jas_image_t
    *jp2_image;

  jas_matrix_t
    *pixels[4];

  jas_stream_t
    *jp2_stream;

  register int
    i,
    x;

  register PixelPacket
    *p;

  unsigned int
    number_components,
    status;

  /*
    Open image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdout) || image->pipet ||
      (image->blob->data != (unsigned char *) NULL))
  /*
    Intialize JPEG 2000 API.
  */
  TransformRGBImage(image,RGBColorspace);
  jas_init();
  jp2_stream=JP2StreamManager(image);
  if (jp2_stream == (jas_stream_t *) NULL)
    ThrowWriterException(FileOpenWarning,"Unable to manage JP2 stream",image);
  number_components=image->matte ? 4 : 3;
  if (IsGrayImage(image))
    number_components=1;
  for (i=0; i < (int) number_components; i++)
  {
    memset(component_info+i,0,sizeof(jas_image_cmptparm_t));
    component_info[i].hstep=1;
    component_info[i].vstep=1;
    component_info[i].width=image->columns;
    component_info[i].height=image->rows;
    component_info[i].prec=image->depth;
  }
  jp2_image=jas_image_create((short)number_components,component_info,
    number_components == 1 ? JAS_IMAGE_CM_GRAY : JAS_IMAGE_CM_RGB);
  if (jp2_image == (jas_image_t *) NULL)
    ThrowWriterException(FileOpenWarning,"Unable to create image",image);
  /*
    Convert to JPEG 2000 pixels.
  */
  for (i=0; i < (int) number_components; i++)
  {
    pixels[i]=jas_matrix_create(1,image->columns);
    if (pixels[i] == (jas_matrix_t *) NULL)
      {
        for (x=0; x < i; x++)
          jas_matrix_destroy(pixels[x]);
        jas_image_destroy(jp2_image);
        ThrowWriterException(ResourceLimitWarning,"Unable to allocate memory",
          image);
      }
  }
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      jas_matrix_setv(pixels[0],x,DownScale(p->red));
      if (number_components > 1)
        {
          jas_matrix_setv(pixels[1],x,DownScale(p->green));
          jas_matrix_setv(pixels[2],x,DownScale(p->blue));
          if (number_components > 3)
            jas_matrix_setv(pixels[3],x,DownScale(p->opacity));
        }
      p++;
    }
    for (i=0; i < (int) number_components; i++)
      (void) jas_image_writecmpt(jp2_image,(short) i,0,y,image->columns,1,
        pixels[i]);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
  }
  for (i=0; i < (int) number_components; i++)
    jas_matrix_destroy(pixels[i]);
  (void) strcpy(magick,image_info->magick);
  LocaleLower(magick);
  format=jas_image_strtofmt(magick);
  FormatString(options,"rate=%lf",(double) image_info->quality/100.0);
  status=jas_image_encode(jp2_image,jp2_stream,format,options);
  if (status)
    ThrowWriterException(FileOpenWarning,"Unable to encode image file",image);
  (void) jas_stream_close(jp2_stream);
  jas_image_destroy(jp2_image);
  CloseBlob(image);
  return(True);
}
#else
static unsigned int WriteJP2Image(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateWarning,"JP2 library is not available",
    image->filename);
}
#endif
