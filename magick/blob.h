/*
  Image Compression/Decompression Methods.
*/
#ifndef _BLOBS_H
#define _BLOBS_H

#if !defined(BlobQuantum)
#define BlobQuantum  65535
#endif

/*
  Blob methods.
*/
extern MagickExport char
  *GetStringBlob(Image *,char *);

extern MagickExport Image
  *BlobToImage(const ImageInfo *,const void *,const size_t,ExceptionInfo *);

extern MagickExport int
  EOFBlob(const Image *),
  ReadByte(Image *),
  SyncBlob(const Image *);

extern MagickExport off_t
  SeekBlob(Image *,const off_t,const int),
  SizeBlob(Image *image),
  TellBlob(Image *image);

extern MagickExport size_t
  LSBFirstWriteLong(Image *,const unsigned long),
  LSBFirstWriteShort(Image *,const unsigned int),
  MSBFirstWriteLong(Image *,const unsigned long),
  MSBFirstWriteShort(Image *,const unsigned int),
  ReadBlob(Image *,const size_t,void *),
  ReadBlobBlock(Image *,char *),
  WriteBlob(Image *,const size_t,const void *),
  WriteByte(Image *,const int value);

extern MagickExport unsigned int
  OpenBlob(const ImageInfo *,Image *,const char *),
  UnmapBlob(void *,const size_t);

extern MagickExport unsigned short
  LSBFirstReadShort(Image *),
  MSBFirstReadShort(Image *);

extern MagickExport unsigned long
  LSBFirstReadLong(Image *),
  MSBFirstReadLong(Image *);

extern MagickExport void
  CloseBlob(Image *),
  DestroyBlobInfo(BlobInfo *),
  *ImageToBlob(const ImageInfo *,Image *,size_t *,ExceptionInfo *),
  *MapBlob(int,const MapMode,size_t *),
  MSBFirstOrderLong(char *,const size_t),
  MSBFirstOrderShort(char *,const size_t),
  GetBlobInfo(BlobInfo *),
  SetBlobQuantum(BlobInfo *,const size_t);

#endif
