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

static mpeg2dec_t mpeg2dec;
#define BUFFER_SIZE 4096
static uint8_t buffer[BUFFER_SIZE];

typedef struct common_instance_s {
    vo_instance_t vo;
    int prediction_index;
    vo_frame_t * frame_ptr[3];
} common_instance_t;


typedef struct pgm_instance_s {
    vo_instance_t vo;
    int prediction_index;
    vo_frame_t * frame_ptr[3];
    vo_frame_t frame[3];
    int width;
    int height;
    int framenum;
    char header[1024];
    char filename[128];
} pgm_instance_t;

int libvo_common_alloc_frames (vo_instance_t * _instance,
			       int width, int height, int frame_size,
			       void (* copy) (vo_frame_t *, uint8_t **),
			       void (* field) (vo_frame_t *, int),
			       void (* draw) (vo_frame_t *))
{
    common_instance_t * instance;
    int size;
    uint8_t * alloc;
    int i;

    instance = (common_instance_t *) _instance;
    instance->prediction_index = 1;
    size = width * height / 4;
    alloc = malloc (18 * size);
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
    free (instance->frame_ptr[0]->base[0]);
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
    sprintf (instance->header, "P5\n\n%d %d\n255\n", width, height * 3 / 2);
    return libvo_common_alloc_frames ((vo_instance_t *) instance,
                                      width, height, sizeof (vo_frame_t),
                                      NULL, NULL, draw_frame);
}

static void pgm_draw_frame (vo_frame_t * frame)
{
    pgm_instance_t * instance;
    int i;
    FILE * file;

    instance = (pgm_instance_t *) frame->instance;
    if (++(instance->framenum) < 0)
        return;
    sprintf (instance->filename, "%d.pgm", instance->framenum);
    file = fopen (instance->filename, "wb");
    if (!file)
        return;

    fwrite (instance->header, strlen (instance->header), 1, file);
    fwrite (frame->base[0], instance->width, instance->height, file);
    for (i = 0; i < instance->height >> 1; i++) {
        fwrite (frame->base[1]+i*instance->width/2, instance->width/2, 1,
                file);
        fwrite (frame->base[2]+i*instance->width/2, instance->width/2, 1,
                file);
    }
    fclose (file);
}

static int pgm_setup (vo_instance_t * instance, int width, int height)
{
   return internal_setup (instance, width, height, pgm_draw_frame);
}

vo_instance_t *OpenVideo(void)
{
  pgm_instance_t
    *instance;

  instance=malloc (sizeof (pgm_instance_t));
  if (instance == NULL)
    return NULL;
  instance->vo.setup=pgm_setup;
  instance->framenum=(-2);
  return(vo_instance_t *) instance;
}

static Image *ReadMPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  int
    num_frames;

  uint8_t
    *end;

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
  video=vo_open(OpenVideo);
  mpeg2_init(&mpeg2dec,0,video);
  do
  {
    end=buffer+ReadBlob(image,BUFFER_SIZE,buffer);
    num_frames=mpeg2_decode_data(&mpeg2dec,buffer,end);
    printf("%d\n",num_frames);
  } while (end == (buffer+BUFFER_SIZE));
  mpeg2_close(&mpeg2dec);
  vo_close(video);
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
