/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                   DDDD   EEEEE   CCCC   OOO   DDDD   EEEEE                  %
%                   D   D  E      C      O   O  D   D  E                      %
%                   D   D  EEE    C      O   O  D   D  EEE                    %
%                   D   D  E      C      O   O  D   D  E                      %
%                   DDDD   EEEEE   CCCC   OOO   DDDD   EEEEE                  %
%                                                                             %
%                                                                             %
%                        Methods to Read Image Formats                        %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1992                                   %
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
%  Methods in this library convert to and from `alien' image formats to the
%  MIFF image format.
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "proxy.h"

/*
  Define declarations.
*/
#define LoadImageText  "  Loading image...  "
#define LoadImagesText  "  Loading images...  "
#define PrematureExit(warning,message,image) \
{ \
  MagickWarning(warning,message,image->filename); \
  DestroyImages(image); \
  return((Image *) NULL); \
}
#define RenderPostscriptText  "  Rendering postscript...  "

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A V S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadAVSImage reads an AVS X image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadAVSImage routine is:
%
%      image=ReadAVSImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadAVSImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadAVSImage(const ImageInfo *image_info)
{
  Image
    *image;

  Quantum
    blue,
    green,
    red;

  register int
    x,
    y;

  register long
    packets;

  register RunlengthPacket
    *q;

  unsigned long
    height,
    max_packets,
    width;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read AVS image.
  */
  width=MSBFirstReadLong(image->file);
  height=MSBFirstReadLong(image->file);
  if ((width == (unsigned long) ~0) || (height == (unsigned long) ~0))
    PrematureExit(CorruptImageWarning,"Not a AVS image file",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert AVS raster image to runlength-encoded packets.
    */
    q=image->pixels;
    SetRunlengthEncoder(q);
    for (y=0; y < (int) image->rows; y++)
    {
      for (x=0; x < (int) image->columns; x++)
      {
        index=UpScale(fgetc(image->file));
        image->matte|=index != Transparent;
        red=UpScale(fgetc(image->file));
        green=UpScale(fgetc(image->file));
        blue=UpScale(fgetc(image->file));
        if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
            (index == q->index) && ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,max_packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  PrematureExit(ResourceLimitWarning,
                    "Memory allocation failed",image);
                q=image->pixels+packets-1;
              }
            q->red=red;
            q->green=green;
            q->blue=blue;
            q->index=index;
            q->length=0;
          }
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    SetRunlengthPackets(image,packets);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    width=MSBFirstReadLong(image->file);
    height=MSBFirstReadLong(image->file);
    if ((width != (unsigned long) ~0) && (height != (unsigned long) ~0))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while ((width != (unsigned long) ~0) && (height != (unsigned long) ~0));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
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
%  Method ReadBMPImage reads a Microsoft Windows bitmap image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadBMPImage routine is:
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
%
*/
Image *ReadBMPImage(const ImageInfo *image_info)
{
  typedef struct _BMPHeader
  {
    unsigned long
      file_size;

    unsigned short
      reserved[2];

    unsigned long
      offset_bits,
      size,
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
      colors_important;

    unsigned short
      red_mask,
      green_mask,
      blue_mask,
      alpha_mask;

    long
      colorspace;

    PointInfo
      red_primary,
      green_primary,
      blue_primary,
      gamma_scale;
  } BMPHeader;

  BMPHeader
    bmp_header;

  Image
    *image;

  long
    start_position;

  register int
    bit,
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *bmp_pixels,
    magick[12];

  unsigned int
    bytes_per_line,
    image_size,
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a BMP file.
  */
  status=ReadData((char *) magick,1,2,image->file);
  do
  {
    /*
      Verify BMP identifier.
    */
    start_position=ftell(image->file)-2;
    if ((status == False) || (strncmp((char *) magick,"BM",2) != 0))
      PrematureExit(CorruptImageWarning,"Not a BMP image file",image);
    bmp_header.file_size=LSBFirstReadLong(image->file);
    bmp_header.reserved[0]=LSBFirstReadShort(image->file);
    bmp_header.reserved[1]=LSBFirstReadShort(image->file);
    bmp_header.offset_bits=LSBFirstReadLong(image->file);
    bmp_header.size=LSBFirstReadLong(image->file);
    if (image->filesize)
      if ((int) (bmp_header.file_size-bmp_header.size) > image->filesize)
        PrematureExit(CorruptImageWarning,"Not a BMP image file",image);
    if (bmp_header.size == 12)
      {
        /*
          OS/2 BMP image file.
        */
        bmp_header.width=(unsigned long) LSBFirstReadShort(image->file);
        bmp_header.height=(unsigned long) LSBFirstReadShort(image->file);
        bmp_header.planes=LSBFirstReadShort(image->file);
        bmp_header.bits_per_pixel=LSBFirstReadShort(image->file);
        bmp_header.x_pixels=0;
        bmp_header.y_pixels=0;
        bmp_header.number_colors=0;
        bmp_header.compression=0;
        bmp_header.image_size=0;
      }
    else
      {
        /*
          Microsoft Windows BMP image file.
        */
        bmp_header.width=LSBFirstReadLong(image->file);
        bmp_header.height=LSBFirstReadLong(image->file);
        bmp_header.planes=LSBFirstReadShort(image->file);
        bmp_header.bits_per_pixel=LSBFirstReadShort(image->file);
        bmp_header.compression=LSBFirstReadLong(image->file);
        bmp_header.image_size=LSBFirstReadLong(image->file);
        bmp_header.x_pixels=LSBFirstReadLong(image->file);
        bmp_header.y_pixels=LSBFirstReadLong(image->file);
        bmp_header.number_colors=LSBFirstReadLong(image->file);
        bmp_header.colors_important=LSBFirstReadLong(image->file);
        for (i=0; i < ((int) bmp_header.size-40); i++)
          (void) fgetc(image->file);
        if ((bmp_header.compression == 3) &&
            ((bmp_header.bits_per_pixel == 16) ||
             (bmp_header.bits_per_pixel == 32)))
          {
            bmp_header.red_mask=LSBFirstReadShort(image->file);
            bmp_header.green_mask=LSBFirstReadShort(image->file);
            bmp_header.blue_mask=LSBFirstReadShort(image->file);
            if (bmp_header.size > 40)
              {
                /*
                  Read color management information.
                */
                bmp_header.alpha_mask=LSBFirstReadShort(image->file);
                bmp_header.colorspace=LSBFirstReadLong(image->file);
                bmp_header.red_primary.x=LSBFirstReadLong(image->file);
                bmp_header.red_primary.y=LSBFirstReadLong(image->file);
                bmp_header.red_primary.z=LSBFirstReadLong(image->file);
                bmp_header.green_primary.x=LSBFirstReadLong(image->file);
                bmp_header.green_primary.y=LSBFirstReadLong(image->file);
                bmp_header.green_primary.z=LSBFirstReadLong(image->file);
                bmp_header.blue_primary.x=LSBFirstReadLong(image->file);
                bmp_header.blue_primary.y=LSBFirstReadLong(image->file);
                bmp_header.blue_primary.z=LSBFirstReadLong(image->file);
                bmp_header.gamma_scale.x=LSBFirstReadShort(image->file);
                bmp_header.gamma_scale.y=LSBFirstReadShort(image->file);
                bmp_header.gamma_scale.z=LSBFirstReadShort(image->file);
              }
          }
      }
    image->matte=bmp_header.bits_per_pixel == 32;
    image->columns=(unsigned int) bmp_header.width;
    image->rows=(unsigned int) bmp_header.height;
    if ((bmp_header.number_colors != 0) || (bmp_header.bits_per_pixel < 16))
      {
        image->class=PseudoClass;
        image->colors=(unsigned int) bmp_header.number_colors;
        if (image->colors == 0)
          image->colors=1 << bmp_header.bits_per_pixel;
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    if (image->class == PseudoClass)
      {
        /*
          Allocate image colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
          image->colormap[i].green=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
          image->colormap[i].blue=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
        }
        if (bmp_header.bits_per_pixel < 16)
          {
            unsigned char
              *bmp_colormap;

            unsigned int
              packet_size;

            /*
              Read BMP raster colormap.
            */
            bmp_colormap=(unsigned char *)
              AllocateMemory(4*image->colors*sizeof(unsigned char));
            if (bmp_colormap == (unsigned char *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            packet_size=4;
            if (bmp_header.size == 12)
              packet_size=3;
            (void) ReadData((char *) bmp_colormap,packet_size,image->colors,
              image->file);
            p=bmp_colormap;
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].blue=UpScale(*p++);
              image->colormap[i].green=UpScale(*p++);
              image->colormap[i].red=UpScale(*p++);
              if (bmp_header.size != 12)
                p++;
            }
            FreeMemory((char *) bmp_colormap);
          }
      }
    while (ftell(image->file) < (int) (start_position+bmp_header.offset_bits))
      (void) fgetc(image->file);
    /*
      Read image data.
    */
    if (bmp_header.compression == 2)
      bmp_header.bits_per_pixel<<=1;
    bytes_per_line=((image->columns*bmp_header.bits_per_pixel+31)/32)*4;
    image_size=bytes_per_line*bmp_header.height;
    bmp_pixels=(unsigned char *)
      AllocateMemory(image_size*sizeof(unsigned char));
    if (bmp_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if ((bmp_header.compression == 0) || (bmp_header.compression == 3))
      (void) ReadData((char *) bmp_pixels,1,(unsigned int) image_size,
        image->file);
    else
      {
        /*
          Convert run-length encoded raster pixels.
        */
        (void) BMPDecodeImage(image->file,(unsigned int) bmp_header.compression,
          (unsigned int) bmp_header.width,(unsigned int) bmp_header.height,
          bmp_pixels);
      }
    /*
      Initialize image structure.
    */
    image->columns=(unsigned int) bmp_header.width;
    image->rows=(unsigned int) bmp_header.height;
    image->units=PixelsPerCentimeterResolution;
    image->x_resolution=bmp_header.x_pixels/100.0;
    image->y_resolution=bmp_header.y_pixels/100.0;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert BMP raster image to runlength-encoded packets.
    */
    switch (bmp_header.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=bmp_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < ((int) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
            {
              q->index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
              q->length=0;
              q++;
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (int) (image->columns % 8); bit++)
              {
                q->index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                q->length=0;
                q++;
              }
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 4:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=bmp_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < ((int) image->columns-1); x+=2)
          {
            q->index=(*p >> 4) & 0xf;
            q->length=0;
            q++;
            q->index=(*p) & 0xf;
            q->length=0;
            p++;
            q++;
          }
          if ((image->columns % 2) != 0)
            {
              q->index=(*p >> 4) & 0xf;
              q->length=0;
              q++;
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        if ((bmp_header.compression == 1) || (bmp_header.compression == 2))
          bytes_per_line=image->columns;
        for (y=image->rows-1; y >= 0; y--)
        {
          p=bmp_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 16:
      {
        unsigned char
          h,
          l;

        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        if (bmp_header.compression == 1)
          bytes_per_line=image->columns << 1;
        for (y=image->rows-1; y >= 0; y--)
        {
          p=bmp_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            h=(*p++);
            l=(*p++);
            q->red=(Quantum) ((MaxRGB*((int) (l & 0x7c) >> 2))/31);
            q->green=(Quantum)
              ((MaxRGB*(((int) (l & 0x03) << 3)+((int) (h & 0xe0) >> 5)))/31);
            q->blue=(Quantum) ((MaxRGB*((int) (h & 0x1f)))/31);
            q->index=0;
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 24:
      case 32:
      {
        /*
          Convert DirectColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=bmp_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->blue=UpScale(*p++);
            q->green=UpScale(*p++);
            q->red=UpScale(*p++);
            q->index=0;
            if (image->matte)
              q->index=Opaque-UpScale(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Not a BMP image file",image);
    }
    FreeMemory((char *) bmp_pixels);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    status=ReadData((char *) magick,1,2,image->file);
    if ((status == True) && (strncmp((char *) magick,"BM",2) == 0))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while ((status == True) && (strncmp((char *) magick,"BM",2) == 0));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C M Y K I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCMYKImage reads an image of raw cyan, magenta, yellow, and
%  black bytes and returns it.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  The format of the ReadCMYKImage routine is:
%
%      image=ReadCMYKImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCMYKImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadCMYKImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    count,
    y;

  register int
    i,
    x;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  unsigned int
    packet_size;

  unsigned short
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      OpenImage(image_info,image,ReadBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) fgetc(image->file);
    }
  /*
    Allocate memory for a scanline.
  */
  packet_size=4*(QuantumDepth >> 3);
  scanline=(unsigned char *)
    AllocateMemory(packet_size*image->tile_info.width*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (int) image->rows; i++)
        (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
          image->file);
    }
  do
  {
    /*
      Initialize image structure.
    */
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert raster image to runlength-encoded packets.
    */
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  CMYKCMYKCMYKCMYKCMYK...
        */
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        q=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            ReadQuantum(q->green,p);
            ReadQuantum(q->blue,p);
            ReadQuantum(q->index,p);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  CCC...MMM...YYY...KKK...CCC...MMM...YYY...KKK...
        */
        packet_size=image->depth >> 3;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            q->length=0;
            q++;
          }
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->green,p);
            q++;
          }
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->blue,p);
            q++;
          }
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->index,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        break;
      }
      case PlaneInterlace:
      case PartitionInterlace:
      {
        /*
          Plane interlacing:  CCCCCC...MMMMMM...YYYYYY...KKKKKK...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("C",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        packet_size=image->depth >> 3;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        i=0;
        q=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->rows << 2))
              ProgressMonitor(LoadImageText,i,image->rows << 2);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("M",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        q=image->pixels;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->green,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->rows << 2))
              ProgressMonitor(LoadImageText,i,image->rows << 2);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("Y",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        q=image->pixels;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->blue,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->rows << 2))
              ProgressMonitor(LoadImageText,i,image->rows << 2);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("K",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        q=image->pixels;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->index,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->rows << 2))
              ProgressMonitor(LoadImageText,i,image->rows << 2);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        break;
      }
    }
    TransformRGBImage(image,CMYKColorspace);
    CondenseImage(image);
    if (feof(image->file))
      MagickWarning(CorruptImageWarning,"not enough pixels",image->filename);
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    /*
      Proceed to next image.
    */
    count=ReadData((char *) scanline,packet_size,image->columns,
      image->file);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (count > 0);
  FreeMemory((char *) scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D C M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDCMImage reads a Digital Imaging and Communications in Medicine
%  (DICOM) file and returns it.  It It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the ReadDCMImage routine is:
%
%      image=ReadDCMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDCMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadDCMImage(const ImageInfo *image_info)
{
#include "dicom.h"

  char
    implicit_vr[3],
    magick[128],
    photometric[MaxTextExtent],
    explicit_vr[3];

  Image
    *image;

  int
    element,
    group,
    length,
    scene,
    y;

  long
    datum;

  Quantum
    blue,
    green,
    red,
    *scale;

  register int
    x,
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  register long
    packets;

  unsigned char
    *data;

  unsigned int
    bytes_per_pixel,
    height,
    high_bit,
    number_scenes,
    quantum,
    samples_per_pixel,
    significant_bits,
    status,
    width;

  unsigned long
    max_packets,
    max_value;

  unsigned short
    *graymap,
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read DCM preamble.
  */
  status=ReadData((char *) magick,1,128,image->file);
  status|=ReadData((char *) magick,1,4,image->file);
  if ((status == False) || (strncmp((char *) magick,"DICM",4) != 0))
    (void) fseek(image->file,0L,SEEK_SET);
  /*
    Read DCM Medical image.
  */
  (void) strcpy(photometric,"MONOCHROME1 ");
  bytes_per_pixel=1;
  data=(unsigned char) NULL;
  element=0;
  graymap=(unsigned short *) NULL;
  group=0;
  height=0;
  high_bit=0;
  number_scenes=1;
  samples_per_pixel=1;
  significant_bits=0;
  explicit_vr[2]='\0';
  width=0;
  while ((group != 0x7FE0) || (element != 0x0010))
  {
    /*
      Read a group.
    */
    image->offset=ftell(image->file);
    group=LSBFirstReadShort(image->file);
    element=LSBFirstReadShort(image->file);
    quantum=0;
    /*
      Find corresponding VR for this group and element.
    */
    for (i=0; dicom_info[i].group < 0xffff; i++)
      if ((group == dicom_info[i].group) &&
          (element == dicom_info[i].element))
        break;
    (void) strcpy(implicit_vr,dicom_info[i].vr);
    ReadData((char *) explicit_vr,1,2,image->file);
    if (strcmp(implicit_vr,"xs") == 0)
      if (isupper((int) *explicit_vr) && isupper((int) *(explicit_vr+1)))
        (void) strcpy(implicit_vr,explicit_vr);
    if (strcmp(implicit_vr,explicit_vr) == 0)
      {
        quantum=2;
        if ((strcmp(explicit_vr,"OB") == 0) ||
            (strcmp(explicit_vr,"OW") == 0) || (strcmp(explicit_vr,"SQ") == 0))
          {
            (void) LSBFirstReadShort(image->file);
            quantum=4;
          }
      }
    else
      {
        if (strcmp(implicit_vr,"xs") != 0)
          (void) fseek(image->file,(off_t) -2,SEEK_CUR);
        else
          if ((strcmp(explicit_vr,"SS") == 0) ||
              (strcmp(explicit_vr,"US") == 0))
            quantum=2;
          else
            (void) fseek(image->file,(off_t) -2,SEEK_CUR);
        if (strcmp(implicit_vr,"SQ") != 0)
          quantum=4;
      }
    datum=0;
    if (quantum == 4)
      datum=LSBFirstReadLong(image->file);
    else
      if (quantum == 2)
        datum=LSBFirstReadShort(image->file);
    quantum=0;
    length=1;
    if (datum != 0)
      {
        if ((strcmp(implicit_vr,"SS") == 0) ||
            (strcmp(implicit_vr,"US") == 0))
          switch (datum)
          {
            case 2:
            default:
            {
              quantum=2;
              datum=datum/2;
              length=datum;
              break;
            }
            case 4:
            {
              quantum=4;
              break;
            }
            case 8:
            {
              quantum=2;
              length=4;
              break;
            }
          }
        else
          if ((strcmp(implicit_vr,"UL") == 0) ||
              (strcmp(implicit_vr,"SL") == 0) ||
              (strcmp(implicit_vr,"FL") == 0))
            quantum=4;
          else
            if (strcmp(implicit_vr,"FD") == 0)
              quantum=4;
            else
              if (strcmp(implicit_vr,"xs") != 0)
                {
                  quantum=1;
                  length=datum;
                }
              else
                if ((strcmp(explicit_vr,"SS") == 0) ||
                    (strcmp(explicit_vr,"US") == 0))
                  quantum=2;
                else
                  {
                    quantum=2;
                    datum=datum/2;
                    length=datum;
                  }
      }
    if (image_info->verbose)
      {
        /*
          Display Dicom info.
        */
        for (i=0; dicom_info[i].description != (char *) NULL; i++)
          if ((group == dicom_info[i].group) &&
              (element == dicom_info[i].element))
            break;
        (void) fprintf(stdout,"0x%04x %4d (0x%04x,0x%04x)",image->offset,
          length,group,element);
        if (dicom_info[i].description != (char *) NULL)
          (void) fprintf(stdout," %.1024s",dicom_info[i].description);
        (void) fprintf(stdout,": ");
      }
    if ((group == 0x7FE0) && (element == 0x0010))
      {
        if (image_info->verbose)
          (void) fprintf(stdout,"\n");
        break;
      }
    /*
      Allocate space and read an array.
    */
    data=(unsigned char *) NULL;
    if ((length == 1) && (quantum == 1))
      datum=fgetc(image->file);
    else
      if ((length == 1) && (quantum == 2))
        datum=LSBFirstReadShort(image->file);
      else
        if ((length == 1) && (quantum == 4))
          datum=LSBFirstReadLong(image->file);
        else
          {
            data=(unsigned char *)
              AllocateMemory(quantum*(length+1)*sizeof(unsigned char));
            if (data == (unsigned char *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            (void) ReadData((char *) data,quantum,length,image->file);
            data[length*quantum]=0;
          }
    switch (group)
    {
      case 0x0028:
      {
        switch (element)
        {
          case 0x0002:
          {
            /*
              Samples per pixel.
            */
            samples_per_pixel=datum;
            break;
          }
          case 0x0004:
          {
            /*
              Photometric interpretation.
            */
            for (i=0; i < length; i++)
              photometric[i]=data[i];
            photometric[i]='\0';
            break;
          }
          case 0x0008:
          {
            /*
              Number of frames.
            */
            number_scenes=atoi((char *) data);
            break;
          }
          case 0x0010:
          {
            /*
              Image rows.
            */
            height=datum;
            break;
          }
          case 0x0011:
          {
            /*
              Image columns.
            */
            width=datum;
            break;
          }
          case 0x0100:
          {
            /*
              Bits allocated.
            */
            bytes_per_pixel=1;
            if (datum > 8)
              bytes_per_pixel=2;
            break;
          }
          case 0x0101:
          {
            /*
              Bits stored.
            */
            significant_bits=datum;
            bytes_per_pixel=1;
            if (significant_bits > 8)
              bytes_per_pixel=2;
            break;
          }
          case 0x0102:
          {
            /*
              High bit.
            */
            high_bit=datum;
            break;
          }
          case 0x1200:
          case 0x3006:
          {
            unsigned int
              colors;

            /*
              Populate image colormap.
            */
            colors=length/bytes_per_pixel;
            datum=colors;
            graymap=(unsigned short *)
              AllocateMemory(colors*sizeof(unsigned short));
            if (graymap == (unsigned short *) NULL)
              {
                MagickWarning(ResourceLimitWarning,"Unable to create graymap",
                 "Memory allocation failed");
                break;
              }
            for (i=0; i < (int) colors; i++)
              if (bytes_per_pixel == 1)
                graymap[i]=data[i];
              else
                graymap[i]=(unsigned short) ((short *) data)[i];
            break;
          }
          case 0x1201:
          case 0x1202:
          case 0x1203:
          {
            /*
              Initialize colormap.
            */
            image->class=PseudoClass;
            image->colors=length >> 1;
            if (image->colormap == (ColorPacket *) NULL)
              image->colormap=(ColorPacket *)
                AllocateMemory(image->colors*sizeof(ColorPacket));
            if (image->colormap == (ColorPacket *) NULL)
              {
                MagickWarning(ResourceLimitWarning,"Unable to create colormap",
                 "Memory allocation failed");
                break;
              }
            p=data;
            for (i=0; i < (int) image->colors; i++)
            {
              index=(*p++);
              index|=(*p++) << 8;
              if (element == 0x1201)
                image->colormap[i].red=(Quantum) XDownScale(index);
              if (element == 0x1202)
                image->colormap[i].green=(Quantum) XDownScale(index);
              if (element == 0x1203)
                image->colormap[i].blue=(Quantum) XDownScale(index);
            }
          }
          break;
        }
        break;
      }
      default:
        break;
    }
    if (image_info->verbose)
      {
        if (data == (unsigned char *) NULL)
          (void) fprintf(stdout,"%ld\n",datum);
        else
          {
            /*
              Display group data.
            */
            for (i=0; i < Max(length,4); i++)
              if (!isprint(data[i]))
                break;
            if ((i != length) && (length <= 4))
              {
                datum=0;
                for (i=length-1; i >= 0; i--)
                  datum=256*datum+data[i];
                  (void) fprintf(stdout,"%lu",datum);
              }
            else
              for (i=0; i < length; i++)
                if (isprint(data[i]))
                  (void) fprintf(stdout,"%c",(char) data[i]);
                else
                  (void) fprintf(stdout,"%c",'.');
            (void) fprintf(stdout,"\n");
          }
      }
    FreeMemory(data);
  }
  if ((width == 0) || (height == 0))
    PrematureExit(CorruptImageWarning,"Not a DCM image file",image);
  max_value=(1 << (8*bytes_per_pixel))-1;
  if (strncmp(photometric,"MONOCHROME",10) == 0)
    max_value=(1 << significant_bits)-1;
  scale=(Quantum *) NULL;
  if (max_value != MaxRGB)
    {
      /*
        Compute pixel scaling table.
      */
      scale=(Quantum *) AllocateMemory((max_value+1)*sizeof(Quantum));
      if (scale == (Quantum *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i <= (int) max_value; i++)
        scale[i]=(Quantum) ((i*MaxRGB+(max_value >> 1))/max_value);
    }
  for (scene=0; scene < (int) number_scenes; scene++)
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if ((image->colormap == (ColorPacket *) NULL) && (samples_per_pixel == 1))
      {
        /*
          Allocate image colormap.
        */
        image->class=PseudoClass;
        image->colors=Min(max_value,MaxRGB)+1;
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
          image->colormap[i].green=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
          image->colormap[i].blue=(Quantum)
            ((long) (MaxRGB*i)/(image->colors-1));
        }
      }
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    /*
      Convert DCM Medical image to runlength-encoded packets.
    */
    red=0;
    green=0;
    blue=0;
    index=0;
    q=image->pixels;
    SetRunlengthEncoder(q);
    for (y=0; y < (int) image->rows; y++)
    {
      for (x=0; x < (int) image->columns; x++)
      {
        if (samples_per_pixel == 1)
          {
            if (bytes_per_pixel == 1)
              index=fgetc(image->file);
            else
              index=LSBFirstReadShort(image->file);
            if (index > max_value)
              index=max_value;
            if (graymap != (unsigned short *) NULL)
              index=graymap[index];
          }
        else
          if (bytes_per_pixel == 1)
            {
              red=fgetc(image->file);
              green=fgetc(image->file);
              blue=fgetc(image->file);
            }
          else
            {
              red=LSBFirstReadShort(image->file);
              green=LSBFirstReadShort(image->file);
              blue=LSBFirstReadShort(image->file);
            }
        if (scale != (Quantum *) NULL)
          {
            red=scale[red];
            green=scale[green];
            blue=scale[blue];
            index=scale[index];
          }
        if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
            (index == q->index) && ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,max_packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    if (scale != (Quantum *) NULL)
                      FreeMemory((char *) scale);
                    PrematureExit(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  }
                q=image->pixels+packets-1;
              }
            q->index=index;
            q->length=0;
          }
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    SetRunlengthPackets(image,packets);
    SyncImage(image);
    /*
      Proceed to next image.
    */
    if (feof(image->file))
      break;
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (scene < (int) (number_scenes-1))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  }
  /*
    Free scale resource.
  */
  if (scale != (Quantum *) NULL)
    FreeMemory((char *) scale);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

#if defined(HasDPS)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D P S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDPSImage reads a Adobe Postscript image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadDPSImage routine is:
%
%      image=ReadDPSImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDPSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadDPSImage(const ImageInfo *image_info)
{
  char
    *client_name;

  Display
    *display;

  float
    pixels_per_point;

  Image
    *image;

  int
    sans,
    status,
    x,
    y;

  Pixmap
    pixmap;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  Screen
    *screen;

  XColor
    *colors;

  XImage
    *dps_image;

  XRectangle
    bounding_box,
    bits_per_pixel;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  /*
    Open X server connection.
  */
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    return((Image *) NULL);
  /*
    Set our forgiving error handler.
  */
  XSetErrorHandler(XError);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    return((Image *) NULL);
  /*
    Get user defaults from X resource database.
  */
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  /*
    Allocate standard colormap.
  */
  map_info=XAllocStandardColormap();
  if (map_info == (XStandardColormap *) NULL)
    MagickWarning(ResourceLimitWarning,"Unable to create standard colormap",
      "Memory allocation failed");
  else
    {
      /*
        Initialize visual info.
      */
      resource_info.visual_type="default";
      visual_info=XBestVisualInfo(display,map_info,&resource_info);
      map_info->colormap=(Colormap) NULL;
    }
  if ((map_info == (XStandardColormap *) NULL) ||
      (visual_info == (XVisualInfo *) NULL))
    {
      DestroyImage(image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  /*
    Create a pixmap the appropriate size for the image.
  */
  screen=ScreenOfDisplay(display,visual_info->screen);
  pixels_per_point=XDPSPixelsPerPoint(screen);
  if ((image->x_resolution != 0.0) && (image->y_resolution != 0.0))
    pixels_per_point=Min(image->x_resolution,image->y_resolution)/72.0;
  status=XDPSCreatePixmapForEPSF((DPSContext) NULL,screen,image->file,
    visual_info->depth,pixels_per_point,&pixmap,&bits_per_pixel,&bounding_box);
  if ((status == dps_status_failure) || (status == dps_status_no_extension))
    {
      DestroyImage(image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  /*
    Rasterize the file into the pixmap.
  */
  status=XDPSImageFileIntoDrawable((DPSContext) NULL,screen,pixmap,image->file,
    bits_per_pixel.height,visual_info->depth,&bounding_box,-bounding_box.x,
    -bounding_box.y,pixels_per_point,True,False,True,&sans);
  if (status != dps_status_success)
    {
      DestroyImage(image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  /*
    Initialize DPS X image.
  */
  dps_image=XGetImage(display,pixmap,0,0,bits_per_pixel.width,
    bits_per_pixel.height,AllPlanes,ZPixmap);
  XFreePixmap(display,pixmap);
  if (dps_image == (XImage *) NULL)
    {
      DestroyImage(image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  /*
    Get the colormap colors.
  */
  colors=(XColor *) AllocateMemory(visual_info->colormap_size*sizeof(XColor));
  if (colors == (XColor *) NULL)
    {
      DestroyImage(image);
      XDestroyImage(dps_image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  if ((visual_info->class != DirectColor) &&
      (visual_info->class != TrueColor))
    for (i=0; i < visual_info->colormap_size; i++)
    {
      colors[i].pixel=i;
      colors[i].pad=0;
    }
  else
    {
      unsigned long
        blue,
        blue_bit,
        green,
        green_bit,
        red,
        red_bit;

      /*
        DirectColor or TrueColor visual.
      */
      red=0;
      green=0;
      blue=0;
      red_bit=visual_info->red_mask & (~(visual_info->red_mask)+1);
      green_bit=visual_info->green_mask & (~(visual_info->green_mask)+1);
      blue_bit=visual_info->blue_mask & (~(visual_info->blue_mask)+1);
      for (i=0; i < visual_info->colormap_size; i++)
      {
        colors[i].pixel=red | green | blue;
        colors[i].pad=0;
        red+=red_bit;
        if (red > visual_info->red_mask)
          red=0;
        green+=green_bit;
        if (green > visual_info->green_mask)
          green=0;
        blue+=blue_bit;
        if (blue > visual_info->blue_mask)
          blue=0;
      }
    }
  XQueryColors(display,XDefaultColormap(display,visual_info->screen),colors,
    visual_info->colormap_size);
  /*
    Convert X image to MIFF format.
  */
  if ((visual_info->class != TrueColor) &&
      (visual_info->class != DirectColor))
    image->class=PseudoClass;
  image->columns=dps_image->width;
  image->rows=dps_image->height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      DestroyImage(image);
      FreeMemory((char *) colors);
      XDestroyImage(dps_image);
      XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
        (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
      return((Image *) NULL);
    }
  SetImage(image);
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      register unsigned long
        color,
        index;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=visual_info->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=visual_info->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=visual_info->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if ((visual_info->colormap_size > 0) &&
          (visual_info->class == DirectColor))
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=XGetPixel(dps_image,x,y);
            index=(pixel >> red_shift) & red_mask;
            p->red=XDownScale(colors[index].red);
            index=(pixel >> green_shift) & green_mask;
            p->green=XDownScale(colors[index].green);
            index=(pixel >> blue_shift) & blue_mask;
            p->blue=XDownScale(colors[index].blue);
            p->index=0;
            p->length=0;
            p++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,y,image->rows);
        }
      else
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=XGetPixel(dps_image,x,y);
            color=(pixel >> red_shift) & red_mask;
            p->red=XDownScale((color*65535L)/red_mask);
            color=(pixel >> green_shift) & green_mask;
            p->green=XDownScale((color*65535L)/green_mask);
            color=(pixel >> blue_shift) & blue_mask;
            p->blue=XDownScale((color*65535L)/blue_mask);
            p->index=0;
            p->length=0;
            p++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,y,image->rows);
        }
      break;
    }
    case PseudoClass:
    {
      /*
        Create colormap.
      */
      image->colors=visual_info->colormap_size;
      image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        {
          DestroyImage(image);
          FreeMemory((char *) colors);
          XDestroyImage(dps_image);
          XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
            (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
          return((Image *) NULL);
        }
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[colors[i].pixel].red=XDownScale(colors[i].red);
        image->colormap[colors[i].pixel].green=XDownScale(colors[i].green);
        image->colormap[colors[i].pixel].blue=XDownScale(colors[i].blue);
      }
      /*
        Convert X image to PseudoClass packets.
      */
      for (y=0; y < (int) image->rows; y++)
      {
        for (x=0; x < (int) image->columns; x++)
        {
          p->index=(unsigned short) XGetPixel(dps_image,x,y);
          p->length=0;
          p++;
        }
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
  }
  FreeMemory((char *) colors);
  XDestroyImage(dps_image);
  if (image->class == PseudoClass)
    SyncImage(image);
  CondenseImage(image);
  /*
    Rasterize matte image.
  */
  status=XDPSCreatePixmapForEPSF((DPSContext) NULL,screen,image->file,1,
    pixels_per_point,&pixmap,&bits_per_pixel,&bounding_box);
  if ((status != dps_status_failure) && (status != dps_status_no_extension))
    {
      status=XDPSImageFileIntoDrawable((DPSContext) NULL,screen,pixmap,
        image->file,bits_per_pixel.height,1,&bounding_box,-bounding_box.x,
        -bounding_box.y,pixels_per_point,True,True,True,&sans);
      if (status == dps_status_success)
        {
          XImage
            *matte_image;

          /*
            Initialize image matte.
          */
          matte_image=XGetImage(display,pixmap,0,0,bits_per_pixel.width,
            bits_per_pixel.height,AllPlanes,ZPixmap);
          XFreePixmap(display,pixmap);
          if (matte_image != (XImage *) NULL)
            {
              image->class=DirectClass;
              image->matte=True;
              p=image->pixels;
              for (y=0; y < (int) image->rows; y++)
                for (x=0; x < (int) image->columns; x++)
                {
                  p->index=Opaque;
                  if (!XGetPixel(matte_image,x,y))
                    p->index=Transparent;
                  p++;
                }
              XDestroyImage(matte_image);
            }
        }
    }
  /*
    Free resources.
  */
  XFreeResources(display,visual_info,map_info,(XPixelInfo *) NULL,
    (XFontStruct *) NULL,&resource_info,(XWindowInfo *) NULL);
  CloseImage(image);
  return(image);
}
#else
Image *ReadDPSImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"Cannot read DPS images",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F A X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFAXImage reads a Group 3 FAX image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFAXImage routine is:
%
%      image=ReadFAXImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadFAXImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadFAXImage(const ImageInfo *image_info)
{
  Image
    *image;

  unsigned int
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  image->class=PseudoClass;
  if (image->columns == 0)
    image->columns=2592;
  if (image->rows == 0)
    image->rows=3508;
  image->packets=Max((image->columns*image->rows+8) >> 4,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  image->colors=2;
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->colormap == (ColorPacket *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Monochrome colormap.
  */
  image->colormap[0].red=MaxRGB;
  image->colormap[0].green=MaxRGB;
  image->colormap[0].blue=MaxRGB;
  image->colormap[1].red=0;
  image->colormap[1].green=0;
  image->colormap[1].blue=0;
  status=HuffmanDecodeImage(image);
  if (status == False)
    PrematureExit(CorruptImageWarning,"Unable to read image data",image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F I T S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFITSImage reads a FITS image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadFITSImage routine is:
%
%      image=ReadFITSImage(image_info)
%
%  A description of each parameter follows:
%
%    o image: Method ReadFITSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
Image *ReadFITSImage(const ImageInfo *image_info)
{
  typedef struct _FITSHeader
  {
    unsigned int
      simple;

    int
      bits_per_pixel;

    unsigned int
      number_of_axis,
      columns,
      rows,
      depth;

    double
      min_data,
      max_data,
      zero,
      scale;
  } FITSHeader;

  char
    long_quantum[8],
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  double
    pixel,
    scale,
    scaled_pixel;

  FITSHeader
    fits_header;

  Image
    *image;

  int
    c,
    j,
    packet_size,
    y;

  long
    count,
    quantum;

  register int
    i,
    x;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *fits_pixels;

  unsigned int
    status,
    value_expected;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image header.
  */
  fits_header.simple=False;
  fits_header.bits_per_pixel=8;
  fits_header.columns=1;
  fits_header.rows=1;
  fits_header.depth=1;
  fits_header.min_data=0.0;
  fits_header.max_data=0.0;
  fits_header.zero=0.0;
  fits_header.scale=1.0;
  /*
    Decode image header.
  */
  c=fgetc(image->file);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  for ( ; ; )
  {
    if (!isalnum(c))
      {
        c=fgetc(image->file);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        if (Latin1Compare(keyword,"END") == 0)
          break;
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=fgetc(image->file);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c) || (c == '-') || (c == '+') || (c == '.'))
        {
          if ((p-value) < (MaxTextExtent-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (Latin1Compare(keyword,"SIMPLE") == 0)
          fits_header.simple=(*value == 'T') || (*value == 't');
        if (Latin1Compare(keyword,"BITPIX") == 0)
          fits_header.bits_per_pixel=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NAXIS") == 0)
          fits_header.number_of_axis=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NAXIS1") == 0)
          fits_header.columns=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NAXIS2") == 0)
          fits_header.rows=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NAXIS3") == 0)
          fits_header.depth=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"DATAMAX") == 0)
          fits_header.max_data=atof(value);
        if (Latin1Compare(keyword,"DATAMIN") == 0)
          fits_header.min_data=atof(value);
        if (Latin1Compare(keyword,"BZERO") == 0)
          fits_header.zero=atof(value);
        if (Latin1Compare(keyword,"BSCALE") == 0)
          fits_header.scale=atof(value);
      }
    while (isspace(c))
    {
      c=fgetc(image->file);
      count++;
    }
  }
  while (count > 2880)
    count-=2880;
  for ( ; count < 2880; count++)
    (void) fgetc(image->file);
  /*
    Verify that required image information is defined.
  */
  if ((!fits_header.simple) || (fits_header.number_of_axis < 1) ||
      (fits_header.number_of_axis > 4) ||
      (fits_header.columns*fits_header.rows) == 0)
    PrematureExit(CorruptImageWarning,"image type not supported",image);
  /*
    Create linear colormap.
  */
  image->columns=fits_header.columns;
  image->rows=fits_header.rows;
  image->class=PseudoClass;
  image->colors=MaxRGB+1;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].red=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].green=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].blue=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
  }
  /*
    Initialize image structure.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  packet_size=fits_header.bits_per_pixel/8;
  if (packet_size < 0)
    packet_size=(-packet_size);
  fits_pixels=(unsigned char *)
    AllocateMemory(image->packets*packet_size*sizeof(unsigned char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (fits_pixels == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  /*
    Convert FITS pixels to runlength-encoded packets.
  */
  status=ReadData((char *) fits_pixels,(unsigned int) packet_size,
    image->packets,image->file);
  if (status == False)
    MagickWarning(CorruptImageWarning,"Insufficient image data in file",
      image->filename);
  if ((fits_header.min_data == 0.0) && (fits_header.max_data == 0.0))
    {
      /*
        Determine minimum and maximum intensity.
      */
      p=fits_pixels;
      long_quantum[0]=(*p);
      quantum=(*p++);
      for (j=0; j < (packet_size-1); j++)
      {
        long_quantum[j+1]=(*p);
        quantum=(quantum << 8) | (*p++);
      }
      pixel=(double) quantum;
      if (fits_header.bits_per_pixel == 16)
        if (pixel > 32767)
          pixel-=65536;
      if (fits_header.bits_per_pixel == -32)
        pixel=(double) (*((float *) &quantum));
      if (fits_header.bits_per_pixel == -64)
        pixel=(double) (*((double *) long_quantum));
      fits_header.min_data=pixel*fits_header.scale+fits_header.zero;
      fits_header.max_data=pixel*fits_header.scale+fits_header.zero;
      for (i=1; i < (int) image->packets; i++)
      {
        long_quantum[0]=(*p);
        quantum=(*p++);
        for (j=0; j < (packet_size-1); j++)
        {
          long_quantum[j+1]=(*p);
          quantum=(quantum << 8) | (*p++);
        }
        pixel=(double) quantum;
        if (fits_header.bits_per_pixel == 16)
          if (pixel > 32767)
            pixel-=65536;
        if (fits_header.bits_per_pixel == -32)
          pixel=(double) (*((float *) &quantum));
        if (fits_header.bits_per_pixel == -64)
          pixel=(double) (*((double *) long_quantum));
        scaled_pixel=pixel*fits_header.scale+fits_header.zero;
        if (scaled_pixel < fits_header.min_data)
          fits_header.min_data=scaled_pixel;
        if (scaled_pixel > fits_header.max_data)
          fits_header.max_data=scaled_pixel;
      }
    }
  /*
    Convert FITS pixels to runlength-encoded packets.
  */
  scale=1.0;
  if (fits_header.min_data != fits_header.max_data)
    scale=MaxRGB/(fits_header.max_data-fits_header.min_data);
  p=fits_pixels;
  q=image->pixels;
  for (y=image->rows-1; y >= 0; y--)
  {
    q=image->pixels+(y*image->columns);
    for (x=0; x < (int) image->columns; x++)
    {
      long_quantum[0]=(*p);
      quantum=(*p++);
      for (j=0; j < (packet_size-1); j++)
      {
        long_quantum[j+1]=(*p);
        quantum=(quantum << 8) | (*p++);
      }
      pixel=(double) quantum;
      if (fits_header.bits_per_pixel == 16)
        if (pixel > 32767)
          pixel-=65536;
      if (fits_header.bits_per_pixel == -32)
        pixel=(double) (*((float *) &quantum));
      if (fits_header.bits_per_pixel == -64)
        pixel=(double) (*((double *) long_quantum));
      scaled_pixel=scale*
        (pixel*fits_header.scale-fits_header.min_data-fits_header.zero);
      if (scaled_pixel < 0)
        scaled_pixel=0;
      else
        if (scaled_pixel > MaxRGB)
          scaled_pixel=MaxRGB;
      q->index=(unsigned short) scaled_pixel;
      q->length=0;
      q++;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  FreeMemory((char *) fits_pixels);
  SyncImage(image);
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

#if defined(HasFPX)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d F P X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadFPXImage reads a FlashPix image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This routine was contributed by BillR@corbis.com.
%
%  The format of the ReadFPXImage routine is:
%
%      image=ReadFPXImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadFPXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadFPXImage(const ImageInfo *image_info)
{
  FPXColorspace
    colorspace;

  FPXImageComponentDesc
    *alpha_component,
    *blue_component,
    *green_component,
    *red_component;

  FPXImageDesc
    fpx_info;

  FPXImageHandle
    *flashpix;

  FPXStatus
    status;

  FPXSummaryInformation
    summary_info;

  Image
    *image;

  int
    x,
    y;

  Quantum
    blue,
    green,
    red;

  register int
    i;

  register long
    packets;

  register RunlengthPacket
    *q;

  register unsigned char
    *a,
    *b,
    *g,
    *r;

  unsigned char
    *scanline;

  unsigned int
    subimage;

  unsigned long
    height,
    max_packets,
    memory_limit,
    tile_width,
    tile_height,
    width;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdin) || image->pipe)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy standard input or pipe to temporary file.
      */
      TemporaryFilename((char *) image_info->filename);
      file=fopen(image_info->filename,WriteBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to write file",image);
      c=fgetc(image->file);
      while (c != EOF)
      {
        (void) putc(c,file);
        c=fgetc(image->file);
      }
      (void) fclose(file);
      (void) strcpy(image->filename,image_info->filename);
      image->temporary=True;
    }
  CloseImage(image);
  /*
    Initialize FPX toolkit.
  */
  memory_limit=20000000;
  status=FPX_SetToolkitMemoryLimit(&memory_limit);
  if (status != FPX_OK)
    PrematureExit(DelegateWarning,"Unable to initialize FPX library",image);
  tile_width=64;
  tile_height=64;
  flashpix=(FPXImageHandle *) NULL;
  {
#if defined(macintosh)
    FSSpec
      fsspec;

    FilenameToFSSpec(image->filename,&fsspec);
    status=FPX_OpenImageByFilename((const FSSpec &) fsspec,
#else
    status=FPX_OpenImageByFilename(image->filename,
#endif
      NULL,&width,&height,&tile_width,&tile_height,&colorspace,&flashpix);
  }
  if (status == FPX_LOW_MEMORY_ERROR)
    {
      FPX_ClearSystem();
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  if (status != FPX_OK)
    {
      FPX_ClearSystem();
      PrematureExit(FileOpenWarning,"Unable to open FPX file",image);
    }
  if (image_info->view == (char *) NULL)
    {
      float
        aspect_ratio;

      /*
        Get the aspect ratio.
      */
      aspect_ratio=(float) width/height;
      status=FPX_GetImageResultAspectRatio(flashpix,&aspect_ratio);
      if (status != FPX_OK)
        MagickWarning(DelegateWarning,"Unable to read aspect ratio",
          image_info->filename);
      if (width != (unsigned long) ((aspect_ratio*height)+0.5))
        Swap(width,height);
    }
  status=FPX_GetSummaryInformation(flashpix,&summary_info);
  if (status != FPX_OK)
    {
      FPX_ClearSystem();
      PrematureExit(FileOpenWarning,"Unable to read summary info",image);
    }
  if (summary_info.title_valid)
    if ((summary_info.title.length != 0) &&
        (summary_info.title.ptr != (unsigned char *) NULL))
      {
        /*
          Note image label.
        */
        image->label=(char *) AllocateMemory((unsigned int)
          (summary_info.title.length+1)*sizeof(char));
        if (image->label == (char *) NULL)
          MagickWarning(DelegateWarning,"Memory allocation failed",
            image_info->filename);
        else
          {
            (void) strncpy(image->label,(char *) summary_info.title.ptr,
              summary_info.title.length);
            image->label[summary_info.title.length]='\0';
          }
      }
  if (summary_info.comments_valid)
    if ((summary_info.comments.length != 0) &&
        (summary_info.comments.ptr != (unsigned char *) NULL))
      {
        /*
          Note image comment.
        */
        image->comments=(char *) AllocateMemory((unsigned int)
          (summary_info.comments.length+1)*sizeof(char));
        if (image->comments == (char *) NULL)
          MagickWarning(DelegateWarning,"Memory allocation failed",
            image_info->filename);
        else
          {
            (void) strncpy(image->comments,(char *) summary_info.comments.ptr,
              summary_info.comments.length);
            image->comments[summary_info.comments.length]='\0';
          }
      }
  /*
    Determine resolution by subimage specification.
  */
  for (i=1; ; i++)
    if (((width >> i) < tile_width) ||
        ((height >> i) < tile_height))
      break;
  subimage=i;
  if (image_info->subrange != 0)
    while (subimage > image_info->subimage)
    {
      width>>=1;
      height>>=1;
      subimage--;
    }
  if (image_info->size != (char *) NULL)
    while ((width > image->columns) || (height > image->rows))
    {
      width>>=1;
      height>>=1;
      subimage--;
    }
  image->columns=width;
  image->rows=height;
  if ((colorspace.numberOfComponents % 2) == 0)
    image->matte=True;
  if (colorspace.numberOfComponents == 1)
    {
      /*
        Create linear colormap.
      */
      image->class=PseudoClass;
      image->colors=MaxRGB+1;
      image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        {
          FPX_ClearSystem();
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",
            image);
        }
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=(Quantum) i;
        image->colormap[i].green=(Quantum) i;
        image->colormap[i].blue=(Quantum) i;
      }
    }
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Allocate memory for the image and pixel buffer.
  */
  packets=0;
  max_packets=Max((image->columns*image->rows+2) >> 2,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  scanline=(unsigned char *) AllocateMemory(colorspace.numberOfComponents*
    image->columns*(tile_height+1)*sizeof(unsigned char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (scanline == (unsigned char *) NULL))
    {
      FPX_ClearSystem();
      (void) FPX_CloseImage(flashpix);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  /*
    Initialize FlashPix image description.
  */
  fpx_info.numberOfComponents=colorspace.numberOfComponents;
  for (i=0; i < 4; i++)
  {
    fpx_info.components[i].myColorType.myDataType=DATA_TYPE_UNSIGNED_BYTE;
    fpx_info.components[i].horzSubSampFactor=1;
    fpx_info.components[i].vertSubSampFactor=1;
    fpx_info.components[i].columnStride=
      fpx_info.numberOfComponents*sizeof(unsigned char);
    fpx_info.components[i].lineStride=
      image->columns*fpx_info.components[i].columnStride;
    fpx_info.components[i].theData=scanline+i;
  }
  fpx_info.components[0].myColorType.myColor=
    fpx_info.numberOfComponents > 2 ? NIFRGB_R : MONOCHROME;
  red_component=(&fpx_info.components[0]);
  fpx_info.components[1].myColorType.myColor=
    fpx_info.numberOfComponents > 2 ? NIFRGB_G : ALPHA;
  green_component=(&fpx_info.components[1]);
  fpx_info.components[2].myColorType.myColor=NIFRGB_B;
  blue_component=(&fpx_info.components[2]);
  fpx_info.components[3].myColorType.myColor=ALPHA;
  alpha_component=(&fpx_info.components[fpx_info.numberOfComponents-1]);
  FPX_SetResampleMethod(FPX_LINEAR_INTERPOLATION);
  /*
    Initialize image pixels.
  */
  red=0;
  green=0;
  blue=0;
  index=0;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    if ((y % tile_height) == 0)
      {
        /*
          Read FPX image tile (with or without viewing transform)..
        */
        if (image_info->view != (char *) NULL)
          status=FPX_ReadImageRectangle(flashpix,0,y,image->columns,y+
            tile_height-1,subimage,&fpx_info);
        else
          status=FPX_ReadImageTransformRectangle(flashpix,0.0F,(float) y/
            image->rows,(float) image->columns/image->rows,(float) (y+
            tile_height-1)/image->rows,(long) image->columns,(long) tile_height,
            &fpx_info);
        if (status == FPX_LOW_MEMORY_ERROR)
          {
            FreeMemory((char *) scanline);
            (void) FPX_CloseImage(flashpix);
            FPX_ClearSystem();
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
      }
    /*
      Transfer a FPX scanline.
    */
    r=red_component->theData+(y % tile_height)*red_component->lineStride;
    g=green_component->theData+(y % tile_height)*green_component->lineStride;
    b=blue_component->theData+(y % tile_height)*blue_component->lineStride;
    a=alpha_component->theData+(y % tile_height)*alpha_component->lineStride;
    for (x=0; x < (int) image->columns; x++)
    {
      if (fpx_info.numberOfComponents > 2)
        {
          red=UpScale(*r);
          green=UpScale(*g);
          blue=UpScale(*b);
        }
      else
        {
          index=UpScale(*r);
          red=index;
          green=index;
          blue=index;
        }
      if (image->matte)
        index=UpScale(*a);
      if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
          (index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  FreeMemory((char *) scanline);
                  (void) FPX_CloseImage(flashpix);
                  FPX_ClearSystem();
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              q=image->pixels+packets-1;
            }
          q->red=red;
          q->green=green;
          q->blue=blue;
          q->index=index;
          q->length=0;
        }
      r+=red_component->columnStride;
      g+=green_component->columnStride;
      b+=blue_component->columnStride;
      a+=alpha_component->columnStride;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  SetRunlengthPackets(image,packets);
  FreeMemory((char *) scanline);
  (void) FPX_CloseImage(flashpix);
  FPX_ClearSystem();
  if (image->temporary)
    {
      (void) remove(image->filename);
      image->temporary=False;
    }
  return(image);
}
#else
Image *ReadFPXImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"FPX library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G I F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGIFImage reads a Compuserve Graphics image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadGIFImage routine is:
%
%      image=ReadGIFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGIFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      an error occurs.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadGIFImage(const ImageInfo *image_info)
{
#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

  char
    geometry[MaxTextExtent];

  Image
    *image;

  int
    status;

  RectangleInfo
    page_info;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  short int
    transparency_index;

  unsigned char
    background,
    c,
    flag,
    *global_colormap,
    header[MaxTextExtent],
    magick[12];

  unsigned int
    delay,
    dispose,
    global_colors,
    image_count,
    iterations;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a GIF file.
  */
  status=ReadData((char *) magick,1,6,image->file);
  if ((status == False) || ((strncmp((char *) magick,"GIF87",5) != 0) &&
      (strncmp((char *) magick,"GIF89",5) != 0)))
    PrematureExit(CorruptImageWarning,"Not a GIF image file",image);
  global_colors=0;
  global_colormap=(unsigned char *) NULL;
  page_info.width=LSBFirstReadShort(image->file);
  page_info.height=LSBFirstReadShort(image->file);
  (void) ReadData((char *) &flag,1,1,image->file);
  (void) ReadData((char *) &background,1,1,image->file);
  (void) ReadData((char *) &c,1,1,image->file);  /* reserved */
  if (BitSet(flag,0x80))
    {
      /*
        Read global colormap.
      */
      global_colors=1 << ((flag & 0x07)+1);
      global_colormap=(unsigned char *)
        AllocateMemory(3*global_colors*sizeof(unsigned char));
      if (global_colormap == (unsigned char *) NULL)
        PrematureExit(ResourceLimitWarning,"Unable to read image colormap file",
          image);
      (void) ReadData((char *) global_colormap,3,global_colors,image->file);
    }
  delay=0;
  dispose=0;
  iterations=1;
  transparency_index=(-1);
  image_count=0;
  for ( ; ; )
  {
    status=ReadData((char *) &c,1,1,image->file);
    if (status == False)
      break;
    if (c == ';')
      break;  /* terminator */
    if (c == '!')
      {
        /*
          GIF Extension block.
        */
        status=ReadData((char *) &c,1,1,image->file);
        if (status == False)
          PrematureExit(CorruptImageWarning,"Unable to read extention block",
            image);
        switch (c)
        {
          case 0xf9:
          {
            /*
              Read Graphics Control extension.
            */
            while (ReadDataBlock((char *) header,image->file) > 0);
            dispose=header[0] >> 2;
            delay=(header[2] << 8) | header[1];
            if ((header[0] & 0x01) == 1)
              transparency_index=header[3];
            break;
          }
          case 0xfe:
          {
            int
              length;

            /*
              Read Comment extension.
            */
            for ( ; ; )
            {
              length=ReadDataBlock((char *) header,image->file);
              if (length <= 0)
                break;
              if (image->comments != (char *) NULL)
                {
                  image->comments=(char *) ReallocateMemory((char *)
                    image->comments,(Extent(image->comments)+length+1)*
                    sizeof(char));
                }
              else
                {
                  image->comments=(char *)
                    AllocateMemory((length+1)*sizeof(char));
                  if (image->comments != (char *) NULL)
                    *image->comments='\0';
                }
              if (image->comments == (char *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              header[length]='\0';
              (void) strcat(image->comments,(char *) header);
            }
            break;
          }
          case 0xff:
          {
            /*
              Read Netscape Loop extension.
            */
            while (ReadDataBlock((char *) header,image->file) > 0);
            iterations=(header[2] << 8) | header[1];
            break;
          }
          default:
          {
            while (ReadDataBlock((char *) header,image->file) > 0);
            break;
          }
        }
      }
    if (c != ',')
      continue;
    if (image_count != 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
    image_count++;
    /*
      Read image attributes.
    */
    image->class=PseudoClass;
    page_info.x=LSBFirstReadShort(image->file);
    page_info.y=LSBFirstReadShort(image->file);
    image->columns=LSBFirstReadShort(image->file);
    image->rows=LSBFirstReadShort(image->file);
    (void) ReadData((char *) &flag,1,1,image->file);
    image->interlace=BitSet(flag,0x40) ? PlaneInterlace : NoInterlace;
    image->colors=!BitSet(flag,0x80) ? global_colors : 1 << ((flag & 0x07)+1);
    FormatString(geometry,"%ux%u%+d%+d",page_info.width,page_info.height,
      page_info.x,page_info.y);
    if (image_info->page == (char *) NULL)
      image->page=PostscriptGeometry(geometry);
    if (image_info->delay == (char *) NULL)
      image->delay=delay;
    if (image_info->dispose == (char *) NULL)
      image->dispose=dispose;
    if (image_info->iterations == (char *) NULL)
      image->iterations=iterations;
    delay=0;
    dispose=0;
    iterations=1;
    if (image_info->ping)
      {
        if (transparency_index >= 0)
          image->class=DirectClass;
        CloseImage(image);
        return(image);
      }
    if ((image->columns == 0) || (image->rows == 0))
      PrematureExit(CorruptImageWarning,"image size is 0",image);
    /*
      Inititialize colormap.
    */
    if (image->pixels != (RunlengthPacket *) NULL)
      FreeMemory((char *) image->pixels);
    image->pixels=(RunlengthPacket *) NULL;
    image->colormap=(ColorPacket *)
      AllocateMemory(image->colors*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (!BitSet(flag,0x80))
      {
        /*
          Use global colormap.
        */
        p=global_colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=UpScale(*p++);
          image->colormap[i].green=UpScale(*p++);
          image->colormap[i].blue=UpScale(*p++);
        }
        image->background_color=
          image->colormap[Min(background,image->colors-1)];
      }
    else
      {
        unsigned char
          *colormap;

        /*
          Read local colormap.
        */
        colormap=(unsigned char *)
          AllocateMemory(3*image->colors*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) colormap,3,image->colors,image->file);
        p=colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=UpScale(*p++);
          image->colormap[i].green=UpScale(*p++);
          image->colormap[i].blue=UpScale(*p++);
        }
        FreeMemory((char *) colormap);
      }
    /*
      Decode image.
    */
    status=GIFDecodeImage(image);
    if (transparency_index >= 0)
      {
        /*
          Create matte channel.
        */
        q=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          if (q->index != (unsigned short) transparency_index)
            q->index=Opaque;
          else
            q->index=Transparent;
          q++;
        }
        transparency_index=(-1);
        image->class=DirectClass;
        image->matte=True;
      }
    if (status == False)
      {
        MagickWarning(CorruptImageWarning,"Corrupt GIF image",image->filename);
        break;
      }
    if (image_info->subrange != 0)
      {
        if (image->scene < image_info->subimage)
          {
            Image
              subimage;

            /*
              Destroy image.
            */
            subimage=(*image);
            image->file=(FILE *) NULL;
            DestroyImage(image);
            image=AllocateImage(image_info);
            if (image == (Image *) NULL)
              return((Image *) NULL);
            image->file=subimage.file;
            image->scene=subimage.scene+1;
            image_count=0;
          }
        else
          if (image->scene >= (image_info->subimage+image_info->subrange-1))
            break;
      }
  }
  if (global_colormap != (unsigned char *) NULL)
    FreeMemory((char *) global_colormap);
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(CorruptImageWarning,"Corrupt GIF image or subimage not found",
      image);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A D A T I O N I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRADATIONImage creates a gradation image and initializes it to
%  the X server color range as specified by the filename.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadGRADATIONImage routine is:
%
%      image=ReadGRADATIONImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRADATIONImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadGRADATIONImage(const ImageInfo *image_info)
{
  char
    colorname[MaxTextExtent];

  double
    brightness,
    brightness_step,
    hue,
    hue_step,
    saturation,
    saturation_step;

  Image
    *image;

  int
    x,
    y;

  register RunlengthPacket
    *q;

  XColor
    color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Initialize Image structure.
  */
  (void) strcpy(image->filename,image_info->filename);
  if (image->columns == 0)
    image->columns=512;
  if (image->rows == 0)
    image->rows=512;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  /*
    Determine (Hue, Saturation, Brightness) gradient.
  */
  (void) strcpy(colorname,image_info->filename);
  (void) sscanf(image_info->filename,"%[^-]",colorname);
  (void) XQueryColorDatabase(colorname,&color);
  TransformHSL((Quantum) XDownScale(color.red),
    (Quantum) XDownScale(color.green),(Quantum) XDownScale(color.blue),
    &hue,&saturation,&brightness);
  (void) strcpy(colorname,"white");
  if (Intensity(color) > 32767)
    (void) strcpy(colorname,"black");
  (void) sscanf(image_info->filename,"%*[^-]-%s",colorname);
  (void) XQueryColorDatabase(colorname,&color);
  TransformHSL((Quantum) XDownScale(color.red),
    (Quantum) XDownScale(color.green),(Quantum) XDownScale(color.blue),
    &hue_step,&saturation_step,&brightness_step);
  hue_step=(hue_step-hue)/(double) image->packets;
  saturation_step=(saturation_step-saturation)/(double) image->packets;
  brightness_step=(brightness_step-brightness)/(double) image->packets;
  /*
    Initialize image pixels.
  */
  q=image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      HSLTransform(hue,saturation,brightness,&q->red,&q->green,&q->blue);
      q->index=0;
      q->length=0;
      q++;
      hue+=hue_step;
      saturation+=saturation_step;
      brightness+=brightness_step;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  CondenseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d G R A Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadGRAYImage reads an image of raw grayscale bytes and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadGRAYImage routine is:
%
%      image=ReadGRAYImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadGRAYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadGRAYImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    count,
    y;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  unsigned int
    packet_size;

  unsigned long
    max_packets;

  unsigned short
    index,
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < image->offset; i++)
    (void) fgetc(image->file);
  /*
    Allocate memory for a scanline.
  */
  packet_size=image->depth >> 3;
  scanline=(unsigned char *)
    AllocateMemory(packet_size*image->tile_info.width*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (int) image->rows; y++)
        (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
          image->file);
    }
  do
  {
    /*
      Initialize image structure.
    */
    packets=0;
    max_packets=Max((image->columns*image->rows+2) >> 2,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Create linear colormap.
    */
    image->class=PseudoClass;
    image->colors=1 << image->depth;
    image->colormap=(ColorPacket *)
      AllocateMemory(image->colors*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    for (i=0; i < (int) image->colors; i++)
    {
      image->colormap[i].red=(Quantum) i;
      image->colormap[i].green=(Quantum) i;
      image->colormap[i].blue=(Quantum) i;
    }
    /*
      Convert raster image to runlength-encoded packets.
    */
    for (y=0; y < image->tile_info.y; y++)
      (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
        image->file);
    q=image->pixels;
    SetRunlengthEncoder(q);
    for (y=0; y < (int) image->rows; y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
          image->file);
      p=scanline+packet_size*image->tile_info.x;
      for (x=0; x < (int) image->columns; x++)
      {
        ReadQuantum(index,p);
        if ((index == q->index) && ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,max_packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    FreeMemory((char *) scanline);
                    PrematureExit(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  }
                q=image->pixels+packets-1;
              }
            q->index=index;
            q->length=0;
          }
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    count=image->tile_info.height-image->rows-image->tile_info.y;
    for (y=0; y < count; y++)
      (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
        image->file);
    if (feof(image->file))
      MagickWarning(CorruptImageWarning,"not enough pixels",image->filename);
    SetRunlengthPackets(image,packets);
    SyncImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadData((char *) scanline,packet_size,image->tile_info.width,
      image->file);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (count > 0);
  FreeMemory((char *) scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

#if defined(HasHDF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d H D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadHDFImage reads a Hierarchical Data Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadHDFImage routine is:
%
%      image=ReadHDFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadHDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadHDFImage(const ImageInfo *image_info)
{
#include "hdf.h"
#undef BSD
#undef LOCAL

  ClassType
    class;

  Image
    *image;

  int
    interlace,
    is_palette,
    status,
    y;

  int32
    height,
    length,
    width;

  register int
    i,
    x;

  register unsigned char
    *p;

  register RunlengthPacket
    *q;

  uint16
    reference;

  unsigned char
    *hdf_pixels;

  unsigned int
    packet_size;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  CloseImage(image);
  /*
    Read HDF image.
  */
  class=DirectClass;
  status=DF24getdims(image->filename,&width,&height,&interlace);
  if (status == -1)
    {
      class=PseudoClass;
      status=DFR8getdims(image->filename,&width,&height,&is_palette);
    }
  if (status == -1)
    PrematureExit(CorruptImageWarning,
      "Image file or does not contain any image data",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->class=class;
    image->columns=width;
    image->rows=height;
    if (image->class == PseudoClass)
      image->colors=256;
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    image->packets=image->columns*image->rows;
    packet_size=1;
    if (image->class == DirectClass)
      packet_size=3;
    hdf_pixels=(unsigned char *)
      AllocateMemory(packet_size*image->packets*sizeof(unsigned char));
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if ((hdf_pixels == (unsigned char *) NULL) ||
        (image->pixels == (RunlengthPacket *) NULL))
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    q=image->pixels;
    if (image->class == PseudoClass)
      {
        unsigned char
          *hdf_palette;

        /*
          Create colormap.
        */
        hdf_palette=(unsigned char *) AllocateMemory(768*sizeof(unsigned char));
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if ((hdf_palette == (unsigned char *) NULL) ||
            (image->colormap == (ColorPacket *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) DFR8getimage(image->filename,hdf_pixels,(int) image->columns,
          (int) image->rows,hdf_palette);
        reference=DFR8lastref();
        /*
          Convert HDF raster image to PseudoClass runlength-encoded packets.
        */
        p=hdf_palette;
        if (is_palette)
          for (i=0; i < 256; i++)
          {
            image->colormap[i].red=UpScale(*p++);
            image->colormap[i].green=UpScale(*p++);
            image->colormap[i].blue=UpScale(*p++);
          }
        else
          for (i=0; i < (int) image->colors; i++)
          {
            image->colormap[i].red=(Quantum) UpScale(i);
            image->colormap[i].green=(Quantum) UpScale(i);
            image->colormap[i].blue=(Quantum) UpScale(i);
          }
        FreeMemory((char *) hdf_palette);
        p=hdf_pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      {
        int
          y;

        /*
          Convert HDF raster image to DirectClass runlength-encoded packets.
        */
        (void) DF24getimage(image->filename,(void *) hdf_pixels,image->columns,
          image->rows);
        reference=DF24lastref();
        p=hdf_pixels;
        image->interlace=interlace ? PlaneInterlace : NoInterlace;
        q=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->index=0;
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      }
    length=DFANgetlablen(image->filename,DFTAG_RIG,reference);
    if (length > 0)
      {
        /*
          Read the image label.
        */
        length+=MaxTextExtent;
        image->label=(char *) AllocateMemory(length*sizeof(char));
        if (image->label != (char *) NULL)
          DFANgetlabel(image->filename,DFTAG_RIG,reference,image->label,length);
      }
    length=DFANgetdesclen(image->filename,DFTAG_RIG,reference);
    if (length > 0)
      {
        /*
          Read the image comments.
        */
        length+=MaxTextExtent;
        image->comments=(char *) AllocateMemory(length*sizeof(char));
        if (image->comments != (char *) NULL)
          DFANgetdesc(image->filename,DFTAG_RIG,reference,image->comments,
            length);
      }
    FreeMemory((char *) hdf_pixels);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    class=DirectClass;
    status=DF24getdims(image->filename,&width,&height,&interlace);
    if (status == -1)
      {
        class=PseudoClass;
        status=DFR8getdims(image->filename,&width,&height,&is_palette);
      }
    if (status != -1)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (status != -1);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}
#else
Image *ReadHDFImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"HDF library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d H I S T O G R A M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadHISTOGRAMImage reads a HISTOGRAM image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadHISTOGRAMImage routine is:
%
%      image=ReadHISTOGRAMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadHISTOGRAMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadHISTOGRAMImage(const ImageInfo *image_info)
{
  Image
    *image;

  image=ReadMIFFImage(image_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C C I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadICCImage reads an image file in the ICC format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the color profile information is useful in the
%  returned image.
%
%  The format of the ReadICCImage routine is:
%
%      image=ReadICCImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadICCImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadICCImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    c;

  register unsigned char
    *q;

  unsigned int
    length;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read ICC image.
  */
  length=MaxTextExtent;
  image->color_profile.info=(unsigned char *)
    AllocateMemory(length*sizeof(unsigned char));
  for (q=image->color_profile.info; ; q++)
  {
    c=fgetc(image->file);
    if (c == EOF)
      break;
    if ((q-image->color_profile.info+1) >= (int) length)
      {
        image->color_profile.length=q-image->color_profile.info;
        length<<=1;
        image->color_profile.info=(unsigned char *) ReallocateMemory((char *)
          image->color_profile.info,length*sizeof(unsigned char));
        if (image->color_profile.info == (unsigned char *) NULL)
          break;
        q=image->color_profile.info+image->color_profile.length;
      }
    *q=(unsigned char) c;
  }
  image->color_profile.length=0;
  if (image->color_profile.info != (unsigned char *) NULL)
    image->color_profile.length=q-image->color_profile.info;
  CloseImage(image);
  return(image);
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
%  Method ReadICONImage reads a Microsoft icon image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadICONImage routine is:
%
%      image=ReadICONImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadICONImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadICONImage(const ImageInfo *image_info)
{
#define MaxIcons  256

  typedef struct _IconEntry
  {
    unsigned char
      width,
      height,
      colors,
      reserved;

    short int
      planes,
      bits_per_pixel;

    unsigned int
      size,
      offset;
  } IconEntry;

  typedef struct _IconFile
  {
    short
      reserved,
      resource_type,
      count;

    IconEntry
      directory[MaxIcons];
  } IconFile;

  typedef struct _IconHeader
  {
    unsigned long
      size,
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
      colors_important;
  } IconHeader;

  IconFile
    icon_file;

  IconHeader
    icon_header;

  Image
    *image;

  register int
    bit,
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *icon_colormap,
    *icon_pixels;

  unsigned int
    bytes_per_line,
    offset;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  icon_file.reserved=LSBFirstReadShort(image->file);
  icon_file.resource_type=LSBFirstReadShort(image->file);
  icon_file.count=LSBFirstReadShort(image->file);
  if ((icon_file.reserved != 0) || (icon_file.resource_type != 1) ||
      (icon_file.count > MaxIcons))
    PrematureExit(CorruptImageWarning,"Not a ICO image file",image);
  for (i=0; i < icon_file.count; i++)
  {
    icon_file.directory[i].width=fgetc(image->file);
    icon_file.directory[i].height=fgetc(image->file);
    icon_file.directory[i].colors=fgetc(image->file);
    icon_file.directory[i].reserved=fgetc(image->file);
    icon_file.directory[i].planes=LSBFirstReadShort(image->file);
    icon_file.directory[i].bits_per_pixel=LSBFirstReadShort(image->file);
    icon_file.directory[i].size=LSBFirstReadLong(image->file);
    icon_file.directory[i].offset=LSBFirstReadLong(image->file);
  }
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify ICON identifier.
    */
    (void) fseek(image->file,icon_file.directory[i].offset,SEEK_SET);
    icon_header.size=LSBFirstReadLong(image->file);
    icon_header.width=LSBFirstReadLong(image->file);
    icon_header.height=LSBFirstReadLong(image->file);
    icon_header.planes=LSBFirstReadShort(image->file);
    icon_header.bits_per_pixel=LSBFirstReadShort(image->file);
    icon_header.compression=LSBFirstReadLong(image->file);
    icon_header.image_size=LSBFirstReadLong(image->file);
    icon_header.x_pixels=LSBFirstReadLong(image->file);
    icon_header.y_pixels=LSBFirstReadLong(image->file);
    icon_header.number_colors=LSBFirstReadLong(image->file);
    icon_header.colors_important=LSBFirstReadLong(image->file);
    image->columns=(unsigned int) icon_header.width;
    image->rows=(unsigned int) icon_header.height;
    image->class=PseudoClass;
    image->colors=image->colors=1 << icon_header.bits_per_pixel;
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    /*
      Allocate image colormap.
    */
    image->colormap=(ColorPacket *)
      AllocateMemory(image->colors*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Read ICON raster colormap.
    */
    icon_colormap=(unsigned char *)
      AllocateMemory(4*image->colors*sizeof(unsigned char));
    if (icon_colormap == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) ReadData((char *) icon_colormap,4,image->colors,image->file);
    p=icon_colormap;
    for (x=0; x < (int) image->colors; x++)
    {
      image->colormap[x].blue=UpScale(*p++);
      image->colormap[x].green=UpScale(*p++);
      image->colormap[x].red=UpScale(*p++);
      p++;
    }
    FreeMemory((char *) icon_colormap);
    /*
      Read image data.
    */
    icon_pixels=(unsigned char *)
      AllocateMemory(icon_file.directory[i].size*sizeof(unsigned char));
    if (icon_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) ReadData((char *) icon_pixels,1,icon_file.directory[i].size,
      image->file);
    /*
      Initialize image structure.
    */
    image->columns=icon_file.directory[i].width;
    image->rows=icon_file.directory[i].height;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert ICON raster image to runlength-encoded packets.
    */
    bytes_per_line=((image->columns*icon_header.bits_per_pixel+31)/32)*4;
    switch (icon_header.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=icon_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < ((int) image->columns-7); x+=8)
          {
            for (bit=0; bit < 8; bit++)
            {
              q->index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
              q->length=0;
              q++;
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (int) (image->columns % 8); bit++)
              {
                q->index=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
                q->length=0;
                q++;
              }
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 4:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=icon_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < ((int) image->columns-1); x+=2)
          {
            q->index=(*p >> 4) & 0xf;
            q->length=0;
            q++;
            q->index=(*p) & 0xf;
            q->length=0;
            p++;
            q++;
          }
          if ((image->columns % 2) != 0)
            {
              q->index=(*p >> 4) & 0xf;
              q->length=0;
              q++;
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=icon_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 16:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        if (icon_header.compression == 1)
          bytes_per_line=image->columns << 1;
        for (y=image->rows-1; y >= 0; y--)
        {
          p=icon_pixels+(image->rows-y-1)*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->index|=(*p++) << 8;
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Not a ICO image file",image);
    }
    SyncImage(image);
    /*
      Convert bitmap scanline to runlength-encoded color packets.
    */
    image->class=DirectClass;
    image->matte=True;
    offset=image->rows*bytes_per_line;
    bytes_per_line=((image->columns+31)/32)*4;
    for (y=image->rows-1; y >= 0; y--)
    {
      p=icon_pixels+offset+(image->rows-y-1)*bytes_per_line;
      q=image->pixels+(y*image->columns);
      for (x=0; x < ((int) image->columns-7); x+=8)
      {
        for (bit=0; bit < 8; bit++)
        {
          q->index=((*p) & (0x80 >> bit) ? Transparent : Opaque);
          q->length=0;
          q++;
        }
        p++;
      }
      if ((image->columns % 8) != 0)
        {
          for (bit=0; bit < (int) (image->columns % 8); bit++)
          {
            q->index=((*p) & (0x80 >> bit) ? Transparent : Opaque);
            q->length=0;
            q++;
          }
          p++;
        }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
    }
    FreeMemory((char *) icon_pixels);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (i < (icon_file.count-1))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e s                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadImages reads a list of image names from a file and then returns
%  the images as a linked list.
%
%  The format of the ReadImage routine is:
%
%      image=ReadImages(image_info)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
static Image *ReadImages(ImageInfo *image_info)
{
  char
    *command,
    **images;

  FILE
    *file;

  Image
    *image,
    *next_image;

  int
    c,
    length,
    number_images;

  register char
    *p;

  register int
    i;

  /*
    Read image list from a file.
  */
  file=(FILE *) fopen(image_info->filename+1,"r");
  if (file == (FILE *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unable to read image list",
        image_info->filename);
      return(False);
    }
  length=MaxTextExtent;
  command=(char *) AllocateMemory(length);
  for (p=command; command != (char *) NULL; p++)
  {
    c=fgetc(file);
    if (c == EOF)
      break;
    if ((p-command+1) >= length)
      {
        *p='\0';
        length<<=1;
        command=(char *) ReallocateMemory((char *) command,length);
        if (command == (char *) NULL)
          break;
        p=command+Extent(command);
      }
    *p=(unsigned char) c;
  }
  (void) fclose(file);
  if (command == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to read image list",
        "Memory allocation failed");
      return(False);
    }
  *p='\0';
  Strip(command);
  images=StringToArgv(command,&number_images);
  FreeMemory((char *) command);
  /*
    Read the images into a linked list.
  */
  image=(Image *) NULL;
  for (i=1; i < number_images; i++)
  {
    (void) strcpy(image_info->filename,images[i]);
    next_image=ReadImage(image_info);
    if (next_image == (Image *) NULL)
      continue;
    if (image == (Image *) NULL)
      image=next_image;
    else
      {
        register Image
          *q;

        /*
          Link image into image list.
        */
        for (q=image; q->next != (Image *) NULL; q=q->next);
        next_image->previous=q;
        q->next=next_image;
      }
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I P T C I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadIPTCImage reads an image file in the IPTC format and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  This method differs from the other decoder
%  methods in that only the iptc profile information is useful in the
%  returned image.
%
%  The format of the ReadIPTCImage routine is:
%
%      image=ReadIPTCImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadIPTCImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadIPTCImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    c;

  register unsigned char
    *q;

  unsigned int
    length;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read IPTC image.
  */
  length=MaxTextExtent;
  image->iptc_profile.info=(unsigned char *)
    AllocateMemory((length+2)*sizeof(unsigned char));
  for (q=image->iptc_profile.info; ; q++)
  {
    c=fgetc(image->file);
    if (c == EOF)
      break;
    if ((q-image->iptc_profile.info+1) >= (int) length)
      {
        image->iptc_profile.length=q-image->iptc_profile.info;
        length<<=1;
        image->iptc_profile.info=(unsigned char *) ReallocateMemory((char *)
          image->iptc_profile.info,(length+2)*sizeof(unsigned char));
        if (image->iptc_profile.info == (unsigned char *) NULL)
          break;
        q=image->iptc_profile.info+image->iptc_profile.length;
      }
    *q=(unsigned char) c;
  }
  image->iptc_profile.length=0;
  if (image->iptc_profile.info != (unsigned char *) NULL)
    image->iptc_profile.length=q-image->iptc_profile.info;
  CloseImage(image);
  return(image);
}

#if defined(HasJBIG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J B I G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJBIGImage reads a JBIG image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadJBIGImage routine is:
%
%      image=ReadJBIGImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJBIGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadJBIGImage(const ImageInfo *image_info)
{
#define MaxBufferSize  8192

  Image
    *image;

  int
    status,
    y;

  long
    length,
    packets;

  register int
    x;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  size_t
    count;

  struct jbg_dec_state
    jbig_info;

  unsigned char
    bit,
    *buffer;

  unsigned int
    byte;

  unsigned long
    max_packets;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize JBIG toolkit.
  */
  jbg_dec_init(&jbig_info);
  jbg_dec_maxsize(&jbig_info,(unsigned long) image->columns,
    (unsigned long) image->rows);
  image->columns=(unsigned int) jbg_dec_getwidth(&jbig_info);
  image->rows=(unsigned int) jbg_dec_getheight(&jbig_info);
  image->class=PseudoClass;
  image->colors=2;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Read JBIG file.
  */
  buffer=(unsigned char *) AllocateMemory(MaxBufferSize*sizeof(unsigned char));
  if (buffer == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  status=JBG_EAGAIN;
  do
  {
    length=(long) fread(buffer,1,MaxBufferSize,image->file);
    if (length == 0)
      break;
    p=buffer;
    count=0;
    while ((length > 0) && ((status == JBG_EAGAIN) || (status == JBG_EOK)))
    {
      status=jbg_dec_in(&jbig_info,p,length,&count);
      p+=count;
      length-=count;
    }
  } while ((status == JBG_EAGAIN) || (status == JBG_EOK));
  /*
    Initialize image structure.
  */
  image->columns=(unsigned int) jbg_dec_getwidth(&jbig_info);
  image->rows=(unsigned int) jbg_dec_getheight(&jbig_info);
  packets=0;
  max_packets=Max((image->columns*image->rows+8) >> 4,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      FreeMemory((char *) buffer);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  /*
    Create colormap.
  */
  image->class=PseudoClass;
  image->colors=2;
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      FreeMemory((char *) buffer);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  image->x_resolution=300;
  image->y_resolution=300;
  /*
    Convert X bitmap image to runlength-encoded packets.
  */
  byte=0;
  p=jbg_dec_getimage(&jbig_info,0);
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    bit=0;
    for (x=0; x < (int) image->columns; x++)
    {
      if (bit == 0)
        byte=(*p++);
      index=(byte & 0x80) ? 0 : 1;
      if ((index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  jbg_dec_free(&jbig_info);
                  FreeMemory((char *) buffer);
                  PrematureExit(ResourceLimitWarning,
                    "Memory allocation failed",image);
                }
              q=image->pixels+packets-1;
            }
          q->index=index;
          q->length=0;
        }
      bit++;
      byte<<=1;
      if (bit == 8)
        bit=0;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  SetRunlengthPackets(image,packets);
  SyncImage(image);
  /*
    Free scale resource.
  */
  jbg_dec_free(&jbig_info);
  FreeMemory((char *) buffer);
  CloseImage(image);
  return(image);
}
#else
Image *ReadJBIGImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"JBIG library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

#if defined(HasJPEG)
static Image
  *image;

static jmp_buf
  error_recovery;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J P E G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJPEGImage reads a JPEG image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadJPEGImage routine is:
%
%      image=ReadJPEGImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJPEGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the jpeg image to read.
%
%
*/

static unsigned int JPEGGetCharacter(j_decompress_ptr jpeg_info)
{
  struct jpeg_source_mgr
    *data;

  data=jpeg_info->src;
  if (data->bytes_in_buffer == 0)
    (*data->fill_input_buffer) (jpeg_info);
  data->bytes_in_buffer--;
  return(GETJOCTET(*data->next_input_byte++));
}

static boolean JPEGColorProfileHandler(j_decompress_ptr jpeg_info)
{
  char
    magick[12];

  long int
    length;

  register unsigned char
    *p;

  register int
    i;

  /*
    Determine length of color profile.
  */
  length=JPEGGetCharacter(jpeg_info) << 8;
  length+=JPEGGetCharacter(jpeg_info);
  length-=2;
  for (i=0; i < 12; i++)
    magick[i]=JPEGGetCharacter(jpeg_info);
  if (Latin1Compare(magick,"ICC_PROFILE") != 0)
    {
      /*
        Not a ICC profile, return.
      */
      for (i=0; i < length-12; i++)
        (void) JPEGGetCharacter(jpeg_info);
      return(True);
    }
  (void) JPEGGetCharacter(jpeg_info);  /* id */
  (void) JPEGGetCharacter(jpeg_info);  /* markers */
  length-=14;
  if (image->color_profile.length == 0)
    image->color_profile.info=(unsigned char *)
      AllocateMemory((unsigned int) length*sizeof(unsigned char));
  else
    image->color_profile.info=(unsigned char *) ReallocateMemory((char *)
      image->color_profile.info,(image->color_profile.length+length)*
      sizeof(unsigned char));
  if (image->color_profile.info == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  /*
    Read color profile.
  */
  p=image->color_profile.info+image->color_profile.length;
  image->color_profile.length+=length;
  while (--length >= 0)
    *p++=JPEGGetCharacter(jpeg_info);
  return(True);
}

static boolean JPEGCommentHandler(j_decompress_ptr jpeg_info)
{
  long int
    length;

  register char
    *p;

  /*
    Determine length of comment.
  */
  length=JPEGGetCharacter(jpeg_info) << 8;
  length+=JPEGGetCharacter(jpeg_info);
  length-=2;
  if (image->comments != (char *) NULL)
    image->comments=(char *) ReallocateMemory((char *) image->comments,
      (unsigned int) (Extent(image->comments)+length+1)*sizeof(char));
  else
    {
      image->comments=(char *)
        AllocateMemory((unsigned int) (length+1)*sizeof(char));
      if (image->comments != (char *) NULL)
        *image->comments='\0';
    }
  if (image->comments == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  /*
    Read comment.
  */
  p=image->comments+Extent(image->comments);
  while (--length >= 0)
    *p++=JPEGGetCharacter(jpeg_info);
  *p='\0';
  return(True);
}

static void JPEGEmitMessage(j_common_ptr jpeg_info,int level)
{
  char
    message[JMSG_LENGTH_MAX];

  struct jpeg_error_mgr
    *jpeg_error;

  jpeg_error=jpeg_info->err;
  (jpeg_error->format_message) (jpeg_info,message);
  if (level < 0)
    {
      if ((jpeg_error->num_warnings == 0) || (jpeg_error->trace_level >= 3))
        MagickWarning(DelegateWarning,(char *) message,image->filename);
      jpeg_error->num_warnings++;
    }
  else
    if (jpeg_error->trace_level >= level)
      MagickWarning(DelegateWarning,(char *) message,image->filename);
}

static void JPEGErrorExit(j_common_ptr jpeg_info)
{
  JPEGEmitMessage(jpeg_info,0);
  longjmp(error_recovery,1);
}

static boolean JPEGNewsProfileHandler(j_decompress_ptr jpeg_info)
{
  long int
    length;

  register unsigned char
    *p;

  unsigned char
    tag[2];

  /*
    Determine length of IPTC profile.
  */
  length=JPEGGetCharacter(jpeg_info) << 8;
  length+=JPEGGetCharacter(jpeg_info);
  length-=2;
  for (*tag='\0'; length > 0; )
  {
    *tag=JPEGGetCharacter(jpeg_info);
    *(tag+1)=JPEGGetCharacter(jpeg_info);
    length-=2;
    if ((*tag == 0x1c) && (*(tag+1) == 0x02))
      break;
  }
  if (length <= 0)
    return(True);
  if (image->iptc_profile.length != 0)
    image->iptc_profile.info=(unsigned char *) ReallocateMemory((char *)
      image->iptc_profile.info,(unsigned int) (image->iptc_profile.length+
      length+2)*sizeof(unsigned char));
  else
    {
      image->iptc_profile.info=(unsigned char *)
        AllocateMemory((unsigned int) (length+2)*sizeof(unsigned char));
      if (image->iptc_profile.info != (unsigned char *) NULL)
        image->iptc_profile.length=0;
    }
  if (image->iptc_profile.info == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  /*
    Read IPTC profile.
  */
  p=image->iptc_profile.info+image->iptc_profile.length;
  image->iptc_profile.length+=length+2;
  *p++=0x1c;
  *p++=0x02;
  while (--length >= 0)
    *p++=JPEGGetCharacter(jpeg_info);
  return(True);
}

Image *ReadJPEGImage(const ImageInfo *image_info)
{
  int
    x,
    y;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  Quantum
    blue,
    green,
    red;

  register int
    i;

  register JSAMPLE
    *p;

  register long
    packets;

  register RunlengthPacket
    *q;

  struct jpeg_decompress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  unsigned long
    max_packets;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=JPEGEmitMessage;
  jpeg_info.err->error_exit=JPEGErrorExit;
  image->pixels=(RunlengthPacket *) NULL;
  jpeg_pixels=(JSAMPLE *) NULL;
  if (setjmp(error_recovery))
    {
      /*
        JPEG image is corrupt.
      */
      if (jpeg_pixels != (JSAMPLE *) NULL)
        FreeMemory((char *) jpeg_pixels);
      jpeg_destroy_decompress(&jpeg_info);
      DestroyImage(image);
      return((Image *) NULL);
    }
  jpeg_create_decompress(&jpeg_info);
  jpeg_stdio_src(&jpeg_info,image->file);
  jpeg_set_marker_processor(&jpeg_info,JPEG_COM,JPEGCommentHandler);
  jpeg_set_marker_processor(&jpeg_info,ICC_MARKER,JPEGColorProfileHandler);
  jpeg_set_marker_processor(&jpeg_info,IPTC_MARKER,JPEGNewsProfileHandler);
  (void) jpeg_read_header(&jpeg_info,True);
  if (jpeg_info.saw_JFIF_marker)
    {
      /*
        Set image resolution.
      */
      image->x_resolution=jpeg_info.X_density;
      image->y_resolution=jpeg_info.Y_density;
      if (jpeg_info.density_unit == 1)
        image->units=PixelsPerInchResolution;
      if (jpeg_info.density_unit == 2)
        image->units=PixelsPerCentimeterResolution;
    }
  if ((image->columns*image->rows) != 0)
    {
      unsigned long
        scale_factor;

      /*
        Let the JPEG library subsample for us.
      */
      jpeg_calc_output_dimensions(&jpeg_info);
      image->magick_columns=jpeg_info.output_width;
      image->magick_rows=jpeg_info.output_height;
      scale_factor=UpShift(jpeg_info.output_width)/image->columns;
      if (scale_factor > (UpShift(jpeg_info.output_height)/image->rows))
        scale_factor=UpShift(jpeg_info.output_height)/image->rows;
      jpeg_info.scale_denom=DownShift(scale_factor);
      jpeg_calc_output_dimensions(&jpeg_info);
    }
  if (image_info->subrange != 0)
    {
      jpeg_info.scale_denom=DownShift(image_info->subrange);
      jpeg_calc_output_dimensions(&jpeg_info);
    }
#if (JPEG_LIB_VERSION >= 61)
  image->interlace=jpeg_info.progressive_mode ? PlaneInterlace : NoInterlace;
#endif
  jpeg_start_decompress(&jpeg_info);
  image->compression=JPEGCompression;
  image->columns=jpeg_info.output_width;
  image->rows=jpeg_info.output_height;
  if (image_info->ping)
    {
      jpeg_destroy_decompress(&jpeg_info);
      CloseImage(image);
      return(image);
    }
  packets=0;
  max_packets=Max((image->columns*image->rows+2) >> 2,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  jpeg_pixels=(JSAMPLE *)
    AllocateMemory(jpeg_info.output_components*image->columns*sizeof(JSAMPLE));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (jpeg_pixels == (JSAMPLE *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  if (jpeg_info.out_color_space == JCS_GRAYSCALE)
    {
      /*
        Initialize grayscale colormap.
      */
      image->class=PseudoClass;
      image->colors=MaxRGB+1;
      image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=UpScale(i);
        image->colormap[i].green=UpScale(i);
        image->colormap[i].blue=UpScale(i);
      }
    }
  /*
    Convert JPEG pixels to runlength-encoded packets.
  */
  red=0;
  green=0;
  blue=0;
  index=0;
  scanline[0]=(JSAMPROW) jpeg_pixels;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    (void) jpeg_read_scanlines(&jpeg_info,scanline,1);
    p=jpeg_pixels;
    for (x=0; x < (int) image->columns; x++)
    {
      if (jpeg_info.data_precision > QuantumDepth)
        {
          if (jpeg_info.out_color_space == JCS_GRAYSCALE)
            index=GETJSAMPLE(*p++) >> 4;
          else
            {
              red=(Quantum) (GETJSAMPLE(*p++) >> 4);
              green=(Quantum) (GETJSAMPLE(*p++) >> 4);
              blue=(Quantum) (GETJSAMPLE(*p++) >> 4);
              if (jpeg_info.out_color_space == JCS_CMYK)
                index=(Quantum) (GETJSAMPLE(*p++) >> 4);
            }
         }
       else
         if (jpeg_info.out_color_space == JCS_GRAYSCALE)
           index=GETJSAMPLE(*p++);
         else
           {
             red=(Quantum) UpScale(GETJSAMPLE(*p++));
             green=(Quantum) UpScale(GETJSAMPLE(*p++));
             blue=(Quantum) UpScale(GETJSAMPLE(*p++));
             if (jpeg_info.out_color_space == JCS_CMYK)
               index=(Quantum) UpScale(GETJSAMPLE(*p++));
           }
      if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
          (index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  FreeMemory((char *) jpeg_pixels);
                  jpeg_destroy_decompress(&jpeg_info);
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              q=image->pixels+packets-1;
            }
          q->red=red;
          q->green=green;
          q->blue=blue;
          q->index=index;
          q->length=0;
        }
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  SetRunlengthPackets(image,packets);
  if (jpeg_info.out_color_space == JCS_CMYK)
    TransformRGBImage(image,CMYKColorspace);
  else
    if (image->class == PseudoClass)
      SyncImage(image);
  /*
    Free jpeg resources..
  */
  (void) jpeg_finish_decompress(&jpeg_info);
  jpeg_destroy_decompress(&jpeg_info);
  FreeMemory((char *) jpeg_pixels);
  CloseImage(image);
  return(image);
}
#else
Image *ReadJPEGImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"JPEG library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d L A B E L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLABELImage reads a LABEL image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadLABELImage routine is:
%
%      image=ReadLABELImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLABELImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

static char *EscapeParenthesis(char *text)
{
  int
    escapes;

  register char
    *p;

  register int
    i;

  static char
    buffer[MaxTextExtent];

  escapes=0;
  p=buffer;
  for (i=0; i < Min((int) strlen(text),(MaxTextExtent-escapes-1)); i++)
  {
    if ((text[i] == '(') || (text[i] == ')'))
      {
        *p++='\\';
        escapes++;
      }
    *p++=text[i];
  }
  *p='\0';
  return(buffer);
}

#if defined(HasTTF)
static void GetFontInfo(TT_Face face,TT_Face_Properties *face_properties,
  Image *image)
{
  char
    *name;

  static const unsigned short
    ids[] = { 4, 5, 0, 7 };

  register char
    *p;

  register int
    i,
    j;

  unsigned short
    encoding,
    id,
    language,
    length,
    platform;

  /*
    Note font info as image comment.
  */
  if (face_properties->num_Names == 0)
    return;
  image->label=(char *)
    AllocateMemory((face_properties->num_Names*MaxTextExtent)*sizeof(char));
  if (image->label == (char *) NULL)
    return;
  *image->label='\0';
  for (i=0; i < (sizeof(ids)/sizeof(unsigned short)); i++)
  {
    TT_Get_Name_ID(face,ids[i],&platform,&encoding,&language,&id);
    if (((platform != 0) || (language != 0)) &&
        ((platform != 3) || (encoding > 1) || ((language & 0x3FF) != 0x009)))
      continue;
    TT_Get_Name_String(face,ids[i],&name,&length);
    p=image->label+strlen(image->label);
    for (j=1; j < (int) Min(length,MaxTextExtent); j+=2)
      *p++=name[j];
    *p='\0';
    break;
  }
  image->label=(char *) ReallocateMemory((char *)
    image->label,(strlen(image->label)+1)*sizeof(char));
}

static void RenderGlyph(TT_Raster_Map *canvas,TT_Raster_Map *character,
  TT_Glyph glyph,int x_off,int y_off,TT_Glyph_Metrics *glyph_metrics)
{
  int
    y;

  register int
    i,
    x;

  register unsigned char
    *p,
    *q;

  XSegment
    bounds;

  /*
    Render Glyph.
  */
  q=(unsigned char *) character->bitmap;
  for (i=0; i < character->size; i++)
    *q++=0;
  TT_Get_Glyph_Pixmap(glyph,character,-(glyph_metrics->bbox.xMin & -64),
    -(glyph_metrics->bbox.yMin & -64));
  /*
    Composite character on canvas.
  */
  x_off=((glyph_metrics->bbox.xMin & -64)/64)+x_off;
  y_off=(-(glyph_metrics->bbox.yMin & -64)/64)-y_off;
  bounds.x1=x_off < 0 ? -x_off : 0;
  bounds.y1=y_off < 0 ? -y_off : 0;
  bounds.x2=(int) canvas->cols-x_off;
  if (bounds.x2 > character->cols)
    bounds.x2=character->cols;
  bounds.y2=(int) canvas->rows-y_off;
  if (bounds.y2 > character->rows)
    bounds.y2=character->rows;
  if (bounds.x1 >= bounds.x2)
    return;
  for (y=bounds.y1; y < bounds.y2; y++)
  {
    p=((unsigned char *) character->bitmap)+y*character->cols+bounds.x1;
    q=((unsigned char *) canvas->bitmap)+(y+y_off)*canvas->cols+bounds.x1+x_off;
    for (x=bounds.x1; x < bounds.x2; x++)
      *q++|=(*p++);
  }
}
#endif

Image *ReadLABELImage(const ImageInfo *image_info)
{
#define MaxGlyphs  65535

  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    text[MaxTextExtent],
    page[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *local_info;

  int
    x,
    y;

  RectangleInfo
    crop_info;

  register int
    i,
    runlength;

  register RunlengthPacket
    *q;

  RunlengthPacket
    corner;

  XColor
    pen_color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Create image label.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  if (local_info->font == (char *) NULL)
    CloneString(&local_info->font,DefaultXFont);
  (void) strcpy(text,local_info->filename);
  (void) XQueryColorDatabase("black",&pen_color);
  if (local_info->pen != (char *) NULL)
    (void) XQueryColorDatabase(local_info->pen,&pen_color);
  if (*local_info->font == '@')
    {
#if defined(HasTTF)
      int
        character_map,
        length,
        number_glyphs;

      register int
        i;

      register unsigned char
        *p;

      TT_CharMap
        char_map;

      TT_Engine
        engine;

      TT_Error
        error;

      TT_Face
        face;

      TT_Face_Properties
        face_properties;

      TT_Glyph
        *glyphs;

      TT_Glyph_Metrics
        glyph_metrics;

      TT_Instance
        instance;

      TT_Instance_Metrics
        instance_metrics;

      TT_Raster_Map
        canvas,
        character;

      TT_UShort
        code;

      unsigned int
        height,
        width;

      unsigned short
        encoding,
        platform,
        *unicode;

      /*
        Initialize font engine.
      */
      error=TT_Init_FreeType(&engine);
      if (error)
        PrematureExit(DelegateWarning,"Cannot initialize TTF engine",image);
      error=TT_Open_Face(engine,local_info->font+1,&face);
      if (error)
        {
          /*
            Use default font.
          */
          MagickWarning(DelegateWarning,"Unable to open TTF font",
            local_info->font+1);
          DestroyImage(image);
          CloneString(&local_info->font,DefaultXFont);
          image=ReadLABELImage(local_info);
          DestroyImageInfo(local_info);
          return(image);
        }
      TT_Get_Face_Properties(face,&face_properties);
      if (strcmp(text,Alphabet) == 0)
        GetFontInfo(face,&face_properties,image);
      error=TT_New_Instance(face,&instance);
      if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
        {
          image->x_resolution=96;
          image->y_resolution=96;
        }
      error|=TT_Set_Instance_Resolutions(instance,(unsigned short)
        image->x_resolution,(unsigned short) image->y_resolution);
      error|=TT_Set_Instance_CharSize(instance,local_info->pointsize*64);
      if (error)
        PrematureExit(DelegateWarning,"Cannot initialize TTF instance",image);
      for (code=0; (int) code < (int) face_properties.num_CharMaps; code++)
      {
        TT_Get_CharMap_ID(face,code,&platform,&encoding);
        if (((platform == 3) && (encoding == 1)) ||
            ((platform == 0) && (encoding == 0)))
          {
            TT_Get_CharMap(face,code,&char_map);
            break;
          }
      }
      number_glyphs=0;
      character_map=True;
      if (code == face_properties.num_CharMaps)
        {
          TT_Get_Face_Properties(face,&face_properties);
          number_glyphs=face_properties.num_Glyphs;
          character_map=False;
        }
      glyphs=(TT_Glyph *) AllocateMemory(MaxGlyphs*sizeof(TT_Glyph));
      if (glyphs == (TT_Glyph *) NULL)
        PrematureExit(DelegateWarning,"Memory allocation failed",image);
      for (i=0; i < MaxGlyphs; i++)
        glyphs[i].z=(TT_Glyph *) NULL;
      unicode=ConvertTextToUnicode(text,&length);
      if (unicode == (unsigned short *) NULL)
        PrematureExit(DelegateWarning,"Memory allocation failed",image);
      for (i=0; i < length; i++)
      {
        if (glyphs[(unsigned char) unicode[i]].z != (TT_Glyph *) NULL)
          continue;
        if (character_map)
          code=TT_Char_Index(char_map,(unsigned char) unicode[i]);
        else
          {
            code=((int) unicode[i]-' '+1) < 0 ? 0 : (unicode[i]-' '+1);
            if ((int) code >= number_glyphs)
              code=0;
          }
        error=TT_New_Glyph(face,&glyphs[(unsigned char) unicode[i]]);
        error|=TT_Load_Glyph(instance,glyphs[(unsigned char) unicode[i]],code,
          TTLOAD_SCALE_GLYPH | TTLOAD_HINT_GLYPH);
        if (error)
          PrematureExit(DelegateWarning,"Cannot initialize TTF glyph",image);
      }
      TT_Get_Face_Properties(face,&face_properties);
      TT_Get_Instance_Metrics(instance,&instance_metrics);
      width=local_info->pointsize >> 1;
      height=((int) (face_properties.horizontal->Ascender*
        instance_metrics.y_ppem)/(int) face_properties.header->Units_Per_EM)-
        ((int) (face_properties.horizontal->Descender*instance_metrics.y_ppem)/
        (int) face_properties.header->Units_Per_EM)+1;
      for (i=0; i < length; i++)
      {
        if (glyphs[(unsigned char) unicode[i]].z == (TT_Glyph *) NULL)
          continue;
        TT_Get_Glyph_Metrics(glyphs[(unsigned char) unicode[i]],&glyph_metrics);
        width+=glyph_metrics.advance/64;
      }
      canvas.rows=height;
      canvas.width=(width+3) & -4;
      canvas.flow=TT_Flow_Down;
      canvas.cols=canvas.width;
      canvas.size=canvas.rows*canvas.width;
      canvas.bitmap=(void *) AllocateMemory(canvas.size);
      if (!canvas.bitmap)
        PrematureExit(DelegateWarning,"Memory allocation failed",image);
      p=(unsigned char *) canvas.bitmap;
      for (i=0; i < canvas.size; i++)
        *p++=0;
      character.rows=height;
      character.width=(instance_metrics.x_ppem+32+3) & -4;
      character.flow=TT_Flow_Down;
      character.cols=character.width;
      character.size=character.rows*character.width;
      character.bitmap=(void *) AllocateMemory(character.size);
      if (!character.bitmap)
        PrematureExit(DelegateWarning,"Memory allocation failed",image);
      x=0;
      y=((int) -(face_properties.horizontal->Descender*instance_metrics.y_ppem)/
        (int) face_properties.header->Units_Per_EM);
      for (i=0; i < length; i++)
      {
        if (glyphs[(unsigned char) unicode[i]].z == (TT_Glyph *) NULL)
          continue;
        TT_Get_Glyph_Metrics(glyphs[(unsigned char) unicode[i]],&glyph_metrics);
        RenderGlyph(&canvas,&character,glyphs[(unsigned char) unicode[i]],x,y,
          &glyph_metrics);
        x+=glyph_metrics.advance/64;
      }
      /*
        Render label with a TrueType font.
      */
      image->matte=True;
      image->columns=canvas.width;
      image->rows=canvas.rows;
      image->packets=image->columns*image->rows;
      image->pixels=(RunlengthPacket *)
        AllocateMemory(image->packets*sizeof(RunlengthPacket));
      if (image->pixels == (RunlengthPacket *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      p=(unsigned char *) canvas.bitmap;
      q=image->pixels;
      x=0;
      for (i=0; i < (int) image->packets; i++)
      {
        q->red=XDownScale(pen_color.red);
        q->green=XDownScale(pen_color.green);
        q->blue=XDownScale(pen_color.blue);
        q->index=(int) (Opaque*Min(*p,4))/4;
        if (q->index == Transparent)
          {
            q->red=(~q->red);
            q->green=(~q->green);
            q->blue=(~q->blue);
          }
        q->length=0;
        x++;
        if (x == image->columns)
          {
            if ((image->columns % 2) != 0)
              p++;
            x=0;
          }
        p++;
        q++;
      }
      /*
        Free TrueType resources.
      */
      FreeMemory((char *) canvas.bitmap);
      FreeMemory((char *) character.bitmap);
      for (i=0; i < MaxGlyphs; i++)
        TT_Done_Glyph(glyphs[i]);
      FreeMemory(glyphs);
      FreeMemory((char *) unicode);
      TT_Done_Instance(instance);
      TT_Close_Face(face);
      TT_Done_FreeType(engine);
      return(image);
#else
      MagickWarning(MissingDelegateWarning,"FreeType library is not available",
        (char *) NULL);
#endif
    }
  if (*local_info->font == '-')
    {
      int
        status;

      static Display
        *display = (Display *) NULL;

      static ImageInfo
        cache_info;

      static XAnnotateInfo
        annotate_info;

      static XFontStruct
        *font_info;

      static XPixelInfo
        pixel_info;

      static XResourceInfo
        resource_info;

      static XrmDatabase
        resource_database;

      static XStandardColormap
        *map_info;

      static XVisualInfo
        *visual_info;

      /*
        Allocate image structure.
      */
      if (display == (Display *) NULL)
        {
          /*
            Open X server connection.
          */
          display=XOpenDisplay(local_info->server_name);
          if (display != (Display *) NULL)
            {
              char
                *client_name;

              /*
                Get user defaults from X resource database.
              */
              XSetErrorHandler(XError);
              client_name=SetClientName((char *) NULL);
              resource_database=XGetResourceDatabase(display,client_name);
              XGetResourceInfo(resource_database,client_name,&resource_info);
              resource_info.close_server=False;
              resource_info.colormap=PrivateColormap;
              CloneString(&resource_info.font,local_info->font);
              resource_info.background_color="black";
              resource_info.foreground_color="white";
              map_info=XAllocStandardColormap();
              if (map_info == (XStandardColormap *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              /*
                Initialize visual info.
              */
              visual_info=XBestVisualInfo(display,map_info,&resource_info);
              if (visual_info == (XVisualInfo *) NULL)
                PrematureExit(XServerWarning,"Unable to get visual",image);
              map_info->colormap=(Colormap) NULL;
              pixel_info.pixels=(unsigned long *) NULL;
              pixel_info.gamma_map=(XColor *) NULL;
              /*
                Initialize Standard Colormap info.
              */
              XGetMapInfo(visual_info,XDefaultColormap(display,
                visual_info->screen),map_info);
              XGetPixelInfo(display,visual_info,map_info,&resource_info,
                (Image *) NULL,&pixel_info);
              pixel_info.annotate_context=
                XDefaultGC(display,visual_info->screen);
              /*
                Initialize font info.
              */
              font_info=XBestFont(display,&resource_info,False);
              if (font_info == (XFontStruct *) NULL)
                PrematureExit(XServerWarning,"Unable to load font",image);
              if ((map_info == (XStandardColormap *) NULL) ||
                  (visual_info == (XVisualInfo *) NULL) ||
                  (font_info == (XFontStruct *) NULL))
                {
                  XFreeResources(display,visual_info,map_info,&pixel_info,
                    font_info,&resource_info,(XWindowInfo *) NULL);
                  display=(Display *) NULL;
                }
              cache_info=(*local_info);
            }
        }
      if (display == (Display *) NULL)
        {
          /*
            Use default font.
          */
          MagickWarning(XServerWarning,"Unable to open X server",
            local_info->server_name);
          DestroyImage(image);
          CloneString(&local_info->font,DefaultPSFont);
          image=ReadLABELImage(local_info);
          DestroyImageInfo(local_info);
          return(image);
        }
      /*
        Initialize annotate info.
      */
      XGetAnnotateInfo(&annotate_info);
      annotate_info.stencil=OpaqueStencil;
      if (cache_info.font != local_info->font)
        {
          /*
            Font name has changed.
          */
          XFreeFont(display,font_info);
          CloneString(&resource_info.font,local_info->font);
          font_info=XBestFont(display,&resource_info,False);
          if (font_info == (XFontStruct *) NULL)
            PrematureExit(ResourceLimitWarning,"Unable to load font",image);
        }
      annotate_info.font_info=font_info;
      annotate_info.text=text;
      annotate_info.width=TextWidth(font_info,text,Extent(text));
      annotate_info.height=font_info->ascent+font_info->descent;
      (void) sprintf(annotate_info.geometry,"%ux%u+0+0",annotate_info.width,
        annotate_info.height);
      cache_info=(*local_info);
      /*
        Render label with a X11 server font.
      */
      image->columns=annotate_info.width;
      image->rows=annotate_info.height;
      image->packets=image->columns*image->rows;
      image->pixels=(RunlengthPacket *)
        AllocateMemory(image->packets*sizeof(RunlengthPacket));
      if (image->pixels == (RunlengthPacket *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      image->background_color=image->border_color;
      status=XAnnotateImage(display,&pixel_info,&annotate_info,image);
      if (status == 0)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      image->matte=True;
      q=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        q->index=Intensity(*q);
        q->red=XDownScale(pen_color.red);
        q->green=XDownScale(pen_color.green);
        q->blue=XDownScale(pen_color.blue);
        if (q->index == Transparent)
          {
            q->red=(~q->red);
            q->green=(~q->green);
            q->blue=(~q->blue);
          }
        q++;
      }
      return(image);
    }
  /*
    Render label with a Postscript font.
  */
  local_info->density=(char *) NULL;
  (void) sprintf(page,"%ux%u+0+0!",local_info->pointsize*Extent(text),
    local_info->pointsize << 1);
  CloneString(&local_info->page,page);
  TemporaryFilename(filename);
  file=fopen(filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) fprintf(file,"%%!PS-Adobe-3.0\n");
  (void) fprintf(file,"/Helvetica findfont %u scalefont setfont\n",
    local_info->pointsize);
  (void) fprintf(file,"/%.1024s findfont %u scalefont setfont\n",
    local_info->font,local_info->pointsize);
  (void) fprintf(file,"0.0 0.0 0.0 setrgbcolor\n");
  (void) fprintf(file,"0 0 %u %u rectfill\n",
    local_info->pointsize*Extent(text),local_info->pointsize << 1);
  (void) fprintf(file,"1.0 1.0 1.0 setrgbcolor\n");
  (void) fprintf(file,"0 %u moveto (%.1024s) show\n",local_info->pointsize,
    EscapeParenthesis(text));
  (void) fprintf(file,"showpage\n");
  (void) fclose(file);
  (void) strcpy(local_info->filename,filename);
  DestroyImage(image);
  image=ReadPSImage(local_info);
  (void) remove(filename);
  /*
    Set bounding box to the image dimensions.
  */
  crop_info.width=0;
  crop_info.height=local_info->pointsize;
  crop_info.x=0;
  crop_info.y=local_info->pointsize >> 2;
  DestroyImageInfo(local_info);
  if (image == (Image *) NULL)
    return(image);
  q=image->pixels;
  runlength=q->length+1;
  corner.red=0;
  corner.green=0;
  corner.blue=0;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          q++;
          runlength=q->length;
        }
      if (!ColorMatch(*q,corner,0))
        if (x > (int) crop_info.width)
          crop_info.width=x;
    }
  }
  crop_info.width++;
  (void) sprintf(geometry,"%ux%u%+d%+d",crop_info.width,crop_info.height,
    crop_info.x,crop_info.y);
  TransformImage(&image,geometry,(char *) NULL);
  image->matte=True;
  q=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    q->index=Intensity(*q);
    q->red=XDownScale(pen_color.red);
    q->green=XDownScale(pen_color.green);
    q->blue=XDownScale(pen_color.blue);
    if (q->index == Transparent)
      {
        q->red=(~q->red);
        q->green=(~q->green);
        q->blue=(~q->blue);
      }
    q++;
  }
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d L O G O I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLOGOImage reads a LOGO image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadLOGOImage routine is:
%
%      image=ReadLOGOImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLOGOImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadLOGOImage(const ImageInfo *image_info)
{
#include "logo.h"

  char
    filename[MaxTextExtent];

  FILE
    *file;

  Image
    *image;

  ImageInfo
    *local_info;

  register const unsigned char
    *p;

  register int
    i;

  unsigned int
    extent;

  /*
    Open temporary output file.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  (void) strcpy(filename,local_info->filename);
  TemporaryFilename(local_info->filename);
  file=fopen(local_info->filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unable to write file",
        local_info->filename);
      DestroyImageInfo(local_info);
      return(ReadXCImage(image_info));
    }
  p=LogoImage;
  extent=LogoImageExtent;
  if (Latin1Compare(local_info->magick,"GRANITE") == 0)
    {
      p=GraniteImage;
      extent=GraniteImageExtent;
    }
  if (Latin1Compare(local_info->magick,"NETSCAPE") == 0)
    {
      p=NetscapeImage;
      extent=NetscapeImageExtent;
    }
  for (i=0; i < (int) extent; i++)
  {
    (void) fputc((char) *p,file);
    p++;
  }
  if (ferror(file))
    {
      MagickWarning(FileOpenWarning,"An error has occurred writing to file",
        filename);
      (void) fclose(file);
      (void) remove(filename);
      DestroyImageInfo(local_info);
      return(ReadXCImage(image_info));
    }
  (void) fclose(file);
  image=ReadGIFImage(local_info);
  (void) remove(local_info->filename);
  if (image != (Image *) NULL)
    (void) strcpy(image->filename,filename);
  DestroyImageInfo(local_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M A P I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMAPImage reads an image of raw RGB colormap and colormap index
%  bytes and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadMAPImage routine is:
%
%      image=ReadMAPImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMAPImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadMAPImage(const ImageInfo *image_info)
{
  Image
    *image;

  register int
    i;

  register unsigned char
    *p;

  unsigned char
    *colormap;

  unsigned int
    packet_size,
    status;

  unsigned short
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize image structure.
  */
  image->class=PseudoClass;
  image->compression=NoCompression;
  image->colors=image->offset;
  image->packets=image->columns*image->rows;
  packet_size=3*(image->depth >> 3);
  colormap=(unsigned char *)
    AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  image->packed_pixels=(unsigned char *)
    AllocateMemory(image->packets*packet_size*sizeof(unsigned char));
  if ((colormap == (unsigned char *) NULL) ||
      (image->colormap == (ColorPacket *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read image colormap.
  */
  (void) ReadData((char *) colormap,packet_size,image->colors,image->file);
  p=colormap;
  for (i=0; i < (int) image->colors; i++)
  {
    ReadQuantum(image->colormap[i].red,p);
    ReadQuantum(image->colormap[i].green,p);
    ReadQuantum(image->colormap[i].blue,p);
  }
  FreeMemory((char *) colormap);
  /*
    Convert raster image to runlength-encoded packets.
  */
  packet_size=1;
  if (image->colors > 256)
    packet_size++;
  if (image->packed_pixels != (unsigned char *) NULL)
    FreeMemory((char *) image->packed_pixels);
  image->packed_pixels=(unsigned char *)
    AllocateMemory(image->packets*packet_size);
  if (image->packed_pixels == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  (void) ReadData((char *) image->packed_pixels,packet_size,image->packets,
    image->file);
  status=RunlengthDecodeImage(image);
  if (status == False)
    {
      DestroyImages(image);
      return((Image *) NULL);
    }
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMIFFImage reads a MIFF image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadMIFFImage routine is:
%
%      image=ReadMIFFImage(filename)
%
%  A description of each parameter follows:
%
%    o image: Method ReadMIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadMIFFImage(const ImageInfo *image_info)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  register int
    c,
    i;

  register unsigned char
    *p;

  unsigned int
    length,
    packet_size,
    status;

  unsigned long
    count,
    max_packets;

  XColor
    color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  image->depth=8;
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  c=fgetc(image->file);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    image->compression=NoCompression;
    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          /*
            Read comment-- any text between { }.
          */
          if (image->comments != (char *) NULL)
            {
              length=Extent(image->comments);
              p=image->comments+length;
            }
          else
            {
              length=MaxTextExtent;
              image->comments=(char *) AllocateMemory(length*sizeof(char));
              p=image->comments;
            }
          for ( ; image->comments != (char *) NULL; p++)
          {
            c=fgetc(image->file);
            if ((c == EOF) || (c == '}'))
              break;
            if ((p-image->comments+1) >= (int) length)
              {
                *p='\0';
                length<<=1;
                image->comments=(char *) ReallocateMemory((char *)
                  image->comments,length*sizeof(char));
                if (image->comments == (char *) NULL)
                  break;
                p=image->comments+Extent(image->comments);
              }
            *p=(unsigned char) c;
          }
          if (image->comments == (char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          *p='\0';
          c=fgetc(image->file);
        }
      else
        if (isalnum(c))
          {
            /*
              Determine a keyword and its value.
            */
            p=keyword;
            do
            {
              if ((p-keyword) < (MaxTextExtent-1))
                *p++=(char) c;
              c=fgetc(image->file);
            } while (isalnum(c) || (c == '-'));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=fgetc(image->file);
            p=value;
            if (c != '"')
              while (!isspace(c) && (c != EOF))
              {
                if ((p-value) < (MaxTextExtent-1))
                  *p++=(char) c;
                c=fgetc(image->file);
              }
            else
              {
                c=fgetc(image->file);
                while ((c != '"') && (c != EOF))
                {
                  if ((p-value) < (MaxTextExtent-1))
                    *p++=(char) c;
                  c=fgetc(image->file);
                }
              }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            if (Latin1Compare(keyword,"background-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->background_color.red=XDownScale(color.red);
                image->background_color.green=XDownScale(color.green);
                image->background_color.blue=XDownScale(color.blue);
                image->background_color.index=0;
              }
            if (Latin1Compare(keyword,"blue-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.blue_primary.x,
                &image->chromaticity.blue_primary.y);
            if (Latin1Compare(keyword,"border-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->border_color.red=XDownScale(color.red);
                image->border_color.green=XDownScale(color.green);
                image->border_color.blue=XDownScale(color.blue);
                image->border_color.index=0;
              }
            if (Latin1Compare(keyword,"class") == 0)
              {
                if (Latin1Compare(value,"PseudoClass") == 0)
                  image->class=PseudoClass;
                else
                  if (Latin1Compare(value,"DirectClass") == 0)
                    image->class=DirectClass;
                  else
                    image->class=UndefinedClass;
              }
            if (Latin1Compare(keyword,"colors") == 0)
              image->colors=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"color-profile") == 0)
              image->color_profile.length=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"compression") == 0)
              {
                if (Latin1Compare(value,"Zip") == 0)
                  image->compression=ZipCompression;
                else
                  if (Latin1Compare(value,"BZip") == 0)
                    image->compression=BZipCompression;
                  else
                    if (Latin1Compare(value,"RunlengthEncoded") == 0)
                      image->compression=RunlengthEncodedCompression;
                    else
                      image->compression=UndefinedCompression;
              }
            if (Latin1Compare(keyword,"columns") == 0)
              image->columns=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"delay") == 0)
              {
                if (image_info->delay == (char *) NULL)
                  image->delay=atoi(value);
              }
            if (Latin1Compare(keyword,"depth") == 0)
              image->depth=atoi(value) <= 8 ? 8 : 16;
            if (Latin1Compare(keyword,"dispose") == 0)
              {
              if (image_info->dispose == (char *) NULL)
                image->dispose=atoi(value);
              }
            if (Latin1Compare(keyword,"gamma") == 0)
              image->gamma=atof(value);
            if (Latin1Compare(keyword,"green-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.green_primary.x,
                &image->chromaticity.green_primary.y);
            if (Latin1Compare(keyword,"id") == 0)
              {
                if (Latin1Compare(value,"ImageMagick") == 0)
                  image->id=ImageMagickId;
                else
                  image->id=UndefinedId;
              }
            if (Latin1Compare(keyword,"iterations") == 0)
              {
                if (image_info->iterations == (char *) NULL)
                  image->iterations=atoi(value);
              }
            if (Latin1Compare(keyword,"label") == 0)
              CloneString(&image->label,value);
            if ((Latin1Compare(keyword,"matte") == 0) ||
                (Latin1Compare(keyword,"alpha") == 0))
              {
                if ((Latin1Compare(value,"True") == 0) ||
                    (Latin1Compare(value,"true") == 0))
                  image->matte=True;
                else
                  image->matte=False;
              }
            if (Latin1Compare(keyword,"matte-color") == 0)
              {
                (void) XQueryColorDatabase(value,&color);
                image->matte_color.red=XDownScale(color.red);
                image->matte_color.green=XDownScale(color.green);
                image->matte_color.blue=XDownScale(color.blue);
                image->matte_color.index=0;
              }
            if (Latin1Compare(keyword,"montage") == 0)
              CloneString(&image->montage,value);
            if (Latin1Compare(keyword,"page") == 0)
              {
                if (image_info->page == (char *) NULL)
                  image->page=PostscriptGeometry(value);
              }
            if (Latin1Compare(keyword,"packets") == 0)
              image->packets=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"red-primary") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.red_primary.x,
                &image->chromaticity.red_primary.y);
            if (Latin1Compare(keyword,"rendering-intent") == 0)
              {
                if (Latin1Compare(value,"saturation") == 0)
                  image->rendering_intent=SaturationIntent;
                else
                  if (Latin1Compare(value,"perceptual") == 0)
                    image->rendering_intent=PerceptualIntent;
                  else
                    if (Latin1Compare(value,"absolute") == 0)
                      image->rendering_intent=AbsoluteIntent;
                    else
                      if (Latin1Compare(value,"relative") == 0)
                        image->rendering_intent=RelativeIntent;
                      else
                        image->rendering_intent=UndefinedIntent;
              }
            if (Latin1Compare(keyword,"resolution") == 0)
              (void) sscanf(value,"%lfx%lf",&image->x_resolution,
                &image->y_resolution);
            if (Latin1Compare(keyword,"rows") == 0)
              image->rows=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"scene") == 0)
              image->scene=(unsigned int) atoi(value);
            if (Latin1Compare(keyword,"signature") == 0)
              CloneString(&image->signature,value);
            if (Latin1Compare(keyword,"units") == 0)
              {
                if (Latin1Compare(value,"undefined") == 0)
                  image->units=UndefinedResolution;
                else
                  if (Latin1Compare(value,"pixels-per-inch") == 0)
                    image->units=PixelsPerInchResolution;
                  else
                    if (Latin1Compare(value,"pixels-per-centimeter") == 0)
                      image->units=PixelsPerCentimeterResolution;
              }
            if (Latin1Compare(keyword,"white-point") == 0)
              (void) sscanf(value,"%f,%f",&image->chromaticity.white_point.x,
                &image->chromaticity.white_point.y);
          }
        else
          c=fgetc(image->file);
      while (isspace(c))
        c=fgetc(image->file);
    }
    (void) fgetc(image->file);
    /*
      Verify that required image information is defined.
    */
    if ((image->id == UndefinedId) || (image->class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      PrematureExit(CorruptImageWarning,"Incorrect image header in file",image);
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        image->directory=(char *) AllocateMemory(MaxTextExtent*sizeof(char));
        if (image->directory == (char *) NULL)
          PrematureExit(CorruptImageWarning,"Unable to read image data",image);
        p=image->directory;
        do
        {
          *p='\0';
          if (((Extent(image->directory)+1) % MaxTextExtent) == 0)
            {
              /*
                Allocate more memory for the image directory.
              */
              image->directory=(char *) ReallocateMemory((char *)
                image->directory,(Extent(image->directory)+MaxTextExtent+1)*
                sizeof(char));
              if (image->directory == (char *) NULL)
                PrematureExit(CorruptImageWarning,"Unable to read image data",
                  image);
              p=image->directory+Extent(image->directory);
            }
          c=fgetc(image->file);
          *p++=(unsigned char) c;
        } while (c != '\0');
      }
    if (image->color_profile.length > 0)
      {
        /*
          Color profile.
        */
        image->color_profile.info=(unsigned char *)
          AllocateMemory(image->color_profile.length*sizeof(unsigned char));
        if (image->color_profile.info == (unsigned char *) NULL)
          PrematureExit(CorruptImageWarning,"Unable to read color profile",
            image);
        (void) fread(image->color_profile.info,1,image->color_profile.length,
          image->file);
      }
    if (image->class == PseudoClass)
      {
        unsigned int
          colors;

        unsigned short
          value;

        /*
          PseudoClass image cannot have matte data.
        */
        if (image->matte)
          PrematureExit(CorruptImageWarning,
            "Matte images must be DirectClass",image);
        /*
          Create image colormap.
        */
        colors=image->colors;
        if (colors == 0)
          colors=256;
        image->colormap=(ColorPacket *)
          AllocateMemory(colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        if (image->colors == 0)
          for (i=0; i < (int) colors; i++)
          {
            image->colormap[i].red=(Quantum) UpScale(i);
            image->colormap[i].green=(Quantum) UpScale(i);
            image->colormap[i].blue=(Quantum) UpScale(i);
            image->colors++;
          }
        else
          {
            unsigned char
              *colormap;

            /*
              Read image colormap from file.
            */
            packet_size=3*(image->depth >> 3);
            colormap=(unsigned char *)
              AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
            if (colormap == (unsigned char *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            (void) ReadData((char *) colormap,1,packet_size*image->colors,
              image->file);
            p=colormap;
            for (i=0; i < (int) image->colors; i++)
            {
              ReadQuantum(image->colormap[i].red,p);
              ReadQuantum(image->colormap[i].green,p);
              ReadQuantum(image->colormap[i].blue,p);
            }
            FreeMemory((char *) colormap);
          }
      }
    /*
      Determine packed packet size.
    */
    if (image->class == PseudoClass)
      {
        image->packet_size=1;
        if (image->colors > 256)
          image->packet_size++;
      }
    else
      {
        image->packet_size=3*(image->depth >> 3);
        if (image->matte)
          image->packet_size++;
      }
    if (image->compression == RunlengthEncodedCompression)
      image->packet_size++;
    packet_size=image->packet_size;
    if (image->compression == ZipCompression)
      packet_size=1;
    /*
      Allocate image pixels.
    */
    if (image->compression == NoCompression)
      image->packets=image->columns*image->rows;
    max_packets=image->packets;
    if (image->packets == 0)
      max_packets=image->columns*image->rows;
    image->packed_pixels=(unsigned char *) AllocateMemory((unsigned int)
      max_packets*packet_size*sizeof(unsigned char));
    if (image->packed_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Read image pixels from file.
    */
    if ((image->compression != RunlengthEncodedCompression) ||
        (image->packets != 0))
      (void) ReadData((char *) image->packed_pixels,1,(unsigned int)
        max_packets*packet_size,image->file);
    else
      {
        /*
          Number of runlength packets is unspecified.
        */
        count=0;
        p=image->packed_pixels;
        do
        {
          (void) ReadData((char *) p,1,packet_size,image->file);
          image->packets++;
          p+=(packet_size-1);
          count+=(*p+1);
          p++;
        }
        while (count < (image->columns*image->rows));
      }
    if (image->compression == BZipCompression)
      {
        unsigned char
          *compressed_pixels;

        /*
          Uncompress image pixels with BZip decoding.
        */
        compressed_pixels=image->packed_pixels;
        max_packets=image->columns*image->rows*image->packet_size;
        image->packed_pixels=(unsigned char *)
          AllocateMemory((max_packets+8)*sizeof(unsigned char));
        if (image->packed_pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasBZLIB)
        {
          unsigned int
            compressed_packets;

          compressed_packets=max_packets;
          status=bzBuffToBuffDecompress((char *) image->packed_pixels,
            &compressed_packets,(char *) compressed_pixels,image->packets,
            image_info->verbose,False);
          max_packets=compressed_packets;
        }
#endif
        image->packets=(unsigned int) (max_packets/image->packet_size);
        FreeMemory((char *) compressed_pixels);
        if (status)
          PrematureExit(DelegateWarning,"Unable to uncompress image",image);
      }
    if (image->compression == ZipCompression)
      {
        unsigned char
          *compressed_pixels;

        /*
          Uncompress image pixels with Zip decoding.
        */
        compressed_pixels=image->packed_pixels;
        max_packets=image->columns*image->rows*image->packet_size;
        image->packed_pixels=(unsigned char *)
          AllocateMemory((max_packets+8)*sizeof(unsigned char));
        if (image->packed_pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasZLIB)
        status=uncompress(image->packed_pixels,&max_packets,compressed_pixels,
          image->packets);
#endif
        image->packets=(unsigned int) (max_packets/image->packet_size);
        FreeMemory((char *) compressed_pixels);
        if (status)
          PrematureExit(DelegateWarning,"Unable to uncompress image",image);
      }
    /*
      Unpack the packed image pixels into runlength-encoded pixel packets.
    */
    status=RunlengthDecodeImage(image);
    if (status == False)
      {
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    do
    {
      c=fgetc(image->file);
    } while (!isgraph(c) && (c != EOF));
    if (c != EOF)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (c != EOF);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M O N O I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMONOImage reads an image of raw bites in LSB order and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadMONOImage routine is:
%
%      image=ReadMONOImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMONOImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadMONOImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    y;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  unsigned char
    bit,
    byte;

  unsigned long
    max_packets;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < image->offset; i++)
    (void) fgetc(image->file);
  /*
    Initialize image structure.
  */
  packets=0;
  max_packets=Max((image->columns*image->rows+8) >> 4,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  image->class=PseudoClass;
  image->colors=2;
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->colormap == (ColorPacket *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
    image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
    image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
  }
  /*
    Convert bi-level image to runlength-encoded packets.
  */
  for (y=0; y < (int) ((image->rows-image->tile_info.y*image->columns+7) >> 3); y++)
    (void) fgetc(image->file);
  byte=0;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    bit=0;
    for (x=0; y < ((image->tile_info.x+7) >> 3); x++)
      (void) fgetc(image->file);
    for (x=0; x < (int) image->columns; x++)
    {
      if (bit == 0)
        byte=fgetc(image->file);
      index=(byte & 0x01) ? 0 : 1;
      if ((index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              q=image->pixels+packets-1;
            }
          q->index=index;
          q->length=0;
        }
      bit++;
      if (bit == 8)
        bit=0;
      byte>>=1;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  SetRunlengthPackets(image,packets);
  SyncImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d M T V I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadMTVImage reads a MTV image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadMTVImage routine is:
%
%      image=ReadMTVImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadMTVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadMTVImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    count,
    y;

  Quantum
    blue,
    green,
    red;

  register int
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  unsigned int
    columns,
    rows;

  unsigned long
    max_packets;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read MTV image.
  */
  count=fscanf(image->file,"%u %u\n",&columns,&rows);
  if (count == 0)
    PrematureExit(CorruptImageWarning,"Not a MTV image file",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=columns;
    image->rows=rows;
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert MTV raster image to runlength-encoded packets.
    */
    q=image->pixels;
    SetRunlengthEncoder(q);
    for (y=0; y < (int) image->rows; y++)
    {
      for (x=0; x < (int) image->columns; x++)
      {
        red=UpScale(fgetc(image->file));
        green=UpScale(fgetc(image->file));
        blue=UpScale(fgetc(image->file));
        if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
            ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,max_packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                q=image->pixels+packets-1;
              }
            q->red=red;
            q->green=green;
            q->blue=blue;
            q->index=0;
            q->length=0;
          }
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    SetRunlengthPackets(image,packets);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=fscanf(image->file,"%u %u\n",&columns,&rows);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (count > 0);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d N U L L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadNULLImage reads a NULL image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadNULLImage routine is:
%
%      image=ReadNULLImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadNULLImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadNULLImage(const ImageInfo *image_info)
{
  return(ReadXCImage(image_info));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P C D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCDImage reads a Photo CD image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.  Much of the PCD decoder was derived from
%  the program hpcdtoppm(1) by Hadmut Danisch.
%
%  The format of the ReadPCDImage routine is:
%
%      image=ReadPCDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
static Image *OverviewImage(const ImageInfo *image_info,Image *image)
{
  char
    *commands[3];

  Image
    *montage_image;

  ImageInfo
    *local_info;

  MontageInfo
    montage_info;

  /*
    Create image tiles.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  commands[0]=SetClientName((char *) NULL);
  commands[1]="-label";
  commands[2]=DefaultTileLabel;
  MogrifyImages(local_info,3,commands,&image);
  DestroyImageInfo(local_info);
  /*
    Create the PCD Overview image.
  */
  GetMontageInfo(&montage_info);
  (void) strcpy(montage_info.filename,image_info->filename);
  CloneString(&montage_info.font,image_info->font);
  montage_info.pointsize=image_info->pointsize;
  montage_image=MontageImages(image,&montage_info);
  DestroyMontageInfo(&montage_info);
  if (montage_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  DestroyImage(image);
  return(montage_image);
}

Image *ReadPCDImage(const ImageInfo *image_info)
{
  Image
    *image;

  long int
    offset;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned char
    *c1,
    *c2,
    *y;

  unsigned char
    *chroma1,
    *chroma2,
    *header,
    *luma;

  unsigned int
    height,
    number_images,
    overview,
    rotate,
    status,
    subimage,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a PCD file.
  */
  header=(unsigned char *) AllocateMemory(3*0x800*sizeof(unsigned char));
  if (header == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  status=ReadData((char *) header,1,3*0x800,image->file);
  overview=strncmp((char *) header,"PCD_OPA",7) == 0;
  if ((status == False) ||
      ((strncmp((char *) header+0x800,"PCD",3) != 0) && !overview))
    PrematureExit(CorruptImageWarning,"Not a PCD image file",image);
  rotate=header[0x0e02] & 0x03;
  number_images=(header[10] << 8) | header[11];
  FreeMemory((char *) header);
  /*
    Determine resolution by subimage specification.
  */
  if ((image->columns*image->rows) == 0)
    subimage=3;
  else
    {
      width=192;
      height=128;
      for (subimage=1; subimage < 6; subimage++)
      {
        if ((width >= image->columns) && (height >= image->rows))
          break;
        width<<=1;
        height<<=1;
      }
    }
  if (image_info->subrange != 0)
    subimage=Min(image_info->subimage,6);
  if (overview)
    subimage=1;
  /*
    Initialize image structure.
  */
  width=192;
  height=128;
  for (i=1; i < Min((int) subimage,3); i++)
  {
    width<<=1;
    height<<=1;
  }
  image->columns=width;
  image->rows=height;
  for ( ; i < (int) subimage; i++)
  {
    image->columns<<=1;
    image->rows<<=1;
  }
  if (image_info->ping)
    {
      if ((rotate == 1) || (rotate == 3))
        Swap(image->columns,image->rows);
      CloseImage(image);
      return(image);
    }
  /*
    Allocate luma and chroma memory.
  */
  image->packets=image->columns*image->rows;
  chroma1=(unsigned char *)
    AllocateMemory((image->packets+1)*sizeof(unsigned char));
  chroma2=(unsigned char *)
    AllocateMemory((image->packets+1)*sizeof(unsigned char));
  luma=(unsigned char *)
    AllocateMemory((image->packets+1)*sizeof(unsigned char));
  if ((chroma1 == (unsigned char *) NULL) ||
      (chroma2 == (unsigned char *) NULL) || (luma == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Advance to image data.
  */
  offset=93;
  if (overview)
    offset=2;
  else
    if (subimage == 2)
      offset=20;
    else
      if (subimage <= 1)
        offset=1;
  for (i=0; i < (offset*0x800); i++)
    (void) fgetc(image->file);
  if (overview)
    {
      Image
        *overview_image;

      MonitorHandler
        handler;

      register int
        j;

      /*
        Read thumbnails from overview image.
      */
      for (j=1; j <= (int) number_images; j++)
      {
        handler=SetMonitorHandler((MonitorHandler) NULL);
        FormatString(image->filename,"images/img%04d.pcd",j);
        FormatString(image->magick_filename,"images/img%04d.pcd",j);
        image->scene=j;
        image->columns=width;
        image->rows=height;
        image->packets=image->columns*image->rows;
        image->pixels=(RunlengthPacket *)
          AllocateMemory(image->packets*sizeof(RunlengthPacket));
        if (image->pixels == (RunlengthPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        SetImage(image);
        y=luma;
        c1=chroma1;
        c2=chroma2;
        for (i=0; i < (int) height; i+=2)
        {
          (void) ReadData((char *) y,1,width,image->file);
          y+=image->columns;
          (void) ReadData((char *) y,1,width,image->file);
          y+=image->columns;
          (void) ReadData((char *) c1,1,width >> 1,image->file);
          c1+=image->columns;
          (void) ReadData((char *) c2,1,width >> 1,image->file);
          c2+=image->columns;
        }
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
        Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
        /*
          Transfer luminance and chrominance channels.
        */
        p=image->pixels;
        y=luma;
        c1=chroma1;
        c2=chroma2;
        for (i=0; i < (int) image->packets; i++)
        {
          p->red=UpScale(*y++);
          p->green=UpScale(*c1++);
          p->blue=UpScale(*c2++);
          p->index=0;
          p->length=0;
          p++;
        }
        if (Latin1Compare(image_info->magick,"PCDS") == 0)
          TransformRGBImage(image,sRGBColorspace);
        else
          TransformRGBImage(image,YCCColorspace);
        if (j < (int) number_images)
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
          }
        (void) SetMonitorHandler(handler);
        ProgressMonitor(LoadImageText,j-1,number_images);
      }
      FreeMemory(chroma2);
      FreeMemory(chroma1);
      FreeMemory(luma);
      CondenseImage(image);
      while (image->previous != (Image *) NULL)
        image=image->previous;
      overview_image=OverviewImage(image_info,image);
      return(overview_image);
    }
  /*
    Allocate image pixels.
  */
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  /*
    Read interleaved image.
  */
  y=luma;
  c1=chroma1;
  c2=chroma2;
  for (i=0; i < (int) height; i+=2)
  {
    (void) ReadData((char *) y,1,width,image->file);
    y+=image->columns;
    (void) ReadData((char *) y,1,width,image->file);
    y+=image->columns;
    (void) ReadData((char *) c1,1,width >> 1,image->file);
    c1+=image->columns;
    (void) ReadData((char *) c2,1,width >> 1,image->file);
    c2+=image->columns;
  }
  if (subimage >= 4)
    {
      /*
        Recover luminance deltas for 1536x1024 image.
      */
      Upsample(768,512,image->columns,luma);
      Upsample(384,256,image->columns,chroma1);
      Upsample(384,256,image->columns,chroma2);
      image->rows=1024;
      for (i=0; i < (4*0x800); i++)
        (void) fgetc(image->file);
      status=PCDDecodeImage(image,luma,chroma1,chroma2);
      if ((subimage >= 5) && status)
        {
          /*
            Recover luminance deltas for 3072x2048 image.
          */
          Upsample(1536,1024,image->columns,luma);
          Upsample(768,512,image->columns,chroma1);
          Upsample(768,512,image->columns,chroma2);
          image->rows=2048;
          offset=ftell(image->file)/0x800+12;
          (void) fseek(image->file,offset*0x800,SEEK_SET);
          status=PCDDecodeImage(image,luma,chroma1,chroma2);
          if ((subimage >= 6) && status)
            {
              /*
                Recover luminance deltas for 6144x4096 image (vaporware).
              */
              Upsample(3072,2048,image->columns,luma);
              Upsample(1536,1024,image->columns,chroma1);
              Upsample(1536,1024,image->columns,chroma2);
              image->rows=4096;
            }
        }
    }
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma1);
  Upsample(image->columns >> 1,image->rows >> 1,image->columns,chroma2);
  /*
    Transfer luminance and chrominance channels.
  */
  p=image->pixels;
  y=luma;
  c1=chroma1;
  c2=chroma2;
  for (i=0; i < (int) image->packets; i++)
  {
    p->red=UpScale(*y++);
    p->green=UpScale(*c1++);
    p->blue=UpScale(*c2++);
    p->index=0;
    p->length=0;
    p++;
    if (QuantumTick(i,image->columns*image->rows))
      ProgressMonitor(LoadImageText,i,image->columns*image->rows);
  }
  FreeMemory(chroma2);
  FreeMemory(chroma1);
  FreeMemory(luma);
  if (Latin1Compare(image_info->magick,"PCDS") == 0)
    TransformRGBImage(image,sRGBColorspace);
  else
    TransformRGBImage(image,YCCColorspace);
  if ((rotate == 1) || (rotate == 3))
    {
      double
        degrees;

      Image
        *rotated_image;

      /*
        Rotate image.
      */
      degrees=rotate == 1 ? -90.0 : 90.0;
      image->orphan=True;
      rotated_image=RotateImage(image,degrees,False,True);
      image->orphan=False;
      if (rotated_image != (Image *) NULL)
        {
          DestroyImage(image);
          image=rotated_image;
        }
    }
  /*
    Set CCIR 709 primaries with a D65 white point.
  */
  image->chromaticity.red_primary.x=0.64f;
  image->chromaticity.red_primary.y=0.33f;
  image->chromaticity.green_primary.x=0.30f;
  image->chromaticity.green_primary.y=0.60f;
  image->chromaticity.blue_primary.x=0.15f;
  image->chromaticity.blue_primary.y=0.06f;
  image->chromaticity.white_point.x=0.3127f;
  image->chromaticity.white_point.y=0.3290f;
  image->gamma=0.5;
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P C L I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCLImage reads a Page Control Language image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadPCLImage routine is:
%
%      image=ReadPCLImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCLImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPCLImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"Cannot read PCL images",
    image_info->filename);
  return((Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P C X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPCXImage reads a ZSoft IBM PC Paintbrush file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadPCXImage routine is:
%
%      image=ReadPCXImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPCXImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPCXImage(const ImageInfo *image_info)
{
  typedef struct _PCXHeader
  {
    unsigned char
      identifier,
      version,
      encoding,
      bits_per_pixel;

    short int
      left,
      top,
      right,
      bottom,
      horizontal_resolution,
      vertical_resolution;

    unsigned char
      reserved,
      planes;

    short int
      bytes_per_line,
      palette_info;

    unsigned char
      colormap_signature;
  } PCXHeader;

  PCXHeader
    pcx_header;

  Image
    *image;

  int
    bits,
    count,
    id,
    mask,
    pcx_packets,
    y;

  Quantum
    blue,
    green,
    red;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  register unsigned char
    *p,
    *r;

  unsigned char
    packet,
    *pcx_colormap,
    *pcx_pixels,
    *scanline;

  unsigned int
    status;

  unsigned long
    max_packets,
    *page_table;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a PCX file.
  */
  page_table=(unsigned long *) NULL;
  if (Latin1Compare(image_info->magick,"DCX") == 0)
    {
      unsigned long
        magic;

      /*
        Read the DCX page table.
      */
      magic=LSBFirstReadLong(image->file);
      if (magic != 987654321)
        PrematureExit(CorruptImageWarning,"Not a DCX image file",image);
      page_table=(unsigned long *) AllocateMemory(1024*sizeof(unsigned long));
      if (page_table == (unsigned long *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (id=0; id < 1024; id++)
      {
        page_table[id]=LSBFirstReadLong(image->file);
        if (page_table[id] == 0)
          break;
      }
    }
  if (page_table != (unsigned long *) NULL)
    (void) fseek(image->file,(long) page_table[0],SEEK_SET);
  status=ReadData((char *) &pcx_header.identifier,1,1,image->file);
  for (id=1; id < 1024; id++)
  {
    /*
      Verify PCX identifier.
    */
    (void) ReadData((char *) &pcx_header.version,1,1,image->file);
    if ((status == False) || (pcx_header.identifier != 0x0a) ||
        ((pcx_header.version != 2) && (pcx_header.version != 3) &&
         (pcx_header.version != 5)))
      PrematureExit(CorruptImageWarning,"Not a PCX image file",image);
    (void) ReadData((char *) &pcx_header.encoding,1,1,image->file);
    (void) ReadData((char *) &pcx_header.bits_per_pixel,1,1,image->file);
    pcx_header.left=LSBFirstReadShort(image->file);
    pcx_header.top=LSBFirstReadShort(image->file);
    pcx_header.right=LSBFirstReadShort(image->file);
    pcx_header.bottom=LSBFirstReadShort(image->file);
    pcx_header.horizontal_resolution=LSBFirstReadShort(image->file);
    pcx_header.vertical_resolution=LSBFirstReadShort(image->file);
    /*
      Read PCX raster colormap.
    */
    image->columns=(pcx_header.right-pcx_header.left)+1;
    image->rows=(pcx_header.bottom-pcx_header.top)+1;
    image->units=PixelsPerInchResolution;
    image->x_resolution=pcx_header.horizontal_resolution;
    image->y_resolution=pcx_header.vertical_resolution;
    image->packets=image->columns*image->rows;
    image->colors=16;
    pcx_colormap=(unsigned char *) AllocateMemory(3*256*sizeof(unsigned char));
    if (pcx_colormap == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) ReadData((char *) pcx_colormap,3,image->colors,image->file);
    (void) ReadData((char *) &pcx_header.reserved,1,1,image->file);
    (void) ReadData((char *) &pcx_header.planes,1,1,image->file);
    if ((pcx_header.bits_per_pixel != 8) || (pcx_header.planes == 1))
      {
        image->class=PseudoClass;
        if ((pcx_header.version == 3) || (pcx_header.version == 5))
          image->colors=1 << (pcx_header.bits_per_pixel*pcx_header.planes);
      }
    if (image_info->ping)
      {
        FreeMemory((char *) pcx_colormap);
        if (page_table != (unsigned long *) NULL)
          FreeMemory((char *) page_table);
        CloseImage(image);
        return(image);
      }
    image->colormap=(ColorPacket *) AllocateMemory(256*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    p=pcx_colormap;
    for (i=0; i < (int) image->colors; i++)
    {
      image->colormap[i].red=UpScale(*p++);
      image->colormap[i].green=UpScale(*p++);
      image->colormap[i].blue=UpScale(*p++);
    }
    if (pcx_header.version == 3)
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
        image->colormap[i].green=(Quantum)
          ((long) (MaxRGB*i)/(image->colors-1));
        image->colormap[i].blue=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
      }
    pcx_header.bytes_per_line=LSBFirstReadShort(image->file);
    pcx_header.palette_info=LSBFirstReadShort(image->file);
    for (i=0; i < 58; i++)
      (void) fgetc(image->file);
    /*
      Read image data.
    */
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    if ((pcx_header.bits_per_pixel*pcx_header.planes) == 1)
      max_packets=Max((image->columns*image->rows+8) >> 4,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    pcx_packets=image->rows*pcx_header.bytes_per_line*pcx_header.planes;
    pcx_pixels=(unsigned char *)
      AllocateMemory(pcx_packets*sizeof(unsigned char));
    scanline=(unsigned char *)
      AllocateMemory(image->columns*pcx_header.planes*sizeof(unsigned char));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (pcx_pixels == (unsigned char *) NULL) ||
        (scanline == (unsigned char *) NULL))
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Uncompress image data.
    */
    p=pcx_pixels;
    while (pcx_packets > 0)
    {
      packet=fgetc(image->file);
      if ((packet & 0xc0) != 0xc0)
        {
          *p++=packet;
          pcx_packets--;
          continue;
        }
      count=packet & 0x3f;
      for (packet=fgetc(image->file); count > 0; count--)
      {
        *p++=packet;
        pcx_packets--;
        if (pcx_packets == 0)
          break;
      }
    }
    if (image->class == DirectClass)
      image->matte=pcx_header.planes > 3;
    else
      if (pcx_header.version == 5)
        {
          /*
            Initialize image colormap.
          */
          if (image->colors > 256)
            PrematureExit(CorruptImageWarning,
              "PCX colormap exceeded 256 colors",image);
          if ((pcx_header.bits_per_pixel*pcx_header.planes) == 1)
            {
              /*
                Monochrome colormap.
              */
              image->colormap[0].red=0;
              image->colormap[0].green=0;
              image->colormap[0].blue=0;
              image->colormap[1].red=MaxRGB;
              image->colormap[1].green=MaxRGB;
              image->colormap[1].blue=MaxRGB;
            }
          else
            if (image->colors > 16)
              {
                /*
                  256 color images have their color map at the end of the file.
                */
                (void) ReadData((char *) &pcx_header.colormap_signature,1,1,
                  image->file);
                (void) ReadData((char *) pcx_colormap,3,image->colors,
                  image->file);
                p=pcx_colormap;
                for (i=0; i < (int) image->colors; i++)
                {
                  image->colormap[i].red=UpScale(*p++);
                  image->colormap[i].green=UpScale(*p++);
                  image->colormap[i].blue=UpScale(*p++);
                }
            }
          FreeMemory((char *) pcx_colormap);
        }
    /*
      Convert PCX raster image to runlength-encoded packets.
    */
    red=0;
    green=0;
    blue=0;
    index=0;
    q=image->pixels;
    SetRunlengthEncoder(q);
    for (y=0; y < (int) image->rows; y++)
    {
      p=pcx_pixels+(y*pcx_header.bytes_per_line*pcx_header.planes);
      r=scanline;
      if (image->class == DirectClass)
        for (i=0; i < (int) pcx_header.planes; i++)
        {
          r=scanline+i;
          for (x=0; x < pcx_header.bytes_per_line; x++)
          {
            switch (i)
            {
              case 0:
              {
                *r=UpScale(*p++);
                break;
              }
              case 1:
              {
                *r=UpScale(*p++);
                break;
              }
              case 2:
              {
                *r=UpScale(*p++);
                break;
              }
              case 3:
              default:
              {
                *r=UpScale(*p++);
                break;
              }
            }
            r+=pcx_header.planes;
          }
        }
      else
        if (pcx_header.planes > 1)
          {
            for (x=0; x < (int) image->columns; x++)
              *r++=0;
            for (i=0; i < (int) pcx_header.planes; i++)
            {
              r=scanline;
              for (x=0; x < pcx_header.bytes_per_line; x++)
              {
                 bits=(*p++);
                 for (mask=0x80; mask != 0; mask>>=1)
                 {
                   if (bits & mask)
                     *r|=1 << i;
                   r++;
                 }
               }
            }
          }
        else
          switch (pcx_header.bits_per_pixel)
          {
            case 1:
            {
              register int
                bit;

              for (x=0; x < ((int) image->columns-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=7; bit >= (int) (8-(image->columns % 8)); bit--)
                    *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((int) image->columns-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((image->columns % 4) != 0)
                {
                  for (i=3; i >= (int) (4-(image->columns % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((int) image->columns-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((image->columns % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              for (x=0; x < (int) image->columns; x++)
                *r++=(*p++);
              break;
            }
            default:
              break;
          }
      /*
        Transfer image scanline.
      */
      r=scanline;
      for (x=0; x < (int) image->columns; x++)
      {
        if (image->class == PseudoClass)
          index=(*r++);
        else
          {
            red=UpScale(*r++);
            green=UpScale(*r++);
            blue=UpScale(*r++);
            if (image->matte)
              index=UpScale(*r++);
          }
        if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
            (index == q->index) && ((int) q->length < MaxRunlength))
          q->length++;
        else
          {
            if (packets != 0)
              q++;
            packets++;
            if (packets == (int) max_packets)
              {
                max_packets<<=1;
                image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                  image->pixels,max_packets*sizeof(RunlengthPacket));
                if (image->pixels == (RunlengthPacket *) NULL)
                  {
                    FreeMemory((char *) scanline);
                    PrematureExit(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  }
                q=image->pixels+packets-1;
              }
            q->red=red;
            q->green=green;
            q->blue=blue;
            q->index=index;
            q->length=0;
          }
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    SetRunlengthPackets(image,packets);
    if (image->class == PseudoClass)
      SyncImage(image);
    FreeMemory((char *) scanline);
    FreeMemory((char *) pcx_pixels);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (page_table == (unsigned long *) NULL)
      break;
    if (page_table[id] == 0)
      break;
    (void) fseek(image->file,(long) page_table[id],SEEK_SET);
    status=ReadData((char *) &pcx_header.identifier,1,1,image->file);
    if ((status == True) && (pcx_header.identifier == 0x0a))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  }
  if (page_table != (unsigned long *) NULL)
    FreeMemory((char *) page_table);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDFImage reads a Portable Document Format image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPDFImage routine is:
%
%      image=ReadPDFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPDFImage(const ImageInfo *image_info)
{
#define MediaBox  "/MediaBox ["

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  int
    count,
    status;

  long int
    filesize;

  PointInfo
    lower,
    upper;

  RectangleInfo
    bounding_box,
    box;

  register char
    *p;

  register int
    c;

  unsigned int
    alias_bits,
    height,
    portrait,
    width;

  if (image_info->monochrome)
    {
      if (!GetDelegateInfo("gs-mono",(char *) NULL,&delegate_info))
        return((Image *) NULL);
    }
  else
    if (!GetDelegateInfo("gs-color",(char *) NULL,&delegate_info))
      return((Image *) NULL);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to write file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
     (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  bounding_box.width=612;
  bounding_box.height=792;
  bounding_box.x=0;
  bounding_box.y=0;
  (void) ParseImageGeometry(PSPageGeometry,&bounding_box.x,&bounding_box.y,
    &bounding_box.width,&bounding_box.height);
  portrait=True;
  /*
    Determine page geometry from the PDF media box.
  */
  box.width=0;
  box.height=0;
  for (p=command; ; )
  {
    c=fgetc(image->file);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=(char) c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    /*
      Continue unless this is a MediaBox statement.
    */
    if (strncmp(command,"/Rotate 90",10) == 0)
      portrait=False;
    if (strncmp(MediaBox,command,Extent(MediaBox)) != 0)
      continue;
    count=sscanf(command,"/MediaBox [ %f %f %f %f",&lower.x,&lower.y,
      &upper.x,&upper.y);
    if (count != 4)
      continue;
    if ((lower.x > upper.x) || (lower.y > upper.y))
      continue;
    /*
      Set Postscript render geometry.
    */
    width=(unsigned int) (upper.x-lower.x+1);
    if ((float) ((int) upper.x) != upper.x)
      width++;
    height=(unsigned int) (upper.y-lower.y+1);
    if ((float) ((int) upper.y) != upper.y)
      height++;
    if ((width <= box.width) && (height <= box.height))
      continue;
    bounding_box.width=width;
    bounding_box.height=height;
    box=bounding_box;
  }
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&bounding_box.x,&bounding_box.y,
      &bounding_box.width,&bounding_box.height);
  FormatString(geometry,"%ux%u",(unsigned int) (((bounding_box.width*
    image->x_resolution)/dx_resolution)+0.5),(unsigned int)
    (((bounding_box.height*image->y_resolution)/dy_resolution)+0.5));
  if (ferror(file))
    {
      MagickWarning(FileOpenWarning,"An error has occurred writing to file",
        postscript_filename);
      (void) fclose(file);
      return((Image *) NULL);
    }
  (void) fclose(file);
  CloseImage(image);
  filesize=image->filesize;
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strcpy(filename,image_info->filename);
  TemporaryFilename((char *) image_info->filename);
  alias_bits=image_info->dither && !image_info->monochrome ? 4 : 1;
  FormatString(command,delegate_info.commands,alias_bits,alias_bits,geometry,
    density,options,image_info->filename,postscript_filename);
  ProgressMonitor(RenderPostscriptText,0,8);
  status=SystemCommand(image_info->verbose,command);
  ProgressMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      MagickWarning(CorruptImageWarning,"Portable Document delegation failed",
        image_info->filename);
      (void) remove(postscript_filename);
      return((Image *) NULL);
    }
  image=ReadPNMImage(image_info);
  (void) remove(postscript_filename);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    {
      MagickWarning(CorruptImageWarning,"Portable Document delegation failed",
        image_info->filename);
      return((Image *) NULL);
    }
  (void) strcpy((char *) image_info->filename,filename);
  do
  {
    (void) strcpy(image->magick,"PDF");
    (void) strcpy(image->filename,image_info->filename);
    image->filesize=filesize;
    if (!portrait)
      {
        Image
          *rotated_image;

        /*
          Rotate image.
        */
        image->orphan=True;
        rotated_image=RotateImage(image,90,False,True);
        image->orphan=False;
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(image);
            image=rotated_image;
          }
      }
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

#if !defined(macintosh)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P I C T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPICTImage reads an Apple Macintosh QuickDraw/PICT image file
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadPICTImage routine is:
%
%      image=ReadPICTImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPICTImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadPICTImage(const ImageInfo *image_info)
{
#include "pict.h"

  char
    geometry[MaxTextExtent];

  Image
    *image,
    *tiled_image;

  int
    c,
    code,
    flags,
    version;

  long
    length;

  PICTRectangle
    frame;

  PICTPixmap
    pixmap;

  register int
    i;

  register RunlengthPacket
    *q;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read PICT header.
  */
  for (i=0; i < 512; i++)
    (void) fgetc(image->file);  /* skip header */
  (void) MSBFirstReadShort(image->file);  /* skip picture size */
  ReadRectangle(frame);
  while ((c=fgetc(image->file)) == 0);
  if (c != 0x11)
    PrematureExit(CorruptImageWarning,"Not a PICT image file",image);
  version=fgetc(image->file);
  if (version == 2)
    {
      c=fgetc(image->file);
      if (c != 0xff)
        PrematureExit(CorruptImageWarning,"Not a PICT image file",image);
    }
  else
    if (version != 1)
      PrematureExit(CorruptImageWarning,"Not a PICT image file",image);
  /*
    Create black canvas.
  */
  image->columns=frame.right-frame.left;
  image->rows=frame.bottom-frame.top;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  image->packets=((Max(image->columns*image->rows,1)-1)/(MaxRunlength+1))+1;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  q=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    q->red=0;
    q->green=0;
    q->blue=0;
    q->index=0;
    SetRunlengthEncoder(q);
    q++;
  }
  q--;
  q->length=image->columns*image->rows-(MaxRunlength+1)*(image->packets-1)-1;
  /*
    Interpret PICT opcodes.
  */
  for ( ; ; )
  {
    if ((version == 1) || ((ftell(image->file) % 2) != 0))
      code=fgetc(image->file);
    if (version == 2)
      code=MSBFirstReadShort(image->file);
    if ((code == 0xff) || (code == 0xffff))
      break;
    if (code < 0xa2)
      switch (code)
      {
        case 0x01:
        {
          /*
            Clipping rectangle.
          */
          length=MSBFirstReadShort(image->file);
          if (length != 0x000a)
            {
              for (i=0; i < (length-2); i++)
                (void) fgetc(image->file);
              break;
            }
          ReadRectangle(frame);
          if ((frame.left & 0x8000) || (frame.top & 0x8000))
            break;
          image->columns=frame.right-frame.left;
          image->rows=frame.bottom-frame.top;
          image->packets=
            ((Max(image->columns*image->rows,1)-1)/(MaxRunlength+1))+1;
          image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
            image->pixels,image->packets*sizeof(RunlengthPacket));
          if (image->pixels == (RunlengthPacket *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          q=image->pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            q->red=0;
            q->green=0;
            q->blue=0;
            q->index=0;
            SetRunlengthEncoder(q);
            q++;
          }
          q--;
          q->length=image->columns*image->rows-(MaxRunlength+1)*
            (image->packets-1)-1;
          break;
        }
        case 0x12:
        case 0x13:
        case 0x14:
        {
          int
            pattern;

          unsigned int
            height,
            width;

          /*
            Skip pattern definition.
          */
          pattern=MSBFirstReadShort(image->file);
          for (i=0; i < 8; i++)
            (void) fgetc(image->file);
          if (pattern == 2)
            {
              for (i=0; i < 5; i++)
                (void) fgetc(image->file);
              break;
            }
          if (pattern != 1)
            PrematureExit(CorruptImageWarning,"Unknown pattern type",image);
          length=MSBFirstReadShort(image->file);
          ReadRectangle(frame);
          ReadPixmap(pixmap);
          (void) MSBFirstReadLong(image->file);
          flags=MSBFirstReadShort(image->file);
          length=MSBFirstReadShort(image->file);
          for (i=0; i <= length; i++)
            (void) MSBFirstReadLong(image->file);
          width=frame.bottom-frame.top;
          height=frame.right-frame.left;
          if (pixmap.bits_per_pixel <= 8)
            length&=0x7fff;
          if (pixmap.bits_per_pixel == 16)
            width<<=1;
          if (length == 0)
            length=width;
          if (length < 8)
            {
              for (i=0; i < (int) (length*height); i++)
                (void) fgetc(image->file);
            }
          else
            for (i=0; i < (int) height; i++)
              if (length > 250)
                for (i=0; i < (int) MSBFirstReadShort(image->file); i++)
                  (void) fgetc(image->file);
              else
                for (i=0; i < fgetc(image->file); i++)
                  (void) fgetc(image->file);
          break;
        }
        case 0x1b:
        {
          /*
            Initialize image background color.
          */
          image->background_color.red=
            XDownScale(MSBFirstReadShort(image->file));
          image->background_color.green=
            XDownScale(MSBFirstReadShort(image->file));
          image->background_color.blue=
            XDownScale(MSBFirstReadShort(image->file));
          break;
        }
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        {
          /*
            Skip polygon or region.
          */
          length=MSBFirstReadShort(image->file);
          for (i=0; i < (length-2); i++)
            (void) fgetc(image->file);
          break;
        }
        case 0x90:
        case 0x91:
        case 0x98:
        case 0x99:
        case 0x9a:
        {
          int
            bytes_per_line,
            j,
            y;

          PICTRectangle
            destination;

          Quantum
            blue,
            green,
            red;

          register int
            x;

          register long
            packets;

          register unsigned char
            *p;

          unsigned char
            *pixels;

          unsigned long
            max_packets;

          unsigned short
            index;

          /*
            Pixmap clipped by a rectangle.
          */
          bytes_per_line=0;
          if (code != 0x9a)
            bytes_per_line=MSBFirstReadShort(image->file);
          else
            {
              (void) MSBFirstReadShort(image->file);
              (void) MSBFirstReadShort(image->file);
              (void) MSBFirstReadShort(image->file);
            }
          ReadRectangle(frame);
          /*
            Initialize tiled_image.
          */
          tiled_image=AllocateImage(image_info);
          if (tiled_image == (Image *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          tiled_image->file=image->file;
          tiled_image->columns=frame.right-frame.left;
          tiled_image->rows=frame.bottom-frame.top;
          if ((code == 0x9a) || (bytes_per_line & 0x8000))
            {
              ReadPixmap(pixmap);
              tiled_image->matte=pixmap.component_count == 4;
            }
          if (code != 0x9a)
            {
              /*
                Initialize colormap.
              */
              tiled_image->colors=2;
              if (bytes_per_line & 0x8000)
                {
                  (void) MSBFirstReadLong(image->file);
                  flags=MSBFirstReadShort(image->file);
                  tiled_image->colors=MSBFirstReadShort(image->file)+1;
                }
              tiled_image->class=PseudoClass;
              tiled_image->colormap=(ColorPacket *)
                AllocateMemory(tiled_image->colors*sizeof(ColorPacket));
              if (tiled_image->colormap == (ColorPacket *) NULL)
                {
                  DestroyImage(tiled_image);
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              for (i=0; i < (int) tiled_image->colors; i++)
              {
                tiled_image->colormap[i].red=(Quantum)
                  ((long) (MaxRGB*i)/(tiled_image->colors-1));
                tiled_image->colormap[i].green=(Quantum)
                  ((long) (MaxRGB*i)/(tiled_image->colors-1));
                tiled_image->colormap[i].blue=(Quantum)
                  ((long) (MaxRGB*i)/(tiled_image->colors-1));
              }
              if (bytes_per_line & 0x8000)
                for (i=0; i < (int) tiled_image->colors; i++)
                {
                  j=MSBFirstReadShort(image->file) % tiled_image->colors;
                  if (flags & 0x8000)
                    j=i;
                  tiled_image->colormap[j].red=
                    XDownScale(MSBFirstReadShort(image->file));
                  tiled_image->colormap[j].green=
                    XDownScale(MSBFirstReadShort(image->file));
                  tiled_image->colormap[j].blue=
                    XDownScale(MSBFirstReadShort(image->file));
                }
            }
          ReadRectangle(destination);
          ReadRectangle(destination);
          (void) MSBFirstReadShort(image->file);
          if ((code == 0x91) || (code == 0x99))
            {
              /*
                Skip region.
              */
              length=MSBFirstReadShort(image->file);
              for (i=0; i < (length-2); i++)
                (void) fgetc(image->file);
            }
          packets=0;
          max_packets=Max((tiled_image->columns*tiled_image->rows+4) >> 3,1);
          tiled_image->pixels=(RunlengthPacket *)
            AllocateMemory(max_packets*sizeof(RunlengthPacket));
          if ((code != 0x9a) && (bytes_per_line & 0x8000) == 0)
            pixels=PICTDecodeImage(tiled_image,bytes_per_line,1);
          else
            pixels=PICTDecodeImage(tiled_image,bytes_per_line,
              pixmap.bits_per_pixel);
          if ((tiled_image->pixels == (RunlengthPacket *) NULL) ||
              (pixels == (unsigned char *) NULL))
            {
              DestroyImage(tiled_image);
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            }
          /*
            Convert PICT tiled_image to runlength-encoded packets.
          */
          red=0;
          green=0;
          blue=0;
          index=0;
          p=pixels;
          q=tiled_image->pixels;
          SetRunlengthEncoder(q);
          for (y=0; y < (int) tiled_image->rows; y++)
          {
            for (x=0; x < (int) tiled_image->columns; x++)
            {
              if (tiled_image->class == PseudoClass)
                index=(*p++);
              else
                {
                  red=UpScale(*p);
                  green=UpScale(*(p+tiled_image->columns));
                  blue=UpScale(*(p+2*tiled_image->columns));
                  if (tiled_image->matte)
                    index=UpScale(*(p+3*tiled_image->columns));
                  p++;
                }
              if ((red == q->red) && (green == q->green) &&
                  (blue == q->blue) && (index == q->index) &&
                  ((int) q->length < MaxRunlength))
                q->length++;
              else
                {
                  if (packets != 0)
                    q++;
                  packets++;
                  if (packets == (int) max_packets)
                    {
                      max_packets<<=1;
                      tiled_image->pixels=(RunlengthPacket *) ReallocateMemory(
                        (char *) tiled_image->pixels,max_packets*
                        sizeof(RunlengthPacket));
                      if (tiled_image->pixels == (RunlengthPacket *) NULL)
                        {
                          (void) FreeMemory((char *) pixels);
                          DestroyImage(tiled_image);
                          PrematureExit(ResourceLimitWarning,
                            "Memory allocation failed",image);
                        }
                      q=tiled_image->pixels+packets-1;
                    }
                  q->red=red;
                  q->green=green;
                  q->blue=blue;
                  q->index=index;
                  q->length=0;
                }
            }
            if (tiled_image->class == DirectClass)
              p+=(pixmap.component_count-1)*tiled_image->columns;
            if (destination.bottom == (int) image->rows)
              if (QuantumTick(y,tiled_image->rows))
                ProgressMonitor(LoadImageText,y,tiled_image->rows);
          }
          SetRunlengthPackets(tiled_image,packets);
          if (tiled_image->class == PseudoClass)
            SyncImage(tiled_image);
          (void) FreeMemory((char *) pixels);
          CompositeImage(image,ReplaceCompositeOp,tiled_image,destination.left,
            destination.top);
          tiled_image->file=(FILE *) NULL;
          DestroyImage(tiled_image);
          if (destination.bottom != (int) image->rows)
            ProgressMonitor(LoadImageText,destination.bottom,image->rows);
          break;
        }
        case 0xa1:
        {
          /*
            Comment.
          */
          (void) MSBFirstReadShort(image->file);
          length=MSBFirstReadShort(image->file);
          if (length == 0)
            break;
          if (image->comments != (char *) NULL)
            FreeMemory((char *) image->comments);
          image->comments=(char *) AllocateMemory((length+1)*sizeof(char));
          if (image->comments == (char *) NULL)
            break;
          for (i=0; i < length; i++)
            image->comments[i]=fgetc(image->file);
          image->comments[i]='\0';
          break;
        }
        default:
        {
          /*
            Skip to next op code.
          */
          if (codes[code].length == -1)
            (void) MSBFirstReadShort(image->file);
          else
            for (i=0; i < codes[code].length; i++)
              (void) fgetc(image->file);
        }
      }
    if (code == 0xc00)
      {
        /*
          Skip header.
        */
        for (i=0; i < 24; i++)
          (void) fgetc(image->file);
        continue;
      }
    if (((code >= 0xb0) && (code <= 0xcf)) ||
        ((code >= 0x8000) && (code <= 0x80ff)))
      continue;
    if (code == 0x8200)
      {
        FILE
          *file;

        ImageInfo
          *local_info;

        /*
          Embedded JPEG.
        */
        local_info=CloneImageInfo(image_info);
        if (local_info == (ImageInfo *) NULL)
          PrematureExit(FileOpenWarning,"Unable to write file",image);
        TemporaryFilename(local_info->filename);
        file=fopen(local_info->filename,WriteBinaryType);
        if (file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to write file",image);
        length=MSBFirstReadLong(image->file);
        for (i=0; i < length; i++)
        {
          c=fgetc(image->file);
          if (i >= 154)
            (void) putc(c,file);
        }
        (void) fclose(file);
        tiled_image=ReadJPEGImage(local_info);
        DestroyImageInfo(local_info);
        (void) remove(local_info->filename);
        if (tiled_image == (Image *) NULL)
          continue;
        FormatString(geometry,"%ux%u",
          Max(image->columns,tiled_image->columns),
          Max(image->rows,tiled_image->rows));
        TransformImage(&image,(char *) NULL,geometry);
        CompositeImage(image,ReplaceCompositeOp,tiled_image,0,0);
        tiled_image->file=(FILE *) NULL;
        DestroyImage(tiled_image);
        continue;
      }
    if (((code >= 0xd0) && (code <= 0xfe)) ||
        ((code >= 0x8100) && (code <= 0xffff)))
      {
        /*
          Skip reserved.
        */
        length=MSBFirstReadShort(image->file);
        for (i=0; i < length; i++)
          (void) fgetc(image->file);
        continue;
      }
    if ((code >= 0x100) && (code <= 0x7fff))
      {
        /*
          Skip reserved.
        */
        length=(code >> 7) & 0xff;
        for (i=0; i < length; i++)
          (void) fgetc(image->file);
        continue;
      }
  }
  CondenseImage(image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P I X I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPIXImage reads a Alias/Wavefront RLE image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPIXImage routine is:
%
%      image=ReadPIXImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPIXImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPIXImage(const ImageInfo *image_info)
{
  Image
    *image;

  register long
    packets;

  register RunlengthPacket
    *q;

  unsigned int
    bits_per_pixel,
    status;

  unsigned long
    height,
    max_packets,
    number_pixels,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read PIX image.
  */
  width=MSBFirstReadShort(image->file);
  height=MSBFirstReadShort(image->file);
  (void) MSBFirstReadShort(image->file);  /* x-offset */
  (void) MSBFirstReadShort(image->file);  /* y-offset */
  bits_per_pixel=MSBFirstReadShort(image->file);
  if ((width == (unsigned long) ~0) || (height == (unsigned long) ~0) ||
      ((bits_per_pixel != 8) && (bits_per_pixel != 24)))
    PrematureExit(CorruptImageWarning,"Not a PIX image file",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if (bits_per_pixel == 8)
      {
        register int
          i;

        /*
          Create linear colormap.
        */
        image->class=PseudoClass;
        image->colors=MaxRGB+1;
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(Quantum) UpScale(i);
          image->colormap[i].green=(Quantum) UpScale(i);
          image->colormap[i].blue=(Quantum) UpScale(i);
        }
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert PIX raster image to runlength-encoded packets.
    */
    number_pixels=0;
    q=image->pixels;
    do
    {
      q->length=fgetc(image->file);
      if (q->length == 0)
        continue;
      if (bits_per_pixel == 8)
        q->index=UpScale(fgetc(image->file));
      else
        {
          q->blue=UpScale(fgetc(image->file));
          q->green=UpScale(fgetc(image->file));
          q->red=UpScale(fgetc(image->file));
          q->index=0;
        }
      number_pixels+=q->length;
      q++;
      packets++;
      if (packets == (int) max_packets)
        {
          max_packets<<=1;
          image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
            image->pixels,max_packets*sizeof(RunlengthPacket));
          if (image->pixels == (RunlengthPacket *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          q=image->pixels+packets;
        }
      if (image->previous == (Image *) NULL)
        ProgressMonitor(LoadImageText,number_pixels,image->columns*image->rows);
    } while (number_pixels < (image->columns*image->rows));
    SetRunlengthPackets(image,packets);
    if (image->class == PseudoClass)
      SyncImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    width=MSBFirstReadLong(image->file);
    height=MSBFirstReadLong(image->file);
    (void) MSBFirstReadShort(image->file);
    (void) MSBFirstReadShort(image->file);
    bits_per_pixel=MSBFirstReadShort(image->file);
    status=(width != (unsigned long) ~0) && (height == (unsigned long) ~0) &&
      ((bits_per_pixel == 8) || (bits_per_pixel == 24));
    if (status == True)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (status == True);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P L A S M A I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPLASMAImage creates a plasma fractal image.  The image is
%  initialized to to the X server color as specified by the filename.
%
%  The format of the ReadPLASMAImage routine is:
%
%      image=ReadPLASMAImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPLASMAImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPLASMAImage(const ImageInfo *image_info)
{
#define PlasmaImageText  "  Applying image plasma...  "
#define PlasmaPixel(x,y) \
{ \
  p=PixelOffset(x,y); \
  p->red=(Quantum) (rand() % (MaxRGB+1)); \
  p->green=(Quantum) (rand() % (MaxRGB+1)); \
  p->blue=(Quantum) (rand() % (MaxRGB+1)); \
}

  Image
    *image;

  register int
    i;

  SegmentInfo
    segment_info;

  unsigned int
    depth,
    max_depth;

  /*
    Recursively apply plasma to the image.
  */
  image=ReadGRADATIONImage(image_info);
  if (image == (Image *) NULL)
    return(image);
  if (!UncondenseImage(image))
    return(image);
  image->class=DirectClass;
  for (i=0; i < (int) image->packets; i++)
    image->pixels[i].index=(Opaque-Transparent) >> 1;
  segment_info.x1=0;
  segment_info.y1=0;
  segment_info.x2=image->columns-1;
  segment_info.y2=image->rows-1;
  srand(time(0));
  if (Latin1Compare(image_info->filename,"fractal") == 0)
    {
      register RunlengthPacket
        *p;

      /*
        Seed pixels before recursion.
      */
      PlasmaPixel(segment_info.x1,segment_info.y1);
      PlasmaPixel(segment_info.x1,(segment_info.y1+segment_info.y2)/2);
      PlasmaPixel(segment_info.x1,segment_info.y2);
      PlasmaPixel((segment_info.x1+segment_info.x2)/2,segment_info.y1);
      PlasmaPixel((segment_info.x1+segment_info.x2)/2,
        (segment_info.y1+segment_info.y2)/2);
      PlasmaPixel((segment_info.x1+segment_info.x2)/2,segment_info.y2);
      PlasmaPixel(segment_info.x2,segment_info.y1);
      PlasmaPixel(segment_info.x2,(segment_info.y1+segment_info.y2)/2);
      PlasmaPixel(segment_info.x2,segment_info.y2);
    }
  i=Max(image->columns,image->rows) >> 1;
  for (max_depth=0; i != 0; max_depth++)
    i>>=1;
  for (depth=1; ; depth++)
  {
    ProgressMonitor(PlasmaImageText,depth,max_depth);
    if (PlasmaImage(image,&segment_info,0,depth))
      break;
  }
  return(image);
}

#if defined(HasPNG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNGImage reads a Portable Network Graphics image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadPNGImage routine is:
%
%      image=ReadPNGImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static void PNGError(png_struct *ping,png_const_charp message)
{
  MagickWarning(DelegateWarning,message,(char *) NULL);
  longjmp(ping->jmpbuf,1);
}

static void PNGTextChunk(png_info *ping_info,unsigned int i,char **value)
{
  unsigned int
    length;

  length=ping_info->text[i].text_length;
  if (*value != (char *) NULL)
    *value=(char *) ReallocateMemory((char *) *value,strlen(*value)+length+1);
  else
    {
      *value=(char *) AllocateMemory(length+1);
      if (*value != (char *) NULL)
        **value='\0';
    }
  if (*value == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return;
    }
  (void) strncat(*value,ping_info->text[i].text,length);
  (*value)[length]='\0';
}

static void PNGWarning(png_struct *ping,png_const_charp message)
{
  MagickWarning(DelegateWarning,message,(char *) NULL);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

Image *ReadPNGImage(const ImageInfo *image_info)
{
  ColorPacket
    transparent_color;

  Image
    *image;

  int
    y;

  register int
    i,
    x;

  register long
    packets;

  register unsigned char
    *p;

  register RunlengthPacket
    *q;

  png_info
    *end_info,
    *ping_info;

  png_struct
    *ping;

  unsigned char
    *png_pixels,
    **scanlines;

  unsigned int
    status;

  unsigned long
    length,
    max_packets;

  unsigned short
    index,
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if (Latin1Compare(image_info->magick,"MNG") == 0)
    {
      char
        magic_number[MaxTextExtent];

      /*
        Verify MNG signature.
      */
      (void) ReadData(magic_number,1,8,image->file);
      if (strncmp(magic_number,"\212MNG\r\n\032\n",8) != 0)
        PrematureExit(CorruptImageWarning,"Not a MNG image file",image);
    }
  do
  {
    if (Latin1Compare(image_info->magick,"MNG") == 0)
      {
        char
          type[MaxTextExtent];

        /*
          Read a new chunk.
        */
        length=MSBFirstReadLong(image->file);
        status=ReadData(type,1,4,image->file);
        if (status == False)
          {
            MagickWarning(CorruptImageWarning,"Corrupt MNG image",
              image->filename);
            break;
          }
        if (strncmp(type,"MEND",4) == 0)
          break;
        for (i=0; i < (int) length; i++)
          (void) fgetc(image->file);
        (void) MSBFirstReadLong(image->file);  /* read crc word */
        if (strncmp(type,"IHDR",4) != 0)
          continue;
        (void) fseek(image->file,-((int) length+12),SEEK_CUR);
        if (image->pixels != (RunlengthPacket *) NULL)
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
            ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
              (unsigned int) image->filesize);
          }
      }
    /*
      Allocate the PNG structures
    */
    ping=png_create_read_struct(PNG_LIBPNG_VER_STRING,(void *) NULL,PNGError,
      PNGWarning);
    if (ping == (png_struct *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,(png_info **) NULL,(png_info **) NULL);
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    end_info=png_create_info_struct(ping);
    if (end_info == (png_info *) NULL)
      {
        png_destroy_read_struct(&ping,&ping_info,(png_info **) NULL);
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    image->pixels=(RunlengthPacket *) NULL;
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
      {
        /*
          PNG image is corrupt.
        */
        png_destroy_read_struct(&ping,&ping_info,&end_info);
        if (scanlines != (unsigned char **) NULL)
          FreeMemory((char *) scanlines);
        if (png_pixels != (unsigned char *) NULL)
          FreeMemory((char *) png_pixels);
        CloseImage(image);
        if ((image->columns == 0) || (image->rows == 0))
          {
            DestroyImage(image);
            return((Image *) NULL);
          }
        return(image);
      }
    /*
      Prepare PNG for reading.
    */
    png_init_io(ping,image->file);
    if (Latin1Compare(image_info->magick,"MNG") == 0)
      png_set_sig_bytes(ping,8);
    png_read_info(ping,ping_info);
    image->depth=ping_info->bit_depth;
    if (ping_info->bit_depth < 8)
      {
        if ((ping_info->color_type != PNG_COLOR_TYPE_PALETTE) &&
            (ping_info->color_type != PNG_COLOR_TYPE_GRAY))
          png_set_packing(ping);
        image->depth=8;
      }
#if defined(PNG_READ_sRGB_SUPPORTED)
    if (ping_info->valid & PNG_INFO_sRGB)
      image->rendering_intent=(RenderingIntent) (ping_info->srgb_intent+1);
#endif
    if (ping_info->valid & PNG_INFO_gAMA)
      image->gamma=ping_info->gamma;
    if (ping_info->valid & PNG_INFO_cHRM)
      {
        image->chromaticity.red_primary.x=ping_info->x_red;
        image->chromaticity.red_primary.y=ping_info->y_red;
        image->chromaticity.green_primary.x=ping_info->x_green;
        image->chromaticity.green_primary.y=ping_info->y_green;
        image->chromaticity.blue_primary.x=ping_info->x_blue;
        image->chromaticity.blue_primary.y=ping_info->y_blue;
        image->chromaticity.white_point.x=ping_info->x_white;
        image->chromaticity.white_point.y=ping_info->y_white;
      }
    if (ping_info->valid & PNG_INFO_pHYs)
      {
        /*
          Set image resolution.
        */
        image->x_resolution=ping_info->x_pixels_per_unit;
        image->y_resolution=ping_info->y_pixels_per_unit;
        if (ping_info->phys_unit_type == PNG_RESOLUTION_METER)
          {
            image->units=PixelsPerCentimeterResolution;
            image->x_resolution=ping_info->x_pixels_per_unit/100.0;
            image->y_resolution=ping_info->y_pixels_per_unit/100.0;
          }
      }
    if (ping_info->valid & PNG_INFO_bKGD)
      {
        /*
          Set image background color.
        */
        image->background_color.red=ping_info->background.red;
        image->background_color.green=ping_info->background.green;
        image->background_color.blue=ping_info->background.blue;
        if (ping_info->bit_depth > QuantumDepth)
          {
            image->background_color.red=XDownScale(ping_info->background.red);
            image->background_color.green=
              XDownScale(ping_info->background.green);
            image->background_color.blue=XDownScale(ping_info->background.blue);
          }
      }
    if (ping_info->valid & PNG_INFO_tRNS)
      {
        /*
          Image has a transparent background.
        */
        transparent_color.red=ping_info->trans_values.red;
        transparent_color.green=ping_info->trans_values.green;
        transparent_color.blue=ping_info->trans_values.blue;
        transparent_color.index=ping_info->trans_values.gray;
        if (ping_info->bit_depth > QuantumDepth)
          {
            transparent_color.red=XDownScale(ping_info->trans_values.red);
            transparent_color.green=XDownScale(ping_info->trans_values.green);
            transparent_color.blue=XDownScale(ping_info->trans_values.blue);
            transparent_color.index=XDownScale(ping_info->trans_values.gray);
          }
      }
    png_read_update_info(ping,ping_info);
    /*
      Initialize image structure.
    */
    image->compression=ZipCompression;
    image->columns=(unsigned int) ping_info->width;
    image->rows=(unsigned int) ping_info->height;
    if ((ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
        (ping_info->color_type == PNG_COLOR_TYPE_GRAY))
      {
        image->class=PseudoClass;
        image->colors=1 << Min(ping_info->bit_depth,QuantumDepth);
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          image->colors=ping_info->num_palette;
      }
    if (image_info->ping)
      {
        png_destroy_read_struct(&ping,&ping_info,&end_info);
        CloseImage(image);
        return(image);
      }
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    if (ping_info->bit_depth == 1)
      max_packets=Max((image->columns*image->rows+8) >> 4,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    png_pixels=(unsigned char *)
      AllocateMemory(ping_info->rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AllocateMemory(image->rows*sizeof(unsigned char *));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (image->class == PseudoClass)
      {
        /*
          Initialize image colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(MaxRGB*i)/Max(image->colors-1,1);
          image->colormap[i].green=(MaxRGB*i)/Max(image->colors-1,1);
          image->colormap[i].blue=(MaxRGB*i)/Max(image->colors-1,1);
        }
        if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
          for (i=0; i < (int) image->colors; i++)
          {
            image->colormap[i].red=UpScale(ping_info->palette[i].red);
            image->colormap[i].green=UpScale(ping_info->palette[i].green);
            image->colormap[i].blue=UpScale(ping_info->palette[i].blue);
          }
      }
    /*
      Read image scanlines.
    */
    for (i=0; i < (int) image->rows; i++)
      scanlines[i]=png_pixels+(i*ping_info->rowbytes);
    png_read_image(ping,scanlines);
    png_read_end(ping,ping_info);
    /*
      Convert PNG pixels to runlength-encoded packets.
    */
    q=image->pixels;
    SetRunlengthEncoder(q);
    if (image->class == DirectClass)
      {
        Quantum
          blue,
          green,
          red;

        /*
          Convert image to DirectClass runlength-encoded packets.
        */
        if ((ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
            (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA) ||
            (ping_info->valid & PNG_INFO_tRNS))
          image->matte=True;
        for (y=0; y < (int) image->rows; y++)
        {
          p=scanlines[y];
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(red,p);
            green=red;
            blue=red;
            if (ping_info->color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
              {
                ReadQuantum(green,p);
                ReadQuantum(blue,p);
              }
            index=Opaque;
            if ((ping_info->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ||
                (ping_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
              ReadQuantum(index,p);
            if (ping_info->valid & PNG_INFO_tRNS)
              if ((red == transparent_color.red) &&
                  (green == transparent_color.green) &&
                  (blue == transparent_color.blue))
                index=Transparent;
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                (index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        FreeMemory((char *) png_pixels);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        SetRunlengthPackets(image,packets);
      }
    else
      {
        Quantum
          *quantum_scanline;

        register Quantum
          *r;

        /*
          Convert image to PseudoClass runlength-encoded packets.
        */
        quantum_scanline=(Quantum *)
          AllocateMemory(image->columns*sizeof(Quantum));
        if (quantum_scanline == (Quantum *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (y=0; y < (int) image->rows; y++)
        {
          p=scanlines[y];
          r=quantum_scanline;
          switch (ping_info->bit_depth)
          {
            case 1:
            {
              register int
                bit;

              for (x=0; x < ((int) image->columns-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((image->columns % 8) != 0)
                {
                  for (bit=7; bit >= (int) (8-(image->columns % 8)); bit--)
                    *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((int) image->columns-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((image->columns % 4) != 0)
                {
                  for (i=3; i >= (int) (4-(image->columns % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((int) image->columns-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((image->columns % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              for (x=0; x < (int) image->columns; x++)
                *r++=(*p++);
              break;
            }
            case 16:
            {
              for (x=0; x < (int) image->columns; x++)
              {
                ReadQuantum(*r,p);
                r++;
              }
              break;
            }
            default:
              break;
          }
          /*
            Transfer image scanline.
          */
          r=quantum_scanline;
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*r++);
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        FreeMemory((char *) quantum_scanline);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        FreeMemory((char *) quantum_scanline);
        SetRunlengthPackets(image,packets);
        if (image->class == PseudoClass)
          SyncImage(image);
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            /*
              Image has a transparent background.
            */
            image->class=DirectClass;
            image->matte=True;
            q=image->pixels;
            for (i=0; i < packets; i++)
            {
              index=q->index;
              q->index=Opaque;
              if (ping_info->color_type == PNG_COLOR_TYPE_PALETTE)
                {
                  if (index < ping_info->num_trans)
                    q->index=UpScale(ping_info->trans[index]);
                }
              else
                if (index == transparent_color.index)
                  q->index=Transparent;
              q++;
            }
          }
      }
    if (ping_info->num_text > 0)
      for (i=0; i < ping_info->num_text; i++)
      {
        if (Latin1Compare(ping_info->text[i].key,"Comment") == 0)
          PNGTextChunk(ping_info,i,&image->comments);
        if (Latin1Compare(ping_info->text[i].key,"Delay") == 0)
          if (image_info->delay == (char *) NULL)
            {
              char
                *delay;

              delay=(char *) NULL;
              PNGTextChunk(ping_info,i,&delay);
              image->delay=atoi(delay);
              FreeMemory(delay);
            }
        if (Latin1Compare(ping_info->text[i].key,"Description") == 0)
          PNGTextChunk(ping_info,i,&image->comments);
        if (Latin1Compare(ping_info->text[i].key,"Directory") == 0)
          PNGTextChunk(ping_info,i,&image->directory);
        if (Latin1Compare(ping_info->text[i].key,"Label") == 0)
          PNGTextChunk(ping_info,i,&image->label);
        if (Latin1Compare(ping_info->text[i].key,"Montage") == 0)
          PNGTextChunk(ping_info,i,&image->montage);
        if (Latin1Compare(ping_info->text[i].key,"Page") == 0)
          if (image_info->page == (char *) NULL)
            PNGTextChunk(ping_info,i,&image->page);
        if (Latin1Compare(ping_info->text[i].key,"Scene") == 0)
          {
            char
              *scene;

            scene=(char *) NULL;
            PNGTextChunk(ping_info,i,&scene);
            image->scene=atoi(scene);
            FreeMemory(scene);
          }
        if (Latin1Compare(ping_info->text[i].key,"Signature") == 0)
          PNGTextChunk(ping_info,i,&image->signature);
        if (Latin1Compare(ping_info->text[i].key,"Title") == 0)
          PNGTextChunk(ping_info,i,&image->label);
      }
    /*
      Free memory.
    */
    png_destroy_read_struct(&ping,&ping_info,&end_info);
    FreeMemory((char *) png_pixels);
    FreeMemory((char *) scanlines);
  } while (Latin1Compare(image_info->magick,"MNG") == 0);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}
#else
Image *ReadPNGImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"PNG library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNMImage reads a Portable Anymap image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadPNMImage routine is:
%
%      image=ReadPNMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

static unsigned int PNMInteger(Image *image,const unsigned int base)
{
#define P7Comment  "END_OF_COMMENTS"

  int
    c;

  unsigned int
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    c=fgetc(image->file);
    if (c == EOF)
      return(0);
    if (c == '#')
      {
        register char
          *p,
          *q;

        unsigned int
          length;

        /*
          Read comment.
        */
        if (image->comments != (char *) NULL)
          {
            p=image->comments+Extent(image->comments);
            length=p-image->comments;
          }
        else
	  {
            length=MaxTextExtent;
            image->comments=(char *) AllocateMemory(length*sizeof(char));
            p=image->comments;
	  }
        q=p;
        if (image->comments != (char *) NULL)
          for ( ; (c != EOF) && (c != '\n'); p++)
          {
            if ((p-image->comments+sizeof(P7Comment)) >= length)
              {
                length<<=1;
                length+=MaxTextExtent;
                image->comments=(char *) ReallocateMemory((char *)
                  image->comments,length*sizeof(char));
                if (image->comments == (char *) NULL)
                  break;
                p=image->comments+Extent(image->comments);
              }
            c=fgetc(image->file);
            *p=(char) c;
            *(p+1)='\0';
          }
        if (image->comments == (char *) NULL)
          {
            MagickWarning(ResourceLimitWarning,"Memory allocation failed",
              (char *) NULL);
            return(0);
          }
        if (Latin1Compare(q,P7Comment) == 0)
          *q='\0';
        continue;
      }
  } while (!isdigit(c));
  if (base == 2)
    return(c-'0');
  /*
    Evaluate number.
  */
  value=0;
  do
  {
    value*=10;
    value+=c-'0';
    c=fgetc(image->file);
    if (c == EOF)
      return(0);
  }
  while (isdigit(c));
  return(value);
}

Image *ReadPNMImage(const ImageInfo *image_info)
{
#define MaxRawValue  255

  char
    format;

  Image
    *image;

  int
    y;

  MonitorHandler
    handler;

  Quantum
    *scale;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  unsigned int
    max_value,
    status;

  unsigned long
    max_packets;

  unsigned short
    blue,
    green,
    index,
    red;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read PNM image.
  */
  status=ReadData((char *) &format,1,1,image->file);
  do
  {
    /*
      Verify PNM identifier.
    */
    if ((status == False) || (format != 'P'))
      PrematureExit(CorruptImageWarning,"Not a PNM image file",image);
    /*
      Initialize image structure.
    */
    format=fgetc(image->file);
    if (format == '7')
      (void) PNMInteger(image,10);
    image->columns=PNMInteger(image,10);
    image->rows=PNMInteger(image,10);
    if ((format == '1') || (format == '4'))
      max_value=1;  /* bitmap */
    else
      max_value=PNMInteger(image,10);
    if ((format != '3') && (format != '6'))
      {
        image->class=PseudoClass;
        image->colors=Min(max_value,MaxRGB)+1;
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    if ((image->columns*image->rows) == 0)
      PrematureExit(CorruptImageWarning,
        "Unable to read image: image dimensions are zero",image);
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    if ((format == '1') || (format == '4'))
      max_packets=Max((image->columns*image->rows+8) >> 4,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    scale=(Quantum *) NULL;
    if (image->class == PseudoClass)
      {
        /*
          Create colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        if (format != '7')
          for (i=0; i < (int) image->colors; i++)
          {
            image->colormap[i].red=(Quantum)
              ((long) (MaxRGB*i)/(image->colors-1));
            image->colormap[i].green=(Quantum)
              ((long) (MaxRGB*i)/(image->colors-1));
            image->colormap[i].blue=(Quantum)
              ((long) (MaxRGB*i)/(image->colors-1));
          }
        else
          {
            /*
              Initialize 332 colormap.
            */
            i=0;
            for (red=0; red < 8; red++)
              for (green=0; green < 8; green++)
                for (blue=0; blue < 4; blue++)
                {
                  image->colormap[i].red=(Quantum) ((long) (red*MaxRGB)/7);
                  image->colormap[i].green=(Quantum) ((long) (green*MaxRGB)/7);
                  image->colormap[i].blue=(Quantum) ((long) (blue*MaxRGB)/3);
                  i++;
                }
          }
      }
    else
      if (max_value != MaxRGB)
        {
          /*
            Compute pixel scaling table.
          */
          scale=(Quantum *) AllocateMemory((max_value+1)*sizeof(Quantum));
          if (scale == (Quantum *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          for (i=0; i <= (int) max_value; i++)
            scale[i]=(Quantum) ((i*MaxRGB+(max_value >> 1))/max_value);
        }
    /*
      Convert PNM pixels to runlength-encoded MIFF packets.
    */
    q=image->pixels;
    SetRunlengthEncoder(q);
    switch (format)
    {
      case '1':
      {
        /*
          Convert PBM image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=!PNMInteger(image,2);
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      PrematureExit(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        break;
      }
      case '2':
      {
        /*
          Convert PGM image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            index=PNMInteger(image,10);
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        if (scale != (Quantum *) NULL)
                          FreeMemory((char *) scale);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        break;
      }
      case '3':
      {
        /*
          Convert PNM image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            red=PNMInteger(image,10);
            green=PNMInteger(image,10);
            blue=PNMInteger(image,10);
            if (scale != (Quantum *) NULL)
              {
                red=scale[red];
                green=scale[green];
                blue=scale[blue];
              }
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        if (scale != (Quantum *) NULL)
                          FreeMemory((char *) scale);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=0;
                q->length=0;
            }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        break;
      }
      case '4':
      {
        unsigned char
          bit,
          byte;

        /*
          Convert PBM raw image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          bit=0;
          byte=0;
          for (x=0; x < (int) image->columns; x++)
          {
            if (bit == 0)
              byte=fgetc(image->file);
            index=(byte & 0x80) ? 0 : 1;
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      PrematureExit(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
            bit++;
            if (bit == 8)
              bit=0;
            byte<<=1;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        break;
      }
      case '5':
      case '7':
      {
        /*
          Convert PGM raw image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if (max_value <= MaxRawValue)
              index=fgetc(image->file);
            else
              index=LSBFirstReadShort(image->file);
            if (index > max_value)
              index=max_value;
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      PrematureExit(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        break;
      }
      case '6':
      {
        /*
          Convert PNM raster image to runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            if (max_value <= MaxRawValue)
              {
                red=fgetc(image->file);
                green=fgetc(image->file);
                blue=fgetc(image->file);
              }
            else
              {
                red=LSBFirstReadShort(image->file);
                green=LSBFirstReadShort(image->file);
                blue=LSBFirstReadShort(image->file);
              }
            if (scale != (Quantum *) NULL)
              {
                red=scale[red];
                green=scale[green];
                blue=scale[blue];
              }
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        if (scale != (Quantum *) NULL)
                          FreeMemory((char *) scale);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=0;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        handler=SetMonitorHandler((MonitorHandler) NULL);
        (void) SetMonitorHandler(handler);
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Not a PNM image file",image);
    }
    if (scale != (Quantum *) NULL)
      FreeMemory((char *) scale);
    if (feof(image->file))
      MagickWarning(CorruptImageWarning,"not enough pixels",image->filename);
    SetRunlengthPackets(image,packets);
    if (image->class == PseudoClass)
      SyncImage(image);
    else
      (void) IsPseudoClass(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if ((format == '1') || (format == '2') || (format == '3'))
      do
      {
        /*
          Skip to end of line.
        */
        status=ReadData(&format,1,1,image->file);
        if (status == False)
          break;
      } while (format != '\n');
    status=ReadData((char *) &format,1,1,image->file);
    if ((status == True) && (format == 'P'))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while ((status == True) && (format == 'P'));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P S I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPSImage reads a Adobe Postscript image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPSImage routine is:
%
%      image=ReadPSImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPSImage(const ImageInfo *image_info)
{
#define BoundingBox  "%%BoundingBox:"
#define DocumentMedia  "%%DocumentMedia:"
#define PageBoundingBox  "%%PageBoundingBox:"
#define PostscriptLevel  "%!PS-"
#define ShowPage  "showpage"

  char
    density[MaxTextExtent],
    command[MaxTextExtent],
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    options[MaxTextExtent],
    postscript_filename[MaxTextExtent],
    translate_geometry[MaxTextExtent];

  DelegateInfo
    delegate_info;

  double
    dx_resolution,
    dy_resolution;

  FILE
    *file;

  Image
    *image,
    *next_image;

  int
    c,
    count,
    status;

  long int
    filesize;

  PointInfo
    lower,
    upper;

  RectangleInfo
    bounding_box,
    box;

  register char
    *p;

  register int
    i;

  unsigned int
    alias_bits,
    eps_level,
    height,
    level,
    width;

  if (image_info->monochrome)
    {
      if (!GetDelegateInfo("gs-mono",(char *) NULL,&delegate_info))
        return((Image *) NULL);
    }
  else
    if (!GetDelegateInfo("gs-color",(char *) NULL,&delegate_info))
      return((Image *) NULL);
  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Open temporary output file.
  */
  TemporaryFilename(postscript_filename);
  file=fopen(postscript_filename,WriteBinaryType);
  if (file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to write file",image);
  FormatString(translate_geometry,"%f %f translate\n              ",0.0,0.0);
  (void) fputs(translate_geometry,file);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  FormatString(density,"%gx%g",image->x_resolution,image->y_resolution);
  bounding_box.width=612;
  bounding_box.height=792;
  bounding_box.x=0;
  bounding_box.y=0;
  (void) ParseImageGeometry(PSPageGeometry,&bounding_box.x,&bounding_box.y,
    &bounding_box.width,&bounding_box.height);
  /*
    Determine page geometry from the Postscript bounding box.
  */
  filesize=0;
  if (Latin1Compare(image_info->magick,"EPT") == 0)
    {
      /*
        Dos binary file header.
      */
      (void) LSBFirstReadLong(image->file);
      count=LSBFirstReadLong(image->file);
      filesize=LSBFirstReadLong(image->file);
      for (i=0; i < (count-12); i++)
        (void) fgetc(image->file);
    }
  box.width=0;
  box.height=0;
  /*
    Copy Postscript to temporary file.
  */
  level=0;
  eps_level=0;
  p=command;
  for (i=0; (Latin1Compare(image_info->magick,"EPT") != 0) || i < filesize; i++)
  {
    c=fgetc(image->file);
    if (c == EOF)
      break;
    (void) fputc(c,file);
    *p++=c;
    if ((c != '\n') && (c != '\r') && ((p-command) < (MaxTextExtent-1)))
      continue;
    *p='\0';
    p=command;
    if (strncmp(PostscriptLevel,command,Extent(PostscriptLevel)) == 0)
      (void) sscanf(command,"%%!PS-Adobe-%d.0 EPSF-%d.0",&level,&eps_level);
    if (strncmp(ShowPage,command,Extent(ShowPage)) == 0)
      eps_level=0;
    /*
      Parse a bounding box statement.
    */
    count=0;
    if (strncmp(BoundingBox,command,Extent(BoundingBox)) == 0)
      count=sscanf(command,"%%%%BoundingBox: %f %f %f %f",&lower.x,&lower.y,
        &upper.x,&upper.y);
    if (strncmp(DocumentMedia,command,Extent(DocumentMedia)) == 0)
      count=sscanf(command,"%%%%DocumentMedia: %*s %f %f",&upper.x,&upper.y)+2;
    if (strncmp(PageBoundingBox,command,Extent(PageBoundingBox)) == 0)
      count=sscanf(command,"%%%%PageBoundingBox: %f %f %f %f",
        &lower.x,&lower.y,&upper.x,&upper.y);
    if (count != 4)
      continue;
    if ((lower.x > upper.x) || (lower.y > upper.y))
      continue;
    /*
      Set Postscript render geometry.
    */
    FormatString(translate_geometry,"%f %f translate\n",-lower.x,-lower.y);
    width=(unsigned int) (upper.x-lower.x+1);
    if ((float) ((int) upper.x) != upper.x)
      width++;
    height=(unsigned int) (upper.y-lower.y+1);
    if ((float) ((int) upper.y) != upper.y)
      height++;
    if ((width <= box.width) && (height <= box.height))
      continue;
    bounding_box.width=width;
    bounding_box.height=height;
    box=bounding_box;
  }
  if (eps_level != 0)
    (void) fputs("showpage\n",file);
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&bounding_box.x,&bounding_box.y,
      &bounding_box.width,&bounding_box.height);
  FormatString(geometry,"%ux%u",(unsigned int) (((bounding_box.width*
    image->x_resolution)/dx_resolution)+0.5),(unsigned int)
    (((bounding_box.height*image->y_resolution)/dy_resolution)+0.5));
  if (ferror(file))
    {
      MagickWarning(FileOpenWarning,"An error has occurred writing to file",
        postscript_filename);
      (void) fclose(file);
      return((Image *) NULL);
    }
  (void) fseek(file,0L,SEEK_SET);
  (void) fputs(translate_geometry,file);
  (void) fclose(file);
  CloseImage(image);
  filesize=image->filesize;
  DestroyImage(image);
  /*
    Use Ghostscript to convert Postscript image.
  */
  *options='\0';
  if (image_info->subrange != 0)
    FormatString(options,"-dFirstPage=%u -dLastPage=%u",
      image_info->subimage+1,image_info->subimage+image_info->subrange);
  (void) strcpy(filename,image_info->filename);
  TemporaryFilename((char *) image_info->filename);
  alias_bits=image_info->dither && !image_info->monochrome ? 4 : 1;
  FormatString(command,delegate_info.commands,alias_bits,alias_bits,geometry,
    density,options,image_info->filename,postscript_filename);
  ProgressMonitor(RenderPostscriptText,0,8);
  status=SystemCommand(image_info->verbose,command);
  if (!IsAccessible(image_info->filename))
    {
      /*
        Ghostscript requires a showpage operator.
      */
      file=fopen(postscript_filename,AppendBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to write file",image);
      (void) fputs("showpage\n",file);
      (void) fclose(file);
      status=SystemCommand(image_info->verbose,command);
    }
  (void) remove(postscript_filename);
  ProgressMonitor(RenderPostscriptText,7,8);
  if (status)
    {
      /*
        Ghostscript has failed-- try the Display Postscript Extension.
      */
      (void) strcpy((char *) image_info->filename,filename);
      image=ReadDPSImage(image_info);
      if (image != (Image *) NULL)
        return(image);
      MagickWarning(CorruptImageWarning,"Postscript delegation failed",
        image_info->filename);
      return((Image *) NULL);
    }
  image=ReadPNMImage(image_info);
  (void) remove(image_info->filename);
  if (image == (Image *) NULL)
    {
      MagickWarning(CorruptImageWarning,"Postscript delegation failed",
        image_info->filename);
      return((Image *) NULL);
    }
  (void) strcpy((char *) image_info->filename,filename);
  do
  {
    (void) strcpy(image->magick,"PS");
    (void) strcpy(image->filename,image_info->filename);
    image->filesize=filesize;
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P S D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPSDImage reads an Adobe Photoshop image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPSDImage routine is:
%
%      image=ReadPSDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPSDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadPSDImage(const ImageInfo *image_info)
{
#define BitmapMode  0
#define GrayscaleMode  1
#define IndexedMode  2
#define CMYKMode  4

  typedef struct _ChannelInfo
  {
    short int
      type;

    unsigned long
      size;
  } ChannelInfo;

  typedef struct _LayerInfo
  {
    unsigned int
      width,
      height;

    int
      x,
      y;

    unsigned short
      channels;

    ChannelInfo
      channel_info[24];

    char
      blendkey[4];

    unsigned char
      opacity,
      clipping,
      flags;

    Image
      *image;
  } LayerInfo;

  typedef struct _PSDHeader
  {
    char
      signature[4];

    unsigned short
      channels,
      version;

    unsigned char
      reserved[6];

    unsigned int
      rows,
      columns;

    unsigned short
      depth,
      mode;
  } PSDHeader;

  char
    type[4];

  Image
    *image;

  int
    number_layers;

  LayerInfo
    *layer_info;

  long
    count,
    length,
    size;

  PSDHeader
    psd_header;

  register int
    i,
    j,
    k;

  register RunlengthPacket
    *q;

  unsigned int
    status;

  unsigned short
    compression;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read image header.
  */
  status=ReadData((char *) psd_header.signature,1,4,image->file);
  psd_header.version=MSBFirstReadShort(image->file);
  if ((status == False) || (strncmp(psd_header.signature,"8BPS",4) != 0) ||
      (psd_header.version != 1))
    PrematureExit(CorruptImageWarning,"Not a PSD image file",image);
  (void) ReadData((char *) psd_header.reserved,1,6,image->file);
  psd_header.channels=MSBFirstReadShort(image->file);
  psd_header.rows=MSBFirstReadLong(image->file);
  psd_header.columns=MSBFirstReadLong(image->file);
  psd_header.depth=MSBFirstReadShort(image->file);
  psd_header.mode=MSBFirstReadShort(image->file);
  /*
    Initialize image.
  */
  if (psd_header.mode != CMYKMode)
    image->matte=psd_header.channels >= 4;
  image->columns=psd_header.columns;
  image->rows=psd_header.rows;
  image->depth=Max(psd_header.depth,QuantumDepth);
  length=MSBFirstReadLong(image->file);
  if ((psd_header.mode == BitmapMode) ||
      (psd_header.mode == GrayscaleMode) ||
      (psd_header.mode == IndexedMode) || (length > 0))
    {
      /*
        Create colormap.
      */
      image->class=PseudoClass;
      image->colors=1 << image->depth;
      image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
        image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
        image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
      }
      if (length > 0)
        {
          /*
            Read PSD raster colormap.
          */
          for (i=0; i < (int) image->colors; i++)
            image->colormap[i].red=fgetc(image->file);
          for (i=0; i < (int) image->colors; i++)
            image->colormap[i].green=fgetc(image->file);
          for (i=0; i < (int) image->colors; i++)
            image->colormap[i].blue=fgetc(image->file);
        }
    }
  length=MSBFirstReadLong(image->file);
  while (length > 0)
  {
    /*
      Read image resource block.
    */
    status=ReadData((char *) type,1,4,image->file);
    if ((status == False) || (strncmp(type,"8BIM",4) != 0))
      PrematureExit(CorruptImageWarning,"Not a PSD image file",image);
    (void) MSBFirstReadShort(image->file);
    count=fgetc(image->file);
    if (count > 0)
      for (i=0; i < count; i++)
        (void) fgetc(image->file);
    if (!(count & 0x01))
      {
        (void) fgetc(image->file);
        length--;
      }
    size=MSBFirstReadLong(image->file);
    for (i=0; i < size; i++)
      (void) fgetc(image->file);
    length-=(count+size+11);
    if (size & 0x01)
      {
        (void) fgetc(image->file);
        length--;
      }
  }
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  layer_info=(LayerInfo *) NULL;
  number_layers=0;
  length=MSBFirstReadLong(image->file);
  if (length > 0)
    {
      /*
        Read layer and mask block.
      */
      size=MSBFirstReadLong(image->file);
      number_layers=MSBFirstReadShort(image->file);
      number_layers=AbsoluteValue(number_layers);
      layer_info=(LayerInfo *) AllocateMemory(number_layers*sizeof(LayerInfo));
      if (layer_info == (LayerInfo *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < number_layers; i++)
      {
        layer_info[i].y=MSBFirstReadLong(image->file);
        layer_info[i].x=MSBFirstReadLong(image->file);
        layer_info[i].height=MSBFirstReadLong(image->file)-layer_info[i].y;
        layer_info[i].width=MSBFirstReadLong(image->file)-layer_info[i].x;
        layer_info[i].channels=MSBFirstReadShort(image->file);
        if (layer_info[i].channels > 24)
          PrematureExit(CorruptImageWarning,"Not a PSD image file",image);
        for (j=0; j < (int) layer_info[i].channels; j++)
        {
          layer_info[i].channel_info[j].type=MSBFirstReadShort(image->file);
          layer_info[i].channel_info[j].size=MSBFirstReadLong(image->file);
        }
        status=ReadData((char *) type,1,4,image->file);
        if ((status == False) || (strncmp(type,"8BIM",4) != 0))
          PrematureExit(CorruptImageWarning,"Not a PSD image file",image);
        (void) ReadData((char *) layer_info[i].blendkey,1,4,image->file);
        layer_info[i].opacity=fgetc(image->file);
        layer_info[i].clipping=fgetc(image->file);
        layer_info[i].flags=fgetc(image->file);
        (void) fgetc(image->file);  /* filler */
        size=MSBFirstReadLong(image->file);
        for (j=0; j < size; j++)
          (void) fgetc(image->file);
      }
      /*
        Read pixel data for each layer.
      */
      for (i=0; i < number_layers; i++)
      {
        /*
          Allocate layered image.
        */
        layer_info[i].image=AllocateImage(image_info);
        if (layer_info[i].image == (Image *) NULL)
          {
            for (j=0; j < i; j++)
              DestroyImage(layer_info[j].image);
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
        layer_info[i].image->file=image->file;
        layer_info[i].image->class=image->class;
        if (psd_header.mode != CMYKMode)
          layer_info[i].image->matte=layer_info[i].channels >= 4;
        if (image->colormap != (ColorPacket *) NULL)
          {
            /*
              Convert pixels to Runlength encoded.
            */
            layer_info[i].image->colormap=(ColorPacket *)
              AllocateMemory(image->colors*sizeof(ColorPacket));
            if (layer_info[i].image->colormap == (ColorPacket *) NULL)
              {
                for (j=0; j < i; j++)
                  DestroyImage(layer_info[j].image);
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              }
          }
        layer_info[i].image->columns=layer_info[i].width;
        layer_info[i].image->rows=layer_info[i].height;
        layer_info[i].image->packets=
          layer_info[i].image->columns*layer_info[i].image->rows;
        layer_info[i].image->pixels=(RunlengthPacket *) AllocateMemory(
          (layer_info[i].image->packets+256)*sizeof(RunlengthPacket));
        if (layer_info[i].image->pixels == (RunlengthPacket *) NULL)
          {
            for (j=0; j < i; j++)
              DestroyImage(layer_info[j].image);
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
        SetImage(layer_info[i].image);
        /*
          Convert pixels to Runlength encoded packets.
        */
        for (j=0; j < (int) layer_info[i].channels; j++)
        {
          compression=MSBFirstReadShort(layer_info[i].image->file);
          if (compression != 0)
            {
              for (k=0; k < (int) layer_info[i].image->rows; k++)
                (void) MSBFirstReadShort(image->file);
              (void) PackbitsDecodeImage(layer_info[i].image,
                layer_info[i].channel_info[j].type);
            }
          else
            {
              /*
                Read uncompressed pixel data as separate planes.
              */
              q=layer_info[i].image->pixels;
              for (k=0; k < (int) layer_info[i].image->packets; k++)
              {
                switch (layer_info[i].channel_info[j].type)
                {
                  case 0:
                  {
                    ReadQuantumFile(q->red);
                    q->index=q->red;
                    break;
                  }
                  case 1:
                  {
                    ReadQuantumFile(q->green);
                    break;
                  }
                  case 2:
                  {
                    ReadQuantumFile(q->blue);
                    break;
                  }
                  case 3:
                  default:
                  {
                    ReadQuantumFile(q->index);
                    break;
                  }
                }
                q->length=0;
                q++;
              }
            }
        }
        if (layer_info[i].image->class == PseudoClass)
          SyncImage(layer_info[i].image);
        CondenseImage(layer_info[i].image);
        if (layer_info[i].opacity != Opaque)
          {
            /*
              Correct for opacity level.
            */
            q=layer_info[i].image->pixels;
            for (k=0; k < (int) layer_info[i].image->packets; k++)
            {
              q->index=(int) (q->index*layer_info[i].opacity)/Opaque;
              q++;
            }
          }
        if (psd_header.mode == CMYKMode)
          {
            /*
              Correct CMYK levels.
            */
            q=layer_info[i].image->pixels;
            for (i=0; i < (int) layer_info[i].image->packets; i++)
            {
              q->red=MaxRGB-q->red;
              q->green=MaxRGB-q->green;
              q->blue=MaxRGB-q->blue;
              q->index=MaxRGB-q->index;
              q++;
            }
            TransformRGBImage(layer_info[i].image,CMYKColorspace);
          }
        layer_info[i].image->file=(FILE *) NULL;
      }
      for (i=0; i < 4; i++)
        (void) fgetc(image->file);
    }
  /*
    Convert pixels to Runlength encoded.
  */
  compression=MSBFirstReadShort(image->file);
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory((image->packets+256)*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  if (compression != 0)
    {
      /*
        Read Packbit encoded pixel data as separate planes.
      */
      for (i=0; i < (int) (image->rows*psd_header.channels); i++)
        (void) MSBFirstReadShort(image->file);
      for (i=0; i < (int) psd_header.channels; i++)
        (void) PackbitsDecodeImage(image,i);
    }
  else
    {
      /*
        Read uncompressed pixel data as separate planes.
      */
      for (i=0; i < (int) psd_header.channels; i++)
      {
        q=image->pixels;
        for (j=0; j < (int) image->packets; j++)
        {
          switch (i)
          {
            case 0:
            {
              ReadQuantumFile(q->red);
              q->index=q->red;
              break;
            }
            case 1:
            {
              ReadQuantumFile(q->green);
              break;
            }
            case 2:
            {
              ReadQuantumFile(q->blue);
              break;
            }
            case 3:
            default:
            {
              ReadQuantumFile(q->index);
              break;
            }
          }
          q->length=0;
          q++;
        }
      }
    }
  if (image->class == PseudoClass)
    SyncImage(image);
  if (psd_header.mode == CMYKMode)
    {
      /*
        Correct CMYK levels.
      */
      q=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        q->red=MaxRGB-q->red;
        q->green=MaxRGB-q->green;
        q->blue=MaxRGB-q->blue;
        q->index=MaxRGB-q->index;
        q++;
      }
      TransformRGBImage(image,CMYKColorspace);
    }
  for (i=0; i < number_layers; i++)
  {
    /*
      Composite layer onto image.
    */
    CompositeImage(image,OverCompositeOp,layer_info[i].image,
      layer_info[i].x,layer_info[i].y);
    layer_info[i].image->colormap=(ColorPacket *) NULL;
    DestroyImage(layer_info[i].image);
  }
  image->matte=False;
  if (psd_header.mode != CMYKMode)
   image->matte=psd_header.channels >= 4;
  CondenseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d R G B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadRGBImage reads an image of raw red, green, and blue bytes and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadRGBImage routine is:
%
%      image=ReadRGBImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRGBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadRGBImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    count,
    y;

  register int
    i,
    x;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  unsigned int
    packet_size;

  unsigned short
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      OpenImage(image_info,image,ReadBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) fgetc(image->file);
    }
  /*
    Allocate memory for a scanline.
  */
  packet_size=3*(image->depth >> 3);
  if (Latin1Compare(image_info->magick,"RGBA") == 0)
    {
      image->matte=True;
      packet_size=4*(image->depth >> 3);
    }
  scanline=(unsigned char *)
    AllocateMemory(packet_size*image->tile_info.width*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (int) image->rows; y++)
        (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
          image->file);
    }
  do
  {
    /*
      Initialize image structure.
    */
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert raster image to runlength-encoded packets.
    */
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  RGBRGBRGBRGBRGBRGB...
        */
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        q=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            ReadQuantum(q->green,p);
            ReadQuantum(q->blue,p);
            q->index=0;
            if (image->matte)
              ReadQuantum(q->index,p);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        packet_size=image->depth >> 3;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            q->index=0;
            q->length=0;
            q++;
          }
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->green,p);
            q++;
          }
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          q=image->pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->blue,p);
            q++;
          }
          if (image->matte)
            {
              (void) ReadData((char *) scanline,packet_size,
                image->tile_info.width,image->file);
              p=scanline+packet_size*image->tile_info.x;
              q=image->pixels+y*image->columns;
              for (x=0; x < (int) image->columns; x++)
              {
                ReadQuantum(q->index,p);
                q++;
              }
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        break;
      }
      case PlaneInterlace:
      case PartitionInterlace:
      {
        unsigned int
          span;

        /*
          Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("R",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        packet_size=image->depth >> 3;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        i=0;
        span=image->rows*(image->matte ? 4 : 3);
        q=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadData((char *) scanline,packet_size,
              image->tile_info.width,image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->red,p);
            q->index=0;
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              ProgressMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("G",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        q=image->pixels;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->green,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              ProgressMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("B",image->filename);
            OpenImage(image_info,image,ReadBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        q=image->pixels;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
          p=scanline+packet_size*image->tile_info.x;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(q->blue,p);
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              ProgressMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (y=0; y < count; y++)
          (void) ReadData((char *) scanline,packet_size,image->tile_info.width,
            image->file);
        if (image->matte)
          {
            /*
              Read matte channel.
            */
            if (image_info->interlace == PartitionInterlace)
              {
                CloseImage(image);
                AppendImageFormat("A",image->filename);
                OpenImage(image_info,image,ReadBinaryType);
                if (image->file == (FILE *) NULL)
                  PrematureExit(FileOpenWarning,"Unable to open file",image);
              }
            q=image->pixels;
            for (y=0; y < image->tile_info.y; y++)
              (void) ReadData((char *) scanline,packet_size,
                image->tile_info.width,image->file);
            for (y=0; y < (int) image->rows; y++)
            {
              (void) ReadData((char *) scanline,packet_size,
                image->tile_info.width,image->file);
              p=scanline+packet_size*image->tile_info.x;
              for (x=0; x < (int) image->columns; x++)
              {
                ReadQuantum(q->index,p);
                q++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,span))
                  ProgressMonitor(LoadImageText,i,span);
              i++;
            }
            count=image->tile_info.height-image->rows-image->tile_info.y;
            for (y=0; y < count; y++)
              (void) ReadData((char *) scanline,packet_size,
                image->tile_info.width,image->file);
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        break;
      }
    }
    CondenseImage(image);
    if (feof(image->file))
      MagickWarning(CorruptImageWarning,"not enough pixels",image->filename);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadData((char *) scanline,packet_size,image->columns,image->file);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (count > 0);
  FreeMemory((char *) scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d R L A I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadRLAImage reads a run-length encoded Wavefront RLA image file
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  Note:  This module was contributed by Lester Vecsey (master@internexus.net).
%
%  The format of the ReadRLAImage routine is:
%
%      image=ReadRLAImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRLAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadRLAImage(const ImageInfo *image_info)
{
  typedef struct _WindowFrame
  {
    short
      left,
      right,
      bottom,
      top;
  } WindowFrame;

  typedef struct _RLAHeader
  {
    WindowFrame
      window,
      active_window;

    short
      frame,
      storage_type,
      number_channels,
      number_matte_channels,
      number_auxillary_channels,
      revision;

    char
      gamma[16],
      red_primary[24],
      green_primary[24],
      blue_primary[24],
      white_point[24];

    long
      job_number;

    char
      name[128],
      description[128],
      program[64],
      machine[32],
      user[32],
      date[20],
      aspect[24],
      aspect_ratio[8],
      chan[32];

    short
      field;

    char
      time[12],
      filter[32];

    short
      bits_per_channel,
      matte_type,
      matte_bits,
      auxillary_type,
      auxillary_bits;

    char
      auxillary[32],
      space[36];

    long
      next;
  } RLAHeader;

  Image
    *image;

  int
    channel,
    length,
    runlength,
    y;

  long
    *scanlines;

  register int
    i;

  register RunlengthPacket
    *q;

  RLAHeader
    rla_header;

  unsigned char
    byte;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  rla_header.window.left=MSBFirstReadShort(image->file);
  rla_header.window.right=MSBFirstReadShort(image->file);
  rla_header.window.bottom=MSBFirstReadShort(image->file);
  rla_header.window.top=MSBFirstReadShort(image->file);
  rla_header.active_window.left=MSBFirstReadShort(image->file);
  rla_header.active_window.right=MSBFirstReadShort(image->file);
  rla_header.active_window.bottom=MSBFirstReadShort(image->file);
  rla_header.active_window.top=MSBFirstReadShort(image->file);
  rla_header.frame=MSBFirstReadShort(image->file);
  rla_header.storage_type=MSBFirstReadShort(image->file);
  rla_header.number_channels=MSBFirstReadShort(image->file);
  rla_header.number_matte_channels=MSBFirstReadShort(image->file);
  if (rla_header.number_channels == 0)
    rla_header.number_channels=3;
  rla_header.number_channels+=rla_header.number_matte_channels;
  rla_header.number_auxillary_channels=MSBFirstReadShort(image->file);
  rla_header.revision=MSBFirstReadShort(image->file);
  (void) ReadData((char *) rla_header.gamma,16,1,image->file);
  (void) ReadData((char *) rla_header.red_primary,24,1,image->file);
  (void) ReadData((char *) rla_header.green_primary,24,1,image->file);
  (void) ReadData((char *) rla_header.blue_primary,24,1,image->file);
  (void) ReadData((char *) rla_header.white_point,24,1,image->file);
  rla_header.job_number=(long) MSBFirstReadLong(image->file);
  (void) ReadData((char *) rla_header.name,128,1,image->file);
  (void) ReadData((char *) rla_header.description,128,1,image->file);
  (void) ReadData((char *) rla_header.program,64,1,image->file);
  (void) ReadData((char *) rla_header.machine,32,1,image->file);
  (void) ReadData((char *) rla_header.user,32,1,image->file);
  (void) ReadData((char *) rla_header.date,20,1,image->file);
  (void) ReadData((char *) rla_header.aspect,24,1,image->file);
  (void) ReadData((char *) rla_header.aspect_ratio,8,1,image->file);
  (void) ReadData((char *) rla_header.chan,32,1,image->file);
  rla_header.field=MSBFirstReadShort(image->file);
  (void) ReadData((char *) rla_header.time,12,1,image->file);
  (void) ReadData((char *) rla_header.filter,32,1,image->file);
  rla_header.bits_per_channel=MSBFirstReadShort(image->file);
  rla_header.matte_type=MSBFirstReadShort(image->file);
  rla_header.matte_bits=MSBFirstReadShort(image->file);
  rla_header.auxillary_type=MSBFirstReadShort(image->file);
  rla_header.auxillary_bits=MSBFirstReadShort(image->file);
  (void) ReadData((char *) rla_header.auxillary,32,1,image->file);
  (void) ReadData((char *) rla_header.space,36,1,image->file);
  rla_header.next=(long) MSBFirstReadLong(image->file);
  /*
    Initialize image structure.
  */
  image->matte=rla_header.number_matte_channels != 0;
  image->depth=QuantumDepth;
  image->columns=rla_header.active_window.right-rla_header.active_window.left;
  image->rows=rla_header.active_window.top-rla_header.active_window.bottom;
  image->packets=image->columns*image->rows;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  scanlines=(long *) AllocateMemory(image->rows*sizeof(long));
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  if (*rla_header.description != '\0')
    {
      /*
        RLA image comment.
      */
      image->comments=(char *)
        AllocateMemory((Extent(rla_header.description)+1)*sizeof(char));
      if (image->comments == (char *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image)
      (void) strcpy(image->comments,rla_header.description);
    }
  /*
    Read offsets to each scanline data.
  */
  for (i=0; i < (int) image->rows; i++)
    scanlines[i]=(long) MSBFirstReadLong(image->file);
  /*
    Read image data.
  */
  q=image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    (void) fseek(image->file,scanlines[image->rows-y-1],SEEK_SET);
    for (channel=0; channel < (int) rla_header.number_channels; channel++)
    {
      length=MSBFirstReadShort(image->file);
      q=image->pixels+(y*image->columns);
      while (length > 0)
      {
        (void) ReadData((char *) &byte,1,1,image->file);
        runlength=byte;
        if (byte > 127)
          runlength=byte-256;
        length--;
        if (length == 0)
          break;
        if (runlength < 0)
          {
            while (runlength < 0)
            {
              (void) ReadData((char *) &byte,1,1,image->file);
              length--;
              switch (channel)
              {
                case 0:
                {
                  q->red=UpScale(byte);
                  q->index=0;
                  q->length=0;
                  break;
                }
                case 1:
                {
                  q->green=UpScale(byte);
                  break;
                }
                case 2:
                {
                  q->blue=UpScale(byte);
                  break;
                }
                case 3:
                default:
                {
                  q->index=UpScale(byte);
                  break;
                }
              }
              q++;
              if ((q-image->pixels) >= (int) image->packets)
                break;
              runlength++;
            }
            continue;
          }
        (void) ReadData((char *) &byte,1,1,image->file);
        length--;
        runlength++;
        do
        {
          switch (channel)
          {
            case 0:
            {
              q->red=UpScale(byte);
              q->index=0;
              q->length=0;
              break;
            }
            case 1:
            {
              q->green=UpScale(byte);
              break;
            }
            case 2:
            {
              q->blue=UpScale(byte);
              break;
            }
            case 3:
            default:
            {
              q->index=UpScale(byte);
              break;
            }
          }
          q++;
          if ((q-image->pixels) >= (int) image->packets)
            break;
          runlength--;
        }
        while (runlength > 0);
      }
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
    if ((q-image->pixels) >= (int) image->packets)
      break;
  }
  for ( ;  (q-image->pixels) < (int) image->packets; q++)
    q->length=0;
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d R L E I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadRLEImage reads a run-length encoded Utah Raster Toolkit
%  image file and returns it.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  The format of the ReadRLEImage routine is:
%
%      image=ReadRLEImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRLEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadRLEImage(const ImageInfo *image_info)
{
#define SkipLinesOp  0x01
#define SetColorOp  0x02
#define SkipPixelsOp  0x03
#define ByteDataOp  0x05
#define RunDataOp  0x06
#define EOFOp  0x07

  char
    magick[12];

  Image
    *image;

  int
    opcode,
    operand,
    status,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    background_color[256],
    *colormap,
    pixel,
    plane,
    *rle_pixels;

  unsigned int
    bits_per_pixel,
    flags,
    map_length,
    number_colormaps,
    number_planes;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a RLE file.
  */
  status=ReadData((char *) magick,1,2,image->file);
  if ((status == False) || (strncmp(magick,"\122\314",2) != 0))
    PrematureExit(CorruptImageWarning,"Not a RLE image file",image);
  do
  {
    /*
      Read image header.
    */
    (void) LSBFirstReadShort(image->file);
    (void) LSBFirstReadShort(image->file);
    image->columns=LSBFirstReadShort(image->file);
    image->rows=LSBFirstReadShort(image->file);
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    image->packets=image->columns*image->rows;
    flags=fgetc(image->file);
    image->matte=flags & 0x04;
    number_planes=fgetc(image->file);
    bits_per_pixel=fgetc(image->file);
    number_colormaps=fgetc(image->file);
    map_length=1 << fgetc(image->file);
    if ((number_planes == 0) || (number_planes == 2) || (bits_per_pixel != 8) ||
        (image->columns == 0))
      PrematureExit(CorruptImageWarning,"Unsupported RLE image file",image);
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < (int) number_planes; i++)
          background_color[i]=(unsigned char) 0;
        (void) fgetc(image->file);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < (int) number_planes; i++)
          *p++=(unsigned char) fgetc(image->file);
      }
    if ((number_planes & 0x01) == 0)
      (void) fgetc(image->file);
    colormap=(unsigned char *) NULL;
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=(unsigned char *)
          AllocateMemory(number_colormaps*map_length*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        p=colormap;
        for (i=0; i < (int) number_colormaps; i++)
          for (j=0; j < (int) map_length; j++)
            *p++=XDownScale(LSBFirstReadShort(image->file));
      }
    if (flags & 0x08)
      {
        unsigned int
          length;

        /*
          Read image comment.
        */
        length=LSBFirstReadShort(image->file);
        image->comments=(char *) AllocateMemory(length*sizeof(char));
        if (image->comments == (char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) image->comments,1,length-1,image->file);
        image->comments[length-1]='\0';
        if ((length & 0x01) == 0)
          (void) fgetc(image->file);
      }
    /*
      Allocate RLE pixels.
    */
    if (image->matte)
      number_planes++;
    rle_pixels=(unsigned char *)
      AllocateMemory(image->packets*number_planes*sizeof(unsigned char));
    if (rle_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if ((flags & 0x01) && !(flags & 0x02))
      {
        /*
          Set background color.
        */
        p=rle_pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          if (!image->matte)
            for (j=0; j < (int) number_planes; j++)
              *p++=background_color[j];
          else
            {
              for (j=0; j < (int) (number_planes-1); j++)
                *p++=background_color[j];
              *p++=0;  /* initialize matte channel */
            }
        }
      }
    /*
      Read runlength-encoded image.
    */
    plane=0;
    x=0;
    y=0;
    opcode=fgetc(image->file);
    do
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            operand=LSBFirstReadShort(image->file);
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=fgetc(image->file);
          plane=(unsigned char) operand;
          if (plane == 255)
            plane=number_planes-1;
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            operand=LSBFirstReadShort(image->file);
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            operand=LSBFirstReadShort(image->file);
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          operand++;
          for (i=0; i < operand; i++)
          {
            pixel=fgetc(image->file);
            if ((y < (int) image->rows) && ((x+i) < (int) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) fgetc(image->file);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            operand=LSBFirstReadShort(image->file);
          pixel=fgetc(image->file);
          (void) fgetc(image->file);
          operand++;
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          for (i=0; i < operand; i++)
          {
            if ((y < (int) image->rows) && ((x+i) < (int) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          x+=operand;
          break;
        }
        default:
          break;
      }
      opcode=fgetc(image->file);
    } while (((opcode & 0x3f) != EOFOp) && (opcode != EOF));
    if (number_colormaps != 0)
      {
        unsigned int
          mask;

        /*
          Apply colormap transformation to image.
        */
        mask=(map_length-1);
        p=rle_pixels;
        if (number_colormaps == 1)
          for (i=0; i < (int) image->packets; i++)
          {
            *p=(unsigned char) colormap[*p & mask];
            p++;
          }
        else
          if ((number_planes >= 3) && (number_colormaps >= 3))
            for (i=0; i < (int) image->packets; i++)
              for (j=0; j < (int) number_planes; j++)
              {
                *p=(unsigned char) colormap[j*map_length+(*p & mask)];
                p++;
              }
      }
    /*
      Initialize image structure.
    */
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    q=image->pixels;
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass runlength-encoded packets.
        */
        p=rle_pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->index=0;
            if (image->matte)
              q->index=UpScale(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      {
        /*
          Create colormap.
        */
        image->class=PseudoClass;
        if (number_colormaps == 0)
          map_length=256;
        image->colors=map_length;
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        p=colormap;
        if (number_colormaps == 0)
          for (i=0; i < (int) image->colors; i++)
          {
            /*
              Grayscale.
            */
            image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
            image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
            image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
          }
        else
          if (number_colormaps == 1)
            for (i=0; i < (int) image->colors; i++)
            {
              /*
                Pseudocolor.
              */
              image->colormap[i].red=(Quantum) UpScale(i);
              image->colormap[i].green=(Quantum) UpScale(i);
              image->colormap[i].blue=(Quantum) UpScale(i);
            }
          else
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=UpScale(*p);
              image->colormap[i].green=UpScale(*(p+map_length));
              image->colormap[i].blue=UpScale(*(p+map_length*2));
              p++;
            }
        p=rle_pixels;
        if (!image->matte)
          {
            /*
              Convert raster image to PseudoClass runlength-encoded packets.
            */
            for (y=0; y < (int) image->rows; y++)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->index=(unsigned short) (*p++);
                q->length=0;
                q++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  ProgressMonitor(LoadImageText,y,image->rows);
            }
            SyncImage(image);
          }
        else
          {
            /*
              Image has a matte channel-- promote to DirectClass.
            */
            for (y=0; y < (int) image->rows; y++)
            {
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=image->colormap[*p++].red;
                q->green=image->colormap[*p++].green;
                q->blue=image->colormap[*p++].blue;
                q->index=UpScale(*p++);
                q->length=0;
                q++;
              }
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  ProgressMonitor(LoadImageText,y,image->rows);
            }
            FreeMemory(image->colormap);
            image->colormap=(ColorPacket *) NULL;
            image->class=DirectClass;
            image->colors=0;
          }
      }
    if (number_colormaps != 0)
      FreeMemory((char *) colormap);
    FreeMemory((char *) rle_pixels);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    (void) fgetc(image->file);
    status=ReadData((char *) magick,1,2,image->file);
    if ((status == True) && (strncmp(magick,"\122\314",2) == 0))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while ((status == True) && (strncmp(magick,"\122\314",2) == 0));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S G I I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSGIImage reads a SGI RGB image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadSGIImage routine is:
%
%      image=ReadSGIImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSGIImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

static void SGIDecode(unsigned char *max_packets,unsigned char *pixels)
{
  unsigned char
    count,
    pixel;

  for ( ; ;)
  {
    pixel=(*max_packets++);
    count=pixel & 0x7f;
    if (count == 0)
      break;
    if (pixel & 0x80)
      for ( ; count != 0; count--)
      {
        *pixels=(*max_packets++);
        pixels+=4;
      }
    else
      {
        pixel=(*max_packets++);
        for ( ; count != 0; count--)
        {
          *pixels=pixel;
          pixels+=4;
        }
      }
  }
}

Image *ReadSGIImage(const ImageInfo *image_info)
{
  typedef struct _SGIHeader
  {
    unsigned short
      magic;

    unsigned char
      storage,
      bytes_per_pixel;

    unsigned short
      dimension,
      columns,
      rows,
      depth;

    unsigned long
      minimum_value,
      maximum_value;

    unsigned char
      filler[492];
  } SGIHeader;

  Image
    *image;

  SGIHeader
    iris_header;

  register int
    i,
    x,
    y,
    z;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *iris_pixels;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read SGI raster header.
  */
  iris_header.magic=MSBFirstReadShort(image->file);
  do
  {
    /*
      Verify SGI identifier.
    */
    if (iris_header.magic != 0x01DA)
      PrematureExit(CorruptImageWarning,"Not a SGI RGB image",image);
    iris_header.storage=fgetc(image->file);
    iris_header.bytes_per_pixel=fgetc(image->file);
    if (iris_header.bytes_per_pixel != 1)
      PrematureExit(CorruptImageWarning,
        "Image must have 1 byte per pixel channel",image);
    iris_header.dimension=MSBFirstReadShort(image->file);
    iris_header.columns=MSBFirstReadShort(image->file);
    iris_header.rows=MSBFirstReadShort(image->file);
    iris_header.depth=MSBFirstReadShort(image->file);
    image->columns=iris_header.columns;
    image->rows=iris_header.rows;
    if (iris_header.depth < 3)
      {
        image->class=PseudoClass;
        image->colors=256;
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    iris_header.minimum_value=MSBFirstReadLong(image->file);
    iris_header.maximum_value=MSBFirstReadLong(image->file);
    (void) ReadData((char *) iris_header.filler,1,
      (unsigned int) sizeof(iris_header.filler),image->file);
    /*
      Allocate SGI pixels.
    */
    iris_pixels=(unsigned char *) AllocateMemory(4*iris_header.columns*
      iris_header.rows*sizeof(unsigned char));
    if (iris_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    if (iris_header.storage != 0x01)
      {
        unsigned char
          *scanline;

        /*
          Read standard image format.
        */
        scanline=(unsigned char *)
          AllocateMemory(iris_header.columns*sizeof(unsigned char));
        if (scanline == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (z=0; z < (int) iris_header.depth; z++)
        {
          p=iris_pixels+z;
          for (y=0; y < (int) iris_header.rows; y++)
          {
            (void) ReadData((char *) scanline,1,iris_header.columns,
              image->file);
            for (x=0; x < (int) iris_header.columns; x++)
            {
              *p=scanline[x];
              p+=4;
            }
          }
        }
        FreeMemory((char *) scanline);
      }
    else
      {
        unsigned char
          *max_packets;

        unsigned int
          data_order;

        unsigned long
          offset,
          *offsets,
          *runlength;

        /*
          Read runlength-encoded image format.
        */
        offsets=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        max_packets=(unsigned char *)
          AllocateMemory(((iris_header.columns << 1)+10)*sizeof(unsigned char));
        runlength=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (max_packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          offsets[i]=MSBFirstReadLong(image->file);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          runlength[i]=MSBFirstReadLong(image->file);
        /*
          Check data order.
        */
        offset=0;
        data_order=0;
        for (y=0; ((y < (int) iris_header.rows) && !data_order); y++)
          for (z=0; ((z < (int) iris_header.depth) && !data_order); z++)
          {
            if (offsets[y+z*iris_header.rows] < offset)
              data_order=1;
            offset=offsets[y+z*iris_header.rows];
          }
        offset=512+4*((iris_header.rows*iris_header.depth) << 1);
        if (data_order == 1)
          {
            for (z=0; z < (int) iris_header.depth; z++)
            {
              p=iris_pixels;
              for (y=0; y < (int) iris_header.rows; y++)
              {
                if (offset != offsets[y+z*iris_header.rows])
                  {
                    offset=offsets[y+z*iris_header.rows];
                    (void) fseek(image->file,(long) offset,SEEK_SET);
                  }
                (void) ReadData((char *) max_packets,1,
                  (unsigned int) runlength[y+z*iris_header.rows],image->file);
                offset+=runlength[y+z*iris_header.rows];
                SGIDecode(max_packets,p+z);
                p+=(iris_header.columns*4);
              }
            }
          }
        else
          {
            p=iris_pixels;
            for (y=0; y < (int) iris_header.rows; y++)
            {
              for (z=0; z < (int) iris_header.depth; z++)
              {
                if (offset != offsets[y+z*iris_header.rows])
                  {
                    offset=offsets[y+z*iris_header.rows];
                    (void) fseek(image->file,(long) offset,SEEK_SET);
                  }
                (void) ReadData((char *) max_packets,1,
                  (unsigned int) runlength[y+z*iris_header.rows],image->file);
                offset+=runlength[y+z*iris_header.rows];
                SGIDecode(max_packets,p+z);
              }
              p+=(iris_header.columns*4);
            }
          }
        FreeMemory(runlength);
        FreeMemory(max_packets);
        FreeMemory(offsets);
      }
    /*
      Initialize image structure.
    */
    image->matte=iris_header.depth == 4;
    image->columns=iris_header.columns;
    image->rows=iris_header.rows;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert SGI raster image to runlength-encoded packets.
    */
    q=image->pixels;
    if (image->class == DirectClass)
      {
        /*
          Convert SGI image to DirectClass runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          p=iris_pixels+((image->rows-1)-y)*(image->columns*4);
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p);
            q->green=UpScale(*(p+1));
            q->blue=UpScale(*(p+2));
            q->index=UpScale(*(p+3));
            q->length=0;
            p+=4;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      {
        unsigned short
          index;

        /*
          Create grayscale map.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(Quantum) UpScale(i);
          image->colormap[i].green=(Quantum) UpScale(i);
          image->colormap[i].blue=(Quantum) UpScale(i);
        }
        /*
          Convert SGI image to PseudoClass runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          p=iris_pixels+((image->rows-1)-y)*(image->columns*4);
          for (x=0; x < (int) image->columns; x++)
          {
            index=(unsigned short) (*p);
            q->index=index;
            q->length=0;
            p+=4;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        SyncImage(image);
      }
    FreeMemory((char *) iris_pixels);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    iris_header.magic=MSBFirstReadShort(image->file);
    if (iris_header.magic == 0x01DA)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (iris_header.magic == 0x01DA);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S T E G A N O I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSTEGANOImage reads a steganographic image hidden within another
%  image type.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadSTEGANOImage routine is:
%
%      image=ReadSTEGANOImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSTEGANOImage returns a pointer to the image
%      after reading.  A null image is returned if there is a memory shortage
%      of if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadSTEGANOImage(const ImageInfo *image_info)
{
#define UnembedBit(byte) \
{ \
  q->index|=(byte & 0x01) << shift; \
  q++; \
  if (q >= (image->pixels+image->packets-1)) \
    { \
      q=image->pixels; \
      shift--; \
      if (shift < 0) \
        break; \
    } \
}

  ImageInfo
    *local_info;

  int
    shift;

  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  Image
    *cloned_image,
    *image,
    *stegano_image;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  /*
    Initialize Image structure.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  *local_info->magick='\0';
  stegano_image=ReadImage(local_info);
  DestroyImageInfo(local_info);
  if (stegano_image == (Image *) NULL)
    return((Image *) NULL);
  if (!UncondenseImage(stegano_image))
    return((Image *) NULL);
  stegano_image->orphan=True;
  cloned_image=CloneImage(stegano_image,image->columns,image->rows,False);
  stegano_image->orphan=False;
  DestroyImage(image);
  if (cloned_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",
      stegano_image);
  image=cloned_image;
  SetImage(image);
  image->class=PseudoClass;
  image->colors=1 << QuantumDepth;
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].red=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].green=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
    image->colormap[i].blue=(Quantum) ((long) (MaxRGB*i)/(image->colors-1));
  }
  /*
    Grab embedded watermark.
  */
  shift=QuantumDepth-1;
  p=stegano_image->pixels+(stegano_image->offset % stegano_image->packets);
  q=image->pixels;
  for (i=0; i < (int) stegano_image->packets; i++)
  {
    if (stegano_image->class == PseudoClass)
      UnembedBit(p->index)
    else
      {
        UnembedBit(p->red);
        UnembedBit(p->green);
        UnembedBit(p->blue);
      }
    p++;
    if (p >= (stegano_image->pixels+stegano_image->packets-1))
      p=stegano_image->pixels;
    if (QuantumTick(i,stegano_image->packets))
      ProgressMonitor(LoadImageText,i,stegano_image->packets);
  }
  SyncImage(image);
  CondenseImage(image);
  DestroyImage(stegano_image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d S U N I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadSUNImage reads a SUN image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadSUNImage routine is:
%
%      image=ReadSUNImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSUNImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadSUNImage(const ImageInfo *image_info)
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_ENCODED  2
#define RT_FORMAT_RGB  3

  typedef struct _SUNHeader
  {
    unsigned long
      magic,
      width,
      height,
      depth,
      length,
      type,
      maptype,
      maplength;
  } SUNHeader;

  Image
    *image;

  register int
    bit,
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  SUNHeader
    sun_header;

  unsigned char
    *sun_data,
    *sun_pixels;

  unsigned int
    bytes_per_line,
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read SUN raster header.
  */
  sun_header.magic=MSBFirstReadLong(image->file);
  do
  {
    /*
      Verify SUN identifier.
    */
    if (sun_header.magic != 0x59a66a95)
      PrematureExit(CorruptImageWarning,"Not a SUN raster image",image);
    sun_header.width=MSBFirstReadLong(image->file);
    sun_header.height=MSBFirstReadLong(image->file);
    sun_header.depth=MSBFirstReadLong(image->file);
    sun_header.length=MSBFirstReadLong(image->file);
    sun_header.type=MSBFirstReadLong(image->file);
    sun_header.maptype=MSBFirstReadLong(image->file);
    sun_header.maplength=MSBFirstReadLong(image->file);
    image->columns=(unsigned int) sun_header.width;
    image->rows=(unsigned int) sun_header.height;
    if (sun_header.depth < 24)
      {
        image->class=PseudoClass;
        image->colors=sun_header.maplength;
        if (sun_header.maptype == RMT_NONE)
          image->colors=1 << sun_header.depth;
        if (sun_header.maptype == RMT_EQUAL_RGB)
          image->colors=(unsigned int) sun_header.maplength/3;
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    switch (sun_header.maptype)
    {
      case RMT_NONE:
      {
        if (sun_header.depth < 24)
          {
            /*
              Create linear color ramp.
            */
            image->colormap=(ColorPacket *)
              AllocateMemory(image->colors*sizeof(ColorPacket));
            if (image->colormap == (ColorPacket *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
            }
          }
        break;
      }
      case RMT_EQUAL_RGB:
      {
        unsigned char
          *sun_colormap;

        /*
          Read SUN raster colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        sun_colormap=(unsigned char *)
          AllocateMemory(image->colors*sizeof(unsigned char));
        if ((image->colormap == (ColorPacket *) NULL) ||
            (sun_colormap == (unsigned char *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) sun_colormap,1,image->colors,image->file);
        for (i=0; i < (int) image->colors; i++)
          image->colormap[i].red=UpScale(sun_colormap[i]);
        (void) ReadData((char *) sun_colormap,1,image->colors,image->file);
        for (i=0; i < (int) image->colors; i++)
          image->colormap[i].green=UpScale(sun_colormap[i]);
        (void) ReadData((char *) sun_colormap,1,image->colors,image->file);
        for (i=0; i < (int) image->colors; i++)
          image->colormap[i].blue=UpScale(sun_colormap[i]);
        FreeMemory((char *) sun_colormap);
        break;
      }
      case RMT_RAW:
      {
        unsigned char
          *sun_colormap;

        /*
          Read SUN raster colormap.
        */
        sun_colormap=(unsigned char *)
          AllocateMemory(sun_header.maplength*sizeof(unsigned char));
        if (sun_colormap == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) sun_colormap,1,(unsigned int)
          sun_header.maplength,image->file);
        FreeMemory((char *) sun_colormap);
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Colormap type is not supported",
          image);
    }
    sun_data=(unsigned char *)
      AllocateMemory(sun_header.length*sizeof(unsigned char));
    if (sun_data == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    status=ReadData((char *) sun_data,1,(unsigned int) sun_header.length,
      image->file);
    if ((status == False) && (sun_header.type != RT_ENCODED))
      PrematureExit(CorruptImageWarning,"Unable to read image data",image);
    sun_pixels=sun_data;
    if (sun_header.type == RT_ENCODED)
      {
        unsigned int
          height;

        /*
          Read run-length encoded raster pixels.
        */
        height=(unsigned int) sun_header.height;
        bytes_per_line=(2*sun_header.width*sun_header.depth+15)/16;
        sun_pixels=(unsigned char *)
          AllocateMemory(bytes_per_line*height*sizeof(unsigned char));
        if (sun_pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) SUNDecodeImage(sun_data,bytes_per_line,height,sun_pixels);
        FreeMemory((char *) sun_data);
      }
    /*
      Initialize image structure.
    */
    image->matte=(sun_header.depth == 32);
    image->columns=(unsigned int) sun_header.width;
    image->rows=(unsigned int) sun_header.height;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert SUN raster image to runlength-encoded packets.
    */
    p=sun_pixels;
    q=image->pixels;
    if (sun_header.depth == 1)
      for (y=0; y < (int) image->rows; y++)
      {
        /*
          Convert bitmap scanline to runlength-encoded color packets.
        */
        for (x=0; x < (int) (image->columns >> 3); x++)
        {
          for (bit=7; bit >= 0; bit--)
          {
            q->index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
            q->length=0;
            q++;
          }
          p++;
        }
        if ((image->columns % 8) != 0)
          {
            for (bit=7; bit >= (int) (8-(image->columns % 8)); bit--)
            {
              q->index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
              q->length=0;
              q++;
            }
            p++;
          }
        if ((((image->columns/8)+(image->columns % 8 ? 1 : 0)) % 2) != 0)
          p++;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,y,image->rows);
      }
    else
      if (image->class == PseudoClass)
        for (y=0; y < (int) image->rows; y++)
        {
          /*
            Convert PseudoColor scanline to runlength-encoded color packets.
          */
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if ((image->columns % 2) != 0)
            p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      else
        for (y=0; y < (int) image->rows; y++)
        {
          /*
            Convert DirectColor scanline to runlength-encoded color packets.
          */
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=0;
            if (image->matte)
              q->index=UpScale(*p++);
            if (sun_header.type == RT_STANDARD)
              {
                q->blue=UpScale(*p++);
                q->green=UpScale(*p++);
                q->red=UpScale(*p++);
              }
            else
              {
                q->red=UpScale(*p++);
                q->green=UpScale(*p++);
                q->blue=UpScale(*p++);
              }
            if (image->colors != 0)
              {
                q->red=image->colormap[q->red].red;
                q->green=image->colormap[q->green].green;
                q->blue=image->colormap[q->blue].blue;
              }
            q->length=0;
            q++;
          }
          if (((image->columns % 2) != 0) && (image->matte == False))
            p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
    FreeMemory((char *) sun_pixels);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    sun_header.magic=MSBFirstReadLong(image->file);
    if (sun_header.magic == 0x59a66a95)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (sun_header.magic == 0x59a66a95);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T G A I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTGAImage reads a Truevision TGA image file and returns it.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the ReadTGAImage routine is:
%
%      image=ReadTGAImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTGAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadTGAImage(const ImageInfo *image_info)
{
#define TGAColormap 1
#define TGARGB 2
#define TGAMonochrome 3
#define TGARLEColormap  9
#define TGARLERGB  10
#define TGARLEMonochrome  11

  typedef struct _TGAHeader
  {
    unsigned char
      id_length,
      colormap_type,
      image_type;

    unsigned short
      colormap_index,
      colormap_length;

    unsigned char
      colormap_size;

    unsigned short
      x_origin,
      y_origin,
      width,
      height;

    unsigned char
      bits_per_pixel,
      attributes;
  } TGAHeader;

  Image
    *image;

  Quantum
    blue,
    green,
    red;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  TGAHeader
    tga_header;

  unsigned char
    j,
    k,
    runlength;

  unsigned int
    base,
    flag,
    offset,
    real,
    skip,
    status;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read TGA header information.
  */
  status=ReadData((char *) &tga_header.id_length,1,1,image->file);
  tga_header.colormap_type=fgetc(image->file);
  tga_header.image_type=fgetc(image->file);
  do
  {
    if ((status == False) || (tga_header.image_type == 0) ||
        (tga_header.image_type > 11))
      PrematureExit(CorruptImageWarning,"Not a TGA image file",image);
    tga_header.colormap_index=LSBFirstReadShort(image->file);
    tga_header.colormap_length=LSBFirstReadShort(image->file);
    tga_header.colormap_size=fgetc(image->file);
    tga_header.x_origin=LSBFirstReadShort(image->file);
    tga_header.y_origin=LSBFirstReadShort(image->file);
    tga_header.width=LSBFirstReadShort(image->file);
    tga_header.height=LSBFirstReadShort(image->file);
    tga_header.bits_per_pixel=fgetc(image->file);
    tga_header.attributes=fgetc(image->file);
    /*
      Initialize image structure.
    */
    image->matte=tga_header.bits_per_pixel == 32;
    image->columns=tga_header.width;
    image->rows=tga_header.height;
    if (tga_header.colormap_type != 0)
      {
        if ((tga_header.image_type == TGARLEColormap) ||
            (tga_header.image_type == TGARLERGB))
          image->class=PseudoClass;
        image->colors=tga_header.colormap_length;
      }
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    if (tga_header.id_length != 0)
      {
        /*
          TGA image comment.
        */
        image->comments=(char *)
          AllocateMemory((tga_header.id_length+1)*sizeof(char));
        if (image->comments == (char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData(image->comments,1,tga_header.id_length,image->file);
        image->comments[tga_header.id_length]='\0';
      }
    red=0;
    green=0;
    blue=0;
    if (tga_header.colormap_type != 0)
      {
        /*
          Read TGA raster colormap.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (i=0; i < (int) image->colors; i++)
        {
          switch (tga_header.colormap_size)
          {
            case 8:
            default:
            {
              /*
                Gray scale.
              */
              red=UpScale(fgetc(image->file));
              green=red;
              blue=red;
              break;
            }
            case 15:
            case 16:
            {
              /*
                5 bits each of red green and blue.
              */
              j=fgetc(image->file);
              k=fgetc(image->file);
              red=(Quantum) ((MaxRGB*((int) (k & 0x7c) >> 2))/31);
              green=(Quantum)
                ((MaxRGB*(((int) (k & 0x03) << 3)+((int) (j & 0xe0) >> 5)))/31);
              blue=(Quantum) ((MaxRGB*((int) (j & 0x1f)))/31);
              break;
            }
            case 24:
            case 32:
            {
              /*
                8 bits each of blue, green and red.
              */
              blue=UpScale(fgetc(image->file));
              green=UpScale(fgetc(image->file));
              red=UpScale(fgetc(image->file));
              break;
            }
          }
          image->colormap[i].red=red;
          image->colormap[i].green=green;
          image->colormap[i].blue=blue;
        }
      }
    /*
      Convert TGA pixels to runlength-encoded packets.
    */
    base=0;
    flag=0;
    index=0;
    skip=False;
    real=0;
    runlength=0;
    offset=0;
    q=image->pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      q->red=0;
      q->green=0;
      q->blue=0;
      q->index=0;
      q->length=0;
      q++;
    }
    for (y=0; y < (int) image->rows; y++)
    {
      real=offset;
      if (((unsigned char) (tga_header.attributes & 0x20) >> 5) == 0)
        real=image->rows-real-1;
      q=image->pixels+(real*image->columns);
      for (x=0; x < (int) image->columns; x++)
      {
        if ((tga_header.image_type == TGARLEColormap) ||
            (tga_header.image_type == TGARLERGB) ||
            (tga_header.image_type == TGARLEMonochrome))
          {
            if (runlength != 0)
              {
                runlength--;
                skip=flag != 0;
              }
            else
              {
                status=ReadData((char *) &runlength,1,1,image->file);
                if (status == False)
                  PrematureExit(CorruptImageWarning,"Unable to read image data",
                    image);
                flag=runlength & 0x80;
                if (flag != 0)
                  runlength-=128;
                skip=False;
              }
          }
        if (!skip)
          switch (tga_header.bits_per_pixel)
          {
            case 8:
            default:
            {
              /*
                Gray scale.
              */
              index=fgetc(image->file);
              if (tga_header.colormap_type == 0)
                {
                  red=(Quantum) UpScale(index);
                  green=(Quantum) UpScale(index);
                  blue=(Quantum) UpScale(index);
                }
              else
                {
                  red=image->colormap[index].red;
                  green=image->colormap[index].green;
                  blue=image->colormap[index].blue;
                }
              break;
            }
            case 15:
            case 16:
            {
              /*
                5 bits each of red green and blue.
              */
              j=fgetc(image->file);
              k=fgetc(image->file);
              red=(Quantum) ((MaxRGB*((int) (k & 0x7c) >> 2))/31);
              green=(Quantum)
                ((MaxRGB*(((int) (k & 0x03) << 3)+((int) (j & 0xe0) >> 5)))/31);
              blue=(Quantum) ((MaxRGB*((int) (j & 0x1f)))/31);
              index=((unsigned short) k << 8)+j;
              break;
            }
            case 24:
            case 32:
            {
              /*
                8 bits each of blue green and red.
              */
              blue=UpScale(fgetc(image->file));
              green=UpScale(fgetc(image->file));
              red=UpScale(fgetc(image->file));
              if (tga_header.bits_per_pixel == 32)
                index=Opaque-UpScale(fgetc(image->file));
              break;
            }
          }
        if (status == False)
          PrematureExit(CorruptImageWarning,"Unable to read image data",image);
        q->red=red;
        q->green=green;
        q->blue=blue;
        q->index=index;
        q->length=0;
        q++;
      }
      if (((unsigned char) (tga_header.attributes & 0xc0) >> 6) == 4)
        offset+=4;
      else
        if (((unsigned char) (tga_header.attributes & 0xc0) >> 6) == 2)
          offset+=2;
        else
          offset++;
      if (offset >= image->rows)
        {
          base++;
          offset=base;
        }
      if (feof(image->file))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
    }
    (void) IsGrayImage(image);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    status=ReadData((char *) &tga_header.id_length,1,1,image->file);
    tga_header.colormap_type=fgetc(image->file);
    tga_header.image_type=fgetc(image->file);
    status&=((tga_header.image_type != 0) && (tga_header.image_type <= 11));
    if (status == True)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (status == True);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTIFFImage reads a Tagged image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTIFFImage routine is:
%
%      image=ReadTIFFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(ICC_SUPPORT)
static unsigned int TIFFColorProfileHandler(char *text,long int length,
  Image *image)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (image->color_profile.length != 0)
    {
      FreeMemory(image->color_profile.info);
      image->color_profile.length=0;
    }
  image->color_profile.info=(unsigned char *)
    AllocateMemory((unsigned int) length*sizeof(unsigned char));
  if (image->color_profile.info == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  image->color_profile.length=length;
  memcpy(image->color_profile.info,p,length);
  return(True);
}
#endif

#if defined(IPTC_SUPPORT)
static unsigned int TIFFNewsProfileHandler(char *text,long int length,
  Image *image,int type)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (image->iptc_profile.length != 0)
    {
      FreeMemory(image->iptc_profile.info);
      image->iptc_profile.length=0;
      image->iptc_profile.info=(char *) NULL;
    }
  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle TIFFTAG_RICHTIFFIPTC tag.
      */
      length*=4;
      image->iptc_profile.info=(unsigned char *)
        AllocateMemory((unsigned int) length*sizeof(unsigned char));
      if (image->iptc_profile.info == (unsigned char *) NULL)
        {
          MagickWarning(ResourceLimitWarning,"Memory allocation failed",
            (char *) NULL);
          return(False);
        }
      image->iptc_profile.length=length;
      memcpy(image->iptc_profile.info,p,length);
      return(True);
    }
  /*
    Handle TIFFTAG_PHOTOSHOP tag.
  */
  while (length > 0)
  {
    if ((p[0] == '8') && (p[1] == 'B') && (p[2] == 'I') && (p[3] == 'M') &&
        (p[4] == 4) && (p[5] == 4))
      break;
    length-=2;
    p+=2;
  }
  if (length <= 0)
    return(False);
  if (image->iptc_profile.length != 0)
    {
      FreeMemory(image->iptc_profile.info);
      image->iptc_profile.length=0;
    }
  /*
    Eat OSType, IPTC ID code, and Pascal string length bytes.
  */
  p+=6;
  length=(*p++);
  if (length)
    p+=length;
  if ((length & 1) == 0)
    p++;  /* align to an even byte boundary */
  length=(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
  image->iptc_profile.info=(unsigned char *)
    AllocateMemory((unsigned int) length*sizeof(unsigned char));
  if (image->iptc_profile.info == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  image->iptc_profile.length=length;
  memcpy(image->iptc_profile.info,p+4,length);
  return(True);
}
#endif

static void TIFFWarningMessage(const char *module,const char *format,
  va_list warning)
{
  char
    message[MaxTextExtent];

  register char
    *p;

  p=message;
  if (module != (char *) NULL)
    {
      FormatString(p,"%.1024s: ",module);
      p+=Extent(message);
    }
  (void) vsprintf(p,format,warning);
  (void) strcat(p,".");
  MagickWarning(DelegateWarning,message,(char *) NULL);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

Image *ReadTIFFImage(const ImageInfo *image_info)
{
  char
    *text;

  float
    *chromaticity,
    x_resolution,
    y_resolution;

  Image
    *image;

  int
    range,
    y;

  Quantum
    blue,
    green,
    red;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  TIFF
    *tiff;

  uint16
    compress_tag,
    extra_samples,
    *sample_info;

  uint32
    length;

  unsigned char
    *scanline;

  unsigned int
    height,
    method,
    status,
    width;

  unsigned long
    max_packets;

  unsigned short
    bits_per_sample,
    index,
    interlace,
    max_sample_value,
    min_sample_value,
    pages,
    photometric,
    samples_per_pixel,
    units,
    value;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdin) || image->pipe)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy standard input or pipe to temporary file.
      */
      TemporaryFilename((char *) image_info->filename);
      file=fopen(image_info->filename,WriteBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to write file",image);
      c=fgetc(image->file);
      while (c != EOF)
      {
        (void) putc(c,file);
        c=fgetc(image->file);
      }
      (void) fclose(file);
      (void) strcpy(image->filename,image_info->filename);
      image->temporary=True;
    }
  CloseImage(image);
  TIFFSetErrorHandler(TIFFWarningMessage);
  TIFFSetWarningHandler(TIFFWarningMessage);
  tiff=TIFFOpen(image->filename,ReadBinaryUnbufferedType);
  if (tiff == (TIFF *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      status=TIFFReadDirectory(tiff);
      if (status == False)
        PrematureExit(CorruptImageWarning,"Unable to read subimage",image);
    }
  do
  {
    if (image_info->verbose)
      TIFFPrintDirectory(tiff,stderr,False);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_COMPRESSION,&compress_tag);
    TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width);
    TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_PLANARCONFIG,&interlace);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_RESOLUTIONUNIT,&units);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_XRESOLUTION,&x_resolution);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_YRESOLUTION,&y_resolution);
    image->x_resolution=x_resolution;
    image->y_resolution=y_resolution;
    chromaticity=(float *) NULL;
    TIFFGetField(tiff,TIFFTAG_WHITEPOINT,&chromaticity);
    if (chromaticity != (float *) NULL)
      {
        image->chromaticity.white_point.x=chromaticity[0];
        image->chromaticity.white_point.y=chromaticity[1];
      }
    chromaticity=(float *) NULL;
    TIFFGetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,&chromaticity);
    if (chromaticity != (float *) NULL)
      {
        image->chromaticity.red_primary.x=chromaticity[0];
        image->chromaticity.red_primary.y=chromaticity[1];
        image->chromaticity.green_primary.x=chromaticity[2];
        image->chromaticity.green_primary.y=chromaticity[3];
        image->chromaticity.blue_primary.x=chromaticity[4];
        image->chromaticity.blue_primary.y=chromaticity[5];
      }
    length=0;
    text=(char *) NULL;
#if defined(ICC_SUPPORT)
    TIFFGetField(tiff,TIFFTAG_ICCPROFILE,&length,&text);
    TIFFColorProfileHandler(text,length,image);
#endif
#if defined(IPTC_SUPPORT)
    length=0;
    text=(char *) NULL;
    TIFFGetField(tiff,TIFFTAG_RICHTIFFIPTC,&length,&text);
    if (length > 0)
      {
        if (TIFFIsByteSwapped(tiff))
          TIFFSwabArrayOfLong((uint32 *) text,length);
        TIFFNewsProfileHandler(text,length,image,TIFFTAG_RICHTIFFIPTC);
      }
    else
      {
        TIFFGetField(tiff,TIFFTAG_PHOTOSHOP,&length,&text);
        if ( length > 0)
          TIFFNewsProfileHandler(text,length,image,TIFFTAG_PHOTOSHOP);
      }
#endif
    /*
      Allocate memory for the image and pixel buffer.
    */
    if (compress_tag == COMPRESSION_LZW)
      image->compression=LZWCompression;
    else
      if (compress_tag == COMPRESSION_JPEG)
        image->compression=JPEGCompression;
      else
        if (compress_tag == COMPRESSION_DEFLATE)
          image->compression=ZipCompression;
    image->columns=width;
    image->rows=height;
    range=max_sample_value-min_sample_value;
    if ((samples_per_pixel == 1) && !TIFFIsTiled(tiff))
      {
        image->class=PseudoClass;
        image->colors=1 << bits_per_sample;
        if (range <= (int) image->colors)
          image->colors=range+1;
        if (bits_per_sample > QuantumDepth)
          image->colors=MaxRGB+1;
      }
    if (image_info->ping)
      {
        TIFFClose(tiff);
        CloseImage(image);
        return(image);
      }
    if (units == RESUNIT_INCH)
      image->units=PixelsPerInchResolution;
    if (units == RESUNIT_CENTIMETER)
      image->units=PixelsPerCentimeterResolution;
    image->depth=bits_per_sample;
    if (bits_per_sample < 8)
      image->depth=8;
    packets=0;
    max_packets=Max((image->columns*image->rows+4) >> 3,1);
    if (bits_per_sample == 1)
      max_packets=Max((image->columns*image->rows+8) >> 4,1);
    image->pixels=(RunlengthPacket *)
      AllocateMemory(max_packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      {
        TIFFClose(tiff);
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    value=0;
    TIFFGetFieldDefaulted(tiff,TIFFTAG_PAGENUMBER,&value,&pages);
    image->scene=value;
    text=(char *) NULL;
    TIFFGetField(tiff,TIFFTAG_PAGENAME,&text);
    if (text != (char *) NULL)
      CloneString(&image->label,text);
    text=(char *) NULL;
    TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text);
    if (text != (char *) NULL)
      CloneString(&image->comments,text);
    if (range < 0)
      range=max_sample_value;
    q=image->pixels;
    SetRunlengthEncoder(q);
    method=0;
    if (samples_per_pixel > 1)
      {
        method=2;
        if ((samples_per_pixel >= 3) && (photometric == PHOTOMETRIC_RGB) &&
            (interlace == PLANARCONFIG_CONTIG))
          method=1;
      }
    if (TIFFIsTiled(tiff))
      method=2;
    switch (method)
    {
      case 0:
      {
        Quantum
          *quantum_scanline;

        register Quantum
          *r;

        /*
          Convert TIFF image to PseudoClass MIFF image.
        */
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        quantum_scanline=(Quantum *) AllocateMemory(width*sizeof(Quantum));
        scanline=(unsigned char *) AllocateMemory(TIFFScanlineSize(tiff)+4);
        if ((image->colormap == (ColorPacket *) NULL) ||
            (quantum_scanline == (Quantum *) NULL) ||
            (scanline == (unsigned char *) NULL))
          {
            TIFFClose(tiff);
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
        /*
          Create colormap.
        */
        switch (photometric)
        {
          case PHOTOMETRIC_MINISBLACK:
          {
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
            }
            break;
          }
          case PHOTOMETRIC_MINISWHITE:
          {
            unsigned int
              colors;

            colors=image->colors;
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[colors-i-1].red=(MaxRGB*i)/(image->colors-1);
              image->colormap[colors-i-1].green=(MaxRGB*i)/(image->colors-1);
              image->colormap[colors-i-1].blue=(MaxRGB*i)/(image->colors-1);
            }
            break;
          }
          case PHOTOMETRIC_PALETTE:
          {
            long
              range;

            unsigned short
              *blue_colormap,
              *green_colormap,
              *red_colormap;

            TIFFGetField(tiff,TIFFTAG_COLORMAP,&red_colormap,&green_colormap,
              &blue_colormap);
            range=256L;  /* might be old style 8-bit colormap */
            for (i=0; i < (int) image->colors; i++)
              if ((red_colormap[i] >= 256) || (green_colormap[i] >= 256) ||
                  (blue_colormap[i] >= 256))
                {
                  range=65535L;
                  break;
                }
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=(Quantum)
                ((long) (MaxRGB*red_colormap[i])/range);
              image->colormap[i].green=(Quantum)
                ((long) (MaxRGB*green_colormap[i])/range);
              image->colormap[i].blue=(Quantum)
                ((long) (MaxRGB*blue_colormap[i])/range);
            }
            break;
          }
          default:
            break;
        }
        /*
          Convert image to PseudoClass runlength-encoded packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          TIFFReadScanline(tiff,(char *) scanline,y,0);
          p=scanline;
          r=quantum_scanline;
          switch (bits_per_sample)
          {
            case 1:
            {
              register int
                bit;

              for (x=0; x < ((int) width-7); x+=8)
              {
                for (bit=7; bit >= 0; bit--)
                  *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                p++;
              }
              if ((width % 8) != 0)
                {
                  for (bit=7; bit >= (int) (8-(width % 8)); bit--)
                    *r++=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                  p++;
                }
              break;
            }
            case 2:
            {
              for (x=0; x < ((int) width-3); x+=4)
              {
                *r++=(*p >> 6) & 0x3;
                *r++=(*p >> 4) & 0x3;
                *r++=(*p >> 2) & 0x3;
                *r++=(*p) & 0x3;
                p++;
              }
              if ((width % 4) != 0)
                {
                  for (i=3; i >= (int) (4-(width % 4)); i--)
                    *r++=(*p >> (i*2)) & 0x03;
                  p++;
                }
              break;
            }
            case 4:
            {
              for (x=0; x < ((int) width-1); x+=2)
              {
                *r++=(*p >> 4) & 0xf;
                *r++=(*p) & 0xf;
                p++;
              }
              if ((width % 2) != 0)
                *r++=(*p++ >> 4) & 0xf;
              break;
            }
            case 8:
            {
              for (x=0; x < width; x++)
                *r++=(*p++);
              break;
            }
            case 16:
            {
              for (x=0; x < (int) image->columns; x++)
              {
                ReadQuantum(*r,p);
                r++;
              }
              break;
            }
            default:
              break;
          }
          /*
            Transfer image scanline.
          */
          r=quantum_scanline;
          for (x=0; x < (int) image->columns; x++)
          {
            index=(*r++);
            if ((index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        FreeMemory((char *) scanline);
                        FreeMemory((char *) quantum_scanline);
                        TIFFClose(tiff);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        FreeMemory((char *) scanline);
        FreeMemory((char *) quantum_scanline);
        break;
      }
      case 1:
      {
        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        scanline=(unsigned char *)
          AllocateMemory((TIFFScanlineSize(tiff) << 1)+4);
        if (scanline == (unsigned char *) NULL)
          {
            TIFFClose(tiff);
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
        TIFFGetFieldDefaulted(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
          &sample_info);
        image->matte=extra_samples == 1;
        for (y=0; y < (int) image->rows; y++)
        {
          TIFFReadScanline(tiff,(char *) scanline,y,0);
          if (bits_per_sample == 4)
            {
              register unsigned char
                *r;

              width=TIFFScanlineSize(tiff);
              p=scanline+width-1;
              r=scanline+(width << 1)-1;
              for (x=0; x < (int) width; x++)
              {
                *r--=((*p) & 0xf) << 4;
                *r--=((*p >> 4) & 0xf) << 4;
                p--;
              }
            }
          p=scanline;
          for (x=0; x < (int) image->columns; x++)
          {
            ReadQuantum(red,p);
            ReadQuantum(green,p);
            ReadQuantum(blue,p);
            index=0;
            if (samples_per_pixel == 4)
              ReadQuantum(index,p);
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                (index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        TIFFClose(tiff);
                        FreeMemory((char *) scanline);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=index;
                q->length=0;
              }
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        FreeMemory((char *) scanline);
        break;
      }
      case 2:
      default:
      {
        register uint32
          *p,
          *pixels;

        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        TIFFGetFieldDefaulted(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
          &sample_info);
        image->matte=
          ((extra_samples == 1) && (sample_info[0] == EXTRASAMPLE_ASSOCALPHA));
        pixels=(uint32 *) AllocateMemory((image->columns*image->rows+
          image->columns)*sizeof(uint32));
        if (pixels == (uint32 *) NULL)
          {
            TIFFClose(tiff);
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          }
        status=TIFFReadRGBAImage(tiff,image->columns,image->rows,pixels,0);
        if (status == False)
          {
            FreeMemory((char *) pixels);
            TIFFClose(tiff);
            PrematureExit(CorruptImageWarning,"Unable to read image",image);
          }
        /*
          Convert image to DirectClass runlength-encoded packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=pixels+y*image->columns;
          for (x=0; x < (int) image->columns; x++)
          {
            red=UpScale(TIFFGetR(*p));
            green=UpScale(TIFFGetG(*p));
            blue=UpScale(TIFFGetB(*p));
            index=image->matte ? UpScale(TIFFGetA(*p)) : 0;
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                (index == q->index) && ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      {
                        FreeMemory((char *) pixels);
                        TIFFClose(tiff);
                        PrematureExit(ResourceLimitWarning,
                          "Memory allocation failed",image);
                      }
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=index;
                q->length=0;
              }
            p++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        FreeMemory((char *) pixels);
        break;
      }
    }
    SetRunlengthPackets(image,packets);
    if (image->class == PseudoClass)
      SyncImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    status=TIFFReadDirectory(tiff);
    if (status == True)
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
        ProgressMonitor(LoadImageText,image->scene-1,image->scene);
      }
  } while (status == True);
  TIFFClose(tiff);
  if (image->temporary)
    {
      (void) remove(image->filename);
      image->temporary=False;
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  return(image);
}
#else
Image *ReadTIFFImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"TIFF library is not available",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I L E I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTILEImage tiles a texture on an image.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadTILEImage routine is:
%
%      image=ReadTILEImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTILEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadTILEImage(const ImageInfo *image_info)
{
  Image
    *cloned_image,
    *image,
    *tiled_image;

  ImageInfo
    *local_info;

  int
    y;

  register int
    x;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  if (*image_info->filename == '\0')
    PrematureExit(OptionWarning,"must specify an image name",image);
  /*
    Initialize Image structure.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  *local_info->magick='\0';
  tiled_image=ReadImage(local_info);
  DestroyImageInfo(local_info);
  if (tiled_image == (Image *) NULL)
    return((Image *) NULL);
  tiled_image->orphan=True;
  cloned_image=CloneImage(tiled_image,image->columns,image->rows,False);
  tiled_image->orphan=False;
  DestroyImage(image);
  if (cloned_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",tiled_image);
  image=cloned_image;
  (void) strcpy(image->filename,image_info->filename);
  /*
    Tile texture onto image.
  */
  for (y=0; y < (int) image->rows; y+=tiled_image->rows)
  {
    for (x=0; x < (int) image->columns; x+=tiled_image->columns)
      CompositeImage(image,ReplaceCompositeOp,tiled_image,x,y);
    ProgressMonitor(LoadImageText,y,image->rows);
  }
  DestroyImage(tiled_image);
  CondenseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d T I M I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTIMImage reads a PSX TIM image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  Contributed by os@scee.sony.co.uk.
%
%  The format of the ReadTIMImage routine is:
%
%      image=ReadTIMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTIMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadTIMImage(const ImageInfo *image_info)
{
#define ScaleColor5to8(x)  ((x) << 3)

  typedef struct _TIMHeader
  {
    unsigned long
      id,
      flag;
  } TIMHeader;

  TIMHeader
    tim_header;

  Image
    *image;

  register int
    i,
    x,
    y;

  int
    bits_per_pixel,
    has_clut,
    pixel_mode;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *tim_data,
    *tim_pixels;

  unsigned short
    word;

  unsigned int
    bytes_per_line,
    height,
    image_size,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a TIM file.
  */
  tim_header.id=LSBFirstReadLong(image->file);
  do
  {
    /*
      Verify TIM identifier.
    */
    if (tim_header.id != 0x00000010)
      PrematureExit(CorruptImageWarning,"Not a TIM image file",image);
    tim_header.flag=LSBFirstReadLong(image->file);
    has_clut=!!(tim_header.flag & (1 << 3));
    pixel_mode=tim_header.flag & 0x07;
    switch (pixel_mode)
    {
      case 0: bits_per_pixel=4; break;
      case 1: bits_per_pixel=8; break;
      case 2: bits_per_pixel=16; break;
      case 3: bits_per_pixel=24; break;
      default: bits_per_pixel=4; break;
    }
    if (has_clut)
      {
        unsigned char
          *tim_colormap;

        /*
          Read TIM raster colormap.
        */
        (void)LSBFirstReadLong(image->file);
        (void)LSBFirstReadShort(image->file);
        (void)LSBFirstReadShort(image->file);
        width=LSBFirstReadShort(image->file);
        height=LSBFirstReadShort(image->file);
        image->class=PseudoClass;
        image->colors=(unsigned int) pixel_mode == 1 ? 256 : 16;
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        tim_colormap=(unsigned char *)
          malloc(image->colors*2*sizeof(unsigned char));
        if ((image->colormap == (ColorPacket *) NULL) ||
            (tim_colormap == (unsigned char *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) tim_colormap,2,image->colors,image->file);
        p=tim_colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          word=(*p++);
          word=word | (*p++ << 8);
          image->colormap[i].blue=UpScale(ScaleColor5to8((word >> 10) & 0x1f));
          image->colormap[i].green=UpScale(ScaleColor5to8((word >> 5) & 0x1f));
          image->colormap[i].red=UpScale(ScaleColor5to8(word & 0x1f));
        }
        free((char *) tim_colormap);
      }
    /*
      Read image data.
    */
    (void) LSBFirstReadLong(image->file);
    (void) LSBFirstReadShort(image->file);
    (void) LSBFirstReadShort(image->file);
    width=LSBFirstReadShort(image->file);
    height=LSBFirstReadShort(image->file);
    image_size=2*width*height;
    bytes_per_line=width*2;
    width=(width*16)/bits_per_pixel;
    tim_data=(unsigned char *) malloc(image_size*sizeof(unsigned char));
    if (tim_data == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Unable to allocate memory",image);
    (void) ReadData((char *) tim_data,1,(unsigned int) image_size,image->file);
    tim_pixels=tim_data;
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      malloc(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Unable to allocate memory",image);
    /*
      Convert TIM raster image to runlength-encoded packets.
    */
    switch (bits_per_pixel)
    {
      case 4:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < ((int) image->columns-1); x+=2)
          {
            q->index=(*p) & 0xf;
            q->length=0;
            q++;
            q->index=(*p >> 4) & 0xf;
            q->length=0;
            p++;
            q++;
          }
          if ((image->columns % 2) != 0)
            {
              q->index=(*p >> 4) & 0xf;
              q->length=0;
              q++;
              p++;
            }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 16:
      {
        /*
          Convert DirectColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=0;
            word=(*p++);
            word=word | (*p++ << 8);
            q->blue=UpScale(ScaleColor5to8((word >> 10) & 0x1f));
            q->green=UpScale(ScaleColor5to8((word >> 5) & 0x1f));
            q->red=UpScale(ScaleColor5to8(word & 0x1f));
            q->length=0;
            q++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 24:
      {
        /*
          Convert DirectColor scanline to runlength-encoded color packets.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          p=tim_pixels+y*bytes_per_line;
          q=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=0;
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            q->length=0;
            q++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Not a TIM image file",image);
    }
    free((char *) tim_pixels);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    tim_header.id=LSBFirstReadLong(image->file);
    if (tim_header.id == 0x00000010)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (tim_header.id == 0x00000010);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

#if defined(HasTTF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T T F I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTTFImage reads a TrueType font file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTTFImage routine is:
%
%      image=ReadTTFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTTFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadTTFImage(const ImageInfo *image_info)
{
  AnnotateInfo
    annotate_info;

  char
    font[MaxTextExtent],
    geometry[MaxTextExtent],
    text[MaxTextExtent];

  Image
    *image;

  int
    y;

  long
    magick;

  register int
    i;

  ImageInfo
    *local_info;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  magick=MSBFirstReadLong(image->file);
  if ((magick != 256) && (magick != 65536))
    PrematureExit(CorruptImageWarning,"Not a TTF font file",image);
  /*
    Start with a white canvas.
  */
  y=0;
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  CloneString(&local_info->size,"800x520");
  CloneString(&local_info->pen,"black");
  CloneString(&local_info->font,font);
  local_info->pointsize=18;
  FormatString(local_info->font,"@%.1024s",image_info->filename);
  GetAnnotateInfo(local_info,&annotate_info);
  image->columns=annotate_info.bounds.width;
  image->rows=annotate_info.bounds.height;
  if (image_info->ping)
    {
      DestroyAnnotateInfo(&annotate_info);
      DestroyImageInfo(local_info);
      CloseImage(image);
      return(image);
    }
  DestroyImage(image);
  (void) strcpy(local_info->filename,"white");
  image=ReadXCImage(local_info);
  DestroyImageInfo(local_info);
  if (image == (Image *) NULL)
    {
      DestroyAnnotateInfo(&annotate_info);
      return((Image *) NULL);
    }
  (void) strcpy(image->filename,image_info->filename);
  if (annotate_info.font_name != (char *) NULL)
    CloneString(&image->label,annotate_info.font_name);
  /*
    Annotate canvas with text rendered with font at different point sizes.
  */
  y=10;
  if (annotate_info.font_name != (char *) NULL)
    {
      annotate_info.pointsize=30;
      FormatString(geometry,"+10%+d",y);
      CloneString(&annotate_info.geometry,geometry);
      CloneString(&annotate_info.text,annotate_info.font_name);
      AnnotateImage(image,&annotate_info);
      y+=42;
    }
  annotate_info.pointsize=18;
  FormatString(geometry,"+10%+d",y);
  CloneString(&annotate_info.geometry,geometry);
  CloneString(&annotate_info.text,"abcdefghijklmnopqrstuvwxyz");
  AnnotateImage(image,&annotate_info);
  y+=20;
  FormatString(geometry,"+10%+d",y);
  CloneString(&annotate_info.geometry,geometry);
  CloneString(&annotate_info.text,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  AnnotateImage(image,&annotate_info);
  y+=20;
  FormatString(geometry,"+10%+d",y);
  CloneString(&annotate_info.geometry,geometry);
  CloneString(&annotate_info.text,"1234567890.:,;(:*!?')");
  AnnotateImage(image,&annotate_info);
  y+=20;
  for (i=12; i <= 72; i+=6)
  {
    y+=i+6;
    annotate_info.pointsize=18;
    FormatString(geometry,"+10%+d",y);
    CloneString(&annotate_info.geometry,geometry);
    FormatString(text,"%d",i);
    CloneString(&annotate_info.text,text);
    AnnotateImage(image,&annotate_info);
    annotate_info.pointsize=i;
    FormatString(geometry,"+40%+d",y);
    CloneString(&annotate_info.geometry,geometry);
    CloneString(&annotate_info.text,
      "That which does not kill us, makes us stronger");
    AnnotateImage(image,&annotate_info);
    if (i >= 24)
      i+=6;
  }
  CondenseImage(image);
  DestroyAnnotateInfo(&annotate_info);
  return(image);
}
#else
Image *ReadTTFImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"Cannot read TTF images",
    image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T X T I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTXTImage reads a text file and returns it as an image.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTXTImage routine is:
%
%      image=ReadTXTImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTXTImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadTXTImage(const ImageInfo *image_info)
{
  AnnotateInfo
    annotate_info;

  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *status,
    text[MaxTextExtent];

  double
    dx_resolution,
    dy_resolution;

  Image
    *image,
    *texture;

  int
    count,
    offset;

  RectangleInfo
    bounding_box;

  register int
    i;

  register RunlengthPacket
    *q;

  XColor
    color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,"r");
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Set the page geometry.
  */
  dx_resolution=72.0;
  dy_resolution=72.0;
  if ((image->x_resolution == 0.0) || (image->y_resolution == 0.0))
    {
      char
        density[MaxTextExtent];

      (void) strcpy(density,PSDensityGeometry);
      count=sscanf(density,"%lfx%lf",&image->x_resolution,&image->y_resolution);
      if (count != 2)
        image->y_resolution=image->x_resolution;
    }
  bounding_box.width=612;
  bounding_box.height=792;
  bounding_box.x=0;
  bounding_box.y=0;
  (void) ParseImageGeometry(TextPageGeometry,&bounding_box.x,&bounding_box.y,
    &bounding_box.width,&bounding_box.height);
  if (image_info->page != (char *) NULL)
    (void) ParseImageGeometry(image_info->page,&bounding_box.x,&bounding_box.y,
      &bounding_box.width,&bounding_box.height);
  /*
    Initialize Image structure.
  */
  image->columns=(unsigned int)
    (((bounding_box.width*image->x_resolution)/dx_resolution)+0.5);
  image->rows=(unsigned int)
    (((bounding_box.height*image->y_resolution)/dy_resolution)+0.5);
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  (void) XQueryColorDatabase(DefaultTextBackground,&color);
  image->background_color.red=XDownScale(color.red);
  image->background_color.green=XDownScale(color.green);
  image->background_color.blue=XDownScale(color.blue);
  SetImage(image);
  texture=(Image *) NULL;
  if (image_info->texture != (char *) NULL)
    {
      ImageInfo
        *local_info;

      local_info=CloneImageInfo(image_info);
      if (local_info == (ImageInfo *) NULL)
        return((Image *) NULL);
      (void) strcpy(local_info->filename,image_info->texture);
      texture=ReadImage(local_info);
      if (texture != (Image *) NULL)
        TextureImage(image,texture);
      DestroyImageInfo(local_info);
    }
  /*
    Annotate the text image.
  */
  GetAnnotateInfo((ImageInfo *) image_info,&annotate_info);
  (void) strcpy(filename,image_info->filename);
  offset=0;
  for ( ; ; )
  {
    /*
      Annotate image with text.
    */
    status=fgets(text,MaxTextExtent,image->file);
    if (status == (char *) NULL)
      break;
    if (Extent(text) > 0)
      text[Extent(text)-1]='\0';
    CloneString(&annotate_info.text,text);
    FormatString(geometry,"%+d%+d",bounding_box.x,bounding_box.y+offset);
    CloneString(&annotate_info.geometry,geometry);
    AnnotateImage(image,&annotate_info);
    offset+=annotate_info.bounds.height;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(bounding_box.y+offset,image->rows))
        ProgressMonitor(LoadImageText,bounding_box.y+offset,image->rows);
    if (((2*bounding_box.y)+offset+annotate_info.bounds.height) < image->rows)
      continue;
    /*
      Page is full-- allocate next image structure.
    */
    image->orphan=True;
    image->next=CloneImage(image,image->columns,image->rows,False);
    image->orphan=False;
    if (image->next == (Image *) NULL)
      {
        DestroyAnnotateInfo(&annotate_info);
        MagickWarning(ResourceLimitWarning,"Unable to annotate image",
          "Memory allocation failed");
        break;
      }
    (void) strcpy(image->next->filename,filename);
    image->next->file=image->file;
    image->next->filesize=image->filesize;
    image->next->scene=image->scene+1;
    image->next->previous=image;
    (void) IsPseudoClass(image);
    image=image->next;
    ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
      (unsigned int) image->filesize);
    /*
      Initialize text image to background color.
    */
    q=image->pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      q->red=XDownScale(color.red);
      q->green=XDownScale(color.green);
      q->blue=XDownScale(color.blue);
      q->index=0;
      q->length=0;
      q++;
    }
    if (texture != (Image *) NULL)
      {
        MonitorHandler
          handler;

        handler=SetMonitorHandler((MonitorHandler) NULL);
        TextureImage(image,texture);
        (void) SetMonitorHandler(handler);
      }
    offset=0;
  }
  if (texture != (Image *) NULL)
    DestroyImage(texture);
  DestroyAnnotateInfo(&annotate_info);
  (void) IsPseudoClass(image);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d U I L I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadUILImage reads a X-Motif UIL table and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadUILImage routine is:
%
%      image=ReadUILImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadUILImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadUILImage(const ImageInfo *image_info)
{
  MagickWarning(MissingDelegateWarning,"Cannot read UIL images",
    image_info->filename);
  return((Image *) NULL);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d U Y V Y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadUYVYImage reads an image in the UYVY (16bit/pixel) format
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadUYVYImage routine is:
%
%      image=ReadUYVYImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadUYVYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadUYVYImage(const ImageInfo *image_info)
{
  Image
    *image;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *uyvy_pixels;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,image_info->filename);
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image)
  for (i=0; i < image->offset; i++)
    (void) fgetc(image->file);
  /*
    Read data.
  */
  image->packets=image->columns*image->rows;
  uyvy_pixels=(unsigned char *)
    AllocateMemory((2*image->packets)*sizeof(unsigned char));
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if ((uyvy_pixels == (unsigned char *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  SetImage(image);
  (void) ReadData((char *) uyvy_pixels,1,2*image->packets,image->file);
  /*
    Accumulate UYVY, then unpack into two pixels.
  */
  p=uyvy_pixels;
  q=image->pixels;
  for (i=0; i < (int) (image->packets >> 1); i++)
  {
    q->red=UpScale(p[1]);
    q->green=UpScale(p[0]);
    q->blue=UpScale(p[2]);
    q->index=0;
    q->length=0;
    q++;
    q->red=UpScale(p[3]);
    q->green=UpScale(p[0]);
    q->blue=UpScale(p[2]);
    q->index=0;
    q->length=0;
    q++;
    p+=4;
    if (QuantumTick(i,image->columns*image->rows >> 1))
      ProgressMonitor(LoadImageText,i,image->columns*image->rows >> 1);
  }
  FreeMemory((char *) uyvy_pixels);
  TransformRGBImage(image,YCbCrColorspace);
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I C A R I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVICARImage reads a VICAR image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadVICARImage routine is:
%
%      image=ReadVICARImage(image_info)
%
%  A description of each parameter follows:
%
%    o image: Method ReadVICARImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
Image *ReadVICARImage(const ImageInfo *image_info)
{
  char
    keyword[MaxTextExtent],
    value[MaxTextExtent];

  Image
    *image;

  int
    c,
    y;

  long
    count;

  register int
    i,
    x;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *vicar_pixels;

  unsigned int
    header_length,
    status,
    value_expected;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Decode image header.
  */
  c=fgetc(image->file);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  header_length=0;
  while (isgraph(c) && ((image->columns*image->rows) == 0))
  {
    if (!isalnum(c))
      {
        c=fgetc(image->file);
        count++;
      }
    else
      {
        register char
          *p;

        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxTextExtent-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=fgetc(image->file);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c))
        {
          if ((p-value) < (MaxTextExtent-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (Latin1Compare(keyword,"LABEL_RECORDS") == 0)
          header_length=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"LBLSIZE") == 0)
          header_length=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"RECORD_BYTES") == 0)
          image->columns=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NS") == 0)
          image->columns=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"LINES") == 0)
          image->rows=(unsigned int) atoi(value);
        if (Latin1Compare(keyword,"NL") == 0)
          image->rows=(unsigned int) atoi(value);
      }
    while (isspace(c))
    {
      c=fgetc(image->file);
      count++;
    }
  }
  image->class=PseudoClass;
  image->colors=256;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Read the rest of the header.
  */
  while (count < (int) header_length)
  {
    c=fgetc(image->file);
    count++;
  }
  /*
    Verify that required image information is defined.
  */
  if ((image->columns*image->rows) == 0)
    PrematureExit(CorruptImageWarning,"image size is zero",image);
  /*
    Create linear colormap.
  */
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < (int) image->colors; i++)
  {
    image->colormap[i].red=(Quantum) UpScale(i);
    image->colormap[i].green=(Quantum) UpScale(i);
    image->colormap[i].blue=(Quantum) UpScale(i);
  }
  /*
    Initialize image structure.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  vicar_pixels=(unsigned char *)
    AllocateMemory(image->packets*sizeof(unsigned char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (vicar_pixels == (unsigned char *) NULL))
    PrematureExit(CorruptImageWarning,"Unable to read image data",image);
  SetImage(image);
  /*
    Convert VICAR pixels to runlength-encoded packets.
  */
  status=ReadData((char *) vicar_pixels,1,image->packets,image->file);
  if (status == False)
    PrematureExit(CorruptImageWarning,"Insufficient image data in file",image);
  /*
    Convert VICAR pixels to runlength-encoded packets.
  */
  p=vicar_pixels;
  q=image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    for (x=0; x < (int) image->columns; x++)
    {
      q->index=(unsigned short) *p;
      q->length=0;
      p++;
      q++;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  FreeMemory((char *) vicar_pixels);
  SyncImage(image);
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVIDImage reads one of more images and creates a Visual Image
%  Directory file.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadVIDImage routine is:
%
%      image=ReadVIDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadVIDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadVIDImage(const ImageInfo *image_info)
{
#define ClientName  "montage"

  char
    *commands[5],
    **filelist,
    **list;

  Image
    *image,
    *montage_image,
    *next_image;

  ImageInfo
    *local_info;

  int
    number_files;

  MonitorHandler
    handler;

  MontageInfo
    montage_info;

  register int
    i;

  /*
    Expand the filename.
  */
  list=(char **) AllocateMemory(sizeof(char *));
  if (list == (char **) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return((Image *) NULL);
    }
  list[0]=(char *) AllocateMemory(Extent(image_info->filename)+1);
  if (list[0] == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return((Image *) NULL);
    }
  (void) strcpy(list[0],image_info->filename);
  number_files=1;
  filelist=list;
  (void) ExpandFilenames(&number_files,&filelist);
  if (number_files == 0)
    {
      MagickWarning(CorruptImageWarning,"unable to read VID image",
        image_info->filename);
      return((Image *) NULL);
    }
  /*
    Read each image and convert them to a tile.
  */
  image=(Image *) NULL;
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    return((Image *) NULL);
  CloneString(&local_info->size,DefaultTileGeometry);
  commands[0]=SetClientName((char *) NULL);
  commands[1]="-label";
  commands[2]=DefaultTileLabel;
  commands[3]="-geometry";
  commands[4]=DefaultTileGeometry;
  for (i=0; i < number_files; i++)
  {
    handler=SetMonitorHandler((MonitorHandler) NULL);
    (void) strcpy(local_info->filename,filelist[i]);
    *local_info->magick='\0';
    next_image=ReadImage(local_info);
    FreeMemory((char *) filelist[i]);
    if (next_image != (Image *) NULL)
      {
        MogrifyImages(local_info,5,commands,&next_image);
        if (image == (Image *) NULL)
          image=next_image;
        else
          {
            image->next=next_image;
            image->next->previous=image;
            image=image->next;
          }
      }
    (void) SetMonitorHandler(handler);
    ProgressMonitor(LoadImageText,i,number_files);
  }
  DestroyImageInfo(local_info);
  FreeMemory((char *) filelist);
  if (image == (Image *) NULL)
    {
      MagickWarning(CorruptImageWarning,"unable to read VID image",
        image_info->filename);
      return((Image *) NULL);
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  /*
    Create the visual image directory.
  */
  GetMontageInfo(&montage_info);
  (void) strcpy(montage_info.filename,image_info->filename);
  CloneString(&montage_info.font,image_info->font);
  montage_info.pointsize=image_info->pointsize;
  montage_image=MontageImages(image,&montage_info);
  DestroyMontageInfo(&montage_info);
  if (montage_image == (Image *) NULL)
    {
      MagickWarning(CorruptImageWarning,"unable to read VID image",
        image_info->filename);
      return((Image *) NULL);
    }
  DestroyImages(image);
  FreeMemory((char *) list[0]);
  FreeMemory((char *) list);
  return(montage_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadVIFFImage reads a Khoros Visualization image file and returns
%  it.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the ReadVIFFImage routine is:
%
%      image=ReadVIFFImage(image_info)
%
%  A description of each parameter follows:
%
%    o image: Method ReadVIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
Image *ReadVIFFImage(const ImageInfo *image_info)
{
#define VFF_CM_genericRGB  15
#define VFF_CM_ntscRGB  1
#define VFF_CM_NONE  0
#define VFF_DEP_DECORDER  0x4
#define VFF_DEP_NSORDER  0x8
#define VFF_DES_RAW  0
#define VFF_LOC_IMPLICIT  1
#define VFF_MAPTYP_NONE  0
#define VFF_MAPTYP_1_BYTE  1
#define VFF_MS_NONE  0
#define VFF_MS_ONEPERBAND  1
#define VFF_MS_SHARED  3
#define VFF_TYP_BIT  0
#define VFF_TYP_1_BYTE  1
#define VFF_TYP_2_BYTE  2
#define VFF_TYP_4_BYTE  4

  typedef struct _ViffHeader
  {
    unsigned char
      identifier,
      file_type,
      release,
      version,
      machine_dependency,
      reserve[3];

    char
      comment[512];

    unsigned long
      rows,
      columns,
      subrows;

    long
      x_offset,
      y_offset;

    float
      x_bits_per_pixel,
      y_bits_per_pixel;

    unsigned long
      location_type,
      location_dimension,
      number_of_images,
      number_data_bands,
      data_storage_type,
      data_encode_scheme,
      map_scheme,
      map_storage_type,
      map_rows,
      map_columns,
      map_subrows,
      map_enable,
      maps_per_cycle,
      color_space_model;
  } ViffHeader;

  Image
    *image;

  register int
    bit,
    i,
    x,
    y;

  register Quantum
    *p;

  register RunlengthPacket
    *q;

  unsigned char
    buffer[7],
    *viff_pixels;

  unsigned int
    bytes_per_pixel,
    status;

  unsigned long
    max_packets;

  ViffHeader
    viff_header;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read VIFF header (1024 bytes).
  */
  status=ReadData((char *) &viff_header.identifier,1,1,image->file);
  do
  {
    /*
      Verify VIFF identifier.
    */
    if ((status == False) || ((unsigned char) viff_header.identifier != 0xab))
      PrematureExit(CorruptImageWarning,"Not a VIFF raster",image);
    /*
      Initialize VIFF image.
    */
    (void) ReadData((char *) buffer,1,7,image->file);
    viff_header.file_type=buffer[0];
    viff_header.release=buffer[1];
    viff_header.version=buffer[2];
    viff_header.machine_dependency=buffer[3];
    (void) ReadData((char *) viff_header.comment,1,512,image->file);
    viff_header.comment[511]='\0';
    if (Extent(viff_header.comment) > 4)
      CloneString(&image->comments,viff_header.comment);
    if ((viff_header.machine_dependency == VFF_DEP_DECORDER) ||
        (viff_header.machine_dependency == VFF_DEP_NSORDER))
      {
        viff_header.rows=LSBFirstReadLong(image->file);
        viff_header.columns=LSBFirstReadLong(image->file);
        viff_header.subrows=LSBFirstReadLong(image->file);
        viff_header.x_offset=(long) LSBFirstReadLong(image->file);
        viff_header.y_offset=(long) LSBFirstReadLong(image->file);
        viff_header.x_bits_per_pixel=(float) LSBFirstReadLong(image->file);
        viff_header.y_bits_per_pixel=(float) LSBFirstReadLong(image->file);
        viff_header.location_type=LSBFirstReadLong(image->file);
        viff_header.location_dimension=LSBFirstReadLong(image->file);
        viff_header.number_of_images=LSBFirstReadLong(image->file);
        viff_header.number_data_bands=LSBFirstReadLong(image->file);
        viff_header.data_storage_type=LSBFirstReadLong(image->file);
        viff_header.data_encode_scheme=LSBFirstReadLong(image->file);
        viff_header.map_scheme=LSBFirstReadLong(image->file);
        viff_header.map_storage_type=LSBFirstReadLong(image->file);
        viff_header.map_rows=LSBFirstReadLong(image->file);
        viff_header.map_columns=LSBFirstReadLong(image->file);
        viff_header.map_subrows=LSBFirstReadLong(image->file);
        viff_header.map_enable=LSBFirstReadLong(image->file);
        viff_header.maps_per_cycle=LSBFirstReadLong(image->file);
        viff_header.color_space_model=LSBFirstReadLong(image->file);
      }
    else
      {
        viff_header.rows=MSBFirstReadLong(image->file);
        viff_header.columns=MSBFirstReadLong(image->file);
        viff_header.subrows=MSBFirstReadLong(image->file);
        viff_header.x_offset=(long) MSBFirstReadLong(image->file);
        viff_header.y_offset=(long) MSBFirstReadLong(image->file);
        viff_header.x_bits_per_pixel=(float) MSBFirstReadLong(image->file);
        viff_header.y_bits_per_pixel=(float) MSBFirstReadLong(image->file);
        viff_header.location_type=MSBFirstReadLong(image->file);
        viff_header.location_dimension=MSBFirstReadLong(image->file);
        viff_header.number_of_images=MSBFirstReadLong(image->file);
        viff_header.number_data_bands=MSBFirstReadLong(image->file);
        viff_header.data_storage_type=MSBFirstReadLong(image->file);
        viff_header.data_encode_scheme=MSBFirstReadLong(image->file);
        viff_header.map_scheme=MSBFirstReadLong(image->file);
        viff_header.map_storage_type=MSBFirstReadLong(image->file);
        viff_header.map_rows=MSBFirstReadLong(image->file);
        viff_header.map_columns=MSBFirstReadLong(image->file);
        viff_header.map_subrows=MSBFirstReadLong(image->file);
        viff_header.map_enable=MSBFirstReadLong(image->file);
        viff_header.maps_per_cycle=MSBFirstReadLong(image->file);
        viff_header.color_space_model=MSBFirstReadLong(image->file);
      }
    for (i=0; i < 420; i++)
      (void) fgetc(image->file);
    image->columns=(unsigned int) viff_header.rows;
    image->rows=(unsigned int) viff_header.columns;
    /*
      Verify that we can read this VIFF image.
    */
    if ((viff_header.columns*viff_header.rows) == 0)
      PrematureExit(CorruptImageWarning,
        "Image column or row size is not supported",image);
    if ((viff_header.data_storage_type != VFF_TYP_BIT) &&
        (viff_header.data_storage_type != VFF_TYP_1_BYTE) &&
        (viff_header.data_storage_type != VFF_TYP_2_BYTE) &&
        (viff_header.data_storage_type != VFF_TYP_4_BYTE))
      PrematureExit(CorruptImageWarning,
        "Data storage type is not supported",image);
    if (viff_header.data_encode_scheme != VFF_DES_RAW)
      PrematureExit(CorruptImageWarning,
        "Data encoding scheme is not supported",image);
    if ((viff_header.map_storage_type != VFF_MAPTYP_NONE) &&
        (viff_header.map_storage_type != VFF_MAPTYP_1_BYTE))
      PrematureExit(CorruptImageWarning,
        "Map storage type is not supported",image);
    if ((viff_header.color_space_model != VFF_CM_NONE) &&
        (viff_header.color_space_model != VFF_CM_ntscRGB) &&
        (viff_header.color_space_model != VFF_CM_genericRGB))
      PrematureExit(CorruptImageWarning,
        "Colorspace model is not supported",image);
    if (viff_header.location_type != VFF_LOC_IMPLICIT)
      {
        MagickWarning(CorruptImageWarning,
          "Location type is not supported",image->filename);
        DestroyImages(image);
        return((Image *) NULL);
      }
    if (viff_header.number_of_images != 1)
      PrematureExit(CorruptImageWarning,
        "Number of images is not supported",image);
    switch (viff_header.map_scheme)
    {
      case VFF_MS_NONE:
      {
        if (viff_header.number_data_bands < 3)
          {
            /*
              Create linear color ramp.
            */
            if (viff_header.data_storage_type == VFF_TYP_BIT)
              image->colors=2;
            else
              image->colors=1 << (viff_header.number_data_bands*QuantumDepth);
            image->colormap=(ColorPacket *)
              AllocateMemory(image->colors*sizeof(ColorPacket));
            if (image->colormap == (ColorPacket *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            for (i=0; i < (int) image->colors; i++)
            {
              image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
              image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
            }
          }
        break;
      }
      case VFF_MS_ONEPERBAND:
      case VFF_MS_SHARED:
      {
        unsigned char
          *viff_colormap;

        /*
          Read VIFF raster colormap.
        */
        image->colors=(unsigned int) viff_header.map_columns;
        image->colormap=(ColorPacket *)
          AllocateMemory(image->colors*sizeof(ColorPacket));
        viff_colormap=(unsigned char *)
          AllocateMemory(image->colors*sizeof(unsigned char));
        if ((image->colormap == (ColorPacket *) NULL) ||
            (viff_colormap == (unsigned char *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        (void) ReadData((char *) viff_colormap,1,image->colors,image->file);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=UpScale(viff_colormap[i]);
          image->colormap[i].green=UpScale(viff_colormap[i]);
          image->colormap[i].blue=UpScale(viff_colormap[i]);
        }
        if (viff_header.map_rows > 1)
          {
            (void) ReadData((char *) viff_colormap,1,image->colors,image->file);
            for (i=0; i < (int) image->colors; i++)
              image->colormap[i].green=UpScale(viff_colormap[i]);
          }
        if (viff_header.map_rows > 2)
          {
            (void) ReadData((char *) viff_colormap,1,image->colors,image->file);
            for (i=0; i < (int) image->colors; i++)
              image->colormap[i].blue=UpScale(viff_colormap[i]);
          }
        FreeMemory((char *) viff_colormap);
        break;
      }
      default:
        PrematureExit(CorruptImageWarning,"Colormap type is not supported",
          image);
    }
    /*
      Allocate VIFF pixels.
    */
    bytes_per_pixel=1;
    if (viff_header.data_storage_type == VFF_TYP_2_BYTE)
      bytes_per_pixel=2;
    if (viff_header.data_storage_type == VFF_TYP_4_BYTE)
      bytes_per_pixel=4;
    if (viff_header.data_storage_type == VFF_TYP_BIT)
      max_packets=((viff_header.columns+7) >> 3)*viff_header.rows;
    else
      max_packets=
        viff_header.columns*viff_header.rows*viff_header.number_data_bands;
    viff_pixels=(unsigned char *)
      AllocateMemory(bytes_per_pixel*max_packets*sizeof(Quantum));
    if (viff_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) ReadData((char *) viff_pixels,bytes_per_pixel,(unsigned int)
      max_packets,image->file);
    switch (viff_header.data_storage_type)
    {
      int
        max_value,
        min_value,
        value;

      register Quantum
        *q;

      unsigned long
        scale_factor;

      case VFF_TYP_1_BYTE:
      {
        register unsigned char
          *p;

        if (QuantumDepth == 8)
          break;
        /*
          Scale integer pixels to [0..MaxRGB].
        */
        p=viff_pixels;
        q=(Quantum *) viff_pixels;
        p+=max_packets-1;
        q+=max_packets-1;
        for (i=0; i < (int) max_packets; i++)
        {
          value=UpScale(*p);
          *q=(Quantum) value;
          p--;
          q--;
        }
        break;
      }
      case VFF_TYP_2_BYTE:
      {
        register short int
          *p;

        /*
          Ensure the header byte-order is most-significant byte first.
        */
        if ((viff_header.machine_dependency == VFF_DEP_DECORDER) ||
            (viff_header.machine_dependency == VFF_DEP_NSORDER))
          MSBFirstOrderShort((char *) &viff_header,
            (unsigned int) (bytes_per_pixel*max_packets));
        /*
          Determine scale factor.
        */
        p=(short int *) viff_pixels;
        max_value=(*p);
        min_value=(*p);
        for (i=0; i < (int) max_packets; i++)
        {
          if (*p > max_value)
            max_value=(*p);
          else
            if (*p < min_value)
              min_value=(*p);
          p++;
        }
        if ((min_value == 0) && (max_value == 0))
          scale_factor=0;
        else
          if (min_value == max_value)
            {
              scale_factor=UpShift(MaxRGB)/min_value;
              min_value=0;
            }
          else
            scale_factor=UpShift(MaxRGB)/(max_value-min_value);
        /*
          Scale integer pixels to [0..MaxRGB].
        */
        p=(short int *) viff_pixels;
        q=(Quantum *) viff_pixels;
        for (i=0; i < (int) max_packets; i++)
        {
          value=DownShift((*p-min_value)*scale_factor);
          if (value > MaxRGB)
            value=MaxRGB;
          else
            if (value < 0)
              value=0;
          *q=(Quantum) value;
          p++;
          q++;
        }
        break;
      }
      case VFF_TYP_4_BYTE:
      {
        register int
          *p;

        /*
          Ensure the header byte-order is most-significant byte first.
        */
        if ((viff_header.machine_dependency == VFF_DEP_DECORDER) ||
            (viff_header.machine_dependency == VFF_DEP_NSORDER))
          MSBFirstOrderLong((char *) &viff_header,
            (unsigned int) (bytes_per_pixel*max_packets));
        /*
          Determine scale factor.
        */
        p=(int *) viff_pixels;
        max_value=(*p);
        min_value=(*p);
        for (i=0; i < (int) max_packets; i++)
        {
          if (*p > max_value)
            max_value=(*p);
          else
            if (*p < min_value)
              min_value=(*p);
          p++;
        }
        if ((min_value == 0) && (max_value == 0))
          scale_factor=0;
        else
          if (min_value == max_value)
            {
              scale_factor=UpShift(MaxRGB)/min_value;
              min_value=0;
            }
          else
            scale_factor=UpShift(MaxRGB)/(max_value-min_value);
        /*
          Scale integer pixels to [0..MaxRGB].
        */
        p=(int *) viff_pixels;
        q=(Quantum *) viff_pixels;
        for (i=0; i < (int) max_packets; i++)
        {
          value=DownShift((*p-min_value)*scale_factor);
          if (value > MaxRGB)
            value=MaxRGB;
          else
            if (value < 0)
              value=0;
          *q=(unsigned char) value;
          p++;
          q++;
        }
        break;
      }
    }
    /*
      Initialize image structure.
    */
    image->matte=(viff_header.number_data_bands == 4);
    image->class=
      (viff_header.number_data_bands < 3 ? PseudoClass : DirectClass);
    image->columns=(unsigned int) viff_header.rows;
    image->rows=(unsigned int) viff_header.columns;
    if (image_info->ping)
      {
        CloseImage(image);
        return(image);
      }
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory(image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert VIFF raster image to runlength-encoded packets.
    */
    p=(Quantum *) viff_pixels;
    q=image->pixels;
    if (viff_header.data_storage_type == VFF_TYP_BIT)
      {
        unsigned int
          polarity;

        /*
          Convert bitmap scanline to runlength-encoded color packets.
        */
        polarity=(viff_header.machine_dependency == VFF_DEP_DECORDER) ||
          (viff_header.machine_dependency == VFF_DEP_NSORDER);
        for (y=0; y < (int) image->rows; y++)
        {
          /*
            Convert bitmap scanline to runlength-encoded color packets.
          */
          for (x=0; x < (int) (image->columns >> 3); x++)
          {
            for (bit=0; bit < 8; bit++)
            {
              q->index=
                ((*p) & (0x01 << bit) ? (int) polarity : (int) !polarity);
              q->length=0;
              q++;
            }
            p++;
          }
          if ((image->columns % 8) != 0)
            {
              for (bit=0; bit < (int) (image->columns % 8); bit++)
              {
                q->index=
                  ((*p) & (0x01 << bit) ? (int) polarity : (int) !polarity);
                q->length=0;
                q++;
              }
              p++;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      if (image->class == PseudoClass)
        for (y=0; y < (int) image->rows; y++)
        {
          /*
            Convert PseudoColor scanline to runlength-encoded color packets.
          */
          for (x=0; x < (int) image->columns; x++)
          {
            q->index=(*p++);
            q->length=0;
            q++;
          }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
      else
        {
          unsigned long
            offset;

          /*
            Convert DirectColor scanline to runlength-encoded color packets.
          */
          offset=image->columns*image->rows;
          for (y=0; y < (int) image->rows; y++)
          {
            for (x=0; x < (int) image->columns; x++)
            {
              q->red=(*p);
              q->green=(*(p+offset));
              q->blue=(*(p+offset*2));
              if (image->colors != 0)
                {
                  q->red=image->colormap[q->red].red;
                  q->green=image->colormap[q->green].green;
                  q->blue=image->colormap[q->blue].blue;
                }
              q->index=(unsigned short) (image->matte ? (*(p+offset*3)) : 0);
              q->length=0;
              p++;
              q++;
            }
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(LoadImageText,y,image->rows);
          }
        }
    FreeMemory((char *) viff_pixels);
    if (image->class == PseudoClass)
      SyncImage(image);
    CondenseImage(image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    status=ReadData((char *) &viff_header.identifier,1,1,image->file);
    if ((status == True) && (viff_header.identifier == 0xab))
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while ((status == True) && (viff_header.identifier == 0xab));
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ReadXImage reads an image from an X window.
%
%  The format of the ReadXImage routine is:
%
%      image=ReadXImage(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
*/
Image *ReadXImage(const ImageInfo *image_info)
{
  XImportInfo
    ximage_info;

  XGetImportInfo(&ximage_info);
  return(XImportImage(image_info,&ximage_info));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X B M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXBMImage reads an X11 bitmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXBMImage routine is:
%
%      image=ReadXBMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXBMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
static int XBMInteger(FILE *file,short int *hex_digits)
{
  int
    c,
    flag,
    value;

  value=0;
  flag=0;
  for ( ; ; )
  {
    c=fgetc(file);
    if (c == EOF)
      {
        value=(-1);
        break;
      }
    c&=0xff;
    if (isxdigit(c))
      {
        value=(value << 4)+hex_digits[c];
        flag++;
        continue;
      }
    if ((hex_digits[c]) < 0 && flag)
      break;
  }
  return(value);
}

Image *ReadXBMImage(const ImageInfo *image_info)
{
  char
    buffer[MaxTextExtent],
    name[MaxTextExtent];

  Image
    *image;

  register int
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  register long
    packets;

  short int
    hex_digits[256];

  unsigned char
    bit,
    *data;

  unsigned int
    byte,
    bytes_per_line,
    padding,
    value,
    version;

  unsigned long
    max_packets;

  unsigned short
    index;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read X bitmap header.
  */
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
    if (sscanf(buffer,"#define %s %u",name,&image->columns) == 2)
      if ((strlen(name) >= 6) &&
          (Latin1Compare(name+strlen(name)-6,"_width") == 0))
          break;
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
    if (sscanf(buffer,"#define %s %u",name,&image->rows) == 2)
      if ((strlen(name) >= 7) &&
          (Latin1Compare(name+strlen(name)-7,"_height") == 0))
          break;
  image->class=PseudoClass;
  image->colors=2;
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Scan until hex digits.
  */
  version=11;
  while (fgets(buffer,MaxTextExtent,image->file) != (char *) NULL)
  {
    if (sscanf(buffer,"static short %s = {",name) == 1)
      version=10;
    else
      if (sscanf(buffer,"static unsigned char %s = {",name) == 1)
        version=11;
      else
        if (sscanf(buffer,"static char %s = {",name) == 1)
          version=11;
        else
          continue;
    p=(unsigned char *) strrchr(name,'_');
    if (p == (unsigned char *) NULL)
      p=(unsigned char *) name;
    else
      p++;
    if (Latin1Compare("bits[]",(char *) p) == 0)
      break;
  }
  if ((image->columns == 0) || (image->rows == 0) || feof(image->file))
    PrematureExit(CorruptImageWarning,"XBM file is not in the correct format",
      image);
  /*
    Initialize image structure.
  */
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  packets=0;
  max_packets=Max((image->columns*image->rows+4) >> 3,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  padding=0;
  if ((image->columns % 16) && ((image->columns % 16) < 9)  && (version == 10))
    padding=1;
  bytes_per_line=(image->columns+7)/8+padding;
  data=(unsigned char *)
    AllocateMemory(bytes_per_line*image->rows*sizeof(unsigned char *));
  if ((image->colormap == (ColorPacket *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL) ||
      (data == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize colormap.
  */
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  /*
    Initialize hex values.
  */
  hex_digits['0']=0;
  hex_digits['1']=1;
  hex_digits['2']=2;
  hex_digits['3']=3;
  hex_digits['4']=4;
  hex_digits['5']=5;
  hex_digits['6']=6;
  hex_digits['7']=7;
  hex_digits['8']=8;
  hex_digits['9']=9;
  hex_digits['A']=10;
  hex_digits['B']=11;
  hex_digits['C']=12;
  hex_digits['D']=13;
  hex_digits['E']=14;
  hex_digits['F']=15;
  hex_digits['a']=10;
  hex_digits['b']=11;
  hex_digits['c']=12;
  hex_digits['d']=13;
  hex_digits['e']=14;
  hex_digits['f']=15;
  hex_digits['x']=0;
  hex_digits[' ']=(-1);
  hex_digits[',']=(-1);
  hex_digits['}']=(-1);
  hex_digits['\n']=(-1);
  hex_digits['\t']=(-1);
  /*
    Read hex image data.
  */
  p=data;
  if (version == 10)
    for (x=0; x < (int) (bytes_per_line*image->rows); (x+=2))
    {
      value=XBMInteger(image->file,hex_digits);
      *p++=value;
      if (!padding || ((x+2) % bytes_per_line))
        *p++=value >> 8;
    }
  else
    for (x=0; x < (int) (bytes_per_line*image->rows); x++)
    {
      value=XBMInteger(image->file,hex_digits);
      *p++=value;
    }
  /*
    Convert X bitmap image to runlength-encoded packets.
  */
  byte=0;
  p=data;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; y < (int) image->rows; y++)
  {
    bit=0;
    for (x=0; x < (int) image->columns; x++)
    {
      if (bit == 0)
        byte=(*p++);
      index=byte & 0x01 ? 0 : 1;
      if ((index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == (int) max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  FreeMemory((char *) data);
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              q=image->pixels+packets-1;
            }
          q->index=index;
          q->length=0;
        }
      bit++;
      byte>>=1;
      if (bit == 8)
        bit=0;
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
  }
  FreeMemory((char *) data);
  SetRunlengthPackets(image,packets);
  SyncImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X C I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXCImage creates a constant image and initializes it to the
%  X server color as specified by the filename.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadXCImage routine is:
%
%      image=ReadXCImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXCImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadXCImage(const ImageInfo *image_info)
{
  Image
    *image;

  register int
    i;

  register RunlengthPacket
    *q;

  XColor
    color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  /*
    Initialize Image structure.
  */
  (void) strcpy(image->filename,image_info->filename);
  image->packets=((Max(image->columns*image->rows,1)-1)/(MaxRunlength+1))+1;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  image->class=PseudoClass;
  image->colors=1;
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->colormap == (ColorPacket *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize colormap.
  */
  (void) XQueryColorDatabase((char *) image_info->filename,&color);
  image->colormap[0].red=XDownScale(color.red);
  image->colormap[0].green=XDownScale(color.green);
  image->colormap[0].blue=XDownScale(color.blue);
  q=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    q->index=0;
    SetRunlengthEncoder(q);
    q++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(LoadImageText,i,image->packets);
  }
  q--;
  q->length=image->columns*image->rows-(MaxRunlength+1)*(image->packets-1)-1;
  SyncImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X P M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXPMImage reads an X11 pixmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXPMImage routine is:
%
%      image=ReadXPMImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXPMImage returns a pointer to the image after
%      creating it. A null image is returned if there is a memory shortage
%      or if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/

static char *ParseColor(char *data)
{
#define NumberTargets  6

  static const char
    *targets[NumberTargets] = { "c ", "g ", "g4 ", "m ", "b ", "s " };

  register char
     *p,
     *r;

  register const char
     *q;

  register int
    i;

  for (i=0; i < NumberTargets; i++)
  {
    r=data;
    for (q=targets[i]; *r != '\0'; r++)
    {
      if (*r != *q)
        continue;
      if (!isspace((int) (*(r-1))))
        continue;
      p=r;
      for ( ; ; )
      {
        if (*q == '\0')
          return(r);
        if (*p++ != *q++)
          break;
      }
      q=targets[i];
    }
  }
  return((char *) NULL);
}

Image *ReadXPMImage(const ImageInfo *image_info)
{
  char
    key[MaxTextExtent],
    target[MaxTextExtent],
    **textlist,
    *xpm_buffer;

  Image
    *image;

  int
    count,
    length,
    x,
    y;

  register char
    *p,
    *q;

  register int
    i,
    j;

  register RunlengthPacket
    *r;

  unsigned int
    width;

  XColor
    color;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,"r");
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Read XPM file.
  */
  length=MaxTextExtent;
  xpm_buffer=(char *) AllocateMemory(length*sizeof(char));
  p=xpm_buffer;
  if (xpm_buffer != (char *) NULL)
    while (fgets(p,MaxTextExtent,image->file) != (char *) NULL)
    {
      if (*p == '#')
        if ((p == xpm_buffer) || (*(p-1) == '\n'))
          continue;
      if ((*p == '}') && (*(p+1) == ';'))
        break;
      p+=Extent(p);
      if ((p-xpm_buffer+MaxTextExtent+1) < length)
        continue;
      length<<=1;
      xpm_buffer=(char *)
        ReallocateMemory((char *) xpm_buffer,length*sizeof(char));
      if (xpm_buffer == (char *) NULL)
        break;
      p=xpm_buffer+Extent(xpm_buffer);
    }
  if (xpm_buffer == (char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Remove comments.
  */
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if ((*p == '"') || (*p == '\''))
      {
        if (*p == '"')
          {
            for (p++; *p != '\0'; p++)
              if ((*p == '"') && (*(p-1) != '\\'))
                break;
          }
        else
          for (p++; *p != '\0'; p++)
            if ((*p == '\'') && (*(p-1) != '\\'))
              break;
        if (*p == '\0')
          break;
        continue;
      }
    if ((*p != '/') || (*(p+1) != '*'))
      continue;
    for (q=p+2; *q != '\0'; q++)
      if ((*q == '*') && (*(q+1) == '/'))
        break;
    (void) strcpy(p,q+2);
  }
  /*
    Remove unquoted characters.
  */
  i=0;
  for (p=xpm_buffer; *p != '\0'; p++)
  {
    if (*p != '"')
      continue;
    for (q=p+1; *q != '\0'; q++)
      if (*q == '"')
        break;
    (void) strncpy(xpm_buffer+i,p+1,q-p-1);
    i+=q-p-1;
    xpm_buffer[i++]='\n';
    p=q+1;
  }
  xpm_buffer[i]='\0';
  textlist=StringToList(xpm_buffer);
  FreeMemory(xpm_buffer);
  if (textlist == (char **) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Read hints.
  */
  image->class=PseudoClass;
  count=sscanf(textlist[0],"%u %u %u %u",&image->columns,&image->rows,
    &image->colors,&width);
  if ((count != 4) || (width > 2) ||
      ((image->columns*image->rows*image->colors) == 0))
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((char *) textlist[i]);
      FreeMemory((char *) textlist);
      PrematureExit(CorruptImageWarning,"Not a XPM image file",image);
    }
  /*
    Initialize image structure.
  */
  image->colormap=(ColorPacket *)
    AllocateMemory(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((char *) textlist[i]);
      FreeMemory((char *) textlist);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  /*
    Read image colormap.
  */
  i=1;
  for (j=0; j < (int) image->colors; j++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    image->colormap[j].key[width]='\0';
    (void) strncpy(image->colormap[j].key,p,width);
    /*
      Parse color.
    */
    (void) strcpy(target,"gray");
    q=ParseColor(p+width);
    if (q != (char *) NULL)
      {
        while (!isspace((int) (*q)) && (*q != '\0'))
          q++;
        (void) strcpy(target,q);
        q=ParseColor(target);
        if (q != (char *) NULL)
          *q='\0';
      }
    Strip(target);
    image->colormap[j].flags=Latin1Compare(target,"none") == 0;
    if (image->colormap[j].flags)
      {
        image->class=DirectClass;
        image->matte=True;
        (void) strcpy(target,"black");
      }
    (void) XQueryColorDatabase(target,&color);
    image->colormap[j].red=XDownScale(color.red);
    image->colormap[j].green=XDownScale(color.green);
    image->colormap[j].blue=XDownScale(color.blue);
  }
  if (j < (int) image->colors)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((char *) textlist[i]);
      FreeMemory((char *) textlist);
      PrematureExit(CorruptImageWarning,"Corrupt XPM image file",image);
    }
  if (image_info->ping)
    {
      CloseImage(image);
      return(image);
    }
  /*
    Read image pixels.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    AllocateMemory(image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((char *) textlist[i]);
      FreeMemory((char *) textlist);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  SetImage(image);
  j=0;
  key[width]='\0';
  r=image->pixels;
  for (y=0; y < (int) image->rows; y++)
  {
    p=textlist[i++];
    if (p == (char *) NULL)
      break;
    for (x=0; x < (int) image->columns; x++)
    {
      (void) strncpy(key,p,width);
      if (strcmp(key,image->colormap[j].key) != 0)
        for (j=0; j < (int) (image->colors-1); j++)
          if (strcmp(key,image->colormap[j].key) == 0)
            break;
      r->red=image->colormap[j].red;
      r->green=image->colormap[j].green;
      r->blue=image->colormap[j].blue;
      if (image->class == PseudoClass)
        r->index=(unsigned short) j;
      else
        if (image->colormap[j].flags)
          r->index=Transparent;
        else
          r->index=Opaque;
      r->length=0;
      r++;
      p+=width;
    }
  }
  if (y < (int) image->rows)
    {
      for (i=0; textlist[i] != (char *) NULL; i++)
        FreeMemory((char *) textlist[i]);
      FreeMemory((char *) textlist);
      PrematureExit(CorruptImageWarning,"Corrupt XPM image file",image);
    }
  /*
    Free resources.
  */
  for (i=0; textlist[i] != (char *) NULL; i++)
    FreeMemory((char *) textlist[i]);
  FreeMemory((char *) textlist);
  CondenseImage(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X W D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXWDImage reads an X Window System window dump image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadXWDImage routine is:
%
%      image=ReadXWDImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXWDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadXWDImage(const ImageInfo *image_info)
{
  Image
    *image;

  int
    status,
    y;

  register int
    i,
    x;

  register long
    packets;

  register RunlengthPacket
    *q;

  register unsigned long
    pixel;

  unsigned long
    lsb_first,
    max_packets;

  unsigned short
    index;

  XColor
    *colors;

  XImage
    *ximage;

  XWDFileHeader
    header;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
     Read in header information.
  */
  status=ReadData((char *) &header,sz_XWDheader,1,image->file);
  if (status == False)
    PrematureExit(CorruptImageWarning,"Unable to read dump file header",image);
  image->columns=header.pixmap_width;
  image->rows=header.pixmap_height;
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &header,sz_XWDheader);
  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    PrematureExit(CorruptImageWarning,"XWD file format version mismatch",image);
  if (header.header_size < sz_XWDheader)
    PrematureExit(CorruptImageWarning,"XWD header size is too small",image);
  max_packets=(header.header_size-sz_XWDheader);
  image->comments=(char *) AllocateMemory((max_packets+1)*sizeof(char));
  if (image->comments == (char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  status=ReadData((char *) image->comments,1,max_packets,image->file);
  image->comments[max_packets]='\0';
  if (status == False)
    PrematureExit(CorruptImageWarning,
      "Unable to read window name from dump file",image);
  /*
    Initialize the X image.
  */
  ximage=(XImage *) AllocateMemory(sizeof(XImage));
  if (ximage == (XImage *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  ximage->depth=header.pixmap_depth;
  ximage->format=header.pixmap_format;
  ximage->xoffset=header.xoffset;
  ximage->data=(char *) NULL;
  ximage->width=header.pixmap_width;
  ximage->height=header.pixmap_height;
  ximage->bitmap_pad=header.bitmap_pad;
  ximage->bytes_per_line=header.bytes_per_line;
  ximage->byte_order=header.byte_order;
  ximage->bitmap_unit=header.bitmap_unit;
  ximage->bitmap_bit_order=header.bitmap_bit_order;
  ximage->bits_per_pixel=header.bits_per_pixel;
  ximage->red_mask=header.red_mask;
  ximage->green_mask=header.green_mask;
  ximage->blue_mask=header.blue_mask;
  status=XInitImage(ximage);
  if (status == False)
    PrematureExit(CorruptImageWarning,"Invalid XWD header",image);
  /*
    Read colormap.
  */
  colors=(XColor *) NULL;
  if (header.ncolors != 0)
    {
      XWDColor
        color;

      colors=(XColor *)
        AllocateMemory((unsigned int) header.ncolors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < (int) header.ncolors; i++)
      {
        status=ReadData((char *) &color,sz_XWDColor,1,image->file);
        if (status == False)
          PrematureExit(CorruptImageWarning,
            "Unable to read color map from dump file",image);
        colors[i].pixel=color.pixel;
        colors[i].red=color.red;
        colors[i].green=color.green;
        colors[i].blue=color.blue;
        colors[i].flags=color.flags;
      }
      /*
        Ensure the header byte-order is most-significant byte first.
      */
      lsb_first=1;
      if (*(char *) &lsb_first)
        for (i=0; i < (int) header.ncolors; i++)
        {
          MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(unsigned long));
          MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(unsigned short));
        }
    }
  /*
    Allocate the pixel buffer.
  */
  if (ximage->format == ZPixmap)
    max_packets=ximage->bytes_per_line*ximage->height;
  else
    max_packets=ximage->bytes_per_line*ximage->height*ximage->depth;
  ximage->data=(char *) AllocateMemory(max_packets*sizeof(unsigned char));
  if (ximage->data == (char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  status=ReadData(ximage->data,1,max_packets,image->file);
  if (status == False)
    PrematureExit(CorruptImageWarning,"Unable to read dump pixmap",image);
  /*
    Convert image to MIFF format.
  */
  image->columns=ximage->width;
  image->rows=ximage->height;
  if ((colors == (XColor *) NULL) || (ximage->red_mask != 0) ||
      (ximage->green_mask != 0) || (ximage->blue_mask != 0))
    image->class=DirectClass;
  else
    image->class=PseudoClass;
  image->colors=header.ncolors;
  if (image_info->ping)
    {
      if (header.ncolors != 0)
        FreeMemory((char *) colors);
      CloseImage(image);
      return(image);
    }
  packets=0;
  max_packets=Max((image->columns*image->rows+4) >> 3,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  q=image->pixels;
  SetRunlengthEncoder(q);
  switch (image->class)
  {
    case DirectClass:
    default:
    {
      register unsigned long
        color;

      unsigned int
        blue,
        green,
        red;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=ximage->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=ximage->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=ximage->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if (image->colors != 0)
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            index=(unsigned short) ((pixel >> red_shift) & red_mask);
            red=XDownScale(colors[index].red);
            index=(unsigned short) ((pixel >> green_shift) & green_mask);
            green=XDownScale(colors[index].green);
            index=(unsigned short) ((pixel >> blue_shift) & blue_mask);
            blue=XDownScale(colors[index].blue);
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      PrematureExit(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=0;
                q->length=0;
              }
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,y,image->rows);
        }
      else
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            red=XDownScale((color*65535L)/red_mask);
            color=(pixel >> green_shift) & green_mask;
            green=XDownScale((color*65535L)/green_mask);
            color=(pixel >> blue_shift) & blue_mask;
            blue=XDownScale((color*65535L)/blue_mask);
            if ((red == q->red) && (green == q->green) && (blue == q->blue) &&
                ((int) q->length < MaxRunlength))
              q->length++;
            else
              {
                if (packets != 0)
                  q++;
                packets++;
                if (packets == (int) max_packets)
                  {
                    max_packets<<=1;
                    image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                      image->pixels,max_packets*sizeof(RunlengthPacket));
                    if (image->pixels == (RunlengthPacket *) NULL)
                      PrematureExit(ResourceLimitWarning,
                        "Memory allocation failed",image);
                    q=image->pixels+packets-1;
                  }
                q->red=red;
                q->green=green;
                q->blue=blue;
                q->index=0;
                q->length=0;
              }
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(LoadImageText,y,image->rows);
        }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert X image to PseudoClass packets.
      */
      image->colormap=(ColorPacket *)
        AllocateMemory(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < (int) image->colors; i++)
      {
        image->colormap[i].red=XDownScale(colors[i].red);
        image->colormap[i].green=XDownScale(colors[i].green);
        image->colormap[i].blue=XDownScale(colors[i].blue);
      }
      for (y=0; y < (int) image->rows; y++)
      {
        for (x=0; x < (int) image->columns; x++)
        {
          pixel=XGetPixel(ximage,x,y);
          index=(unsigned short) pixel;
          if ((index == q->index) && ((int) q->length < MaxRunlength))
            q->length++;
          else
            {
              if (packets != 0)
                q++;
              packets++;
              if (packets == (int) max_packets)
                {
                  max_packets<<=1;
                  image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                    image->pixels,max_packets*sizeof(RunlengthPacket));
                  if (image->pixels == (RunlengthPacket *) NULL)
                    PrematureExit(ResourceLimitWarning,
                      "Memory allocation failed",image);
                  q=image->pixels+packets-1;
                }
              q->index=index;
              q->length=0;
            }
        }
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
  }
  SetRunlengthPackets(image,packets);
  if (image->class == PseudoClass)
    SyncImage(image);
  /*
    Free image and colormap.
  */
  if (header.ncolors != 0)
    FreeMemory((char *) colors);
  FreeMemory(ximage->data);
  FreeMemory(ximage);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d Y U V I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadYUVImage reads an image with digital YUV (CCIR 601 4:1:1) bytes
%  and returns it.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the ReadYUVImage routine is:
%
%      image=ReadYUVImage(image_info)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadYUVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Image *ReadYUVImage(const ImageInfo *image_info)
{
  Image
    *image,
    *zoomed_image;

  int
    count,
    y;

  register int
    i,
    x;

  register RunlengthPacket
    *q,
    *r;

  register unsigned char
    *p;

  unsigned char
    *scanline;

  /*
    Allocate image structure.
  */
  image=AllocateImage(image_info);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if ((image->columns == 0) || (image->rows == 0))
    PrematureExit(OptionWarning,"must specify image size",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      OpenImage(image_info,image,ReadBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) fgetc(image->file);
    }
  /*
    Allocate memory for a scanline.
  */
  scanline=(unsigned char *)
    AllocateMemory(image->columns*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  do
  {
    /*
      Initialize image structure.
    */
    image->columns>>=1;
    image->rows>>=1;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      AllocateMemory((image->packets << 2)*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    SetImage(image);
    /*
      Convert raster image to runlength-encoded packets.
    */
    if (image_info->interlace == PartitionInterlace)
      {
        AppendImageFormat("Y",image->filename);
        OpenImage(image_info,image,ReadBinaryType);
        if (image->file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to open file",image);
      }
    i=0;
    q=image->pixels;
    for (y=0; y < (int) (image->rows << 1); y++)
    {
      if ((y > 0) || (image->previous == (Image *) NULL))
        (void) ReadData((char *) scanline,1,image->columns << 1,image->file);
      p=scanline;
      for (x=0; x < (int) (image->columns << 1); x++)
      {
        q->red=UpScale(*p++);
        q->index=0;
        q->length=0;
        q++;
      }
      if (image->previous == (Image *) NULL)
        ProgressMonitor(LoadImageText,i,3);
      i++;
    }
    if (image_info->interlace == PartitionInterlace)
      {
        CloseImage(image);
        AppendImageFormat("U",image->filename);
        OpenImage(image_info,image,ReadBinaryType);
        if (image->file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to open file",image);
      }
    q=image->pixels;
    for (y=0; y < (int) image->rows; y++)
    {
      (void) ReadData((char *) scanline,1,image->columns,image->file);
      p=scanline;
      for (x=0; x < (int) image->columns; x++)
      {
        q->green=UpScale(*p++);
        q++;
      }
      if (image->previous == (Image *) NULL)
        ProgressMonitor(LoadImageText,i,3);
      i++;
    }
    if (image_info->interlace == PartitionInterlace)
      {
        CloseImage(image);
        AppendImageFormat("V",image->filename);
        OpenImage(image_info,image,ReadBinaryType);
        if (image->file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to open file",image);
      }
    q=image->pixels;
    for (y=0; y < (int) image->rows; y++)
    {
      (void) ReadData((char *) scanline,1,image->columns,image->file);
      p=scanline;
      for (x=0; x < (int) image->columns; x++)
      {
        q->blue=UpScale(*p++);
        q++;
      }
      if (image->previous == (Image *) NULL)
        ProgressMonitor(LoadImageText,i,3);
      i++;
    }
    /*
      Scale image.
    */
    image->orphan=True;
    zoomed_image=MagnifyImage(image);
    image->orphan=False;
    if (zoomed_image == (Image *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    image->columns<<=1;
    image->rows<<=1;
    image->packets=image->columns*image->rows;
    q=image->pixels;
    r=zoomed_image->pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      q->green=r->green;
      q->blue=r->blue;
      q++;
      r++;
    }
    DestroyImage(zoomed_image);
    TransformRGBImage(image,YCbCrColorspace);
    CondenseImage(image);
    if (image_info->interlace == PartitionInterlace)
      (void) strcpy(image->filename,image_info->filename);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadData((char *) scanline,1,image->columns,image->file);
    if (count > 0)
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
        ProgressMonitor(LoadImagesText,(unsigned int) ftell(image->file),
          (unsigned int) image->filesize);
      }
  } while (count > 0);
  FreeMemory((char *) scanline);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadImage reads an image and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.  By default, the image format is determined by its magic
%  number. To specify a particular image format, precede the filename with an
%  explicit image format name and a colon (i.e.  ps:image) or as the filename
%  suffix  (i.e. image.ps).
%
%  The format of the ReadImage routine is:
%
%      image=ReadImage(image_info)
%
%  A description of each parameter follows:
%
%    o image: Method ReadImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%
*/
Export Image *ReadImage(ImageInfo *image_info)
{
  char
    filename[MaxTextExtent];

  DelegateInfo
    delegate_info;

  Image
    *image,
    *next_image;

  MagickInfo
    *magick_info;

  register char
    *p;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->filename != (char *) NULL);
  if (*image_info->filename == '@')
    return(ReadImages(image_info));
  SetImageInfo(image_info,False);
  /*
    Call appropriate image reader based on image type.
  */
  image=(Image *) NULL;
  magick_info=(MagickInfo *) GetMagickInfo(image_info->magick);
  if ((magick_info != (MagickInfo *) NULL) &&
      (magick_info->decoder != (Image *(*)(const ImageInfo *)) NULL))
    image=(magick_info->decoder)(image_info);
  else
    if (!GetDelegateInfo(image_info->magick,(char *) NULL,&delegate_info))
      MagickWarning(MissingDelegateWarning,"no delegate for this image format",
        image_info->magick);
    else
      {
        unsigned int
          status;

        /*
          Let our decoding delegate process the image.
        */
        image=AllocateImage(image_info);
        if (image == (Image *) NULL)
          return((Image *) NULL);
        (void) strcpy(filename,image_info->filename);
        (void) strcpy(image->filename,image_info->filename);
        TemporaryFilename(image_info->filename);
        status=
          InvokeDelegate(image_info,image,image_info->magick,(char *) NULL);
        DestroyImages(image);
        if (status == False)
          image_info->temporary=True;
        SetImageInfo(image_info,False);
        magick_info=(MagickInfo *) GetMagickInfo(image_info->magick);
        if ((magick_info != (MagickInfo *) NULL) &&
            (magick_info->decoder != (Image *(*)(const ImageInfo *)) NULL))
          image=(magick_info->decoder)(image_info);
        else
          MagickWarning(MissingDelegateWarning,
            "no delegate for this image format",image_info->magick);
      }
  if (image_info->temporary)
    {
      (void) remove(image_info->filename);
      image_info->temporary=False;
      if (image != (Image *) NULL)
        (void) strcpy(image->filename,filename);
    }
  if (image == (Image *) NULL)
    return(image);
  if (image->temporary)
    (void) remove(image_info->filename);
  if (IsSubimage(image_info->tile,False))
    {
      int
        count,
        offset,
        quantum;

      Image
        *subimages;

      unsigned int
        last,
        target;

      /*
        User specified subimages (e.g. image.miff[1,3-5,7-6,2]).
      */
      subimages=(Image *) NULL;
      target=atoi(image_info->tile);
      for (p=image_info->tile; *p != '\0'; p+=Max(offset,1))
      {
        offset=0;
        count=sscanf(p,"%u%n-%u%n",&target,&offset,&last,&offset);
        if (count == 0)
          continue;
        if (count == 1)
          last=target;
        quantum=target > last ? -1 : 1;
        for ( ; target != (last+quantum); target+=quantum)
        {
          for (next_image=image; next_image; next_image=next_image->next)
          {
            Image
              *clone_image;

            if (next_image->scene != target)
              continue;
            /*
              Clone this subimage.
            */
            next_image->orphan=True;
            clone_image=CloneImage(next_image,next_image->columns,
              next_image->rows,True);
            next_image->orphan=False;
            if (clone_image == (Image *) NULL)
              {
                MagickWarning(ResourceLimitWarning,"Memory allocation failed",
                  image_info->filename);
                break;
              }
            if (subimages == (Image *) NULL)
              {
                subimages=clone_image;
                continue;
              }
            subimages->next=clone_image;
            subimages->next->previous=subimages;
            subimages=subimages->next;
          }
        }
      }
      DestroyImages(image);
      if (subimages == (Image *) NULL)
        {
          MagickWarning(OptionWarning,
            "Subimage specification returns no images",image_info->filename);
          return((Image *) NULL);
        }
      while (subimages->previous != (Image *) NULL)
        subimages=subimages->previous;
      image=subimages;
    }
  else
    if ((image_info->subrange != 0) && (image->next != (Image *) NULL))
      {
        int
          retain;

        /*
          User specified subimages (e.g. image.miff[1]).
        */
        for ( ; ; )
        {
          retain=(image->scene >= image_info->subimage) &&
            (image->scene <= (image_info->subimage+image_info->subrange-1));
          if (image->next != (Image *) NULL)
            {
              image=image->next;
              if (!retain)
                DestroyImage(image->previous);
              continue;
            }
          if (image->previous != (Image *) NULL)
            {
              image=image->previous;
              if (!retain)
                DestroyImage(image->next);
              break;
            }
          if (!retain)
            {
              DestroyImage(image);
              image=(Image *) NULL;
            }
          break;
        }
        if (image == (Image *) NULL)
          {
            MagickWarning(OptionWarning,
              "Subimage specification returns no images",image_info->filename);
            return((Image *) NULL);
          }
        while (image->previous != (Image *) NULL)
          image=image->previous;
      }
  if (image->status)
    MagickWarning(CorruptImageWarning,"An error has occurred reading file",
      image->filename);
  for (next_image=image; next_image; next_image=next_image->next)
  {
    (void) strcpy(next_image->magick_filename,image_info->filename);
    if (image->temporary)
      (void) strcpy(next_image->filename,image_info->filename);
    next_image->magick_columns=next_image->columns;
    next_image->magick_rows=next_image->rows;
    if (next_image->class == PseudoClass)
      if (IsMonochromeImage(next_image))
        {
          next_image->background_color.red=MaxRGB;
          next_image->background_color.green=MaxRGB;
          next_image->background_color.blue=MaxRGB;
        }
  }
  return(image);
}
