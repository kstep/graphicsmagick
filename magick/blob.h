/*
  Image Compression/Decompression Methods.
*/
#ifndef _BLOBS_H
#define _BLOBS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#define MaxBlobExtent  65535

/*
  Blob methods.
*/
extern Export char
  *GetStringBlob(Image *,char *),
  *ImageToBlob(const ImageInfo *,Image *,unsigned long *);

extern Export Image
  *BlobToImage(const ImageInfo *,const char *,const unsigned long);

extern Export int
  EOFBlob(const Image *),
  FlushBlob(const Image *),
  SeekBlob(Image *,const long,const int),
  TellBlob(const Image *image);

extern Export unsigned int
  OpenBlob(const ImageInfo *,Image *,const char *);

extern Export unsigned long
  ReadBlob(Image *,const unsigned long,char *),
  WriteBlob(Image *,const unsigned long,const char *);

extern Export void
  CloseBlob(Image *),
  GetBlobInfo(BlobInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
