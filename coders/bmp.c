/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            BBBB   M   M  PPPP                               %
%                            B   B  MM MM  P   P                              %
%                            BBBB   M M M  PPPP                               %
%                            B   B  M   M  P                                  %
%                            BBBB   M   M  P                                  %
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
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "define.h"

/*
  Typedef declarations.
*/
typedef struct _BMPInfo
{
  unsigned long
    file_size,
    ba_offset,
    offset_bits,
    size;

  long
    width,
    height;

  unsigned short
    planes,
    bits_per_pixel;

  unsigned long
    compression,
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    red_mask,
    green_mask,
    blue_mask,
    alpha_mask,
    colors_important;

  long
    intent,
    colorspace;

  PointInfo
    red_primary,
    green_primary,
    blue_primary,
    gamma_scale;
} BMPInfo;

/*
  Forward declarations.
*/
static unsigned int
  WriteBMPImage(const ImageInfo *,Image *);

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
%      unsigned int DecodeImage(Image *image,const unsigned long compression,
%        unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o compression:  Zero means uncompressed.  A value of 1 means the
%      compressed pixels are runlength encoded for a 256-color bitmap.
%      A value of 2 means a 16-color bitmap.  A value of 3 means bitfields
%      encoding.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,const unsigned long compression,
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
            *q++=(unsigned char)
              ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
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
        switch ((int) count)
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
                  *q++=(unsigned char)
                    ((i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
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
%   E n c o d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method EncodeImage compresses pixels using a runlength encoded format.
%
%  The format of the EncodeImage method is:
%
%    static unsigned int EncodeImage(Image *image,
%      const unsigned long bytes_per_line,const unsigned char *pixels,
%      unsigned char *compressed_pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method EncodeImage returns the number of bytes in the
%      runlength encoded compress_pixels array.
%
%    o image:  A pointer to a Image structure.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the compression process.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%
*/
static size_t EncodeImage(Image *image,const unsigned long bytes_per_line,
  const unsigned char *pixels,unsigned char *compressed_pixels)
{
  long
    y;

  register const unsigned char
    *p;

  register long
    i,
    x;

  register unsigned char
    *q;

  /*
    Runlength encode pixels.
  */
  assert(image != (Image *) NULL);
  assert(pixels != (const unsigned char *) NULL);
  assert(compressed_pixels != (unsigned char *) NULL);
  p=pixels;
  q=compressed_pixels;
  i=0;
  for (y=0; y < (long) image->rows; y++)
  {
    for (x=0; x < (long) bytes_per_line; x+=i)
    {
      /*
        Determine runlength.
      */
      for (i=1; ((x+i) < (long) bytes_per_line); i++)
        if ((*(p+i) != *p) || (i == 255))
          break;
      *q++=(unsigned char) i;
      *q++=(*p);
      p+=i;
    }
    /*
      End of line.
    */
    *q++=0x00;
    *q++=0x00;
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  /*
    End of bitmap.
  */
  *q++=0;
  *q++=0x01;
  return(q-compressed_pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s B M P                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsBMP returns True if the image format type, identified by the
%  magick string, is BMP.
%
%  The format of the IsBMP method is:
%
%      unsigned int IsBMP(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsBMP returns True if the image format type is BMP.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsBMP(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (LocaleNCompare((char *) magick,"BA",2) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"BM",2) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"IC",2) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"PI",2) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"CI",2) == 0)
    return(True);
  if (LocaleNCompare((char *) magick,"CP",2) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d B M P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBMPImage reads a Microsoft Windows bitmap image file, Version
%  2, 3 (for Windows or NT), or 4, and  returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.  It currently only handles masks RGB555, RGB565, RGB8880, and
%  RGBA8888.
%
%  The format of the ReadBMPImage method is:
%
%      image=ReadBMPImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadBMPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadBMPImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  BMPInfo
    bmp_info;

  Image
    *image;

  IndexPacket
    index;

  long
    bit,
    y;

  unsigned long
    blue,
    green,
    opacity,
    red;

  off_t
    start_position;

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

  size_t
    count,
    length;

  unsigned char
    magick[12],
    *pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a BMP file.
  */
  (void) memset(&bmp_info,0,sizeof(BMPInfo));
  bmp_info.ba_offset=0;
  start_position=0;
  count=ReadBlob(image,2,(char *) magick);
  do
  {
    PixelPacket
      quantum_bits,
      shift;

    /*
      Verify BMP identifier.
    */
    if (bmp_info.ba_offset == 0)
      start_position=TellBlob(image)-2;
    bmp_info.ba_offset=0;
    while (LocaleNCompare((char *) magick,"BA",2) == 0)
    {
      bmp_info.file_size=ReadBlobLSBLong(image);
      bmp_info.ba_offset=ReadBlobLSBLong(image);
      bmp_info.offset_bits=ReadBlobLSBLong(image);
      count=ReadBlob(image,2,(char *) magick);
    }
    if ((count == 0) || ((LocaleNCompare((char *) magick,"BM",2) != 0) &&
        (LocaleNCompare((char *) magick,"CI",2) != 0)))
      ThrowReaderException(CorruptImageWarning,"Not a BMP image file",image);
    bmp_info.file_size=ReadBlobLSBLong(image);
    (void) ReadBlobLSBLong(image);
    bmp_info.offset_bits=ReadBlobLSBLong(image);
    bmp_info.size=ReadBlobLSBLong(image);
    if (bmp_info.size == 12)
      {
        /*
          OS/2 BMP image file.
        */
        bmp_info.width=(short) ReadBlobLSBShort(image);
        bmp_info.height=(short) ReadBlobLSBShort(image);
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.x_pixels=0;
        bmp_info.y_pixels=0;
        bmp_info.number_colors=0;
        bmp_info.compression=0;
        bmp_info.image_size=0;
        bmp_info.alpha_mask=0;
      }
    else
      {
        /*
          Microsoft Windows BMP image file.
        */
        bmp_info.width=(short) ReadBlobLSBLong(image);
        bmp_info.height=(short) ReadBlobLSBLong(image);
        bmp_info.planes=ReadBlobLSBShort(image);
        bmp_info.bits_per_pixel=ReadBlobLSBShort(image);
        bmp_info.compression=ReadBlobLSBLong(image);
        bmp_info.image_size=ReadBlobLSBLong(image);
        bmp_info.x_pixels=ReadBlobLSBLong(image);
        bmp_info.y_pixels=ReadBlobLSBLong(image);
        bmp_info.number_colors=ReadBlobLSBLong(image);
        bmp_info.colors_important=ReadBlobLSBLong(image);
        for (i=0; i < (long) (bmp_info.size-40); i++)
          (void) ReadBlobByte(image);
        if ((bmp_info.compression == 3) && ((bmp_info.bits_per_pixel == 16) ||
            (bmp_info.bits_per_pixel == 32)))
          {
            bmp_info.red_mask=ReadBlobLSBLong(image);
            bmp_info.green_mask=ReadBlobLSBLong(image);
            bmp_info.blue_mask=ReadBlobLSBLong(image);
            if (bmp_info.size > 40)
              {
                /*
                  Read color management information.
                */
                bmp_info.alpha_mask=ReadBlobLSBLong(image);
                bmp_info.colorspace=(long) ReadBlobLSBLong(image);
                bmp_info.red_primary.x=ReadBlobLSBLong(image);
                /*
                  The primaries are formatted in 2^30 fixed point.
                */
                bmp_info.red_primary.y=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.red_primary.z=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.green_primary.x=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.green_primary.y=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.green_primary.z=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.blue_primary.x=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.blue_primary.y=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                bmp_info.blue_primary.z=(double)
                  ReadBlobLSBLong(image)/0x3ffffff;
                /*
                  The gamma_scales are formatted in 16^16 fixed point.
                */
                bmp_info.gamma_scale.x=(double) ReadBlobLSBLong(image)/0xffff;
                bmp_info.gamma_scale.y=(double) ReadBlobLSBLong(image)/0xffff;
                bmp_info.gamma_scale.z=(double) ReadBlobLSBLong(image)/0xffff;
              }
            if (bmp_info.size > 108)
              {
                /*
                  Skip BMP Version 5 color management information.
                */
                for (i=0; i < 4; i++)
                  ReadBlobLSBLong(image);
                (void) ReadBlobByte(image);
              }
            if (bmp_info.size > 124)
              {
                /*
                  Skip future BMP Version information.
                */
                for (i=124; i < bmp_info.size; i++)
                  ReadBlobByte(image);
              }
            }
      }
    switch (bmp_info.compression)
    {
      case 0:
      case 1:
      case 2:
      case 3:
        break;
      case 4:
        ThrowReaderException(CorruptImageWarning,
          "JPEG compression not supported",image);
      case 5:
        ThrowReaderException(CorruptImageWarning,
          "PNG compression not supported",image);
      default:
        ThrowReaderException(CorruptImageWarning,
          "Unrecognized compression method",image);
    }
    image->columns=bmp_info.width;
    image->rows=AbsoluteValue(bmp_info.height);
    image->depth=8;
    image->matte=bmp_info.alpha_mask != 0;
    if ((bmp_info.number_colors != 0) || (bmp_info.bits_per_pixel < 16))
      {
        image->storage_class=PseudoClass;
        image->colors=bmp_info.number_colors;
        if (image->colors == 0)
          image->colors=1L << bmp_info.bits_per_pixel;
      }
    if (image->storage_class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        unsigned int
          packet_size;

        /*
          Read BMP raster colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        bmp_colormap=(unsigned char *) AcquireMemory(4*image->colors);
        if (bmp_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        if ((bmp_info.size == 12) || (bmp_info.size == 64))
          packet_size=3;
        else
          packet_size=4;
        (void) ReadBlob(image,packet_size*image->colors,(char *) bmp_colormap);
        p=bmp_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          image->colormap[i].blue=Upscale(*p++);
          image->colormap[i].green=Upscale(*p++);
          image->colormap[i].red=Upscale(*p++);
          if (packet_size == 4)
            p++;
        }
        LiberateMemory((void **) &bmp_colormap);
      }
    if (image_info->ping && (image_info->subrange != 0))
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Read image data.
    */
    (void) SeekBlob(image,start_position+bmp_info.offset_bits,SEEK_SET);
    if (bmp_info.compression == 2)
      bmp_info.bits_per_pixel<<=1;
    bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
    length=bytes_per_line*image->rows;
    pixels=(unsigned char *) AcquireMemory(length);
    if (pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    if ((bmp_info.compression == 0) || (bmp_info.compression == 3))
      (void) ReadBlob(image,length,(char *) pixels);
    else
      {
        /*
          Convert run-length encoded raster pixels.
        */
        status=DecodeImage(image,bmp_info.compression,pixels);
        if (status == False)
          ThrowReaderException(CorruptImageWarning,"runlength decoding failed",
            image);
      }
    /*
      Initialize image structure.
    */
    image->units=PixelsPerCentimeterResolution;
    image->x_resolution=bmp_info.x_pixels/100.0;
    image->y_resolution=bmp_info.y_pixels/100.0;
    /*
      Convert BMP raster image to pixel packets.
    */
    if (bmp_info.compression == 0)
      {
        bmp_info.alpha_mask=0;
        bmp_info.red_mask=0x00ff0000L;
        bmp_info.green_mask=0x0000ff00L;
        bmp_info.blue_mask=0x000000ffL;
        if (bmp_info.bits_per_pixel == 16)
          {
            /*
              RGB555.
            */
            bmp_info.red_mask=0x00007c00L;
            bmp_info.green_mask=0x000003e0L;
            bmp_info.blue_mask=0x0000001fL;
          }
      }
    if ((bmp_info.bits_per_pixel == 16) || (bmp_info.bits_per_pixel == 32))
      {
        /*
          Get shift and quantum bits info from bitfield masks.
        */
        memset(&shift,0,sizeof(PixelPacket));
        memset(&quantum_bits,0,sizeof(PixelPacket));
        if (bmp_info.red_mask != 0)
          while (((bmp_info.red_mask << shift.red) & 0x80000000L) == 0)
            shift.red++;
        if (bmp_info.green_mask != 0)
          while (((bmp_info.green_mask << shift.green) & 0x80000000L) == 0)
            shift.green++;
        if (bmp_info.blue_mask != 0)
          while (((bmp_info.blue_mask << shift.blue) & 0x80000000L) == 0)
            shift.blue++;
        if (bmp_info.alpha_mask != 0)
          while (((bmp_info.alpha_mask << shift.opacity) & 0x80000000L) == 0)
            shift.opacity++;
        i=shift.red;
        while (((bmp_info.red_mask << i) & 0x80000000L) != 0)
          i++;
        quantum_bits.red=(Quantum) (i-shift.red);
        i=shift.green;
        while (((bmp_info.green_mask << i) & 0x80000000L) != 0)
          i++;
        quantum_bits.green=(Quantum) (i-shift.green);
        i=shift.blue;
        while (((bmp_info.blue_mask << i) & 0x80000000L) != 0)
          i++;
        quantum_bits.blue=(Quantum) (i-shift.blue);
        i=shift.opacity;
        while (((bmp_info.alpha_mask << i) & 0x80000000L) != 0)
          i++;
        quantum_bits.opacity=(Quantum) (i-shift.opacity);
      }
    switch (bmp_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        for (y=(long) image->rows-1; y >= 0; y--)
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
              index=((*p) & (0x80 >> bit)) ? 0x00 : 0x01;
              indexes[x+bit]=index;
              *q++=image->colormap[index];
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (long) (image->columns % 8); bit++)
              {
                index=((*p) & (0x80 >> bit)) ? 0x00 : 0x01;
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
        for (y=(long) image->rows-1; y >= 0; y--)
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
        if ((bmp_info.compression == 1) || (bmp_info.compression == 2))
          bytes_per_line=image->columns;
        for (y=(long) image->rows-1; y >= 0; y--)
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
        unsigned long
          pixel;

        /*
          Convert bitfield encoded 16-bit PseudoColor scanline.
        */
        if (bmp_info.compression != 0 && bmp_info.compression != 3)
          ThrowReaderException(CorruptImageWarning,
            "Compression mode != 0 or 3 in 16-bit BMP image file",image)
        bytes_per_line=2*(image->columns+image->columns%2);
        image->storage_class=DirectClass;
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=(*p++);
            pixel|=(*p++) << 8;
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            opacity=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            q->red=XDownscale(red);
            if (quantum_bits.red == 5)
              q->red|=(q->red >> 5);
            q->green=XDownscale(green);
            if (quantum_bits.green == 5)
              q->green|=(q->green >> 5);
            if (quantum_bits.green == 6)
              q->green|=(q->green >> 6);
            q->blue=XDownscale(blue);
            if (quantum_bits.blue == 5)
              q->blue|=(q->blue >> 5);
            if (image->matte)
              q->opacity=XDownscale(opacity);
#if QuantumDepth == 16
            if (quantum_bits.blue <= 8)
              q->blue|=((q->blue & 0xff00) >> 8);
            if (quantum_bits.green <= 8)
              q->green|=((q->green & 0xff00) >> 8);
            if (quantum_bits.red <= 8)
              q->red|=((q->red & 0xff00) >> 8);
            if (quantum_bits.opacity <= 8)
              q->opacity|=((q->opacity & 0xff00) >> 8);
#endif
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
      {
        /*
          Convert DirectColor scanline.
        */
        bytes_per_line=4*((image->columns*24+31)/32);
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            q->blue=Upscale(*p++);
            q->green=Upscale(*p++);
            q->red=Upscale(*p++);
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
      case 32:
      {
        /*
          Convert bitfield encoded DirectColor scanline.
        */
        if (bmp_info.compression != 0 && bmp_info.compression != 3)
          ThrowReaderException(CorruptImageWarning,
            "Compression mode != 0 or 3 in 32-bit BMP image file",image)
        bytes_per_line=4*(image->columns);
        for (y=(long) image->rows-1; y >= 0; y--)
        {
          unsigned long
            pixel;

          p=pixels+(image->rows-y-1)*bytes_per_line;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=(*p++);
            pixel|=(*p++ << 8);
            pixel|=(*p++ << 16);
            pixel|=(*p++ << 24);
            red=((pixel & bmp_info.red_mask) << shift.red) >> 16;
            green=((pixel & bmp_info.green_mask) << shift.green) >> 16;
            blue=((pixel & bmp_info.blue_mask) << shift.blue) >> 16;
            opacity=((pixel & bmp_info.alpha_mask) << shift.opacity) >> 16;
            q->red=XDownscale(red);
            q->green=XDownscale(green);
            q->blue=XDownscale(blue);
            if (image->matte)
              q->opacity=XDownscale(opacity);
#if QuantumDepth == 16
            if (quantum_bits.red == 8)
              q->red|=(q->red >> 8);
            if (quantum_bits.green == 8)
              q->green|=(q->green >> 8);
            if (quantum_bits.blue == 8)
              q->blue|=(q->blue >> 8);
            if (quantum_bits.opacity == 8)
              q->opacity|=(q->opacity >> 8);
#endif
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
        ThrowReaderException(CorruptImageWarning,"Not a BMP image file",image)
    }
    LiberateMemory((void **) &pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    if (bmp_info.height < 0)
      {
        Image
          *flipped_image;

        /*
          Correct image orientation.
        */
        flipped_image=FlipImage(image,exception);
        if (flipped_image == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        DestroyImage(image);
        image=flipped_image;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    *magick='\0';
    if (bmp_info.ba_offset != 0)
      (void) SeekBlob(image,bmp_info.ba_offset,SEEK_SET);
    (void) ReadBlob(image,2,(char *) magick);
    if (IsBMP(magick,2))
      {
        /*
          Acquire next image structure.
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
  } while (IsBMP(magick,2));
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
%   R e g i s t e r B M P I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterBMPImage adds attributes for the BMP image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterBMPImage method is:
%
%      RegisterBMPImage(void)
%
*/
ModuleExport void RegisterBMPImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("BMP");
  entry->decoder=ReadBMPImage;
  entry->encoder=WriteBMPImage;
  entry->magick=IsBMP;
  entry->description=AllocateString("Microsoft Windows bitmap image");
  entry->module=AllocateString("BMP");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r B M P I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterBMPImage removes format registrations made by the
%  BMP module from the list of supported formats.
%
%  The format of the UnregisterBMPImage method is:
%
%      UnregisterBMPImage(void)
%
*/
ModuleExport void UnregisterBMPImage(void)
{
  (void) UnregisterMagickInfo("BMP");
  (void) UnregisterMagickInfo("DIB");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e B M P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBMPImage writes an image in Microsoft Windows bitmap encoded
%  image format, version 3 for Windows or (if the image has a matte channel)
%  version 4.
%
%  The format of the WriteBMPImage method is:
%
%      unsigned int WriteBMPImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteBMPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteBMPImage(const ImageInfo *image_info,Image *image)
{
  BMPInfo
    bmp_info;

  long
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register unsigned char
    *q;

  unsigned char
    *bmp_data,
    *pixels;

  unsigned int
    scene,
    status;

  unsigned long
    bytes_per_line;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize BMP raster file header.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    bmp_info.file_size=14+40;
    bmp_info.offset_bits=14+40;
    bmp_info.compression=0;
    if (image->storage_class != DirectClass)
      {
        /*
          Colormapped BMP raster.
        */
        bmp_info.bits_per_pixel=8;
        if (IsMonochromeImage(image,&image->exception))
          bmp_info.bits_per_pixel=1;
        bmp_info.number_colors=1 << bmp_info.bits_per_pixel;
        if (image->matte)
          SetImageType(image,TrueColorMatteType);
        else
          if (bmp_info.number_colors < image->colors)
            SetImageType(image, TrueColorType);
          else
            {
              bmp_info.file_size+=4*(1 << bmp_info.bits_per_pixel);
              bmp_info.offset_bits+=4*(1 << bmp_info.bits_per_pixel);
            }
      }
    if (image->storage_class == DirectClass)
      {
        /*
          Full color BMP raster.
        */
        bmp_info.number_colors=0;
        bmp_info.bits_per_pixel=image->matte ? 32 : 24;
        bmp_info.compression=image->matte ? 3 : 0;
      }
    bytes_per_line=4*((image->columns*bmp_info.bits_per_pixel+31)/32);
    bmp_info.ba_offset=0;
    if (!image->matte)
      bmp_info.size=40;
    else
      {
        bmp_info.size=108;
        bmp_info.file_size+=68;
        bmp_info.offset_bits+=68;
      }
    bmp_info.width=(long) image->columns;
    bmp_info.height=(long) image->rows;
    bmp_info.planes=1;
    bmp_info.image_size=bytes_per_line*image->rows;
    bmp_info.file_size+=bmp_info.image_size;
    bmp_info.x_pixels=75*39;
    bmp_info.y_pixels=75*39;
    if (image->units == PixelsPerInchResolution)
      {
        bmp_info.x_pixels=(unsigned long) (100.0*image->x_resolution/2.54);
        bmp_info.y_pixels=(unsigned long) (100.0*image->y_resolution/2.54);
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        bmp_info.x_pixels=(unsigned long) (100.0*image->x_resolution);
        bmp_info.y_pixels=(unsigned long) (100.0*image->y_resolution);
      }
    bmp_info.colors_important=bmp_info.number_colors;
    /*
      Convert MIFF to BMP raster pixels.
    */
    pixels=(unsigned char *) AcquireMemory(bmp_info.image_size);
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    switch (bmp_info.bits_per_pixel)
    {
      case 1:
      {
        register unsigned char
          bit,
          byte,
          polarity;

        /*
          Convert PseudoClass image to a BMP monochrome image.
        */
        polarity=Intensity(&image->colormap[0]) < (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(&image->colormap[1]) < Intensity(&image->colormap[0]);
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          q=pixels+(image->rows-y-1)*bytes_per_line;
          bit=0;
          byte=0;
          for (x=0; x < (long) image->columns; x++)
          {
            byte<<=1;
            if (indexes[x] == polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
             p++;
           }
         if (bit != 0)
           *q++=byte << (8-bit);
         if (image->previous == (Image *) NULL)
           if (QuantumTick(y,image->rows))
             MagickMonitor(SaveImageText,y,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoClass packet to BMP pixel.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          q=pixels+(image->rows-y-1)*bytes_per_line;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=indexes[x];
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        break;
      }
      case 24:
      case 32:
      {
        /*
          Convert DirectClass packet to BMP BGR888 or BGRA8888 pixel.
        */
        for (y=0; y < (long) image->rows; y++)
        {
          p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
          if (p == (const PixelPacket *) NULL)
            break;
          q=pixels+(image->rows-y-1)*bytes_per_line;
          for (x=0; x < (long) image->columns; x++)
          {
            *q++=Downscale(p->blue);
            *q++=Downscale(p->green);
            *q++=Downscale(p->red);
            if (bmp_info.bits_per_pixel == 32)
              *q++=Downscale(p->opacity);
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        break;
      }
    }
    if (bmp_info.bits_per_pixel == 8)
      if (image_info->compression != NoCompression)
        {
          size_t
            length;

          /*
            Convert run-length encoded raster pixels.
          */
          length=2*(bytes_per_line+2)*(image->rows+2)+2;
          bmp_data=(unsigned char *) AcquireMemory(length);
          if (pixels == (unsigned char *) NULL)
            {
              LiberateMemory((void **) &pixels);
              ThrowWriterException(ResourceLimitWarning,
                "Memory allocation failed",image)
            }
          bmp_info.file_size-=bmp_info.image_size;
          bmp_info.image_size=EncodeImage(image,bytes_per_line,pixels,bmp_data);
          bmp_info.file_size+=bmp_info.image_size;
          LiberateMemory((void **) &pixels);
          pixels=bmp_data;
          bmp_info.compression=1;
        }
    /*
      Write BMP Version 3 for Windows 3.x 14-byte header.
    */
    (void) WriteBlob(image,2,"BM");
    (void) WriteBlobLSBLong(image,bmp_info.file_size);
    (void) WriteBlobLSBLong(image,bmp_info.ba_offset);
    (void) WriteBlobLSBLong(image,bmp_info.offset_bits);
    /*
      Write 40-byte bitmap header.
    */
    (void) WriteBlobLSBLong(image,bmp_info.size);
    (void) WriteBlobLSBLong(image,bmp_info.width);
    (void) WriteBlobLSBLong(image,bmp_info.height);
    (void) WriteBlobLSBShort(image,bmp_info.planes);
    (void) WriteBlobLSBShort(image,bmp_info.bits_per_pixel);
    (void) WriteBlobLSBLong(image,bmp_info.compression);
    (void) WriteBlobLSBLong(image,bmp_info.image_size);
    (void) WriteBlobLSBLong(image,bmp_info.x_pixels);
    (void) WriteBlobLSBLong(image,bmp_info.y_pixels);
    (void) WriteBlobLSBLong(image,bmp_info.number_colors);
    (void) WriteBlobLSBLong(image,bmp_info.colors_important);
    if (image->matte)
      {
        int
          j;

        /*
          Write the rest of the 108-byte BMP Version 4 header
        */
        (void) WriteBlobLSBLong(image,0x00ff0000L);  /* Red mask */
        (void) WriteBlobLSBLong(image,0x0000ff00L);  /* Green mask */
        (void) WriteBlobLSBLong(image,0x000000ffL);  /* Blue mask */
        (void) WriteBlobLSBLong(image,0xff000000L);  /* Alpha mask */
        (void) WriteBlobLSBLong(image,0x00000001L);  /* CSType==RGB */
        for (j=0; j < 9; j++)
          (void) WriteBlobLSBLong(image,0x00000000L);  /* Unused RedX etc. */
        for (j=0; j < 3; j++)
          (void) WriteBlobLSBLong(image,0x00000000L);  /* 3 comp. gamma scale */
      }
    if (image->storage_class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        /*
          Dump colormap to file.
        */
        bmp_colormap=(unsigned char *)
          AcquireMemory(4*(1 << bmp_info.bits_per_pixel));
        if (bmp_colormap == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
        q=bmp_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          *q++=Downscale(image->colormap[i].blue);
          *q++=Downscale(image->colormap[i].green);
          *q++=Downscale(image->colormap[i].red);
          *q++=(Quantum) 0x0;
        }
        for ( ; i < (1L << bmp_info.bits_per_pixel); i++)
        {
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
        }
        (void) WriteBlob(image,4*(1 << bmp_info.bits_per_pixel),
          (char *) bmp_colormap);
        LiberateMemory((void **) &bmp_colormap);
      }
    (void) WriteBlob(image,bmp_info.image_size,(char *) pixels);
    LiberateMemory((void **) &pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
