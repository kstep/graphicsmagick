/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     BBBB   L       OOO   BBBB   SSSSS                       %
%                     B   B  L      O   O  B   B  SS                          %
%                     BBBB   L      O   O  BBBB    SSS                        %
%                     B   B  L      O   O  B   B     SS                       %
%                     BBBB   LLLLL   OOO   BBBB   SSSSS                       %
%                                                                             %
%                                                                             %
%                    ImageMagick Binary Large OBjectS Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright 1999 E. I. du Pont de Nemours and Company                        %
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
%  E. I. du Pont de Nemours and Company be liable for any claim, damages or   %
%  other liability, whether in an action of contract, tort or otherwise,      %
%  arising from, out of or in connection with ImageMagick or the use or other %
%  dealings in ImageMagick.                                                   %
%                                                                             %
%  Except as contained in this notice, the name of the E. I. du Pont de       %
%  Nemours and Company shall not be used in advertising or otherwise to       %
%  promote the sale, use or other dealings in ImageMagick without prior       %
%  written authorization from the E. I. du Pont de Nemours and Company.       %
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
#include "defines.h"

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
%  The format of the BlobToImage routine is:
%
%      image=BlobToImage(blob,length)
%
%  A description of each parameter follows:
%
%    o image:  Method BlobToImage returns an image from the supplied blob.
%      If an error occurs NULL is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by ImageMagick.
%
%    o length: This unsigned integer reflects the length in bytes of the blob.
%
%
*/
Export Image *BlobToImage(const ImageInfo *image_info,const char *blob,
  const unsigned long length)
{
  FILE
    *file;

  Image
    *image;

  ImageInfo
    *local_info;

  MagickInfo
    *magick_info;

  local_info=CloneImageInfo(image_info);
  SetImageInfo(local_info,False);
  magick_info=(MagickInfo *) GetMagickInfo(local_info->magick);
  if (magick_info == (MagickInfo *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unrecognized image format",
        local_info->magick);
      DestroyImageInfo(local_info);
      return((Image *) NULL);
    }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      local_info->blob.data=(char *) blob;
      local_info->blob.offset=0;
      local_info->blob.length=length;
      image=ReadImage(local_info);
      DestroyImageInfo(local_info);
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  TemporaryFilename(local_info->filename);
  file=fopen(local_info->filename,"w");
  if (file == (FILE *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unable to convert blob to an image",
        local_info->filename);
      DestroyImageInfo(local_info);
      return((Image *) NULL);
    }
  (void) fwrite(blob,1,length,file);
  (void) fclose(file);
  image=ReadImage(local_info);
  (void) remove(local_info->filename);
  DestroyImageInfo(local_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloseBlob closes a file associated with the image.  If the
%  filename prefix is '|', the file is a pipe and is closed with PipeClose.
%
%  The format of the CloseBlob routine is:
%
%      CloseBlob(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
Export void CloseBlob(Image *image)
{
  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  if (image->blob.data != (char *) NULL)
    {
      image->filesize=image->blob.length;
      return;
    }
  if (image->file == (FILE *) NULL)
    return;
  (void) FlushBlob(image);
  image->status=ferror(image->file);
  (void) SeekBlob(image,0L,SEEK_END);
  image->filesize=TellBlob(image);
#if !defined(vms) && !defined(macintosh) && !defined(WIN32)
  if (image->pipe)
    (void) pclose(image->file);
  else
#endif
    if (!image->exempt)
      (void) fclose(image->file);
  image->file=(FILE *) NULL;
  if (!image->orphan)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      for ( ; image != (Image *) NULL; image=image->next)
        image->file=(FILE *) NULL;
    }
  errno=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  E O F B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EOFBlob returns a non-zero value when EOF has been detected reading
%  from a blob or file.
%
%  The format of the EOFBlob routine is:
%
%      status=EOFBlob(image)
%
%  A description of each parameter follows:
%
%    o status:  Method EOFBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export int EOFBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(feof(image->file));
  return(image->blob.offset > image->blob.length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F l u s h B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FlushBlob flushes the datastream if it is a file.
%
%  The format of the FlushBlob routine is:
%
%      status=FlushBlob(image)
%
%  A description of each parameter follows:
%
%    o status:  Method FlushBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export int FlushBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(fflush(image->file));
  return(0);
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
%  The format of the GetBlobInfo routine is:
%
%      GetBlobInfo(blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%
*/
Export void GetBlobInfo(BlobInfo *blob_info)
{
  assert(blob_info != (BlobInfo *) NULL);
  blob_info->data=(char *) NULL;
  blob_info->offset=0;
  blob_info->length=0;
  blob_info->extent=0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t S t r i n g B l o b                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetStringBlob reads characters from a blob or file until a newline
%  character is read or an end-of-file  condition is encountered.
%  from a blob or file.
%
%  The format of the GetStringBlob routine is:
%
%      status=GetStringBlob(image,string)
%
%  A description of each parameter follows:
%
%    o status:  Method GetStringBlob returns the string on success, otherwise,
%      a null is returned.
%
%    o image: The address of a structure of type Image.
%
%    o string: The address of a character buffer.
%
%
*/
Export char *GetStringBlob(Image *image,char *string)
{
  register int
    i;

  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(fgets((char *) string,MaxTextExtent,image->file));
  for (i=0; i < (MaxTextExtent-1); i++)
  {
    string[i]=ReadByte(image);
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
%   I m a g e T o B l o b                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ImageToBlob implements direct to memory image formats.  It returns
%  the image as a blob and its length.
%
%  The format of the ImageToBlob routine is:
%
%      blob=ImageToBlob(image_info,image,length)
%
%  A description of each parameter follows:
%
%    o blob:  Method ImageToBlob returns a chunk of memory written in the
%      desired image format (e.g. JPEG, GIF, etc.).  If an error occurs
%      NULL is returned.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o length: This pointer to an unsigned int is set to the length of
%      the image blob.
%
%
*/
Export char *ImageToBlob(const ImageInfo *image_info,Image *image,
  unsigned long *length)
{
  char
    *blob,
    filename[MaxTextExtent];

  FILE
    *file;

  ImageInfo
    *local_info;

  MagickInfo
    *magick_info;

  unsigned int
    status;

  local_info=CloneImageInfo(image_info);
  SetImageInfo(local_info,False);
  magick_info=(MagickInfo *) GetMagickInfo(local_info->magick);
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      local_info->blob.data=AllocateMemory(MaxTextExtent);
      local_info->blob.offset=0;
      local_info->blob.length=0;
      status=WriteImage(local_info,image);
      DestroyImageInfo(local_info);
      if (status == False)
        {
          MagickWarning(FileOpenWarning,"Unable to convert image to a blob",
            local_info->magick);
          return((char *) NULL);
        }
      *length=image->blob.length;
      return(image->blob.data);
    }
  /*
    Write file to disk in blob image format.
  */
  *length=0;
  local_info=CloneImageInfo(image_info);
  (void) strcpy(filename,image->filename);
  FormatString(image->filename,"%.1024s:%.1024s",image->magick,
    local_info->unique);
  status=WriteImage(local_info,image);
  if (status == False)
    {
      MagickWarning(FileOpenWarning,"Unable to convert image to a blob",
        image->filename);
      return((char *) NULL);
    }
  /*
    Read image from disk as blob.
  */
  file=fopen(image->filename,"rb");
  (void) remove(image->filename);
  (void) strcpy(image->filename,filename);
  DestroyImageInfo(local_info);
  if (file == (FILE *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unable to convert image to a blob",
        image->filename);
      return((char *) NULL);
    }
  (void) fseek(file,0L,SEEK_END);
  *length=ftell(file);
  (void) fseek(file,0L,SEEK_SET);
  blob=(char *) AllocateMemory(*length*sizeof(char));
  if (blob != (char *) NULL)
    (void) fread((char *) blob,1,*length,file);
  else
    MagickError(ResourceLimitWarning,"Unable to create blob",
      "Memory allocation failed");
  (void) fclose(file);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method OpenBlob open a file associated with the image.  A file name of
%  '-' sets the file to stdin for type 'r' and stdout for type 'w'.  If the
%  filename suffix is '.gz' or '.Z', the image is decompressed for type 'r'
%  and compressed for type 'w'.  If the filename prefix is '|', it is piped
%  to or from a system command.
%
%  The format of the OpenBlob routine is:
%
%      status=OpenBlob(image_info,image,type)
%
%  A description of each parameter follows:
%
%    o status:  Method OpenBlob returns True if the file is successfully
%      opened otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
%    o type: 'r' for reading; 'w' for writing.
%
*/
Export unsigned int OpenBlob(const ImageInfo *image_info,Image *image,
  const char *type)
{
  char
    filename[MaxTextExtent];

  register char
    *p;

  assert(image_info != (ImageInfo *) NULL);
  assert(image != (Image *) NULL);
  assert(type != (char *) NULL);
  if (image_info->blob.data != (char *) NULL)
    {
      image->blob=image_info->blob;
      return(True);
    }
  image->exempt=False;
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
      if ((Extent(filename) > 4) &&
          (Latin1Compare(filename+Extent(filename)-4,".pgp") == 0))
        {
          /*
            Decrypt image file with PGP encryption utilities.
          */
          if (*type == 'r')
            p=GetDelegateCommand(image_info,image,"pgp",(char *) NULL);
        }
      else
        if ((Extent(filename) > 4) &&
            (Latin1Compare(filename+Extent(filename)-4,".bz2") == 0))
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
          if ((Extent(filename) > 3) &&
              (Latin1Compare(filename+Extent(filename)-3,".gz") == 0))
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
            if ((Extent(filename) > 2) &&
                (Latin1Compare(filename+Extent(filename)-2,".Z") == 0))
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
      (void) strcpy(filename,p);
      FreeMemory((char *) p);
    }
  /*
    Open image file.
  */
  image->pipe=False;
  if (Latin1Compare(filename,"-") == 0)
    {
      image->file=(*type == 'r') ? stdin : stdout;
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
        image->pipe=True;
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
                  if ((Latin1Compare(filename,image->filename) == 0) ||
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
        if (image->file != (FILE *) NULL)
          {
            (void) SeekBlob(image,0L,SEEK_END);
            image->filesize=TellBlob(image);
            (void) SeekBlob(image,0L,SEEK_SET);
          }
      }
  image->status=False;
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  return(image->file != (FILE *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlob reads data from the blob or image file and returns it.  It
%  returns the number of bytes read.
%
%  The format of the ReadBlob routine is:
%
%      count=ReadBlob(image,number_bytes,data);
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlob returns the number of items read.
%
%    o image: The address of a structure of type Image.
%
%    o number_bytes:  Specifies an integer representing the number of bytes
%      to read from the file.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
Export unsigned long ReadBlob(Image *image,const unsigned long number_bytes,
  char *data)
{
  register int
    i;

  unsigned long
    count,
    offset;

  assert(image != (Image *) NULL);
  assert(data != (char *) NULL);
  if (image->blob.data != (char *) NULL)
    {
      /*
        Read bytes from blob.
      */
      offset=Min(number_bytes,image->blob.length-image->blob.offset);
      if (number_bytes > 0)
        (void) memcpy(data,image->blob.data+image->blob.offset,offset);
      image->blob.offset+=offset;
      return(offset);
    }
  /*
    Read bytes from a file handle.
  */
  offset=0;
  for (i=number_bytes; i > 0; i-=count)
  {
    count=fread(data+offset,1,number_bytes,image->file);
    if (count <= 0)
      break;
    offset+=count;
  }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S e e k B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SeekBlob sets the offset in bytes from the beginning of a blob or
%  file.
%
%  The format of the SeekBlob routine is:
%
%      status=SeekBlob(image,offset,whence)
%
%  A description of each parameter follows:
%
%    o status:  Method SeekBlob returns 0 on success; otherwise,  it
%      returned -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
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
Export int SeekBlob(Image *image,const long offset,const int whence)
{
  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(fseek(image->file,offset,whence));
  switch(whence)
  {
    case SEEK_SET:
    default:
    {
      if (offset < 0)
        return(-1);
      if (offset >= image->blob.length)
        return(-1);
      image->blob.offset=offset;
      break;
    }
    case SEEK_CUR:
    {
      if ((image->blob.offset+offset) < 0)
        return(-1);
      if ((image->blob.offset+offset) >= (long) image->blob.length)
        return(-1);
      image->blob.offset+=offset;
      break;
    }
    case SEEK_END:
    {
      if ((image->blob.offset+image->blob.length+offset) < 0)
        return(-1);
      if ((image->blob.offset+image->blob.length+offset) >= image->blob.length)
        return(-1);
      image->blob.offset+=image->blob.length+offset;
      break;
    }
  }
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T e l l B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TellBlob obtains the current value of the blob or file position.
%
%  The format of the TellBlob routine is:
%
%      status=TellBlob(image)
%
%  A description of each parameter follows:
%
%    o status:  Method TellBlob returns 0 on success; otherwise,  it
%      returned -1 and set errno to indicate the error.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export int TellBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  if (image->blob.data == (char *) NULL)
    return(ftell(image->file));
  return(image->blob.offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  W r i t e B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlob writes data to a blob or image file.  It returns the
%  number of items written.
%
%  The format of the WriteBlob routine is:
%
%      count=WriteBlob(image,size,number_items,data)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteBlob returns True if all the data requested
%      is obtained without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o size:  Specifies an integer representing the length of an
%      individual item to be written to the file.
%
%    o number_items:  Specifies an integer representing the number of items
%      to write to the file.
%
%    o data:  Specifies an data to write to the file.
%
%
*/
Export unsigned long WriteBlob(Image *image,const unsigned long number_bytes,
  const char *data)
{
  unsigned long
    count;

  assert(image != (Image *) NULL);
  assert(data != (const char *) NULL);
  if (image->blob.data == (char *) NULL)
    {
      count=(long) fwrite((char *) data,1,number_bytes,image->file);
      return(count);
    }
  if (number_bytes > (image->blob.extent-image->blob.offset))
    {
      image->blob.extent+=number_bytes+MaxBlobExtent;
      image->blob.data=
        ReallocateMemory(image->blob.data,image->blob.extent);
      if (image->blob.data == (char *) NULL)
        {
          image->blob.extent=0;
          return(0);
        }
    }
  memcpy(image->blob.data+image->blob.offset,data,number_bytes);
  image->blob.offset+=number_bytes;
  if (image->blob.offset > image->blob.length)
    image->blob.length=image->blob.offset;
  return(number_bytes);
}
