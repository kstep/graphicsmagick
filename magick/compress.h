/*
  Compress utility routines.
*/
extern Export unsigned char
  *PICTDecodeImage(Image *,int,int);

extern Export unsigned int
  BMPDecodeImage(FILE *,unsigned char *,const unsigned int,const unsigned int,
    const unsigned int),
  BMPEncodeImage(unsigned char *,unsigned char *,const unsigned int,
    const unsigned int),
  GIFDecodeImage(Image *),
  GIFEncodeImage(Image *,const unsigned int),
  HuffmanDecodeImage(Image *),
  HuffmanEncodeImage(ImageInfo *,Image *),
  Huffman2DEncodeImage(ImageInfo *,Image *),
  LZWEncodeImage(FILE *,unsigned char *,const unsigned int),
  PackbitsDecodeImage(Image *,int),
  PackbitsEncodeImage(FILE *,unsigned char *,unsigned int),
  PCDDecodeImage(Image *,unsigned char *,unsigned char *,unsigned char *),
  PICTEncodeImage(Image *,unsigned char *,unsigned char *),
  RunlengthDecodeImage(Image *),
  RunlengthEncodeImage(Image *),
  SUNDecodeImage(unsigned char *,unsigned char *,const unsigned int,
    const unsigned int),
  ZLIBEncodeImage(FILE *,unsigned char *,const unsigned long,
    const unsigned int);

extern Export void
  Ascii85Encode(const unsigned int,FILE *),
  Ascii85Flush(FILE *),
  Ascii85Initialize(void),
  SetRunlengthEncoder(RunlengthPacket *),
  SetRunlengthPackets(Image *,unsigned long);
