/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M  PPPP   EEEEE   GGGG                           %
%                        MM MM  P   P  E      G                               %
%                        M M M  PPPP   EEE    G  GG                           %
%                        M   M  P      E      G   G                           %
%                        M   M  P      EEEEE   GGGG                           %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
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

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M P E G                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMPEG returns True if the image format type, identified by the
%  magick string, is MPEG.
%
%  The format of the IsMPEG method is:
%
%      unsigned int IsMPEG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMPEG returns True if the image format type is MPEG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsMPEG(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\000\000\001\263",4) == 0)
    return(True);
  return(False);
}

#define HasMPEG2
#if defined(HasMPEG2)
#include <inttypes.h>
#include "video_out.h"
#include "mpeg2.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M P E G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMPEGImage reads an binary file in the MPEG format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the Photoshop resource (MPEG) information is useful in
%  the returned image.
%
%  The format of the ReadMPEGImage method is:
%
%      Image *ReadMPEGImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMPEGImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

Image
  *image;

ImageInfo
  *clone_info;

typedef struct common_instance_s
{
  vo_instance_t
    vo;

  int
    prediction_index;

  vo_frame_t
    *frame_ptr[3];
} common_instance_t;

typedef struct pgm_instance_s
{
  vo_instance_t
    vo;

  int
    prediction_index;

  vo_frame_t
    *frame_ptr[3],
    frame[3];

  int
    width,
    height;

  int
    framenum;
} pgm_instance_t;

int libvo_common_alloc_frames (vo_instance_t * _instance,
			       int width, int height, int frame_size,
			       void (* copy) (vo_frame_t *, uint8_t **),
			       void (* field) (vo_frame_t *, int),
			       void (* draw) (vo_frame_t *))
{
    common_instance_t * instance;
    size_t size;
    uint8_t * alloc;
    int i;

    instance = (common_instance_t *) _instance;
    instance->prediction_index = 1;
    size = width * height/4;
    alloc = (uint8_t *) AcquireMemory(18*size);
    if (alloc == NULL)
	return 1;

    for (i = 0; i < 3; i++) {
	instance->frame_ptr[i] =
	    (vo_frame_t *) (((char *) instance) + sizeof (common_instance_t) +
			    i * frame_size);
	instance->frame_ptr[i]->base[0] = alloc;
	instance->frame_ptr[i]->base[1] = alloc + 4 * size;
	instance->frame_ptr[i]->base[2] = alloc + 5 * size;
	instance->frame_ptr[i]->copy = copy;
	instance->frame_ptr[i]->field = field;
	instance->frame_ptr[i]->draw = draw;
	instance->frame_ptr[i]->instance = (vo_instance_t *) instance;
	alloc += 6 * size;
    }

    return 0;
}

void libvo_common_free_frames (vo_instance_t * _instance)
{
    common_instance_t * instance;

    instance = (common_instance_t *) _instance;
    LiberateMemory( (void **) &(instance->frame_ptr[0]->base[0]));
}

vo_frame_t * libvo_common_get_frame (vo_instance_t * _instance, int flags)
{
    common_instance_t * instance;

    instance = (common_instance_t *)_instance;
    if (flags & VO_PREDICTION_FLAG) {
        instance->prediction_index ^= 1;
        return instance->frame_ptr[instance->prediction_index];
    } else
        return instance->frame_ptr[2];
}


static int internal_setup (vo_instance_t * _instance, int width, int height,
                           void (* draw_frame) (vo_frame_t *))
{
    pgm_instance_t * instance;

    instance = (pgm_instance_t *) _instance;

    instance->vo.close = libvo_common_free_frames;
    instance->vo.get_frame = libvo_common_get_frame;
    instance->width = width;
    instance->height = height;
    return libvo_common_alloc_frames ((vo_instance_t *) instance,
                                      width, height, sizeof (vo_frame_t),
                                      NULL, NULL, draw_frame);
}

static void pgm_draw_frame (vo_frame_t * frame)
{
  Image
    *chroma_image,
    *clone_image,
    *resize_image;

  long
    y;

  register const PixelPacket
    *r;

  register long
    x;
  
  register PixelPacket
    *q;

  register unsigned char
    *p;

  pgm_instance_t
    *instance;

  instance=(pgm_instance_t *) frame->instance;
  instance->framenum++;
  if (instance->framenum < 0)
    return;
  if (clone_info->subrange != 0)
    if (instance->framenum < clone_info->subimage)
      return;
  if (image->columns != 0)
    {
      /*
        Allocate next image structure.
      */
      AllocateNextImage(clone_info,image);
      if (image->next == (Image *) NULL)
        {
          DestroyImages(image);
          return;
        }
      image=image->next;
      MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
    } 
  image->columns=instance->width;
  image->rows=instance->height;
  image->scene=instance->framenum;
  p=frame->base[0];
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->red=UpScale(*p++);
      q->green=0;
      q->blue=0;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  chroma_image=CloneImage(image,image->columns/2,image->rows/2,True,
    &image->exception);
  if (chroma_image == (Image *) NULL)
    return;
  p=frame->base[1];
  for (y=0; y < (long) chroma_image->rows; y++)
  {
    q=SetImagePixels(chroma_image,0,y,chroma_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) chroma_image->columns; x++)
    {
      q->red=0;
      q->green=UpScale(*p++);
      q->blue=0;
      q++;
    }
    if (!SyncImagePixels(chroma_image))
      break;
  }
  p=frame->base[2];
  for (y=0; y < (long) chroma_image->rows; y++)
  {
    q=GetImagePixels(chroma_image,0,y,chroma_image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (long) chroma_image->columns; x++)
    {
      q->blue=UpScale(*p++);
      q++;
    }
    if (!SyncImagePixels(chroma_image))
      break;
  }
  /*
    Scale image.
  */
  clone_image=CloneImage(chroma_image,0,0,True,&image->exception);
  if (clone_image == (Image *) NULL)
    return;
  resize_image=ResizeImage(clone_image,image->columns,image->rows,
    TriangleFilter,1.0,&image->exception);
  DestroyImage(clone_image);
  DestroyImage(chroma_image);
  if (resize_image == (Image *) NULL)
    return;
  for (y=0; y < (long) image->rows; y++)
  {
    q=GetImagePixels(image,0,y,image->columns,1);
    r=AcquireImagePixels(resize_image,0,y,resize_image->columns,1,
      &resize_image->exception);
    if ((q == (PixelPacket *) NULL) || (r == (const PixelPacket *) NULL))
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      q->green=r->green;
      q->blue=r->blue;
      r++;
      q++;
    }
    if (!SyncImagePixels(image))
      break;
  }
  DestroyImage(resize_image);
  (void) TransformRGBImage(image,YCbCrColorspace);
}

static int pgm_setup (vo_instance_t * instance, int width, int height)
{
   return internal_setup (instance, width, height, pgm_draw_frame);
}

vo_instance_t *OpenVideo(void)
{
  pgm_instance_t
    *instance;

  instance=(pgm_instance_t *) AcquireMemory(sizeof(pgm_instance_t));
  if (instance == (pgm_instance_t *) NULL)
    return NULL;
  instance->vo.setup=pgm_setup;
  instance->framenum=(-2);
  return(vo_instance_t *) instance;
}

static Image *ReadMPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  static uint8_t
    buffer[4096];

  mpeg2dec_t
    mpeg_info;

  register uint8_t
    *q;

  unsigned int
    status;

  vo_instance_t
    *video;

  /*
    Open image.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  clone_info=CloneImageInfo(image_info);
  video=vo_open(OpenVideo);
  mpeg2_init(&mpeg_info,0,video);
  do
  {
    q=buffer+ReadBlob(image,4096,buffer);
    (void) mpeg2_decode_data(&mpeg_info,buffer,q);
    if (clone_info->subrange != 0)
      if (image->scene >= (clone_info->subimage+clone_info->subrange-1))
        break;
  } while (q == (buffer+4096));
  mpeg2_close(&mpeg_info);
  vo_close(video);
  DestroyImageInfo(clone_info);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadMPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "MPEG2 library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M P E G I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterMPEGImage adds attributes for the MPEG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterMPEGImage method is:
%
%      RegisterMPEGImage(void)
%
*/
ModuleExport void RegisterMPEGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("MPEG");
  entry->decoder=ReadMPEGImage;
  entry->magick=IsMPEG;
  entry->thread_support=False;
  entry->description=AllocateString("MPEG Video Stream");
  entry->module=AllocateString("MPEG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M P E G I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMPEGImage removes format registrations made by the
%  BIM module from the list of supported formats.
%
%  The format of the UnregisterBIMImage method is:
%
%      UnregisterMPEGImage(void)
%
*/
ModuleExport void UnregisterMPEGImage(void)
{
  (void) UnregisterMagickInfo("MPEG");
}
