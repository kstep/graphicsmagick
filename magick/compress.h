/*
  Image Compression/Decompression Methods.
*/
#ifndef _COMPRESS_H
#define _COMPRESS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

extern Export unsigned char
  *PICTDecodeImage(const Image *,int,const int);

extern Export unsigned int
  BMPDecodeImage(FILE *,const unsigned int,const unsigned int,
    const unsigned int,unsigned char *),
  BMPEncodeImage(const unsigned char *,const unsigned int,const unsigned int,
    unsigned char *),
  GIFDecodeImage(Image *),
  GIFEncodeImage(const Image *,const unsigned int),
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(const ImageInfo *,const Image *),
  Huffman2DEncodeImage(ImageInfo *,const Image *),
  LZWEncodeImage(FILE *,const unsigned int,unsigned char *),
  PackbitsDecodeImage(Image *,const int),
  PackbitsEncodeImage(FILE *,const unsigned int,unsigned char *),
  PCDDecodeImage(const Image *,unsigned char *,unsigned char *,unsigned char *),
  PICTEncodeImage(Image *,const unsigned char *,unsigned char *),
  RunlengthDecodeImage(Image *),
  RunlengthEncodeImage(Image *),
  SUNDecodeImage(const unsigned char *,const unsigned int,const unsigned int,
    unsigned char *),
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
