/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         BBBB   L       OOO   BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   L      O   O  BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   LLLLL   OOO   BBBB                           %
%                                                                             %
%                                                                             %
%                    ImageMagick Binary Large OBjectS Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "studio.h"
#include "blob.h"
#include "cache.h"
#include "constitute.h"
#include "delegate.h"
#include "log.h"
#include "magick.h"
#include "resource.h"
#include "stream.h"
#include "utility.h"
#if defined(HasZLIB)
#include "zlib.h"
#endif
#if defined(HasBZLIB)
#include "bzlib.h"
#endif

/*
  Define declarations.
*/
#define DefaultBlobQuantum  65541

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A t t a c h B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AttachBlob() attaches a blob to the BlobInfo structure.
%
%  The format of the AttachBlob method is:
%
%      void AttachBlob(BlobInfo *blob_info,const void *blob,const size_t length)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by ImageMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%
*/
MagickExport void AttachBlob(BlobInfo *blob_info,const void *blob,
  const size_t length)
{
  assert(blob_info != (BlobInfo *) NULL);
  blob_info->length=length;
  blob_info->extent=length;
  blob_info->quantum=DefaultBlobQuantum;
  blob_info->offset=0;
  blob_info->type=BlobStream;
  blob_info->file=(FILE *) NULL;
  blob_info->data=(unsigned char *) blob;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o F i l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobToFile() writes a blob to a file.  It returns False if an error occurs
%  otherwise True.
%
%  The format of the BlobToFile method is:
%
%      unsigned int BlobToFile(const char *filename,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  BlobToFile returns True on success; otherwise,  it
%      returns False if an error occurs.
%
%    o filename: Write the blob to this file.
%
%    o blob: The address of a blob.
%
%    o length: This length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int BlobToFile(const char *filename,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  ExtendedSignedIntegralType
    count;

  int
    file;

  register size_t
    i;

  assert(filename != (const char *) NULL);
  assert(blob != (const void *) NULL);
  file=open(filename,O_WRONLY | O_CREAT | O_BINARY | O_EXCL,0777);
  if (file == -1)
    file=open(filename,O_WRONLY | O_CREAT | O_BINARY,0777);
  if (file == -1)
    {
      ThrowException(exception,BlobError,"UnableToWriteBlob",filename);
      return(False);
    }
  count=0;
  for (i=0; i < length; i+=count)
  {
    count=write(file,(char *) blob+i,length-i);
    if (count <= 0)
      break;
  }
  (void) close(file);
  return(i == length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobToImage() implements direct to memory image formats.  It returns the
%  blob as an image.
%
%  The format of the BlobToImage method is:
%
%      Image *BlobToImage(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by ImageMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *BlobToImage(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  Image
    *image;

  ImageInfo
    *clone_info;

  unsigned int
    status;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  if ((blob == (const void *) NULL) || (length == 0))
    {
      ThrowException(exception,BlobError,"ZeroLengthBlobNotPermitted",
        image_info->magick);
      return((Image *) NULL);
    }
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) blob;
  clone_info->length=length;
  (void) SetImageInfo(clone_info,False,exception);
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      (void) strncpy(clone_info->filename,image_info->filename,MaxTextExtent-1);
      (void) strncpy(clone_info->magick,image_info->magick,MaxTextExtent-1);
      image=ReadImage(clone_info,exception);
      if (image != (Image *) NULL)
        DetachBlob(image->blob);
      DestroyImageInfo(clone_info);
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  clone_info->blob=(void *) NULL;
  clone_info->length=0;
  TemporaryFilename(clone_info->filename);
  status=BlobToFile(clone_info->filename,blob,length,exception);
  if (status == False)
    {
      DestroyImageInfo(clone_info);
      return((Image *) NULL);
    }
  image=ReadImage(clone_info,exception);
  (void) remove(clone_info->filename);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e B l o b I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneBlobInfo() makes a duplicate of the given blob info structure, or if
%  blob info is NULL, a new one.
%
%  The format of the CloneBlobInfo method is:
%
%      BlobInfo *CloneBlobInfo(const BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneBlobInfo returns a duplicate of the given
%      blob info, or if blob info is NULL a new one.
%
%    o quantize_info: a structure of type info.
%
%
*/
MagickExport BlobInfo *CloneBlobInfo(const BlobInfo *blob_info)
{
  BlobInfo
    *clone_info;

  clone_info=(BlobInfo *) AcquireMemory(sizeof(BlobInfo));
  if (clone_info == (BlobInfo *) NULL)
    MagickFatalError(ResourceLimitFatalError,"MemoryAllocationFailed",
      "UnableToCloneBlobInfo");
  GetBlobInfo(clone_info);
  if (blob_info == (BlobInfo *) NULL)
    return(clone_info);
  clone_info->length=blob_info->length;
  clone_info->extent=blob_info->extent;
  clone_info->quantum=blob_info->quantum;
  clone_info->mapped=blob_info->mapped;
  clone_info->eof=blob_info->eof;
  clone_info->offset=blob_info->offset;
  clone_info->size=blob_info->size;
  clone_info->exempt=blob_info->exempt;
  clone_info->status=blob_info->status;
  clone_info->temporary=blob_info->temporary;
  clone_info->type=blob_info->type;
  clone_info->file=blob_info->file;
  clone_info->stream=blob_info->stream;
  clone_info->data=blob_info->data;
  clone_info->reference_count=1;
  clone_info->semaphore=(SemaphoreInfo *) NULL;
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloseBlob() closes a stream associated with the image.
%
%  The format of the CloseBlob method is:
%
%      void CloseBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void CloseBlob(Image *image)
{
  int
    status;

  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  status=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      status=ferror(image->blob->file);
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      gzerror(image->blob->file,&status);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      BZ2_bzerror(image->blob->file,&status);
#endif
      break;
    }
    case FifoStream:
    case BlobStream:
      break;
  }
  errno=0;
  image->taint=False;
  image->blob->size=GetBlobSize(image);
  image->blob->eof=False;
  image->blob->status=status < 0;
  if (image->blob->exempt)
    return;
  switch (image->blob->type)
  {
    case UndefinedStream:
    case FifoStream:
      break;
    case FileStream:
    case StandardStream:
    {
      status=fclose(image->blob->file);
      break;
    }
    case PipeStream:
    {
#if !defined(vms) && !defined(macintosh)
      status=pclose(image->blob->file);
#endif
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      status=gzclose(image->blob->file);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      BZ2_bzclose(image->blob->file);
#endif
      break;
    }
    case BlobStream:
      break;
  }
  DetachBlob(image->blob);
  image->blob->status=status < 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y B l o b I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyBlobInfo() deallocates memory associated with an BlobInfo structure.
%
%  The format of the DestroyBlobInfo method is:
%
%      void DestroyBlobInfo(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void DestroyBlobInfo(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  assert(blob->signature == MagickSignature);
  AcquireSemaphoreInfo(&blob->semaphore);
  blob->reference_count--;
  if (blob->reference_count > 0)
    {
      LiberateSemaphoreInfo(&blob->semaphore);
      return;
    }
  LiberateSemaphoreInfo(&blob->semaphore);
  if (blob->mapped)
    (void) UnmapBlob(blob->data,blob->length);
  if (blob->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(&blob->semaphore);
  LiberateMemory((void **) &blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e t a c h B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DetachBlob() detaches a blob from the BlobInfo structure.
%
%  The format of the DetachBlob method is:
%
%      void DetachBlob(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void DetachBlob(BlobInfo *blob_info)
{
  assert(blob_info != (BlobInfo *) NULL);
  if (blob_info->mapped)
    (void) UnmapBlob(blob_info->data,blob_info->length);
  blob_info->mapped=False;
  blob_info->length=0;
  blob_info->offset=0;
  blob_info->eof=False;
  blob_info->exempt=False;
  blob_info->type=UndefinedStream;
  blob_info->file=(FILE *) NULL;
  blob_info->data=(unsigned char *) NULL;
  blob_info->stream=(StreamHandler) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  E O F B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EOFBlob() returns a non-zero value when EOF has been detected reading from
%  a blob or file.
%
%  The format of the EOFBlob method is:
%
%      int EOFBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method EOFBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The image.
%
%
*/
MagickExport int EOFBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      image->blob->eof=feof(image->blob->file);
      break;
    }
    case ZipStream:
    {
      image->blob->eof=False;
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      int
        status;

      (void) BZ2_bzerror(image->blob->file,&status);
      image->blob->eof=status == BZ_UNEXPECTED_EOF;
#endif
      break;
    }
    case FifoStream:
    {
      image->blob->eof=False;
      break;
    }
    case BlobStream:
      break;
  }
  return(image->blob->eof);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i l e T o B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FileToBlob() returns the contents of a file as a blob.  It returns the
%  file as a blob and its length.  If an error occurs, NULL is returned.
%
%  The format of the FileToBlob method is:
%
%      void *FileToBlob(const char *filename,size_t *length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  FileToBlob() returns the contents of a file as a blob.  If
%      an error occurs NULL is returned.
%
%    o filename: The filename.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *FileToBlob(const char *filename,size_t *length,
  ExceptionInfo *exception)
{
  ExtendedSignedIntegralType
    offset;

  int
    file;

  unsigned char
    *blob;

  void
    *map;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  file=open(filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    {
      ThrowException(exception,BlobError,"UnableToOpenFile",filename);
      return((void *) NULL);
    }
  offset=MagickSeek(file,0,SEEK_END);
  if ((offset < 0) || (offset != (size_t) offset))
    {
      (void) close(file);
      ThrowException(exception,BlobError,"MemoryAllocationFailed",
        "UnableToCreateBlob");
      return((void *) NULL);
    }
  *length=(size_t) offset;
  blob=(unsigned char *) AcquireMemory(*length+1);
  if (blob == (unsigned char *) NULL)
    {
      (void) close(file);
      ThrowException(exception,BlobError,"MemoryAllocationFailed",
        "UnableToCreateBlob");
      return((void *) NULL);
    }
  map=MapBlob(file,ReadMode,0,*length);
  if (map != (void *) NULL)
    {
      (void) memcpy(blob,map,*length);
      UnmapBlob(map,*length);
    }
  else
    {
      ExtendedSignedIntegralType
        count;

      register size_t
        i;

      (void) MagickSeek(file,0,SEEK_SET);
      count=0;
      for (i=0; i < *length; i+=count)
      {
        count=read(file,blob+i,*length-i);
        if (count <= 0)
          break;
      }
      if (i < *length)
        {
          (void) close(file);
          LiberateMemory((void **) &blob);
          ThrowException(exception,BlobError,"UnableToReadBlob",filename);
          return((void *) NULL);
        }
    }
  blob[*length]='\0';
  (void) close(file);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobInfo() initializes the BlobInfo structure.
%
%  The format of the GetBlobInfo method is:
%
%      void GetBlobInfo(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void GetBlobInfo(BlobInfo *blob_info)
{
  assert(blob_info != (BlobInfo *) NULL);
  (void) memset(blob_info,0,sizeof(BlobInfo));
  blob_info->quantum=DefaultBlobQuantum;
  blob_info->reference_count=1;
  blob_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t B l o b S i z e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobSize() returns the current length of the image file or blob.
%
%  The format of the GetBlobSize method is:
%
%      ExtendedSignedIntegralType GetBlobSize(const Image *image)
%
%  A description of each parameter follows:
%
%    o size:  Method GetBlobSize returns the current length of the image file
%      or blob.
%
%    o image: The image.
%
%
*/
MagickExport ExtendedSignedIntegralType GetBlobSize(const Image *image)
{
  struct stat
    attributes;

  ExtendedSignedIntegralType
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  if (image->blob->type == UndefinedStream)
    return(image->blob->size);
  offset=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    {
      offset=fstat(fileno(image->blob->file),&attributes) < 0 ? 0 :
        attributes.st_size;
      break;
    }
    case StandardStream:
    case PipeStream:
      break;
    case ZipStream:
    {
#if defined(HasZLIB)
      offset=stat(image->filename,&attributes) < 0 ? 0 : attributes.st_size;
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      offset=stat(image->filename,&attributes) < 0 ? 0 : attributes.st_size;
#endif
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
    {
      offset=image->blob->length;
      break;
    }
  }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b S t r e a m D a t a                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobStreamData() returns the stream data for the image.
%
%  The format of the GetBlobStreamData method is:
%
%      unsigned char *GetBlobStreamData(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned char *GetBlobStreamData(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  return(image->blob->data);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b S t r e a m T y p e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobStreamType() returns the stream type for the image.
%
%  The format of the GetBlobStreamType method is:
%
%      StreamType GetBlobStreamType(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport StreamType GetBlobStreamType(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  return(image->blob->type);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t C o n f i g u r e B l o b                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetConfigureBlob() returns the specified configure file as a blob.
%
%  The format of the GetConfigureBlob method is:
%
%      void *GetConfigureBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The configure file name.
%
%    o path: return the full path information of the configure file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if !defined(UseInstalledImageMagick) && defined(POSIX)
static void ChopPathComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path);
  if (*p == *DirectorySeparator)
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == *DirectorySeparator)
      {
        *p='\0';
        count++;
      }
}
#endif

MagickExport void *GetConfigureBlob(const char *filename,char *path,
  size_t *length,ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Searching for configure file \"%s\" ...",filename);
#if defined(UseInstalledImageMagick)
#if defined(WIN32)
  {
    char
      *key_value;

    /*
      Locate file via registry key.
    */
    key_value=NTRegistryKeyLookup("ConfigurePath");
    if (key_value != (char *) NULL)
      {
        FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
          filename);
        if (!IsAccessible(path))
          ThrowException(exception,ConfigureError,"UnableToAccessConfigureFile",
            path);
        return(FileToBlob(path,length,exception));
      }
  }
#endif
#if defined(MagickLibPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  if (!IsAccessible(path))
    ThrowException(exception,ConfigureError,"UnableToAccessConfigureFile",path);
  return(FileToBlob(path,length,exception));
#endif
#else
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];

      /*
        Search based on executable directory if directory is known.
      */
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,"%.1024s/lib/%s/%.1024s",prefix,MagickLibSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      /*
        Search MAGICK_HOME.
      */
#if defined(POSIX)
      FormatString(path,"%.1024s/lib/%s/%.1024s",getenv("MAGICK_HOME"),
        MagickLibSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  if (getenv("HOME") != (char *) NULL)
    {
      /*
        Search $HOME/.magick.
      */
      FormatString(path,"%.1024s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,filename);
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  if (IsAccessible(path))
    return(FileToBlob(path,length,exception));
#if defined(WIN32)
  return(NTResourceToBlob(filename));
#endif
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessConfigureFile",path);
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t M o d u l e B l o b                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetModuleBlob() returns the specified module file as a blob.
%
%  The format of the GetModuleBlob method is:
%
%      void *GetModuleBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The module file name.
%
%    o path: return the full path information of the module file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *GetModuleBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Searching for module file \"%s\" ...",filename);
#if defined(UseInstalledImageMagick)
#if defined(WIN32)
  {
    char
      *key_value;

    /*
      Locate path via registry key.
    */
    key_value=NTRegistryKeyLookup("ModulesPath");
    if (key_value != (char *) NULL)
      {
        FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
          filename);
        if (!IsAccessible(path))
          ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",
            path);
        return(FileToBlob(path,length,exception));
      }
  }
#endif
#if defined(MagickModulesPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickModulesPath,filename);
  if (!IsAccessible(path))
    ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",path);
  return(FileToBlob(path,length,exception));
#endif
#else
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];

      /*
        Search based on executable directory if directory is known.
      */
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,"%.1024s/lib/%s/modules/coders/%.1024s",prefix,
        MagickLibSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  if (getenv("MAGICK_HOME") != (char *) NULL)
    {
      /*
        Search MAGICK_HOME.
      */
#if defined(POSIX)
      FormatString(path,"%.1024s/lib/%s/%.1024s",
        getenv("MAGICK_HOME"),MagickModulesSubdir,filename);
#else
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_HOME"),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  if (getenv("HOME") != (char *) NULL)
    {
      /*
        Search $HOME/.magick.
      */
      FormatString(path,"%.1024s%s%s%.1024s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator,filename);
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  if (IsAccessible(path))
    return(FileToBlob(path,length,exception));
#if defined(WIN32)
  return(NTResourceToBlob(path));
#endif
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessModuleFile",path);
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t T y p e B l o b                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetTypeBlob() returns the specified font file as a blob.
%
%  The format of the GetTypeBlob method is:
%
%      void *GetTypeBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The font file name.
%
%    o path: return the full path information of the font file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *GetTypeBlob(const char *filename,char *path,size_t *length,
  ExceptionInfo *exception)
{
  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  (void) strncpy(path,filename,MaxTextExtent-1);
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
    "Searching for type file \"%s\" ...",filename);
  if (getenv("MAGICK_FONT_PATH") != (char *) NULL)
    {
      /*
        Search MAGICK_FONT_PATH.
      */
      FormatString(path,"%.1024s%s%.1024s",getenv("MAGICK_FONT_PATH"),
        DirectorySeparator,filename);
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
#if defined(UseInstalledImageMagick)
#if defined(WIN32)
  {
    char
      *key_value;

    /*
      Locate file via registry key.
    */
    key_value=NTRegistryKeyLookup("ConfigurePath");
    if (key_value != (char *) NULL)
      {
        FormatString(path,"%.1024s%s%.1024s",key_value,DirectorySeparator,
          filename);
        if (!IsAccessible(path))
          ThrowException(exception,ConfigureError,"UnableToAccessFontFile",
            path);
        return(FileToBlob(path,length,exception));
      }
  }
#endif
#if defined(MagickLibPath)
  /*
    Search hard coded paths.
  */
  FormatString(path,"%.1024s%.1024s",MagickLibPath,filename);
  if (!IsAccessible(path))
    ThrowException(exception,ConfigureError,"UnableToAccessFontFile",path);
  return(FileToBlob(path,length,exception));
#endif
#else
  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];

      /*
        Search based on executable directory if directory is known.
      */
      (void) strncpy(prefix,SetClientPath((char *) NULL),MaxTextExtent-1);
      ChopPathComponents(prefix,1);
      FormatString(path,"%.1024s/lib/%s/%.1024s",prefix,MagickLibSubdir,
        filename);
#else
      FormatString(path,"%.1024s%s%.1024s",SetClientPath((char *) NULL),
        DirectorySeparator,filename);
#endif
      if (IsAccessible(path))
        return(FileToBlob(path,length,exception));
    }
  /*
    Search current directory.
  */
  if (IsAccessible(path))
    return(FileToBlob(path,length,exception));
#if defined(WIN32)
  return(NTResourceToBlob(filename));
#endif
#endif
  ThrowException(exception,ConfigureError,"UnableToAccessFontFile",path);
  return((void *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o B l o b                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageToBlob() implements direct to memory image formats.  It returns the
%  image as a blob and its length.  The magick member of the Image structure
%  determines the format of the returned blob(GIG, JPEG,  PNG, etc.)
%
%  The format of the ImageToBlob method is:
%
%      void *ImageToBlob(const ImageInfo *image_info,Image *image,
%        size_t *length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o image: The image.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *ImageToBlob(const ImageInfo *image_info,Image *image,
  size_t *length,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    unique[MaxTextExtent];

  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  unsigned char
    *blob;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  clone_info=CloneImageInfo(image_info);
  (void) strncpy(clone_info->magick,image->magick,MaxTextExtent-1);
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
     {
       DestroyImageInfo(clone_info);
       return((void *) NULL);
     }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      clone_info->blob=(void *) AcquireMemory(65535L);
      if (clone_info->blob == (void *) NULL)
        {
          ThrowException(exception,BlobError,"MemoryAllocationFailed",
            "UnableToCreateBlob");
          DestroyImageInfo(clone_info);
          return((void *) NULL);
        }
      clone_info->length=0;
      image->blob->exempt=True;
      *image->filename='\0';
      status=WriteImage(clone_info,image);
      if (status == False)
        {
          ThrowException(exception,BlobError,"UnableToWriteBlob",
            clone_info->magick);
          LiberateMemory((void **) &image->blob->data);
          DestroyImageInfo(clone_info);
          return((void *) NULL);
        }
      ReacquireMemory((void **) &image->blob->data,image->blob->length+1);
      blob=image->blob->data;
      *length=image->blob->length;
      DetachBlob(image->blob);
      DestroyImageInfo(clone_info);
      return(blob);
    }
  /*
    Write file to disk in blob image format.
  */
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  TemporaryFilename(unique);
  FormatString(image->filename,"%.1024s:%.1024s",image->magick,unique);
  status=WriteImage(clone_info,image);
  DestroyImageInfo(clone_info);
  if (status == False)
    {
      ThrowException(exception,BlobError,"UnableToWriteBlob",image->filename);
      return((void *) NULL);
    }
  /*
    Read image from disk as blob.
  */
  blob=(unsigned char *) FileToBlob(image->filename,length,exception);
  (void) remove(image->filename);
  (void) strncpy(image->filename,filename,MaxTextExtent-1);
  if (blob == (unsigned char ) NULL)
    {
      ThrowException(exception,BlobError,"UnableToReadFile",filename);
      return((void *) NULL);
    }
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o F i l e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageToFile() writes an image to a file.  It returns False if an error
%  occurs otherwise True.
%
%  The format of the ImageToFile method is:
%
%      unsigned int ImageToFile(Image *image,const char *filename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  ImageToFile returns True on success; otherwise,  it
%      returns False if an error occurs.
%
%    o image: The image.
%
%    o filename: Write the image to this file.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ImageToFile(Image *image,const char *filename,
  ExceptionInfo *exception)
{
#define MaxBufferSize  65541

  char
    *buffer;

  ExtendedSignedIntegralType
    count;

  int
    file;

  register size_t
    i;

  size_t
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(filename != (const char *) NULL);
  file=open(filename,O_WRONLY | O_CREAT | O_BINARY | O_EXCL,0777);
  if (file == -1)
    file=open(filename,O_WRONLY | O_CREAT | O_BINARY,0777);
  if (file == -1)
    {
      ThrowException(exception,BlobError,"UnableToWriteBlob",filename);
      return(False);
    }
  buffer=(char *) AcquireMemory(MaxBufferSize);
  if (buffer == (char *) NULL)
    {
      ThrowException(exception,ResourceLimitError,"MemoryAllocationError",
        filename);
      (void) close(file);
      return(False);
    }
  i=0;
  while ((length=ReadBlob(image,MaxBufferSize,buffer)) != 0)
  {
    count=0;
    for (i=0; i < length; i+=count)
    {
      count=write(file,buffer+i,length-i);
      if (count <= 0)
        break;
    }
    if (i < length)
      break;
  }
  (void) close(file);
  LiberateMemory((void **) &buffer);
  return(i < length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M a p B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MapBlob() creates a mapping from a file to a binary large object.
%
%  The format of the MapBlob method is:
%
%      void *MapBlob(int file,const MapMode mode,off_t offset,size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method MapBlob returns the address of the blob as well as
%      its length in bytes.
%
%    o file: map this file descriptor.
%
%    o mode: ReadMode, WriteMode, or IOMode.
%
%    o offset: starting at this offset within the file.
%
%    o length: the length of the mapping is returned in this pointer.
%
%
*/
MagickExport void *MapBlob(int file,const MapMode mode,off_t offset,
  size_t length)
{
#if defined(HAVE_MMAP)
  void
    *map;

  /*
    Map file.
  */
  if (file == -1)
    return((void *) NULL);
  if (!AcquireMagickResource(MapResource,length))
    return((void *) NULL);
  switch (mode)
  {
    case ReadMode:
    default:
    {
      map=(void *) mmap((char *) NULL,length,PROT_READ,MAP_PRIVATE,file,offset);
      break;
    }
    case WriteMode:
    {
      map=(void *) mmap((char *) NULL,length,PROT_WRITE,MAP_SHARED,file,offset);
      break;
    }
    case IOMode:
    {
      map=(void *) mmap((char *) NULL,length,(PROT_READ | PROT_WRITE),
        MAP_SHARED,file,offset);
      break;
    }
  }
  if (map == (void *) MAP_FAILED)
    {
      LiberateMagickResource(MapResource,length);
      return((void *) NULL);
    }
  return((void *) map);
#else
  return((void *) NULL);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B O r d e r L o n g                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBOrderLong converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBOrderLong method is:
%
%      void MSBOrderLong(unsigned char *buffer,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderLong(unsigned char *buffer,const size_t length)
{
  int
    c;

  register unsigned char
    *p,
    *q;

  assert(buffer != (unsigned char *) NULL);
  q=buffer+length;
  while (buffer < q)
  {
    p=buffer+3;
    c=(*p);
    *p=(*buffer);
    *buffer++=(unsigned char) c;
    p=buffer+1;
    c=(*p);
    *p=(*buffer);
    *buffer++=(unsigned char) c;
    buffer+=2;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B O r d e r S h o r t                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBOrderShort converts a least-significant byte first buffer of
%  integers to most-significant byte first.
%
%  The format of the MSBOrderShort method is:
%
%      void MSBOrderShort(unsigned char *p,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderShort(unsigned char *p,const size_t length)
{
  int
    c;

  register unsigned char
    *q;

  assert(p != (unsigned char *) NULL);
  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=(unsigned char) c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpenBlob() opens a file associated with the image.  A file name of '-' sets
%  the file to stdin for type 'r' and stdout for type 'w'.  If the filename
%  suffix is '.gz' or '.Z', the image is decompressed for type 'r' and
%  compressed for type 'w'.  If the filename prefix is '|', it is piped to or
%  from a system command.
%
%  The format of the OpenBlob method is:
%
%      unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
%        const BlobMode mode,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  Method OpenBlob returns True if the file is successfully
%      opened otherwise False.
%
%    o image_info: The image info.
%
%    o image: The image.
%
%    o mode: The mode for opening the file.
%
*/
MagickExport unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
  const BlobMode mode,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    *type;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image_info->blob != (void *) NULL)
    {
      AttachBlob(image->blob,image_info->blob,image_info->length);
      return(True);
    }
  DetachBlob(image->blob);
  switch (mode)
  {
    default: type=(char *) "r"; break;
    case ReadBlobMode: type=(char *) "r"; break;
    case ReadBinaryBlobMode: type=(char *) "rb"; break;
    case WriteBlobMode: type=(char *) "w"; break;
    case WriteBinaryBlobMode: type=(char *) "wb"; break;
  }
  if (image_info->stream != (StreamHandler) NULL)
    {
      image->blob->stream=image_info->stream;
      image->blob->type=FifoStream;
      if (*type == 'w')
        return(True);
    }
  /*
    Open image file.
  */
  (void) strncpy(filename,image->filename,MaxTextExtent-1);
  if (LocaleCompare(filename,"-") == 0)
    {
      image->blob->file=(*type == 'r') ? stdin : stdout;
#if defined(WIN32)
      if (strchr(type,'b') != (char *) NULL)
        _setmode(_fileno(image->blob->file),_O_BINARY);
#endif
      image->blob->type=StandardStream;
      image->blob->exempt=True;
    }
  else
#if !defined(vms) && !defined(macintosh)
    if (*filename == '|')
      {
        char
          mode[MaxTextExtent];

        /*
          Pipe image to or from a system command.
        */
#if !defined(WIN32)
        if (*type == 'w')
          (void) signal(SIGPIPE,SIG_IGN);
#endif
        (void) strncpy(mode,type,1);
        mode[1]='\0';
        image->blob->file=(FILE *) popen(filename+1,mode);
        if (image->blob->file != (FILE *) NULL)
          image->blob->type=PipeStream;
      }
    else
#endif
      {
        if (*type == 'w')
          {
            char
              *p;

            /*
              Form filename for multi-part images.
            */
            (void) strncpy(filename,image->filename,MaxTextExtent-1);
            for (p=strchr(filename,'%'); p != (char *) NULL; p=strchr(p+1,'%'))
            {
              char
                *q;

              q=p+1;
              if (*q == '0')
                (void) strtol(q,&q,10);
              if (*q == 'd')
                {
                  char
                    format[MaxTextExtent];

                  (void) strncpy(format,p,MaxTextExtent-1);
                  FormatString(p,format,GetImageIndexInList(image));
                  break;
                }
            }
            if (!image_info->adjoin)
              if ((image->previous != (Image *) NULL) ||
                  (image->next != (Image *) NULL))
                {
                  if (LocaleCompare(filename,image->filename) == 0)
                    FormatString(filename,"%.1024s.%lu",image->filename,
                      GetImageIndexInList(image));
                  if (image->next != (Image *) NULL)
                    (void) strncpy(image->next->magick,image->magick,
                      MaxTextExtent-1);
                }
            (void) strncpy(image->filename,filename,MaxTextExtent-1);
#if defined(macintosh)
            SetApplicationType(filename,image_info->magick,'8BIM');
#endif
          }
#if defined(HasZLIB)
      if ((strlen(filename) > 3) &&
          ((LocaleCompare(filename+strlen(filename)-3,".gz") == 0) ||
           (LocaleCompare(filename+strlen(filename)-2,".Z") == 0)))
        {
          image->blob->file=(FILE *) gzopen(filename,type);
          if (image->blob->file != (FILE *) NULL)
            image->blob->type=ZipStream;
        }
      else
#endif
#if defined(HasBZLIB)
        if ((strlen(filename) > 4) &&
            (LocaleCompare(filename+strlen(filename)-4,".bz2") == 0))
          {
            image->blob->file=(FILE *) BZ2_bzopen(filename,type);
            if (image->blob->file != (FILE *) NULL)
              image->blob->type=BZipStream;
          }
        else
#endif
          if (image_info->file != (FILE *) NULL)
            {
              image->blob->file=image_info->file;
              image->blob->type=FileStream;
              image->blob->exempt=True;
            }
          else
            {
              image->blob->file=(FILE *) fopen(filename,type);
              if (image->blob->file != (FILE *) NULL)
                {
                  unsigned char
                    magick[MaxTextExtent];

                  image->blob->type=FileStream;
                  (void) fread(magick,MaxTextExtent,1,image->blob->file);
                  (void) rewind(image->blob->file);
#if defined(HasZLIB)
                  if ((magick[0] == 0x1F) && (magick[1] == 0x8B) &&
                      (magick[2] == 0x08))
                    {
                      (void) fclose(image->blob->file);
                      image->blob->file=(FILE *) gzopen(filename,type);
                      if (image->blob->file != (FILE *) NULL)
                        image->blob->type=ZipStream;
                     }
#endif
#if defined(HasBZLIB)
                  if (strncmp((char *) magick,"BZh",3) == 0)
                    {
                      (void) fclose(image->blob->file);
                      image->blob->file=(FILE *) BZ2_bzopen(filename,type);
                      if (image->blob->file != (FILE *) NULL)
                        image->blob->type=BZipStream;
                    }
#endif
                }
            }
        if ((image->blob->type == FileStream) && (*type == 'r'))
          {
            const MagickInfo
              *magick_info;

            struct stat
              attributes;

            magick_info=GetMagickInfo(image_info->magick,&image->exception);
            if ((magick_info != (const MagickInfo *) NULL) &&
                magick_info->blob_support)
              if ((fstat(fileno(image->blob->file),&attributes) >= 0) &&
                  (attributes.st_size > MinBlobExtent) &&
                  (attributes.st_size == (size_t) attributes.st_size))
                {
                  size_t
                    length;

                  void
                    *blob;

                  length=(size_t) attributes.st_size;
                  blob=MapBlob(fileno(image->blob->file),ReadMode,0,length);
                  if (blob != (void *) NULL)
                    {
                      /*
                        Format supports blobs-- use memory-mapped I/O.
                      */
                      if (image_info->file != (FILE *) NULL)
                        image->blob->exempt=False;
                      else
                        (void) fclose(image->blob->file);
                      AttachBlob(image->blob,blob,length);
                      image->blob->mapped=True;
                    }
              }
          }
      }
  image->blob->status=False;
  if (image->blob->type != UndefinedStream)
    image->blob->size=GetBlobSize(image);
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return(image->blob->type != UndefinedStream);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PingBlob() returns all the attributes of an image or image sequence except
%  for the pixels.  It is much faster and consumes far less memory than
%  BlobToImage().  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%
%  The format of the PingBlob method is:
%
%      Image *PingBlob(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by ImageMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static unsigned int PingStream(const Image *image,const void *pixels,
  const size_t columns)
{
  return(True);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport Image *PingBlob(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  SetExceptionInfo(exception,UndefinedException);
  if ((blob == (const void *) NULL) || (length == 0))
    {
      ThrowException(exception,BlobError,"UnrecognizedImageFormat",
        image_info->magick);
      return((Image *) NULL);
    }
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) blob;
  clone_info->length=length;
  clone_info->ping=True;
  if (clone_info->size == (char *) NULL)
    clone_info->size=AllocateString(DefaultTileGeometry);
  image=ReadStream(clone_info,&PingStream,exception);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlob() reads data from the blob or image file and returns it.  It
%  returns the number of bytes read.
%
%  The format of the ReadBlob method is:
%
%      size_t ReadBlob(Image *image,const size_t length,void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlob returns the number of bytes read.
%
%    o image: The image.
%
%    o length:  Specifies an integer representing the number of bytes
%      to read from the file.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
MagickExport size_t ReadBlob(Image *image,const size_t length,void *data)
{
  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  assert(data != (void *) NULL);
  count=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      count=fread(data,1,length,image->blob->file);
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      count=gzread(image->blob->file,data,length);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      count=BZ2_bzread(image->blob->file,data,length);
#endif
      break;
    }
    case FifoStream:
      break;
    case BlobStream:
    {
      count=Min(length,image->blob->length-image->blob->offset);
      if (count != 0)
        (void) memcpy(data,image->blob->data+image->blob->offset,count);
      image->blob->offset+=count;
      if (count < length)
        image->blob->eof=True;
      break;
    }
  }
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b B y t e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobByte reads a single byte from the image file and returns it.
%
%  The format of the ReadBlobByte method is:
%
%      int ReadBlobByte(Image *image)
%
%  A description of each parameter follows.
%
%    o value: Method ReadBlobByte returns an integer read from the file.
%
%    o image: The image.
%
%
*/
MagickExport int ReadBlobByte(Image *image)
{
  size_t
    count;

  unsigned char
    buffer[1];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  count=ReadBlob(image,1,(unsigned char *) buffer);
  if (count == 0)
    return(EOF);
  return(*buffer);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B L o n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBLong reads a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the ReadBlobLSBLong method is:
%
%      unsigned long ReadBlobLSBLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBLong returns an unsigned long read from
%      the file.
%
%    o image: The image.
%
%
*/
MagickExport unsigned long ReadBlobLSBLong(Image *image)
{
  unsigned char
    buffer[4];

  unsigned long
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  value=ReadBlob(image,4,(unsigned char *) buffer);
  if (value == 0)
    return((unsigned long) ~0);
  value=buffer[3] << 24;
  value|=buffer[2] << 16;
  value|=buffer[1] << 8;
  value|=buffer[0];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B S h o r t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBShort reads a short value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the ReadBlobLSBShort method is:
%
%      unsigned short ReadBlobLSBShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBShort returns an unsigned short read from
%      the file.
%
%    o image: The image.
%
%
*/
MagickExport unsigned short ReadBlobLSBShort(Image *image)
{
  unsigned char
    buffer[2];

  unsigned short
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  value=ReadBlob(image,2,(unsigned char *) buffer);
  if (value == 0)
    return((unsigned short) ~0);
  value=buffer[1] << 8;
  value|=buffer[0];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B L o n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobMSBLong() reads a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the ReadBlobMSBLong method is:
%
%      unsigned long ReadBlobMSBLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBLong returns an unsigned long read from
%      the file.
%
%    o image: The image.
%
%
%
*/
MagickExport unsigned long ReadBlobMSBLong(Image *image)
{
  unsigned char
    buffer[4];

  unsigned long
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  value=ReadBlob(image,4,(unsigned char *) buffer);
  if (value == 0)
    return((unsigned long) ~0);
  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B S h o r t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBShort reads a short value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the ReadBlobMSBShort method is:
%
%      unsigned short ReadBlobMSBShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBShort returns an unsigned short read from
%      the file.
%
%    o image: The image.
%
%
*/
MagickExport unsigned short ReadBlobMSBShort(Image *image)
{
  unsigned char
    buffer[2];

  unsigned short
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  value=ReadBlob(image,2,(unsigned char *) buffer);
  if (value == 0)
    return((unsigned short) ~0);
  value=buffer[0] << 8;
  value|=buffer[1];
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d B l o b S t r i n g                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobString() reads characters from a blob or file until a newline
%  character is read or an end-of-file condition is encountered.
%
%  The format of the ReadBlobString method is:
%
%      char *ReadBlobString(Image *image,char *string)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadBlobString returns the string on success, otherwise,
%      a null is returned.
%
%    o image: The image.
%
%    o string: The address of a character buffer.
%
%
*/
MagickExport char *ReadBlobString(Image *image,char *string)
{
  int
    c;

  register long
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < (MaxTextExtent-1); i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      {
        if (i == 0)
          return((char *) NULL);
        break;
      }
    string[i]=c;
    if ((string[i] == '\n') || (string[i] == '\r'))
      break;
  }
  string[i]='\0';
  return(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e B l o b                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceBlob() increments the reference count associated with the pixel
%  blob returning a pointer to the blob.
%
%  The format of the ReferenceBlob method is:
%
%      BlobInfo ReferenceBlob(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: The blob_info.
%
%
*/
MagickExport BlobInfo *ReferenceBlob(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  assert(blob->signature == MagickSignature);
  AcquireSemaphoreInfo(&blob->semaphore);
  blob->reference_count++;
  LiberateSemaphoreInfo(&blob->semaphore);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S e e k B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SeekBlob() sets the offset in bytes from the beginning of a blob or file
%  and returns the resulting offset.
%
%  The format of the SeekBlob method is:
%
%      ExtendedSignedIntegralType SeekBlob(Image *image,
%        const ExtendedSignedIntegralType offset,const int whence)
%
%  A description of each parameter follows:
%
%    o offset:  Method SeekBlob returns the offset from the beginning
%      of the file or blob.
%
%    o image: The image.
%
%    o offset:  Specifies an integer representing the offset in bytes.
%
%    o whence:  Specifies an integer representing how the offset is
%      treated relative to the beginning of the blob as follows:
%
%        SEEK_SET  Set position equal to offset bytes.
%        SEEK_CUR  Set position to current location plus offset.
%        SEEK_END  Set position to EOF plus offset.
%
%
*/
MagickExport ExtendedSignedIntegralType SeekBlob(Image *image,
   const ExtendedSignedIntegralType offset,const int whence)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    {
      if (fseek(image->blob->file,(off_t) offset,whence) < 0)
        return(-1);
      image->blob->offset=TellBlob(image);
      break;
    }
    case StandardStream:
    case PipeStream:
    case ZipStream:
    {
#if defined(HasZLIB)
      if (gzseek(image->blob->file,(off_t) offset,whence) < 0)
        return(-1);
#endif
      image->blob->offset=TellBlob(image);
      break;
    }
    case BZipStream:
      return(-1);
    case FifoStream:
      return(-1);
    case BlobStream:
    {
      switch (whence)
      {
        case SEEK_SET:
        default:
        {
          if (offset < 0)
            return(-1);
          image->blob->offset=offset;
          break;
        }
        case SEEK_CUR:
        {
          if ((image->blob->offset+offset) < 0)
            return(-1);
          image->blob->offset+=offset;
          break;
        }
        case SEEK_END:
        {
          if ((ExtendedSignedIntegralType)
              (image->blob->offset+image->blob->length+offset) < 0)
            return(-1);
          image->blob->offset=image->blob->length+offset;
          break;
        }
      }
      if (image->blob->offset <= (ExtendedSignedIntegralType)
          image->blob->length)
        image->blob->eof=False;
      else
        if (image->blob->mapped)
          return(-1);
        else
          {
            image->blob->extent=image->blob->offset+image->blob->quantum;
            ReacquireMemory((void **) &image->blob->data,image->blob->extent+1);
            (void) SyncBlob(image);
            if (image->blob->data == (unsigned char *) NULL)
              {
                DetachBlob(image->blob);
                return(-1);
              }
          }
      break;
    }
  }
  return(image->blob->offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S y n c B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SyncBlob() flushes the datastream if it is a file or synchonizes the data
%  attributes if it is an blob.
%
%  The format of the SyncBlob method is:
%
%      int SyncBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method SyncBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The image.
%
%
*/
MagickExport int SyncBlob(Image *image)
{
  int
    status;

  register Image
    *p;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  for (p=image; p->previous != (Image *) NULL; p=p->previous);
  for ( ; p->next != (Image *) NULL; p=p->next)
    *p->blob=(*image->blob);
  status=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      status=fflush(image->blob->file);
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      status=gzflush(image->blob->file,Z_SYNC_FLUSH);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      status=BZ2_bzflush(image->blob->file);
#endif
      break;
    }
    case FifoStream:
    case BlobStream:
      break;
  }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  T e l l B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TellBlob() obtains the current value of the blob or file position.
%
%  The format of the TellBlob method is:
%
%      ExtendedSignedIntegralType TellBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o offset:  Method TellBlob returns the current value of the blob or
%      file position success; otherwise, it returns -1 and sets errno to
%      indicate the error.
%
%    o image: The image.
%
%
*/
MagickExport ExtendedSignedIntegralType TellBlob(const Image *image)
{
  ExtendedSignedIntegralType
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  offset=(-1);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    {
      offset=ftell(image->blob->file);
      break;
    }
    case StandardStream:
    case PipeStream:
    case ZipStream:
    {
#if defined(HasZLIB)
      offset=gztell(image->blob->file);
#endif
      break;
    }
    case BZipStream:
      break;
    case FifoStream:
      break;
    case BlobStream:
    {
      offset=image->blob->offset;
      break;
    }
  }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  U n m a p B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnmapBlob deallocates the binary large object previously allocated
%  with the MapBlob method.
%
%  The format of the UnmapBlob method is:
%
%      unsigned int UnmapBlob(void *map,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method UnmapBlob returns True on success; otherwise,  it
%      returns False and sets errno to indicate the error.
%
%    o map: The address  of the binary large object.
%
%    o length: The length of the binary large object.
%
%
*/
MagickExport unsigned int UnmapBlob(void *map,const size_t length)
{
#if defined(HAVE_MMAP)
  int
    status;

  status=munmap(map,length);
  LiberateMagickResource(MapResource,length);
  return(status == 0);
#else
  return(False);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteBlob() writes data to a blob or image file.  It returns the number of
%  bytes written.
%
%  The format of the WriteBlob method is:
%
%      size_t WriteBlob(Image *image,const size_t length,const void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method WriteBlob returns the number of bytes written to the
%      blob.
%
%    o image: The image.
%
%    o length:  Specifies an integer representing the number of bytes to
%      write to the file.
%
%    o data:  The address of the data to write to the blob or file.
%
%
*/
MagickExport size_t WriteBlob(Image *image,const size_t length,const void *data)
{
  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (const char *) NULL);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  count=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      count=fwrite((char *) data,1,length,image->blob->file);
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      count=gzwrite(image->blob->file,(void *) data,length);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      count=BZ2_bzwrite(image->blob->file,(void *) data,length);
#endif
      break;
    }
    case FifoStream:
    {
      count=image->blob->stream(image,data,length);
      break;
    }
    case BlobStream:
    {
      if ((image->blob->offset+length) >= image->blob->extent)
        {
          if (image->blob->mapped)
            return(0);
          image->blob->quantum<<=1;
          image->blob->extent+=length+image->blob->quantum;
          ReacquireMemory((void **) &image->blob->data,image->blob->extent+1);
          (void) SyncBlob(image);
          if (image->blob->data == (unsigned char *) NULL)
            {
              DetachBlob(image->blob);
              return(0);
            }
        }
      (void) memcpy(image->blob->data+image->blob->offset,data,length);
      image->blob->offset+=length;
      if (image->blob->offset > (ExtendedSignedIntegralType)
          image->blob->length)
        image->blob->length=image->blob->offset;
      count=length;
    }
  }
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b B y t e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobByte write an integer to a blob.  It returns the number of
%  bytes written (either 0 or 1);
%
%  The format of the WriteBlobByte method is:
%
%      size_t WriteBlobByte(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count:  Method WriteBlobByte returns the number of bytes written.
%
%    o image: The image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobByte(Image *image,const unsigned long value)
{
  unsigned char
    buffer[1];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) value;
  return(WriteBlob(image,1,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b L S B L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBLong writes a long value as a 32 bit quantity in
%  least-significant byte first order.
%
%  The format of the WriteBlobLSBLong method is:
%
%      size_t WriteBlobLSBLong(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBLong returns the number of unsigned longs
%      written.
%
%    o image: The image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBLong(Image *image,const unsigned long value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) value;
  buffer[1]=(unsigned char) (value >> 8);
  buffer[2]=(unsigned char) (value >> 16);
  buffer[3]=(unsigned char) (value >> 24);
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e B l o b L S B S h o r t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBShort writes a long value as a 16 bit quantity in
%  least-significant byte first order.
%
%  The format of the WriteBlobLSBShort method is:
%
%      size_t WriteBlobLSBShort(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBShort returns the number of unsigned longs
%      written.
%
%    o image: The image.
%
%    o value:  Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBShort(Image *image,const unsigned long value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) value;
  buffer[1]=(unsigned char) (value >> 8);
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBLong writes a long value as a 32 bit quantity in
%  most-significant byte first order.
%
%  The format of the WriteBlobMSBLong method is:
%
%      size_t WriteBlobMSBLong(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobMSBLong returns the number of unsigned longs
%      written.
%
%    o value:  Specifies the value to write.
%
%    o image: The image.
%
%
*/
MagickExport size_t WriteBlobMSBLong(Image *image,const unsigned long value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) (value >> 24);
  buffer[1]=(unsigned char) (value >> 16);
  buffer[2]=(unsigned char) (value >> 8);
  buffer[3]=(unsigned char) value;
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBShort writes a long value as a 16 bit quantity in
%  most-significant byte first order.
%
%  The format of the WriteBlobMSBShort method is:
%
%      size_t WriteBlobMSBShort(Image *image,const unsigned long value)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
MagickExport size_t WriteBlobMSBShort(Image *image,const unsigned long value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) (value >> 8);
  buffer[1]=(unsigned char) value;
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b S t r i n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobString write a string to a blob.  It returns the number of
%  characters written.
%
%  The format of the WriteBlobString method is:
%
%      size_t WriteBlobString(Image *image,const char *string)
%
%  A description of each parameter follows.
%
%    o count:  Method WriteBlobString returns the number of characters written.
%
%    o image: The image.
%
%    o string: Specifies the string to write.
%
%
*/
MagickExport size_t WriteBlobString(Image *image,const char *string)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(string != (const char *) NULL);
  return(WriteBlob(image,strlen(string),string));
}
