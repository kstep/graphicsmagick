/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        JJJJJ  PPPP   EEEEE   GGGG                           %
%                          J    P   P  E      G                               %
%                          J    PPPP   EEE    G  GG                           %
%                        J J    P      E      G   G                           %
%                        JJJ    P      EEEEE   GGG                            %
%                                                                             %
%                                                                             %
%                   Read/Write GraphicsMagick Image Format.                   %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 GraphicsMagick Group, an organization dedicated         %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files                   %
%  ("GraphicsMagick"), to deal in GraphicsMagick without restriction,         %
%  including without limitation the rights to use, copy, modify, merge,       %
%  publish, distribute, sublicense, and/or sell copies of GraphicsMagick,     %
%  and to permit persons to whom GraphicsMagick is furnished to do so,        %
%  subject to the following conditions:                                       %
%                                                                             %
%  The above copyright notice and this permission notice shall be included    %
%  in all copies or substantial portions of GraphicsMagick.                   %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express    %
%  or implied, including but not limited to the warranties of                 %
%  merchantability, fitness for a particular purpose and noninfringement.     %
%  In no event shall GraphicsMagick Group be liable for any claim,            %
%  damages or other liability, whether in an action of contract, tort or      %
%  otherwise, arising from, out of or in connection with GraphicsMagick       %
%  or the use or other dealings in GraphicsMagick.                            %
%                                                                             %
%  Except as contained in this notice, the name of the GraphicsMagick         %
%  Group shall not be used in advertising or otherwise to promote the         %
%  sale, use or other dealings in GraphicsMagick without prior written        %
%  authorization from the GraphicsMagick Group.                               %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This software is based in part on the work of the Independent JPEG Group.
% See ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz for copyright and
% licensing restrictions.  Blob support contributed by Glenn Randers-Pehrson.
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "attribute.h"
#include "blob.h"
#include "cache.h"
#include "color.h"
#include "log.h"
#include "magick.h"
#include "monitor.h"
#include "utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteJPEGImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J P E G                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJPEG returns True if the image format type, identified by the
%  magick string, is JPEG.
%
%  The format of the IsJPEG  method is:
%
%      unsigned int IsJPEG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJPEG returns True if the image format type is JPEG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsJPEG(const unsigned char *magick,const size_t length)
{
  if (length < 3)
    return(False);
  if (memcmp(magick,"\377\330\377",3) == 0)
    return(True);
  return(False);
}

#if defined(HasJPEG)
#ifdef HAVE_STDLIB_H
# undef HAVE_STDLIB_H
# define HAVE_STDLIB_H  /* Avoid conflicting macro with jpeglib,h */
#endif
#define JPEG_INTERNAL_OPTIONS
#if defined(__MINGW32__)
# define XMD_H 1  /* Avoid conflicting typedef for INT32 */
#endif
#include <setjmp.h>
#include "jpeglib.h"
#include "jerror.h"

/*
  Define declarations.
*/
#define ICC_MARKER  (JPEG_APP0+2)
#define IPTC_MARKER  (JPEG_APP0+13)
#define MaxBufferExtent  8192

typedef struct _DestinationManager
{
  struct jpeg_destination_mgr
    manager;

  Image
    *image;

  JOCTET
    *buffer;
} DestinationManager;

typedef struct _ErrorManager
{
  Image
    *image;

  jmp_buf
    error_recovery;
} ErrorManager;

typedef struct _SourceManager
{
  struct jpeg_source_mgr
    manager;

  Image
    *image;

  JOCTET
    *buffer;

  boolean
    start_of_blob;
} SourceManager;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P E G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJPEGImage reads a JPEG image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadJPEGImage method is:
%
%      Image *ReadJPEGImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJPEGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static unsigned int EmitMessage(j_common_ptr jpeg_info,int level)
{
  char
    message[JMSG_LENGTH_MAX];

  ErrorManager
    *error_manager;

  Image
    *image;

  (jpeg_info->err->format_message)(jpeg_info,message);
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  if (level < 0)
    {
      if ((jpeg_info->err->num_warnings == 0) ||
          (jpeg_info->err->trace_level >= 3))
        ThrowBinaryException(CorruptImageWarning,(char *) message,
          image->filename);
      jpeg_info->err->num_warnings++;
    }
  else
    if (jpeg_info->err->trace_level >= level)
      ThrowBinaryException(CoderError,(char *) message,image->filename);
  return(True);
}

static boolean FillInputBuffer(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->manager.bytes_in_buffer=
    ReadBlob(source->image,MaxBufferExtent,(char *) source->buffer);
  if (source->manager.bytes_in_buffer == 0)
    {
      if (source->start_of_blob)
        ERREXIT(cinfo,JERR_INPUT_EMPTY);
      WARNMS(cinfo,JWRN_JPEG_EOF);
      source->buffer[0]=(JOCTET) 0xff;
      source->buffer[1]=(JOCTET) JPEG_EOI;
      source->manager.bytes_in_buffer=2;
    }
  source->manager.next_input_byte=source->buffer;
  source->start_of_blob=FALSE;
  return(TRUE);
}

static unsigned int GetCharacter(j_decompress_ptr jpeg_info)
{
  if (jpeg_info->src->bytes_in_buffer == 0)
    (void) (*jpeg_info->src->fill_input_buffer)(jpeg_info);
  jpeg_info->src->bytes_in_buffer--;
  return(GETJOCTET(*jpeg_info->src->next_input_byte++));
}

static void InitializeSource(j_decompress_ptr cinfo)
{
  SourceManager
    *source;

  source=(SourceManager *) cinfo->src;
  source->start_of_blob=TRUE;
}

static void JPEGErrorHandler(j_common_ptr jpeg_info)
{
  ErrorManager
    *error_manager;

  (void) EmitMessage(jpeg_info,0);
  error_manager=( ErrorManager *) jpeg_info->client_data;
  longjmp(error_manager->error_recovery,1);
}

static boolean ReadComment(j_decompress_ptr jpeg_info)
{
  char
    *comment;

  ErrorManager
    *error_manager;

  Image
    *image;

  long int
    length;

  register char
    *p;

  /*
    Determine length of comment.
  */
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  comment=(char *) AcquireMemory(length+1);
  if (comment == (char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      (char *) NULL);
  /*
    Read comment.
  */
  for (p=comment; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  *p='\0';
  (void) SetImageAttribute(image,"comment",comment);
  LiberateMemory((void **) &comment);
  return(True);
}

static boolean ReadGenericProfile(j_decompress_ptr jpeg_info)
{
  ErrorManager
    *error_manager;

  Image
    *image;

  long
    length;

  register long
    i;

  register unsigned char
    *p;

  int
    marker;

  /*
    Determine length of generic profile.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  marker=jpeg_info->unread_marker-JPEG_APP0;
  /*
    Allocate generic profile.
  */
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  i=(long) image->generic_profiles;
  if (image->generic_profile == (ProfileInfo *) NULL)
    image->generic_profile=(ProfileInfo *) AcquireMemory(sizeof(ProfileInfo));
  else
    ReacquireMemory((void **) &image->generic_profile,
      (i+1)*sizeof(ProfileInfo));
  if (image->generic_profile == (ProfileInfo *) NULL)
    {
      image->generic_profiles=0;
      ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
        (char *) NULL)
    }
  image->generic_profile[i].name=AllocateString((char *) NULL);
  FormatString(image->generic_profile[i].name,"APP%d",marker);
  image->generic_profile[i].info=(unsigned char *) AcquireMemory(length);
  if (image->generic_profile[i].info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      (char *) NULL);
  /*
    Read generic profile.
  */
  image->generic_profile[i].length=length;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Profile: %s, %ld bytes",
    image->generic_profile[i].name,length);
  for (p=image->generic_profile[i].info; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  /*
    Do special checks for either Exif data or Adobe XMP
  */
  length=image->generic_profile[i].length;
  p=image->generic_profile[i].info;
  if ((marker==1) && (length>4) && (strncmp((char *) p,"Exif",4) == 0))
    FormatString(image->generic_profile[i].name,"EXIF");
  else if (((marker==1) && length>5) && (strncmp((char *) p,"http:",5) == 0))
    FormatString(image->generic_profile[i].name,"XMP");
  image->generic_profiles++;
  return(True);
}

static boolean ReadICCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[12];

  ErrorManager
    *error_manager;

  Image
    *image;

  long
    length;

  register long
    i;

  register unsigned char
    *p;

  /*
    Determine length of color profile.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 14)
    {
      while (--length >= 0)
        (void) GetCharacter(jpeg_info);
      return(True);
    }
  for (i=0; i < 12; i++)
    magick[i]=GetCharacter(jpeg_info);
  if (LocaleCompare(magick,"ICC_PROFILE") != 0)
    {
      /*
        Not a ICC profile, return.
      */
      for (i=0; i < length-12; i++)
        (void) GetCharacter(jpeg_info);
      return(True);
    }
  (void) GetCharacter(jpeg_info);  /* id */
  (void) GetCharacter(jpeg_info);  /* markers */
  length-=14;
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  if (image->color_profile.length == 0)
    image->color_profile.info=(unsigned char *) AcquireMemory(length);
  else
    ReacquireMemory((void **) &image->color_profile.info,
      image->color_profile.length+length);
  if (image->color_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      (char *) NULL);
  /*
    Read color profile.
  */
  p=image->color_profile.info+image->color_profile.length;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"ICC profile: %ld bytes",
    length);
  for (image->color_profile.length+=length; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  return(True);
}

static boolean ReadIPTCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[MaxTextExtent];

  ErrorManager
    *error_manager;

  Image
    *image;

  long
    length,
    tag_length;

  register unsigned char
    *p;

  register long
    i;

#ifdef GET_ONLY_IPTC_DATA
  unsigned char
    tag[MaxTextExtent];
#endif

  /*
    Determine length of binary data stored here.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  tag_length=0;
#ifdef GET_ONLY_IPTC_DATA
  *tag='\0';
#endif
  error_manager=(ErrorManager *) jpeg_info->client_data;
  image=error_manager->image;
  if (image->iptc_profile.length == 0)
    {
#ifdef GET_ONLY_IPTC_DATA
      /*
        Find the beginning of the IPTC portion of the binary data.
      */
      for (*tag='\0'; length > 0; )
      {
        *tag=GetCharacter(jpeg_info);
        *(tag+1)=GetCharacter(jpeg_info);
        length-=2;
        if ((*tag == 0x1c) && (*(tag+1) == 0x02))
          break;
      }
      tag_length=2;
#else
      /*
        Validate that this was written as a Photoshop resource format slug.
      */
      for (i=0; i < 10; i++)
        magick[i]=GetCharacter(jpeg_info);
      magick[10]='\0';
      length-=10;
      if (LocaleCompare(magick,"Photoshop ") != 0)
        {
          /*
            Not a ICC profile, return.
          */
          for (i=0; i < length; i++)
            (void) GetCharacter(jpeg_info);
          return(True);
        }
      /*
        Remove the version number.
      */
      for (i=0; i < 4; i++)
        (void) GetCharacter(jpeg_info);
      length-=4;
      tag_length=0;
#endif
    }
  if (length <= 0)
    return(True);
  if (image->iptc_profile.length != 0)
    ReacquireMemory((void **) &image->iptc_profile.info,
      image->iptc_profile.length+length);
  else
    {
      image->iptc_profile.info=(unsigned char *)
        AcquireMemory(length+tag_length);
      if (image->iptc_profile.info != (unsigned char *) NULL)
        image->iptc_profile.length=0;
    }
  if (image->iptc_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      (char *) NULL);
  /*
    Read the payload of this binary data.
  */
  p=image->iptc_profile.info+image->iptc_profile.length;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Profile: IPTC, %ld bytes",
    length);
  for (image->iptc_profile.length+=length; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  return(True);
}

static void SkipInputData(j_decompress_ptr cinfo,long number_bytes)
{
  SourceManager
    *source;

  if (number_bytes <= 0)
    return;
  source=(SourceManager *) cinfo->src;
  while (number_bytes > (long) source->manager.bytes_in_buffer)
  {
    number_bytes-=(long) source->manager.bytes_in_buffer;
    (void) FillInputBuffer(cinfo);
  }
  source->manager.next_input_byte+=(size_t) number_bytes;
  source->manager.bytes_in_buffer-=(size_t) number_bytes;
}

static void TerminateSource(j_decompress_ptr cinfo)
{
  cinfo=cinfo;
}

static void JPEGSourceManager(j_decompress_ptr cinfo,Image *image)
{
  SourceManager
    *source;

  cinfo->src=(struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(SourceManager));
  source=(SourceManager *) cinfo->src;
  source->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  source=(SourceManager *) cinfo->src;
  source->manager.init_source=InitializeSource;
  source->manager.fill_input_buffer=FillInputBuffer;
  source->manager.skip_input_data=SkipInputData;
  source->manager.resync_to_restart=jpeg_resync_to_restart;
  source->manager.term_source=TerminateSource;
  source->manager.bytes_in_buffer=0;
  source->manager.next_input_byte=NULL;
  source->image=image;
}

static Image *ReadJPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ErrorManager
    error_manager;

  Image
    *image;

  IndexPacket
    index;

  long
    x,
    y;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  register IndexPacket
    *indexes;

  register long
    i;

  struct jpeg_decompress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  register JSAMPLE
    *p;

  register PixelPacket
    *q;

  unsigned int
    logging,
    status;

  unsigned long
    number_pixels;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"UnableToOpenFile",image);
  /*
    Initialize image structure.
  */
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=(void (*)(j_common_ptr,int)) EmitMessage;
  jpeg_info.err->error_exit=(void (*)(j_common_ptr)) JPEGErrorHandler;
  jpeg_pixels=(JSAMPLE *) NULL;
  error_manager.image=image;
  if (setjmp(error_manager.error_recovery))
    {
      if (jpeg_pixels != (JSAMPLE *) NULL)
        LiberateMemory((void **) &jpeg_pixels);
      jpeg_destroy_decompress(&jpeg_info);
      ThrowException(exception,image->exception.severity,
        image->exception.reason,image->exception.description);
      number_pixels=image->columns*image->rows;
      if (number_pixels != 0)
        return(image);
      DestroyImage(image);
      return((Image *) NULL);
    }
  jpeg_info.client_data=(void *) &error_manager;
  jpeg_create_decompress(&jpeg_info);
  JPEGSourceManager(&jpeg_info,image);
  jpeg_set_marker_processor(&jpeg_info,JPEG_COM,ReadComment);
  jpeg_set_marker_processor(&jpeg_info,ICC_MARKER,ReadICCProfile);
  jpeg_set_marker_processor(&jpeg_info,IPTC_MARKER,ReadIPTCProfile);
  for (i=1; i < 16; i++)
    if ((i != 2) && (i != 13) && (i != 14))
      jpeg_set_marker_processor(&jpeg_info,JPEG_APP0+i,ReadGenericProfile);
  i=jpeg_read_header(&jpeg_info,True);
  if (jpeg_info.out_color_space == JCS_CMYK)
      image->colorspace=CMYKColorspace;
  if (jpeg_info.saw_JFIF_marker)
    {
      if ((jpeg_info.X_density != 1) && (jpeg_info.Y_density != 1))
        {
          /*
            Set image resolution.
          */
          image->x_resolution=jpeg_info.X_density;
          image->y_resolution=jpeg_info.Y_density;
          if (jpeg_info.density_unit == 1)
            image->units=PixelsPerInchResolution;
          if (jpeg_info.density_unit == 2)
            image->units=PixelsPerCentimeterResolution;
        }
    }
  number_pixels=image->columns*image->rows;
  if (number_pixels != 0)
    {
      double
        scale_factor;

      /*
        Let the JPEG library subsample for us.
      */
      jpeg_calc_output_dimensions(&jpeg_info);
      image->magick_columns=jpeg_info.output_width;
      image->magick_rows=jpeg_info.output_height;
      scale_factor=(double) jpeg_info.output_width/image->columns;
      if (scale_factor > ((double) jpeg_info.output_height/image->rows))
        scale_factor=(double) jpeg_info.output_height/image->rows;
      jpeg_info.scale_denom=(unsigned int) scale_factor;
      jpeg_calc_output_dimensions(&jpeg_info);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Scale_factor: %ld",
        (long) scale_factor);
    }
  if (image_info->subrange != 0)
    {
      jpeg_info.scale_denom=(int) image_info->subrange;
      jpeg_calc_output_dimensions(&jpeg_info);
    }
#if (JPEG_LIB_VERSION >= 61) && defined(D_PROGRESSIVE_SUPPORTED)
#ifdef D_LOSSLESS_SUPPORTED
  image->interlace=
    jpeg_info.process == JPROC_PROGRESSIVE ? PlaneInterlace : NoInterlace;
  image->compression=jpeg_info.process == JPROC_LOSSLESS ?
    LosslessJPEGCompression : JPEGCompression;
  if (jpeg_info.data_precision > 8)
    MagickError(OptionError,
      "12-bit JPEG not supported. Reducing pixel data to 8 bits",(char *) NULL);
#else
  image->interlace=jpeg_info.progressive_mode ? PlaneInterlace : NoInterlace;
  image->compression=JPEGCompression;
#endif
#else
  image->compression=JPEGCompression;
  image->interlace=PlaneInterlace;
#endif
  (void) jpeg_start_decompress(&jpeg_info);
  image->columns=jpeg_info.output_width;
  image->rows=jpeg_info.output_height;
  if (logging)
    {
      if (image->interlace == PlaneInterlace)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Interlace: progressive");
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Interlace: nonprogressive");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Data precision: %d",
        (int) jpeg_info.data_precision);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %dx%d",
        (int) jpeg_info.output_width,(int) jpeg_info.output_height);
#ifdef D_LOSSLESS_SUPPORTED
      if (image->compression==LosslessJPEGCompression)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Quality: 100 (lossless)");
      else
#endif
      {
        int
          hashval,
          sum;

        /*
          Log the JPEG quality that was used for compression.A
        */
        sum=0;
        for (i=0; i < NUM_QUANT_TBLS; i++)
        {
          int
            j;

          if (jpeg_info.quant_tbl_ptrs[i] != NULL)
            for (j=0; j < DCTSIZE2; j++)
            {
              UINT16 *c;
              c=jpeg_info.quant_tbl_ptrs[i]->quantval;
              sum+=c[j];
            }
         }
         if ((jpeg_info.quant_tbl_ptrs[0] != NULL) &&
             (jpeg_info.quant_tbl_ptrs[1] != NULL))
           {
             int
               hash[]=
                 {1020, 1015,  932,  848,  780,  735,  702,  679,  660,  645,
                   632,  623,  613,  607,  600,  594,  589,  585,  581,  571,
                   555,  542,  529,  514,  494,  474,  457,  439,  424,  410,
                   397,  386,  373,  364,  351,  341,  334,  324,  317,  309,
                   299,  294,  287,  279,  274,  267,  262,  257,  251,  247,
                   243,  237,  232,  227,  222,  217,  213,  207,  202,  198,
                   192,  188,  183,  177,  173,  168,  163,  157,  153,  148,
                   143,  139,  132,  128,  125,  119,  115,  108,  104,   99,
                    94,   90,   84,   79,   74,   70,   64,   59,   55,   49,
                    45,   40,   34,   30,   25,   20,   15,   11,    6,    4,
                     0},
               sums[]=
                 {32640,32635,32266,31495,30665,29804,29146,28599,28104,27670,
                  27225,26725,26210,25716,25240,24789,24373,23946,23572,22846,
                  21801,20842,19949,19121,18386,17651,16998,16349,15800,15247,
                  14783,14321,13859,13535,13081,12702,12423,12056,11779,11513,
                  11135,10955,10676,10392,10208, 9928, 9747, 9564, 9369, 9193,
                   9017, 8822, 8639, 8458, 8270, 8084, 7896, 7710, 7527, 7347,
                   7156, 6977, 6788, 6607, 6422, 6236, 6054, 5867, 5684, 5495,
                   5305, 5128, 4945, 4751, 4638, 4442, 4248, 4065, 3888, 3698,
                   3509, 3326, 3139, 2957, 2775, 2586, 2405, 2216, 2037, 1846,
                   1666, 1483, 1297, 1109,  927,  735,  554,  375,  201,  128,
                      0};

             hashval=(jpeg_info.quant_tbl_ptrs[0]->quantval[2]+
               jpeg_info.quant_tbl_ptrs[0]->quantval[53]+
               jpeg_info.quant_tbl_ptrs[1]->quantval[0]+
               jpeg_info.quant_tbl_ptrs[1]->quantval[DCTSIZE2-1]);
             for (i=0; i < 100; i++)
             {
               if ((hashval >= hash[i]) || (sum >= sums[i]))
                 {
                   if ((hashval > hash[i]) || (sum > sums[i]))
                     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "Quality: %ld (approximate)",i+1);
                   else
                     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "Quality: %ld",i+1);
                   break;
                 }
             }
           }
         else
           if (jpeg_info.quant_tbl_ptrs[0] != NULL)
           {
             int
               bwhash[]=
                { 510,  505,  422,  380,  355,  338,  326,  318,  311,  305,
                  300,  297,  293,  291,  288,  286,  284,  283,  281,  280,
                  279,  278,  277,  273,  262,  251,  243,  233,  225,  218,
                  211,  205,  198,  193,  186,  181,  177,  172,  168,  164,
                  158,  156,  152,  148,  145,  142,  139,  136,  133,  131,
                  129,  126,  123,  120,  118,  115,  113,  110,  107,  105,
                  102,  100,   97,   94,   92,   89,   87,   83,   81,   79,
                   76,   74,   70,   68,   66,   63,   61,   57,   55,   52,
                   50,   48,   44,   42,   39,   37,   34,   31,   29,   26,
                   24,   21,   18,   16,   13,   11,    8,    6,    3,    2,
                    0},
               bwsum[]=
                 {16320,16315,15946,15277,14655,14073,13623,13230,12859,12560,
                  12240,11861,11456,11081,10714,10360,10027, 9679, 9368, 9056,
                   8680, 8331, 7995, 7668, 7376, 7084, 6823, 6562, 6345, 6125,
                   5939, 5756, 5571, 5421, 5240, 5086, 4976, 4829, 4719, 4616, 
                   4463, 4393, 4280, 4166, 4092, 3980, 3909, 3835, 3755, 3688,
                   3621, 3541, 3467, 3396, 3323, 3247, 3170, 3096, 3021, 2952,
                   2874, 2804, 2727, 2657, 2583, 2509, 2437, 2362, 2290, 2211,
                   2136, 2068, 1996, 1915, 1858, 1773, 1692, 1620, 1552, 1477,
                   1398, 1326, 1251, 1179, 1109, 1031,  961,  884,  814,  736,
                    667,  592,  518,  441,  369,  292,  221,  151,   86,   64,
                      0};

             hashval=(jpeg_info.quant_tbl_ptrs[0]->quantval[2]+
               jpeg_info.quant_tbl_ptrs[0]->quantval[53]);
             for (i=0; i < 100; i++)
             {
               if ((hashval >= bwhash[i]) || (sum >= bwsum[i]))
                 {
                   if ((hashval > bwhash[i]) || (sum > bwsum[i]))
                     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "Quality: %ld (approximate)",i+1);
                   else
                     (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                       "Quality: %ld",i+1);
                   break;
                 }
             }
           }
      }
      switch (jpeg_info.out_color_space)
      {
        case JCS_CMYK:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: CMYK");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor,
            jpeg_info.comp_info[3].h_samp_factor,
            jpeg_info.comp_info[3].v_samp_factor);
            break;
        }
        case JCS_GRAYSCALE:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: GRAYSCALE");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor);
            break;
        }
        case JCS_RGB:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: RGB");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor);
            break;
        }
        default:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: %d",
            jpeg_info.out_color_space);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor,
            jpeg_info.comp_info[3].h_samp_factor,
            jpeg_info.comp_info[3].v_samp_factor);
          break;
        }
      }
    }
  image->depth=jpeg_info.data_precision <= 8 ? 8 : 16;
  if (jpeg_info.out_color_space == JCS_GRAYSCALE)
    if (!AllocateImageColormap(image,1 << image->depth))
      ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
  if (image_info->ping)
    {
      jpeg_destroy_decompress(&jpeg_info);
      CloseBlob(image);
      return(image);
    }
  jpeg_pixels=(JSAMPLE *)
    AcquireMemory(jpeg_info.output_components*image->columns*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed",image);
  /*
    Convert JPEG pixels to pixel packets.
  */
  scanline[0]=(JSAMPROW) jpeg_pixels;
  for (y=0; y < (long) image->rows; y++)
  {
    (void) jpeg_read_scanlines(&jpeg_info,scanline,1);
    p=jpeg_pixels;
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    if (jpeg_info.data_precision > 8)
      {
        if (jpeg_info.out_color_space == JCS_GRAYSCALE)
          {
            for (x=0; x < (long) image->columns; x++)
            {
              index=(IndexPacket) (16*GETJSAMPLE(*p++));
              VerifyColormapIndex(image,index);
              indexes[x]=index;
              *q++=image->colormap[index];
            }
          }
        else
          {
            for (x=0; x < (long) image->columns; x++)
            {
              q->red=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
              q->green=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
              q->blue=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
              if (image->colorspace == CMYKColorspace)
                q->opacity=ScaleShortToQuantum(16*GETJSAMPLE(*p++));
              q++;
            }
          }
      }
    else
      if (jpeg_info.out_color_space == JCS_GRAYSCALE)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=(IndexPacket) (GETJSAMPLE(*p++));
            VerifyColormapIndex(image,index);
            indexes[x]=index;
            *q++=image->colormap[index];
          }
        }
      else
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=ScaleCharToQuantum(GETJSAMPLE(*p++));
            q->green=ScaleCharToQuantum(GETJSAMPLE(*p++));
            q->blue=ScaleCharToQuantum(GETJSAMPLE(*p++));
            if (image->colorspace == CMYKColorspace)
              q->opacity=ScaleCharToQuantum(GETJSAMPLE(*p++));
            q++;
          }
        }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      if (!MagickMonitor(LoadImageText,y,image->rows,exception))
        break;
  }
  if (image->colorspace == CMYKColorspace)
    {
      /*
        Correct CMYK levels.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=(Quantum) (MaxRGB-q->red);
          q->green=(Quantum) (MaxRGB-q->green);
          q->blue=(Quantum) (MaxRGB-q->blue);
          q->opacity=(Quantum) (MaxRGB-q->opacity);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  /*
    Free jpeg resources.
  */
  (void) jpeg_finish_decompress(&jpeg_info);
  jpeg_destroy_decompress(&jpeg_info);
  LiberateMemory((void **) &jpeg_pixels);
  CloseBlob(image);
  if (logging) 
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  ThrowException(exception,image->exception.severity,image->exception.reason,
    image->exception.description);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r J P E G I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterJPEGImage adds attributes for the JPEG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterJPEGImage method is:
%
%      RegisterJPEGImage(void)
%
*/
ModuleExport void RegisterJPEGImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("JPEG");
  entry->thread_support=False;
#if defined(HasJPEG)
  entry->decoder=(DecoderHandler) ReadJPEGImage;
  entry->encoder=(EncoderHandler) WriteJPEGImage;
#endif
  entry->magick=(MagickHandler) IsJPEG;
  entry->adjoin=False;
  entry->description=
    AcquireString("Joint Photographic Experts Group JFIF format");
#if defined(JPEG_LIB_VERSION)
  {
    char
      version[MaxTextExtent];

    FormatString(version,"%d",JPEG_LIB_VERSION);
    entry->version=AcquireString(version);
  }
#endif
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPG");
  entry->thread_support=False;
#if defined(HasJPEG)
  entry->decoder=(DecoderHandler) ReadJPEGImage;
  entry->encoder=(EncoderHandler) WriteJPEGImage;
#endif
  entry->adjoin=False;
  entry->description=
    AcquireString("Joint Photographic Experts Group JFIF format");
  entry->module=AcquireString("JPEG");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r J P E G I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterJPEGImage removes format registrations made by the
%  JPEG module from the list of supported formats.
%
%  The format of the UnregisterJPEGImage method is:
%
%      UnregisterJPEGImage(void)
%
*/
ModuleExport void UnregisterJPEGImage(void)
{
  (void) UnregisterMagickInfo("JPEG");
  (void) UnregisterMagickInfo("JPG");
}

#if defined(HasJPEG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  W r i t e J P E G I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJPEGImage writes a JPEG image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the WriteJPEGImage method is:
%
%      unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteJPEGImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o jpeg_image:  A pointer to a Image structure.
%
%
*/

static boolean EmptyOutputBuffer(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->manager.free_in_buffer=WriteBlob(destination->image,
    MaxBufferExtent,(char *) destination->buffer);
  if (destination->manager.free_in_buffer != MaxBufferExtent)
    ERREXIT(cinfo,JERR_FILE_WRITE);
  destination->manager.next_output_byte=destination->buffer;
  return(TRUE);
}

static void InitializeDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  destination->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,MaxBufferExtent*sizeof(JOCTET));
  destination->manager.next_output_byte=destination->buffer;
  destination->manager.free_in_buffer=MaxBufferExtent;
}

static unsigned int JPEGWarningHandler(j_common_ptr jpeg_info,int level)
{
  char
    message[JMSG_LENGTH_MAX];

  Image
    *image;

  (jpeg_info->err->format_message)(jpeg_info,message);
  image=(Image *) jpeg_info->client_data;
  if (level < 0)
    {
      if ((jpeg_info->err->num_warnings == 0) ||
          (jpeg_info->err->trace_level >= 3))
        ThrowBinaryException(CoderError,(char *) message,image->filename);
      jpeg_info->err->num_warnings++;
    }
  else
    if (jpeg_info->err->trace_level >= level)
      ThrowBinaryException(CoderError,(char *) message,image->filename);
  return(True);
}

static void TerminateDestination(j_compress_ptr cinfo)
{
  DestinationManager
    *destination;

  destination=(DestinationManager *) cinfo->dest;
  if ((MaxBufferExtent-(int) destination->manager.free_in_buffer) > 0)
    {
      unsigned long
        number_bytes;

      number_bytes=WriteBlob(destination->image,MaxBufferExtent-
        destination->manager.free_in_buffer,(char *) destination->buffer);
      if (number_bytes != (MaxBufferExtent-destination->manager.free_in_buffer))
        ERREXIT(cinfo,JERR_FILE_WRITE);
    }
  if (SyncBlob(destination->image))
    ERREXIT(cinfo,JERR_FILE_WRITE);
}

static void WriteICCProfile(j_compress_ptr jpeg_info,Image *image)
{
  register long
    i,
    j;

  size_t
    length;

  unsigned char
    *profile;

  /*
    Save color profile as a APP marker.
  */
  for (i=0; i < (long) image->color_profile.length; i+=65519)
  {
    length=Min(image->color_profile.length-i,65519);
    profile=(unsigned char *) AcquireMemory(length+14);
    if (profile == (unsigned char *) NULL)
      break;
    (void) strcpy((char *) profile,"ICC_PROFILE");
    profile[12]=(unsigned char) ((i/65519)+1);
    profile[13]=(image->color_profile.length/65519)+1;
    for (j=0; j < (long) length; j++)
      profile[j+14]=image->color_profile.info[i+j];
    jpeg_write_marker(jpeg_info,ICC_MARKER,profile,(unsigned int) length+14);
    LiberateMemory((void **) &profile);
  }
}

static void WriteIPTCProfile(j_compress_ptr jpeg_info,Image *image)
{
  register long
    i;

  size_t
    length;

  unsigned char
    *profile;

  unsigned long
    roundup,
    tag_length;

  /*
    Save binary Photoshop resource data using an APP marker.
  */
#ifdef GET_ONLY_IPTC_DATA
  tag_length=26;
#else
  tag_length=14;
#endif
  for (i=0; i < (long) image->iptc_profile.length; i+=65500)
  {
    length=Min(image->iptc_profile.length-i,65500);
    roundup=(length & 0x01); /* round up for Photoshop */
    profile=(unsigned char *) AcquireMemory(length+roundup+tag_length);
    if (profile == (unsigned char *) NULL)
      break;
#ifdef GET_ONLY_IPTC_DATA
    (void) memcpy(profile,"Photoshop 3.0 8BIM\04\04\0\0\0\0",24);
    profile[13]=0x00;
    profile[24]=length >> 8;
    profile[25]=length & 0xff;
#else
    (void) memcpy(profile,"Photoshop 3.0 ",14);
    profile[13]=0x00;
#endif
    (void) memcpy(&(profile[tag_length]),&(image->iptc_profile.info[i]),length);
    if (roundup)
      profile[length+tag_length]=0;
    jpeg_write_marker(jpeg_info,IPTC_MARKER,profile,(unsigned int)
      (length+roundup+tag_length));
    LiberateMemory((void **) &profile);
  }
}

static void JPEGDestinationManager(j_compress_ptr cinfo,Image * image)
{
  DestinationManager
    *destination;

  cinfo->dest=(struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)
    ((j_common_ptr) cinfo,JPOOL_IMAGE,sizeof(DestinationManager));
  destination=(DestinationManager *) cinfo->dest;
  destination->manager.init_destination=InitializeDestination;
  destination->manager.empty_output_buffer=EmptyOutputBuffer;
  destination->manager.term_destination=TerminateDestination;
  destination->image=image;
}

static unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  const ImageAttribute
    *attribute;

  double
    x_resolution,
    y_resolution;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  long
    y;

  register const PixelPacket
    *p;

  register JSAMPLE
    *q;

  register long
    i,
    x;

  struct jpeg_compress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  unsigned int
    logging,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,"UnableToOpenFile",image);
  /*
    Initialize JPEG parameters.
  */
  jpeg_info.client_data=(void *) image;
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=(void (*)(j_common_ptr,int)) JPEGWarningHandler;
  jpeg_create_compress(&jpeg_info);
  JPEGDestinationManager(&jpeg_info,image);
  jpeg_info.image_width=(unsigned int) image->columns;
  jpeg_info.image_height=(unsigned int) image->rows;
  jpeg_info.input_components=3;
  jpeg_info.data_precision=(int) Min(image->depth,BITS_IN_JSAMPLE);

  jpeg_info.in_color_space=JCS_RGB;
  switch (image_info->colorspace)
  {
    case CMYKColorspace:
    {
      jpeg_info.input_components=4;
      jpeg_info.in_color_space=JCS_CMYK;
      TransformColorspace(image,CMYKColorspace);
      break;
    }
    case YCbCrColorspace:
    {
      jpeg_info.in_color_space=JCS_YCbCr;
      TransformColorspace(image,YCbCrColorspace);
      break;
    }
    default:
    {
      if (image->colorspace == CMYKColorspace)
        {
          jpeg_info.input_components=4;
          jpeg_info.in_color_space=JCS_CMYK;
          break;
        }
      if (image->colorspace == YCbCrColorspace)
        {
          jpeg_info.in_color_space=JCS_YCbCr;
          break;
        }
      TransformColorspace(image,RGBColorspace);
      break;
    }
  }
  if ((image_info->type != TrueColorType) &&
      IsGrayImage(image,&image->exception))
    {
      jpeg_info.input_components=1;
      jpeg_info.in_color_space=JCS_GRAYSCALE;
    }
  jpeg_set_defaults(&jpeg_info);
  x_resolution=72.0;
  y_resolution=72.0;
  if (image_info->density != (char *) NULL)
    {
      int
        count;

      count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
      if (count != 2)
        y_resolution=x_resolution;
    }
  image->x_resolution=x_resolution;
  image->y_resolution=y_resolution;
  jpeg_info.density_unit=1;  /* default to DPI */
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Image resolution: %ld,%ld",(long) image->x_resolution,
      (long) image->y_resolution);
  if ((image->x_resolution != 0) && (image->y_resolution != 0))
    {
      /*
        Set image resolution.
      */
      jpeg_info.write_JFIF_header=True;
      jpeg_info.X_density=(short) image->x_resolution;
      jpeg_info.Y_density=(short) image->y_resolution;
      if (image->units == PixelsPerInchResolution)
        jpeg_info.density_unit=1;
      if (image->units == PixelsPerCentimeterResolution)
        jpeg_info.density_unit=2;
    }
  jpeg_info.dct_method=JDCT_FLOAT;
  jpeg_info.optimize_coding=True;
#if (JPEG_LIB_VERSION >= 61) && defined(C_PROGRESSIVE_SUPPORTED)
  if (image_info->interlace != NoInterlace)
    jpeg_simple_progression(&jpeg_info);
  if (logging)
    {
      if (image_info->interlace != NoInterlace)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Interlace: progressive");
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Interlace: nonprogressive");
    }
#else
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Interlace:  nonprogressive");
#endif
  if ((image->compression == LosslessJPEGCompression) ||
      (image_info->quality > 100))
    {
#if defined(C_LOSSLESS_SUPPORTED)
      if (image_info->quality < 100)
        ThrowException(&image->exception,CoderWarning,
          "LosslessToLossyJPEGConversion",(char *) NULL);
      else
        {
          int
            point_transform,
            predictor;

          predictor=image_info->quality/100;  /* range 1-7 */
          point_transform=image_info->quality % 20;  /* range 0-15 */
          jpeg_simple_lossless(&jpeg_info,predictor,point_transform);
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Compression: lossless");
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Predictor: %d",predictor);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                "Point Transform: %d",point_transform);
            }
        }
#else
        {
          jpeg_set_quality(&jpeg_info,100,True);
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Quality: 100");
        }
#endif
    }
  else
    {
      jpeg_set_quality(&jpeg_info,(int) image_info->quality,True);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Quality: %lu",
          image_info->quality);
    }
  if (image_info->sampling_factor != (char *) NULL)
    {
      double
        horizontal_factor,
        vertical_factor;

      long
        count;

      /*
        Set sampling factor.
      */
      count=sscanf(image_info->sampling_factor,"%lfx%lf",&horizontal_factor,
        &vertical_factor);
      if (count != 2)
        vertical_factor=horizontal_factor;
      for (i=0; i < MAX_COMPONENTS; i++)
      {
        jpeg_info.comp_info[i].h_samp_factor=(int) horizontal_factor;
        jpeg_info.comp_info[i].v_samp_factor=(int) vertical_factor;
      }
    }
   else
    {
      if (image_info->quality >= 90)
        for (i=0; i < MAX_COMPONENTS; i++)
        {
          jpeg_info.comp_info[i].h_samp_factor=1;
          jpeg_info.comp_info[i].v_samp_factor=1;
        }
    }
  jpeg_start_compress(&jpeg_info,True);
  if (logging)
    {
      if (image->storage_class == PseudoClass)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Storage class: PseudoClass");
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Storage class: DirectClass");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Depth: %lu",
        image->depth);
      if (image->colors)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Number of colors: %lu",image->colors);
      else
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Number of colors: unspecified");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "JPEG data precision: %d",(int) jpeg_info.data_precision);
      switch (image_info->colorspace)
      {
        case CMYKColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Storage class: DirectClass");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: CMYK");
          break;
        }
        case YCbCrColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: YCbCr");
          break;
        }
          default:
          break;
      }
      switch (image->colorspace)
      {
        case CMYKColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: CMYK");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor,
            jpeg_info.comp_info[3].h_samp_factor,
            jpeg_info.comp_info[3].v_samp_factor);
          break;
        }
        case GRAYColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: GRAY");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d)",jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor);
          break;
        }
        case RGBColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            " Image colorspace is RGB");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor);
          break;
        }
        case YCbCrColorspace:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Colorspace: YCbCr");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor);
          break;
        }
        default:
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Colorspace: %d",
            image->colorspace);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "Sampling factors: (%d,%d),(%d,%d),(%d,%d),(%d,%d)",
            jpeg_info.comp_info[0].h_samp_factor,
            jpeg_info.comp_info[0].v_samp_factor,
            jpeg_info.comp_info[1].h_samp_factor,
            jpeg_info.comp_info[1].v_samp_factor,
            jpeg_info.comp_info[2].h_samp_factor,
            jpeg_info.comp_info[2].v_samp_factor,
            jpeg_info.comp_info[3].h_samp_factor,
            jpeg_info.comp_info[3].v_samp_factor);
          break;
        }
      }
    }
  /*
    Write JPEG profiles.
  */
  attribute=GetImageAttribute(image,"comment");
  if ((attribute != (const ImageAttribute *) NULL) &&
    (attribute->value != (char *) NULL))
    for (i=0; i < (long) strlen(attribute->value); i+=65533L)
      jpeg_write_marker(&jpeg_info,JPEG_COM,(unsigned char *) attribute->value+
        i,(int) Min(strlen(attribute->value+i),65533L));
  if (image->color_profile.length != 0)
    {
      WriteICCProfile(&jpeg_info,image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "ICC profile: %ld bytes",(long) image->color_profile.length);
    }
  if (image->iptc_profile.length != 0)
    {
      WriteIPTCProfile(&jpeg_info,image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "IPTC profile: %ld bytes",(long) image->iptc_profile.length);
    }
  for (i=0; i < (long) image->generic_profiles; i++)
  {
    register long
      j;

    if (LocaleNCompare(image->generic_profile[i].name,"APP",3) != 0)
      continue;
    x=atol(image->generic_profile[i].name+3);
    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "Profile: %s, %ld bytes",image->generic_profile[i].name,
        (long) image->generic_profile[i].length);
    for (j=0; j < (long) image->generic_profile[i].length; j+=65533L)
      jpeg_write_marker(&jpeg_info,JPEG_APP0+(int) x,
        image->generic_profile[i].info+j,(int)
        Min(image->generic_profile[i].length-j,65533L));
  }
  /*
    Convert MIFF to JPEG raster pixels.
  */
  jpeg_pixels=(JSAMPLE *)
    AcquireMemory(jpeg_info.input_components*image->columns*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed",image);
  scanline[0]=(JSAMPROW) jpeg_pixels;
  if (jpeg_info.data_precision > 8)
    {
      if (jpeg_info.in_color_space == JCS_GRAYSCALE)
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=jpeg_pixels;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=(JSAMPLE) (ScaleQuantumToShort(PixelIntensityToQuantum(p))/16);
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
              break;
        }
      else
        if ((jpeg_info.in_color_space == JCS_RGB) ||
            (jpeg_info.in_color_space == JCS_YCbCr))
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            q=jpeg_pixels;
            for (x=0; x < (long) image->columns; x++)
            {
              *q++=(JSAMPLE) (ScaleQuantumToShort(p->red)/16);
              *q++=(JSAMPLE) (ScaleQuantumToShort(p->green)/16);
              *q++=(JSAMPLE) (ScaleQuantumToShort(p->blue)/16);
              p++;
            }
            (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
          }
        else
          for (y=0; y < (long) image->rows; y++)
          {
            p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            q=jpeg_pixels;
            for (x=0; x < (long) image->columns; x++)
            {
              /*
                Convert DirectClass packets to contiguous CMYK scanlines.
              */
              *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->red)/16);
              *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->green)/16);
              *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->blue)/16);
              *q++=(JSAMPLE) (4095-ScaleQuantumToShort(p->opacity)/16);
              p++;
            }
            (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
            if (QuantumTick(y,image->rows))
              if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
                break;
          }
    }
  else
    if (jpeg_info.in_color_space == JCS_GRAYSCALE)
      for (y=0; y < (long) image->rows; y++)
      {
        p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        q=jpeg_pixels;
        for (x=0; x < (long) image->columns; x++)
        {
          *q++=(JSAMPLE) ScaleQuantumToChar(PixelIntensityToQuantum(p));
          p++;
        }
        (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
        if (QuantumTick(y,image->rows))
          if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
            break;
      }
    else
      if ((jpeg_info.in_color_space == JCS_RGB) ||
          (jpeg_info.in_color_space == JCS_YCbCr))
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=jpeg_pixels;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=(JSAMPLE) ScaleQuantumToChar(p->red);
            *q++=(JSAMPLE) ScaleQuantumToChar(p->green);
            *q++=(JSAMPLE) ScaleQuantumToChar(p->blue);
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
              break;
        }
      else
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=jpeg_pixels;
          for (x=0; x < (long) image->columns; x++)
          {
            /*
              Convert DirectClass packets to contiguous CMYK scanlines.
            */
            *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->red));
            *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->green));
            *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->blue));
            *q++=(JSAMPLE) (ScaleQuantumToChar(MaxRGB-p->opacity));
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(SaveImageText,y,image->rows,&image->exception))
              break;
        }
  jpeg_finish_compress(&jpeg_info);
  /*
    Free memory.
  */
  jpeg_destroy_compress(&jpeg_info);
  LiberateMemory((void **) &jpeg_pixels);
  CloseBlob(image);
  if (logging) 
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"return");
  return(True);
}
#endif
