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
#include "define.h"

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
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,const unsigned int compression,
  unsigned char *pixels)
{
  long
    byte,
    count,
    y;

  register long
    i,
    x;

  register unsigned char
    *q;

  assert(image != (Image *) NULL);
  assert(pixels != (unsigned char *) NULL);
  (void) memset(pixels,0,image->columns*image->rows);
  byte=0;
  x=0;
  q=pixels;
  for (y=0; y < (long) image->rows; )
  {
    count=ReadBlobByte(image);
    if (count == EOF)
      break;
    if (count != 0)
      {
        /*
          Encoded mode.
        */
        byte=ReadBlobByte(image);
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
        count=ReadBlobByte(image);
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
            q=pixels+y*image->columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=ReadBlobByte(image);
            y+=ReadBlobByte(image);
            q=pixels+y*image->columns+x;
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
                *q++=ReadBlobByte(image);
              else
                {
                  if ((i & 0x01) == 0)
                    byte=ReadBlobByte(image);
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
                  (void) ReadBlobByte(image);
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                (void) ReadBlobByte(image);
            break;
          }
        }
      }
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  (void) ReadBlobByte(image);  /* end of line */
  (void) ReadBlobByte(image);
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
%      unsigned long IsAVI(const unsigned char *magick,const size_t length)
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
static unsigned int IsAVI(const unsigned char *magick,const size_t length)
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
    id[MaxTextExtent],
    message[MaxTextExtent];

  Image
    *image;

  IndexPacket
    index;

  long
    bit,
    y;

  PixelPacket
    *colormap;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  StreamInfo
    stream_info;

  unsigned char
    *pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line,
    chunk_size,
    count,
    number_colors;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  (void) memset(&avi_info,0,sizeof(AVIInfo));
  (void) memset(&bmp_info,0,sizeof(BMPInfo));
  colormap=(PixelPacket *) NULL;
  number_colors=0;
  for ( ; ; )
  {
    count=ReadBlob(image,4,id);
    if (count == 0)
      break;
    id[4]='\0';
    chunk_size=ReadBlobLSBLong(image);
    if (chunk_size == 0)
      break;
    if (chunk_size & 0x01)
      chunk_size++;
    if (image_info->verbose)
      {
        (void) fprintf(stdout,"AVI cid %.1024s\n",id);
        (void) fprintf(stdout,"  chunk size %lu\n",chunk_size);
      }
    if ((LocaleCompare(id,"00db") == 0) || (LocaleCompare(id,"00dc") == 0))
      {
        /*
          Initialize image structure.
        */
        image->columns= avi_info.width;
        image->rows= avi_info.height;
        image->units=PixelsPerCentimeterResolution;
        image->x_resolution=bmp_info.x_pixels/100.0;
        image->y_resolution=bmp_info.y_pixels/100.0;
        if (!AllocateImageColormap(image,number_colors ? number_colors : 256))
          ThrowReaderException(ResourceLimitWarning,
            "Memory allocation failed",image);
        if (number_colors != 0)
          (void) memcpy(image->colormap,colormap,
            number_colors*sizeof(PixelPacket));
        bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
        pixels=(unsigned char *) AcquireMemory(bytes_per_line*image->rows);
        if (LocaleCompare(id,"00db") == 0)
          (void) ReadBlob(image,bytes_per_line*image->rows,(char *) pixels);
        else
          {
            status=DecodeImage(image,1,pixels);
            if (status == False)
              ThrowReaderException(CorruptImageWarning,
                "unable to runlength decode",image);
          }
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
              for (x=0; x < ((long) image->columns-7); x+=8)
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
                  for (bit=0; bit < (long) (image->columns % 8); bit++)
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
              for (x=0; x < ((long) image->columns-1); x+=2)
              {
                index=ValidateColormapIndex(image,(*p >> 4) & 0xf);
                indexes[x]=index;
                *q++=image->colormap[index];
                index=ValidateColormapIndex(image,*p & 0xf);
                indexes[x+1]=index;
                *q++=image->colormap[index];
                p++;
              }
              if ((image->columns % 2) != 0)
                {
                  index=ValidateColormapIndex(image,(*p >> 4) & 0xf);
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
            */
            bytes_per_line=image->columns;
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < (long) image->columns; x++)
              {
                index=ValidateColormapIndex(image,*p);
                indexes[x]=index;
                *q=image->colormap[index];
                p++;
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
              for (x=0; x < (long) image->columns; x++)
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
            image->storage_class=DirectClass;
            for (y=image->rows-1; y >= 0; y--)
            {
              p=pixels+(image->rows-y-1)*bytes_per_line;
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) image->columns; x++)
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
              image)
        }
        LiberateMemory((void **) &pixels);
        if ((unsigned long) image->scene < (avi_info.total_frames-1))
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
            MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
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
        avi_info.delay=ReadBlobLSBLong(image);
        avi_info.max_data_rate=ReadBlobLSBLong(image);
        avi_info.pad_granularity=ReadBlobLSBLong(image);
        avi_info.flags=ReadBlobLSBLong(image);
        avi_info.total_frames=ReadBlobLSBLong(image);
        avi_info.initial_frames=ReadBlobLSBLong(image);
        avi_info.number_streams=ReadBlobLSBLong(image);
        avi_info.buffer_size=ReadBlobLSBLong(image);
        avi_info.width=ReadBlobLSBLong(image);
        avi_info.height=ReadBlobLSBLong(image);
        avi_info.time_scale=ReadBlobLSBLong(image);
        avi_info.data_rate=ReadBlobLSBLong(image);
        avi_info.start_time=ReadBlobLSBLong(image);
        avi_info.data_length=ReadBlobLSBLong(image);
        continue;
      }
    if (LocaleCompare(id,"idx1") == 0)
      {
        for ( ; chunk_size != 0; chunk_size--)
          (void) ReadBlobByte(image);
        continue;
      }
    if (LocaleCompare(id,"JUNK") == 0)
      {
        for ( ; chunk_size != 0; chunk_size--)
          (void) ReadBlobByte(image);
        continue;
      }
    if (LocaleCompare(id,"LIST") == 0)
      {
        /*
          List of chunks.
        */
        (void) ReadBlob(image,4,id);
        if (image_info->verbose)
          (void) fprintf(stdout,"  List type %.1024s\n",id);
        continue;
      }
    if (LocaleCompare(id,"RIFF") == 0)
      {
        /*
          File header.
        */
        (void) ReadBlob(image,4,id);
        if (image_info->verbose)
          (void) fprintf(stdout,"  RIFF form type %.1024s\n",id);
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
          continue;
        if (LocaleCompare(stream_info.data_type,"pads") == 0)
          continue;
        if (LocaleCompare(stream_info.data_type,"txts") == 0)
          continue;
        if (LocaleCompare(stream_info.data_type,"vids") == 0)
          {
            bmp_info.size=ReadBlobLSBLong(image);
            bmp_info.width=ReadBlobLSBLong(image);
            bmp_info.height=ReadBlobLSBLong(image);
            bmp_info.planes=ReadBlobLSBShort(image);
            bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
            (void) ReadBlob(image,4,bmp_info.compression);
            bmp_info.compression[4]='\0';
            bmp_info.image_size=ReadBlobLSBLong(image);
            bmp_info.x_pixels=ReadBlobLSBLong(image);
            bmp_info.y_pixels=ReadBlobLSBLong(image);
            bmp_info.number_colors=ReadBlobLSBLong(image);
            bmp_info.important_colors=ReadBlobLSBLong(image);
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
                for (i=0; i < (long) number_colors; i++)
                {
                  colormap[i].blue=UpScale(ReadBlobByte(image));
                  colormap[i].green=UpScale(ReadBlobByte(image));
                  colormap[i].red=UpScale(ReadBlobByte(image));
                  (void) ReadBlobByte(image);
                  chunk_size-=4;
                }
              }
            for ( ; chunk_size != 0; chunk_size--)
              (void) ReadBlobByte(image);
            if (image_info->verbose)
              (void) fprintf(stdout,"Video compression: %.1024s\n",
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
        stream_info.flags=ReadBlobLSBLong(image);
        stream_info.priority=ReadBlobLSBLong(image);
        stream_info.initial_frames=ReadBlobLSBLong(image);
        stream_info.time_scale=ReadBlobLSBLong(image);
        stream_info.data_rate=ReadBlobLSBLong(image);
        stream_info.start_time=ReadBlobLSBLong(image);
        stream_info.data_length=ReadBlobLSBLong(image);
        stream_info.buffer_size=ReadBlobLSBLong(image);
        stream_info.quality=ReadBlobLSBLong(image);
        stream_info.sample_size=ReadBlobLSBLong(image);
        if (chunk_size & 0x01)
          chunk_size++;
        for (chunk_size-=48; chunk_size != 0; chunk_size--)
          (void) ReadBlobByte(image);
        if (image_info->verbose)
          (void) fprintf(stdout,"AVI Test handler: %.1024s\n",
            stream_info.data_handler);
        continue;
      }
    if (LocaleCompare(id,"strn") == 0)
      {
        for ( ; chunk_size != 0; chunk_size--)
          (void) ReadBlobByte(image);
        continue;
      }
    if (LocaleCompare(id,"vedt") == 0)
      {
        for ( ; chunk_size != 0; chunk_size--)
          (void) ReadBlobByte(image);
        continue;
      }
    FormatString(message,"AVI support for chunk %.1024s not yet available",id);
    ThrowException(exception,CorruptImageWarning,message,image->filename);
    break;
  }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
  CloseBlob(image);
  if ((image->columns == 0) || (image->rows == 0))
    {
      DestroyImages(image);
      return((Image *) NULL);
    }
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
  (void) RegisterMagickInfo(entry);
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
  (void) UnregisterMagickInfo("AVI");
}
