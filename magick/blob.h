/*
  Image Compression/Decompression Methods.
*/
#ifndef _MAGICK_BLOB_H
#define _MAGICK_BLOB_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
  Blob methods.
*/
extern MagickExport BlobInfo
  *CloneBlobInfo(const BlobInfo *);

extern MagickExport char
  *ReadBlobString(Image *,char *);

extern MagickExport Image
  *BlobToImage(const ImageInfo *,const void *,const size_t,ExceptionInfo *);

extern MagickExport int
  EOFBlob(const Image *),
  ReadBlobByte(Image *),
  SyncBlob(Image *);

extern MagickExport off_t
  SeekBlob(Image *,const off_t,const int),
  SizeBlob(const Image *image),
  TellBlob(const Image *image);

extern MagickExport size_t
  ReadBlob(Image *,const size_t,void *),
  ReadBlobBlock(Image *,unsigned char *),
  WriteBlob(Image *,const size_t,const void *),
  WriteBlobByte(Image *,const long),
  WriteBlobLSBLong(Image *,const unsigned long),
  WriteBlobLSBShort(Image *,const unsigned long),
  WriteBlobMSBLong(Image *,const unsigned long),
  WriteBlobMSBShort(Image *,const unsigned long),
  WriteBlobString(Image *,const char *);

extern MagickExport unsigned int
  OpenBlob(const ImageInfo *,Image *,const char *,ExceptionInfo *),
  UnmapBlob(void *,const size_t);

extern MagickExport unsigned long
  ReadBlobLSBLong(Image *),
  ReadBlobMSBLong(Image *);

extern MagickExport unsigned short
  ReadBlobLSBShort(Image *),
  ReadBlobMSBShort(Image *);

extern MagickExport void
  CloseBlob(Image *),
  DestroyBlobInfo(BlobInfo *),
  DetachBlob(BlobInfo *),
  *FileToBlob(const char *,size_t *,ExceptionInfo *),
  GetBlobInfo(BlobInfo *),
  *ImageToBlob(const ImageInfo *,Image *,size_t *,ExceptionInfo *),
  *MapBlob(int,const MapMode,size_t *),
  MSBOrderLong(char *,const size_t),
  MSBOrderShort(char *,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
