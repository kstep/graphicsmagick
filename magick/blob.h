/*
  Copyright (C) 2003, 2004 GraphicsMagick Group
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

/*
  Typedef declarations.
*/
typedef struct _BlobInfo BlobInfo;

/*
  Blob methods.
*/
extern MagickExport BlobInfo
  *CloneBlobInfo(const BlobInfo *blob_info),
  *ReferenceBlob(BlobInfo *blob);

extern MagickExport char
  *ReadBlobString(Image *image,char *string);

extern MagickExport magick_off_t
  GetBlobSize(const Image *image),
  SeekBlob(Image *image,const magick_off_t offset,const int whence),
  TellBlob(const Image *image);

extern MagickExport Image
  *BlobToImage(const ImageInfo *image_info,const void *blob,
    const size_t length,ExceptionInfo *exception),
  *PingBlob(const ImageInfo *image_info,const void *blob,
    const size_t length,ExceptionInfo *exception);

extern MagickExport int
  EOFBlob(const Image *image),
  GetBlobStatus(const Image *image),
  ReadBlobByte(Image *image),
  SyncBlob(Image *image);

extern MagickExport size_t
  ReadBlob(Image *image,const size_t length,void *data),
  ReadBlobZC(Image *image,const size_t length,void **data),
  WriteBlob(Image *image,const size_t length,const void *data),
  WriteBlobByte(Image *image,const unsigned long value),
  WriteBlobLSBLong(Image *image,const unsigned long value),
  WriteBlobLSBShort(Image *image,const unsigned long value),
  WriteBlobMSBLong(Image *image,const unsigned long value),
  WriteBlobMSBShort(Image *image,const unsigned long value),
  WriteBlobString(Image *image,const char *string);

extern MagickExport FILE
  *GetBlobFileHandle(const Image *image);

extern MagickExport StreamHandler
  GetBlobStreamHandler(const Image *image);

#if 0
extern MagickExport StreamType
  GetBlobStreamType(const Image *image);
#endif

extern MagickExport unsigned char
  *GetBlobStreamData(const Image *image);

extern MagickExport unsigned int
  BlobToFile(const char *filename,const void *blob,const size_t length,
    ExceptionInfo *exception),
  BlobIsSeekable(const Image *image),
  ImageToFile(Image *image,const char *filename,ExceptionInfo *exception),
  GetBlobClosable(const Image *image),
  GetBlobTemporary(const Image *image),
  OpenBlob(const ImageInfo *image_info,Image *image,const BlobMode mode,
    ExceptionInfo *exception),
  UnmapBlob(void *map,const size_t length);

extern MagickExport unsigned long
  ReadBlobLSBLong(Image *image),
  ReadBlobMSBLong(Image *image);

extern MagickExport unsigned short
  ReadBlobLSBShort(Image *image),
  ReadBlobMSBShort(Image *image);

extern MagickExport void
  AttachBlob(BlobInfo *blob_info,const void *blob,const size_t length),
  SetBlobClosable(Image *image, unsigned int closable),
  SetBlobTemporary(Image *image, unsigned int isTemporary),
  CloseBlob(Image *image),
  DestroyBlob(Image *image),
  DestroyBlobInfo(BlobInfo *blob),
  DetachBlob(BlobInfo *blob),
  *FileToBlob(const char *filename,size_t *length,ExceptionInfo *exception),
  GetBlobInfo(BlobInfo *blob),
  *GetConfigureBlob(const char *filename,char *path,size_t *length,
    ExceptionInfo *exception),
  *ImageToBlob(const ImageInfo *image_info,Image *image,size_t *length,
    ExceptionInfo *exception),
  *MapBlob(int file,const MapMode mode,magick_off_t offset,size_t length),
  MSBOrderLong(unsigned char *buffer,const size_t length),
  MSBOrderShort(unsigned char *p,const size_t length);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
