/*
  Image Compression/Decompression Methods.
*/
#ifndef _MAGICK_COMPRESS_H
#define _MAGICK_COMPRESS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

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
