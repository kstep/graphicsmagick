/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Image Compression/Decompression Methods.
*/
#ifndef _MAGICK_COMPRESS_H
#define _MAGICK_COMPRESS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Typedef declaration.
*/
typedef struct _Ascii85Info
{
  long
    offset,
    line_break;

  unsigned char
    buffer[10];
} Ascii85Info;

/*
  TODO: Clean up the interface between BLOB write functions,
  compression functions, and encoding functions so they
  may be hooked into/stacked on top of each other. Most are
  (or can be changed to be) stream based.
*/
typedef unsigned int
  (*WriteByteHook)(Image *, const magick_uint8_t, void *);

/*
  Commonly used byte writer hooks.
*/
extern MagickExport unsigned int 
  Ascii85WriteByteHook(Image *image, const magick_uint8_t, void *),
  BlobWriteByteHook(Image *image, const magick_uint8_t, void *);

/*
  Compress methods.
*/
extern MagickExport unsigned int
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(const ImageInfo *,Image *),
  HuffmanEncode2Image(const ImageInfo *,Image *,WriteByteHook,void*),
  LZWEncodeImage(Image *,const size_t,unsigned char *),
  LZWEncode2Image(Image *,const size_t,unsigned char *,WriteByteHook,void*),
  PackbitsEncodeImage(Image *,const size_t,unsigned char *),
  PackbitsEncode2Image(Image *,const size_t,unsigned char *,WriteByteHook,void*);

extern MagickExport void
  Ascii85Encode(Image *,const unsigned long),
  Ascii85Flush(Image *),
  Ascii85Initialize(Image *);

extern MagickExport const char
  *CompressionTypeToString(const CompressionType compression_type);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
