/*
% Copyright (C) 2003-2015 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        PPPP    AAA   L      M   M                           %
%                        P   P  A   A  L      MM MM                           %
%                        PPPP   AAAAA  L      M M M                           %
%                        P      A   A  L      M   M                           %
%                        P      A   A  LLLLL  M   M                           %
%                                                                             %
%                                                                             %
%                          Read/Write Palm Pixmap.                            %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                            Christopher R. Hawks                             %
%                               December 2001                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Based on pnmtopalm by Bill Janssen and ppmtobmp by Ian Goldberg.
%  See http://www.trantor.de/kawt/doc/palmimages.html
%
% gm convert MasterImage_70x46.ppm -monochrome -colors 2 pnm:- | pnmtopalm -verbose -depth 1 > input_gray_01bit.palm
%
% gm convert MasterImage_70x46.ppm -colorspace gray -map pnm:/usr/share/netpbm/palmgray2.map pnm:- | pnmtopalm -verbose -depth 2 > input_gray_02bit.palm
%
% gm convert MasterImage_70x46.ppm -colorspace gray -map pnm:/usr/share/netpbm/palmgray4.map pnm:- | pnmtopalm -verbose -depth 4 > input_gray_04bit.palm
%
% gm convert MasterImage_70x46.ppm -map pnm:/usr/share/netpbm/palmcolor8.map pnm:- | pnmtopalm -verbose -depth 8 > input_rgb_08bit.palm
%
% pnmtopalm -verbose -depth 16 < MasterImage_70x46.ppm > input_rgb_16bit.palm
%
% Add -rle or -scanline to add compression.  Add -transparent color to make color transparent.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/paint.h"
#include "magick/pixel_cache.h"
#include "magick/quantize.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define PALM_IS_COMPRESSED_FLAG         0x8000
#define PALM_HAS_COLORMAP_FLAG          0x4000
#define PALM_HAS_TRANSPARENCY_FLAG      0x2000
#define PALM_INDIRECT_BITMAP_FLAG       0x1000
#define PALM_FOR_SCREEN_FLAG            0x0800
#define PALM_DIRECT_COLOR_FLAG          0x0400
#define PALM_INDIRECT_COLORMAP_FLAG     0x0200 /* or PALM_HAS_FOUR_BYTE_FIELD? */
#define PALM_NO_DITHER_FLAG             0x0100

#define PALM_COMPRESSION_SCANLINE       0x00 /* Scanline (Palm OS 2.0) */
#define PALM_COMPRESSION_RLE            0x01 /* RLE (Palm OS 3.5) */
#define PALM_COMPRESSION_PACKBITS       0x02 /* Packbits (Palm OS 4.0) */
#define PALM_COMPRESSION_END            0x03
#define PALM_COMPRESSION_BEST           0x64
#define PALM_COMPRESSION_NONE           0xFF

/*
  2 color (1 bit) palette
*/
static unsigned char
PalmPalette1[2][3] =
  {
    {  0,  0,  0},
    {255,255,255}
  };

/*
  4 color (2 bit) palette
*/
static unsigned char
PalmPalette2[4][3] =
  {
    {  0,  0,  0},
    { 85, 85, 85},
    {170,170,170},
    {255,255,255}
  };

/*
  16 color (4 bit) palette
*/
static unsigned char
PalmPalette4[16][3] =
  {
    {  0,  0,  0},
    { 17, 17, 17},
    { 34, 34, 34},
    { 51, 51, 51},
    { 68, 68, 68},
    { 85, 85, 85},
    {102,102,102},
    {119,119,119},
    {136,136,136},
    {153,153,153},
    {170,170,170},
    {187,187,187},
    {204,204,204},
    {221,221,221},
    {238,238,238},
    {255,255,255}
  };

/*
 The 256 color system palette for Palm Computing Devices.
*/
static unsigned char
  PalmPalette[256][3] =
  {
    {255, 255,255},
    {255, 204,255},
    {255, 153,255},
    {255, 102,255},
    {255,  51,255},
    {255,   0,255},
    {255, 255,204},
    {255, 204,204},
    {255, 153,204},
    {255, 102,204},
    {255,  51,204},
    {255,   0,204},
    {255, 255,153},
    {255, 204,153},
    {255, 153,153},
    {255, 102,153},
    {255,  51,153},
    {255,   0,153},
    {204, 255,255},
    {204, 204,255},
    {204, 153,255},
    {204, 102,255},
    {204,  51,255},
    {204,   0,255},
    {204, 255,204},
    {204, 204,204},
    {204, 153,204},
    {204, 102,204},
    {204,  51,204},
    {204,   0,204},
    {204, 255,153},
    {204, 204,153},
    {204, 153,153},
    {204, 102,153},
    {204,  51,153},
    {204,   0,153},
    {153, 255,255},
    {153, 204,255},
    {153, 153,255},
    {153, 102,255},
    {153,  51,255},
    {153,   0,255},
    {153, 255,204},
    {153, 204,204},
    {153, 153,204},
    {153, 102,204},
    {153,  51,204},
    {153,   0,204},
    {153, 255,153},
    {153, 204,153},
    {153, 153,153},
    {153, 102,153},
    {153,  51,153},
    {153,   0,153},
    {102, 255,255},
    {102, 204,255},
    {102, 153,255},
    {102, 102,255},
    {102,  51,255},
    {102,   0,255},
    {102, 255,204},
    {102, 204,204},
    {102, 153,204},
    {102, 102,204},
    {102,  51,204},
    {102,   0,204},
    {102, 255,153},
    {102, 204,153},
    {102, 153,153},
    {102, 102,153},
    {102,  51,153},
    {102,   0,153},
    { 51, 255,255},
    { 51, 204,255},
    { 51, 153,255},
    { 51, 102,255},
    { 51,  51,255},
    { 51,   0,255},
    { 51, 255,204},
    { 51, 204,204},
    { 51, 153,204},
    { 51, 102,204},
    { 51,  51,204},
    { 51,   0,204},
    { 51, 255,153},
    { 51, 204,153},
    { 51, 153,153},
    { 51, 102,153},
    { 51,  51,153},
    { 51,   0,153},
    {  0, 255,255},
    {  0, 204,255},
    {  0, 153,255},
    {  0, 102,255},
    {  0,  51,255},
    {  0,   0,255},
    {  0, 255,204},
    {  0, 204,204},
    {  0, 153,204},
    {  0, 102,204},
    {  0,  51,204},
    {  0,   0,204},
    {  0, 255,153},
    {  0, 204,153},
    {  0, 153,153},
    {  0, 102,153},
    {  0,  51,153},
    {  0,   0,153},
    {255, 255,102},
    {255, 204,102},
    {255, 153,102},
    {255, 102,102},
    {255,  51,102},
    {255,   0,102},
    {255, 255, 51},
    {255, 204, 51},
    {255, 153, 51},
    {255, 102, 51},
    {255,  51, 51},
    {255,   0, 51},
    {255, 255,  0},
    {255, 204,  0},
    {255, 153,  0},
    {255, 102,  0},
    {255,  51,  0},
    {255,   0,  0},
    {204, 255,102},
    {204, 204,102},
    {204, 153,102},
    {204, 102,102},
    {204,  51,102},
    {204,   0,102},
    {204, 255, 51},
    {204, 204, 51},
    {204, 153, 51},
    {204, 102, 51},
    {204,  51, 51},
    {204,   0, 51},
    {204, 255,  0},
    {204, 204,  0},
    {204, 153,  0},
    {204, 102,  0},
    {204,  51,  0},
    {204,   0,  0},
    {153, 255,102},
    {153, 204,102},
    {153, 153,102},
    {153, 102,102},
    {153,  51,102},
    {153,   0,102},
    {153, 255, 51},
    {153, 204, 51},
    {153, 153, 51},
    {153, 102, 51},
    {153,  51, 51},
    {153,   0, 51},
    {153, 255,  0},
    {153, 204,  0},
    {153, 153,  0},
    {153, 102,  0},
    {153,  51,  0},
    {153,   0,  0},
    {102, 255,102},
    {102, 204,102},
    {102, 153,102},
    {102, 102,102},
    {102,  51,102},
    {102,   0,102},
    {102, 255, 51},
    {102, 204, 51},
    {102, 153, 51},
    {102, 102, 51},
    {102,  51, 51},
    {102,   0, 51},
    {102, 255,  0},
    {102, 204,  0},
    {102, 153,  0},
    {102, 102,  0},
    {102,  51,  0},
    {102,   0,  0},
    { 51, 255,102},
    { 51, 204,102},
    { 51, 153,102},
    { 51, 102,102},
    { 51,  51,102},
    { 51,   0,102},
    { 51, 255, 51},
    { 51, 204, 51},
    { 51, 153, 51},
    { 51, 102, 51},
    { 51,  51, 51},
    { 51,   0, 51},
    { 51, 255,  0},
    { 51, 204,  0},
    { 51, 153,  0},
    { 51, 102,  0},
    { 51,  51,  0},
    { 51,   0,  0},
    {  0, 255,102},
    {  0, 204,102},
    {  0, 153,102},
    {  0, 102,102},
    {  0,  51,102},
    {  0,   0,102},
    {  0, 255, 51},
    {  0, 204, 51},
    {  0, 153, 51},
    {  0, 102, 51},
    {  0,  51, 51},
    {  0,   0, 51},
    {  0, 255,  0},
    {  0, 204,  0},
    {  0, 153,  0},
    {  0, 102,  0},
    {  0,  51,  0},
    { 17,  17, 17},
    { 34,  34, 34},
    { 68,  68, 68},
    { 85,  85, 85},
    {119, 119,119},
    {136, 136,136},
    {170, 170,170},
    {187, 187,187},
    {221, 221,221},
    {238, 238,238},
    {192, 192,192},
    {128,   0,  0},
    {128,   0,128},
    {  0, 128,  0},
    {  0, 128,128},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0},
    {  0,   0,  0}
  };

typedef struct _PalmHeader
{
  /* Image width */
  magick_uint16_t columns;
  /* Image height */
  magick_uint16_t rows;
  /* Number of bytes in each row of the image (must be even) */
  magick_uint16_t bytes_per_row;
  /* Flags:
     0x8000 : Compressed
     0x4000 : Has colormap
     0x2000 : Has transparent color
  */
  magick_uint16_t flags;
  /* Number of bits per pixel */
  magick_uint8_t  bits_per_pixel;
  /* Version:
     0: Palm OS 1
     1: Palm OS 3, no transparency, no RLE compression.
     2: Palm OS 3.5; transparency and RLE compression supported 
  */
  magick_uint8_t  version;
  /* Offset (in four byte words) to start of next image frame. */
  magick_uint16_t next_depth_offset;
  /* Index of transparent color if transparent flag set */
  magick_uint8_t  transparent_index;
  /* Compression type:
     0 : scanline
     1 : RLE
  */
  magick_uint8_t  compression_type;
  /* Reserved for future extensions (overlaps with V3 size and pixel_format) */
  magick_uint8_t  reserved[2];
/* 16 byte mark */

#if 0
  /*
    Version 3 parts below
  */
  /* Bitmap header size (usually 24) */
  magick_uint8_t  size;

  /* Pixel format */
  magick_uint8_t  pixel_format;

  /* Screen density (72, 108, 144, 216, 288) */
  magick_uint16_t  density;

  /* Transparent value */
  magick_uint32_t  transparent_value;

#endif

} PalmHeader;

/*
  Forward declarations.
*/
static unsigned int
  WritePALMImage(const ImageInfo *,Image *);

void LogPALMHeader(const PalmHeader* palm_header)
{
  static const struct
  {
    unsigned short mask;
    const char * text;
  }
  FlagDecodes[] =
    {
      { PALM_IS_COMPRESSED_FLAG,    "COMPRESSED" },
      { PALM_HAS_COLORMAP_FLAG,     "COLORMAP" },
      { PALM_HAS_TRANSPARENCY_FLAG, "TRANSPARENCY" },
      { PALM_INDIRECT_BITMAP_FLAG,  "INDIRECT_BITMAP" },
      { PALM_FOR_SCREEN_FLAG,       "FOR_SCREEN" },
      { PALM_DIRECT_COLOR_FLAG,     "DIRECT_COLOR" },
      { PALM_INDIRECT_COLORMAP_FLAG,"INDIRECT_COLORMAP" },
      { PALM_NO_DITHER_FLAG,        "NO_DITHER" }
    };

  char flags[MaxTextExtent];
  unsigned int i;

  flags[0]='\0';

  for (i=0; i < sizeof(FlagDecodes)/sizeof(FlagDecodes[0]); i++)
    {
      if (palm_header->flags & FlagDecodes[i].mask)
        {
          if (flags[0])
            (void) strlcat(flags,", ",sizeof(flags));
          (void) strlcat(flags,FlagDecodes[i].text,sizeof(flags));
        }
    }

  (void) LogMagickEvent
    (CoderEvent,GetMagickModule(),
     "PALMHeader:\n"
     "     Columns:          %u\n"
     "     Rows:             %u\n"
     "     BytesPerRow:      %u\n"
     "     Flags:            0x%04x (%s)\n"
     "     BitsPerPixel:     %u\n"
     "     Version:          %u\n"
     "     NextDepthOffset:  %u\n"
     "     TransparentIndex: %u\n"
     "     Compression:      %u (%s)",
     palm_header->columns,
     palm_header->rows,
     palm_header->bytes_per_row,
     palm_header->flags,
     flags,
     palm_header->bits_per_pixel,
     palm_header->version,
     palm_header->next_depth_offset,
     palm_header->transparent_index,
     palm_header->compression_type,
     (palm_header->compression_type == PALM_COMPRESSION_SCANLINE ? "Scanline" :
      (palm_header->compression_type == PALM_COMPRESSION_RLE ? "RLE" :
       (palm_header->compression_type == PALM_COMPRESSION_PACKBITS ? "PackBits" :
        (palm_header->compression_type == PALM_COMPRESSION_NONE ? "None" : "?")))));
}

static const unsigned char *
GetPalmPaletteGivenBits(const unsigned int bits,
                        size_t *size)
{
  const unsigned char
    *palette = (const unsigned char *) NULL;

  *size=0;
  switch (bits)
    {
    case 1:
      palette=(const unsigned char *) PalmPalette1;
      *size=sizeof(PalmPalette1);
      break;
    case 2:
      palette=(const unsigned char *) PalmPalette2;
      *size=sizeof(PalmPalette2);
      break;
    case 4:
      palette=(const unsigned char *) PalmPalette4;
      *size=sizeof(PalmPalette4);
      break;
    case 8:
      palette=(const unsigned char *) PalmPalette;
      *size=sizeof(PalmPalette);
      break;
    }
  return palette;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i n d C o l o r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FindColor returns the index of the matching entry from
%  PalmPalette for a given PixelPacket.
%
%  The format of the FindColor method is:
%
%      int FindColor(PixelPacket *pixel)
%
%  A description of each parameter follows:
%
%    o int: the index of the matching color or -1 if not found/
%
%    o pixel: a pointer to the PixelPacket to be matched.
%
%
*/
static int FindColor(PixelPacket *pixel)
{
  register long
    i;

  for (i=0; i < 256; i++)
    if (ScaleQuantumToChar(pixel->red) == PalmPalette[i][0] &&
        ScaleQuantumToChar(pixel->green) == PalmPalette[i][1] &&
        ScaleQuantumToChar(pixel->blue) == PalmPalette[i][2])
      return(i);
  return(-1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P A L M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPALMImage reads an image of raw bites in LSB order and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPALMImage method is:
%
%      Image *ReadPALMImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPALMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define ThrowPALMReaderException(code_,reason_,image_) \
do { \
  MagickFreeMemory(one_row); \
  MagickFreeMemory(lastrow); \
  ThrowReaderException(code_,reason_,image_); \
} while (0);
static Image *ReadPALMImage(const ImageInfo *image_info,
                            ExceptionInfo *exception)
{
  Image
    *image;

  IndexPacket
    index;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  PixelPacket
    transpix;

  register PixelPacket
    *q;

  unsigned char
    *lastrow,
    *one_row,
    *ptr;

  unsigned int
    status;

  unsigned int
    i,
    mask,
    bit;

  PalmHeader
    palm_header;

  unsigned short
    color16;

  /*
    Open image file.
  */
  one_row=(unsigned char*) NULL;
  lastrow=(unsigned char*) NULL;

  (void) memset(&palm_header,0,sizeof(palm_header));
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowPALMReaderException(FileOpenError,UnableToOpenFile,image);

  palm_header.columns = ReadBlobMSBShort(image);
  palm_header.rows = ReadBlobMSBShort(image);
  palm_header.bytes_per_row = ReadBlobMSBShort(image);
  palm_header.flags = ReadBlobMSBShort(image);
  palm_header.bits_per_pixel = ReadBlobByte(image);
  palm_header.version = ReadBlobByte(image);
  palm_header.next_depth_offset = ReadBlobMSBShort(image);
  palm_header.transparent_index = ReadBlobByte(image);
  palm_header.compression_type = ReadBlobByte(image);
  if (ReadBlob(image,sizeof(palm_header.reserved),&palm_header.reserved) != 2)
    ThrowPALMReaderException(CorruptImageError,UnexpectedEndOfFile,image);

  if (image->logging)
    LogPALMHeader(&palm_header);

  /*
    Validate version (0-2, 3 not supported yet)
  */
  if ((palm_header.version != 0U) &&
      (palm_header.version != 1U) &&
      (palm_header.version != 2U))
    ThrowPALMReaderException(CorruptImageError,InvalidFileFormatVersion,image);

  /*
    Validate bits per pixel.
  */
  if ((palm_header.bits_per_pixel != 1) &&
      (palm_header.bits_per_pixel != 2) &&
      (palm_header.bits_per_pixel != 4) &&
      (palm_header.bits_per_pixel != 8) &&
      (palm_header.bits_per_pixel != 16))
    ThrowPALMReaderException(CorruptImageError,UnrecognizedBitsPerPixel,image);

  /*
    Validate compression type.
  */
  if ((palm_header.compression_type != PALM_COMPRESSION_NONE) &&
      (palm_header.compression_type != PALM_COMPRESSION_SCANLINE ) &&
      (palm_header.compression_type != PALM_COMPRESSION_RLE))
    ThrowPALMReaderException(CorruptImageError,UnrecognizedImageCompression,image);

  /*
    Validate flags.
  */
  if ((palm_header.version < 2) &&
      ((palm_header.flags & PALM_HAS_TRANSPARENCY_FLAG) ||
       ((palm_header.flags & PALM_IS_COMPRESSED_FLAG) &&
        (palm_header.compression_type == PALM_COMPRESSION_RLE))
       ))
    ThrowPALMReaderException(CorruptImageError,ImproperImageHeader,image);

  /*
    Validate bytes per row sanity checks
  */
  if (
      (palm_header.bytes_per_row % 2)
      ||
      ((palm_header.bits_per_pixel == 16) &&
       ((size_t) palm_header.columns >
        (size_t) 2*palm_header.bytes_per_row))
      ||
      (((size_t) palm_header.bytes_per_row*8) <
       ((size_t) palm_header.columns*palm_header.bits_per_pixel)))
    ThrowPALMReaderException(CorruptImageError,ImproperImageHeader,image);

  image->columns = palm_header.columns;
  image->rows = palm_header.rows;

  ClearPixelPacket(&transpix);
  if (palm_header.bits_per_pixel == 16)  /* Direct Color */
    {
      (void) ReadBlobByte(image);     /* # of bits of red */
      (void) ReadBlobByte(image);    /* # of bits of green */
      (void) ReadBlobByte(image);    /* # of bits of blue */
      (void) ReadBlobByte(image);    /* reserved by Palm */
      (void) ReadBlobByte(image);    /* reserved by Palm */
      transpix.red = (unsigned char) (ReadBlobByte(image) * MaxRGB / 31);
      transpix.green = (unsigned char) (ReadBlobByte(image) * MaxRGB / 63);
      transpix.blue = (unsigned char) (ReadBlobByte(image) * MaxRGB / 31);
    }

  /*
    Initialize image colormap.
  */
  if ((palm_header.bits_per_pixel < 16) &&
      !AllocateImageColormap(image,1L << palm_header.bits_per_pixel))
    ThrowPALMReaderException(ResourceLimitError,MemoryAllocationFailed,image);

  if ((palm_header.bits_per_pixel == 1) ||
      (palm_header.bits_per_pixel == 2) ||
      (palm_header.bits_per_pixel == 4) ||
      ((palm_header.bits_per_pixel == 8) &&
       !(palm_header.flags & PALM_HAS_COLORMAP_FLAG)))
    {
      const unsigned char
        *palette;

      size_t
        size;

      palette=GetPalmPaletteGivenBits(palm_header.bits_per_pixel,
                                      &size);
      if (palette)
        {
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Default %u bit palette of %"
                                  MAGICK_SIZE_T_F "u bytes...",
                                  palm_header.bits_per_pixel,
                                  (MAGICK_SIZE_T) size);
          for (index=0; index < image->colors; index++)
            {
              image->colormap[index].red=ScaleCharToQuantum(*palette++);
              image->colormap[index].green=ScaleCharToQuantum(*palette++);
              image->colormap[index].blue=ScaleCharToQuantum(*palette++);
            }
        }
    }

  else if (palm_header.bits_per_pixel == 8)
    {
      i = 0;
      if (palm_header.flags & PALM_HAS_COLORMAP_FLAG)
        {
          unsigned int
            count;

          count = ReadBlobMSBShort(image);

          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Custom %u bit palette with %u colors (%"
                                  MAGICK_SIZE_T_F "u bytes)...",
                                  palm_header.bits_per_pixel,
                                  count,
                                  (MAGICK_SIZE_T) count*3);

          if (count > image->colors)
            ThrowPALMReaderException(CorruptImageError,ColormapExceedsColorsLimit,image);

          for (i = 0; i < count; i++)
            {
              (void) ReadBlobByte(image);
              index=255 - i;
              VerifyColormapIndex(image,index);
              image->colormap[index].red = ScaleCharToQuantum(ReadBlobByte(image));
              image->colormap[index].green = ScaleCharToQuantum(ReadBlobByte(image));
              image->colormap[index].blue = ScaleCharToQuantum(ReadBlobByte(image));
              if (EOFBlob(image))
                ThrowPALMReaderException(CorruptImageError,UnexpectedEndOfFile,image);
            }
        }
      for (; i < (1U << palm_header.bits_per_pixel); i++)
        {
          index=255 - i;
          VerifyColormapIndex(image,index);
          image->colormap[index].red = ScaleCharToQuantum(PalmPalette[i][0]);
          image->colormap[index].green = ScaleCharToQuantum(PalmPalette[i][1]);
          image->colormap[index].blue = ScaleCharToQuantum(PalmPalette[i][2]);
          if (EOFBlob(image))
            ThrowPALMReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        }
    }

  if (palm_header.bits_per_pixel < 16)
    {
      image->storage_class = PseudoClass;
      image->depth = 8;
    }
  else
    {
      image->storage_class = DirectClass;
      image->depth = 8;
    }

#if 0
  if (image->storage_class == PseudoClass)
    for (i=0; i < image->colors; i++)
      fprintf(stderr,"%03u: %3u, %3u, %3u\n",
              i,
              ScaleQuantumToChar(image->colormap[i].red),
              ScaleQuantumToChar(image->colormap[i].green),
              ScaleQuantumToChar(image->colormap[i].blue));
#endif

  image->compression = NoCompression;
  if (palm_header.flags & PALM_IS_COMPRESSED_FLAG)
    {
      if (palm_header.compression_type == PALM_COMPRESSION_RLE)
        image->compression = RLECompression;
      else if (palm_header.compression_type == PALM_COMPRESSION_SCANLINE)
        image->compression = FaxCompression;
    }

  /*
    Skip reading pixels if ping requested.
  */
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }

  if (CheckImagePixelLimits(image, exception) != MagickPass)
    ThrowPALMReaderException(ResourceLimitError,ImagePixelLimitExceeded,image);

  one_row = MagickAllocateMemory(unsigned char *,Max(palm_header.bytes_per_row,2*image->columns));
  if (one_row == (unsigned char *) NULL)
    ThrowPALMReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  if (palm_header.compression_type == PALM_COMPRESSION_SCANLINE)
    lastrow = MagickAllocateMemory(unsigned char *,Max(palm_header.bytes_per_row,2*image->columns));

  mask = (1l << palm_header.bits_per_pixel) - 1;

  for (y = 0; y < (long) image->rows; y++)
    {
      if (palm_header.flags & PALM_IS_COMPRESSED_FLAG)
        {
          int
            count,
            byte;

          if (palm_header.compression_type == PALM_COMPRESSION_RLE)
            {
              for (i = 0; i < palm_header.bytes_per_row; )
                {
                  if ((count = ReadBlobByte(image)) == EOF)
                    break;
                  count = Min((unsigned int) count, (unsigned int) palm_header.bytes_per_row-i);
                  if (count == 0)
                    break;
                  if ((byte = ReadBlobByte(image)) == EOF)
                    break;
                  (void) memset(one_row + i, byte, count);
                  i += count;
                }
            }
          else
            if (palm_header.compression_type == PALM_COMPRESSION_SCANLINE)
              {
                for (i = 0; i < (long) palm_header.bytes_per_row; i += 8)
                  {
                    if ((count = ReadBlobByte(image)) == EOF)
                      break;
                    byte = Min(palm_header.bytes_per_row - i, 8);
                    for (bit = 0; bit < (unsigned int) byte; bit++)
                      {
                        if ((y == 0) || (count & 0xff & (1 << (7 - bit))))
                          {
                            int
                              c;

                            if ((c = ReadBlobByte(image)) == EOF)
                              break;
                            one_row[i + bit] = c & 0xff;
                          }
                        else
                          {
                            one_row[i + bit] = lastrow[i + bit];
                          }
                      }
                  }
                (void) memcpy(lastrow, one_row, palm_header.bytes_per_row);
              }
        }
      else
        {
          (void) ReadBlob(image, palm_header.bytes_per_row, one_row);
        }

      if (EOFBlob(image))
        ThrowPALMReaderException(CorruptImageError,UnexpectedEndOfFile,image);

      ptr = one_row;
      q = SetImagePixels(image, 0, y, image->columns, 1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=AccessMutableIndexes(image);
      if (palm_header.bits_per_pixel == 16)
        {
          if (image->columns > 2*palm_header.bytes_per_row)
            ThrowPALMReaderException(CorruptImageError,CorruptImage,image);
          for (x=0; x < (long) image->columns; x++)
            {
              color16 = (*ptr++ << 8);
              color16 |= *ptr++;
              q->red = (MaxRGB*((color16 >> 11) & 0x1f))/0x1f;
              q->green = (MaxRGB*((color16 >> 5) & 0x3f))/0x3f;
              q->blue = (MaxRGB*((color16 >> 0) & 0x1f))/0x1f;
              q->opacity = OpaqueOpacity;
              q++;
            }
        }
      else
        {
          bit = 8 - palm_header.bits_per_pixel;
          for (x = 0; x < (long) image->columns; x++)
            {
              if ((unsigned int) (ptr - one_row) >= palm_header.bytes_per_row)
                ThrowPALMReaderException(CorruptImageError,CorruptImage,image);
              index =(IndexPacket) (mask - (((*ptr) & (mask << bit)) >> bit));
              VerifyColormapIndex(image,index);
              indexes[x] = index;
              *q++ = image->colormap[index];
              if (!bit)
                {
                  ++ptr;
                  bit = 8 - palm_header.bits_per_pixel;
                }
              else
                {
                  bit -= palm_header.bits_per_pixel;
                }
            }
        }
      if (!SyncImagePixels(image))
        break;
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
                                    LoadImageText,image->filename,
                                    image->columns,image->rows))
          break;
    }

  if (palm_header.flags & PALM_HAS_TRANSPARENCY_FLAG)
    {
      if (image->storage_class == PseudoClass)
        {
          index = mask - palm_header.transparent_index;
          VerifyColormapIndex(image,index);
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Index %u rgb(%u,%u,%u) set transparent",
                                  index,
                                  image->colormap[index].red,
                                  image->colormap[index].green,
                                  image->colormap[index].blue);
          image->colormap[index].opacity=TransparentOpacity;
          SyncImage(image);
          image->matte=MagickTrue;
          image->storage_class=DirectClass;
        }
      else
        {
          if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "rgb(%u,%u,%u) set transparent",
                                  transpix.red,
                                  transpix.green,
                                  transpix.blue);
          (void) TransparentImage(image, transpix, TransparentOpacity);
        }
    }

  MagickFreeMemory(one_row);
  MagickFreeMemory(lastrow);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P A L M I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPALMImage adds attributes for the PALM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPALMImage method is:
%
%      RegisterPALMImage(void)
%
*/
ModuleExport void RegisterPALMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PALM");
  entry->decoder=ReadPALMImage;
  entry->encoder=WritePALMImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Palm pixmap";
  entry->module="PALM";
  entry->coder_class=UnstableCoderClass;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P A L M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPALMImage removes format registrations made by the
%  PALM module from the list of supported formats.
%
%  The format of the UnregisterPALMImage method is:
%
%      UnregisterPALMImage(void)
%
*/
ModuleExport void UnregisterPALMImage(void)
{
  (void) UnregisterMagickInfo("PALM");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P A L M I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePALMImage writes an image of raw bits in LSB order to a file.
%
%  The format of the WritePALMImage method is:
%
%      unsigned int WritePALMImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePALMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
#if 0
static Image *
OptimizePALMImage(const ImageInfo *image_info,Image *image)
{
  Image
    *map_image,
    *optimize_image;

  ImageCharacteristics
    characteristics;

  unsigned int
    depth;

  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    return (Image *) NULL;

  for (depth = 1; depth < 8; depth *= 2)
    {
      // PalmPalette1, PalmPalette2, PalmPalette4, PalmPalette
      optimize_image=CloneImage(image,0,0,True,&image->exception);
    }

  image->error->normalized_mean_error;

  return 0;
}
#endif
static unsigned int WritePALMImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  QuantizeInfo
    quantize_info;

  Image
    *map;

  register IndexPacket
    *indexes;

  register long
    x;

  PixelPacket
    transpix;

  register PixelPacket
    *p;

  unsigned char
    bit,
    byte,
    color,
    *lastrow = 0,
    *one_row = 0,
    *ptr,
    version = 0;

  unsigned int
    transparentIndex = 0,
    status;

  unsigned int
    count = 0,
    bits_per_pixel,
    bytes_per_row;

  unsigned short
    color16;

  ImageCharacteristics
    characteristics;

  PalmHeader
    palm_header;

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Dimensions: %lux%lu",
                          image->columns,image->rows);

  (void) memset(&palm_header,0,sizeof(palm_header));

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Make sure that image is in an RGB type space.
  */
  (void) TransformColorspace(image,RGBColorspace);

  /*
    If image is colormapped, but there are too many colors, then force
    to DirectClass.
  */
  if ((image->storage_class == PseudoClass) && (image->colors > 256))
    image->storage_class=DirectClass;

  /*
    Analyze image to be written.
  */
  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    {
      CloseBlob(image);
      return MagickFail;
    }

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Characteristics:\n"
                          "    cmyk       : %u\n"
                          "    grayscale  : %u\n"
                          "    monochrome : %u\n"
                          "    opaque     : %u\n"
                          "    palette    : %u",
                          characteristics.cmyk,
                          characteristics.grayscale,
                          characteristics.monochrome,
                          characteristics.opaque,
                          characteristics.palette);

  bits_per_pixel=16; /* Default to 16 */
  if (characteristics.palette)
    {
      palm_header.flags |= PALM_HAS_COLORMAP_FLAG;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Set flag PALM_HAS_COLORMAP_FLAG");
      for (bits_per_pixel=1;  ((1UL << bits_per_pixel) < image->colors) ;  bits_per_pixel*=2)
        {};
      if (characteristics.grayscale)
        (void) SortColormapByIntensity(image);
    }
  else
    {
      palm_header.flags |= PALM_DIRECT_COLOR_FLAG;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Set flag PALM_DIRECT_COLOR_FLAG");
    }
  palm_header.bits_per_pixel=bits_per_pixel;
  LogMagickEvent(CoderEvent,GetMagickModule(),"Bits per pixel: %u",bits_per_pixel);

  /* Write Tbmp header. */
  palm_header.columns=image->columns;
  palm_header.rows=image->rows;
  /* bytes per row - always a word boundary */
  bytes_per_row=((image->columns+(16/bits_per_pixel-1))/(16/bits_per_pixel))*2;
  palm_header.bytes_per_row=bytes_per_row;
  if ((image->compression == RLECompression) ||
      (image->compression == FaxCompression))
    palm_header.flags|=PALM_IS_COMPRESSED_FLAG;
  if (((bytes_per_row*image->rows) > 48000) &&
      (palm_header.flags & PALM_IS_COMPRESSED_FLAG))
    palm_header.flags|=PALM_INDIRECT_COLORMAP_FLAG;
  palm_header.bits_per_pixel=bits_per_pixel;
  if(bits_per_pixel > 1)
    version=1;
  if ((image->compression == RLECompression) ||
      (image->compression == FaxCompression))
    version=2;
  palm_header.version=version;
  palm_header.next_depth_offset=0;
  palm_header.transparent_index=transparentIndex;
  if (image->compression == RLECompression)
    palm_header.compression_type=PALM_COMPRESSION_RLE;
  else
    if (image->compression == FaxCompression)
      palm_header.compression_type=PALM_COMPRESSION_SCANLINE;
    else
      palm_header.compression_type=PALM_COMPRESSION_NONE;
  (void) memset(palm_header.reserved,0,sizeof(palm_header.reserved));

  /*
    Log and write out header.
  */
  if (image->logging)
    LogPALMHeader(&palm_header);

  (void) WriteBlobMSBShort(image,palm_header.columns);  /* width */
  (void) WriteBlobMSBShort(image,palm_header.rows );  /* height */
  (void) WriteBlobMSBShort(image,palm_header.bytes_per_row);
  (void) WriteBlobMSBShort(image, palm_header.flags);
  (void) WriteBlobByte(image, palm_header.bits_per_pixel);
  (void) WriteBlobByte(image,palm_header.version);
  (void) WriteBlobMSBShort(image,palm_header.next_depth_offset);  /* offset */
  (void) WriteBlobByte(image,palm_header.transparent_index);
  (void) WriteBlobByte(image,palm_header.compression_type);
  (void) WriteBlob(image, sizeof(palm_header.reserved), palm_header.reserved);

  if (bits_per_pixel < 8)  /* not color */
    {
      if (palm_header.flags & PALM_IS_COMPRESSED_FLAG)  /* compressed size */
        {
          if (palm_header.flags & PALM_INDIRECT_COLORMAP_FLAG)  /* big size */
            (void) WriteBlobMSBLong(image, 0);
          else
            if (bits_per_pixel == 16)
              {
                (void) WriteBlobByte(image, 5);  /* # of bits of red */
                (void) WriteBlobByte(image, 6);  /* # of bits of green */
                (void) WriteBlobByte(image, 5);  /* # of bits of blue */
                (void) WriteBlobByte(image, 0);  /* reserved by Palm */
                (void) WriteBlobMSBLong(image, 0); /* no transparent color, YET */
              }
            else
              (void) WriteBlobMSBShort(image, 0);
        }
    }
  else  /* is color */
    {
      if (palm_header.flags & PALM_HAS_COLORMAP_FLAG)  /* Write out colormap */
        {
          GetQuantizeInfo(&quantize_info);
          quantize_info.dither=image_info->dither;
          quantize_info.number_colors=image->colors;
          (void) QuantizeImage(&quantize_info,image);
          (void) WriteBlobMSBShort(image, image->colors);
          for (count = 0; count < image->colors; count++)
            {
              (void) WriteBlobByte(image, count);
              (void) WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].red));
              (void) WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].green));
              (void) WriteBlobByte(image, ScaleQuantumToChar(image->colormap[count].blue));
            }
        }
      else  /* Map colors to Palm standard colormap */
        {
          int
            index;

          map = ConstituteImage(256, 1, "RGB", CharPixel,
                                &PalmPalette, &image->exception);
          (void) SetImageType(map, PaletteType);
          (void) MapImage(image, map, False);
          for (y = 0; y < (long) image->rows; y++)
            {
              p = GetImagePixels(image, 0, y, image->columns, 1);
              indexes=AccessMutableIndexes(image);
              for (x = 0; x < (long) image->columns; x++)
                {
                  index = FindColor(&image->colormap[indexes[x]]);
                  if (index < 0)
                    index=0;
                  indexes[x] = index;
                }
            }
          DestroyImage(map);
        }
    }

  if (image->compression == FaxCompression)
    lastrow = MagickAllocateMemory(unsigned char *,bytes_per_row);
  one_row = MagickAllocateMemory(unsigned char *,bytes_per_row);
  if (one_row == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);

  ClearPixelPacket(&transpix);
  for (y=0; y < (int) image->rows; y++)
    {
      ptr = one_row;
      (void) memset(ptr, 0, bytes_per_row);
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      indexes=AccessMutableIndexes(image);
      if (bits_per_pixel == 16)
        {
          for (x=0; x < (int) image->columns; x++)
            {
              color16 = (unsigned short) (((p->red * 31) / MaxRGB) << 11
                                          | ((p->green * 63) / MaxRGB) << 5
                                          | ((p->blue * 31) / MaxRGB));
              if (p->opacity == TransparentOpacity)
                {
                  transpix.red = p->red;
                  transpix.green = p->green;
                  transpix.blue = p->blue;
                  transpix.opacity = p->opacity;
                  palm_header.flags |= PALM_HAS_TRANSPARENCY_FLAG;
                }
              *ptr++ = (color16 >> 8) &0xff;
              *ptr++ = color16 & 0xff;
              p++;
            }
        }
      else
        {
          byte = 0x00;
          bit = (unsigned char) (8 - bits_per_pixel);
          for (x=0; x < (int) image->columns; x++)
            {
              if (bits_per_pixel < 8) /* Make sure we use the entire colorspace for bits_per_pixel */
                color = (unsigned char) (indexes[x] * ((1 << bits_per_pixel) - 1) /
                                         (image->colors - 1));
              else
                color = (unsigned char) indexes[x];
              byte |= color << bit;
              if (bit == 0)
                {
                  *ptr++ = byte;
                  byte = 0x00;
                  bit = (unsigned char) (8 - bits_per_pixel);
                }
              else
                {
                  bit -= bits_per_pixel;
                }
            }
          if ((image->columns % (8 / bits_per_pixel)) != 0) /* Handle case of partial byte */
            *ptr++ = byte;
        }

      if (image->compression == RLECompression)
        {
          x = 0;
          while (x < (long) bytes_per_row)
            {
              byte = one_row[x];
              count = 1;
              while (one_row[++x] == byte && count < 255 && x < (long) bytes_per_row)
                count++;
              (void) WriteBlobByte(image, count);
              (void) WriteBlobByte(image, byte);
            }
        }
      else
        if (image->compression == FaxCompression)  /* Scanline really */
          {
            char tmpbuf[8];
            char *tptr;

            for (x = 0;  x < (long) bytes_per_row;  x += 8)
              {
                tptr = tmpbuf;
                for (bit = 0, byte = 0; bit < Min(8, bytes_per_row - x); bit++)
                  {
                    if ((y == 0) || (lastrow[x + bit] != one_row[x + bit]))
                      {
                        byte |= (1 << (7 - bit));
                        *tptr++ = one_row[x + bit];
                      }
                  }
                (void) WriteBlobByte(image, byte);
                (void) WriteBlob(image, tptr - tmpbuf, tmpbuf);
              }
            (void) memcpy (lastrow, one_row, bytes_per_row);
          }
        else
          {
            (void) WriteBlob(image, bytes_per_row, one_row);
          }
    }

  if (bits_per_pixel < 8 && palm_header.flags & PALM_IS_COMPRESSED_FLAG)
    {
      count = (unsigned long) GetBlobSize(image);  /* compressed size */
    }

  if (palm_header.flags & PALM_HAS_TRANSPARENCY_FLAG)
    {
      (void) SeekBlob(image, 6, SEEK_SET);
      (void) WriteBlobMSBShort(image, palm_header.flags);
      (void) SeekBlob(image, 12, SEEK_SET);
      (void) WriteBlobByte(image,transparentIndex);  /* trans index */

      if (bits_per_pixel == 16)
        {
          (void) SeekBlob(image, 20, SEEK_SET);
          (void) WriteBlobByte(image, 0);  /* reserved by Palm */
          (void) WriteBlobByte(image, (transpix.red * 31) / MaxRGB);
          (void) WriteBlobByte(image, (transpix.green * 63) / MaxRGB);
          (void) WriteBlobByte(image, (transpix.blue * 31) / MaxRGB);
        }
      (void) SeekBlob(image, 16, SEEK_SET);
      if (palm_header.flags & PALM_INDIRECT_COLORMAP_FLAG)
        (void) WriteBlobMSBLong(image, count - 16);
      else
        (void) WriteBlobMSBShort(image, count - 16);
    }

  CloseBlob(image);
  MagickFreeMemory(one_row);
  MagickFreeMemory(lastrow);
  return(True);
}
