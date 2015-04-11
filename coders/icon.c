/*
% Copyright (C) 2003-2015 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        IIIII   CCCC   OOO   N   N                           %
%                          I    C      O   O  NN  N                           %
%                          I    C      O   O  N N N                           %
%                          I    C      O   O  N  NN                           %
%                        IIIII   CCCC   OOO   N   N                           %
%                                                                             %
%                                                                             %
%                   Read Microsoft Windows Icon Format.                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                Re-written                                   %
%                              Bob Friesenhahn                                %
%                                January 2015                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/* http://msdn.microsoft.com/en-us/library/ms997538.aspx */

#define MaxIcons  256
/*
  Icon Entry
*/
typedef struct _IconDirEntry
{
  magick_uint8_t
    width,
    height,
    colors,
    reserved;
  
  magick_uint16_t
    planes,
    bits_per_pixel;
  
  magick_uint32_t
    size,
    offset;
} IconDirEntry;

/*
  Icon Directory
*/
typedef struct _IconFile
{
  magick_uint16_t
    reserved,
    resource_type, /* 1 = ICON, 2 = CURSOR */
    count;
  
  IconDirEntry
    directory[MaxIcons];
} IconFile;

static void LogICONDirEntry(const unsigned int i, const IconDirEntry *icon_entry)
{
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "IconDirEntry[%u]:\n"
                        "    Width:  %u\n"
                        "    Height: %u\n"
                        "    Colors: %u\n"
                        "    Reserved: %u\n"
                        "    Planes: %u\n"
                        "    BPP:    %u\n"
                        "    Size:   %u\n"
                        "    Offset: %u",
                        i,
                        (unsigned int) icon_entry->width,
                        (unsigned int) icon_entry->height,
                        (unsigned int) icon_entry->colors,
                        (unsigned int) icon_entry->reserved,
                        (unsigned int) icon_entry->planes,
                        (unsigned int) icon_entry->bits_per_pixel,
                        (unsigned int) icon_entry->size,
                        (unsigned int) icon_entry->offset
                        );
}
static void LogICONFile(const IconFile *icon_file)
{
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "IconFile:\n"
                        "    Reserved:     %u\n"
                        "    ResourceType: %u\n"
                        "    Count:        %u",
                        (unsigned int) icon_file->reserved,
                        (unsigned int) icon_file->resource_type,
                        (unsigned int) icon_file->count
                        );
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C O N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadIconImage reads a Microsoft icon image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadIconImage method is:
%
%      Image *ReadIconImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadIconImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadIconImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  IconFile
    icon_file;

  Image
    *image;

  unsigned char
    *data;

  size_t
    data_alloc_size;

  register long
    i;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read and validate icon header
  */
  icon_file.reserved=ReadBlobLSBShort(image);
  icon_file.resource_type=ReadBlobLSBShort(image);
  icon_file.count=ReadBlobLSBShort(image);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if (image->logging)
    LogICONFile(&icon_file);
  /*
    Windows ICO and CUR formats are essentially the same except that
    .CUR uses resource_type==1 while .ICO uses resource_type=2.
  */
  if ((icon_file.reserved != 0) ||
      ((icon_file.resource_type != 1) &&
       (icon_file.resource_type != 2)) ||
      (icon_file.count > MaxIcons))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Read and validate icon directory.
  */
  data_alloc_size=0;
  data=(unsigned char *) NULL;
  for (i=0; i < icon_file.count; i++)
  {
    /* 0 - 255, 0 means 256! */
    icon_file.directory[i].width=ReadBlobByte(image);
    /* 0 - 255, 0 means 256! */
    icon_file.directory[i].height=ReadBlobByte(image);
    icon_file.directory[i].colors=ReadBlobByte(image);
    icon_file.directory[i].reserved=ReadBlobByte(image);
    icon_file.directory[i].planes=ReadBlobLSBShort(image);
    icon_file.directory[i].bits_per_pixel=ReadBlobLSBShort(image);
    icon_file.directory[i].size=ReadBlobLSBLong(image);
    icon_file.directory[i].offset=ReadBlobLSBLong(image);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    if (image->logging)
      LogICONDirEntry(i,&icon_file.directory[i]);
    if (
        /*
          Restrict allocation size
        */
        (icon_file.directory[i].size < 20) ||
        (icon_file.directory[i].size > 256+256*256*2*4) ||

        /*
          planes

          In ICO format (1): Specifies color planes. Should be 0 or 1.

          In CUR format (2): Specifies the horizontal coordinates of
          the hotspot in number of pixels from the left.
        */
        ((icon_file.resource_type == 1) &&
         (icon_file.directory[i].planes != 0) &&
         (icon_file.directory[i].planes != 1)) ||
        /*
          bits_per_pixel

          In ICO format (1): Specifies bits per pixel.

          In CUR format (2): Specifies the vertical coordinates of the
          hotspot in number of pixels from the top.
         */
        ((icon_file.resource_type == 1) &&
         ((icon_file.directory[i].bits_per_pixel >= 8) &&
          (icon_file.directory[i].colors != 0))) ||

        (icon_file.directory[i].size == 0) ||
        (icon_file.directory[i].offset == 0))
      ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
    data_alloc_size=Max(data_alloc_size,icon_file.directory[i].size);
  }
  data=MagickAllocateMemory(unsigned char *,data_alloc_size);
  if (data == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify and read icons
    */
    Image
      *icon_image;

    ImageInfo
      *read_info;

    char
      dib_size[MaxTextExtent],
      format[MaxTextExtent];

    size_t
      count;

    if (SeekBlob(image,icon_file.directory[i].offset,SEEK_SET) !=
        (magick_off_t) icon_file.directory[i].offset)
      {
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Failed to seek to offset %u",
                                icon_file.directory[i].offset);
        MagickFreeMemory(data);
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    if ((count=ReadBlob(image,icon_file.directory[i].size,data)) != icon_file.directory[i].size)
      {
        if (image->logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Read %" MAGICK_SIZE_T_F  "u bytes from blob"
                                  " (expected %" MAGICK_SIZE_T_F  "u bytes)",
                                  (MAGICK_SIZE_T) count,
                                  (MAGICK_SIZE_T) icon_file.directory[i].size);
        MagickFreeMemory(data);
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    format[0]='\0';
    if (memcmp(data,"\050\000\000\000",4) == 0)
      (void) strcpy(format,"ICODIB");
    else if (memcmp(data,"\211PNG\r\n\032\n",8) == 0)
      (void) strcpy(format,"PNG");
    if (format[0] == '\0')
      {
        MagickFreeMemory(data);
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }
    if (image->logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Reading icon using %s format",
                            format);
    icon_image=(Image *) NULL;
    read_info=CloneImageInfo(image_info);
    if (read_info != (const ImageInfo *) NULL)
      {
        (void) strlcpy(read_info->magick,format,MaxTextExtent);
        (void) strlcpy(read_info->filename,format,MaxTextExtent);
        (void) strlcat(read_info->filename,":",MaxTextExtent);
        FormatString(dib_size,"%ux%u",
                     icon_file.directory[i].width == 0 ? 256 :
                     icon_file.directory[i].width,
                     icon_file.directory[i].height == 0 ? 256 :
                     icon_file.directory[i].height);
        (void) CloneString(&read_info->size,dib_size);
        icon_image=BlobToImage(read_info,data,icon_file.directory[i].size,exception);
        DestroyImageInfo(read_info);
        read_info=(ImageInfo *) NULL;
      }
    if (icon_image == (Image *) NULL)
      {
        MagickFreeMemory(data);
        DestroyImageList(image);
        return((Image *) NULL);
      }
    DestroyBlob(icon_image);
    icon_image->blob=ReferenceBlob(image->blob);
    icon_image->scene=i;
    (void) strlcpy(icon_image->magick,image_info->magick,
                   sizeof(icon_image->magick));
    ReplaceImageInList(&image,icon_image);

    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (i < (long) (icon_file.count-1))
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
        if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),exception,
                                    LoadImagesText,image->filename))
          break;
      }
  } /* for (i=0; i < icon_file.count; i++) */
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  MagickFreeMemory(data);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r I C O N I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterICONImage adds attributes for the Icon image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterICONImage method is:
%
%      RegisterICONImage(void)
%
*/
ModuleExport void RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUR");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Cursor Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICO");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("ICON");
  entry->decoder=(DecoderHandler) ReadIconImage;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Microsoft Icon";
  entry->module="ICON";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I C O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterICONImage removes format registrations made by the
%  ICON module from the list of supported formats.
%
%  The format of the UnregisterICONImage method is:
%
%      UnregisterICONImage(void)
%
*/
ModuleExport void UnregisterICONImage(void)
{
  (void) UnregisterMagickInfo("CUR");
  (void) UnregisterMagickInfo("ICO");
  (void) UnregisterMagickInfo("ICON");
}
