/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  Image Compression/Decompression Methods.
*/
#ifndef _MAGICK_BLOB_H
#define _MAGICK_BLOB_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "magick/image.h"

/*
  Define declarations.
*/
#define MinBlobExtent  32768L

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedBlobMode,
  ReadBlobMode,
  ReadBinaryBlobMode,
  WriteBlobMode,
  WriteBinaryBlobMode,
  IOBinaryBlobMode
} BlobMode;

typedef enum
{
  ReadMode,
  WriteMode,
  IOMode
} MapMode;

typedef enum
{
  UndefinedStream,
  FileStream,
  StandardStream,
  PipeStream,
  ZipStream,
  BZipStream,
  FifoStream,
  BlobStream
} StreamType;

/*
  Typedef declarations.
*/
typedef int
  *(*BlobFifo)(const Image *,const void *,const size_t);

typedef struct _BlobInfo
{
  size_t
    length,
    extent,
    quantum;

  unsigned int
    mapped,
    eof;

  magick_off_t
    offset,
    size;

  unsigned int
    exempt,
    status,
    temporary;

  StreamType
    type;

  FILE
    *file;

  StreamHandler
    stream;

  unsigned char
    *data;

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

extern MagickExport magick_off_t
  GetBlobSize(const Image *image),
  SeekBlob(Image *image,const magick_off_t offset,const int whence),
  TellBlob(const Image *image);

extern MagickExport Image
  *BlobToImage(const ImageInfo *,const void *,const size_t,ExceptionInfo *),
  *PingBlob(const ImageInfo *,const void *,const size_t,ExceptionInfo *);

extern MagickExport int
  EOFBlob(const Image *),
  ReadBlobByte(Image *),
  SyncBlob(Image *);

extern MagickExport size_t
  ReadBlob(Image *,const size_t,void *),
  ReadBlobZC(Image *,const size_t,void **),
  WriteBlob(Image *,const size_t,const void *),
  WriteBlobByte(Image *,const unsigned long),
  WriteBlobLSBLong(Image *,const unsigned long),
  WriteBlobLSBShort(Image *,const unsigned long),
  WriteBlobMSBLong(Image *,const unsigned long),
  WriteBlobMSBShort(Image *,const unsigned long),
  WriteBlobString(Image *,const char *);

extern MagickExport StreamType
  GetBlobStreamType(const Image *);

extern MagickExport unsigned char
  *GetBlobStreamData(const Image *);

extern MagickExport unsigned int
  BlobToFile(const char *,const void *,const size_t,ExceptionInfo *),
  ImageToFile(Image *,const char *,ExceptionInfo *),
  OpenBlob(const ImageInfo *,Image *,const BlobMode,ExceptionInfo *),
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
  *GetConfigureBlob(const char *,char *,size_t *,ExceptionInfo *),
  *ImageToBlob(const ImageInfo *,Image *,size_t *,ExceptionInfo *),
  *MapBlob(int,const MapMode,magick_off_t,size_t),
  MSBOrderLong(unsigned char *,const size_t),
  MSBOrderShort(unsigned char *,const size_t);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
