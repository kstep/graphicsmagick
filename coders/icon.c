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
typedef struct _IconInfo
{
  magick_uint32_t
    size,
    width,
    height;
  
  magick_uint16_t
    planes,
    bits_per_pixel;
  
  magick_uint32_t
    compression,
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    colors_important;
} IconInfo;

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
static void LogICONInfo(const const unsigned int i, const IconInfo *icon_info)
{
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "IconInfo[%u]:\n"
                        "    Size:             %u\n"
                        "    Width:            %u\n"
                        "    Height:           %u\n"
                        "    Planes:           %u\n"
                        "    BPP:              %u\n"
                        "    Compression:      %u\n"
                        "    Image Size:       %u\n"
                        "    X Pixels:         %u\n"
                        "    Y Pixels:         %u\n"
                        "    Number Colors:    %u\n"
                        "    Important Colors: %u\n",
                        i,
                        (unsigned int) icon_info->size,
                        (unsigned int) icon_info->width,
                        (unsigned int) icon_info->height,
                        (unsigned int) icon_info->planes,
                        (unsigned int) icon_info->bits_per_pixel,
                        (unsigned int) icon_info->compression,
                        (unsigned int) icon_info->image_size,
                        (unsigned int) icon_info->x_pixels,
                        (unsigned int) icon_info->y_pixels,
                        (unsigned int) icon_info->number_colors,
                        (unsigned int) icon_info->colors_important
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

  IconInfo
    icon_info;

  Image
    *image;

  int
    bit,
    byte;

  long
    y;
 
  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

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
  }
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify and read icons
    */
    if (SeekBlob(image,icon_file.directory[i].offset,SEEK_SET) !=
        (magick_off_t) icon_file.directory[i].offset)
      {
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Failed to seek to offset %u",
                                icon_file.directory[i].offset);
        ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    icon_info.size=ReadBlobLSBLong(image);
    icon_info.width=ReadBlobLSBLong(image);
    icon_info.height=ReadBlobLSBLong(image);
    icon_info.planes=ReadBlobLSBShort(image);
    icon_info.bits_per_pixel=ReadBlobLSBShort(image);
    icon_info.compression=ReadBlobLSBLong(image);
    icon_info.image_size=ReadBlobLSBLong(image);
    icon_info.x_pixels=ReadBlobLSBLong(image);
    icon_info.y_pixels=ReadBlobLSBLong(image);
    icon_info.number_colors=ReadBlobLSBLong(image);
    icon_info.colors_important=ReadBlobLSBLong(image);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    if (image->logging)
      LogICONInfo(i, &icon_info);
    if (icon_info.number_colors > 256)
      {
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Unexpected number of colors %u",
                                icon_info.number_colors);
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }
    if ((icon_info.bits_per_pixel != 0) &&
        (icon_info.bits_per_pixel != 1) &&
        (icon_info.bits_per_pixel != 4) &&
        (icon_info.bits_per_pixel != 8) &&
        (icon_info.bits_per_pixel != 16) &&
        (icon_info.bits_per_pixel != 24) &&
        (icon_info.bits_per_pixel != 32))
      {
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Unexpected bits per pixel %u",
                                icon_info.bits_per_pixel);
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }
    image->matte=(unsigned int) (icon_info.bits_per_pixel == 32U);
    /* FIXME: Sometimes 1 or 4bpp images claim 2X the height, but we
       fail to read such images. */
    if ((icon_info.width == 0) || (icon_info.width > 256) ||
        (icon_info.height == 0) || (icon_info.height > 256))
      {
        if (image->logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Unexpected width or height: %ux%u",
                                icon_info.width, icon_info.height);
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
      }
    image->columns=icon_info.width;
    image->rows=icon_info.height;
    image->depth=8;
    if ((icon_info.number_colors != 0) || (icon_info.bits_per_pixel <= 16U))
      {
        image->storage_class=PseudoClass;
        image->colors=icon_info.number_colors;
        if (image->colors == 0)
          image->colors=1 << icon_info.bits_per_pixel;
      }
  if (image->storage_class == PseudoClass)
    {
      register long
        i;

      unsigned char
        *icon_colormap;

      /*
        Read Icon raster colormap.
      */
      if (!AllocateImageColormap(image,1 << icon_info.bits_per_pixel))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      icon_colormap=MagickAllocateArray(unsigned char *,4,image->colors);
      if (icon_colormap == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      if (ReadBlob(image,4*image->colors,(char *) icon_colormap) !=
          4*image->colors)
        {
          MagickFreeMemory(icon_colormap);
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        }
      p=icon_colormap;
      for (i=0; i < (long) image->colors; i++)
      {
        image->colormap[i].blue=(Quantum) ScaleCharToQuantum(*p++);
        image->colormap[i].green=(Quantum) ScaleCharToQuantum(*p++);
        image->colormap[i].red=(Quantum) ScaleCharToQuantum(*p++);
        p++;
      }
      MagickFreeMemory(icon_colormap);
    } /* if (image->storage_class == PseudoClass) */
    /*
      Convert Icon raster image to pixel packets.
    */
    image->columns=icon_file.directory[i].width;
    image->rows=icon_file.directory[i].height;
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    switch (icon_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        unsigned int
          index;

        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < ((long) image->columns-7); x+=8)
          {
            byte=ReadBlobByte(image);
            for (bit=0; bit < 8; bit++)
              {
                index=(byte & (0x80 >> bit) ? 0x01 : 0x00);
                indexes[x+bit]=index;
                q[x+bit]=image->colormap[index];
              }
          }
          if ((image->columns % 8) != 0)
            {
              byte=ReadBlobByte(image);
              for (bit=0; bit < (long) (image->columns % 8); bit++)
                {
                  index=((byte) & (0x80 >> bit) ? 0x01 : 0x00);
                  indexes[x+bit]=index;
                  q[x+bit]=image->colormap[index];
                }
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename,
					  image->columns,image->rows))
                break;
        }
        break;
      }
      case 4:
      {
        /*
          Read 4-bit Icon scanline.
        */
        unsigned int
          index;

        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < ((long) image->columns-1); x+=2)
          {
            byte=ReadBlobByte(image);

            index=(byte >> 4) & 0xf;
            indexes[x]=index;
            q[x]=image->colormap[index];

            index=(byte) & 0xf;
            indexes[x+1]=index;
            q[x+1]=image->colormap[index];
          }
          if ((image->columns % 2) != 0)
            {
              byte=ReadBlobByte(image);
              index=(byte >> 4) & 0xf;
              indexes[x]=index;
              q[x]=image->colormap[index];
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename,
					  image->columns,image->rows))
                break;
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        unsigned int
          index;

        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            byte=ReadBlobByte(image);
            index=(IndexPacket) byte;
            indexes[x]=index;
            q[x]=image->colormap[index];
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename,
					  image->columns,image->rows))
                break;
        }
        break;
      }
      case 16:
      {
        /*
          Convert PseudoColor scanline.

          Does not seem likely. Expects 16-bit IndexPacket.
        */
        unsigned int
          index;

        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            byte=ReadBlobByte(image);
            index=(IndexPacket) byte;

            byte=ReadBlobByte(image);
            index |= byte << 8;

            indexes[x]=(IndexPacket) index;
            q[x]=image->colormap[index];
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename,
					  image->columns,image->rows))
                break;
        }
        break;
      }
      case 24:
      case 32:
      {
        /*
          Convert DirectColor scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->blue=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q->green=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q->red=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            if (image->matte)
              q->opacity=(Quantum) ScaleCharToQuantum(ReadBlobByte(image));
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                          LoadImageText,image->filename,
					  image->columns,image->rows))
                break;
        }
        break;
      }
      default:
        ThrowReaderException(CorruptImageError,ImproperImageHeader,image)
    }
    /*
      Handle opacity mask.

      Should this be disabled for 32-bit images (with matte)?
    */
    image->storage_class=DirectClass;
    image->matte=True;
    for (y=(long) image->rows-1; y >= 0; y--)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < ((long) image->columns-7); x+=8)
      {
        byte=ReadBlobByte(image);
        for (bit=0; bit < 8; bit++)
          q[x+bit].opacity=(Quantum) 
            (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
      }
      if ((image->columns % 8) != 0)
        {
          byte=ReadBlobByte(image);
          for (bit=0; bit < (long) (image->columns % 8); bit++)
            q[x+bit].opacity=(Quantum) 
              (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
        }
     if (image->columns % 32) 
       for (x=0; x < (long) ((32-(image->columns % 32))/8); x++)
         (void) ReadBlobByte(image);
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(image->rows-y-1,image->rows,&image->exception,
                                      LoadImageText,image->filename,
				      image->columns,image->rows))
            break;
    }
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
          image->filename);
        break;
      }
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
