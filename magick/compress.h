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
  Compress methods.
*/
extern MagickExport unsigned int
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(const ImageInfo *,Image *),
  LZWEncodeImage(Image *,const size_t,unsigned char *),
  PackbitsEncodeImage(Image *,const size_t,unsigned char *);

extern MagickExport void
  Ascii85Encode(Image *,const unsigned long),
  Ascii85Flush(Image *),
  Ascii85Initialize(Image *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
