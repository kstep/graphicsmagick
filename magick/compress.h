/*
  Compress utility routines.
*/
extern Export unsigned char
  *PICTDecodeImage(Image *,int,int);

extern Export unsigned int
  BMPDecodeImage(FILE *,const unsigned int,const unsigned int,
    const unsigned int,unsigned char *),
  BMPEncodeImage(const unsigned char *,const unsigned int,const unsigned int,
    unsigned char *),
  GIFDecodeImage(Image *),
  GIFEncodeImage(Image *,const unsigned int),
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(ImageInfo *,Image *),
  Huffman2DEncodeImage(ImageInfo *,Image *),
  LZWEncodeImage(FILE *,const unsigned int,unsigned char *),
  PackbitsDecodeImage(Image *,const int),
  PackbitsEncodeImage(FILE *,unsigned int,unsigned char *),
  PCDDecodeImage(Image *,unsigned char *,unsigned char *,unsigned char *),
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
