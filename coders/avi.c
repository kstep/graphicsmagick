/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                             AAA   V   V  IIIII                              %
%                            A   A  V   V    I                                %
%                            AAAAA  V   V    I                                %
%                            A   A   V V     I                                %
%                            A   A    V    IIIII                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"

/*
  Typedef declaractions.
*/
typedef struct _AVIInfo
{
  unsigned long
    delay,
    max_data_rate,
    pad_granularity,
    flags,
    total_frames,
    initial_frames,
    number_streams,
    buffer_size,
    width,
    height,
    time_scale,
    data_rate,
    start_time,
    data_length;
} AVIInfo;

typedef struct _BMPInfo
{
  unsigned long
    size,
    width,
    height,
    planes,
    bits_per_pixel;

  char
    compression[5];

  unsigned long
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    important_colors;
} BMPInfo;

typedef struct _StreamInfo
{
  char
    data_type[5],
    data_handler[5];

  unsigned long
    flags,
    priority,
    initial_frames,
    time_scale,
    data_rate,
    start_time,
    data_length,
    buffer_size,
    quality,
    sample_size;
} StreamInfo;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DecodeImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,const unsigned int compression,
%        const unsigned int number_columns,const unsigned int number_rows,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o compression:  A value of 1 means the compressed pixels are runlength
%      encoded for a 256-color bitmap.  A value of 2 means a 16-color bitmap.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,const unsigned int compression,
  const unsigned int number_columns,const unsigned int number_rows,
  unsigned char *pixels)
{
  int
    byte,
    count,
    y;

  register int
    i,
    x;

  register unsigned char
    *q;

  assert(image != (Image *) NULL);
  assert(pixels != (unsigned char *) NULL);
  for (i=0; i < (int) (number_columns*number_rows); i++)
    pixels[i]=0;
  byte=0;
  x=0;
  q=pixels;
  for (y=0; y < (int) number_rows; )
  {
    count=ReadByte(image);
    if (count == EOF)
      break;
    if (count != 0)
      {
        /*
          Encoded mode.
        */
        byte=ReadByte(image);
        for (i=0; i < count; i++)
        {
          if (compression == 1)
            *q++=(unsigned char) byte;
          else
            *q++=(i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
          x++;
        }
      }
    else
      {
        /*
          Escape mode.
        */
        count=ReadByte(image);
        if (count == 0x01)
          return(True);
        switch (count)
        {
          case 0x00:
          {
            /*
              End of line.
            */
            x=0;
            y++;
            q=pixels+y*number_columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=ReadByte(image);
            y+=ReadByte(image);
            q=pixels+y*number_columns+x;
            break;
          }
          default:
          {
            /*
              Absolute mode.
            */
            for (i=0; i < count; i++)
            {
              if (compression == 1)
                *q++=ReadByte(image);
              else
                {
                  if ((i & 0x01) == 0)
                    byte=ReadByte(image);
                  *q++=(i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
                }
              x++;
            }
            /*
              Read pad byte.
            */
            if (compression == 1)
              {
                if (count & 0x01)
                  (void) ReadByte(image);
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                (void) ReadByte(image);
            break;
          }
        }
      }
    if (QuantumTick(y,number_rows))
      MagickMonitor(LoadImageText,y,number_rows);
  }
  (void) ReadByte(image);  /* end of line */
  (void) ReadByte(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s A V I                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsAVI returns True if the image format type, identified by the
%  magick string, is Audio/Video Interleaved file format.
%
%  The format of the IsAVI method is:
%
%      unsigned long IsAVI(const unsigned char *magick,
%        const unsigned long int)
%
%  A description of each parameter follows:
%
%    o status:  Method IsAVI returns True if the image format type is AVI.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsAVI(const unsigned char *magick,
  const unsigned int length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"RIFF",4) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A V I I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadAVIImage reads an Audio Video Interleave image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadAVIImage method is:
%
%      Image *ReadAVIImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadAVIImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadAVIImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  AVIInfo
    avi_info;

  BMPInfo
    bmp_info;

  char
    id[5],
    message[MaxTextExtent];

  Image
    *image;

  IndexPacket
    index;

  int
    bit,
    y;

  PixelPacket
    *colormap;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  StreamInfo
    stream_info;

  unsigned char
    *pixels;

  unsigned int
    bytes_per_line,
    number_colors,
    chunk_size,
    status;

  unsigned long
    count;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  memset(&avi_info,0,sizeof(AVIInfo));
  memset(&bmp_info,0,sizeof(BMPInfo));
  colormap=(PixelPacket *) NULL;
  number_colors=0;
  for ( ; ; )
  {
    count=ReadBlob(image,4,id);
    if (count == 0)
      break;
    id[4]='\0';
    chunk_size=LSBFirstReadLong(image);
    if (chunk_size & 0x01)
      chunk_size++;
    if (image_info->verbose)
      {
        (void) fprintf(stdout,"AVI cid %s\n",id);
        (void) fprintf(stdout,"  chunk size %u\n",chunk_size);
      }
    if (LocaleCompare(id,"00dc") == 0)
      {
        /*
          Initialize image structure.
        */
        image->columns=avi_info.width;
        image->rows=avi_info.height;
        image->units=PixelsPerCentimeterResolution;
        image->x_resolution=bmp_info.x_pixels/100.0;
        image->y_resolution=bmp_info.y_pixels/100.0;
        if (!AllocateImageColormap(image,number_colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        memcpy(image->colormap,colormap,image->colors*sizeof(PixelPacket));
        bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
        pixels=(unsigned char *) AcquireMemory(bytes_per_line*image->rows);
        status=DecodeImage(image,1,avi_info.width,avi_info.height,pixels);
        if (status == False)
          ThrowReaderException(CorruptImageWarning,"unable to runlength decode",
            image);
        /*
          Initialize image structure.
        */
        image->units=PixelsPerCentimeterResolution;
        image->x_resolution=bmp_info.x_pixels/100.0;
        image->y_resolution=bmp_info.y_pixels/100.0;
        /*
          Convert BMP raster image to pixel packets.
        */
        switch (bmp_info.bits_per_pixel)
        {
          case 1:
          {
            /*
              Convert bitmap scanline.
            */
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < ((int) image->columns-7); x+=8)
              {
                for (bit=0; bit < 8; bit++)
                {
                  index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                  indexes[x+bit]=index;
                  *q++=image->colormap[index];
                }
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=0; bit < (int) (image->columns % 8); bit++)
                  {
                    index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                    indexes[x+bit]=index;
                    *q++=image->colormap[index];
                  }
                  p++;
                }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
            }
            break;
          }
          case 4:
          {
            /*
              Convert PseudoColor scanline.
            */
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < ((int) image->columns-1); x+=2)
              {
                index=(*p >> 4) & 0xf;
                indexes[x]=index;
                *q++=image->colormap[index];
                index=(*p) & 0xf;
                indexes[x+1]=index;
                *q++=image->colormap[index];
                p++;
              }
              if ((image->columns % 2) != 0)
                {
                  index=(*p >> 4) & 0xf;
                  indexes[x]=index;
                  *q++=image->colormap[index];
                  p++;
                }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
            }
            break;
          }
          case 8:
          {
            /*
              Convert PseudoColor scanline.
            if ((bmp_info.compression == 1) || (bmp_info.compression == 2))
            */
              bytes_per_line=image->columns;
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < (int) image->columns; x++)
              {
                index=(*p++);
                indexes[x]=index;
                *q++=image->colormap[index];
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
            }
            break;
          }
          case 16:
          {
            unsigned short
              word;

            /*
              Convert PseudoColor scanline.
            if (bmp_info.compression == 1)
            */
              bytes_per_line=image->columns << 1;
            image->storage_class=DirectClass;
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) image->columns; x++)
              {
                word=(*p++);
                word|=(*p++ << 8);
                q->red=UpScale(ScaleColor5to8((word >> 11) & 0x1f));
                q->green=UpScale(ScaleColor6to8((word >> 5) & 0x3f));
                q->blue=UpScale(ScaleColor5to8(word & 0x1f));
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
            }
            break;
          }
          case 24:
          case 32:
          {
            /*
              Convert DirectColor scanline.
            */
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) image->columns; x++)
              {
                q->blue=UpScale(*p++);
                q->green=UpScale(*p++);
                q->red=UpScale(*p++);
                if (image->matte)
                  q->opacity=UpScale(*p++);
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,image->rows-y-1,image->rows);
            }
            break;
          }
          default:
            ThrowReaderException(CorruptImageWarning,"Not a avi image file",
              image);
        }
        LiberateMemory((void **) &pixels);
        if (image->scene < (avi_info.total_frames-1))
          {
            /*
              Allocate next image structure.
            */
            AllocateNextImage(image_info,image);
            if (image->next == (Image *) NULL)
              {
                DestroyImages(image);
                return((Image *) NULL);
              }
            image=image->next;
            MagickMonitor(LoadImagesText,TellBlob(image),image->filesize);
          }
        continue;
      }
    if (LocaleCompare(id,"avih") == 0)
      {
        /*
          AVI header.
        */
        if (image_info->verbose)
          (void) fprintf(stdout,"  AVI header\n");
        avi_info.delay=LSBFirstReadLong(image);
        avi_info.max_data_rate=LSBFirstReadLong(image);
        avi_info.pad_granularity=LSBFirstReadLong(image);
        avi_info.flags=LSBFirstReadLong(image);
        avi_info.total_frames=LSBFirstReadLong(image);
        avi_info.initial_frames=LSBFirstReadLong(image);
        avi_info.number_streams=LSBFirstReadLong(image);
        avi_info.buffer_size=LSBFirstReadLong(image);
        avi_info.width=LSBFirstReadLong(image);
        avi_info.height=LSBFirstReadLong(image);
        avi_info.time_scale=LSBFirstReadLong(image);
        avi_info.data_rate=LSBFirstReadLong(image);
        avi_info.start_time=LSBFirstReadLong(image);
        avi_info.data_length=LSBFirstReadLong(image);
        continue;
      }
    if (LocaleCompare(id,"idx1") == 0)
      {
        for ( ; chunk_size > 0; chunk_size--)
          (void) ReadByte(image);
        continue;
      }
    if (LocaleCompare(id,"JUNK") == 0)
      {
        for ( ; chunk_size > 0; chunk_size--)
          (void) ReadByte(image);
        continue;
      }
    if (LocaleCompare(id,"LIST") == 0)
      {
        /*
          List of chunks.
        */
        (void) ReadBlob(image,4,id);
        if (image_info->verbose)
          (void) fprintf(stdout,"  List type %s\n",id);
        continue;
      }
    if (LocaleCompare(id,"RIFF") == 0)
      {
        /*
          File header.
        */
        (void) ReadBlob(image,4,id);
        if (image_info->verbose)
          (void) fprintf(stdout,"  RIFF form type %s\n",id);
        continue;
      }
    if (LocaleCompare(id,"strf") == 0)
      {
        /*
          Stream format.
        */
        if (image_info->verbose)
          (void) fprintf(stdout,"  Stream fcc\n");
        if (LocaleCompare(stream_info.data_type,"auds") == 0)
          {
            continue;
          }
        if (LocaleCompare(stream_info.data_type,"pads") == 0)
          {
            continue;
          }
        if (LocaleCompare(stream_info.data_type,"txts") == 0)
          {
            continue;
          }
        if (LocaleCompare(stream_info.data_type,"vids") == 0)
          {
            bmp_info.size=LSBFirstReadLong(image);
            bmp_info.width=LSBFirstReadLong(image);
            bmp_info.height=LSBFirstReadLong(image);
            bmp_info.planes=LSBFirstReadShort(image);
            bmp_info.bits_per_pixel=LSBFirstReadShort(image);
            (void) ReadBlob(image,4,bmp_info.compression);
            bmp_info.compression[4]='\0';
            bmp_info.image_size=LSBFirstReadLong(image);
            bmp_info.x_pixels=LSBFirstReadLong(image);
            bmp_info.y_pixels=LSBFirstReadLong(image);
            bmp_info.number_colors=LSBFirstReadLong(image);
            bmp_info.important_colors=LSBFirstReadLong(image);
            chunk_size-=40;
            number_colors=bmp_info.number_colors;
            if ((number_colors == 0) && (bmp_info.bits_per_pixel <= 8))
              number_colors=1 << bmp_info.bits_per_pixel;
            if (number_colors != 0)
              {
                colormap=(PixelPacket *)
                  AcquireMemory(number_colors*sizeof(PixelPacket));
                if (colormap == (PixelPacket *) NULL)
                  ThrowReaderException(ResourceLimitWarning,
                    "Memory allocation failed",image);
                for (i=0; i < number_colors; i++)
                {
                  colormap[i].blue=ReadByte(image);
                  colormap[i].green=ReadByte(image);
                  colormap[i].red=ReadByte(image);
                  (void) ReadByte(image);
                  chunk_size-=4;
                }
              }
            for ( ; chunk_size > 0; chunk_size--)
              (void) ReadByte(image);
            if (image_info->verbose)
              (void) fprintf(stdout,"Video compression: %s\n",
                bmp_info.compression);
            continue;
          }
        continue;
      }
    if (LocaleCompare(id,"strh") == 0)
      {
        if (image_info->verbose)
          (void) fprintf(stdout,"  Stream header\n");
        (void) ReadBlob(image,4,stream_info.data_type);
        stream_info.data_type[4]='\0';
        (void) ReadBlob(image,4,stream_info.data_handler);
        stream_info.data_handler[4]='\0';
        stream_info.flags=LSBFirstReadLong(image);
        stream_info.priority=LSBFirstReadLong(image);
        stream_info.initial_frames=LSBFirstReadLong(image);
        stream_info.time_scale=LSBFirstReadLong(image);
        stream_info.data_rate=LSBFirstReadLong(image);
        stream_info.start_time=LSBFirstReadLong(image);
        stream_info.data_length=LSBFirstReadLong(image);
        stream_info.buffer_size=LSBFirstReadLong(image);
        stream_info.quality=LSBFirstReadLong(image);
        stream_info.sample_size=LSBFirstReadLong(image);
        if (chunk_size & 0x01)
          chunk_size++;
        for (chunk_size-=48; chunk_size > 0; chunk_size--)
          (void) ReadByte(image);
        if (image_info->verbose)
          (void) fprintf(stdout,"AVI Test handler: %s\n",
            stream_info.data_handler);
        continue;
      }
    if (LocaleCompare(id,"vedt") == 0)
      {
        for ( ; chunk_size > 0; chunk_size--)
          (void) ReadByte(image);
        continue;
      }
    FormatString(message,"AVI support for chunk %.1024s not yet available",id);
    ThrowReaderException(CorruptImageWarning,message,image);
  }
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
%   R e g i s t e r A V I I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterAVIImage adds attributes for the AVI image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterAVIImage method is:
%
%      RegisterAVIImage(void)
%
*/
ModuleExport void RegisterAVIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("AVI");
  entry->decoder=ReadAVIImage;
  entry->magick=IsAVI;
  entry->description=AllocateString("Audio/Visual Interleaved");
  entry->module=AllocateString("AVI");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r A V I I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterAVIImage removes format registrations made by the
%  AVI module from the list of supported formats.
%
%  The format of the UnregisterAVIImage method is:
%
%      UnregisterAVIImage(void)
%
*/
ModuleExport void UnregisterAVIImage(void)
{
  UnregisterMagickInfo("AVI");
}
