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
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
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
#include "magick.h"
#include "define.h"
#include <signal.h>

#if !defined(BlobQuantum)
#define BlobQuantum  65535
#endif

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
%  Method BlobToImage implements direct to memory image formats.  It returns
%  the blob as an image.
%
%  The format of the BlobToImage method is:
%
%      Image *BlobToImage(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method BlobToImage returns an image from the supplied blob.
%      If an error occurs NULL is returned.
%
%    o image_info: The image info..
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
  Image
    *image;

  ImageInfo
    *clone_info;

  int
    file;

  long
    count;

  MagickInfo
    *magick_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  GetExceptionInfo(exception);
  clone_info=CloneImageInfo(image_info);
  clone_info->blob->data=(unsigned char *) blob;
  clone_info->blob->offset=0;
  clone_info->blob->length=length;
  clone_info->blob->extent=length;
  SetImageInfo(clone_info,False,exception);
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (MagickInfo *) NULL)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unrecognized image format",
        clone_info->magick);
      return((Image *) NULL);
    }
  RewindBlob(clone_info->blob);
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      *clone_info->filename='\0';
      clone_info->blob->data=(unsigned char *) blob;
      clone_info->blob->length=length;
      clone_info->blob->extent=length;
      image=ReadImage(clone_info,exception);
      DestroyImageInfo(clone_info);
      if (image != (Image *) NULL)
        RewindBlob(image->blob);
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  TemporaryFilename(clone_info->filename);
  file=open(clone_info->filename,O_WRONLY | O_CREAT | O_EXCL | O_BINARY,0777);
  if (file == -1)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unable to convert blob to an image",
        clone_info->filename);
      return((Image *) NULL);
    }
  count=write(file,blob,length);
  (void) close(file);
  if ((size_t) count != length)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unable to convert blob to an image",
        clone_info->filename);
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
%  Method CloneBlobInfo makes a duplicate of the given blob info structure,
%  or if blob info is NULL, a new one.
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
    MagickError(ResourceLimitError,"Unable to clone blob info",
      "Memory allocation failed");
  if (blob_info == (BlobInfo *) NULL)
    {
      GetBlobInfo(clone_info);
      return(clone_info);
    }
  *clone_info=(*blob_info);
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
%  Method CloseBlob closes a file associated with the image.  If the
%  filename prefix is '|', the file is a pipe and is closed with PipeClose.
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
  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  CloseImagePixels(image);
  image->taint=False;
  image->blob->maximum_extent=SizeBlob(image);
  if (image->blob->data != (unsigned char *) NULL)
    {
      register Image
        *p;

      while (image->previous != (Image *) NULL)
        image=image->previous;
      image->blob->extent=image->blob->length;
      image->blob->eof=False;
      for (p=image->next; p != (Image *) NULL; p=p->next)
        RewindBlob(p->blob);
      return;
    }
  if (image->fifo != (int (*)(const Image *,const void *,const size_t)) NULL)
    {
      (void) image->fifo(image,(const void *) NULL,0);
      image->fifo=(int (*)(const Image *,const void *,const size_t)) NULL;
      return;
    }
  if (image->file == (FILE *) NULL)
    return;
  image->status=ferror(image->file);
  errno=0;
  if (image->exempt)
    return;
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  if (image->pipet)
    (void) pclose(image->file);
  else
#endif
    (void) fclose(image->file);
  image->file=(FILE *) NULL;
  if (!image->orphan)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      for ( ; image != (Image *) NULL; image=image->next)
        image->file=(FILE *) NULL;
    }
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
%  Method DestroyBlobInfo deallocates memory associated with an BlobInfo
%  structure.
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
  if (blob->mapped)
    (void) UnmapBlob(blob->data,blob->length);
  LiberateMemory((void **) &blob);
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
%  Method EOFBlob returns a non-zero value when EOF has been detected reading
%  from a blob or file.
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
  if (image->blob->data == (unsigned char *) NULL)
    return(feof(image->file));
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
%  Method FileToBlob returns the contents of a file as a blob.  It returns
%  the file as a blob and its length.
%
%  The format of the FileToBlob method is:
%
%      void *FileToBlob(const char *filename,size_t *length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  Method FileToBlob returns the contents of a file as a blob.  If
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
  int
    file;

  long
    count;

  struct stat
    attributes;

  unsigned char
    *blob;

  assert(filename != (const char *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  file=open(filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    {
#if defined(WIN32)
      blob=NTResourceToBlob(filename);
      if (blob != (unsigned char *) NULL)
        return(blob);
#endif
      ThrowException(exception,BlobWarning,"Unable to open file",filename);
      return((void *) NULL);
    }
  *length=fstat(file,&attributes) < 0 ? 0 : attributes.st_size;
  blob=(unsigned char *) AcquireMemory(*length+1);
  if (blob == (unsigned char *) NULL)
    {
      (void) close(file);
      ThrowException(exception,BlobWarning,"Unable to create blob",
        "Memory allocation failed");
      return((void *) NULL);
    }
  count=read(file,blob,*length);
  blob[*length]='\0';
  (void) close(file);
  if ((size_t) count != *length)
    {
      ThrowException(exception,BlobWarning,"Unable to read file",filename);
      return((void *) NULL);
    }
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
%  Method GetBlobInfo initializes the BlobInfo structure.
%
%  The format of the GetBlobInfo method is:
%
%      void GetBlobInfo(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void GetBlobInfo(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  memset(blob,0,sizeof(BlobInfo));
  blob->quantum=BlobQuantum;
  blob->signature=MagickSignature;
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
%  Method ImageToBlob implements direct to memory image formats.  It returns
%  the image as a blob and its length.  The magick member of the Image
%  structure determines the format of the returned blob (GIG, JPEG, PNG, etc.).
%
%  The format of the ImageToBlob method is:
%
%      void *ImageToBlob(const ImageInfo *image_info,Image *image,
%        size_t *length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  Method ImageToBlob returns a chunk of memory written in the
%      desired image format (e.g. JPEG, GIF, etc.).  If an error occurs
%      NULL is returned.
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
    filename[MaxTextExtent];

  ImageInfo
    *clone_info;

  int
    file;

  long
    count;

  MagickInfo
    *magick_info;

  struct stat
    attributes;

  unsigned char
    *blob;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  GetExceptionInfo(exception);
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(clone_info->magick,image->magick);
  magick_info=(MagickInfo *) GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (MagickInfo *) NULL)
     {
       DestroyImageInfo(clone_info);
       ThrowException(exception,BlobWarning,"No delegate for this image format",
         clone_info->magick);
       return((void *) NULL);
     }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      *image->filename='\0';
      clone_info->blob->extent=Max(*length,image->blob->quantum);
      clone_info->blob->data=(unsigned char *)
        AcquireMemory(clone_info->blob->extent);
      if (clone_info->blob->data == (unsigned char *) NULL)
        {
          DestroyImageInfo(clone_info);
          ThrowException(exception,BlobWarning,"Unable to create blob",
            "Memory allocation failed");
          return((void *) NULL);
        }
      clone_info->blob->offset=0;
      clone_info->blob->length=0;
      status=WriteImage(clone_info,image);
      if (status == False)
        {
          DestroyImageInfo(clone_info);
          ThrowException(exception,BlobWarning,"Unable to write blob",
            clone_info->magick);
          DestroyImageInfo(clone_info);
          return((void *) NULL);
        }
      DestroyImageInfo(clone_info);
      blob=image->blob->data;
      *length=image->blob->extent;
      return(blob);
    }
  /*
    Write file to disk in blob image format.
  */
  if (clone_info != (ImageInfo *) NULL)
    DestroyImageInfo(clone_info);
  *length=0;
  clone_info=CloneImageInfo(image_info);
  (void) strcpy(filename,image->filename);
  FormatString(image->filename,"%.1024s:%.1024s",image->magick,
    clone_info->unique);
  status=WriteImage(clone_info,image);
  if (status == False)
    {
      DestroyImageInfo(clone_info);
      ThrowException(exception,BlobWarning,"Unable to write blob",
        image->filename);
      return((void *) NULL);
    }
  DestroyImageInfo(clone_info);
  /*
    Read image from disk as blob.
  */
  file=open(image->filename,O_RDONLY | O_BINARY,0777);
  if (file == -1)
    {
      (void) remove(image->filename);
      (void) strcpy(image->filename,filename);
      ThrowException(exception,BlobWarning,"Unable to read file",
        image->filename);
      return((void *) NULL);
    }
  *length=fstat(file,&attributes) < 0 ? 0 : attributes.st_size;
  blob=(unsigned char *) AcquireMemory(*length);
  if (blob == (unsigned char *) NULL)
    {
      (void) remove(image->filename);
      (void) strcpy(image->filename,filename);
      ThrowException(exception,BlobWarning,"Unable to create blob",
        "Memory allocation failed");
      return((void *) NULL);
    }
  count=read(file,blob,*length);
  (void) close(file);
  (void) remove(image->filename);
  (void) strcpy(image->filename,filename);
  if ((size_t) count != *length)
    {
      ThrowException(exception,BlobWarning,"Unable to read file",filename);
      return((void *) NULL);
    }
  return(blob);
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
%  Method MapBlob creates a mapping from a file to a binary large object.
%
%  The format of the MapBlob method is:
%
%      void *MapBlob(int file,const MapMode mode,size_t *length)
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
%    o length: the length of the mapping is returned in this pointer.
%
%
*/
MagickExport void *MapBlob(int file,const MapMode mode,size_t *length)
{
#if defined(HAVE_MMAP)
  struct stat
    attributes;

  void
    *map;

  /*
    Map file.
  */
  assert(length != (size_t *) NULL);
  if (file == -1)
    return((void *) NULL);
  if (fstat(file,&attributes) == -1)
    return((void *) NULL);
  *length=attributes.st_size;
  if (*length != attributes.st_size)
    return((void *) NULL);
  switch (mode)
  {
    case ReadMode:
    default:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_READ,MAP_PRIVATE,file,0);
      break;
    }
    case WriteMode:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_WRITE,MAP_SHARED,file,0);
      break;
    }
    case IOMode:
    {
      map=(void *) mmap((char *) NULL,*length,PROT_READ | PROT_WRITE,MAP_SHARED,
        file,0);
      break;
    }
  }
  if (map == (void *) MAP_FAILED)
    return((void *) NULL);
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
%      void MSBOrderLong(register char *p,const unsigned int length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderLong(register char *p,const size_t length)
{
  register char
    c,
    *q,
    *sp;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    sp=p+3;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    sp=p+1;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    p+=2;
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
%      void MSBOrderShort(register char *p,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderShort(register char *p,const size_t length)
{
  register char
    c,
    *q;

  assert(p != (char *) NULL);
  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=c;
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
%  Method OpenBlob opens a file associated with the image.  A file name of
%  '-' sets the file to stdin for type 'r' and stdout for type 'w'.  If the
%  filename suffix is '.gz' or '.Z', the image is decompressed for type 'r'
%  and compressed for type 'w'.  If the filename prefix is '|', it is piped
%  to or from a system command.
%
%  The format of the OpenBlob method is:
%
%      unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
%        const char *type)
%
%  A description of each parameter follows:
%
%    o status:  Method OpenBlob returns True if the file is successfully
%      opened otherwise False.
%
%    o image_info: The image info..
%
%    o image: The image.
%
%    o type: 'r' for reading; 'w' for writing.
%
*/
MagickExport unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
  const char *type)
{
  char
    filename[MaxTextExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(type != (char *) NULL);
  if (image_info->blob->data != (unsigned char *) NULL)
    {
      *image->blob=(*image_info->blob);
      return(True);
    }
  RewindBlob(image->blob);
  image->exempt=False;
  if (image_info->fifo !=
      (int (*)(const Image *,const void *,const size_t)) NULL)
    {
      /*
        Use stream fifo.
      */
      image->fifo=image_info->fifo;
      image->exempt=True;
    }
  if (image_info->file != (FILE *) NULL)
    {
      /*
        Use previously opened filehandle.
      */
      image->file=image_info->file;
      image->exempt=True;
      return(True);
    }
  (void) strcpy(filename,image->filename);
  p=(char *) NULL;
  if (*filename != '|')
    {
      if ((strlen(filename) > 4) &&
          (LocaleCompare(filename+strlen(filename)-4,".pgp") == 0))
        {
          /*
            Decrypt image file with PGP encryption utilities.
          */
          if (*type == 'r')
            p=GetDelegateCommand(image_info,image,"pgp",(char *) NULL);
        }
      else
        if ((strlen(filename) > 4) &&
            (LocaleCompare(filename+strlen(filename)-4,".bz2") == 0))
          {
            /*
              Uncompress/compress image file with BZIP compress utilities.
            */
            if (*type == 'r')
              p=GetDelegateCommand(image_info,image,"bzip",(char *) NULL);
            else
              p=GetDelegateCommand(image_info,image,(char *) NULL,"bzip");
          }
        else
          if ((strlen(filename) > 3) &&
              (LocaleCompare(filename+strlen(filename)-3,".gz") == 0))
            {
              /*
                Uncompress/compress image file with GNU compress utilities.
              */
              if (*type == 'r')
                p=GetDelegateCommand(image_info,image,"zip",(char *) NULL);
              else
                p=GetDelegateCommand(image_info,image,(char *) NULL,"zip");
            }
          else
            if ((strlen(filename) > 2) &&
                (LocaleCompare(filename+strlen(filename)-2,".Z") == 0))
              {
                /*
                  Uncompress/compress image file with UNIX compress utilities.
                */
                if (*type == 'r')
                  p=GetDelegateCommand(image_info,image,"compress",
                    (char *) NULL);
                else
                  p=GetDelegateCommand(image_info,image,(char *) NULL,
                    "compress");
              }
    }
  if (p != (char *) NULL)
    {
      char
        *command;

      (void) strcpy(filename,p);
      command=p;
      LiberateMemory((void **) &command);
    }
  /*
    Open image file.
  */
  image->pipet=False;
  if (LocaleCompare(filename,"-") == 0)
    {
      image->file=(*type == 'r') ? stdin : stdout;
#if defined(WIN32)
      if (strchr(type,'b') != (char *) NULL)
        _setmode(_fileno(image->file),_O_BINARY);
#endif
      image->exempt=True;
    }
  else
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
    if (*filename == '|')
      {
        char
          mode[MaxTextExtent];

        /*
          Pipe image to or from a system command.
        */
        if (*type == 'w')
          (void) signal(SIGPIPE,SIG_IGN);
        (void) strncpy(mode,type,1);
        mode[1]='\0';
        image->file=(FILE *) popen(filename+1,mode);
        image->pipet=True;
        image->exempt=True;
      }
    else
#endif
      {
        if (*type == 'w')
          {
            /*
              Form filename for multi-part images.
            */
            FormatString(filename,image->filename,image->scene);
            if (!image_info->adjoin)
              if ((image->previous != (Image *) NULL) ||
                  (image->next != (Image *) NULL))
                {
                  if ((LocaleCompare(filename,image->filename) == 0) ||
                      (strchr(filename,'%') != (char *) NULL))
                    FormatString(filename,"%.1024s.%u",image->filename,
                      image->scene);
                  if (image->next != (Image *) NULL)
                    (void) strcpy(image->next->magick,image->magick);
                }
            (void) strcpy(image->filename,filename);
          }
#if defined(macintosh)
        if (*type == 'w')
          SetApplicationType(filename,image_info->magick,'8BIM');
#endif
        image->file=(FILE *) fopen(filename,type);
        if ((image->file != (FILE *) NULL) && (*type == 'r'))
          {
            MagickInfo
              *magick_info;

            magick_info=(MagickInfo *)
              GetMagickInfo(image_info->magick,&image->exception);
            if (magick_info != (MagickInfo *) NULL)
              {
                if (magick_info->blob_support)
                  {
                    /*
                      Format supports blobs-- try memory-mapped I/O.
                    */
                    image->blob->length=0;
                    image->blob->data=(unsigned char *) MapBlob(
                      fileno(image->file),ReadMode,&image->blob->length);
                    image->blob->mapped=
                      image->blob->data != (unsigned char *) NULL;
                    if (image->blob->mapped)
                      {
                        (void) fclose(image->file);
                        image->file=(FILE *) NULL;
                      }
                  }
              }
          }
        image->blob->maximum_extent=SizeBlob(image);
      }
  image->status=False;
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return((image->file != (FILE *) NULL) || (image->blob->data != NULL));
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
%  Method ReadBlob reads data from the blob or image file and returns it.  It
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
  assert(data != (void *) NULL);
  if (image->blob->data != (unsigned char *) NULL)
    {
      /*
        Read bytes from blob.
      */
      count=Min(length,image->blob->length-image->blob->offset);
      if (count > 0)
        memcpy(data,image->blob->data+image->blob->offset,count);
      image->blob->offset+=count;
      if (count < length)
        image->blob->eof=True;
      return(count);
    }
  /*
    Read bytes from a file.
  */
  count=fread(data,1,length,image->file);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  The number
%  or bytes read is returned.
%
%  The format of the ReadBlobBlock method is:
%
%      size_t ReadBlobBlock(Image *image,unsigned char *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlobBlock returns the number of bytes read.
%
%    o image: The image.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
MagickExport size_t ReadBlobBlock(Image *image,unsigned char *data)
{
  size_t
    count;

  unsigned char
    block_count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (unsigned char *) NULL);
  count=ReadBlob(image,1,&block_count);
  if (count == 0)
    return(0);
  count=ReadBlob(image,(size_t) block_count,data);
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
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  count=ReadBlob(image,1,(unsigned char *) &value);
  if (count == 0)
    return(EOF);
  return(value);
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
%  Method ReadBlobMSBLong reads a long value as a 32 bit quantity in
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
%  Method ReadBlobString reads characters from a blob or file until a newline
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

  register int
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < (MaxTextExtent-1); i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      return((char *) NULL);
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
%   R e w i n d B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RewindBlob rewinds the BlobInfo structure.
%
%  The format of the RewindBlob method is:
%
%      void RewindBlob(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void RewindBlob(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  blob->extent=blob->length;
  blob->eof=False;
  blob->mapped=False;
  blob->length=0;
  blob->offset=0;
  blob->data=(unsigned char *) NULL;
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
%  Method SeekBlob sets the offset in bytes from the beginning of a blob or
%  file and returns the resulting offset.
%
%  The format of the SeekBlob method is:
%
%      off_t SeekBlob(Image *image,const off_t offset,const int whence)
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
MagickExport off_t SeekBlob(Image *image,const off_t offset,const int whence)
{
  int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->blob->data == (unsigned char *) NULL)
    {
      status=fseek(image->file,offset,whence);
      if (status == -1)
        return(status);
      return(TellBlob(image));
    }
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
      if ((off_t) (image->blob->offset+image->blob->length+offset) < 0)
        return(-1);
      image->blob->offset=image->blob->length+offset;
      break;
    }
  }
  if (image->blob->offset <= image->blob->length)
    image->blob->eof=False;
  else
    {
      image->blob->length=image->blob->offset;
      ReacquireMemory((void **) &image->blob->data,image->blob->length);
      if (image->blob->data == (unsigned char *) NULL)
        {
          image->blob->length=0;
          return(-1);
        }
      image->blob->extent=image->blob->length;
    }
  return(image->blob->offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S e t B l o b Q u a n t u m                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetBlobQuantum set the current value of the blob quantum.  This
%  is the size in bytes to add to a blob when writing to a blob exceeds its
%  current length.
%
%  The format of the SetBlobQuantum method is:
%
%      void SetBlobQuantum(BlobInfo *blob,const size_t quantum)
%
%  A description of each parameter follows:
%
%    o blob:  A pointer to a BlobInfo structure.
%
%    o quantum: A size_t that reflects the number of bytes to increase a blob.
%
%
*/
MagickExport void SetBlobQuantum(BlobInfo *blob,const size_t quantum)
{
  assert(blob != (BlobInfo *) NULL);
  blob->quantum=quantum;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S i z e B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SizeBlob returns the current length of the image file or blob.
%
%  The format of the SizeBlob method is:
%
%      off_t SizeBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o size:  Method SizeBlob returns the current length of the image file
%      or blob.
%
%    o image: The image.
%
%
*/
MagickExport off_t SizeBlob(Image *image)
{
  struct stat
    attributes;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->blob->maximum_extent != 0)
    return(image->blob->maximum_extent);
  if (image->file == (FILE *) NULL)
    return(image->blob->length);
  SyncBlob(image);
  return(fstat(fileno(image->file),&attributes) < 0 ? 0 : attributes.st_size);
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
%  Method SyncBlob flushes the datastream if it is a file.
%
%  The format of the SyncBlob method is:
%
%      int SyncBlob(const Image *image)
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
MagickExport int SyncBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->file != (FILE *) NULL)
    return(fflush(image->file));
  return(0);
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
%  Method TellBlob obtains the current value of the blob or file position.
%
%  The format of the TellBlob method is:
%
%      off_t TellBlob(const Image *image)
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
MagickExport off_t TellBlob(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->file != (FILE *) NULL)
    return(ftell(image->file));
  return(image->blob->offset);
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
%  Method WriteBlob writes data to a blob or image file.  It returns the
%  number of bytes written.
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
  if (image->fifo != (int (*)(const Image *,const void *,const size_t)) NULL)
    return(image->fifo(image,data,length));
  if (image->blob->data == (unsigned char *) NULL)
    {
      count=fwrite((char *) data,1,length,image->file);
      return(count);
    }
  if (length > (image->blob->extent-image->blob->offset))
    {
      image->blob->extent+=length+image->blob->quantum;
      ReacquireMemory((void **) &image->blob->data,image->blob->extent);
      if (image->blob->data == (unsigned char *) NULL)
        {
          image->blob->extent=0;
          return(0);
        }
    }
  memcpy(image->blob->data+image->blob->offset,data,length);
  image->blob->offset+=length;
  if (image->blob->offset > image->blob->length)
    image->blob->length=image->blob->offset;
  return(length);
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
%      size_t WriteBlobByte(Image *image,const int value)
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
MagickExport size_t WriteBlobByte(Image *image,const int value)
{
  char
    c;

  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  c=value;
  count=WriteBlob(image,1,&c);
  return(count);
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
  buffer[0]=value;
  buffer[1]=value >> 8;
  buffer[2]=value >> 16;
  buffer[3]=value >> 24;
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
%      size_t WriteBlobLSBShort(Image *image,const unsigned int value)
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
MagickExport size_t WriteBlobLSBShort(Image *image,const unsigned int value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=value;
  buffer[1]=(value) >> 8;
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
  buffer[0]=value >> 24;
  buffer[1]=value >> 16;
  buffer[2]=value >> 8;
  buffer[3]=value;
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
%      size_t WriteBlobMSBShort(Image *image,const unsigned short value)
%
%  A description of each parameter follows.
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
MagickExport size_t WriteBlobMSBShort(Image *image,const unsigned int value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(value) >> 8;
  buffer[1]=value;
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
  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(string != (const char *) NULL);
  count=WriteBlob(image,strlen(string),string);
  return(count);
}
