/*
  Image Compression/Decompression Methods.
*/
#ifndef _BLOBS_H
#define _BLOBS_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Blob methods.
*/
extern Export char
  *ImageToBlob(const ImageInfo *,Image *,unsigned long *);

extern Export Image
  *BlobToImage(const ImageInfo *,const char *,const unsigned long);

extern Export int
  SeekBlob(Image *,const long,const unsigned long),
  TellBlob(Image *image);

extern Export unsigned long
  ReadBlob(Image *,const unsigned long,char *),
  WriteBlob(Image *,const unsigned long,const char *);

extern Export void
  GetBlobInfo(BlobInfo *);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
