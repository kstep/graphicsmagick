/*
% Copyright (C) 2005 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            DDDD   PPPP   X   X                              %
%                            D   D  P   P   X X                               %
%                            D   D  PPPP    XXX                               %
%                            D   D  P       X X                               %
%                            DDDD   P      X   X                              %
%                                                                             %
%                                                                             %
%               Read/Write SMTPE 268M-2003 DPX Image Format.                  %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                March 2005                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
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
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteDPXImage(const ImageInfo *,Image *);

#define MSBOrderFloat(a,b) /* FIXME */

typedef magick_uint8_t U8;
typedef magick_uint16_t U16;
typedef magick_uint32_t U32;
typedef float R32;

typedef struct _DPXFileInfo
{
  U32  magic;                     /* Magick number (SDPX ASCII) */
  U32  image_data_offset;         /* Offset to image data in bytes */
  char header_format_version[8];  /* Version number of header format */
  U32  file_size;                 /* Total image file size in bytes  */
  U32  ditto_key;                 /* (0 = same as previous frame; 1 = new) */
  U32  generic_section_length;    /* Generic section header length in bytes */
  U32  industry_section_length;   /* Industry specific header length in bytes */
  U32  user_defined_length;       /* User defined header length in bytes */
  char image_filename[100];       /* Image filename */
  char creation_datetime[24];     /* Creation date/time: yyyy:mm:dd:hh:mm:ssLTZ */
  char creator[100];              /* Creator */
  char project_name[200];         /* Project name */
  char copyright[200];            /* Right to use or copyright */
  U32  encrption_key;             /* Enscryption key (FFFFFFFF unencrypted ) */
  char reserved[104];             /* Reserved for future use */
} DPXFileInfo;

typedef struct _DPXImageElement
{
  U32  data_sign;                 /* Data sign (0 = unsigned; 1 = signed) */
  U32  reference_low_data_code;   /* Reference low data code value */
  R32  reference_low_quantity;    /* Low quantity represented */
  U32  reference_high_data_code;  /* Reference high data code value */
  R32  reference_high_quantity;   /* Reference high quantity represented */
  U8   descriptor;                /* Descriptor */
  U8   transfer_characteristic;   /* Transfer characteristic */
  U8   colorimetric;              /* Colorimetric specification */
  U8   bits_per_sample;           /* Bit depth */
  U16  packing;                   /* Packing */
  U16  encoding;                  /* Encoding */
  U32  data_offset;               /* Offset to data */
  U32  eol_pad;                   /* End of line padding */
  U32  eoi_pad;                   /* End of image padding */
  char description[32];           /* Description of image element */
} DPXImageElement;

typedef struct _DPXImageInfo
{
  U16  orientation;               /* Image orientation */
  U16  elements;                  /* Number of image elements (1-8) */
  U32  pixels_per_line;           /* Pixels per line (columns) */
  U32  lines_per_image_element;   /* Lines per image element (rows) */
  DPXImageElement element_info[8];/* Description of elements */
  char reserved[52];              /* Reserved for future use */
} DPXImageInfo;

typedef struct _DPXImageSourceBorderValidity
{
  U16  XL;                        /* Border validity XL border */
  U16  XR;                        /* Border validity XR border */
  U16  YT;                        /* Border validity YT border */
  U16  YB;                        /* Border validity YB border */
} DPXImageSourceBorderValidity;

typedef struct _DPXImageSourcePixelAspectRatio
{
  U32  horizontal;                /* Horizontal */
  U32  vertical;                  /* Vertical */
} DPXImageSourcePixelAspectRatio;

typedef struct _DPXImageSourceInfo
{
  U32  x_offset;                  /* X offset */
  U32  y_offset;                  /* Y offset */
  R32  x_center;                  /* X center */
  R32  y_center;                  /* Y center */
  U32  x_original_size;           /* X original size */
  U32  y_original_size;           /* Y original size */
  char source_image_filename[100];/* Source image filename */
  char source_image_datetime[24]; /* Source image date/time: yyyy:mmm:dd:hh:mm:ssLTZ */
  char input_device_name[32];     /* Input device name */
  char input_device_serialnumber[32]; /* Input device serial number */
  DPXImageSourceBorderValidity border_validity; /* Border validity */
  DPXImageSourcePixelAspectRatio aspect_ratio; /* Aspect ratio */
  R32  x_scanned_size;            /* X scanned size */
  R32  y_scanned_size;            /* Y scanned size */
  char reserved[20];              /* Reserved for future use */
} DPXImageSourceInfo;

typedef struct _DPXMPFilmInfo
{
  char film_mfg_id_code[2];       /* Film mfg. ID code (2 digits from film edge code) */
  char film_type[2];              /* Film type (2 digits from film edge code) */
  char perfs_offset[2];           /* Offset in perfs (2 digits from film edge code) */
  char prefix[6];                 /* Prefix (6 digits from film edge code) */
  char count[4];                  /* Count (4 digits from film edge code) */
  char format[32];                /* Format -- e.g. Academy */
  U32  frame_position;            /* Frame position in sequence */
  U32  sequence_length;           /* Sequence length (frames) */
  U32  held_count;                /* Held count (1 = default) */
  R32  frame_rate;                /* Frame rate of original (frames/s) */
  R32  shutter_angle;             /* Shutter angle of camera in degrees */
  char frame_id[32];              /* Frame identification - e.g. keyframe */
  char slate_info[100];           /* Slate information */
  char reserved[56];              /* Reserved for future use */
} DPXMPFilmInfo;

typedef struct _DPXTVInfo
{
  U32  time_code;                 /* SMPTE time code */
  U32  user_bits;                 /* SMPTE user bits */
  U8   interlace;                 /* Interlace (0 = noninterlaced; 1 = 2:1 interlace */
  U8   field_number;              /* Field number */
  U8   video_signal;              /* Video signal standard */
  U8   zero;                      /* Zero (for byte alignment) */
  R32  horizontal_sample;         /* Horizontal sampling rate */
  R32  vertical_sample;           /* Vertical sampling rate */
  R32  temporal_sample;           /* Temporal sampling rate or frame rate (Hz) */
  R32  sync_time;                 /* Time offset from sync to first pixel (ms) */
  R32  gamma;                     /* Gamma */
  R32  black_level;               /* Black level code value */
  R32  black_gain;                /* Black gain */
  R32  breakpoint;                /* Breakpoint */
  R32  white_level;               /* Reference white level code value */
  R32  integration_time;          /* Integration time (s) */
  char reserved[76];              /* Reserved for future use */
} DPXTVInfo;

typedef struct _DPXUserDefinedData
{
  char  user_id[32];              /* User identification */
  /* Up to 1MB of user-defined data after this point */
} DPXUserDefinedData;

typedef struct _DPXHeader
{
  DPXFileInfo file_info;          /* File information header */
  DPXImageInfo image_info;        /* Image information header */
  DPXImageSourceInfo source_info; /* Image source information header */
  DPXMPFilmInfo mp_info;          /* Motion picture film information header */
  DPXTVInfo tv_info;              /* Television information header */
} DPXHeader;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D P X                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDPX returns True if the image format type, identified by the
%  magick string, is DPX.
%
%  The format of the IsDPX method is:
%
%      unsigned int IsDPX(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDPX returns True if the image format type is DPX.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsDPX(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"SDPX",4) == 0)
    return(True);
  if (memcmp(magick,"XPDS",4) == 0)
    return(True);
  return(False);
}

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D P X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDPXImage reads an DPX X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadDPXImage method is:
%
%      Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDPXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define StringToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (member[0]) \
    { \
      strlcpy(buffer,member,Min(sizeof(member)+1,MaxTextExtent)); \
      SetImageAttribute(image,name,buffer); \
    } \
}
#define U16ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (member != 0xFFFFU) \
    { \
      FormatString(buffer,"%u",(unsigned int) member); \
      SetImageAttribute(image,name,buffer); \
    } \
}
#define U32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (member != 0xFFFFFFFFU) \
    { \
      FormatString(buffer,"%u",member); \
      SetImageAttribute(image,name,buffer); \
    } \
}
#define R32ToAttribute(image,name,member) \
{ \
  char \
    buffer[MaxTextExtent]; \
\
  if (member != 0xFFFFFFFFU) \
    { \
      FormatString(buffer,"%g",member); \
      SetImageAttribute(image,name,buffer); \
    } \
}
static void SwabDPXFileInfo(DPXFileInfo *file_info)
{
  MSBOrderLong((unsigned char *) &file_info->magic,1);
  MSBOrderLong((unsigned char *) &file_info->image_data_offset,1);
  MSBOrderLong((unsigned char *) &file_info->file_size,1);
  MSBOrderLong((unsigned char *) &file_info->ditto_key,1);
  MSBOrderLong((unsigned char *) &file_info->generic_section_length,1);
  MSBOrderLong((unsigned char *) &file_info->industry_section_length,1);
  MSBOrderLong((unsigned char *) &file_info->user_defined_length,1);
  MSBOrderLong((unsigned char *) &file_info->encrption_key,1);
}
static void SwabDPXImageInfo(DPXImageInfo *image_info)
{
  int
    i;

  MSBOrderShort((unsigned char *) &image_info->orientation,1);
  MSBOrderShort((unsigned char *) &image_info->elements,1);
  MSBOrderLong((unsigned char *) &image_info->pixels_per_line,1);
  MSBOrderLong((unsigned char *) &image_info->lines_per_image_element,1);
  for (i=0 ; i < 8 ; i++)
    {
      MSBOrderLong((unsigned char *) &image_info->element_info[i].data_sign,1);
      MSBOrderLong((unsigned char *) &image_info->element_info[i].reference_low_data_code,1);
      MSBOrderFloat((unsigned char *) &image_info->element_info[i].reference_low_quantity,1);
      MSBOrderLong((unsigned char *) &image_info->element_info[i].reference_high_data_code,1);
      MSBOrderFloat((unsigned char *) &image_info->element_info[i].reference_high_quantity,1);
      MSBOrderShort((unsigned char *) &image_info->element_info[i].packing,1);
      MSBOrderShort((unsigned char *) &image_info->element_info[i].encoding,1);
      MSBOrderLong((unsigned char *) &image_info->element_info[i].data_offset,1);
      MSBOrderLong((unsigned char *) &image_info->element_info[i].eol_pad,1);
      MSBOrderLong((unsigned char *) &image_info->element_info[i].eoi_pad,1);
    }
}
static void SwabDPXImageSourceInfo(DPXImageSourceInfo *source_info)
{
  MSBOrderLong((unsigned char *) &source_info->x_offset,1);
  MSBOrderLong((unsigned char *) &source_info->y_offset,1);
  MSBOrderFloat((unsigned char *) &source_info->x_center,1);
  MSBOrderFloat((unsigned char *) &source_info->y_center,1);
  MSBOrderLong((unsigned char *) &source_info->x_original_size,1);
  MSBOrderLong((unsigned char *) &source_info->y_original_size,1);
  MSBOrderShort((unsigned char *) &source_info->border_validity.XL,1);
  MSBOrderShort((unsigned char *) &source_info->border_validity.XR,1);
  MSBOrderShort((unsigned char *) &source_info->border_validity.YT,1);
  MSBOrderShort((unsigned char *) &source_info->border_validity.YB,1);
  MSBOrderShort((unsigned char *) &source_info->aspect_ratio.horizontal,1);
  MSBOrderShort((unsigned char *) &source_info->aspect_ratio.vertical,1);
  MSBOrderFloat((unsigned char *) &source_info->x_scanned_size,1);
  MSBOrderFloat((unsigned char *) &source_info->y_scanned_size,1);
}
static void SwabDPXMPFilmInfo(DPXMPFilmInfo *mp_info)
{
  MSBOrderLong((unsigned char *) &mp_info->frame_position,1);
  MSBOrderLong((unsigned char *) &mp_info->sequence_length,1);
  MSBOrderLong((unsigned char *) &mp_info->held_count,1);
  MSBOrderFloat((unsigned char *) &mp_info->frame_rate,1);
  MSBOrderFloat((unsigned char *) &mp_info->shutter_angle,1);
}
static void SwabDPXTVInfo(DPXTVInfo *tv_info)
{
  MSBOrderLong((unsigned char *) &tv_info->time_code,1);
  MSBOrderLong((unsigned char *) &tv_info->user_bits,1);
  MSBOrderFloat((unsigned char *) &tv_info->horizontal_sample,1);
  MSBOrderFloat((unsigned char *) &tv_info->vertical_sample,1);
  MSBOrderFloat((unsigned char *) &tv_info->temporal_sample,1);
  MSBOrderFloat((unsigned char *) &tv_info->sync_time,1);
  MSBOrderFloat((unsigned char *) &tv_info->gamma,1);
  MSBOrderFloat((unsigned char *) &tv_info->black_level,1);
  MSBOrderFloat((unsigned char *) &tv_info->black_gain,1);
  MSBOrderFloat((unsigned char *) &tv_info->breakpoint,1);
  MSBOrderFloat((unsigned char *) &tv_info->white_level,1);
  MSBOrderFloat((unsigned char *) &tv_info->integration_time,1);
}
static Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MonoColorType  6
#define RGBColorType  50

  DPXHeader
    header;

  char
    attribute[MaxTextExtent];

  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  size_t
    offset;    

  unsigned char
    colortype;    

  unsigned int
    status;

  unsigned long
    pixels_offset,
    pixel;

  MagickBool
    swab=MagickFalse;

  /*
    Open image file.
  */
  assert(sizeof(DPXHeader) == 2048);
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read DPX image.
  */
  offset=ReadBlob(image,sizeof(header.file_info),&header.file_info);
  if (offset != sizeof(header.file_info) ||
      ((LocaleNCompare((char *) &header.file_info.magic,"SDPX",4) != 0) &&
       (LocaleNCompare((char *) &header.file_info.magic,"XPDS",4) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Check for swapped endian order.
  */
  if (header.file_info.magic != 0x53445058)
    swab=MagickTrue;
  if (swab)
    SwabDPXFileInfo(&header.file_info);

  StringToAttribute(image,"artist",header.file_info.creator);
  StringToAttribute(image,"comment",header.file_info.project_name);
  StringToAttribute(image,"copyright",header.file_info.copyright);
  StringToAttribute(image,"document",header.file_info.image_filename);
  StringToAttribute(image,"timestamp",header.file_info.creation_datetime);

  StringToAttribute(image,"DPX:file.filename",header.file_info.image_filename);
  StringToAttribute(image,"DPX:file.creation.datetime",header.file_info.creation_datetime);
  StringToAttribute(image,"DPX:file.creator",header.file_info.creator);
  StringToAttribute(image,"DPX:file.project.name",header.file_info.project_name);
  StringToAttribute(image,"DPX:file.copyright",header.file_info.copyright);
  U32ToAttribute(image,"DPX:file.encryption.key",header.file_info.encrption_key);

  /*
    Obtain offset to pixels.
  */
  pixels_offset=header.file_info.image_data_offset;
  if (pixels_offset < 1408)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Read image information header.
  */
  offset += ReadBlob(image,sizeof(header.image_info),&header.image_info);
  if (offset != (size_t) 1408L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabDPXImageInfo(&header.image_info);
  image->columns=header.image_info.pixels_per_line;
  image->rows=header.image_info.lines_per_image_element;
  colortype=header.image_info.element_info[0].descriptor;
  image->depth=Min(QuantumDepth,header.image_info.element_info[0].bits_per_sample);

  U16ToAttribute(image,"DPX:image.orientation",header.image_info.orientation);

  if (pixels_offset >= 1664UL)
    {
      /*
        Read Image source information header.
      */
      offset += ReadBlob(image,sizeof(header.source_info),&header.source_info);
      if (offset != (size_t) 1664L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXImageSourceInfo(&header.source_info);
    }
  if (pixels_offset >= 1920UL)
    {
      /*
        Read Motion-picture film information header.
      */
      offset += ReadBlob(image,sizeof(header.mp_info),&header.mp_info);
      if (offset != (size_t) 1920L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXMPFilmInfo(&header.mp_info);
    }
  if (pixels_offset >= 2048UL)
    {
      /*
        Read Television information header.
      */
      offset += ReadBlob(image,sizeof(header.tv_info),&header.tv_info);
      if (offset != (size_t) 2048L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXTVInfo(&header.tv_info);
    }
  /*
    Skip reading pixels if ping requested.
  */
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
    Convert DPX raster image to pixel packets.
  */
  switch ((int) colortype)
  {
    case (MonoColorType):
    {
      q=SetImagePixels(image,0,0,image->columns,image->rows);
      for (x=0; x < (long) ((image->columns*image->rows)/3); x++)
      {
        pixel=ReadBlobMSBLong(image);
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 0) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 10) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
        q->red=(Quantum) ((double) MaxRGB*((pixel >> 20) & 0x3ff)/1023+0.5);
        q->green=q->red;
        q->blue=q->red;
        q++;
      }
      break;
    }
    case (RGBColorType):
    {
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          pixel=ReadBlobMSBLong(image);
          q->red=(Quantum) ((double) MaxRGB*((pixel >> 22) & 0x3ff)/1023+0.5);
          q->green=(Quantum) ((double) MaxRGB*((pixel >> 12) & 0x3ff)/1023+0.5);
          q->blue=(Quantum) ((double) MaxRGB*((pixel >> 2) & 0x3ff)/1023+0.5);
          q++;
        }
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            if (!MagickMonitor(LoadImageText,y,image->rows,exception))
              break;
      }
      break;
    }
    default:
      ThrowReaderException(CoderError,ColorTypeNotSupported,image)
  }
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
      image->filename);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D P X I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDPXImage adds attributes for the DPX image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDPXImage method is:
%
%      RegisterDPXImage(void)
%
*/
ModuleExport void RegisterDPXImage(void)
{
  MagickInfo
    *entry;

  static const char
    *DPXNote =
    {
      "File Format for Digital Moving-Picture Exchange (DPX) Version 2.0.\n"
      "The SMPTE 268M-2003 specification is available from http://www.smtpe.org\n"
    };

  entry=SetMagickInfo("DPX");
  entry->decoder=(DecoderHandler) ReadDPXImage;
  entry->encoder=(EncoderHandler) WriteDPXImage;
  entry->magick=(MagickHandler) IsDPX;
  entry->description=AcquireString("SMPTE 268M-2003 (DPX)");
  entry->note=AcquireString(DPXNote);
  entry->module=AcquireString("DPX");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D P X I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDPXImage removes format registrations made by the
%  DPX module from the list of supported formats.
%
%  The format of the UnregisterDPXImage method is:
%
%      UnregisterDPXImage(void)
%
*/
ModuleExport void UnregisterDPXImage(void)
{
  (void) UnregisterMagickInfo("DPX");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e D P X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteDPXImage writes an image in DPX encoded image format.
%
%  The format of the WriteDPXImage method is:
%
%      unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteDPXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteDPXImage(const ImageInfo *image_info,Image *image)
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

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  (void) TransformColorspace(image,RGBColorspace);
  (void) WriteBlobMSBLong(image,0x53445058);
  (void) WriteBlobMSBLong(image,0x2000);
  (void) WriteBlobMSBLong(image,0x56312E30);
  (void) WriteBlobMSBLong(image,0x00000000);
  (void) WriteBlobMSBLong(image,4*image->columns*image->rows+0x2000);
  (void) WriteBlobMSBLong(image,0x00000001);
  (void) WriteBlobMSBLong(image,0x00000680);
  (void) WriteBlobMSBLong(image,0x00000180);
  (void) WriteBlobMSBLong(image,0x00001800);
  for (i=0; i < 124; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobMSBLong(image,0x496D6167);
  (void) WriteBlobMSBLong(image,0x654D6167);
  (void) WriteBlobMSBLong(image,0x69636B20);
  for (i=0; i < 599; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x01);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBLong(image,image->rows);
  for (i=0; i < 20; i++)
    (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,RGBColorType);
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,10);  /* bit depth */
  (void) WriteBlobByte(image,0x00);
  (void) WriteBlobByte(image,0x01);
  for (i=0; i < (0x2000-806); i++)
    (void) WriteBlobByte(image,0x00);
  /*
    Convert pixel packets to DPX raster image.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (long) image->columns; x++)
    {
      pixel=(((long) ((1023L*p->red+MaxRGB/2)/MaxRGB) & 0x3ffU) << 22U) |
        (((long) ((1023L*p->green+MaxRGB/2)/MaxRGB) &0x3ffU) << 12U) |
        (((long) ((1023L*p->blue+MaxRGB/2)/MaxRGB) &0x3ffU) << 2U);
      (void) WriteBlobMSBLong(image,pixel);
      p++;
    }
  }
  CloseBlob(image);  
  return(status);
}
