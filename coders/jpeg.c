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
%                    Read/Write ImageMagick Image Format.                     %
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
% This software is based in part on the work of the Independent JPEG Group.
% See ftp://ftp.uu.net/graphics/jpeg/jpegsrc.v6b.tar.gz for copyright and
% licensing restrictions.  Blob support contributed by Glenn Randers-Pehrson.
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

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
#define JPEG_INTERNAL_OPTIONS
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

static jmp_buf
  error_recovery;

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

  Image
    *image;

  (jpeg_info->err->format_message)(jpeg_info,message);
  image=(Image *) jpeg_info->client_data;
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
      ThrowBinaryException(DelegateWarning,(char *) message,image->filename);
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
  (void) EmitMessage(jpeg_info,0);
  longjmp(error_recovery,1);
}

static boolean ReadComment(j_decompress_ptr jpeg_info)
{
  char
    *comment;

  Image
    *image;

  long int
    length;

  register char
    *p;

  /*
    Determine length of comment.
  */
  image=(Image *) jpeg_info->client_data;
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  comment=(char *) AcquireMemory(length+1);
  if (comment == (char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
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
  Image
    *image;

  long
    length;

  register long
    i;

  register unsigned char
    *p;

  /*
    Determine length of generic profile.
  */
  length=(long) GetCharacter(jpeg_info) << 8;
  length+=(long) GetCharacter(jpeg_info);
  length-=2;
  if (length <= 0)
    return(True);
  /*
    Allocate generic profile.
  */
  image=(Image *) jpeg_info->client_data;
  i=image->generic_profiles;
  if (image->generic_profile == (ProfileInfo *) NULL)
    image->generic_profile=(ProfileInfo *) AcquireMemory(sizeof(ProfileInfo));
  else
    ReacquireMemory((void **) &image->generic_profile,
      (i+1)*sizeof(ProfileInfo));
  if (image->generic_profile == (ProfileInfo *) NULL)
    {
      image->generic_profiles=0;
      ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL)
    }
  image->generic_profile[i].name=AllocateString((char *) NULL);
  FormatString(image->generic_profile[i].name,"APP%d",
    jpeg_info->unread_marker-JPEG_APP0);
  image->generic_profile[i].info=(unsigned char *) AcquireMemory(length);
  if (image->generic_profile[i].info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      (char *) NULL);
  /*
    Read generic profile.
  */
  image->generic_profile[i].length=length;
  for (p=image->generic_profile[i].info; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  image->generic_profiles++;
  return(True);
}

static boolean ReadICCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[12];

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
  if (length <= 0)
    return(True);
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
  image=(Image *) jpeg_info->client_data;
  if (image->color_profile.length == 0)
    image->color_profile.info=(unsigned char *) AcquireMemory(length);
  else
    ReacquireMemory((void **) &image->color_profile.info,
      image->color_profile.length+length);
  if (image->color_profile.info == (unsigned char *) NULL)
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      (char *) NULL);
  /*
    Read color profile.
  */
  p=image->color_profile.info+image->color_profile.length;
  for (image->color_profile.length+=length; --length >= 0; p++)
    *p=GetCharacter(jpeg_info);
  return(True);
}

static boolean ReadIPTCProfile(j_decompress_ptr jpeg_info)
{
  char
    magick[MaxTextExtent];

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
  image=(Image *) jpeg_info->client_data;
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
    ThrowBinaryException(ResourceLimitWarning,"Memory allocation failed",
      (char *) NULL);
  /*
    Read the payload of this binary data.
  */
  p=image->iptc_profile.info+image->iptc_profile.length;
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

  if (cinfo->src == (JOCTET) NULL)
    {
      cinfo->src=(struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
        ((j_common_ptr) cinfo,JPOOL_PERMANENT,sizeof(SourceManager));
      source=(SourceManager *) cinfo->src;
      source->buffer=(JOCTET *) (*cinfo->mem->alloc_small)
        ((j_common_ptr) cinfo,JPOOL_PERMANENT,MaxBufferExtent*sizeof(JOCTET));
    }
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

  register int
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
    status;

  unsigned long
    number_pixels;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  jpeg_info.client_data=(void *) image;
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=(void (*)(j_common_ptr,int)) EmitMessage;
  jpeg_info.err->error_exit=(void (*)(j_common_ptr)) JPEGErrorHandler;
  jpeg_pixels=(JSAMPLE *) NULL;
  if (setjmp(error_recovery))
    {
      if (jpeg_pixels != (JSAMPLE *) NULL)
        LiberateMemory((void **) &jpeg_pixels);
      jpeg_destroy_decompress(&jpeg_info);
      *exception=image->exception;
      GetExceptionInfo(&image->exception);
      number_pixels=image->columns*image->rows;
      if (number_pixels != 0)
        return(image);
      DestroyImage(image);
      return((Image *) NULL);
    }
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
  if (jpeg_info.data_precision > QuantumDepth)
    MagickWarning(OptionWarning,
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
  image->depth=jpeg_info.data_precision <= 8 ? 8 : QuantumDepth;
  if (jpeg_info.out_color_space == JCS_GRAYSCALE)
    if (!AllocateImageColormap(image,1 << jpeg_info.data_precision))
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
  jpeg_pixels=(JSAMPLE *)
    AcquireMemory(jpeg_info.output_components*image->columns*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
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
    if (jpeg_info.data_precision > QuantumDepth)
      {
        if (jpeg_info.out_color_space == JCS_GRAYSCALE)
          {
            for (x=0; x < (long) image->columns; x++)
            {
              index=ValidateColormapIndex(image,GETJSAMPLE(*p++)/16);
              indexes[x]=index;
              *q++=image->colormap[index];
            }
          }
        else
          {
            for (x=0; x < (long) image->columns; x++)
            {
              q->red=(Quantum) (GETJSAMPLE(*p++)/16);
              q->green=(Quantum) (GETJSAMPLE(*p++)/16);
              q->blue=(Quantum) (GETJSAMPLE(*p++)/16);
              if (image->colorspace == CMYKColorspace)
                q->opacity=(IndexPacket) (GETJSAMPLE(*p++)/16);
              q++;
            }
          }
      }
    else
      if (jpeg_info.out_color_space == JCS_GRAYSCALE)
        {
          for (x=0; x < (long) image->columns; x++)
          {
            index=ValidateColormapIndex(image,GETJSAMPLE(*p++));
            indexes[x]=index;
            *q++=image->colormap[index];
          }
        }
      else
        {
          for (x=0; x < (long) image->columns; x++)
          {
            q->red=(Quantum) UpScale(GETJSAMPLE(*p++));
            q->green=(Quantum) UpScale(GETJSAMPLE(*p++));
            q->blue=(Quantum) UpScale(GETJSAMPLE(*p++));
            if (image->colorspace == CMYKColorspace)
              q->opacity=(IndexPacket) UpScale(GETJSAMPLE(*p++));
            q++;
          }
        }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
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
  *exception=image->exception;
  GetExceptionInfo(&image->exception);
  return(image);
}
#else
static Image *ReadJPEGImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "JPEG library is not available",image_info->filename);
  return((Image *) NULL);
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
  entry->decoder=ReadJPEGImage;
  entry->encoder=WriteJPEGImage;
  entry->magick=IsJPEG;
  entry->adjoin=False;
  entry->description=
    AllocateString("Joint Photographic Experts Group JFIF format");
  entry->module=AllocateString("JPEG");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("JPG");
  entry->decoder=ReadJPEGImage;
  entry->encoder=WriteJPEGImage;
  entry->adjoin=False;
  entry->description=
    AllocateString("Joint Photographic Experts Group JFIF format");
  entry->module=AllocateString("JPEG");
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
        ThrowBinaryException(DelegateWarning,(char *) message,image->filename);
      jpeg_info->err->num_warnings++;
    }
  else
    if (jpeg_info->err->trace_level >= level)
      ThrowBinaryException(DelegateWarning,(char *) message,image->filename);
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
  for (i=0; i < image->color_profile.length; i+=65519)
  {
    length=Min(image->color_profile.length-i,65519);
    profile=(unsigned char *) AcquireMemory(length+14);
    if (profile == (unsigned char *) NULL)
      break;
    (void) strcpy((char *) profile,"ICC_PROFILE");
    profile[12]=(i/65519)+1;
    profile[13]=(image->color_profile.length/65519)+1;
    for (j=0; j < length; j++)
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
  for (i=0; i < image->iptc_profile.length; i+=65500)
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

  if (cinfo->dest == (JOCTET) NULL)
    cinfo->dest=(struct jpeg_destination_mgr *) (*cinfo->mem->alloc_small)
      ((j_common_ptr) cinfo,JPOOL_PERMANENT,sizeof(DestinationManager));
  destination=(DestinationManager *) cinfo->dest;
  destination->manager.init_destination=InitializeDestination;
  destination->manager.empty_output_buffer=EmptyOutputBuffer;
  destination->manager.term_destination=TerminateDestination;
  destination->image=image;
}

static unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  double
    x_resolution,
    y_resolution;

  ImageAttribute
    *attribute;

  long
    y;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

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
    status;

  /*
    Open image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
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
      if (image->colorspace != CMYKColorspace)
        (void) RGBTransformImage(image,CMYKColorspace);
      break;
    }
    case YCbCrColorspace:
    {
      jpeg_info.in_color_space=JCS_YCbCr;
      if (image->colorspace != YCbCrColorspace)
        (void) RGBTransformImage(image,YCbCrColorspace);
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
      if (image->colorspace == CMYKColorspace)
        {
          jpeg_info.in_color_space=JCS_YCbCr;
          break;
        }
      if (image->colorspace != RGBColorspace)
        (void) TransformRGBImage(image,RGBColorspace);
      break;
    }
  }
  if (image->storage_class != DirectClass)
    if (IsGrayImage(image,&image->exception))
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
  if (image_info->quality >= 75)
    for (i=0; i < MAX_COMPONENTS; i++)
    {
      jpeg_info.comp_info[i].h_samp_factor=1;
      jpeg_info.comp_info[i].v_samp_factor=1;
    }
  jpeg_info.dct_method=JDCT_FLOAT;
  jpeg_info.optimize_coding=True;
#if (JPEG_LIB_VERSION >= 61) && defined(C_PROGRESSIVE_SUPPORTED)
  if (image_info->interlace != NoInterlace)
    jpeg_simple_progression(&jpeg_info);
#endif
  if ((image->compression == LosslessJPEGCompression) ||
      (image_info->quality > 100))
#if defined(C_LOSSLESS_SUPPORTED)
    if (image_info->quality < 100)
       MagickWarning(OptionWarning,
         "Lossless JPEG is being converted to lossy JPEG",(char *) NULL);
    else
      {
        int
          point_transform,
          predictor;

        predictor=image_info->quality/100;  /* range 1-7 */
        point_transform=image_info->quality % 20;  /* range 0-15 */
        jpeg_simple_lossless(&jpeg_info,predictor,point_transform);
      }
#else
    jpeg_set_quality(&jpeg_info,100,True);
  else
#endif
    jpeg_set_quality(&jpeg_info,(int) image_info->quality,True);
  jpeg_start_compress(&jpeg_info,True);
  /*
    Write JPEG profiles.
  */
  attribute=GetImageAttribute(image,"comment");
  if ((attribute != (ImageAttribute *) NULL) && (attribute->value != NULL))
    for (i=0; i < strlen(attribute->value); i+=65533)
      jpeg_write_marker(&jpeg_info,JPEG_COM,(unsigned char *) attribute->value+
        i,(int) Min(strlen(attribute->value+i),65533));
  if (image->color_profile.length != 0)
    WriteICCProfile(&jpeg_info,image);
  if (image->iptc_profile.length != 0)
    WriteIPTCProfile(&jpeg_info,image);
  for (i=0; i < image->generic_profiles; i++)
  {
    register long
      j;

    if (LocaleNCompare(image->generic_profile[i].name,"APP",3) != 0)
      continue;
    x=atol(image->generic_profile[i].name+3);
    for (j=0; j < image->generic_profile[i].length; j+=65533)
      jpeg_write_marker(&jpeg_info,JPEG_APP0+(int) x,
        image->generic_profile[i].info+j,(int)
        Min(image->generic_profile[i].length-j,65533));
  }
  /*
    Convert MIFF to JPEG raster pixels.
  */
  jpeg_pixels=(JSAMPLE *)
    AcquireMemory(jpeg_info.input_components*image->columns*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
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
            *q++=(JSAMPLE) (Intensity(*p)/16);
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
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
              *q++=(JSAMPLE) (p->red/16);
              *q++=(JSAMPLE) (p->green/16);
              *q++=(JSAMPLE) (p->blue/16);
              p++;
            }
            (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
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
              *q++=(JSAMPLE) (MaxRGB-(p->red/16));
              *q++=(JSAMPLE) (MaxRGB-(p->green/16));
              *q++=(JSAMPLE) (MaxRGB-(p->blue/16));
              *q++=(JSAMPLE) (MaxRGB-(p->opacity/16));
              p++;
            }
            (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
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
          *q++=(JSAMPLE) DownScale(Intensity(*p));
          p++;
        }
        (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
        if (QuantumTick(y,image->rows))
          MagickMonitor(SaveImageText,y,image->rows);
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
            *q++=(JSAMPLE) DownScale(p->red);
            *q++=(JSAMPLE) DownScale(p->green);
            *q++=(JSAMPLE) DownScale(p->blue);
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
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
            *q++=(JSAMPLE) (MaxRGB-DownScale(p->red));
            *q++=(JSAMPLE) (MaxRGB-DownScale(p->green));
            *q++=(JSAMPLE) (MaxRGB-DownScale(p->blue));
            *q++=(JSAMPLE) (MaxRGB-DownScale(p->opacity));
            p++;
          }
          (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
        }
  jpeg_finish_compress(&jpeg_info);
  /*
    Free memory.
  */
  jpeg_destroy_compress(&jpeg_info);
  LiberateMemory((void **) &jpeg_pixels);
  CloseBlob(image);
  return(True);
}
#else
static unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateWarning,"JPEG library is not available",
    image->filename);
}
#endif
