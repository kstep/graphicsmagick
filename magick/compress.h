/*
  Image Compression/Decompression Methods.
*/
#ifndef _COMPRESS_H
#define _COMPRESS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern Export unsigned int
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(const ImageInfo *,const Image *),
  Huffman2DEncodeImage(ImageInfo *,const Image *),
  LZWEncodeImage(FILE *,const unsigned int,unsigned char *),
  PackbitsEncodeImage(FILE *,const unsigned int,unsigned char *),
  RunlengthDecodeImage(Image *),
  RunlengthEncodeImage(Image *),
  ZLIBEncodeImage(FILE *,const unsigned long,const unsigned int,
    unsigned char *);

extern Export void
  Ascii85Encode(const unsigned int,FILE *),
  Ascii85Flush(FILE *),
  Ascii85Initialize(void),
  SetRunlengthEncoder(RunlengthPacket *),
  SetRunlengthPackets(Image *,const unsigned long);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
