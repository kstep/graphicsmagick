/*
  Image Compression/Decompression Methods.
*/
#ifndef _MAGICK_BLOB_H
#define _MAGICK_BLOB_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef struct _BlobInfo
{
  size_t
    length,
    extent,
    quantum;

  unsigned int
    mapped,
    eof;

  unsigned char
    *data;

  off_t
    offset,
    size;

  unsigned int
    exempt,
    status,
    pipet,
    temporary;

  FILE
    *file;

  int
    (*fifo)(const Image *,const void *,const size_t);

  SemaphoreInfo
    *semaphore;

  long
    reference_count;

  unsigned long
    signature;
} BlobInfo;

/*
  Blob methods.
*/
extern MagickExport BlobInfo
  *CloneBlobInfo(const BlobInfo *),
  *ReferenceBlob(BlobInfo *);

extern MagickExport char
  *ReadBlobString(Image *,char *);

extern MagickExport Image
  *BlobToImage(const ImageInfo *,const void *,const size_t,ExceptionInfo *),
  *PingBlob(const ImageInfo *,const void *,const size_t,ExceptionInfo *);

extern MagickExport int
  EOFBlob(const Image *),
  ReadBlobByte(Image *),
  SyncBlob(Image *);

extern MagickExport off_t
  GetBlobSize(const Image *image),
  SeekBlob(Image *,const off_t,const int),
  SizeBlob(const Image *image),
  TellBlob(const Image *image);

extern MagickExport size_t
  ReadBlob(Image *,const size_t,void *),
  WriteBlob(Image *,const size_t,const void *),
  WriteBlobByte(Image *,const unsigned long),
  WriteBlobLSBLong(Image *,const unsigned long),
  WriteBlobLSBShort(Image *,const unsigned long),
  WriteBlobMSBLong(Image *,const unsigned long),
  WriteBlobMSBShort(Image *,const unsigned long),
  WriteBlobString(Image *,const char *);

extern MagickExport unsigned int
  BlobToFile(const char *,const void *,const size_t,ExceptionInfo *),
  OpenBlob(const ImageInfo *,Image *,const char *,ExceptionInfo *),
  UnmapBlob(void *,const size_t);

extern MagickExport unsigned long
  ReadBlobLSBLong(Image *),
  ReadBlobMSBLong(Image *);

extern MagickExport unsigned short
  ReadBlobLSBShort(Image *),
  ReadBlobMSBShort(Image *);

extern MagickExport void
  AttachBlob(BlobInfo *,const void *,const size_t),
  CloseBlob(Image *),
  DestroyBlobInfo(BlobInfo *),
  DetachBlob(BlobInfo *),
  *FileToBlob(const char *,size_t *,ExceptionInfo *),
  GetBlobInfo(BlobInfo *),
  *ImageToBlob(const ImageInfo *,Image *,size_t *,ExceptionInfo *),
  *MapBlob(int,const MapMode,off_t,size_t),
  MSBOrderLong(unsigned char *,const size_t),
  MSBOrderShort(unsigned char *,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
