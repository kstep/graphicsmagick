/*
  Macintosh PICT definitions.
*/
#define ReadPixmap(pixmap) \
{ \
  pixmap.version=MSBFirstReadShort(image->file); \
  pixmap.pack_type=MSBFirstReadShort(image->file); \
  pixmap.pack_size=MSBFirstReadLong(image->file); \
  pixmap.horizontal_resolution=MSBFirstReadLong(image->file); \
  pixmap.vertical_resolution=MSBFirstReadLong(image->file); \
  pixmap.pixel_type=MSBFirstReadShort(image->file); \
  pixmap.bits_per_pixel=MSBFirstReadShort(image->file); \
  pixmap.component_count=MSBFirstReadShort(image->file); \
  pixmap.component_size=MSBFirstReadShort(image->file); \
  pixmap.plane_bytes=MSBFirstReadLong(image->file); \
  pixmap.table=MSBFirstReadLong(image->file); \
  pixmap.reserved=MSBFirstReadLong(image->file); \
}

#define ReadRectangle(rectangle) \
{ \
  rectangle.top=MSBFirstReadShort(image->file); \
  rectangle.left=MSBFirstReadShort(image->file); \
  rectangle.bottom=MSBFirstReadShort(image->file); \
  rectangle.right=MSBFirstReadShort(image->file); \
}

typedef struct _PICTCode
{
  char
    *name;

  int
    length;

  char
    *description;
} PICTCode;

typedef struct _PICTPixmap
{
  short
    version,
    pack_type;

  long int
    pack_size,
    horizontal_resolution,
    vertical_resolution;

  short
    pixel_type,
    bits_per_pixel,
    component_count,
    component_size;

  long int
    plane_bytes,
    table,
    reserved;
} PICTPixmap;

typedef struct _PICTRectangle
{
  short
    top,
    left,
    bottom,
    right;
} PICTRectangle;

static PICTCode
  codes[] =
  {
    /* 0x00 */  { "NOP",            0, "nop" },
    /* 0x01 */  { "Clip",           0, "clip" },
    /* 0x02 */  { "BkPat",          8, "background pattern" },
    /* 0x03 */  { "TxFont",         2, "text font (word)" },
    /* 0x04 */  { "TxFace",         1, "text face (byte)" },
    /* 0x05 */  { "TxMode",         2, "text mode (word)" },
    /* 0x06 */  { "SpExtra",        4, "space extra (fixed point)" },
    /* 0x07 */  { "PnSize",         4, "pen size (point)" },
    /* 0x08 */  { "PnMode",         2, "pen mode (word)" },
    /* 0x09 */  { "PnPat",          8, "pen pattern" },
    /* 0x0a */  { "FillPat",        8, "fill pattern" },
    /* 0x0b */  { "OvSize",         4, "oval size (point)" },
    /* 0x0c */  { "Origin",         4, "dh, dv (word)" },
    /* 0x0d */  { "TxSize",         2, "text size (word)" },
    /* 0x0e */  { "FgColor",        4, "foreground color (longword)" },
    /* 0x0f */  { "BkColor",        4, "background color (longword)" },
    /* 0x10 */  { "TxRatio",        8, "numerator (point), denominator (point)" },
    /* 0x11 */  { "Version",        1, "version (byte)" },
    /* 0x12 */  { "BkPixPat",       0, "color background pattern" },
    /* 0x13 */  { "PnPixPat",       0, "color pen pattern" },
    /* 0x14 */  { "FillPixPat",     0, "color fill pattern" },
    /* 0x15 */  { "PnLocHFrac",     2, "fractional pen position" },
    /* 0x16 */  { "ChExtra",        2, "extra for each character" },
    /* 0x17 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x18 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x19 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x1a */  { "RGBFgCol",       6, "RGB foreColor" },
    /* 0x1b */  { "RGBBkCol",       6, "RGB backColor" },
    /* 0x1c */  { "HiliteMode",     0, "hilite mode flag" },
    /* 0x1d */  { "HiliteColor",    6, "RGB hilite color" },
    /* 0x1e */  { "DefHilite",      0, "Use default hilite color" },
    /* 0x1f */  { "OpColor",        6, "RGB OpColor for arithmetic modes" },
    /* 0x20 */  { "Line",           8, "pnLoc (point), newPt (point)" },
    /* 0x21 */  { "LineFrom",       4, "newPt (point)" },
    /* 0x22 */  { "ShortLine",      6, "pnLoc (point, dh, dv (-128 .. 127))" },
    /* 0x23 */  { "ShortLineFrom",  2, "dh, dv (-128 .. 127)" },
    /* 0x24 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x25 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x26 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x27 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x28 */  { "LongText",       0, "txLoc (point), count (0..255), text" },
    /* 0x29 */  { "DHText",         0, "dh (0..255), count (0..255), text" },
    /* 0x2a */  { "DVText",         0, "dv (0..255), count (0..255), text" },
    /* 0x2b */  { "DHDVText",       0, "dh, dv (0..255), count (0..255), text" },
    /* 0x2c */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x2d */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x2e */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x2f */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x30 */  { "frameRect",      8, "rect" },
    /* 0x31 */  { "paintRect",      8, "rect" },
    /* 0x32 */  { "eraseRect",      8, "rect" },
    /* 0x33 */  { "invertRect",     8, "rect" },
    /* 0x34 */  { "fillRect",       8, "rect" },
    /* 0x35 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x36 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x37 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x38 */  { "frameSameRect",  0, "rect" },
    /* 0x39 */  { "paintSameRect",  0, "rect" },
    /* 0x3a */  { "eraseSameRect",  0, "rect" },
    /* 0x3b */  { "invertSameRect", 0, "rect" },
    /* 0x3c */  { "fillSameRect",   0, "rect" },
    /* 0x3d */  { "reserved",       0, "reserved for Apple use" },
    /* 0x3e */  { "reserved",       0, "reserved for Apple use" },
    /* 0x3f */  { "reserved",       0, "reserved for Apple use" },
    /* 0x40 */  { "frameRRect",     8, "rect" },
    /* 0x41 */  { "paintRRect",     8, "rect" },
    /* 0x42 */  { "eraseRRect",     8, "rect" },
    /* 0x43 */  { "invertRRect",    8, "rect" },
    /* 0x44 */  { "fillRRrect",     8, "rect" },
    /* 0x45 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x46 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x47 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x48 */  { "frameSameRRect", 0, "rect" },
    /* 0x49 */  { "paintSameRRect", 0, "rect" },
    /* 0x4a */  { "eraseSameRRect", 0, "rect" },
    /* 0x4b */  { "invertSameRRect",0, "rect" },
    /* 0x4c */  { "fillSameRRect",  0, "rect" },
    /* 0x4d */  { "reserved",       0, "reserved for Apple use" },
    /* 0x4e */  { "reserved",       0, "reserved for Apple use" },
    /* 0x4f */  { "reserved",       0, "reserved for Apple use" },
    /* 0x50 */  { "frameOval",      8, "rect" },
    /* 0x51 */  { "paintOval",      8, "rect" },
    /* 0x52 */  { "eraseOval",      8, "rect" },
    /* 0x53 */  { "invertOval",     8, "rect" },
    /* 0x54 */  { "fillOval",       8, "rect" },
    /* 0x55 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x56 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x57 */  { "reserved",       8, "reserved for Apple use" },
    /* 0x58 */  { "frameSameOval",  0, "rect" },
    /* 0x59 */  { "paintSameOval",  0, "rect" },
    /* 0x5a */  { "eraseSameOval",  0, "rect" },
    /* 0x5b */  { "invertSameOval", 0, "rect" },
    /* 0x5c */  { "fillSameOval",   0, "rect" },
    /* 0x5d */  { "reserved",       0, "reserved for Apple use" },
    /* 0x5e */  { "reserved",       0, "reserved for Apple use" },
    /* 0x5f */  { "reserved",       0, "reserved for Apple use" },
    /* 0x60 */  { "frameArc",      12, "rect, startAngle, arcAngle" },
    /* 0x61 */  { "paintArc",      12, "rect, startAngle, arcAngle" },
    /* 0x62 */  { "eraseArc",      12, "rect, startAngle, arcAngle" },
    /* 0x63 */  { "invertArc",     12, "rect, startAngle, arcAngle" },
    /* 0x64 */  { "fillArc",       12, "rect, startAngle, arcAngle" },
    /* 0x65 */  { "reserved",      12, "reserved for Apple use" },
    /* 0x66 */  { "reserved",      12, "reserved for Apple use" },
    /* 0x67 */  { "reserved",      12, "reserved for Apple use" },
    /* 0x68 */  { "frameSameArc",   4, "rect, startAngle, arcAngle" },
    /* 0x69 */  { "paintSameArc",   4, "rect, startAngle, arcAngle" },
    /* 0x6a */  { "eraseSameArc",   4, "rect, startAngle, arcAngle" },
    /* 0x6b */  { "invertSameArc",  4, "rect, startAngle, arcAngle" },
    /* 0x6c */  { "fillSameArc",    4, "rect, startAngle, arcAngle" },
    /* 0x6d */  { "reserved",       4, "reserved for Apple use" },
    /* 0x6e */  { "reserved",       4, "reserved for Apple use" },
    /* 0x6f */  { "reserved",       4, "reserved for Apple use" },
    /* 0x70 */  { "framePoly",      0, "poly" },
    /* 0x71 */  { "paintPoly",      0, "poly" },
    /* 0x72 */  { "erasePoly",      0, "poly" },
    /* 0x73 */  { "invertPoly",     0, "poly" },
    /* 0x74 */  { "fillPoly",       0, "poly" },
    /* 0x75 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x76 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x77 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x78 */  { "frameSamePoly",  0, "poly (NYI)" },
    /* 0x79 */  { "paintSamePoly",  0, "poly (NYI)" },
    /* 0x7a */  { "eraseSamePoly",  0, "poly (NYI)" },
    /* 0x7b */  { "invertSamePoly", 0, "poly (NYI)" },
    /* 0x7c */  { "fillSamePoly",   0, "poly (NYI)" },
    /* 0x7d */  { "reserved",       0, "reserved for Apple use" },
    /* 0x7e */  { "reserved",       0, "reserved for Apple use" },
    /* 0x7f */  { "reserved",       0, "reserved for Apple use" },
    /* 0x80 */  { "frameRgn",       0, "region" },
    /* 0x81 */  { "paintRgn",       0, "region" },
    /* 0x82 */  { "eraseRgn",       0, "region" },
    /* 0x83 */  { "invertRgn",      0, "region" },
    /* 0x84 */  { "fillRgn",        0, "region" },
    /* 0x85 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x86 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x87 */  { "reserved",       0, "reserved for Apple use" },
    /* 0x88 */  { "frameSameRgn",   0, "region (NYI)" },
    /* 0x89 */  { "paintSameRgn",   0, "region (NYI)" },
    /* 0x8a */  { "eraseSameRgn",   0, "region (NYI)" },
    /* 0x8b */  { "invertSameRgn",  0, "region (NYI)" },
    /* 0x8c */  { "fillSameRgn",    0, "region (NYI)" },
    /* 0x8d */  { "reserved",       0, "reserved for Apple use" },
    /* 0x8e */  { "reserved",       0, "reserved for Apple use" },
    /* 0x8f */  { "reserved",       0, "reserved for Apple use" },
    /* 0x90 */  { "BitsRect",       0, "copybits, rect clipped" },
    /* 0x91 */  { "BitsRgn",        0, "copybits, rgn clipped" },
    /* 0x92 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x93 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x94 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x95 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x96 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x97 */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x98 */  { "PackBitsRect",   0, "packed copybits, rect clipped" },
    /* 0x99 */  { "PackBitsRgn",    0, "packed copybits, rgn clipped" },
    /* 0x9a */  { "Opcode_9A",      0, "the mysterious opcode 9A" },
    /* 0x9b */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x9c */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x9d */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x9e */  { "reserved",      -1, "reserved for Apple use" },
    /* 0x9f */  { "reserved",      -1, "reserved for Apple use" },
    /* 0xa0 */  { "ShortComment",   2, "kind (word)" },
    /* 0xa1 */  { "LongComment",    0, "kind (word), size (word), data" }
  };
