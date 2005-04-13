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
#include "magick/bit_stream.h"
#include "magick/cache.h"
#include "magick/color.h"
#include "magick/endian.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteDPXImage(const ImageInfo *,Image *);

typedef char ASCII;
typedef magick_uint8_t U8;
typedef magick_uint16_t U16;
typedef magick_uint32_t U32;
typedef float R32;

#define UNDEFINED_U8_VALUE 0xffU
#define UNDEFINED_U16_VALUE 0xffffU
#define UNDEFINED_U32_VALUE 0xffffffffU
#define UNDEFINED_ASCII_VALUE '\0'

typedef enum
{
  ImageElementUnspecified=0,
  ImageElementRed=1,
  ImageElementGreen=2,
  ImageElementBlue=3,
  ImageElementAlpha=4,
  ImageElementLuma=6,
  ImageElementColorDifferenceCbCr=7,
  ImageElementDepth=8,
  ImageElementCompositeVideo=9,
  ImageElementRGB=50,              /* BGR order */
  ImageElementRGBA=51,             /* BGRA order */
  ImageElementABGR=52,             /* ARGB order */
  ImageElementCbYCrY422=100,       /* SMPTE 125M, 4:2:2 */
  ImageElementCbYACrYA4224=101,    /* SMPTE 125M, 4:2:2:4 */
  ImageElementCbYCr444=102,        /* SMPTE 125M, 4:4:4 */
  ImageElementCbYCrA4444=103       /* SMPTE 125M, 4:4:4:4 */
} DPXImageElementDescriptor;

typedef enum
{
  TransferCharacteristicUserDefined=0,
  TransferCharacteristicPrintingDensity=1,
  TransferCharacteristicLinear=2,
  TransferCharacteristicLogarithmic=3,
  TransferCharacteristicUnspecifiedVideo=4,
  TransferCharacteristicSMTPE274M=5,     /* 1920x1080 TV */
  TransferCharacteristicITU_R704_4=6,
  TransferCharacteristicITU_R601_5_BG=7, /* 625 Line */
  TransferCharacteristicITU_R601_5_M=8,  /* 525 Line */
  TransferCharacteristicNTSCCompositeVideo=9,
  TransferCharacteristicPALCompositeVideo=10,
  TransferCharacteristicZDepthLinear=11,
  TransferCharacteristicZDepthHomogeneous=12
} DPXTransferCharacteristic;

typedef enum
{
  PackingMethodPacked=0,           /* Packed with no padding */
  PackingMethodWordsFillLSB=1,     /* Method 'A', padding bits in LSB of 32-bit word */
  PackingMethodWordsFillMSB=2      /* Method 'B', padding bits in MSB of 32-bit word (deprecated) */
}  ImageComponentPackingMethod;

typedef struct _DPXFileInfo
{
  U32   magic;                     /* Magick number (SDPX ASCII) */
  U32   image_data_offset;         /* Offset to image data in bytes */
  ASCII header_format_version[8];  /* Version number of header format */
  U32   file_size;                 /* Total image file size in bytes  */
  U32   ditto_key;                 /* (0 = same as previous frame; 1 = new) */
  U32   generic_section_length;    /* Generic section header length in bytes */
  U32   industry_section_length;   /* Industry specific header length in bytes */
  U32   user_defined_length;       /* User defined header length in bytes */
  ASCII image_filename[100];       /* Image filename */
  ASCII creation_datetime[24];     /* Creation date/time: yyyy:mm:dd:hh:mm:ssLTZ */
  ASCII creator[100];              /* Creator */
  ASCII project_name[200];         /* Project name */
  ASCII copyright[200];            /* Right to use or copyright */
  U32   encryption_key;            /* Encryption key (FFFFFFFF unencrypted ) */
  ASCII reserved[104];             /* Reserved for future use */
} DPXFileInfo;

typedef struct _DPXImageElement
{
  U32   data_sign;                 /* Data sign (0 = unsigned; 1 = signed) */
  U32   reference_low_data_code;   /* Reference low data code value */
  R32   reference_low_quantity;    /* Low quantity represented */
  U32   reference_high_data_code;  /* Reference high data code value */
  R32   reference_high_quantity;   /* Reference high quantity represented */
  U8    descriptor;                /* Descriptor */
  U8    transfer_characteristic;   /* Transfer characteristic */
  U8    colorimetric;              /* Colorimetric specification */
  U8    bits_per_sample;           /* Bit depth */
  U16   packing;                   /* Packing */
  U16   encoding;                  /* Encoding */
  U32   data_offset;               /* Offset to data */
  U32   eol_pad;                   /* End of line padding */
  U32   eoi_pad;                   /* End of image padding */
  ASCII description[32];           /* Description of image element */
} DPXImageElement;

typedef struct _DPXImageInfo
{
  U16   orientation;               /* Image orientation */
  U16   elements;                  /* Number of image elements (1-8) */
  U32   pixels_per_line;           /* Pixels per line (columns) */
  U32   lines_per_image_element;   /* Lines per image element (rows) */
  DPXImageElement element_info[8]; /* Description of elements */
  ASCII reserved[52];              /* Reserved for future use */
} DPXImageInfo;

typedef struct _DPXImageSourceBorderValidity
{
  /*
    Border validity indicates portion of border pixels which have been
    eroded due to processing.
  */
  U16   XL;                        /* Border validity XL border */
  U16   XR;                        /* Border validity XR border */
  U16   YT;                        /* Border validity YT border */
  U16   YB;                        /* Border validity YB border */
} DPXImageSourceBorderValidity;

typedef struct _DPXImageSourcePixelAspectRatio
{
  U32   horizontal;                /* Horizontal */
  U32   vertical;                  /* Vertical */
} DPXImageSourcePixelAspectRatio;

typedef struct _DPXImageSourceInfo
{
  U32   x_offset;                  /* X offset */
  U32   y_offset;                  /* Y offset */
  R32   x_center;                  /* X center */
  R32   y_center;                  /* Y center */
  U32   x_original_size;           /* X original size */
  U32   y_original_size;           /* Y original size */
  ASCII source_image_filename[100];/* Source image filename */
  ASCII source_image_datetime[24]; /* Source image date/time: yyyy:mmm:dd:hh:mm:ssLTZ */
  ASCII input_device_name[32];     /* Input device name */
  ASCII input_device_serialnumber[32]; /* Input device serial number */
  DPXImageSourceBorderValidity border_validity; /* Border validity */
  DPXImageSourcePixelAspectRatio aspect_ratio; /* Aspect ratio */
  R32   x_scanned_size;            /* X scanned size */
  R32   y_scanned_size;            /* Y scanned size */
  ASCII reserved[20];              /* Reserved for future use */
} DPXImageSourceInfo;

typedef struct _DPXMPFilmInfo
{
  ASCII film_mfg_id_code[2];       /* Film mfg. ID code (2 digits from film edge code) */
  ASCII film_type[2];              /* Film type (2 digits from film edge code) */
  ASCII perfs_offset[2];           /* Offset in perfs (2 digits from film edge code) */
  ASCII prefix[6];                 /* Prefix (6 digits from film edge code) */
  ASCII count[4];                  /* Count (4 digits from film edge code) */
  ASCII format[32];                /* Format -- e.g. Academy */
  U32   frame_position;            /* Frame position in sequence */
  U32   sequence_length;           /* Sequence length (frames) */
  U32   held_count;                /* Held count (1 = default) */
  R32   frame_rate;                /* Frame rate of original (frames/s) */
  R32   shutter_angle;             /* Shutter angle of camera in degrees */
  ASCII frame_id[32];              /* Frame identification - e.g. keyframe */
  ASCII slate_info[100];           /* Slate information */
  ASCII reserved[56];              /* Reserved for future use */
} DPXMPFilmInfo;

typedef struct _DPXTVInfo
{
  U32   time_code;                 /* SMPTE time code */
  U32   user_bits;                 /* SMPTE user bits */
  U8    interlace;                 /* Interlace (0 = noninterlaced; 1 = 2:1 interlace */
  U8    field_number;              /* Field number */
  U8    video_signal;              /* Video signal standard */
  U8    zero;                      /* Zero (for byte alignment) */
  R32   horizontal_sample;         /* Horizontal sampling rate */
  R32   vertical_sample;           /* Vertical sampling rate */
  R32   temporal_sample;           /* Temporal sampling rate or frame rate (Hz) */
  R32   sync_time;                 /* Time offset from sync to first pixel (ms) */
  R32   gamma;                     /* Gamma */
  R32   black_level;               /* Black level code value */
  R32   black_gain;                /* Black gain */
  R32   breakpoint;                /* Breakpoint */
  R32   white_level;               /* Reference white level code value */
  R32   integration_time;          /* Integration time (s) */
  ASCII reserved[76];              /* Reserved for future use */
} DPXTVInfo;

typedef struct _DPXUserDefinedData
{
  ASCII user_id[32];               /* User identification */
  /* Up to 1MB of user-defined data after this point */
} DPXUserDefinedData;

typedef struct _DPXHeader
{
  DPXFileInfo file_info;           /* File information header */
  DPXImageInfo image_info;         /* Image information header */
  DPXImageSourceInfo source_info;  /* Image source information header */
  DPXMPFilmInfo mp_info;           /* Motion picture film information header */
  DPXTVInfo tv_info;               /* Television information header */
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
  MagickSwabUInt32(&file_info->magic);
  MagickSwabUInt32(&file_info->image_data_offset);
  MagickSwabUInt32(&file_info->file_size);
  MagickSwabUInt32(&file_info->ditto_key);
  MagickSwabUInt32(&file_info->generic_section_length);
  MagickSwabUInt32(&file_info->industry_section_length);
  MagickSwabUInt32(&file_info->user_defined_length);
  MagickSwabUInt32(&file_info->encryption_key);
}
static void SwabDPXImageInfo(DPXImageInfo *image_info)
{
  int
    i;

  MagickSwabUInt16(&image_info->orientation);
  MagickSwabUInt16(&image_info->elements);
  MagickSwabUInt32(&image_info->pixels_per_line);
  MagickSwabUInt32(&image_info->lines_per_image_element);
  for (i=0 ; i < 8 ; i++)
    {
      MagickSwabUInt32(&image_info->element_info[i].data_sign);
      MagickSwabUInt32(&image_info->element_info[i].reference_low_data_code);
      MagickSwabFloat(&image_info->element_info[i].reference_low_quantity);
      MagickSwabUInt32(&image_info->element_info[i].reference_high_data_code);
      MagickSwabFloat(&image_info->element_info[i].reference_high_quantity);
      MagickSwabUInt16(&image_info->element_info[i].packing);
      MagickSwabUInt16(&image_info->element_info[i].encoding);
      MagickSwabUInt32(&image_info->element_info[i].data_offset);
      MagickSwabUInt32(&image_info->element_info[i].eol_pad);
      MagickSwabUInt32(&image_info->element_info[i].eoi_pad);
    }
}
static void SwabDPXImageSourceInfo(DPXImageSourceInfo *source_info)
{
  MagickSwabUInt32(&source_info->x_offset);
  MagickSwabUInt32(&source_info->y_offset);
  MagickSwabFloat(&source_info->x_center);
  MagickSwabFloat(&source_info->y_center);
  MagickSwabUInt32(&source_info->x_original_size);
  MagickSwabUInt32(&source_info->y_original_size);
  MagickSwabUInt16(&source_info->border_validity.XL);
  MagickSwabUInt16(&source_info->border_validity.XR);
  MagickSwabUInt16(&source_info->border_validity.YT);
  MagickSwabUInt16(&source_info->border_validity.YB);
  MagickSwabUInt32(&source_info->aspect_ratio.horizontal);
  MagickSwabUInt32(&source_info->aspect_ratio.vertical);
  MagickSwabFloat(&source_info->x_scanned_size);
  MagickSwabFloat(&source_info->y_scanned_size);
}
static void SwabDPXMPFilmInfo(DPXMPFilmInfo *mp_info)
{
  MagickSwabUInt32(&mp_info->frame_position);
  MagickSwabUInt32(&mp_info->sequence_length);
  MagickSwabUInt32(&mp_info->held_count);
  MagickSwabFloat(&mp_info->frame_rate);
  MagickSwabFloat(&mp_info->shutter_angle);
}
static void SwabDPXTVInfo(DPXTVInfo *tv_info)
{
  MagickSwabUInt32(&tv_info->time_code);
  MagickSwabUInt32(&tv_info->user_bits);
  MagickSwabFloat(&tv_info->horizontal_sample);
  MagickSwabFloat(&tv_info->vertical_sample);
  MagickSwabFloat(&tv_info->temporal_sample);
  MagickSwabFloat(&tv_info->sync_time);
  MagickSwabFloat(&tv_info->gamma);
  MagickSwabFloat(&tv_info->black_level);
  MagickSwabFloat(&tv_info->black_gain);
  MagickSwabFloat(&tv_info->breakpoint);
  MagickSwabFloat(&tv_info->white_level);
  MagickSwabFloat(&tv_info->integration_time);
}
static const char *DescribeImageElementDescriptor(const DPXImageElementDescriptor descriptor)
{
  char *
    description="Unknown";

  switch(descriptor)
    {
    case ImageElementUnspecified:
      description="Unspecified(0)";
      break;
    case ImageElementRed:
      description="Red(1)";
      break;
    case ImageElementGreen:
      description="Green(2)";
      break;
    case ImageElementBlue:
      description="Blue(3)";
      break;
    case ImageElementAlpha:
      description="Alpha(4)";
      break;
    case ImageElementLuma:
      description="Luma(6)";
      break;
    case ImageElementColorDifferenceCbCr:
      description="ColorDifferenceCbCr(7)";
      break;
    case ImageElementDepth:
      description="Depth(8)";
      break;
    case ImageElementCompositeVideo:
      description="CompositeVideo(9)";
      break;
    case ImageElementRGB:
      description="RGB(50)";
      break;
    case ImageElementRGBA:
      description="RGBA(51)";
      break;
    case ImageElementABGR:
      description="ABGR(52)";
      break;
    case ImageElementCbYCrY422:
      description="CbYACrYA 4:2:2:4 (100)";
      break;
    case ImageElementCbYACrYA4224:
      description="CbYACrYA 4:2:2:4 (101)";
      break;
    case ImageElementCbYCr444:
      description="CbYCr 4:4:4 (102)";
      break;
    case ImageElementCbYCrA4444:
      description="CbYCrA 4:4:4:4 (103)";
      break;
    default:
      {
      }
    }

  return description;
}
static void DescribeDPXImageElement(const DPXImageElement *element_info,
                                    const unsigned int element)
{
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: data_sign=%s",element, 
                        element_info->data_sign == 0 ?
                        "unsigned" : "signed");
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: reference_low_data_code=%u reference_low_quantity=%g",
                        element,
                        element_info->reference_low_data_code,
                        element_info->reference_low_quantity);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: reference_high_data_code=%u reference_high_quantity=%g",
                        element,
                        element_info->reference_low_data_code,
                        element_info->reference_low_quantity);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: descriptor=%s characteristic=%u colorimetric=%u",
                        element,
                        DescribeImageElementDescriptor(element_info->descriptor),
                        (unsigned int) element_info->transfer_characteristic,
                        (unsigned int) element_info->colorimetric);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: bits-per-sample=%u",
                        element,
                        (unsigned int) element_info->bits_per_sample);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: packing=%u encoding=%u data_offset=%u eol_pad=%u eoi_pad=%u",
                        element,
                        (unsigned int) element_info->packing,
                        (unsigned int) element_info->encoding,
                        (unsigned int) element_info->data_offset,
                        (unsigned int) element_info->eol_pad,
                        (unsigned int) element_info->eoi_pad);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element %u: description=\"%.32s\"",
                        element,
                        element_info->description);
}
static Image *ReadDPXImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  DPXFileInfo
    dpx_file_info;

  DPXImageInfo
    dpx_image_info;

  DPXImageSourceInfo
    dpx_source_info;

  DPXMPFilmInfo
    dpx_mp_info;

  DPXTVInfo
    dpx_tv_info;

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

  unsigned int
    bits_per_sample,
    element,
    scale_to_short,
    samples[8],
    samples_per_element,
    status;

  unsigned long
    i,
    pixels_offset;

  MagickBool
    is_grayscale=MagickFalse,
    is_monochrome=MagickFalse,
    word_pad_lsb=MagickFalse,
    word_pad_msb=MagickFalse,
    swab=MagickFalse;

  WordStreamReadHandle
    word_stream;

  DPXImageElementDescriptor
    element_descriptor;

  ImageComponentPackingMethod
    packing_method;

  WordStreamReadFunc
    word_read_func;

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
  offset=ReadBlob(image,sizeof(dpx_file_info),&dpx_file_info);
  if (offset != sizeof(dpx_file_info) ||
      ((LocaleNCompare((char *) &dpx_file_info.magic,"SDPX",4) != 0) &&
       (LocaleNCompare((char *) &dpx_file_info.magic,"XPDS",4) != 0)))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Check for swapped endian order.
  */
  if (dpx_file_info.magic != 0x53445058U)
    swab=MagickTrue;

#if defined(WORDS_BIGENDIAN)
  word_read_func = (swab ? (WordStreamReadFunc) ReadBlobLSBLong :
                    (WordStreamReadFunc) ReadBlobMSBLong);
#else
  word_read_func = (swab ? (WordStreamReadFunc) ReadBlobMSBLong :
                    (WordStreamReadFunc) ReadBlobLSBLong);
#endif
  
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "%s endian DPX format",
#if defined(WORDS_BIGENDIAN)
                        (swab ? "Little" : "Big")
#else
                        (swab ? "Big" : "Little")
#endif
                        );
  if (swab)
    SwabDPXFileInfo(&dpx_file_info);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "File size: %u", dpx_file_info.file_size);

  StringToAttribute(image,"artist",dpx_file_info.creator);
  StringToAttribute(image,"comment",dpx_file_info.project_name);
  StringToAttribute(image,"copyright",dpx_file_info.copyright);
  StringToAttribute(image,"document",dpx_file_info.image_filename);
  StringToAttribute(image,"timestamp",dpx_file_info.creation_datetime);

  StringToAttribute(image,"DPX:file.filename",dpx_file_info.image_filename);
  StringToAttribute(image,"DPX:file.creation.datetime",dpx_file_info.creation_datetime);
  StringToAttribute(image,"DPX:file.creator",dpx_file_info.creator);
  StringToAttribute(image,"DPX:file.project.name",dpx_file_info.project_name);
  StringToAttribute(image,"DPX:file.copyright",dpx_file_info.copyright);
  U32ToAttribute(image,"DPX:file.encryption.key",dpx_file_info.encryption_key);

  /*
    Obtain offset to pixels.
  */
  pixels_offset=dpx_file_info.image_data_offset;
  if (pixels_offset < 1408)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Read image information header.
  */
  offset += ReadBlob(image,sizeof(dpx_image_info),&dpx_image_info);
  if (offset != (size_t) 1408L)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (swab)
    SwabDPXImageInfo(&dpx_image_info);
  image->columns=dpx_image_info.pixels_per_line;
  image->rows=dpx_image_info.lines_per_image_element;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Columns %ld, Rows %ld, Elements %u",
                        image->columns, image->rows,
                        (unsigned int) dpx_image_info.elements);

  U16ToAttribute(image,"DPX:image.orientation",dpx_image_info.orientation);

  if (pixels_offset >= 1664UL)
    {
      /*
        Read Image source information header.
      */
      offset += ReadBlob(image,sizeof(dpx_source_info),&dpx_source_info);
      if (offset != (size_t) 1664L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXImageSourceInfo(&dpx_source_info);
    }
  if (pixels_offset >= 1920UL)
    {
      /*
        Read Motion-picture film information header.
      */
      offset += ReadBlob(image,sizeof(dpx_mp_info),&dpx_mp_info);
      if (offset != (size_t) 1920L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXMPFilmInfo(&dpx_mp_info);
    }
  if (pixels_offset >= 2048UL)
    {
      /*
        Read Television information header.
      */
      offset += ReadBlob(image,sizeof(dpx_tv_info),&dpx_tv_info);
      if (offset != (size_t) 2048L)
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      if (swab)
        SwabDPXTVInfo(&dpx_tv_info);
    }
  /*
    Obtain image depth.
  */
  image->depth=dpx_image_info.element_info[0].bits_per_sample;
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
  for (element=0; element < dpx_image_info.elements; element++)
    {
      DescribeDPXImageElement(&dpx_image_info.element_info[element],element+1);

      /*
        Data sign, (0 = unsigned; 1 = signed)
      */
      if (dpx_image_info.element_info[element].data_sign != 0)
        continue;
      /*
        Bits per sample.
      */
      if (dpx_image_info.element_info[element].bits_per_sample > 16)
        continue;

      /*
        Move to element data
      */

      if ((dpx_image_info.element_info[element].data_offset != UNDEFINED_U32_VALUE) &&
          (dpx_image_info.element_info[element].data_offset != 0U))
        {
          pixels_offset=dpx_image_info.element_info[element].data_offset;
          offset=SeekBlob(image,(magick_off_t) pixels_offset,SEEK_SET);

#if 0
          if (pixels_offset > offset)
            {
              /* Data is ahead of current position.  Good! */
              for ( ; offset < pixels_offset ; offset++ )
                (void) ReadBlobByte(image);
            }
          else
            {
              /* Data is behind current position.  Bad! */
              offset=SeekBlob(image,(magick_off_t) pixels_offset,SEEK_SET);
            }
#endif
        }

      bits_per_sample=dpx_image_info.element_info[element].bits_per_sample;
      image->depth=Max(image->depth,bits_per_sample);
      element_descriptor=(DPXImageElementDescriptor)
        dpx_image_info.element_info[element].descriptor;
      packing_method=dpx_image_info.element_info[element].packing;
      if (dpx_image_info.element_info[element].transfer_characteristic ==
          TransferCharacteristicLogarithmic)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Setting colorspace to CineonLogRGBColorspace");
          image->colorspace=CineonLogRGBColorspace;
        }

      /* FIXME: hack around Cinepaint oddity which mis-marks files. */
      if ((element_descriptor == ImageElementUnspecified) &&
          (dpx_image_info.elements == 1))
        packing_method=PackingMethodWordsFillMSB;

      /*
        Determine component packing method.
      */
      if ((bits_per_sample == 10) || (bits_per_sample == 12))
        {
          if (packing_method == PackingMethodWordsFillLSB)
            word_pad_lsb=MagickTrue;
          else if (packing_method == PackingMethodWordsFillMSB)
            word_pad_msb=MagickTrue;
        }
      /*
        Decide if the image is grayscale.
      */
      if (((dpx_image_info.elements < 3) &&
           (element_descriptor == ImageElementLuma)) ||
          ((dpx_image_info.elements == 1) &&
           (element_descriptor == ImageElementUnspecified)))
        is_grayscale=MagickTrue;
      if ((is_grayscale) && (bits_per_sample == 1))
        is_monochrome=MagickTrue;

      switch (element_descriptor)
        {
        case ImageElementUnspecified:
        case ImageElementRed:
        case ImageElementGreen:
        case ImageElementBlue:
        case ImageElementAlpha:
        case ImageElementLuma:
        case ImageElementRGB:
        case ImageElementRGBA:
        case ImageElementABGR:
          {
            scale_to_short=1U;
            if (bits_per_sample < 16U)
              scale_to_short=(65535U / (65535U >> (16-bits_per_sample)));

            /*
              Determine number of samples per element.
            */
            switch (element_descriptor)
              {
              case ImageElementUnspecified:
              case ImageElementRed:
              case ImageElementGreen:
              case ImageElementBlue:
              case ImageElementAlpha:
              case ImageElementLuma:
                samples_per_element=1;
                break;
              case ImageElementRGB:
                samples_per_element=3;
                break;
              case ImageElementRGBA:
              case ImageElementABGR:
                samples_per_element=4;
                break;
              default:
                samples_per_element=0;
                break;
              }

            /*
              Determine if matte channel is supported.
            */
            switch (element_descriptor)
              {
              case ImageElementAlpha:
              case ImageElementRGBA:
              case ImageElementABGR:
                image->matte=MagickTrue;
                break;
              default:
                break;
              }

            WordStreamInitializeRead(&word_stream,word_read_func,image);
            for (y=0; y < (long) image->rows; y++)
              {
                if (element == 0)
                  q=SetImagePixels(image,0,y,image->columns,1);
                else
                  q=GetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;

                for (x=0; x < (long) image->columns; x++)
                  {
                    if (word_pad_lsb)
                      {
                        /*
                          Padding in LSB (Method A).
                        */
                        if (bits_per_sample == 10)
                          {
                            for (i=0; i < samples_per_element; i++)
                              {
                                if (word_stream.bits_remaining == 0)
                                  (void) WordStreamLSBRead(&word_stream,2);
                                samples[i]=WordStreamLSBRead(&word_stream,
                                                             bits_per_sample);
                              }
                          }
                        else if (bits_per_sample == 12)
                          {
                            for (i=0; i < samples_per_element; i++)
                              {
                                (void) WordStreamLSBRead(&word_stream,4);
                                samples[i]=WordStreamLSBRead(&word_stream,
                                                             bits_per_sample);
                              }
                          }
                      }
                    else if (word_pad_msb)
                      {
                        /*
                          Padding in MSB (Method B).
                        */
                        if (bits_per_sample == 10)
                          {
                            for (i=0; i < samples_per_element; i++)
                              {
                                if (word_stream.bits_remaining == 2)
                                  (void) WordStreamLSBRead(&word_stream,2);
                                samples[i]=WordStreamLSBRead(&word_stream,
                                                             bits_per_sample);
                              }
                          }
                        else if (bits_per_sample == 12)
                          {
                            for (i=0; i < samples_per_element; i++)
                              {
                                samples[i]=WordStreamLSBRead(&word_stream,
                                                             bits_per_sample);
                                (void) WordStreamLSBRead(&word_stream,4);
                              }
                          }
                      }
                    else
                      {
                        /*
                          Packed data. Packing is broken on scan-line boundaries.
                        */
                        for (i=0; i < samples_per_element; i++)
                          samples[i]=WordStreamLSBRead(&word_stream,bits_per_sample);
                      }
                    /*
                      Scale samples.
                    */
                    for (i=0; i < samples_per_element; i++)
                      samples[i]=ScaleShortToQuantum(samples[i]*scale_to_short);
                    /*
                      Assign samples.
                    */
                    switch (element_descriptor)
                      {
                      case ImageElementRed:
                        q->red=samples[0];
                        break;
                      case ImageElementGreen:
                        q->green=samples[0];
                        break;
                      case ImageElementBlue:
                        q->blue=samples[0];
                        break;
                      case ImageElementAlpha:
                        q->opacity=MaxRGB-samples[0];
                        break;
                      case ImageElementUnspecified:
                      case ImageElementLuma:
                        q->red=q->green=q->blue=samples[0];
                        break;
                      case ImageElementRGB:
                        q->blue=samples[0];
                        q->green=samples[1];
                        q->red=samples[2];
                        q->opacity=OpaqueOpacity;
                        break;
                      case ImageElementRGBA:
                        q->blue=samples[0];
                        q->green=samples[1];
                        q->red=samples[2];
                        q->opacity=MaxRGB-samples[3];
                        break;
                      case ImageElementABGR:
                        q->opacity=MaxRGB-samples[0];
                        q->red=samples[1];
                        q->green=samples[2];
                        q->blue=samples[3];
                        break;
                      default:
                        break;
                      }
                    q++;
                  }
                if (!SyncImagePixels(image))
                  break;

                /*
                  Advance to next 32-bit word if components are packed.
                */
                if (packing_method == PackingMethodPacked)
                  WordStreamInitializeRead(&word_stream,word_read_func,image);

                /*
                  FIXME: Add support for optional EOL padding.
                */

                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitor(LoadImageText,y,image->rows,exception))
                      break;

                if (EOFBlob(image))
                  {
                    printf("### File length %u, TellBlob says %u\n",
                           dpx_file_info.file_size,
                           (unsigned int) TellBlob(image));
                    break;
                  }
              }
            break;
          }
        default:
          ThrowReaderException(CoderError,ColorTypeNotSupported,image);
        }
    }
  if (EOFBlob(image))
    ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
                   image->filename);
  image->is_monochrome=is_monochrome;
  image->is_grayscale=is_grayscale;
  image->depth=Min(image->depth,QuantumDepth);
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
  DPXFileInfo
    dpx_file_info;

  DPXImageInfo
    dpx_image_info;

  DPXImageSourceInfo
    dpx_source_info;

  DPXImageElementDescriptor
    element_descriptor;

  ImageComponentPackingMethod
    packing_method;

  unsigned long
    y;

  register const PixelPacket
    *p;
  
  register unsigned long
    i,
    x;

  unsigned int
    bits_per_sample=0,
    element,
    number_of_elements,
    samples[8],
    samples_per_component,
    samples_per_element,
    scale_from_short,
    status;
 
  MagickBool
    word_pad_lsb=MagickFalse,
    word_pad_msb=MagickFalse,
    swab=MagickFalse;

  WordStreamWriteHandle
    word_stream;

  WordStreamWriteFunc
    word_write_func;

  magick_int64_t
    element_size,
    row_samples;

  const char *
    definition_value;

  size_t
    offset=0;

  MagickBool
    is_grayscale;

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

#if defined(WORDS_BIGENDIAN)
  word_write_func=(WordStreamWriteFunc) WriteBlobMSBLong;
#else
  word_write_func=(WordStreamWriteFunc) WriteBlobLSBLong;
#endif

  if ((definition_value=AccessDefinition(image_info,"dpx","bits-per-sample")))
    bits_per_sample=atoi(definition_value);

  if (bits_per_sample == 0)
    {
      if (image->depth > 12 )
        bits_per_sample=16;
      else if (image->depth > 10)
        bits_per_sample=12;
      else if (image->depth > 8)
        bits_per_sample=10;
      else if (image->depth > 1)
        bits_per_sample=8;
      else
        bits_per_sample=1;
    }

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Bits per sample: %u", bits_per_sample);

  is_grayscale=image->is_grayscale;

  if (is_grayscale)
    {
      samples_per_component=1;
      number_of_elements=1;
      if (image->matte)
        number_of_elements++;
    }
  else
    {
      if (image_info->interlace == PlaneInterlace)
        {
          samples_per_component=1;
          number_of_elements=3;
          if (image->matte)
            number_of_elements++;
        }
      else
        {
          samples_per_component=3;
          number_of_elements=1;
          if (image->matte)
            samples_per_component++;
        }
    }

  packing_method=PackingMethodPacked;

  if ((definition_value=AccessDefinition(image_info,"dpx","packing-method")))
    {
      if (LocaleCompare(definition_value,"packed") == 0)
        {
          packing_method=PackingMethodPacked;
        }
      else if ((bits_per_sample == 10) || (bits_per_sample == 12))
        {
          if ((LocaleCompare(definition_value,"lsbpad") == 0) ||
              (LocaleCompare(definition_value,"a") == 0))
            packing_method=PackingMethodWordsFillLSB;
          else if ((LocaleCompare(definition_value,"msbpad") == 0) ||
                   (LocaleCompare(definition_value,"b") == 0))
            packing_method=PackingMethodWordsFillMSB;
        }
    }

  row_samples=((magick_int64_t) image->columns*samples_per_component);
  
  /* samples packed end-to-end, but padded to next word at end of each row */
  element_size=((row_samples*bits_per_sample+31)/32)*sizeof(magick_int32_t)*image->rows;
  if ((packing_method == PackingMethodWordsFillLSB) ||
      (packing_method == PackingMethodWordsFillMSB))
    {
      if (bits_per_sample == 10)
        {
          /* three 10-bit samples per 32-bit word, no extra padding at end of row */
          element_size=((row_samples*image->rows+2)/3)*sizeof(magick_int32_t);
        }
      else if (bits_per_sample == 12)
        {
          /* samples packed end-to-end, but padded to next word at end of each row */
          element_size=((row_samples*image->rows+1)/2)*sizeof(magick_int32_t);
        }
    }

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Element size: %u", (unsigned int) element_size);

  /*
    Adjust image colorspace if necessary.
  */
  if (image_info->colorspace == CineonLogRGBColorspace)
    (void) TransformColorspace(image,CineonLogRGBColorspace);
  else
    (void) TransformColorspace(image,RGBColorspace);

  /*
    Image information header
  */
  memset(&dpx_image_info,0,sizeof(dpx_image_info));
  dpx_image_info.orientation=0; /* Left to right, top to bottom */
  dpx_image_info.elements=number_of_elements;
  dpx_image_info.pixels_per_line=image->columns;
  dpx_image_info.lines_per_image_element=image->rows;

  dpx_image_info.element_info[0].data_sign=0; /* unsigned */
  dpx_image_info.element_info[0].reference_low_data_code=0;
  dpx_image_info.element_info[0].reference_low_quantity=0.00; /* FIXME */
  dpx_image_info.element_info[0].reference_high_data_code=
    MaxValueGivenBits(bits_per_sample);
  dpx_image_info.element_info[0].reference_high_quantity=2.048; /* FIXME */
  dpx_image_info.element_info[0].descriptor=image->matte ? ImageElementRGBA : ImageElementRGB;
  
  dpx_image_info.element_info[0].transfer_characteristic=
    (image->colorspace == CineonLogRGBColorspace ?
     TransferCharacteristicLogarithmic :
     TransferCharacteristicLinear);
  dpx_image_info.element_info[0].colorimetric=6; /* ITU 709-4 */
  dpx_image_info.element_info[0].bits_per_sample=bits_per_sample;
  dpx_image_info.element_info[0].packing=0;
  if ((bits_per_sample == 10) || (bits_per_sample == 12))
    dpx_image_info.element_info[0].packing=packing_method;
  dpx_image_info.element_info[0].encoding=0; /* No encoding */
  dpx_image_info.element_info[0].data_offset=0x2000; /* FIXME */
  dpx_image_info.element_info[0].eol_pad=0;
  dpx_image_info.element_info[0].eoi_pad=0;

  for (i=1; i < number_of_elements; i++)
    {
      /* Clone settings from preceding element */
      dpx_image_info.element_info[i]=dpx_image_info.element_info[i-1];
      /* Compute offset to data */
      dpx_image_info.element_info[i].data_offset=
        dpx_image_info.element_info[i-1].data_offset+element_size;
    }

  if (is_grayscale)
    {
      /*
        Grayscale with alpha plane.
      */
      dpx_image_info.element_info[0].descriptor=ImageElementLuma;
      if (number_of_elements > 1)
        dpx_image_info.element_info[1].descriptor=ImageElementAlpha;
    }
  else
    {
      if (number_of_elements > 1)
        {
          /*
            Planar image configuration.
          */
          dpx_image_info.element_info[0].descriptor=ImageElementRed;
          dpx_image_info.element_info[1].descriptor=ImageElementGreen;
          dpx_image_info.element_info[2].descriptor=ImageElementBlue;
          if ((image->matte) && (number_of_elements == 4))
            dpx_image_info.element_info[3].descriptor=ImageElementAlpha;
        }
    }

  /*
    File information header.
  */
  memset(&dpx_file_info,0,sizeof(dpx_file_info));
  dpx_file_info.magic=0x53445058U;
  dpx_file_info.image_data_offset=dpx_image_info.element_info[0].data_offset;
  strlcpy(dpx_file_info.header_format_version,"V2.0",
          sizeof(dpx_file_info.header_format_version));
  dpx_file_info.file_size=
    dpx_file_info.image_data_offset+number_of_elements*element_size;
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Estimated file length: %u",dpx_file_info.file_size);
  dpx_file_info.ditto_key=1; /* new frame */
  dpx_file_info.generic_section_length=sizeof(dpx_file_info)+
    sizeof(dpx_image_info)+sizeof(dpx_source_info);
  dpx_file_info.industry_section_length=0;
  dpx_file_info.user_defined_length=0;
  strlcpy(dpx_file_info.image_filename,image->filename,
          sizeof(dpx_file_info.image_filename));
  strlcpy(dpx_file_info.creation_datetime,"",
          sizeof(dpx_file_info.creation_datetime));
  strlcpy(dpx_file_info.creator,"",sizeof(dpx_file_info.creator));
  strlcpy(dpx_file_info.project_name,"",sizeof(dpx_file_info.project_name));
  strlcpy(dpx_file_info.copyright,"",sizeof(dpx_file_info.copyright));
  dpx_file_info.encryption_key=UNDEFINED_U32_VALUE;

  /*
    Image source information header
  */
  memset(&dpx_source_info,0,sizeof(dpx_source_info));
  dpx_source_info.x_offset=0; /* FIXME, use geometry for offset? */
  dpx_source_info.y_offset=0;
  dpx_source_info.x_center=image->columns/2.0;
  dpx_source_info.y_center=image->rows/2.0;
  dpx_source_info.x_original_size=image->magick_columns;
  dpx_source_info.y_original_size=image->magick_rows;
  strlcpy(dpx_source_info.source_image_filename,image->magick_filename,
          sizeof(dpx_source_info.source_image_filename));
  strlcpy(dpx_source_info.source_image_datetime,"",
          sizeof(dpx_source_info.source_image_datetime));
  strlcpy(dpx_source_info.input_device_name,"",
          sizeof(dpx_source_info.input_device_name));
  strlcpy(dpx_source_info.input_device_serialnumber,"",
          sizeof(dpx_source_info.input_device_serialnumber));
  dpx_source_info.border_validity.XL=0;
  dpx_source_info.border_validity.XR=0;
  dpx_source_info.border_validity.YT=0;
  dpx_source_info.border_validity.YB=0;
  dpx_source_info.aspect_ratio.horizontal=1;
  dpx_source_info.aspect_ratio.vertical=1;
  dpx_source_info.x_scanned_size=UNDEFINED_U32_VALUE;
  dpx_source_info.y_scanned_size=UNDEFINED_U32_VALUE;

  /*
    Write file headers.
  */
  offset += WriteBlob(image,sizeof(dpx_file_info),&dpx_file_info);
  offset += WriteBlob(image,sizeof(dpx_image_info),&dpx_image_info);
  offset += WriteBlob(image,sizeof(dpx_source_info),&dpx_source_info);
  for( ; offset < dpx_image_info.element_info[0].data_offset; offset++)
    (void) WriteBlobByte(image,0x00);

  for (element=0; element < dpx_image_info.elements; element++)
    {
      if ((magick_off_t) dpx_image_info.element_info[element].data_offset !=
          TellBlob(image))
        {
          printf("### Descriptor %u offset %u, TellBlob says %u\n",
                 element+1, dpx_image_info.element_info[element].data_offset,
                 (unsigned int) TellBlob(image));
        }
      DescribeDPXImageElement(&dpx_image_info.element_info[element],element+1);

      bits_per_sample=dpx_image_info.element_info[element].bits_per_sample;

      scale_from_short=1U;
      if (bits_per_sample < 16U)
        scale_from_short=(65535U / (65535U >> (16-bits_per_sample)));

      element_descriptor=(DPXImageElementDescriptor)
        dpx_image_info.element_info[element].descriptor;

      /*
        Determine component packing method.
      */
      packing_method=dpx_image_info.element_info[element].packing;
      if ((bits_per_sample == 10) || (bits_per_sample == 12))
        {
          if (packing_method == PackingMethodWordsFillLSB)
            word_pad_lsb=MagickTrue;
          else if (packing_method == PackingMethodWordsFillMSB)
            word_pad_msb=MagickTrue;
        }

      switch (element_descriptor)
        {
        case ImageElementUnspecified:
        case ImageElementRed:
        case ImageElementGreen:
        case ImageElementBlue:
        case ImageElementAlpha:
        case ImageElementLuma:
          samples_per_element=1;
          break;
        case ImageElementRGB:
          samples_per_element=3;
          break;
        case ImageElementRGBA:
        case ImageElementABGR:
          samples_per_element=4;
          break;
        default:
          samples_per_element=0;
          break;
        }

      WordStreamInitializeWrite(&word_stream,word_write_func,image);
      for (y=0; y < image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;

          for (x=0; x < image->columns; x++)
            {
              /*
                Prepare samples.
              */
              switch (element_descriptor)
                {
                case ImageElementRed:
                  samples[0]=p->red;
                  break;
                case ImageElementGreen:
                  samples[0]=p->green;
                  break;
                case ImageElementBlue:
                  samples[0]=p->blue;
                  break;
                case ImageElementAlpha:
                  samples[0]=MaxRGB-p->opacity;
                  break;
                case ImageElementUnspecified:
                case ImageElementLuma:
                  samples[0]=PixelIntensity(p);
                  break;
                case ImageElementRGB:
                  samples[0]=p->blue;
                  samples[1]=p->green;
                  samples[2]=p->red;
                  break;
                case ImageElementRGBA:
                  samples[0]=p->blue;
                  samples[1]=p->green;
                  samples[2]=p->red;
                  samples[3]=MaxRGB-p->opacity;
                  break;
                case ImageElementABGR:
                  samples[0]=MaxRGB-p->opacity;
                  samples[1]=p->red;
                  samples[2]=p->green;
                  samples[3]=p->blue;
                  break;
                default:
                  break;
                }
              /*
                Scale samples.
              */
              for (i=0; i < samples_per_element; i++)
                samples[i]=ScaleQuantumToShort(samples[i])/scale_from_short;

              /*
                Output samples.
              */
              if (word_pad_lsb)
                {
                  /*
                    Padding in LSB (Method A).
                  */
                  if (bits_per_sample == 10)
                    {
                      for (i=0; i < samples_per_element; i++)
                        {
                          if (word_stream.bits_remaining == 32)
                            (void) WordStreamLSBWrite(&word_stream,2,0);
                          WordStreamLSBWrite(&word_stream,
                                             bits_per_sample,samples[i]);
                        }
                    }
                  else if (bits_per_sample == 12)
                    {
                      for (i=0; i < samples_per_element; i++)
                        {
                          (void) WordStreamLSBWrite(&word_stream,4,0);
                          (void) WordStreamLSBWrite(&word_stream,
                                                    bits_per_sample,samples[i]);
                        }
                    }
                }
              else if (word_pad_msb)
                {
                  /*
                    Padding in MSB (Method B).
                  */
                  if (bits_per_sample == 10)
                    {
                      for (i=0; i < samples_per_element; i++)
                        {
                          if (word_stream.bits_remaining == 2)
                            (void) WordStreamLSBWrite(&word_stream,2,0);
                          (void) WordStreamLSBWrite(&word_stream,
                                                    bits_per_sample,samples[i]);
                        }
                    }
                  else if (bits_per_sample == 12)
                    {
                      for (i=0; i < samples_per_element; i++)
                        {
                          (void) WordStreamLSBWrite(&word_stream,
                                                    bits_per_sample,samples[i]);
                          (void) WordStreamLSBWrite(&word_stream,4,0);
                        }
                    }
                }
              else
                {
                  /*
                    Packed data. Packing is broken on scan-line boundaries.
                  */
                  for (i=0; i < samples_per_element; i++)
                    (void) WordStreamLSBWrite(&word_stream,
                                              bits_per_sample,samples[i]);
                }
              p++;
            }
          /*
            Flush packed row.
          */
          if (packing_method == PackingMethodPacked)
            WordStreamLSBWriteFlush(&word_stream);
        }
      /*
        Flush any remaining element output.
      */
      WordStreamLSBWriteFlush(&word_stream);
    }


  if ((magick_off_t) dpx_file_info.file_size != TellBlob(image))
    {
      printf("### File length %u, TellBlob says %u\n",
             dpx_file_info.file_size,
             (unsigned int) TellBlob(image));
    }

  CloseBlob(image);  
  return(status);
}
