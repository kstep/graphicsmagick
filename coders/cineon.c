/*
% Copyright (C) 2003 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                  C        I    NN  N  E     O   O  NN  N                    %
%                  C        I    N N N  EEEE  O   O  N N N                    %
%                  C        I    N  NN  E     O   O  N  NN                    %
%                   CCCC  IIIII  N   N  EEEEE  OOO   N   N                    %
%                                                                             %
%                                                                             %
%                    Read/Write Kodak Cineon Image Format.                    %
%                Cineon Image Format is a subset of SMTPE DPX                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                             Kelly Bergougnoux                               %
%                      <three3@users.sourceforge.net>                         %
%                               October 2003                                  %
%                                                                             %
%                                                                             %
%  Copyright (C) 1999-2003 ImageMagick Studio, a non-profit organization      %
%  dedicated to making software imaging solutions freely available.           %
%                                                                             %
%  This software and documentation is provided "as is," and the copyright     %
%  holders and contributing author(s) make no representations or warranties,  %
%  express or implied, including but not limited to, warranties of            %
%  merchantability or fitness for any particular purpose or that the use of   %
%  the software or documentation will not infringe any third party patents,   %
%  copyrights, trademarks or other rights.                                    %
%                                                                             %
%  The copyright holders and contributing author(s) will not be held liable   %
%  for any direct, indirect, special or consequential damages arising out of  %
%  any use of the software or documentation, even if advised of the           %
%  possibility of such damage.                                                %
%                                                                             %
%  Permission is hereby granted to use, copy, modify, and distribute this     %
%  source code, or portions hereof, documentation and executables, for any    %
%  purpose, without fee, subject to the following restrictions:               %
%                                                                             %
%    1. The origin of this source code must not be misrepresented.            %
%    2. Altered versions must be plainly marked as such and must not be       %
%       misrepresented as being the original source.                          %
%    3. This Copyright notice may not be removed or altered from any source   %
%       or altered source distribution.                                       %
%                                                                             %
%  The copyright holders and contributing author(s) specifically permit,      %
%  without fee, and encourage the use of this source code as a component for  %
%  supporting image processing in commercial products.  If you use this       %
%  source code in a product, acknowledgment is not required but would be      %
%  be appreciated.                                                            %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Cineon image file format draft is available at
%  http://www.cineon.com/ff_draft.php.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/bit_stream.h"
#include "magick/pixel_cache.h"
#include "magick/magick_endian.h"
#include "magick/error.h"
#include "magick/list.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/profile.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteCINEONImage(const ImageInfo *,Image *);

typedef char ASCII;
typedef magick_uint8_t U8;
typedef magick_uint16_t U16;
typedef magick_uint32_t U32;
typedef magick_int32_t S32;
typedef float R32;
typedef unsigned int sample_t;

#define SET_UNDEFINED_U8(value)  (value=~0)
#define SET_UNDEFINED_U16(value) (value=~0)
#define SET_UNDEFINED_U32(value) (value=~0)
#define SET_UNDEFINED_S32(value) (value=0x80000000)
#define SET_UNDEFINED_R32(value) (*((U32 *) &value)=0x7F800000U)
#define SET_UNDEFINED_ASCII(value) (memset(value,0,sizeof(value)))

#define IS_UNDEFINED_U8(value) (value == ((U8) ~0))
#define IS_UNDEFINED_U16(value) (value == ((U16) ~0))
#define IS_UNDEFINED_U32(value) (value == ((U32) ~0))
#define IS_UNDEFINED_S32(value) (value == ((S32) 0x80000000))
#define IS_UNDEFINED_R32(value) (*((U32 *) &value) == ((U32) 0x7F800000U))
#define IS_UNDEFINED_ASCII(value) (!(value[0] > 0))

typedef struct _CineonFileInfo
{
  U32   magic;                     /* Magick number (0x802A5FD7) */
  U32   image_data_offset;         /* Offset to image data in bytes */
  U32   generic_section_length;    /* Generic section header length in bytes */
  U32   industry_section_length;   /* Industry specific header length in bytes */
  U32   user_defined_length;       /* User defined header length in bytes */
  U32   file_size;                 /* Total image file size in bytes  */
  ASCII header_format_version[8];  /* Version number of header format */
  ASCII image_filename[100];       /* Image filename */
  ASCII creation_date[12];         /* Creation date: yyyy:mm:dd */
  ASCII creation_time[12];         /* Creation time: hh:mm:ssLTZ */
  ASCII reserved[36];              /* Reserved for future use */
} CineonFileInfo;

typedef struct _CineonImageChannel
{
  U8    designator_byte_0;         /* 0 = universal metric */
  U8    designator_byte_1;         /* */
  U8    bits_per_sample;           /* Bit depth */
  U8    unused;
  U32   pixels_per_line;           /* Pixels per line (columns) */
  U32   lines_per_image;           /* Lines per image (rows) */
  R32   reference_low_data_code;   /* Reference low data code value */
  R32   reference_low_quantity;    /* Low quantity represented */
  R32   reference_high_data_code;  /* Reference high data code value */
  R32   reference_high_quantity;   /* Reference high quantity represented */
} DPXImageChannel;

typedef struct _CineonImageInfo
{
  U8    orientation;               /* Image orientation */
  U8    channels;                  /* Number of image channels (1-8) */
  U8    pad[2];                    /* Unused (2 byte space for word allignment */
  DPXImageChannel channel_info[8]; /* Description of channels */
  R32   white_point[2];            /* White point (color temperature) - x,y pair */
  R32   red_primary_chromaticity[2]; /* Red primary chromaticity - x,y pair */
  R32   green_primary_chromaticity[2]; /* Green primary chromaticity - x,y pair */
  R32   blue_primary_chromaticity[2]; /* Blue primary chromaticity - x,y pair */
  ASCII label_text[200];           /* Label text */
  ASCII reserved[28];              /* Reserved for future use */
  /* Image Data Format Information */
  U8    data_interleave;           /* Data interleave */
  U8    packing;                   /* Packing method */
  U8    sign;                      /* Data sign: 0=unsigned, 1=signed */
  U8    sense;                     /* Image sense: 0=positive, 1=negative */
  U32   eol_pad;                   /* End of line padding */
  U32   eoc_pad;                   /* End of channel padding */
  ASCII reserved2[20];             /* Reserved for future use */  
} CineonImageInfo;

typedef struct _CineonImageOriginationInfo
{
  S32   x_offset;                  /* X offset */
  S32   y_offset;                  /* Y offset */
  ASCII source_image_filename[100];/* Source image filename */
  ASCII creation_date[12];         /* Creation date: yyyy:mm:dd */
  ASCII creation_time[12];         /* Creation time: hh:mm:ssLTZ */
  ASCII input_device[64];          /* Input device */
  ASCII input_device_model[32];    /* Input device model number */
  ASCII input_device_serial[32];   /* Input device serial number */
  R32   input_device_pitch_x;      /* Input device pitch for X (samples/mm) */
  R32   input_device_pitch_y;      /* Input device pitch for Y (samples/mm) */
  R32   gamma;                     /* Image gamma of capture device */
  ASCII reserved[40];              /* Reserved for future use */
} CineonImageOriginationInfo;

typedef struct _CineonFilmInfo
{
  U8    film_mfg_id_code;          /* Film mfg. ID code (2 digits from film edge code) */
  U8    film_type;                 /* Film type (2 digits from film edge code) */
  U8    perfs_offset;              /* Offset in perfs (2 digits from film edge code) */
  U8    unused;                    /* Unused, for word allignment */
  U32   prefix;                    /* Prefix (6 digits from film edge code) */
  U32   count;                     /* Count (4 digits from film edge code) */
  ASCII format[32];                /* Format -- e.g. Academy */
  U32   frame_position;            /* Frame position in sequence */
  R32   frame_rate;                /* Frame rate of original (frames/s) */
  ASCII frame_id[32];              /* Frame identification - e.g. keyframe */
  ASCII slate_info[200];           /* Slate information */
  ASCII reserved[740];             /* Reserved for future use */
} CineonFilmInfo;


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s C I N E O N                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsCINEON() returns True if the image format type, identified by the magick
%  string, is CINEON.
%
%  The format of the IsCINEON method is:
%
%      unsigned int IsCINEON(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsCINEON(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\200\052\137\327",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C I N E O N I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadCINEONImage() reads an CINEON X image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a point to the
%  new image.
%
%  The format of the ReadCINEONImage method is:
%
%      Image *ReadCINEONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define LogSetImageAttribute(name,value) \
{ \
  (void) LogMagickEvent(CoderEvent,GetMagickModule(), \
                        "Attribute \"%s\" set to \"%s\"", \
                        name,value); \
}

#define StringToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_ASCII(member)) \
    { \
      strlcpy(buffer,member,Min(sizeof(member)+1,MaxTextExtent)); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define U8ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U8(member)) \
    { \
      FormatString(buffer,"%u",(unsigned int) member); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define U16ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U16(member)) \
    { \
      FormatString(buffer,"%u",(unsigned int) member); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define U32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_U32(member)) \
    { \
      FormatString(buffer,"%u",member); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define R32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_R32(member)) \
    { \
      FormatString(buffer,"%g",member); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
#define S32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (!IS_UNDEFINED_S32(member)) \
    { \
      FormatString(buffer,"%d",member); \
      SetImageAttribute(image,name,buffer); \
      LogSetImageAttribute(name,buffer); \
    } \
}
static void SwabCineonFileInfo(CineonFileInfo *file_info)
{
  MagickSwabUInt32(&file_info->magic);
  MagickSwabUInt32(&file_info->image_data_offset);
  MagickSwabUInt32(&file_info->generic_section_length);
  MagickSwabUInt32(&file_info->industry_section_length);
  MagickSwabUInt32(&file_info->user_defined_length);
  MagickSwabUInt32(&file_info->file_size);
}

static void SwabCineonImageInfo(CineonImageInfo *image_info)
{
  unsigned int
    i;

  for (i=0 ; i < (unsigned int) image_info->channels  ; i++)
    {
      MagickSwabUInt32(&image_info->channel_info[i].pixels_per_line);
      MagickSwabUInt32(&image_info->channel_info[i].lines_per_image);
      MagickSwabFloat(&image_info->channel_info[i].reference_low_data_code);
      MagickSwabFloat(&image_info->channel_info[i].reference_low_quantity);
      MagickSwabFloat(&image_info->channel_info[i].reference_high_data_code);
      MagickSwabFloat(&image_info->channel_info[i].reference_high_quantity);
    }

  MagickSwabFloat(&image_info->white_point[0]);
  MagickSwabFloat(&image_info->white_point[1]);
  MagickSwabFloat(&image_info->red_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->red_primary_chromaticity[1]);
  MagickSwabFloat(&image_info->green_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->green_primary_chromaticity[1]);
  MagickSwabFloat(&image_info->blue_primary_chromaticity[0]);
  MagickSwabFloat(&image_info->blue_primary_chromaticity[1]);
  MagickSwabUInt32(&image_info->eol_pad);
  MagickSwabUInt32(&image_info->eoc_pad);  
}

static void SwabCineonImageOriginationInfo(CineonImageOriginationInfo *image_info)
{
  MagickSwabUInt32((U32 *) &image_info->x_offset);
  MagickSwabUInt32((U32 *) &image_info->y_offset);
  MagickSwabFloat(&image_info->input_device_pitch_x);
  MagickSwabFloat(&image_info->input_device_pitch_y);
  MagickSwabFloat(&image_info->gamma);
}

static void SwabCineonFilmInfo(CineonFilmInfo *mp_info)
{
  MagickSwabUInt32(&mp_info->prefix);
  MagickSwabUInt32(&mp_info->count);
  MagickSwabUInt32(&mp_info->frame_position);
  MagickSwabFloat(&mp_info->frame_rate);
}

static Image *ReadCINEONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  CineonFileInfo
    cin_file_info;

  CineonImageInfo
    cin_image_info;

  CineonImageOriginationInfo
    cin_source_info;

  CineonFilmInfo
    cin_mp_info;

  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    offset;    

  unsigned int
    channel,
    max_bits_per_sample,
    max_lines_per_image,
    max_pixels_per_line,
    number_of_channels;    

  MagickPassFail
    status;

  MagickBool
    swab=MagickFalse;

  unsigned long
    pixels_offset;

  unsigned char
    *scandata;
  
  unsigned char
    *scanline;

  const char *
    definition_value;

  size_t
    scandata_bytes;
  
  BitStreamReadHandle
    bit_stream;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
#if !defined(WORDS_BIGENDIAN)
  swab=MagickTrue;
#endif /* !defined(WORDS_BIGENDIAN) */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    {
      DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read CINEON image.
  */
  offset=(ReadBlob(image,sizeof(cin_file_info),&cin_file_info));
  if (offset != sizeof(cin_file_info))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  if (swab)
    SwabCineonFileInfo(&cin_file_info);

  if (cin_file_info.magic != 0x802A5FD7)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Image offset %u, Generic length %u, Industry length %u, User length %u",
                        cin_file_info.image_data_offset,
                        cin_file_info.generic_section_length,
                        cin_file_info.industry_section_length,
                        cin_file_info.user_defined_length);

  StringToAttribute(image,"document",cin_file_info.image_filename);
  StringToAttribute(image,"DPX:file.filename",cin_file_info.image_filename);
  {
    char
      creation_datetime[24];

    creation_datetime[0]='\0';
    strlcat(creation_datetime,cin_file_info.creation_date,sizeof(cin_file_info.creation_date)+1);
    if (creation_datetime[0]!='\0')
      strlcat(creation_datetime,":",sizeof(creation_datetime));
    strlcat(creation_datetime,cin_file_info.creation_time,sizeof(creation_datetime));
    StringToAttribute(image,"timestamp",creation_datetime);
    StringToAttribute(image,"DPX:file.creation.datetime",creation_datetime);
  }

  /*
    Obtain offset to pixels.
  */
  pixels_offset=cin_file_info.image_data_offset;
  if (pixels_offset < 712)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);

  /*
    Read image information header.
  */
  offset += ReadBlob(image,sizeof(cin_image_info),&cin_image_info);
  if (offset != (size_t) 712L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabCineonImageInfo(&cin_image_info);
  number_of_channels=cin_image_info.channels;
  U8ToAttribute(image,"DPX:image.orientation",cin_image_info.orientation);
  max_bits_per_sample=0;
  max_pixels_per_line=0;
  max_lines_per_image=0;
  for (channel=0; channel < number_of_channels; channel++)
    {
      max_bits_per_sample=Max(max_bits_per_sample,
                              cin_image_info.channel_info[channel].bits_per_sample);
      max_pixels_per_line=Max(max_pixels_per_line,
                              cin_image_info.channel_info[channel].pixels_per_line);
      max_lines_per_image=Max(max_lines_per_image,
                              cin_image_info.channel_info[channel].lines_per_image);
    }
  
  image->depth=max_bits_per_sample;
  image->columns=max_pixels_per_line;
  image->rows=max_lines_per_image;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Columns %ld, Rows %ld, Channels %u",
                        image->columns, image->rows,
                        (unsigned int) cin_image_info.channels);
  /*
    Read image origination header.
  */
  offset += ReadBlob(image,sizeof(cin_source_info),&cin_source_info);
  if (offset != (size_t) 1024L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabCineonImageOriginationInfo(&cin_source_info);
  S32ToAttribute(image,"DPX:source.x-offset",cin_source_info.x_offset);
  S32ToAttribute(image,"DPX:source.y-offset",cin_source_info.y_offset);
  StringToAttribute(image,"DPX:source.filename",cin_source_info.source_image_filename);
  {
    char
      source_creation_datetime[MaxTextExtent];

    source_creation_datetime[0]='\0';
    strlcat(source_creation_datetime,cin_file_info.creation_date,sizeof(cin_file_info.creation_date)+1);
    if (source_creation_datetime[0]!='\0')
      strlcat(source_creation_datetime,":",sizeof(source_creation_datetime));
    strlcat(source_creation_datetime,cin_file_info.creation_time,sizeof(source_creation_datetime));
    StringToAttribute(image,"DPX:source.creation.datetime",source_creation_datetime);
  }
  StringToAttribute(image,"DPX:source.device.name",cin_source_info.input_device);
  StringToAttribute(image,"DPX:source.device.model",cin_source_info.input_device_model);
  StringToAttribute(image,"DPX:source.device.serialnumber",cin_source_info.input_device_serial);
  R32ToAttribute(image,"DPX:source.device.pitch.x",cin_source_info.input_device_pitch_x);
  R32ToAttribute(image,"DPX:source.device.pitch.y",cin_source_info.input_device_pitch_y);
  R32ToAttribute(image,"DPX:source.device.gamma",cin_source_info.gamma);

  if ((pixels_offset >= 2048) && (cin_file_info.industry_section_length > 0))
    {
      /*
        Read Motion-picture film information header.
      */
      offset += ReadBlob(image,sizeof(cin_mp_info),&cin_mp_info);
      if (offset != (size_t) 2048L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabCineonFilmInfo(&cin_mp_info);
      U8ToAttribute(image,"DPX:mp.film.manufacturer.id",cin_mp_info.film_mfg_id_code);
      U8ToAttribute(image,"DPX:mp.film.type",cin_mp_info.film_type);
      U8ToAttribute(image,"DPX:mp.perfs.offset",cin_mp_info.perfs_offset);
      U32ToAttribute(image,"DPX:mp.prefix",cin_mp_info.prefix);
      U32ToAttribute(image,"DPX:mp.count",cin_mp_info.count);
      StringToAttribute(image,"DPX:mp.format",cin_mp_info.format);
      U32ToAttribute(image,"DPX:mp.frame.position",cin_mp_info.frame_position);
      R32ToAttribute(image,"DPX:mp.frame.rate",cin_mp_info.frame_rate);
      StringToAttribute(image,"DPX:mp.frame.id",cin_mp_info.frame_id);
      StringToAttribute(image,"DPX:mp.slate.info",cin_mp_info.slate_info);
    }

  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }

  /*
    Read remainder of header.
  */
  for ( ; offset < pixels_offset ; offset++ )
    (void) ReadBlobByte(image);

  /*
    Convert CINEON raster image to pixel packets.
  */
  switch (number_of_channels)
    {
    case 1:
      {
        scandata_bytes=4;
        scandata=MagickAllocateMemory(unsigned char *,scandata_bytes);
        for (y=0; y < (long) image->rows; y++)
          {
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            /*
              Packed 10 bit samples with 2 bit pad at end of 32-bit word.
            */
            scanline=(void *) scandata;
            i=3;
            for (x=(long) image->columns; x > 0; x--, i++)
              {
                if (i > 2)
                  {
                    scanline=scandata;
                    if (ReadBlobZC(image,scandata_bytes,(void *) &scanline) !=
                        scandata_bytes)
                      break;
                    BitStreamInitializeRead(&bit_stream,scanline);
                    i=0;
                  }
                q->red=q->green=q->blue=
                  ScaleShortToQuantum(BitStreamMSBRead(&bit_stream,10)*64);
                q->opacity=0U;
                q++;
              }
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                  break;
          }
        MagickFreeMemory(scandata);
        break;
      }
    case 3:
      {
        scandata_bytes=image->columns*4;
        scandata=MagickAllocateMemory(unsigned char *,scandata_bytes);
        for (y=0; y < (long) image->rows; y++)
          {
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            scanline=scandata;
            if (ReadBlobZC(image,scandata_bytes,(void *) &scanline) != scandata_bytes)
              break;
            for (x=0 ; x < (long) image->columns; x++)
              {
                /*
                  Packed 10 bit samples with 2 bit pad at end of 32-bit word.
                */
#if 0
                if ((x == 0) && (y == 0))
                  {
                    printf("%x %x %x %x\n",
                           ((unsigned char *)scanline)[0],
                           ((unsigned char *)scanline)[1],
                           ((unsigned char *)scanline)[2],
                           ((unsigned char *)scanline)[3]);
                  }
#endif
                BitStreamInitializeRead(&bit_stream,scanline);
                q->red=ScaleShortToQuantum(BitStreamMSBRead(&bit_stream,10)*64);
                q->green=ScaleShortToQuantum(BitStreamMSBRead(&bit_stream,10)*64);
                q->blue=ScaleShortToQuantum(BitStreamMSBRead(&bit_stream,10)*64);
                q->opacity=0U;
                scanline += 4;
                q++;
              }
            if (!SyncImagePixels(image))
              break;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                  break;
          }
        MagickFreeMemory(scandata);
        break;
      }
    default:
      ThrowReaderException(CorruptImageError,ImageTypeNotSupported,image);
    }
  image->depth=Min(image->depth,QuantumDepth);
  image->colorspace=CineonLogRGBColorspace;

  if ((definition_value=AccessDefinition(image_info,"cineon","colorspace")))
    {
      ColorspaceType
        colorspace;

      colorspace=StringToColorspaceType(definition_value);
      if (colorspace != UndefinedColorspace)
        {
          image->colorspace=colorspace;
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Explicitly set colorspace to %s",
                                ColorspaceTypeToString(image->colorspace));
        }
      else
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Unrecognized source colorspace \"%s\"\n",
                                definition_value);
          ThrowException(&image->exception,OptionError,UnrecognizedColorspace,
                         definition_value);
        }
    }

  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
  CloseBlob(image);
  return(GetFirstImageInList(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C I N E O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCINEONImage adds attributes for the CINEON image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCINEONImage method is:
%
%      RegisterCINEONImage(void)
%
*/
ModuleExport void RegisterCINEONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CIN");
  entry->decoder=(DecoderHandler) ReadCINEONImage;
  entry->encoder=(EncoderHandler) WriteCINEONImage;
  entry->magick=(MagickHandler) IsCINEON;
  entry->description=AcquireString("Cineon Image File");
  entry->module=AcquireString("CINEON");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C I N E O N I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCINEONImage removes format registrations made by the
%  CINEON module from the list of supported formats.
%
%  The format of the UnregisterCINEONImage method is:
%
%      UnregisterCINEONImage(void)
%
*/
ModuleExport void UnregisterCINEONImage(void)
{
  (void) UnregisterMagickInfo("CIN");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C I N E O N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteCINEONImage() writes an image in CINEON encoded image format.
%
%  The format of the WriteCINEONImage method is:
%
%      unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: The image info.
%
%    o image:  The image.
%
%
*/
static unsigned int WriteCINEONImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;
  
  register long
    i,
    x;

  unsigned int
    status;
 
  unsigned long
    pixel;

  unsigned int
    maxr,
    maxg,
    maxb,
    minr,
    ming,
    minb;

  /* 
     Get the max values for each component... I guess it "needs" it...
  */
  maxr=maxg=maxb=0;
  minr=ming=minb=MaxRGB;
  for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
        {
          if ( p->red > maxr ) maxr = p->red;
          if ( p->green > maxg ) maxg = p->green;
          if ( p->blue > maxb ) maxb = p->blue;
          if ( p->red < minr ) minr = p->red;
          if ( p->green < ming ) ming = p->green;
          if ( p->blue < minb ) minb = p->blue;
          p++;
        }
    }
  /*
    Div by 64 to get 10bit values since that's all I'll do right now.
  */
  maxr=ScaleQuantumToShort(maxr)/64;
  maxg=ScaleQuantumToShort(maxg)/64;
  maxb=ScaleQuantumToShort(maxb)/64;
  minr=ScaleQuantumToShort(minr)/64;
  ming=ScaleQuantumToShort(ming)/64;
  minb=ScaleQuantumToShort(minb)/64;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    return(status);
  /* Magick Number... */
  (void) (void) TransformColorspace(image,RGBColorspace);
  (void) WriteBlobMSBLong(image,0x802A5FD7);
  /* Offset to image */
  (void) WriteBlobMSBLong(image,0x0800);
  /* Generic section header length */
  (void) WriteBlobMSBLong(image,0x400);	
  /* Industry Specific lenght */
  (void) WriteBlobMSBLong(image,0x400);	
  /* variable length section */
  (void) WriteBlobMSBLong(image,0x0);
  /* Total image file size */
  (void) WriteBlobMSBLong(image,4*image->columns*image->rows+0x2000); 
  /* Version number of header format */
  (void) WriteBlobString(image,"V4.5");	
  (void) WriteBlobMSBLong(image,0x0);
  /* Filename */
  (void) WriteBlobString(image,image->filename);
  for ( i =0 ; i < ( 100 - (long) strlen(image->filename) ); i++ )
    (void) WriteBlobByte(image,0);
  /* Creation Date. */
  (void) WriteBlobString(image,"yyyy:mm:dd  ");
  /* Creation Time. */
  (void) WriteBlobString(image,"hh:mm:ssxxx ");	
  for ( i =0 ; i < 36 ; i++)
    (void) WriteBlobByte(image,0);
  /* 0 left to right top to bottom */
  (void) WriteBlobByte(image,0);
  /* 3 channels */
  (void) WriteBlobByte(image,3);
  /* alignment */
  (void) WriteBlobByte(image,0x0);
  (void) WriteBlobByte(image,0x0);
  /* Do R */ 
  /* Channel 1 designator byte 0 */
  (void) WriteBlobByte(image,0);
  /* Channel 1 designator byte 1 */
  (void) WriteBlobByte(image,1);
  /* Bits per pixel... */
  (void) WriteBlobByte(image,10);
  /* alignment */
  (void) WriteBlobByte(image,0);
  /* pixels per line */
  (void) WriteBlobMSBLong(image,image->columns);
  /* lines per image */
  (void) WriteBlobMSBLong(image,image->rows);
  /* Minimum data value */
  (void) WriteBlobMSBLong(image,minr);
  /* Minimum quantity represented */
  (void) WriteBlobMSBLong(image,0x0);
  /* Maximum data value */
  (void) WriteBlobMSBLong(image,maxr);
  /* Maximum quantity represented */
  (void) WriteBlobMSBLong(image,0x40000000);
  /* Do G */ 
  /* see above. */
  (void) WriteBlobByte(image,0);
  (void) WriteBlobByte(image,2);
  (void) WriteBlobByte(image,10);
  (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows);   
  (void) WriteBlobMSBLong(image,ming);	
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,maxg);
  (void) WriteBlobMSBLong(image,0x40000000);
  /* Go B */
  /* see above. */
  (void) WriteBlobByte(image,0);
  (void) WriteBlobByte(image,3);
  (void) WriteBlobByte(image,10);
  (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows); 
  (void) WriteBlobMSBLong(image,minb);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,maxb);
  (void) WriteBlobMSBLong(image,0x40000000);
  /* pad channel 4-8 */
  for (i=0; i < 5*28; i++)
    (void) WriteBlobByte(image,0x00);
  /* k here's where it gets ugly and I should be shot */
  /* White point(colour temp.) x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* White point(colour temp.) y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Red primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Red primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Green primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Green primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Blue primary chromaticity x */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Blue primary chromaticity y */
  (void) WriteBlobMSBLong(image,0x4EFF0000);
  /* Label Text... */
  for (i=0; i < 200+28; i++)
    (void) WriteBlobByte(image,0x00);
  /* pixel interleave (rgbrgbr...) */
  (void) WriteBlobByte(image,0);
  /* Packing longword (32bit) boundaries */
  (void) WriteBlobByte(image,5);
  /* Data unsigned */
  (void) WriteBlobByte(image,0);
  /* Image sense: positive image */
  (void) WriteBlobByte(image,0);
  /* End of line padding */
  (void) WriteBlobMSBLong(image,0x0);
  /* End of channel padding */
  (void) WriteBlobMSBLong(image,0x0);
  /* Reseved for future Use.. */
  for (i=0; i < 20; i++)
    (void) WriteBlobByte(image,0x00);

  /*
    Generic Orientation Header
  */

  /* X offset */
  (void) WriteBlobMSBLong(image,0x0);
  /* Y offset */
  (void) WriteBlobMSBLong(image,0x0);
  /* Filename */
  (void) WriteBlobString(image,image->filename);
  for ( i =0 ; i < ( 100 - (long) strlen(image->filename) ); i++ ) 
    (void) WriteBlobByte(image,0);
  /* date. who cares ? */
  for ( i =0 ; i < 12 ; i++ ) 
    (void) WriteBlobByte(image,0);
  /* time who cares ? */
  for ( i =0 ; i < 12 ; i++ ) 
    (void) WriteBlobByte(image,0);
  (void) WriteBlobString(image,"GraphicsMagick");
  for (i = 0; i < 64-11 ; i++)
    (void) WriteBlobByte(image,0);
  for (i = 0; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  for (i = 0; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  /* X input device pitch */
  (void) WriteBlobMSBLong(image,0x4326AB85);
  /* Y input device pitch */
  (void) WriteBlobMSBLong(image,0x4326AB85);
  /* Image gamma */
  (void) WriteBlobMSBLong(image,0x3F800000);
  /* Reserved for future use */
  for (i = 0; i < 40 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 4 ; i++)
    (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,0x0);
  for ( i = 0 ; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  (void) WriteBlobMSBLong(image,0x0);
  (void) WriteBlobMSBLong(image,0x0);
  for ( i = 0 ; i < 32 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 200 ; i++)
    (void) WriteBlobByte(image,0);
  for ( i = 0 ; i < 740 ; i++)
    (void) WriteBlobByte(image,0);
  /*
    Convert pixel packets to CINEON raster image.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
        {
#if 0
          BitStreamInitializeWrite(&bit_stream,packet);
          BitStreamMSBWrite(&bit_stream,10,(unsigned int) ScaleQuantumToShort(p->red)/64U);
          BitStreamMSBWrite(&bit_stream,10,(unsigned int) ScaleQuantumToShort(p->green)/64U);
          BitStreamMSBWrite(&bit_stream,10,(unsigned int) ScaleQuantumToShort(p->blue)/64U);
/*           if ((x == 0) && (y == 0)) */
/*             printf("%x %x %x %x\n",packet[0],packet[1],packet[2],packet[3]); */
          (void) WriteBlob(image,4,packet);
#else
          pixel=(unsigned long)
            (((long) ((1023L*p->red+MaxRGB/2)/MaxRGB) & 0x3ff) << 22) |
            (((long) ((1023L*p->green+MaxRGB/2)/MaxRGB) & 0x3ff) << 12) |
            (((long) ((1023L*p->blue+MaxRGB/2)/MaxRGB) & 0x3ff) << 2);
          (void) WriteBlobMSBLong(image,pixel);
#endif
          p++;
        }
    }
  CloseBlob(image);  
  return(status);
}
