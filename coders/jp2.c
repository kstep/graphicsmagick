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
%                               Nathan Brown                                  %
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
#include "defines.h"
#if defined(HasJP2)
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
  if (length < 12)
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
static int
  JP2CloseBlob(stream_obj_t *),
  JP2ReadBlob(stream_obj_t *,char *,int),
  JP2WriteBlob(stream_obj_t *,char *, int);

static long
  JP2SeekBlob(stream_obj_t *,long,int);

static stream_ops_t
  jp2_stream_fileops =
  {
    JP2ReadBlob,
    JP2WriteBlob,
    JP2SeekBlob,
    JP2CloseBlob
  };

typedef struct _SourceManager
{
  stream_t
    *stream;

  Image
    *image;

  unsigned char
    *buffer;

  unsigned char
    start_of_blob;
} SourceManager;

static int JP2CloseBlob(stream_object_t *object)
{
  CloseBlob(((SourceManager *) object)->image);
  return(0);
}

static int JP2ReadBlob(stream_object_t *object,char *buffer,int count)
{
  return(ReadBlob(((SourceManager *) object)->image,count,(void *) buffer));
}

static long JP2SeekBlob(stream_object_t *object,long offset,int origin)
{
  return(SeekBlob(((SourceManager *) object)->image,offset,origin));
}

static void JP2SourceManager(stream_t *stream,Image *image)
{
  SourceManager
    *source;

  if (stream->obj_ == (stream_obj_t *) NULL)
    {
      stream->obj_=(stream_obj_t *) malloc(sizeof(SourceManager));
      source=(SourceManager *) stream->obj_;
      source->buffer=(unsigned char *) NULL;
    }
  source->image=image;
  stream->openmode_=0;
  stream->bufmode_=0;
  stream->flags_=0;
  stream->bufbase_=0;
  stream->bufstart_=0;
  stream->bufsize_=0;
  stream->ptr_=0;
  stream->cnt_=0;
  stream->ops_=0;
  stream->rwcnt_=0;
  stream->rwlimit_=(-1);
  stream->ops_=&jp2_stream_fileops;
  stream->openmode_=STREAM_READ | STREAM_WRITE | STREAM_BINARY;
  stream->bufbase_=stream->tinybuf_;
  stream->bufsize_=1;
  stream->bufstart_=(&stream->bufbase_[STREAM_MAXPUTBACK]);
  stream->ptr_=stream->bufstart_;
  stream->cnt_=0;
  stream->bufmode_|=STREAM_UNBUF & STREAM_BUFMODEMASK;
}

static int JP2WriteBlob(stream_object_t *object,char *buffer,int count)
{
  return(WriteBlob(((SourceManager *) object)->image,count,(void *) buffer));
}
#endif

#if defined(HasJP2)
static Image *ReadJP2Image(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    magic_number[MaxTextExtent];

  Image
    *image;

  stream_t
    jp2_stream;

  image_t
    *jp2_image;

  int
    y;

  matrix_t
    *components[4];

  register int
    i,
    x;

  register PixelPacket
    *q;

  uint_fast16_t
    depth,
    number_components;

  uint_fast32_t
    height,
    width;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  jp2_stream.obj_=(stream_obj_t *) NULL;
  JP2SourceManager(&jp2_stream,image);
  if (LocaleCompare(image_info->magick,"JP2") == 0)
    {
      /*
        Verify JP2 signature.
      */
      (void) ReadBlob(image,12,magic_number);
      if (memcmp(magic_number+4,"\152\120\040\040\015",5) != 0)
        ThrowReaderException(CorruptImageWarning,"Not a JP2 image file",image);
      SeekBlob(image,0,SEEK_SET);
      jp2_image=image_load_jp2(&jp2_stream,"");
    }
  else
    if (LocaleCompare(image_info->magick,"JPC") == 0)
      {
        /*
          Verify JPC signature.
        */
        (void) ReadBlob(image,2,magic_number);
        if (memcmp(magic_number,"\xff\x4f",2) != 0)
          ThrowReaderException(CorruptImageWarning,"Not a JPC image file",
            image);
        SeekBlob(image, 0, SEEK_SET);
        jp2_image=image_load_jpc(&jp2_stream,"");
      }
  if (jp2_image->number_components_ == 0)
    ThrowReaderException(CorruptImageWarning,"Not image data",image);
  number_components=image_number_components(jp2_image);
  width=image_cmptwidth(jp2_image, 0);
  height=image_cmptheight(jp2_image, 0);
  depth=image_cmptprec(jp2_image, 0);
  for (i=0; i < number_components; i++)
  {
    if ((image_cmptwidth(jp2_image,i) != width) ||
        (image_cmptheight(jp2_image, i) != height) ||
        (image_cmptprec(jp2_image, i) != depth) ||
        (image_componentsgnd(jp2_image, i) != False) ||
        (image_cmpttlx(jp2_image, i) != 0) ||
        (image_cmpttly(jp2_image, i) != 0))
      ThrowReaderException(CorruptImageWarning,
        "Unable to represent an image with this geometry.",image);
  }
  if ((depth != 1) && (depth != 4) && (depth != 8))
    ThrowReaderException(CorruptImageWarning,
      "The component depths must be 1, 4, or 8.",image);
  /*
    Initialize image.
  */
  image->columns=width;
  image->rows=height;
  image->depth=depth;
  image->colorspace=(image_colormodel(jp2_image) == CM_GRAY) ?
    GRAYColorspace : RGBColorspace;
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  for (i = 0; i < number_components; i++)
    components[i]=0;
  for (i = 0; i < number_components; i++)
  {
    components[i]=matrix_create(1,image->columns);
    if (components[i] == NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
  }
  /*
    Convert JP2 pixels to pixel packets.
  */
  for (y=0; y < (int) image->rows; y++)
  {
    for (i = 0; i < number_components && i < 4; ++i)
    {
      if (image_readcmpt(jp2_image,i,0,y,image->columns,1,components[i]))
        break;
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < (int) image->columns; x++)
      {
        if (image->colorspace == GRAYColorspace)
          {
            q->red=(Quantum) UpScale(matrix_getv(components[0],x));
            q->green=q->red;
            q->blue=q->red;
          }
        else
          {
            q->red=(Quantum) UpScale(matrix_getv(components[0],x));
            q->green=(Quantum) UpScale(matrix_getv(components[1],x));
            q->blue=(Quantum) UpScale(matrix_getv(components[2],x));
            if (image->colorspace == CMYKColorspace)
              q->opacity=(Quantum) UpScale(matrix_getv(components[3],x));
          }
        q++;
      }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        MagickMonitor(LoadImageText,y,image->rows);
    }
    for (i=0; i < number_components; i++)
      if (components[i] != NULL)
        matrix_destroy(components[i]);
  }
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
  entry->decoder=ReadJP2Image;
  entry->encoder=WriteJP2Image;
  entry->magick=IsJP2;
  entry->description=AllocateString("JPEG 2000 Image and Metadata");
  entry->module=AllocateString("JP2");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPC");
  entry->decoder=ReadJP2Image;
  entry->encoder=WriteJP2Image;
  entry->magick=IsJPC;
  entry->adjoin=False;
  entry->description=AllocateString("JPEG 2000 Image Code Stream");
  entry->module=AllocateString("JPC");
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
  image_t
    *jp2_image;

  image_cmptparm_t
    *parameters;

  int
    y;

  matrix_t
    *components[4];

  register int
    i,
    x;

  register PixelPacket
    *p;

  stream_t
    jp2_stream;

  uint_fast16_t
     number_components;

  uint_fast32_t
     x_resolution,
     y_resolution;

  unsigned int
     status;

  /*
    Open image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  if (IsGrayImage(image) || (image->colorspace != RGBColorspace))
    TransformRGBImage(image,RGBColorspace);
  jp2_stream.obj_=(stream_obj_t *) NULL;
  JP2SourceManager(&jp2_stream,image);
  number_components=IsGrayImage(image) ? 1 : 3;
  parameters=(image_cmptparm_t *)
    AcquireMemory(number_components*sizeof(image_cmptparm_t));
  parameters->tlx=0;
  parameters->tly=0;
  parameters->prec=image->depth;
  parameters->sgnd=False;
  parameters->height=image->rows;
  parameters->width=image->columns;
  x_resolution=72;
  y_resolution=72;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
      if (count != 2)
        y_resolution=x_resolution;
    }
  parameters->vstep=1;
  parameters->hstep=1;
  /*
    Initialize image structure.
  */
  memcpy(parameters+1,parameters,sizeof(image_cmptparm_t));
  memcpy(parameters+2,parameters,sizeof(image_cmptparm_t));
  jp2_image=image_create(number_components,parameters,
    (IsGrayImage(image) ? CM_GRAY : CM_RGB));
  if (!jp2_image)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  for (i = 0; i < number_components; ++i)
    components[i]=0;
  for (i = 0; i < number_components; ++i)
  {
    components[i]=matrix_create(1,image->columns);
    if (components[i] == NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
  }
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      matrix_setv(components[0],x,DownScale(p->red));
      matrix_setv(components[1],x,DownScale(p->green));
      matrix_setv(components[2],x,DownScale(p->blue));
      p++;
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
    for (i=0; i < (number_components && (i < 3)); i++)
    {
      if (image_writecmpt(jp2_image,i,0,y,image->columns,1,components[i]))
        break;
    }
  }
  if (LocaleCompare(image_info->magick,"JP2") == 0)
    image_save_jp2(jp2_image,&jp2_stream,"");
  else if (LocaleCompare(image_info->magick,"JPC") == 0)
    image_save_jpc(jp2_image, &jp2_stream,"");
  for (i = 0; i < number_components; i++)
    if (components[i] != NULL)
      matrix_destroy(components[i]);
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
