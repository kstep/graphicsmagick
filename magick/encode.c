/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                   EEEEE  N   N   CCCC   OOO   DDDD   EEEEE                  %
%                   E      NN  N  C      O   O  D   D  E                      %
%                   EEE    N N N  C      O   O  D   D  EEE                    %
%                   E      N  NN  C      O   O  D   D  E                      %
%                   EEEEE  N   N   CCCC   OOO   DDDD   EEEEE                  %
%                                                                             %
%                                                                             %
%                        Methods to Write Image Formats                       %
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
#include "Colorlist.h"

/*
  Define declarations.
*/
#define SaveImageText  "  Saving image...  "
#define SaveImagesText  "  Saving images...  "
#define PrematureExit(error,message,image) \
{ \
  MagickWarning(error,message,image->filename); \
  if (image_info->adjoin) \
    while (image->previous != (Image *) NULL) \
      image=image->previous; \
  CloseImage(image); \
  return(False); \
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e A V S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteAVSImage writes an image to a file in AVS X image format.
%
%  The format of the WriteAVSImage routine is:
%
%      status=WriteAVSImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteAVSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteAVSImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Write AVS header.
    */
    MSBFirstWriteLong(image->columns,image->file);
    MSBFirstWriteLong(image->rows,image->file);
    /*
      Allocate memory for pixels.
    */
    pixels=(unsigned char *)
      AllocateMemory(image->columns*sizeof(RunlengthPacket));
    if (pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert MIFF to AVS raster pixels.
    */
    x=0;
    y=0;
    p=image->pixels;
    q=pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        *q++=DownScale(image->matte ? p->index : Opaque);
        *q++=DownScale(p->red);
        *q++=DownScale(p->green);
        *q++=DownScale(p->blue);
        x++;
        if (x == (int) image->columns)
          {
            (void) fwrite((char *) pixels,1,q-pixels,image->file);
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            q=pixels;
            x=0;
            y++;
          }
      }
      p++;
    }
    FreeMemory((char *) pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
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
%  image format.
%
%  The format of the WriteBMPImage routine is:
%
%      status=WriteBMPImage(image_info,image)
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
unsigned int WriteBMPImage(const ImageInfo *image_info,Image *image)
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
      bit_count;

    unsigned long
      compression,
      image_size,
      x_pixels,
      y_pixels,
      number_colors,
      colors_important;
  } BMPHeader;

  BMPHeader
    bmp_header;

  register int
    i,
    j,
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *bmp_data,
    *bmp_pixels;

  unsigned int
    bytes_per_line,
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize BMP raster file header.
    */
    bmp_header.file_size=14+40;
    bmp_header.offset_bits=14+40;
    if ((Latin1Compare(image_info->magick,"BMP24") == 0) ||
        (!IsPseudoClass(image) && !IsGrayImage(image)))
      {
        /*
          Full color BMP raster.
        */
        image->class=DirectClass;
        bmp_header.number_colors=0;
        bmp_header.bit_count=image->matte ? 32 : 24;
        bytes_per_line=((image->columns*bmp_header.bit_count+31)/32)*4;
      }
    else
      {
        /*
          Colormapped BMP raster.
        */
        bmp_header.bit_count=8;
        bytes_per_line=((image->columns*bmp_header.bit_count+31)/32)*4;
        if (image_info->compression != NoCompression)
          bytes_per_line=image->columns;
        if (IsMonochromeImage(image))
          {
            bmp_header.bit_count=1;
            bytes_per_line=((image->columns*bmp_header.bit_count+31)/32)*4;
          }
        bmp_header.file_size+=4*(1 << bmp_header.bit_count);
        bmp_header.offset_bits+=4*(1 << bmp_header.bit_count);
        bmp_header.number_colors=1 << bmp_header.bit_count;
      }
    bmp_header.reserved[0]=0;
    bmp_header.reserved[1]=0;
    bmp_header.size=40;
    bmp_header.width=image->columns;
    bmp_header.height=image->rows;
    bmp_header.planes=1;
    bmp_header.compression=0;
    bmp_header.image_size=bytes_per_line*image->rows;
    bmp_header.file_size+=bmp_header.image_size;
    bmp_header.x_pixels=75*39;
    bmp_header.y_pixels=75*39;
    if (image->units == PixelsPerInchResolution)
      {
        bmp_header.x_pixels=(unsigned long) (100.0*image->x_resolution/2.54);
        bmp_header.y_pixels=(unsigned long) (100.0*image->y_resolution/2.54);
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        bmp_header.x_pixels=(unsigned long) (100.0*image->x_resolution);
        bmp_header.y_pixels=(unsigned long) (100.0*image->y_resolution);
      }
    bmp_header.colors_important=bmp_header.number_colors;
    /*
      Convert MIFF to BMP raster pixels.
    */
    bmp_pixels=(unsigned char *)
      AllocateMemory(bmp_header.image_size*sizeof(unsigned char));
    if (bmp_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    x=0;
    y=image->rows-1;
    switch (bmp_header.bit_count)
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
        p=image->pixels;
        polarity=Intensity(image->colormap[0]) < (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(image->colormap[1]) > Intensity(image->colormap[0]);
        bit=0;
        byte=0;
        q=bmp_pixels+y*bytes_per_line;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            byte<<=1;
            if (p->index == polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
            x++;
            if (x == (int) image->columns)
              {
                /*
                  Advance to the next scanline.
                */
                if (bit != 0)
                  *q++=byte << (8-bit);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(image->rows-y-1,image->rows))
                    ProgressMonitor(SaveImageText,image->rows-y-1,image->rows);
                bit=0;
                byte=0;
                x=0;
                y--;
                q=bmp_pixels+y*bytes_per_line;
             }
          }
          p++;
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoClass packet to BMP pixel.
        */
        p=image->pixels;
        q=bmp_pixels+y*bytes_per_line;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=p->index;
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(image->rows-y-1,image->rows))
                    ProgressMonitor(SaveImageText,image->rows-y-1,image->rows);
                x=0;
                y--;
                q=bmp_pixels+y*bytes_per_line;
              }
          }
          p++;
        }
        break;
      }
      case 24:
      case 32:
      {
        /*
          Convert DirectClass packet to BMP RGB pixel.
        */
        p=image->pixels;
        q=bmp_pixels+y*bytes_per_line;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=DownScale(p->blue);
            *q++=DownScale(p->green);
            *q++=DownScale(p->red);
            if (image->matte)
              *q++=Opaque-DownScale(p->index);
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(image->rows-y-1,image->rows))
                    ProgressMonitor(SaveImageText,image->rows-y-1,image->rows);
                x=0;
                y--;
                q=bmp_pixels+y*bytes_per_line;
              }
          }
          p++;
        }
        break;
      }
    }
    if (bmp_header.bit_count == 8)
      if (image_info->compression != NoCompression)
        {
          unsigned int
            packets;

          /*
            Convert run-length encoded raster pixels.
          */
          packets=(unsigned int)
            ((bytes_per_line*(bmp_header.height+2)+1) << 1);
          bmp_data=(unsigned char *)
            AllocateMemory(packets*sizeof(unsigned char));
          if (bmp_pixels == (unsigned char *) NULL)
            {
              FreeMemory((char *) bmp_pixels);
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            }
          bmp_header.file_size-=bmp_header.image_size;
          bmp_header.image_size=
            BMPEncodeImage(bmp_pixels,image->columns,image->rows,bmp_data);
          bmp_header.file_size+=bmp_header.image_size;
          FreeMemory((char *) bmp_pixels);
          bmp_pixels=bmp_data;
          bmp_header.compression=1;
        }
    /*
      Write BMP header.
    */
    (void) fwrite("BM",1,2,image->file);
    LSBFirstWriteLong(bmp_header.file_size,image->file);
    LSBFirstWriteShort(bmp_header.reserved[0],image->file);
    LSBFirstWriteShort(bmp_header.reserved[1],image->file);
    LSBFirstWriteLong(bmp_header.offset_bits,image->file);
    LSBFirstWriteLong(bmp_header.size,image->file);
    LSBFirstWriteLong(bmp_header.width,image->file);
    LSBFirstWriteLong(bmp_header.height,image->file);
    LSBFirstWriteShort(bmp_header.planes,image->file);
    LSBFirstWriteShort(bmp_header.bit_count,image->file);
    LSBFirstWriteLong(bmp_header.compression,image->file);
    LSBFirstWriteLong(bmp_header.image_size,image->file);
    LSBFirstWriteLong(bmp_header.x_pixels,image->file);
    LSBFirstWriteLong(bmp_header.y_pixels,image->file);
    LSBFirstWriteLong(bmp_header.number_colors,image->file);
    LSBFirstWriteLong(bmp_header.colors_important,image->file);
    if (image->class == PseudoClass)
      {
        unsigned char
          *bmp_colormap;

        /*
          Dump colormap to file.
        */
        bmp_colormap=(unsigned char *)
          AllocateMemory(4*(1 << bmp_header.bit_count)*sizeof(unsigned char));
        if (bmp_colormap == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        q=bmp_colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          *q++=DownScale(image->colormap[i].blue);
          *q++=DownScale(image->colormap[i].green);
          *q++=DownScale(image->colormap[i].red);
          *q++=(Quantum) 0x0;
        }
        for ( ; i < (int) (1 << bmp_header.bit_count); i++)
        {
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
          *q++=(Quantum) 0x0;
        }
        (void) fwrite((char *) bmp_colormap,4,1 << bmp_header.bit_count,
          image->file);
        FreeMemory((char *) bmp_colormap);
      }
    (void) fwrite((char *) bmp_pixels,1,(int) bmp_header.image_size,
      image->file);
    FreeMemory((char *) bmp_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C M Y K I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCMYKImage writes an image to a file in cyan, magenta,
%  yellow, and black rasterfile format.
%
%  The format of the WriteCMYKImage routine is:
%
%      status=WriteCMYKImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteCMYKImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteCMYKImage(const ImageInfo *image_info,Image *image)
{
  double
    black_generation,
    undercolor;

  int
    black,
    cyan,
    magenta,
    yellow,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    scene;

  image->depth=QuantumDepth;
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
    }
  /*
    Convert MIFF to CMYK raster pixels.
  */
  undercolor=1.0;
  black_generation=1.0;
  if (image_info->undercolor != (char *) NULL)
    {
      (void) sscanf(image_info->undercolor,"%lfx%lf",&undercolor,
        &black_generation);
      if (black_generation == 1.0)
        black_generation=undercolor;
    }
  scene=0;
  do
  {
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        register unsigned char
          *q;

        unsigned char
          *pixels;

        unsigned short
          value;

        /*
          Allocate memory for pixels.
        */
        pixels=(unsigned char *)
          AllocateMemory(image->columns*sizeof(RunlengthPacket));
        if (pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        /*
          No interlacing:  CMYKCMYKCMYKCMYKCMYKCMYK...
        */
        x=0;
        y=0;
        p=image->pixels;
        q=pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum((unsigned int) (cyan-undercolor*black),q);
            WriteQuantum((unsigned int) (magenta-undercolor*black),q);
            WriteQuantum((unsigned int) (yellow-undercolor*black),q);
            WriteQuantum((unsigned int) (black_generation*black),q);
            x++;
            if (x == (int) image->columns)
              {
                (void) fwrite((char *) pixels,1,q-pixels,image->file);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
        FreeMemory((char *) pixels);
        break;
      }
      case LineInterlace:
      {
        register int
          x,
          y;

        /*
          Line interlacing:  CCC...MMM...YYY...CCC...MMM...YYY...
        */
        if (!UncondenseImage(image))
          return(False);
        for (y=0; y < (int) image->rows; y++)
        {
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            cyan=MaxRGB-p->red;
            magenta=MaxRGB-p->green;
            yellow=MaxRGB-p->blue;
            black=cyan;
            if (magenta < black)
              black=magenta;
            if (yellow < black)
              black=yellow;
            WriteQuantumFile((unsigned int) (cyan-undercolor*black));
            p++;
          }
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            cyan=MaxRGB-p->red;
            magenta=MaxRGB-p->green;
            yellow=MaxRGB-p->blue;
            black=cyan;
            if (magenta < black)
              black=magenta;
            if (yellow < black)
              black=yellow;
            WriteQuantumFile((unsigned int) (magenta-undercolor*black));
            p++;
          }
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            cyan=MaxRGB-p->red;
            magenta=MaxRGB-p->green;
            yellow=MaxRGB-p->blue;
            black=cyan;
            if (magenta < black)
              black=magenta;
            if (yellow < black)
              black=yellow;
            WriteQuantumFile((unsigned int) (yellow-undercolor*black));
            p++;
          }
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            cyan=MaxRGB-p->red;
            magenta=MaxRGB-p->green;
            yellow=MaxRGB-p->blue;
            black=cyan;
            if (magenta < black)
              black=magenta;
            if (yellow < black)
              black=yellow;
            WriteQuantumFile((unsigned int) (black_generation*black));
            p++;
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
        }
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
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile((unsigned int) (cyan-undercolor*black));
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("M",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        ProgressMonitor(SaveImageText,100,400);
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile((unsigned int) (magenta-undercolor*black));
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("Y",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        ProgressMonitor(SaveImageText,200,400);
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile((unsigned int) (yellow-undercolor*black));
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("K",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        ProgressMonitor(SaveImageText,300,400);
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile((unsigned int) (black_generation*black));
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        ProgressMonitor(SaveImageText,400,400);
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e E P T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteEPTImage writes an image in the Adobe Encapsulated Postscript
%  format with a TIFF preview.
%
%  The format of the WriteEPTImage routine is:
%
%      status=WriteEPTImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteEPTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteEPTImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent];

  FILE
    *ps_file,
    *tiff_file;

  int
    c;

  unsigned int
    status;

  unsigned long
    eps_length,
    tiff_length;

  ps_file=(FILE *) NULL;
  if (Latin1Compare(image_info->magick,"PS") == 0)
    ps_file=fopen(image->magick_filename,ReadBinaryType);
  if (ps_file != (FILE *) NULL)
    {
      /*
        Read existing Encapsulated Postscript.
      */
      (void) fseek(ps_file,0L,SEEK_END);
      eps_length=ftell(ps_file);
      (void) fseek(ps_file,0L,SEEK_SET);
    }
  else
    {
      /*
        Write image as Encapsulated Postscript to a temporary file.
      */
      (void) strcpy(filename,image->filename);
      TemporaryFilename(image->filename);
      status=WritePSImage(image_info,image);
      if (status == False)
        return(status);
      OpenImage(image_info,image,ReadBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      (void) remove(image->filename);
      eps_length=image->filesize;
      ps_file=image->file;
      image->file=(FILE *) NULL;
    }
  /*
    Write image as TIFF to a temporary file.
  */
  TemporaryFilename(image->filename);
  status=WriteTIFFImage(image_info,image);
  if (status == False)
    return(status);
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) remove(image->filename);
  tiff_length=image->filesize;
  tiff_file=image->file;
  image->file=(FILE *) NULL;
  /*
    Write EPT image.
  */
  (void) strcpy(image->filename,filename);
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  LSBFirstWriteLong(0xc6d3d0c5ul,image->file);
  LSBFirstWriteLong(30,image->file);
  LSBFirstWriteLong(eps_length,image->file);
  LSBFirstWriteLong(0,image->file);
  LSBFirstWriteLong(0,image->file);
  LSBFirstWriteLong(eps_length+30,image->file);
  LSBFirstWriteLong(tiff_length,image->file);
  LSBFirstWriteShort(0xffff,image->file);
  for (c=fgetc(ps_file); c != EOF; c=fgetc(ps_file))
    (void) fputc((char) c,image->file);
  for (c=fgetc(tiff_file); c != EOF; c=fgetc(tiff_file))
    (void) fputc((char) c,image->file);
  (void) fclose(tiff_file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F A X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteFAXImage writes an image to a file in 1 dimensional Huffman
%  encoded format.
%
%  The format of the WriteFAXImage routine is:
%
%      status=WriteFAXImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFAXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteFAXImage(const ImageInfo *image_info,Image *image)
{
  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) strcpy((char *) image_info->magick,"FAX");
  scene=0;
  do
  {
    /*
      Convert MIFF to monochrome.
    */
    if (!IsMonochromeImage(image))
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=2;
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,image);
        SyncImage(image);
      }
    status=HuffmanEncodeImage((ImageInfo *) image_info,image);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F I T S I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteFITSImage writes a Flexible Image Transport System image to a
%  file as gray scale intensities [0..255].
%
%  The format of the WriteFITSImage routine is:
%
%      status=WriteFITSImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFITSImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteFITSImage(const ImageInfo *image_info,Image *image)
{
  char
    buffer[81],
    *fits_header;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  if (!UncondenseImage(image))
    return(False);
  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Allocate image memory.
  */
  fits_header=(char *) AllocateMemory(2880*sizeof(unsigned char));
  pixels=(unsigned char *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if ((fits_header == (char *) NULL) || (pixels == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize image header.
  */
  for (i=0; i < 2880; i++)
    fits_header[i]=' ';
  (void) strcpy(buffer,"SIMPLE  =                    T");
  (void) strncpy(fits_header+0,buffer,Extent(buffer));
  (void) strcpy(buffer,"BITPIX  =                    8");
  (void) strncpy(fits_header+80,buffer,Extent(buffer));
  (void) strcpy(buffer,"NAXIS   =                    2");
  (void) strncpy(fits_header+160,buffer,Extent(buffer));
  FormatString(buffer,"NAXIS1  =           %10u",image->columns);
  (void) strncpy(fits_header+240,buffer,Extent(buffer));
  FormatString(buffer,"NAXIS2  =           %10u",image->rows);
  (void) strncpy(fits_header+320,buffer,Extent(buffer));
  (void) strcpy(buffer,"HISTORY Created by ImageMagick.");
  (void) strncpy(fits_header+400,buffer,Extent(buffer));
  (void) strcpy(buffer,"END");
  (void) strncpy(fits_header+480,buffer,Extent(buffer));
  (void) fwrite((char *) fits_header,1,2880,image->file);
  FreeMemory((char *) fits_header);
  /*
    Convert image to fits scale PseudoColor class.
  */
  for (y=image->rows-1; y >= 0; y--)
  {
    p=image->pixels+(y*image->columns);
    q=pixels;
    for (x=0; x < (int) image->columns; x++)
    {
      *q++=DownScale(Intensity(*p));
      p++;
    }
    (void) fwrite((char *) pixels,1,q-pixels,image->file);
    if (QuantumTick(image->rows-y-1,image->rows))
      ProgressMonitor(SaveImageText,image->rows-y-1,image->rows);
  }
  FreeMemory((char *) pixels);
  CloseImage(image);
  return(True);
}

#if defined(HasFPX)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F P X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteFPXImage writes an image in the FlashPix image format.  This
%  routine was contributed by BillR@corbis.com.
%
%  The format of the WriteFPXImage routine is:
%
%      status=WriteFPXImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteFPXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static void ColorTwistMultiply(FPXColorTwistMatrix first,
  FPXColorTwistMatrix second,FPXColorTwistMatrix *color_twist)
{
  /*
    Matrix multiply.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  color_twist->byy=(first.byy*second.byy)+(first.byc1*second.bc1y)+(first.byc2*
    second.bc2y)+(first.dummy1_zero*second.dummy4_zero);
  color_twist->byc1=(first.byy*second.byc1)+(first.byc1*second.bc1c1)+
    (first.byc2*second.bc2c1)+(first.dummy1_zero*second.dummy5_zero);
  color_twist->byc2=(first.byy*second.byc2)+(first.byc1*second.bc1c2)+
    (first.byc2*second.bc2c2)+(first.dummy1_zero*second.dummy6_zero);
  color_twist->dummy1_zero=(first.byy*second.dummy1_zero)+(first.byc1*
    second.dummy2_zero)+(first.byc2*second.dummy3_zero)+(first.dummy1_zero*
    second.dummy7_one);
  color_twist->bc1y=(first.bc1y*second.byy)+(first.bc1c1*second.bc1y)+
    (first.bc1c2*second.bc2y)+(first.dummy2_zero*second.dummy4_zero);
  color_twist->bc1c1=(first.bc1y*second.byc1)+(first.bc1c1*second.bc1c1)+
    (first.bc1c2*second.bc2c1)+(first.dummy2_zero*second.dummy5_zero);
  color_twist->bc1c2=(first.bc1y*second.byc2)+(first.bc1c1*second.bc1c2)+
    (first.bc1c2*second.bc2c2)+(first.dummy2_zero*second.dummy6_zero);
  color_twist->dummy2_zero=(first.bc1y*second.dummy1_zero)+(first.bc1c1*
    second.dummy2_zero)+(first.bc1c2*second.dummy3_zero)+(first.dummy2_zero*
    second.dummy7_one);
  color_twist->bc2y=(first.bc2y*second.byy)+(first.bc2c1*second.bc1y)+
    (first.bc2c2*second.bc2y)+(first.dummy3_zero*second.dummy4_zero);
  color_twist->bc2c1=(first.bc2y*second.byc1)+(first.bc2c1*second.bc1c1)+
    (first.bc2c2*second.bc2c1)+(first.dummy3_zero*second.dummy5_zero);
  color_twist->bc2c2=(first.bc2y*second.byc2)+(first.bc2c1*second.bc1c2)+
    (first.bc2c2*second.bc2c2)+(first.dummy3_zero*second.dummy6_zero);
  color_twist->dummy3_zero=(first.bc2y*second.dummy1_zero)+(first.bc2c1*
    second.dummy2_zero)+(first.bc2c2*second.dummy3_zero)+(first.dummy3_zero*
    second.dummy7_one);
  color_twist->dummy4_zero=(first.dummy4_zero*second.byy)+(first.dummy5_zero*
    second.bc1y)+(first.dummy6_zero*second.bc2y)+(first.dummy7_one*
    second.dummy4_zero);
  color_twist->dummy5_zero=(first.dummy4_zero*second.byc1)+(first.dummy5_zero*
    second.bc1c1)+(first.dummy6_zero*second.bc2c1)+(first.dummy7_one*
    second.dummy5_zero);
  color_twist->dummy6_zero=(first.dummy4_zero*second.byc2)+(first.dummy5_zero*
    second.bc1c2)+(first.dummy6_zero*second.bc2c2)+(first.dummy7_one*
    second.dummy6_zero);
  color_twist->dummy7_one=(first.dummy4_zero*second.dummy1_zero)+
    (first.dummy5_zero*second.dummy2_zero)+(first.dummy6_zero*
    second.dummy3_zero)+(first.dummy7_one*second.dummy7_one);
}

static void SetBrightness(double brightness,FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    effect,
    result;

  /*
    Set image brightness in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  brightness=sqrt((double) brightness);
  effect.byy=brightness;
  effect.byc1=0.0;
  effect.byc2=0.0;
  effect.dummy1_zero=0.0;
  effect.bc1y=0.0;
  effect.bc1c1=brightness;
  effect.bc1c2=0.0;
  effect.dummy2_zero=0.0;
  effect.bc2y=0.0;
  effect.bc2c1=0.0;
  effect.bc2c2=brightness;
  effect.dummy3_zero=0.0;
  effect.dummy4_zero=0.0;
  effect.dummy5_zero=0.0;
  effect.dummy6_zero=0.0;
  effect.dummy7_one=1.0;
  ColorTwistMultiply(*color_twist,effect,&result);
  *color_twist=result;
}

static void SetColorBalance(double red,double green,double blue,
  FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    blue_effect,
    green_effect,
    result,
    rgb_effect,
    rg_effect,
    red_effect;

  /*
    Set image color balance in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  red=sqrt((double) red)-1.0;
  green=sqrt((double) green)-1.0;
  blue=sqrt((double) blue)-1.0;
  red_effect.byy=1.0;
  red_effect.byc1=0.0;
  red_effect.byc2=0.299*red;
  red_effect.dummy1_zero=0.0;
  red_effect.bc1y=(-0.299)*red;
  red_effect.bc1c1=1.0-0.299*red;
  red_effect.bc1c2=(-0.299)*red;
  red_effect.dummy2_zero=0.0;
  red_effect.bc2y=0.701*red;
  red_effect.bc2c1=0.0;
  red_effect.bc2c2=1.0+0.402*red;
  red_effect.dummy3_zero=0.0;
  red_effect.dummy4_zero=0.0;
  red_effect.dummy5_zero=0.0;
  red_effect.dummy6_zero=0.0;
  red_effect.dummy7_one=1.0;
  green_effect.byy=1.0;
  green_effect.byc1=(-0.114)*green;
  green_effect.byc2=(-0.299)*green;
  green_effect.dummy1_zero=0.0;
  green_effect.bc1y=(-0.587)*green;
  green_effect.bc1c1=1.0-0.473*green;
  green_effect.bc1c2=0.299*green;
  green_effect.dummy2_zero=0.0;
  green_effect.bc2y=(-0.587)*green;
  green_effect.bc2c1=0.114*green;
  green_effect.bc2c2=1.0-0.288*green;
  green_effect.dummy3_zero=0.0;
  green_effect.dummy4_zero=0.0;
  green_effect.dummy5_zero=0.0;
  green_effect.dummy6_zero=0.0;
  green_effect.dummy7_one=1.0;
  blue_effect.byy=1.0;
  blue_effect.byc1=0.114*blue;
  blue_effect.byc2=0.0;
  blue_effect.dummy1_zero=0.0;
  blue_effect.bc1y=0.886*blue;
  blue_effect.bc1c1=1.0+0.772*blue;
  blue_effect.bc1c2=0.0;
  blue_effect.dummy2_zero=0.0;
  blue_effect.bc2y=(-0.114)*blue;
  blue_effect.bc2c1=(-0.114)*blue;
  blue_effect.bc2c2=1.0-0.114*blue;
  blue_effect.dummy3_zero=0.0;
  blue_effect.dummy4_zero=0.0;
  blue_effect.dummy5_zero=0.0;
  blue_effect.dummy6_zero=0.0;
  blue_effect.dummy7_one=1.0;
  ColorTwistMultiply(red_effect,green_effect,&rg_effect);
  ColorTwistMultiply(rg_effect,blue_effect,&rgb_effect);
  ColorTwistMultiply(*color_twist,rgb_effect,&result);
  *color_twist=result;
}

static void SetSaturation(double saturation,FPXColorTwistMatrix *color_twist)
{
  FPXColorTwistMatrix
    effect,
    result;

  /*
    Set image saturation in color twist matrix.
  */
  assert(color_twist != (FPXColorTwistMatrix *) NULL);
  effect.byy=1.0;
  effect.byc1=0.0;
  effect.byc2=0.0;
  effect.dummy1_zero=0.0;
  effect.bc1y=0.0;
  effect.bc1c1=saturation;
  effect.bc1c2=0.0;
  effect.dummy2_zero=0.0;
  effect.bc2y=0.0;
  effect.bc2c1=0.0;
  effect.bc2c2=saturation;
  effect.dummy3_zero=0.0;
  effect.dummy4_zero=0.0;
  effect.dummy5_zero=0.0;
  effect.dummy6_zero=0.0;
  effect.dummy7_one=1.0;
  ColorTwistMultiply(*color_twist,effect,&result);
  *color_twist=result;
}

unsigned int WriteFPXImage(const ImageInfo *image_info,Image *image)
{
  FPXBackground
    background_color;

  FPXColorspace
    colorspace =
    {
      TRUE, 4,
      NIFRGB_R, DATA_TYPE_UNSIGNED_BYTE,
      NIFRGB_G, DATA_TYPE_UNSIGNED_BYTE,
      NIFRGB_B, DATA_TYPE_UNSIGNED_BYTE,
      ALPHA, DATA_TYPE_UNSIGNED_BYTE
    };

  FPXCompressionOption
    compression;

  FPXImageDesc
    fpx_info;

  FPXImageHandle
    *flashpix;

  FPXStatus
    status;

  FPXSummaryInformation
    summary_info;

  Image
    fpx_image;

  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned long
    memory_limit,
    tile_height,
    tile_width;

  unsigned short
    value;

  /*
    Open input file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file != stdout) && !image->pipe)
    (void) remove(image->filename);
  else
    {
      /*
        Write standard output or pipe to temporary file.
      */
      fpx_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
    }
  CloseImage(image);
  /*
    Initialize FPX toolkit.
  */
  memory_limit=20000000;
  status=FPX_SetToolkitMemoryLimit(&memory_limit);
  if (status != FPX_OK)
    PrematureExit(ResourceLimitWarning,"Unable to initialize FPX library",
      image);
  tile_width=64;
  tile_height=64;
  colorspace.numberOfComponents=3;
  if (image->matte)
    colorspace.numberOfComponents=4;
  if (IsGrayImage(image))
    {
      colorspace.numberOfComponents=1;
      colorspace.theComponents[0].myColor=MONOCHROME;
    }
  background_color.color1_value=0;
  background_color.color2_value=0;
  background_color.color3_value=0;
  background_color.color4_value=0;
  compression=NONE;
  if (image_info->compression == JPEGCompression)
    compression=JPEG_UNSPECIFIED;
  {
#if defined(macintosh)
    FSSpec
      fsspec;

    FilenameToFSSpec(image->filename,&fsspec);
    status=FPX_CreateImageByFilename((const FSSpec &) fsspec,
#else
    status=FPX_CreateImageByFilename(image->filename,
#endif
    image->columns,image->rows,tile_width,tile_height,colorspace,
    background_color,compression,&flashpix);
  }
  if (status != FPX_OK)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if (image_info->compression == JPEGCompression)
    {
      /*
        Initialize the compression by quality for the entire image.
      */
      status=
        FPX_SetJPEGCompression(flashpix,(unsigned short) (image_info->quality));
      if (status != FPX_OK)
        MagickWarning(DelegateWarning,"Unable to set JPEG level",(char *) NULL);
    }
  /*
    Set image summary info.
  */
  summary_info.subject_valid=False;
  summary_info.author_valid=False;
  summary_info.keywords_valid=False;
  summary_info.OLEtemplate_valid=False;
  summary_info.last_author_valid=False;
  summary_info.rev_number_valid=False;
  summary_info.edit_time_valid=False;
  summary_info.last_printed_valid=False;
  summary_info.create_dtm_valid=False;
  summary_info.last_save_dtm_valid=False;
  summary_info.page_count_valid=False;
  summary_info.word_count_valid=False;
  summary_info.char_count_valid=False;
  summary_info.thumbnail_valid=False;
  summary_info.appname_valid=False;
  summary_info.security_valid=False;
  if (image->label != (char *) NULL)
    {
      /*
        Note image label.
      */
      summary_info.title_valid=True;
      summary_info.title.length=strlen(image->label);
      summary_info.title.ptr=(unsigned char *)
        AllocateMemory((strlen(image->label)+1)*sizeof(unsigned char));
      if (summary_info.title.ptr != (unsigned char *) NULL)
        (void) strcpy((char *) summary_info.title.ptr,image->label);
      else
        MagickWarning(DelegateWarning,"Unable to set image title",
          (char *) NULL);
    }
  if (image->comments != (char *) NULL)
    {
      /*
        Note image comment.
      */
      summary_info.comments_valid=True;
      summary_info.comments.length=strlen(image->comments);
      summary_info.comments.ptr=(unsigned char *)
        AllocateMemory((strlen(image->comments)+1)*sizeof(unsigned char));
      if (summary_info.comments.ptr != (unsigned char *) NULL)
        (void) strcpy((char *) summary_info.comments.ptr,image->comments);
      else
        MagickWarning(DelegateWarning,"Unable to set image comments",
          (char *) NULL);
    }
  status=FPX_SetSummaryInformation(flashpix,&summary_info);
  if (status != FPX_OK)
    MagickWarning(DelegateWarning,"Unable to set summary info",(char *) NULL);
  /*
    Allocate pixels.
  */
  pixels=(unsigned char *) AllocateMemory(colorspace.numberOfComponents*
    image->columns*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      (void) FPX_CloseImage(flashpix);
      FPX_ClearSystem();
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  /*
    Initialize FlashPix image description.
  */
  fpx_info.numberOfComponents=colorspace.numberOfComponents;
  for (i=0; i < fpx_info.numberOfComponents; i++)
  {
    fpx_info.components[i].myColorType.myDataType=DATA_TYPE_UNSIGNED_BYTE;
    fpx_info.components[i].horzSubSampFactor=1;
    fpx_info.components[i].vertSubSampFactor=1;
    fpx_info.components[i].columnStride=
      fpx_info.numberOfComponents*sizeof(unsigned char);
    fpx_info.components[i].lineStride=
      image->columns*fpx_info.components[i].columnStride;
    fpx_info.components[i].theData=pixels+i;
  }
  fpx_info.components[0].myColorType.myColor=
    fpx_info.numberOfComponents != 1 ? NIFRGB_R : MONOCHROME;
  fpx_info.components[1].myColorType.myColor=NIFRGB_G;
  fpx_info.components[2].myColorType.myColor=NIFRGB_B;
  fpx_info.components[3].myColorType.myColor=ALPHA;
  /*
    Write image scanlines.
  */
  x=0;
  y=0;
  p=image->pixels;
  q=pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= (int) p->length; j++)
    {
      if (fpx_info.numberOfComponents == 1)
        WriteQuantum(Intensity(*p),q)
      else
        {
          WriteQuantum(p->red,q);
          WriteQuantum(p->green,q);
          WriteQuantum(p->blue,q);
          if (image->matte)
            WriteQuantum(p->index,q);
        }
      x++;
      if (x == (int) image->columns)
        {
          status=FPX_WriteImageLine(flashpix,&fpx_info);
          if (status != FPX_OK)
            break;
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
          q=pixels;
          x=0;
          y++;
        }
    }
    p++;
  }
  if (image_info->view != (char *) NULL)
    {
      FPXAffineMatrix
        affine;

      FPXColorTwistMatrix
        color_twist;

      FPXContrastAdjustment
        contrast;

      FPXFilteringValue
        sharpen;

      FPXResultAspectRatio
        aspect_ratio;

      FPXROI
        view_rect;

      unsigned int
        affine_valid,
        aspect_ratio_valid,
        color_twist_valid,
        contrast_valid,
        sharpen_valid,
        view_rect_valid;

      /*
        Initialize default viewing parameters.
      */
      contrast=1.0;
      contrast_valid=False;
      color_twist.byy=1.0;
      color_twist.byc1=0.0;
      color_twist.byc2=0.0;
      color_twist.dummy1_zero=0.0;
      color_twist.bc1y=0.0;
      color_twist.bc1c1=1.0;
      color_twist.bc1c2=0.0;
      color_twist.dummy2_zero=0.0;
      color_twist.bc2y=0.0;
      color_twist.bc2c1=0.0;
      color_twist.bc2c2=1.0;
      color_twist.dummy3_zero=0.0;
      color_twist.dummy4_zero=0.0;
      color_twist.dummy5_zero=0.0;
      color_twist.dummy6_zero=0.0;
      color_twist.dummy7_one=1.0;
      color_twist_valid=False;
      sharpen=0.0;
      sharpen_valid=False;
      aspect_ratio=(double) image->columns/image->rows;
      aspect_ratio_valid=False;
      view_rect.left=0.1;
      view_rect.width=aspect_ratio-0.2;
      view_rect.top=0.1;
      view_rect.height=1.0-0.2;
      view_rect_valid=False;
      affine.a11=1.0;
      affine.a12=0.0;
      affine.a13=0.0;
      affine.a14=0.0;
      affine.a21=0.0;
      affine.a22=1.0;
      affine.a23=0.0;
      affine.a24=0.0;
      affine.a31=0.0;
      affine.a32=0.0;
      affine.a33=1.0;
      affine.a34=0.0;
      affine.a41=0.0;
      affine.a42=0.0;
      affine.a43=0.0;
      affine.a44=1.0;
      affine_valid=False;
      if (0)
        {
          /*
            Color color_twist.
          */
          SetBrightness(0.5,&color_twist);
          SetSaturation(0.5,&color_twist);
          SetColorBalance(0.5,1.0,1.0,&color_twist);
          color_twist_valid=True;
        }
      if (affine_valid)
        {
          status=FPX_SetImageAffineMatrix(flashpix,&affine);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set affine matrix",
              (char *) NULL);
        }
      if (aspect_ratio_valid)
        {
          status=FPX_SetImageResultAspectRatio(flashpix,&aspect_ratio);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set aspect ratio",
              (char *) NULL);
        }
      if (color_twist_valid)
        {
          status=FPX_SetImageColorTwistMatrix(flashpix,&color_twist);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set color color twist",
              (char *) NULL);
        }
      if (contrast_valid)
        {
          status=FPX_SetImageContrastAdjustment(flashpix,&contrast);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set contrast",
              (char *) NULL);
        }
      if (sharpen_valid)
        {
          status=FPX_SetImageFilteringValue(flashpix,&sharpen);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set filtering value",
              (char *) NULL);
        }
      if (view_rect_valid)
        {
          status=FPX_SetImageROI(flashpix, &view_rect);
          if (status != FPX_OK)
            MagickWarning(DelegateWarning,"Unable to set region of interest",
              (char *) NULL);
        }
    }
  (void) FPX_CloseImage(flashpix);
  FPX_ClearSystem();
  FreeMemory((char *) pixels);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) putc(c,fpx_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseImage(&fpx_image);
    }
  return(True);
}
#else
unsigned int WriteFPXImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"FPX library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G I F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGIFImage writes an image to a file in the Compuserve Graphics
%  image format.
%
%  The format of the WriteGIFImage routine is:
%
%      status=WriteGIFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGIFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteGIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *next_image;

  RectangleInfo
    page_info;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    bits_per_pixel,
    c,
    *colormap,
    *global_colormap,
    *matte;

  unsigned int
    colors,
    height,
    interlace,
    scene,
    status,
    transparency,
    width;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Determine image bounding box.
  */
  page_info.width=image->columns;
  page_info.height=image->rows;
  page_info.x=0;
  page_info.y=0;
  next_image=image;
  for (next_image=image; next_image != (Image *) NULL; )
  {
    (void) XParseGeometry(next_image->page,&page_info.x,&page_info.y,
      &width,&height);
    if ((next_image->columns+page_info.x) > page_info.width)
      page_info.width=next_image->columns+page_info.x;
    if ((next_image->rows+page_info.y) > page_info.height)
      page_info.height=next_image->rows+page_info.y;
    next_image=next_image->next;
  }
  /*
    Allocate colormap.
  */
  matte=(unsigned char *)
    AllocateMemory(page_info.width*page_info.height*sizeof(unsigned char));
  global_colormap=(unsigned char *) AllocateMemory(768*sizeof(unsigned char));
  colormap=(unsigned char *) AllocateMemory(768*sizeof(unsigned char));
  if ((matte == (unsigned char *) NULL) ||
      (global_colormap == (unsigned char *) NULL) ||
      (colormap == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=0; i < 768; i++)
    colormap[i]=0;
  /*
    Write GIF header.
  */
  if ((image->comments == (char *) NULL) && !image_info->adjoin &&
      !image->matte)
    (void) fwrite("GIF87a",1,6,image->file);
  else
    if (Latin1Compare(image_info->magick,"GIF87") == 0)
      (void) fwrite("GIF87a",1,6,image->file);
    else
      (void) fwrite("GIF89a",1,6,image->file);
  if (image_info->page != (char *) NULL)
    (void) XParseGeometry(image_info->page,&page_info.x,&page_info.y,
      &page_info.width,&page_info.height);
  LSBFirstWriteShort(page_info.width,image->file);
  LSBFirstWriteShort(page_info.height,image->file);
  /*
    Write images to file.
  */
  interlace=image_info->interlace;
  if (image_info->adjoin && (image->next != (Image *) NULL))
    interlace=NoInterlace;
  scene=0;
  do
  {
    transparency=False;
    if (IsPseudoClass(image))
      colors=image->colors;
    else
      {
        QuantizeInfo
          quantize_info;

        if (image->matte)
          {
            /*
              Track all the transparent pixels.
            */
            if (!UncondenseImage(image))
              return(False);
            p=image->pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              matte[i]=p->index == Transparent;
              if (p->index == Transparent)
                transparency=True;
              p++;
            }
          }
        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=transparency ? 255 : 256;
        quantize_info.dither=image_info->dither;
        (void) QuantizeImage(&quantize_info,image);
        SyncImage(image);
        colors=image->colors;
        if (transparency)
          {
            /*
              Set the transparent pixel index.
            */
            if (!UncondenseImage(image))
              return(False);
            p=image->pixels;
            image->class=DirectClass;
            image->matte=True;
            for (i=0; i < (int) image->packets; i++)
            {
              if (matte[i])
                p->index=image->colors;
              p++;
            }
            colors++;
          }
      }
    for (bits_per_pixel=1; bits_per_pixel < 8; bits_per_pixel++)
      if ((1 << bits_per_pixel) >= (int) colors)
        break;
    q=colormap;
    for (i=0; i < (int) image->colors; i++)
    {
      *q++=DownScale(image->colormap[i].red);
      *q++=DownScale(image->colormap[i].green);
      *q++=DownScale(image->colormap[i].blue);
    }
    if (transparency)
      {
        *q++=DownScale(image->background_color.red);
        *q++=DownScale(image->background_color.green);
        *q++=DownScale(image->background_color.blue);
        i++;
      }
    for ( ; i < (int) (1 << bits_per_pixel); i++)
    {
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
      *q++=(Quantum) 0x0;
    }
    if ((image->previous == (Image *) NULL) || !image_info->adjoin)
      {
        /*
          Write global colormap.
        */
        c=0x80;
        c|=(8-1) << 4;  /* color resolution */
        c|=(bits_per_pixel-1);   /* size of global colormap */
        (void) fputc((char) c,image->file);
        for (j=0; j < (int) (image->colors-1); j++)
          if (ColorMatch(image->background_color,image->colormap[j],0))
            break;
        (void) fputc(j,image->file);  /* background color */
        (void) fputc(0x0,image->file);  /* reserved */
        (void) fwrite(colormap,1,3*(1 << bits_per_pixel),image->file);
        for (j=0; j < 768; j++)
          global_colormap[j]=colormap[j];
      }
    if (Latin1Compare(image_info->magick,"GIF87") != 0)
      {
        /*
          Write Graphics Control extension.
        */
        (void) fputc(0x21,image->file);
        (void) fputc(0xf9,image->file);
        (void) fputc(0x04,image->file);
        c=image->dispose << 2;
        if (transparency)
          c|=0x01;
        (void) fputc(c,image->file);
        LSBFirstWriteShort(image->delay,image->file);
        (void) fputc((char) image->colors,image->file);
        (void) fputc(0x00,image->file);
        if (image->comments != (char *) NULL)
          {
            register char
              *p;

            register unsigned int
              count;

            /*
              Write Comment extension.
            */
            (void) fputc(0x21,image->file);
            (void) fputc(0xfe,image->file);
            p=image->comments;
            while (Extent(p) > 0)
            {
              count=Min(Extent(p),255);
              (void) fputc(count,image->file);
              for (i=0; i < (int) count; i++)
                (void) fputc(*p++,image->file);
            }
            (void) fputc(0x0,image->file);
          }
        if ((image->previous == (Image *) NULL) &&
            (image->next != (Image *) NULL) && (image->iterations != 1))
          {
            /*
              Write Netscape Loop extension.
            */
            (void) fputc(0x21,image->file);
            (void) fputc(0xff,image->file);
            (void) fputc(0x0b,image->file);
            (void) fwrite("NETSCAPE2.0",1,11,image->file);
            (void) fputc(0x03,image->file);
            (void) fputc(0x01,image->file);
            LSBFirstWriteShort(image->iterations,image->file);
            (void) fputc(0x00,image->file);
          }
      }
    (void) fputc(',',image->file);  /* image separator */
    /*
      Write the image header.
    */
    if (image->page != (char *) NULL)
      (void) XParseGeometry(image->page,&page_info.x,&page_info.y,
        &page_info.width,&page_info.height);
    LSBFirstWriteShort(page_info.x,image->file);
    LSBFirstWriteShort(page_info.y,image->file);
    LSBFirstWriteShort(image->columns,image->file);
    LSBFirstWriteShort(image->rows,image->file);
    c=0x00;
    if (interlace != NoInterlace)
      c|=0x40;  /* pixel data is interlaced */
    for (j=0; j < (int) colors; j++)
      if (colormap[j] != global_colormap[j])
        break;
    if (j == (int) colors)
      (void) fputc((char) c,image->file);
    else
      {
        c|=0x80;
        c|=(bits_per_pixel-1);   /* size of local colormap */
        (void) fputc((char) c,image->file);
        (void) fwrite(colormap,1,3*(1 << bits_per_pixel),image->file);
      }
    /*
      Write the image data.
    */
    c=Max(bits_per_pixel,2);
    (void) fputc((char) c,image->file);
    if (interlace == NoInterlace)
      status=GIFEncodeImage(image,Max(bits_per_pixel,2)+1);
    else
      {
        Image
          *interlaced_image;

        int
          pass,
          y;

        register RunlengthPacket
          *q;

        static const int
          interlace_rate[4] = { 8, 8, 4, 2 },
          interlace_start[4] = { 0, 4, 2, 1 };

        /*
          Interlace image.
        */
        if (!UncondenseImage(image))
          return(False);
        image->orphan=True;
        interlaced_image=CloneImage(image,image->columns,image->rows,False);
        image->orphan=False;
        if (interlaced_image == (Image *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        p=image->pixels;
        q=interlaced_image->pixels;
        for (pass=0; pass < 4; pass++)
        {
          y=interlace_start[pass];
          while (y < (int) image->rows)
          {
            p=image->pixels+(y*image->columns);
            for (x=0; x < (int) image->columns; x++)
            {
              *q=(*p);
              p++;
              q++;
            }
            y+=interlace_rate[pass];
          }
        }
        interlaced_image->file=image->file;
        status=GIFEncodeImage(interlaced_image,Max(bits_per_pixel,2)+1);
        interlaced_image->file=(FILE *) NULL;
        DestroyImage(interlaced_image);
      }
    if (status == False)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    (void) fputc(0x0,image->file);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  (void) fputc(';',image->file); /* terminator */
  FreeMemory((char *) global_colormap);
  FreeMemory((char *) colormap);
  FreeMemory((char *) matte);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGRAYImage writes an image to a file as gray scale intensity
%  values.
%
%  The format of the WriteGRAYImage routine is:
%
%      status=WriteGRAYImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteGRAYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    scene;

  unsigned short
    value;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Convert image to gray scale PseudoColor class.
  */
  image->depth=QuantumDepth;
  scene=0;
  do
  {
    /*
      Allocate memory for pixels.
    */
    pixels=(unsigned char *)
      AllocateMemory(image->columns*sizeof(RunlengthPacket));
    if (pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert MIFF to GRAY raster pixels.
    */
    x=0;
    y=0;
    p=image->pixels;
    q=pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        WriteQuantum(Intensity(*p),q);
        x++;
        if (x == (int) image->columns)
          {
            (void) fwrite((char *) pixels,1,q-pixels,image->file);
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            q=pixels;
            x=0;
            y++;
          }
      }
      p++;
    }
    FreeMemory((char *) pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

#if defined(HasHDF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHDFImage writes an image in the Hierarchial Data Format image
%  format.
%
%  The format of the WriteHDFImage routine is:
%
%      status=WriteHDFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteHDFImage(const ImageInfo *image_info,Image *image)
{
#include "hdf.h"
#undef BSD
#undef LOCAL

  int
    status;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  uint16
    reference;

  unsigned char
    *hdf_pixels;

  unsigned int
    compression,
    packet_size,
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  CloseImage(image);
  scene=0;
  do
  {
    /*
      Initialize raster file header.
    */
    packet_size=1;
    if (image->class == DirectClass)
      packet_size=3;
    hdf_pixels=(unsigned char *) AllocateMemory(packet_size*image->columns*
      image->rows*sizeof(unsigned char));
    if (hdf_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        /*
          Convert DirectClass packet to HDF pixels.
        */
        q=hdf_pixels;
        switch (image_info->interlace)
        {
          case NoInterlace:
          default:
          {
            /*
              No interlacing:  RGBRGBRGBRGBRGBRGB...
            */
            DF24setil(DFIL_PIXEL);
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                *q++=DownScale(p->red);
                *q++=DownScale(p->green);
                *q++=DownScale(p->blue);
              }
              p++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(SaveImageText,i,image->packets);
            }
            break;
          }
          case LineInterlace:
          {
            register int
              x,
              y;

            /*
              Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
            */
            if (!UncondenseImage(image))
              return(False);
            DF24setil(DFIL_LINE);
            for (y=0; y < (int) image->rows; y++)
            {
              p=image->pixels+(y*image->columns);
              for (x=0; x < (int) image->columns; x++)
              {
                *q++=DownScale(p->red);
                p++;
              }
              p=image->pixels+(y*image->columns);
              for (x=0; x < (int) image->columns; x++)
              {
                *q++=DownScale(p->green);
                p++;
              }
              p=image->pixels+(y*image->columns);
              for (x=0; x < (int) image->columns; x++)
              {
                *q++=DownScale(p->blue);
                p++;
              }
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            }
            break;
          }
          case PlaneInterlace:
          case PartitionInterlace:
          {
            /*
              Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
            */
            DF24setil(DFIL_PLANE);
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=DownScale(p->red);
              p++;
            }
            ProgressMonitor(SaveImageText,100,400);
            p=image->pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=DownScale(p->green);
              p++;
            }
            ProgressMonitor(SaveImageText,250,400);
            p=image->pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=DownScale(p->blue);
              p++;
            }
            ProgressMonitor(SaveImageText,400,400);
            break;
          }
        }
        if (scene == 0)
          status=DF24putimage(image->filename,(void *) hdf_pixels,
            image->columns,image->rows);
        else
          status=DF24addimage(image->filename,(void *) hdf_pixels,
            image->columns,image->rows);
        reference=DF24lastref();
      }
    else
      {
        /*
          Convert PseudoClass packet to HDF pixels.
        */
        q=hdf_pixels;
        if (IsGrayImage(image))
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
              *q++=DownScale(p->red);
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
        else
          {
            unsigned char
              *hdf_palette;

            hdf_palette=(unsigned char *)
              AllocateMemory(768*sizeof(unsigned char));
            if (hdf_palette == (unsigned char *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            q=hdf_palette;
            for (i=0; i < (int) image->colors; i++)
            {
              *q++=DownScale(image->colormap[i].red);
              *q++=DownScale(image->colormap[i].green);
              *q++=DownScale(image->colormap[i].blue);
            }
            (void) DFR8setpalette(hdf_palette);
            FreeMemory(hdf_palette);
            q=hdf_pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=p->index;
              p++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(SaveImageText,i,image->packets);
            }
          }
        compression=image_info->compression == NoCompression ? 0 : DFTAG_RLE;
        if (scene == 0)
          status=DFR8putimage(image->filename,(void *) hdf_pixels,
            image->columns,image->rows,compression);
        else
          status=DFR8addimage(image->filename,(void *) hdf_pixels,
            image->columns,image->rows,compression);
        reference=DFR8lastref();
      }
    if (image->label != (char *) NULL)
      (void) DFANputlabel(image->filename,DFTAG_RIG,reference,image->label);
    if (image->comments != (char *) NULL)
      (void) DFANputdesc(image->filename,DFTAG_RIG,reference,image->comments,
        Extent(image->comments)+1);
    FreeMemory(hdf_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  return(status != -1);
}
#else
unsigned int WriteHDFImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"HDF library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H I S T O G R A M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHISTOGRAMImage writes an image to a file in HISTOGRAM format.
%  The image shows a histogram of the color (or gray) values in the image.  The
%  image consists of three overlaid histograms:  a red one for the red channel,
%  a green one for the green channel, and a blue one for the blue channel.  The
%  image comment contains a list of unique pixel values and the number of times
%  each occurs in the image.
%
%  This routine is strongly based on a similiar one written by
%  muquit@warm.semcor.com which in turn is based on ppmhistmap of netpbm.
%
%  The format of the WriteHISTOGRAMImage routine is:
%
%      status=WriteHISTOGRAMImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHISTOGRAMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteHISTOGRAMImage(const ImageInfo *image_info,
  Image *image)
{
#define HistogramDensity  "256x200"

  char
    filename[MaxTextExtent];

  double
    scale;

  FILE
    *file;

  Image
    *histogram_image;

  int
    *blue,
    *green,
    maximum,
    *red,
    sans_offset;

  register RunlengthPacket
    *p,
    *q;

  register int
    i,
    j;

  unsigned int
    height,
    status,
    width;

  /*
    Allocate histogram image.
  */
  width=image->columns;
  height=image->rows;
  if (image_info->density != (char *) NULL)
    (void) XParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &width,&height);
  else
    (void) XParseGeometry(HistogramDensity,&sans_offset,&sans_offset,
      &width,&height);
  image->orphan=True;
  histogram_image=CloneImage(image,width,height,False);
  image->orphan=False;
  if (histogram_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  histogram_image->class=DirectClass;
  /*
    Allocate histogram count arrays.
  */
  red=(int *) AllocateMemory (histogram_image->columns*sizeof(int));
  green=(int *) AllocateMemory (histogram_image->columns*sizeof(int));
  blue=(int *) AllocateMemory (histogram_image->columns*sizeof(int));
  if ((red == (int *) NULL) || (green == (int *) NULL) ||
      (blue == (int *) NULL))
    {
      DestroyImage(histogram_image);
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  /*
    Initialize histogram count arrays.
  */
  for (i=0; i < (int) histogram_image->columns; i++)
  {
    red[i]=0;
    green[i]=0;
    blue[i]=0;
  }
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    red[DownScale(p->red)]+=(p->length+1);
    green[DownScale(p->green)]+=(p->length+1);
    blue[DownScale(p->blue)]+=(p->length+1);
    p++;
  }
  maximum=0;
  for (i=0; i < (int) histogram_image->columns; i++)
  {
    if (maximum < red[i])
      maximum=red[i];
    if (maximum < green[i])
      maximum=green[i];
    if (maximum < blue[i])
      maximum=blue[i];
  }
  for (i=0; i < (int) histogram_image->columns; i++)
  {
    if (red[i] > maximum)
      red[i]=maximum;
    if (green[i] > maximum)
      green[i]=maximum;
    if (blue[i] > maximum)
      blue[i]=maximum;
  }
  /*
    Initialize histogram image.
  */
  q=histogram_image->pixels;
  for (i=0; i < (int) histogram_image->packets; i++)
  {
    q->red=0;
    q->green=0;
    q->blue=0;
    q->index=0;
    q->length=0;
    q++;
  }
  scale=(double) histogram_image->rows/maximum;
  q=histogram_image->pixels;
  for (i=0; i < (int) histogram_image->columns; i++)
  {
    j=histogram_image->rows-(int) (scale*red[i]);
    while (j < (int) histogram_image->rows)
    {
      q=histogram_image->pixels+(j*histogram_image->columns+i);
      q->red=MaxRGB;
      j++;
    }
    j=histogram_image->rows-(int) (scale*green[i]);
    while (j < (int) histogram_image->rows)
    {
      q=histogram_image->pixels+(j*histogram_image->columns+i);
      q->green=MaxRGB;
      j++;
    }
    j=histogram_image->rows-(int) (scale*blue[i]);
    while (j < (int) histogram_image->rows)
    {
      q=histogram_image->pixels+(j*histogram_image->columns+i);
      q->blue=MaxRGB;
      j++;
    }
    if (QuantumTick(i,histogram_image->columns))
      ProgressMonitor(SaveImageText,i,histogram_image->columns);
  }
  free ((char *) blue);
  free ((char *) green);
  free ((char *) red);
  TemporaryFilename(filename);
  file=fopen(filename,WriteBinaryType);
  if (file != (FILE *) NULL)
    {
      char
        command[MaxTextExtent];

      /*
        Add a histogram as an image comment.
      */
      if (image->comments != (char *) NULL)
        (void) fprintf(file,"%s\n",image->comments);
      NumberColors(image,file);
      (void) fclose(file);
      FormatString(command,"@%.1024s",filename);
      CommentImage(histogram_image,command);
      (void) remove(filename);
    }
  /*
    Write HISTOGRAM image as MIFF.
  */
  status=WriteMIFFImage(image_info,histogram_image);
  DestroyImage(histogram_image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H T M L I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteHTMLImage writes an image in the HTML encoded image format.
%
%  The format of the WriteHTMLImage routine is:
%
%      status=WriteHTMLImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteHTMLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteHTMLImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent],
    mapname[MaxTextExtent],
    url[MaxTextExtent];

  Image
    *next;

  ImageInfo
    *local_info;

  int
    x,
    y;

  register char
    *p;

  unsigned int
    height,
    status,
    width;

  /*
    Open image.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  CloseImage(image);
  *url='\0';
  if ((Latin1Compare(image_info->magick,"FTP") == 0) ||
      (Latin1Compare(image_info->magick,"HTTP") == 0))
    {
      /*
        Extract URL base from filename.
      */
      p=strrchr(image->filename,'/');
      if (p)
        {
          p++;
          (void) strcpy(url,image_info->magick);
          (void) strcat(url,":");
          url[Extent(url)+p-image->filename]='\0';
          (void) strncat(url,image->filename,p-image->filename);
          (void) strcpy(image->filename,p);
        }
    }
  /*
    Refer to image map file.
  */
  (void) strcpy(filename,image->filename);
  AppendImageFormat("map",filename);
  (void) strcpy(mapname,BaseFilename(filename));
  (void) strcpy(image->filename,image_info->filename);
  (void) strcpy(filename,image->filename);
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    PrematureExit(FileOpenWarning,"Unable to allocate memory",image);
  local_info->adjoin=True;
  status=True;
  if (Latin1Compare(image_info->magick,"SHTML") != 0)
    {
      /*
        Open output image file.
      */
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      /*
        Write the HTML image file.
      */
      (void) fprintf(image->file,"<html version=\"2.0\">\n");
      (void) fprintf(image->file,"<head>\n");
      (void) fprintf(image->file,"<title>%.1024s</title>\n",
        image->label ? image->label : BaseFilename(image->filename));
      (void) fprintf(image->file,"</head>\n");
      (void) fprintf(image->file,"<body>\n");
      (void) fprintf(image->file,"<center>\n");
      (void) fprintf(image->file,"<h1>%.1024s</h1>\n",image->filename);
      (void) fprintf(image->file,"<br><br>\n");
      (void) strcpy(filename,image->filename);
      AppendImageFormat("gif",filename);
      (void) fprintf(image->file,
        "<img ismap usemap=#%.1024s src=\"%.1024s\" border=0>\n",
        mapname,filename);
      /*
        Determine the size and location of each image tile.
      */
      width=image->columns;
      height=image->rows;
      x=0;
      y=0;
      if (image->montage != (char *) NULL)
        (void) XParseGeometry(image->montage,&x,&y,&width,&height);
      /*
        Write an image map.
      */
      (void) fprintf(image->file,"<map name=%.1024s>\n",mapname);
      (void) fprintf(image->file,"  <area href=""%.1024s""",url);
      if (image->directory == (char *) NULL)
        (void) fprintf(image->file,"%.1024s shape=rect coords=0,0,%u,%u>\n",
          image->filename,width-1,height-1);
      else
        for (p=image->directory; *p != '\0'; p++)
          if (*p != '\n')
            (void) fputc(*p,image->file);
          else
            {
              (void) fprintf(image->file," shape=rect coords=%d,%d,%d,%d>\n",
                x,y,x+(int) width-1,y+(int) height-1);
              if (*(p+1) != '\0')
                (void) fprintf(image->file,"  <area href=""%.1024s""",url);
              x+=width;
              if (x >= (int) image->columns)
                {
                  x=0;
                  y+=height;
                }
            }
      (void) fprintf(image->file,"</map>\n");
      if (image->montage != (char *) NULL)
        {
          char
            color[MaxTextExtent];

          /*
            Make montage background transparent.
          */
          FormatString(color,HexColorFormat,
            (unsigned int) image->pixels[0].red,
            (unsigned int) image->pixels[0].green,
            (unsigned int) image->pixels[0].blue);
          TransparentImage(image,color);
        }
      (void) strcpy(filename,image->filename);
      (void) fprintf(image->file,"</center>\n");
      (void) fprintf(image->file,"</body>\n");
      status=fprintf(image->file,"</html>\n");
      CloseImage(image);
      /*
        Write the image as transparent GIF.
      */
      (void) strcpy(image->filename,filename);
      AppendImageFormat("gif",image->filename);
      next=image->next;
      image->next=(Image *) NULL;
      status|=WriteGIFImage(local_info,image);
      image->next=next;
      /*
        Determine image map filename.
      */
      (void) strcpy(image->filename,filename);
      for (p=filename+Extent(filename)-1; p > (filename+1); p--)
        if (*p == '.')
          {
            (void) strncpy(image->filename,filename,p-filename);
            image->filename[p-filename]='\0';
            break;
          }
      (void) strcat(image->filename,"_map.shtml");
    }
  /*
    Open image map.
  */
  OpenImage(local_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  DestroyImageInfo(local_info);
  /*
    Determine the size and location of each image tile.
  */
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  if (image->montage != (char *) NULL)
    (void) XParseGeometry(image->montage,&x,&y,&width,&height);
  /*
    Write an image map.
  */
  (void) fprintf(image->file,"<map name=%.1024s>\n",mapname);
  (void) fprintf(image->file,"  <area href=""%.1024s""",url);
  if (image->directory == (char *) NULL)
    (void) fprintf(image->file,"%.1024s shape=rect coords=0,0,%u,%u>\n",
      image->filename,width-1,height-1);
  else
    for (p=image->directory; *p != '\0'; p++)
      if (*p != '\n')
        (void) fputc(*p,image->file);
      else
        {
          (void) fprintf(image->file," shape=rect coords=%d,%d,%d,%d>\n",x,y,
            x+(int) width-1,y+(int) height-1);
          if (*(p+1) != '\0')
            (void) fprintf(image->file,"  <area href=""%.1024s""",url);
          x+=width;
          if (x >= (int) image->columns)
            {
              x=0;
              y+=height;
            }
        }
  (void) fprintf(image->file,"</map>\n");
  CloseImage(image);
  (void) strcpy(image->filename,filename);
  return(status);
}

#if defined(HasJBIG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J B I G I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJBIGImage writes an image in the JBIG encoded image format.
%
%  The format of the WriteJBIGImage routine is:
%
%      status=WriteJBIGImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteJBIGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static void JBIGEncode(unsigned char  *start,size_t length,void *file)
{
  (void) fwrite(start,length,1,(FILE *) file);
  return;
}

unsigned int WriteJBIGImage(const ImageInfo *image_info,Image *image)
{
  int
    sans_offset,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    *q;

  struct jbg_enc_state
    jbig_info;

  unsigned char
    *pixels,
    polarity;

  unsigned int
    byte,
    scene,
    x_resolution,
    y_resolution;

  unsigned long
    number_packets;

  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Allocate pixel data.
    */
    number_packets=((image->columns+7) >> 3)*image->rows;
    pixels=(unsigned char *)
      AllocateMemory(number_packets*sizeof(unsigned char));
    if (pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert Runlength encoded pixels to a bitmap.
    */
    if (!IsMonochromeImage(image))
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=2;
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,image);
        SyncImage(image);
      }
    polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
    if (image->colors == 2)
      polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
    bit=0;
    byte=0;
    x=0;
    y=0;
    p=image->pixels;
    q=pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        byte<<=1;
        if (p->index == polarity)
          byte|=0x01;
        bit++;
        if (bit == 8)
          {
            *q++=byte;
            bit=0;
            byte=0;
          }
        x++;
        if (x == (int) image->columns)
          {
            /*
              Advance to the next scanline.
            */
            if (bit != 0)
              *q++=byte << (8-bit);
            if (QuantumTick(y,image->rows))
              ProgressMonitor(SaveImageText,y,image->rows);
            bit=0;
            byte=0;
            x=0;
            y++;
         }
      }
      p++;
    }
    /*
      Initialize JBIG info structure.
    */
    jbg_enc_init(&jbig_info,image->columns,image->rows,1,&pixels,
      (void (*)(unsigned char *,size_t,void *)) JBIGEncode,image->file);
    x_resolution=640;
    y_resolution=480;
    (void) XParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &x_resolution,&y_resolution);
    if (image_info->subimage != 0)
      jbg_enc_layers(&jbig_info,image_info->subimage);
    else
      jbg_enc_lrlmax(&jbig_info,x_resolution,y_resolution);
    jbg_enc_lrange(&jbig_info,-1,-1);
    jbg_enc_options(&jbig_info,JBG_ILEAVE | JBG_SMID,JBG_TPDON | JBG_TPBON |
      JBG_DPON,-1,-1,-1);
    /*
      Write JBIG image.
    */
    jbg_enc_out(&jbig_info);
    jbg_enc_free(&jbig_info);
    FreeMemory((char *) pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}
#else
unsigned int WriteJBIGImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"JBIG library is not available",
    image->filename);
  return(False);
}
#endif

#if defined(HasJPEG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  W r i t e J P E G I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteJPEGImage writes a JPEG image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the WriteJPEGImage routine is:
%
%      status=WriteJPEGImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteJPEGImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o jpeg_image:  A pointer to a Image structure.
%
%
*/

static void JPEGColorProfileHandler(j_compress_ptr jpeg_info,Image *image)
{
  register int
    i,
    j;

  unsigned char
    *profile;

  unsigned int
    length;

  /*
    Save color profile as a APP marker.
  */
  for (i=0; i < (int) image->color_profile.length; i+=65519)
  {
    length=Min(image->color_profile.length-i,65519);
    profile=(unsigned char *) AllocateMemory((length+14)*sizeof(unsigned char));
    if (profile == (unsigned char *) NULL)
      break;
    (void) strcpy((char *) profile,"ICC_PROFILE");
    profile[12]=(i/65519)+1;
    profile[13]=(image->color_profile.length/65519)+1;
    for (j=0; j < (int) length; j++)
      profile[j+14]=image->color_profile.info[j];
    jpeg_write_marker(jpeg_info,ICC_MARKER,profile,(unsigned int) length+14);
    FreeMemory((char *) profile);
  }
}

static void JPEGNewsProfileHandler(j_compress_ptr jpeg_info,Image *image)
{
  register int
    i,
    j;

  unsigned char
    *profile;

  unsigned int
    length;

  /*
    Save IPTC profile as a APP marker.
  */
  for (i=0; i < (int) image->iptc_profile.length; i+=65507)
  {
    length=Min(image->iptc_profile.length-i,65507);
    profile=(unsigned char *) AllocateMemory((length+27)*sizeof(unsigned char));
    if (profile == (unsigned char *) NULL)
      break;
    (void) memcpy((char *) profile,"Photoshop 3.0 8BIM\04\04\0\0\0\0",24);
    profile[13]=0x00;
    profile[24]=length >> 8;
    profile[25]=length & 0xff;
    for (j=0; j < (int) length; j++)
      profile[j+26]=image->iptc_profile.info[j];
    profile[j+26]=0x001;
    jpeg_write_marker(jpeg_info,IPTC_MARKER,profile,(unsigned int) length+
      (length & 0x01 ? 1 : 0)+26);
    FreeMemory((char *) profile);
  }
}

static void JPEGWarningHandler(j_common_ptr jpeg_info,int level)
{
  char
    message[JMSG_LENGTH_MAX];

  struct jpeg_error_mgr
    *jpeg_error;

  jpeg_error=jpeg_info->err;
  (jpeg_error->format_message) (jpeg_info,message);
  if (level < 0)
    {
      if (jpeg_error->num_warnings == 0 || jpeg_error->trace_level >= 3)
        MagickWarning(DelegateWarning,(char *) message,(char *) NULL);
      jpeg_error->num_warnings++;
    }
  else
    if (jpeg_error->trace_level >= level)
      MagickWarning(DelegateWarning,(char *) message,(char *) NULL);
}

unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  double
    black_generation,
    undercolor;

  int
    black,
    cyan,
    magenta,
    yellow,
    x,
    y,
    flags,
    sans_offset;

  JSAMPLE
    *jpeg_pixels;

  JSAMPROW
    scanline[1];

  register int
    i,
    j;

  register JSAMPLE
    *q;

  register RunlengthPacket
    *p;

  struct jpeg_compress_struct
    jpeg_info;

  struct jpeg_error_mgr
    jpeg_error;

  unsigned int
    packets,
    x_resolution,
    y_resolution;

  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize JPEG parameters.
  */
  jpeg_info.err=jpeg_std_error(&jpeg_error);
  jpeg_info.err->emit_message=JPEGWarningHandler;
  jpeg_create_compress(&jpeg_info);
  jpeg_stdio_dest(&jpeg_info,image->file);
  jpeg_info.image_width=image->columns;
  jpeg_info.image_height=image->rows;
  jpeg_info.input_components=3;
  jpeg_info.in_color_space=JCS_RGB;
  if (Latin1Compare(image_info->magick,"JPEG24") != 0)
    if (IsGrayImage(image))
      {
        jpeg_info.input_components=1;
        jpeg_info.in_color_space=JCS_GRAYSCALE;
      }
  if (image_info->colorspace == CMYKColorspace)
    {
      jpeg_info.input_components=4;
      jpeg_info.in_color_space=JCS_CMYK;
      undercolor=1.0;
      black_generation=1.0;
      if (image_info->undercolor != (char *) NULL)
        {
          (void) sscanf(image_info->undercolor,"%lfx%lf",&undercolor,
            &black_generation);
          if (black_generation == 1.0)
            black_generation=undercolor;
        }
    }
  jpeg_set_defaults(&jpeg_info);
  flags=NoValue;
  x_resolution=72;
  y_resolution=72;
  if (image_info->density != (char *) NULL)
    flags=XParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &x_resolution,&y_resolution);
  if (flags & WidthValue)
    image->x_resolution=x_resolution;
  if (flags & HeightValue)
    image->y_resolution=y_resolution;
  jpeg_info.density_unit=1;  /* default to DPI */
  if ((image->x_resolution != 0) && (image->y_resolution != 0))
    {
      /*
        Set image resolution.
      */
      jpeg_info.X_density=(short) image->x_resolution;
      jpeg_info.Y_density=(short) image->y_resolution;
      if (image->units == PixelsPerInchResolution)
        jpeg_info.density_unit=1;
      if (image->units == PixelsPerCentimeterResolution)
        jpeg_info.density_unit=2;
    }
  CondenseImage(image);
  if (image->class == DirectClass)
    {
      if (image->packets >= ((3*image->columns*image->rows) >> 2))
        image->compression=NoCompression;
    }
  else
    if (image->packets >= ((image->columns*image->rows) >> 1))
      image->compression=NoCompression;
  if (image->compression != NoCompression)
    for (i=0; i < MAX_COMPONENTS; i++)
    {
      jpeg_info.comp_info[i].h_samp_factor=1;
      jpeg_info.comp_info[i].v_samp_factor=1;
    }
  jpeg_set_quality(&jpeg_info,image_info->quality,True);
  jpeg_info.optimize_coding=True;
#if (JPEG_LIB_VERSION >= 61)
  if (image_info->interlace != NoInterlace)
    jpeg_simple_progression(&jpeg_info);
#endif
  jpeg_start_compress(&jpeg_info,True);
  if (image->comments != (char *) NULL)
    for (i=0; i < Extent(image->comments); i+=65533)
      jpeg_write_marker(&jpeg_info,JPEG_COM,(unsigned char *) image->comments+i,
        (unsigned int) Min(Extent(image->comments+i),65533));
  if (image->color_profile.length > 0)
    JPEGColorProfileHandler(&jpeg_info,image);
  if (image->iptc_profile.length > 0)
    JPEGNewsProfileHandler(&jpeg_info,image);
  /*
    Convert MIFF to JPEG raster pixels.
  */
  packets=jpeg_info.input_components*image->columns;
  jpeg_pixels=(JSAMPLE *) AllocateMemory(packets*sizeof(JSAMPLE));
  if (jpeg_pixels == (JSAMPLE *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  x=0;
  y=0;
  p=image->pixels;
  q=jpeg_pixels;
  scanline[0]=(JSAMPROW) jpeg_pixels;
  if ((jpeg_info.data_precision > 8) && (QuantumDepth > 8))
    {
      if (jpeg_info.in_color_space == JCS_GRAYSCALE)
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=(JSAMPLE) (Intensity(*p) >> 4);
            x++;
            if (x == (int) image->columns)
              {
                (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                if (QuantumTick(y,image->rows))
                  ProgressMonitor(SaveImageText,y,image->rows);
                q=jpeg_pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
      else
        if (jpeg_info.in_color_space == JCS_RGB)
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=(JSAMPLE) (p->red >> 4);
              *q++=(JSAMPLE) (p->green >> 4);
              *q++=(JSAMPLE) (p->blue >> 4);
              x++;
              if (x == (int) image->columns)
                {
                  (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                  q=jpeg_pixels;
                  x=0;
                  y++;
                }
            }
            p++;
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
          }
        else
          for (i=0; i < (int) image->packets; i++)
          {
            cyan=MaxRGB-p->red;
            magenta=MaxRGB-p->green;
            yellow=MaxRGB-p->blue;
            black=cyan;
            if (magenta < black)
              black=magenta;
            if (yellow < black)
              black=yellow;
            for (j=0; j <= ((int) p->length); j++)
            {
              /*
                Convert DirectClass packets to contiguous RGB scanlines.
              */
              *q++=(JSAMPLE) (cyan-undercolor*black) >> 4;
              *q++=(JSAMPLE) (magenta-undercolor*black) >> 4;
              *q++=(JSAMPLE) (yellow-undercolor*black) >> 4;
              *q++=(JSAMPLE) (black_generation*black) >> 4;
              x++;
              if (x == (int) image->columns)
                {
                  (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                  q=jpeg_pixels;
                  x=0;
                  y++;
                }
            }
            p++;
          }
    }
  else
    if (jpeg_info.in_color_space == JCS_GRAYSCALE)
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          *q++=(JSAMPLE) DownScale(Intensity(*p));
          x++;
          if (x == (int) image->columns)
            {
              (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
              q=jpeg_pixels;
              x=0;
              y++;
            }
        }
        p++;
      }
    else
      if (jpeg_info.in_color_space == JCS_RGB)
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=(JSAMPLE) DownScale(p->red);
            *q++=(JSAMPLE) DownScale(p->green);
            *q++=(JSAMPLE) DownScale(p->blue);
            x++;
            if (x == (int) image->columns)
              {
                (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                if (QuantumTick(y,image->rows))
                  ProgressMonitor(SaveImageText,y,image->rows);
                q=jpeg_pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
      else
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
          {
            /*
              Convert DirectClass packets to contiguous RGB scanlines.
            */
            *q++=(JSAMPLE) DownScale(cyan-undercolor*black);
            *q++=(JSAMPLE) DownScale(magenta-undercolor*black);
            *q++=(JSAMPLE) DownScale(yellow-undercolor*black);
            *q++=(JSAMPLE) DownScale(black_generation*black);
            x++;
            if (x == (int) image->columns)
              {
                (void) jpeg_write_scanlines(&jpeg_info,scanline,1);
                if (QuantumTick(y,image->rows))
                  ProgressMonitor(SaveImageText,y,image->rows);
                q=jpeg_pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
  jpeg_finish_compress(&jpeg_info);
  /*
    Free memory.
  */
  jpeg_destroy_compress(&jpeg_info);
  FreeMemory((char *) jpeg_pixels);
  CloseImage(image);
  return(True);
}
#else
unsigned int WriteJPEGImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"JPEG library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I C C I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteICCImage writes an image in the ICC format.
%
%  The format of the WriteICCImage routine is:
%
%      status=WriteICCImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteICCImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteICCImage(const ImageInfo *image_info,Image *image)
{
  if (image->color_profile.length == 0)
    PrematureExit(FileOpenWarning,"No color profile available",image);
  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) fwrite((char *) image->color_profile.info,1,
    (int) image->color_profile.length,image->file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I P T C I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteIPTCImage writes an image in the IPTC format.
%
%  The format of the WriteIPTCImage routine is:
%
%      status=WriteIPTCImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteIPTCImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteIPTCImage(const ImageInfo *image_info,Image *image)
{
  if (image->iptc_profile.length == 0)
    PrematureExit(FileOpenWarning,"No IPTC profile available",image);
  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) fwrite((char *) image->iptc_profile.info,1,
    (int) image->iptc_profile.length,image->file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e L O G O I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteLOGOImage writes an image in the LOGO encoded image format.
%  We use GIF because it is the only format that is compressed without
%  requiring addition optional delegates (TIFF, ZIP, etc).
%
%  The format of the WriteLOGOImage routine is:
%
%      status=WriteLOGOImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteLOGOImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteLOGOImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  int
    c;

  register int
    i;

  unsigned int
    status;

  unsigned long
    filesize;

  /*
    Write logo as PseudoColor MIFF image to a temporary file.
  */
  (void) strcpy(filename,image->filename);
  TemporaryFilename(image->filename);
  status=WriteGIFImage(image_info,image);
  if (status == False)
    return(status);
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) remove(image->filename);
  filesize=image->filesize;
  file=image->file;
  /*
    Write logo image.
  */
  (void) strcpy(image->filename,filename);
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) fprintf(image->file,"/*\n");
  (void) fprintf(image->file,"  Logo image declaration.\n");
  (void) fprintf(image->file,"*/\n");
  (void) fprintf(image->file,"#define LogoImageExtent  %lu\n\n",filesize);
  (void) fprintf(image->file,"static unsigned char\n");
  (void) fprintf(image->file,"  LogoImage[]=\n");
  (void) fprintf(image->file,"  {\n");
  (void) fprintf(image->file,"    ");
  for (i=0; ; i++)
  {
    c=fgetc(file);
    if (c < 0)
      break;
    (void) fprintf(image->file,"0x%02x, ",c);
    if (((i+1) % 12) == 0)
      (void) fprintf(image->file,"\n    ");
  }
  (void) fprintf(image->file,"\n  };\n");
  (void) fclose(file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A P I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMAPImage writes an image to a file as red, green, and blue
%  colormap bytes followed by the colormap indexes.
%
%  The format of the WriteMAPImage routine is:
%
%      status=WriteMAPImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMAPImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteMAPImage(const ImageInfo *image_info,Image *image)
{
  register int
    i;

  register unsigned char
    *q;

  unsigned char
    *colormap;

  unsigned int
    packets,
    packet_size;

  unsigned short
    value;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Allocate colormap.
  */
  if (image->class == DirectClass)
    {
      QuantizeInfo
        quantize_info;

      /*
        Demote DirectClass to PseudoClass.
      */
      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=MaxColormapSize;
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
    }
  packet_size=3*(image->depth >> 3);
  colormap=(unsigned char *)
    AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
  if (colormap == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write colormap to file.
  */
  q=colormap;
  for (i=0; i < (int) image->colors; i++)
  {
    WriteQuantum(image->colormap[i].red,q);
    WriteQuantum(image->colormap[i].green,q);
    WriteQuantum(image->colormap[i].blue,q);
  }
  (void) fwrite((char *) colormap,packet_size,(int) image->colors,
    image->file);
  FreeMemory((char *) colormap);
  /*
    Write image pixels to file.
  */
  image->compression=NoCompression;
  packets=RunlengthEncodeImage(image);
  (void) fwrite((char *) image->packed_pixels,(int) image->packet_size,
    (int) packets,image->file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M A T T E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteMATTEImage writes an image of matte bytes to a file.  It
%  consists of data from the matte component of the image [0..255].
%
%  The format of the WriteMATTEImage routine is:
%
%      status=WriteMATTEImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteMATTEImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteMATTEImage(const ImageInfo *image_info,Image *image)
{
  Image
    *matte_image;

  register int
    i;

  unsigned int
    status;

  if (!image->matte)
    PrematureExit(ResourceLimitWarning,"Image does not have a matte channel",
      image);
  image->orphan=True;
  matte_image=CloneImage(image,image->columns,image->rows,True);
  image->orphan=False;
  if (matte_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  matte_image->class=PseudoClass;
  matte_image->colors=(Opaque-Transparent)+1;
  matte_image->colormap=(ColorPacket *)
    malloc(matte_image->colors*sizeof(ColorPacket));
  if (matte_image->colormap == (ColorPacket *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  for (i=Transparent; i <= Opaque; i++)
  {
    matte_image->colormap[i].red=i;
    matte_image->colormap[i].green=i;
    matte_image->colormap[i].blue=i;
  }
  SyncImage(matte_image);
  status=WriteMIFFImage(image_info,matte_image);
  DestroyImage(matte_image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMIFFImage writes an image to a file.
%
%  The format of the WriteMIFFImage routine is:
%
%      status=WriteMIFFImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteMIFFImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
unsigned int WriteMIFFImage(const ImageInfo *image_info,Image *image)
{
  char
    color[MaxTextExtent];

  CompressionType
    compression;

  register int
    i;

  unsigned int
    scene;

  unsigned long
    packets;

  if ((image->class != DirectClass) && (image->class != PseudoClass))
    PrematureExit(ResourceLimitWarning,"Unknown image class",image);
  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  CondenseImage(image);
  if (image->class == DirectClass)
    {
      if (image->packets >= ((3*image->columns*image->rows) >> 2))
        image->compression=NoCompression;
    }
  else
    if (image->packets >= ((image->columns*image->rows) >> 1))
      image->compression=NoCompression;
  compression=image_info->compression;
#if defined(HasZLIB)
  if (compression == UndefinedCompression)
    compression=ZipCompression;
#endif
#if defined(HasBZLIB)
  if (compression == UndefinedCompression)
    compression=BZipCompression;
#endif
  if (compression == UndefinedCompression)
    compression=RunlengthEncodedCompression;
  (void) strcpy((char *) image_info->magick,"MIFF");
  scene=0;
  do
  {
    /*
      Pack image pixels.
    */
    image->compression=compression;
    packets=RunlengthEncodeImage(image);
    if ((image->compression != NoCompression) &&
        (image->compression != RunlengthEncodedCompression))
      {
        int
          status;

        unsigned char
          *compressed_pixels;

        unsigned long
          compressed_packets;

        /*
          Compress image pixels with Zip encoding.
        */
        compressed_packets=(long unsigned int)
          (1.001*(packets*image->packet_size)+12);
        compressed_pixels=(unsigned char *)
          AllocateMemory(compressed_packets*sizeof(unsigned char));
        if (compressed_pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        status=True;
#if defined(HasBZLIB)
        if (compression == BZipCompression)
          {
            unsigned int
              max_packets;

            /*
              BZip compress the image pixels.
            */
            max_packets=compressed_packets;
            status=bzBuffToBuffCompress((char *) compressed_pixels,&max_packets,
              (char *) image->packed_pixels,packets*image->packet_size,
              Min(image_info->quality/10,9),image_info->verbose,
              (image_info->quality % 10)*(image_info->quality % 10)+5);
            compressed_packets=max_packets;
          }
#endif
#if defined(HasZLIB)
        if (status)
          {
            z_stream
              stream;

            /*
              BZlib compress the image pixels.
            */
            stream.next_in=image->packed_pixels;
            stream.avail_in=packets*image->packet_size;
            stream.next_out=compressed_pixels;
            stream.avail_out=compressed_packets;
            stream.zalloc=(alloc_func) NULL;
            stream.zfree=(free_func) NULL;
            stream.opaque=(voidpf) NULL;
            status=deflateInit(&stream,Min(image_info->quality/10,9));
            if (status == Z_OK)
              {
                status=deflate(&stream,Z_FINISH);
                if (status == Z_STREAM_END)
                  status=deflateEnd(&stream);
                else
                  (void) deflateEnd(&stream);
                compressed_packets=stream.total_out;
              }
            if (status == Z_OK)
              compression=ZipCompression;
          }
#endif
        if (status)
          {
            FreeMemory((char *) compressed_pixels);
            PrematureExit(DelegateWarning,"Unable to compress image",image);
          }
        else
          {
            FreeMemory((char *) image->packed_pixels);
            image->packed_pixels=compressed_pixels;
            image->packet_size=1;
            packets=compressed_packets;
          }
      }
    /*
      Write header to file.
    */
    (void) fprintf(image->file,"id=ImageMagick\n");
    if (image->class == PseudoClass)
      (void) fprintf(image->file,"class=PseudoClass  colors=%u\n",
        image->colors);
    else
      if (image->matte)
        (void) fprintf(image->file,"class=DirectClass  matte=True\n");
      else
        (void) fprintf(image->file,"class=DirectClass\n");
    if (image->compression == RunlengthEncodedCompression)
      (void) fprintf(image->file,"compression=RunlengthEncoded  packets=%lu\n",
        packets);
    else
      if (image->compression == BZipCompression)
        (void) fprintf(image->file,"compression=BZip  packets=%lu\n",packets);
      else
        if (image->compression != NoCompression)
          (void) fprintf(image->file,"compression=Zip  packets=%lu\n",packets);
    (void) fprintf(image->file,"columns=%u  rows=%u  depth=%u\n",image->columns,
      image->rows,image->depth);
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        char
          units[MaxTextExtent];

        /*
          Set image resolution.
        */
        (void) strcpy(units,"undefined");
        if (image->units == PixelsPerInchResolution)
          (void) strcpy(units,"pixels-per-inch");
        if (image->units == PixelsPerCentimeterResolution)
          (void) strcpy(units,"pixels-per-centimeter");
        (void) fprintf(image->file,"resolution=%gx%g  units=%.1024s\n",
          image->x_resolution,image->y_resolution,units);
      }
    SignatureImage(image);
    if (image->signature != (char *) NULL)
      (void) fprintf(image->file,"signature=%.1024s\n",image->signature);
    if (image->page != (char *) NULL)
      (void) fprintf(image->file,"page=%.1024s\n",image->page);
    (void) QueryColorName(&image->background_color,color);
    (void) fprintf(image->file,"background-color=%.1024s  ",color);
    (void) QueryColorName(&image->border_color,color);
    (void) fprintf(image->file,"border-color=%.1024s  ",color);
    (void) QueryColorName(&image->matte_color,color);
    (void) fprintf(image->file,"matte-color=%.1024s\n",color);
    if ((image->next != (Image *) NULL) || (image->previous != (Image *) NULL))
      (void) fprintf(image->file,
        "scene=%u  iterations=%u  delay=%u  dispose=%u\n",
        image->scene,image->iterations,image->delay,image->dispose);
    else
      {
        if (image->scene != 0)
          (void) fprintf(image->file,"scene=%u\n",image->scene);
        if (image->iterations != 1)
          (void) fprintf(image->file,"iterations=%u\n",image->iterations);
        if (image->delay != 0)
          (void) fprintf(image->file,"delay=%u\n",image->delay);
        if (image->dispose != 0)
          (void) fprintf(image->file,"dispose=%u\n",image->dispose);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        if (image->rendering_intent == SaturationIntent)
          (void) fprintf(image->file,"rendering-intent=saturation\n");
        else
          if (image->rendering_intent == PerceptualIntent)
            (void) fprintf(image->file,"rendering-intent=perceptual\n");
          else
            if (image->rendering_intent == AbsoluteIntent)
              (void) fprintf(image->file,"rendering-intent=absolute\n");
            else
              (void) fprintf(image->file,"rendering-intent=relative\n");
      }
    if (image->gamma != 0.0)
      (void) fprintf(image->file,"gamma=%g\n",image->gamma);
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        (void) fprintf(image->file,
          "red-primary=%g,%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) fprintf(image->file,"white-point=%g,%g\n",
          image->chromaticity.white_point.x,image->chromaticity.white_point.y);
      }
    if (image->color_profile.length > 0)
      (void) fprintf(image->file,"color-profile=%u\n",
        image->color_profile.length);
    if (image->montage != (char *) NULL)
      (void) fprintf(image->file,"montage=%.1024s\n",image->montage);
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"label=\"%.1024s\"\n",image->label);
    if (image->comments != (char *) NULL)
      (void) fprintf(image->file,"{%s}\n",image->comments);
    (void) fprintf(image->file,"\f\n:\n");
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) fprintf(image->file,"%s",image->directory);
        (void) fputc('\0',image->file);
      }
    if (image->color_profile.length > 0)
      (void) fwrite((char *) image->color_profile.info,1,(int)
        image->color_profile.length,image->file);
    if (image->class == PseudoClass)
      {
        register unsigned char
          *q;

        unsigned char
          *colormap;

        unsigned int
          packet_size;

        unsigned short
          value;

        /*
          Allocate colormap.
        */
        packet_size=3*(image->depth >> 3);
        colormap=(unsigned char *)
          AllocateMemory(packet_size*image->colors*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        q=colormap;
        for (i=0; i < (int) image->colors; i++)
        {
          WriteQuantum(image->colormap[i].red,q);
          WriteQuantum(image->colormap[i].green,q);
          WriteQuantum(image->colormap[i].blue,q);
        }
        /*
          Write colormap to file.
        */
        (void) fwrite((char *) colormap,1,(int) image->colors*packet_size,
          image->file);
        FreeMemory((char *) colormap);
      }
    /*
      Write image pixels to file.
    */
    (void) fwrite((char *) image->packed_pixels,(int) image->packet_size,
      (int) packets,image->file);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M O N O I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMONOImage writes an image of raw bits in LSB order to a file.
%
%  The format of the WriteMONOImage routine is:
%
%      status=WriteMONOImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMONOImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteMONOImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register unsigned char
    bit,
    byte,
    polarity;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Convert image to a bi-level image.
  */
  if (!IsMonochromeImage(image))
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=2;
      quantize_info.dither=image_info->dither;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
    }
  polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
  if (image->colors == 2)
    polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
  bit=0;
  byte=0;
  x=0;
  y=0;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      byte>>=1;
      if (p->index == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          (void) fputc(byte,image->file);
          bit=0;
          byte=0;
        }
      x++;
      if (x == (int) image->columns)
        {
          /*
            Advance to the next scanline.
          */
          if (bit != 0)
            (void) fputc(byte >> (8-bit),image->file);
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
          bit=0;
          byte=0;
          x=0;
          y++;
       }
    }
    p++;
  }
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M T V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMTVImage writes an image to a file in red, green, and blue
%  MTV rasterfile format.
%
%  The format of the WriteMTVImage routine is:
%
%      status=WriteMTVImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMTVImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteMTVImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Allocate memory for pixels.
    */
    pixels=(unsigned char *)
      AllocateMemory(image->columns*sizeof(RunlengthPacket));
    if (pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Initialize raster file header.
    */
    (void) fprintf(image->file,"%u %u\n",image->columns,image->rows);
    x=0;
    y=0;
    p=image->pixels;
    q=pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        *q++=DownScale(p->red);
        *q++=DownScale(p->green);
        *q++=DownScale(p->blue);
        x++;
        if (x == (int) image->columns)
          {
            (void) fwrite((char *) pixels,1,q-pixels,image->file);
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            q=pixels;
            x=0;
            y++;
          }
      }
      p++;
    }
    FreeMemory((char *) pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCDImage writes an image in the Photo CD encoded image
%  format.
%
%  The format of the WritePCDImage routine is:
%
%      status=WritePCDImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

unsigned int WritePCDTile(const ImageInfo *image_info,Image *image,
  char *geometry,char *tile_geometry)
{
  Image
    *downsampled_image,
    *tile_image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  unsigned int
    height,
    width;

  /*
    Scale image to tile size.
  */
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
  if ((width % 2) != 0)
    width--;
  if ((height % 2) != 0)
    height--;
  image->orphan=True;
  tile_image=ZoomImage(image,width,height);
  image->orphan=False;
  if (tile_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Unable to scale image",image);
  (void) sscanf(geometry,"%ux%u",&width,&height);
  if ((tile_image->columns != width) || (tile_image->rows != height))
    {
      Image
        *bordered_image;

      RectangleInfo
        border_info;

      /*
        Put a border around the image.
      */
      border_info.width=(width-tile_image->columns+1) >> 1;
      border_info.height=(height-tile_image->rows+1) >> 1;
      bordered_image=BorderImage(tile_image,&border_info);
      if (bordered_image == (Image *) NULL)
        PrematureExit(ResourceLimitWarning,"Unable to border image",image);
      DestroyImage(tile_image);
      tile_image=bordered_image;
    }
  TransformImage(&tile_image,(char *) NULL,tile_geometry);
  RGBTransformImage(tile_image,YCCColorspace);
  downsampled_image=MinifyImage(tile_image);
  if (downsampled_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Unable to scale image",image);
  if (!UncondenseImage(tile_image))
    return(False);
  if (!UncondenseImage(downsampled_image))
    return(False);
  /*
    Write tile to PCD file.
  */
  p=tile_image->pixels;
  for (y=0; y < (int) tile_image->rows; y+=2)
  {
    for (x=0; x < (int) (tile_image->columns << 1); x++)
    {
      (void) fputc(DownScale(p->red),image->file);
      p++;
    }
    q=downsampled_image->pixels+(y >> 1)*downsampled_image->columns;
    for (x=0; x < (int) downsampled_image->columns; x++)
    {
      (void) fputc(DownScale(q->green),image->file);
      q++;
    }
    q=downsampled_image->pixels+(y >> 1)*downsampled_image->columns;
    for (x=0; x < (int) downsampled_image->columns; x++)
    {
      (void) fputc(DownScale(q->blue),image->file);
      q++;
    }
    if (QuantumTick(y,tile_image->rows))
      ProgressMonitor(SaveImageText,y,tile_image->rows);
  }
  for (i=0; i < 0x800; i++)
    (void) fputc('\0',image->file);
  DestroyImage(downsampled_image);
  DestroyImage(tile_image);
  return(True);
}

unsigned int WritePCDImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pcd_image;

  register int
    i;

  unsigned int
    status;

  pcd_image=image;
  if (image->columns < image->rows)
    {
      Image
        *rotated_image;

      /*
        Rotate portrait to landscape.
      */
      image->orphan=True;
      rotated_image=RotateImage(image,90.0,False,True);
      image->orphan=False;
      if (rotated_image == (Image *) NULL)
        PrematureExit(ResourceLimitWarning,"Unable to rotate image",image);
      pcd_image=rotated_image;
    }
  /*
    Open output image file.
  */
  OpenImage(image_info,pcd_image,WriteBinaryType);
  if (pcd_image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",pcd_image);
  /*
    Write PCD image header.
  */
  for (i=0; i < 0x800; i++)
    (void) fputc('\0',pcd_image->file);
  (void) fwrite("PCD_IPI",1,7,pcd_image->file);
  (void) fputc(0x06,pcd_image->file);
  for (i=0; i < 1530; i++)
    (void) fputc('\0',pcd_image->file);
  if (image->columns < image->rows)
    (void) fputc('\1',pcd_image->file);
  else
    (void) fputc('\0',pcd_image->file);
  for (i=0; i < 3*0x800-1539; i++)
    (void) fputc('\0',pcd_image->file);
  /*
    Write PCD tiles.
  */
  status=WritePCDTile(image_info,pcd_image,"768x512>","192x128");
  status|=WritePCDTile(image_info,pcd_image,"768x512>","384x256");
  status|=WritePCDTile(image_info,pcd_image,"768x512>","768x512");
  CloseImage(pcd_image);
  if (image->columns < image->rows)
    DestroyImage(pcd_image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             % %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCLImage writes an image in the Page Control Language encoded
%  image format.
%
%  The format of the WritePCLImage routine is:
%
%      status=WritePCLImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCLImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
%
*/
unsigned int WritePCLImage(const ImageInfo *image_info,Image *image)
{
  char
    geometry[MaxTextExtent];

  int
    sans_offset,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  RectangleInfo
    media_info;

  unsigned int
    density,
    height,
    page_size,
    text_size,
    width;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize the printer.
  */
  (void) fprintf(image->file,"\033E");  /* portrait orientation */
  (void) fprintf(image->file,"\033&l0O");  /* portrait orientation */
  (void) fprintf(image->file,"\033&l0E");  /* top margin 0 */
  /*
    Center image on PCL page.
  */
  text_size=0;
  if (image->label != (char *) NULL)
    text_size=MultilineCensus(image->label)*image_info->pointsize+12;
  width=image->columns;
  height=image->rows;
  x=0;
  y=text_size;
  if (image_info->page != (char *) NULL)
    (void) strcpy(geometry,image_info->page);
  else
    if (image->page != (char *) NULL)
      (void) strcpy(geometry,image->page);
    else
      (void) strcpy(geometry,PCLPageGeometry);
  (void) ParseImageGeometry(geometry,&x,&y,&width,&height);
  (void) GetGeometry(geometry,&media_info.x,&media_info.y,
    &media_info.width,&media_info.height);
  page_size=2;
  if ((media_info.width == 540) && (media_info.height == 720))
    page_size=1;  /* executive */
  if ((media_info.width == 612) && (media_info.height == 792))
    page_size=2;  /* letter */
  if ((media_info.width == 612) && (media_info.height == 1008))
    page_size=3;  /* legal */
  if ((media_info.width == 1224) && (media_info.height == 792))
    page_size=6;  /* ledger */
  if ((media_info.width == 595) && (media_info.height == 842))
    page_size=26;  /* A4 */
  if ((media_info.width == 842) && (media_info.height == 1191))
    page_size=27;  /* A3 */
  if ((media_info.width == 729) && (media_info.height == 1032))
    page_size=45;  /* B5 */
  if ((media_info.width == 516) && (media_info.height == 729))
    page_size=46;  /* B4 */
  (void) fprintf(image->file,"\033&l%uA",page_size);  /* papersize */
  density=72;
  if (image_info->density != (char *) NULL)
    (void) XParseGeometry(image_info->density,&sans_offset,&sans_offset,
      &density,&density);
  else
    (void) XParseGeometry(PCLDensityGeometry,&sans_offset,&sans_offset,
      &density,&density);
  (void) fprintf(image->file,"\033*p%dx%dY",x,y);
  if (image->label != (char *) NULL)
    {
      /*
        Print label.
      */
      (void) fprintf(image->file,"\033&k2G");
      (void) fprintf(image->file,"\033(s1p%uv5t3b",image_info->pointsize);
      (void) fprintf(image->file,"\n%.1024s\n",image->label);
      (void) fprintf(image->file,"\033(s0B");
    }
  (void) fprintf(image->file,"\033*t%uR",density);  /* graphic resolution */
  width=(density*width)/75;
  height=(density*height)/75;
  if (!IsGrayImage(image))
    {
      /*
        Write PCL color image.
      */
      (void) fprintf(image->file,"\033*r%us%uT",image->columns,image->rows);
      (void) fprintf(image->file,"\033*t%uh%uV",width,height);
      (void) fprintf(image->file,"\033*v6W");
      (void) fputc('\000',image->file);  /* color model */
      (void) fputc('\003',image->file);  /* direct pixel encoding */
      (void) fputc('\000',image->file);  /* bits per index */
      (void) fputc('\010',image->file);  /* bits red*/
      (void) fputc('\010',image->file);  /* bits green*/
      (void) fputc('\010',image->file);  /* bits blue */
      (void) fprintf(image->file,"\033*r2A");  /* start graphics */
      (void) fprintf(image->file,"\033*b0M");  /* no compression */
      (void) fprintf(image->file,"\033*b%uW",3*image->columns);
      x=0;
      y=0;
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          (void) fprintf(image->file,"%c%c%c",(int) DownScale(p->red),
            (int) DownScale(p->green),(int) DownScale(p->blue));
        x++;
        if (x == (int) image->columns)
          {
            (void) fprintf(image->file,"\033*b%uW",3*image->columns);
            if (QuantumTick(y,image->rows))
              ProgressMonitor(SaveImageText,y,image->rows);
            x=0;
            y++;
          }
        p++;
      }
      (void) fprintf(image->file,"\033*rC");  /* end graphics */
    }
  else
    {
      Image
        *monochrome_image;

      register unsigned char
        bit,
        byte,
        polarity;

      /*
        Write PCL monochrome image.
      */
      monochrome_image=image;
      if ((width != image->columns) || (height != image->rows))
        {
          /*
            Scale image.
          */
          image->orphan=True;
          monochrome_image=ZoomImage(image,width,height);
          image->orphan=False;
          if (monochrome_image == (Image *) NULL)
            PrematureExit(ResourceLimitWarning,"Unable to scale image",image);
        }
      if (IsMonochromeImage(monochrome_image))
        {
          QuantizeInfo
            quantize_info;

          GetQuantizeInfo(&quantize_info);
          quantize_info.number_colors=2;
          quantize_info.dither=image_info->dither;
          quantize_info.colorspace=GRAYColorspace;
          (void) QuantizeImage(&quantize_info,monochrome_image);
          SyncImage(monochrome_image);
        }
      p=monochrome_image->pixels;
      polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
      if (monochrome_image->colors == 2)
        polarity=Intensity(monochrome_image->colormap[0]) >
          Intensity(monochrome_image->colormap[1]);
      bit=0;
      byte=0;
      x=0;
      y=0;
      (void) fprintf(image->file,"\033*r%us%uT",monochrome_image->columns,
        monochrome_image->rows);
      (void) fprintf(image->file,"\033*r1A");  /* start graphics */
      (void) fprintf(image->file,"\033*b0M");  /* no compression */
      (void) fprintf(image->file,"\033*b%uW",(image->columns+7)/8);
      for (i=0; i < (int) monochrome_image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte<<=1;
          if (p->index == polarity)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              (void) fputc(byte,image->file);
              bit=0;
              byte=0;
            }
          x++;
          if (x == (int) monochrome_image->columns)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                (void) fputc(byte  << (8-bit),image->file);
              if (QuantumTick(y,monochrome_image->rows))
                ProgressMonitor(SaveImageText,y,monochrome_image->rows);
              bit=0;
              byte=0;
              x=0;
              y++;
              if (y < (int) monochrome_image->rows)
                (void) fprintf(image->file,"\033*b%uW",
                  (monochrome_image->columns+7)/8);
           }
        }
        p++;
      }
      (void) fprintf(image->file,"\033*rB");  /* end graphics */
      if (image != monochrome_image)
        DestroyImage(monochrome_image);
    }
  (void) fprintf(image->file,"\033&l0H");
  (void) fprintf(image->file,"\033E");  /* portrait orientation */
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P C X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePCXImage writes an image in the ZSoft IBM PC Paintbrush file
%  format.
%
%  The format of the WritePCXImage routine is:
%
%      status=WritePCXImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePCXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePCXImage(const ImageInfo *image_info,Image *image)
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

  register int
    i,
    j,
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    count,
    packet,
    *pcx_colormap,
    *pcx_pixels,
    previous;

  unsigned int
    packets,
    scene;

  unsigned long
    *page_table;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  page_table=(unsigned long *) NULL;
  if (image_info->adjoin)
    {
      /*
        Write the DCX page table.
      */
      LSBFirstWriteLong(0x3ADE68B1L,image->file);
      page_table=(unsigned long *) AllocateMemory(1024*sizeof(unsigned long));
      if (page_table == (unsigned long *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (scene=0; scene < 1024; scene++)
        LSBFirstWriteLong(0x00000000L,image->file);
    }
  scene=0;
  do
  {
    if (page_table != (unsigned long *) NULL)
      page_table[scene]=ftell(image->file);
    /*
      Initialize PCX raster file header.
    */
    pcx_header.identifier=0x0a;
    pcx_header.version=5;
    pcx_header.encoding=1;
    pcx_header.bits_per_pixel=8;
    if (IsMonochromeImage(image))
      pcx_header.bits_per_pixel=1;
    pcx_header.left=0;
    pcx_header.top=0;
    pcx_header.right=image->columns-1;
    pcx_header.bottom=image->rows-1;
    pcx_header.horizontal_resolution=(short) image->columns;
    pcx_header.vertical_resolution=(short) image->rows;
    if (image->units == PixelsPerInchResolution)
      {
        pcx_header.horizontal_resolution=(short) image->x_resolution;
        pcx_header.vertical_resolution=(short) image->y_resolution;
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        pcx_header.horizontal_resolution=(short) (2.54*image->x_resolution);
        pcx_header.vertical_resolution=(short) (2.54*image->y_resolution);
      }
    pcx_header.reserved=0;
    pcx_header.planes=1;
    if (!IsPseudoClass(image))
      {
        pcx_header.planes=3;
        if (image->matte)
          pcx_header.planes++;
      }
    pcx_header.bytes_per_line=(image->columns*pcx_header.bits_per_pixel+7)/8;
    pcx_header.palette_info=1;
    pcx_header.colormap_signature=0x0c;
    /*
      Write PCX header.
    */
    (void) fwrite(&pcx_header.identifier,1,1,image->file);
    (void) fwrite(&pcx_header.version,1,1,image->file);
    (void) fwrite(&pcx_header.encoding,1,1,image->file);
    (void) fwrite(&pcx_header.bits_per_pixel,1,1,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.left,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.top,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.right,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.bottom,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.horizontal_resolution,
      image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.vertical_resolution,
      image->file);
    /*
      Dump colormap to file.
    */
    pcx_colormap=(unsigned char *) AllocateMemory(3*256*sizeof(unsigned char));
    if (pcx_colormap == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    for (i=0; i < (3*256); i++)
      pcx_colormap[i]=0;
    q=pcx_colormap;
    if (image->class == PseudoClass)
      for (i=0; i < (int) image->colors; i++)
      {
        *q++=DownScale(image->colormap[i].red);
        *q++=DownScale(image->colormap[i].green);
        *q++=DownScale(image->colormap[i].blue);
      }
    (void) fwrite((char *) pcx_colormap,3,16,image->file);
    (void) fwrite(&pcx_header.reserved,1,1,image->file);
    (void) fwrite(&pcx_header.planes,1,1,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.bytes_per_line,image->file);
    LSBFirstWriteShort((unsigned int) pcx_header.palette_info,image->file);
    for (i=0; i < 58; i++)
      (void) fwrite("\0",1,1,image->file);
    packets=image->rows*pcx_header.bytes_per_line*pcx_header.planes;
    pcx_pixels=(unsigned char *) AllocateMemory(packets*sizeof(unsigned char));
    if (pcx_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    x=0;
    y=0;
    p=image->pixels;
    q=pcx_pixels;
    if (image->class == DirectClass)
      {
        /*
          Convert DirectClass image to PCX raster pixels.
        */
        if (!UncondenseImage(image))
          return(False);
        for (y=0; y < (int) image->rows; y++)
        {
          q=pcx_pixels+(y*pcx_header.bytes_per_line*pcx_header.planes);
          for (i=0; i < (int) pcx_header.planes; i++)
          {
            p=image->pixels+y*image->columns;
            for (x=0; x < pcx_header.bytes_per_line; x++)
            {
              switch (i)
              {
                case 0:
                {
                  *q++=DownScale(p->red);
                  break;
                }
                case 1:
                {
                  *q++=DownScale(p->green);
                  break;
                }
                case 2:
                {
                  *q++=DownScale(p->blue);
                  break;
                }
                case 3:
                default:
                {
                  *q++=DownScale(p->index);
                  break;
                }
              }
              p++;
            }
          }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
        }
      }
    else
      if (pcx_header.bits_per_pixel > 1)
        for (i=0; i < (int) image->packets; i++)
        {
          /*
            Convert PseudoClass image to PCX raster pixels.
          */
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=p->index;
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                x=0;
                y++;
                q=pcx_pixels+y*pcx_header.bytes_per_line;
              }
          }
          p++;
        }
      else
        {
          register unsigned char
            bit,
            byte,
            polarity;

          /*
            Convert PseudoClass image to a PCX monochrome image.
          */
          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=Intensity(image->colormap[0]) <
              Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              byte<<=1;
              if (p->index == polarity)
                byte|=0x01;
              bit++;
              if (bit == 8)
                {
                  *q++=byte;
                  bit=0;
                  byte=0;
                }
              x++;
              if (x == (int) image->columns)
                {
                  /*
                    Advance to the next scanline.
                  */
                  if (bit != 0)
                    *q++=byte << (8-bit);
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  bit=0;
                  byte=0;
                  x=0;
                  y++;
                  q=pcx_pixels+y*pcx_header.bytes_per_line;
                }
            }
            p++;
          }
        }
    /*
      Runlength-encoded PCX pixels.
    */
    for (y=0; y < (int) image->rows; y++)
    {
      q=pcx_pixels+(y*pcx_header.bytes_per_line*pcx_header.planes);
      for (i=0; i < (int) pcx_header.planes; i++)
      {
        previous=(*q++);
        count=1;
        for (x=0; x < (pcx_header.bytes_per_line-1); x++)
        {
          packet=(*q++);
          if ((packet == previous) && (count < 63))
            {
              count++;
              continue;
            }
          if ((count > 1) || ((previous & 0xc0) == 0xc0))
            {
              count|=0xc0;
              (void) fwrite(&count,1,1,image->file);
            }
          (void) fwrite(&previous,1,1,image->file);
          previous=packet;
          count=1;
        }
        if ((count > 1) || ((previous & 0xc0) == 0xc0))
          {
            count|=0xc0;
            (void) fwrite(&count,1,1,image->file);
          }
        (void) fwrite(&previous,1,1,image->file);
        if (QuantumTick(y,image->rows))
          ProgressMonitor(SaveImageText,y,image->rows);
      }
    }
    (void) fwrite(&pcx_header.colormap_signature,1,1,image->file);
    (void) fwrite((char *) pcx_colormap,3,256,image->file);
    FreeMemory((char *) pcx_pixels);
    FreeMemory((char *) pcx_colormap);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
    if (scene >= 1023)
      break;
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  if (page_table != (unsigned long *) NULL)
    {
      /*
        Write the DCX page table.
      */
      page_table[scene+1]=0;
      (void) fseek(image->file,0L,SEEK_SET);
      LSBFirstWriteLong(0x3ADE68B1L,image->file);
      for (i=0; i <= (int) scene; i++)
        LSBFirstWriteLong(page_table[i],image->file);
      FreeMemory((char *) page_table);
    }
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDFImage writes an image in the Portable Document image
%  format.
%
%  The format of the WritePDFImage routine is:
%
%      status=WritePDFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePDFImage(const ImageInfo *image_info,Image *image)
{
#define ObjectsPerImage  12

  char
    date[MaxTextExtent],
    density[MaxTextExtent],
    geometry[MaxTextExtent],
    **labels;

  CompressionType
    compression;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  int
    count,
    status,
    x,
    y;

  Image
    encode_image,
    *tile_image;

  RectangleInfo
    media_info;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register int
    i,
    j;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    info_id,
    object,
    pages_id,
    root_id,
    scene,
    text_size,
    width;

  unsigned long
    length,
    number_packets,
    *xref;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file == stdout) || image->pipe)
    {
      /*
        Write standard output or pipe to temporary file.
      */
      encode_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
    }
  compression=image_info->compression;
#if defined(HasZLIB)
  if (compression == UndefinedCompression)
    compression=ZipCompression;
#else
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
#endif
  /*
    Allocate X ref memory.
  */
  xref=(unsigned long *) AllocateMemory(2048*sizeof(unsigned long));
  if (xref == (unsigned long *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write Info object.
  */
  object=0;
  (void) fprintf(image->file,"%%PDF-1.1 \n");
  xref[object++]=ftell(image->file);
  info_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  timer=time((time_t *) NULL);
  (void) localtime(&timer);
  (void) strcpy(date,ctime(&timer));
  date[Extent(date)-1]='\0';
  (void) fprintf(image->file,"/CreationDate (%.1024s)\n",date);
  (void) fprintf(image->file,"/Producer (%.1024s)\n",MagickVersion);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  /*
    Write Catalog object.
  */
  xref[object++]=ftell(image->file);
  root_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Type /Catalog\n");
  (void) fprintf(image->file,"/Pages %u 0 R\n",object+1);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  /*
    Write Pages object.
  */
  xref[object++]=ftell(image->file);
  pages_id=object;
  (void) fprintf(image->file,"%u 0 obj\n",object);
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Type /Pages\n");
  (void) fprintf(image->file,"/Kids [ %u 0 R ",object+1);
  count=pages_id+ObjectsPerImage+1;
  if (image_info->adjoin)
    {
      Image
        *kid_image;

      /*
        Predict page object id's.
      */
      kid_image=image;
      for ( ; kid_image->next != (Image *) NULL; count+=ObjectsPerImage)
      {
        (void) fprintf(image->file,"%d 0 R ",count);
        kid_image=kid_image->next;
      }
      xref=(unsigned long *)
        ReallocateMemory((char *) xref,(count+2048)*sizeof(unsigned long));
      if (xref == (unsigned long *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    }
  (void) fprintf(image->file,"]\n");
  (void) fprintf(image->file,"/Count %u\n",(count-pages_id)/ObjectsPerImage);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"endobj\n");
  scene=0;
  do
  {
    /*
      Scale image to size of Portable Document page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    FormatString(geometry,"%ux%u",image->columns,image->rows);
    if (Latin1Compare(image_info->magick,"PDF") == 0)
      {
        if (image_info->page != (char *) NULL)
          {
            (void) ParseImageGeometry(image_info->page,&x,&y,&width,&height);
            (void) strcpy(geometry,image_info->page);
          }
        else
          if (image->page != (char *) NULL)
            {
              (void) ParseImageGeometry(image->page,&x,&y,&width,&height);
              (void) strcpy(geometry,image->page);
            }
          else
            (void) strcpy(geometry,PDFPageGeometry);
      }
    (void) GetGeometry(geometry,&media_info.x,&media_info.y,
      &media_info.width,&media_info.height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        x_resolution=image->x_resolution;
        y_resolution=image->y_resolution;
      }
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    /*
      Write Page object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /Page\n");
    (void) fprintf(image->file,"/Parent %u 0 R\n",pages_id);
    (void) fprintf(image->file,"/Resources <<\n");
    (void) fprintf(image->file,"/Font << /F%u %u 0 R >>\n",image->scene,
      object+4);
    (void) fprintf(image->file,"/XObject << /Im%u %u 0 R >>\n",image->scene,
      object+5);
    (void) fprintf(image->file,"/ProcSet %u 0 R >>\n",object+3);
    (void) fprintf(image->file,"/MediaBox [ %d %d %d %d ]\n",0,0,
      media_info.width,media_info.height);
    (void) fprintf(image->file,"/Contents %u 0 R\n",object+1);
    (void) fprintf(image->file,"/Thumb %u 0 R\n",object+8);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Contents object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    (void) fprintf(image->file,"q\n");
    labels=StringToList(image->label);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) fprintf(image->file,"BT\n");
          (void) fprintf(image->file,"/F%u %u Tf\n",image->scene,
            image_info->pointsize);
          (void) fprintf(image->file,"%d %u Td\n",x,y+height+
            i*image_info->pointsize+12);
          (void) fprintf(image->file,"(%.1024s) Tj\n",labels[i]);
          (void) fprintf(image->file,"ET\n");
          FreeMemory(labels[i]);
        }
        FreeMemory((char *) labels);
      }
    (void) fprintf(image->file,"%f 0 0 %f %d %d cm\n",x_scale,y_scale,x,y);
    (void) fprintf(image->file,"/Im%u Do\n",image->scene);
    (void) fprintf(image->file,"Q\n");
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"endstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Procset object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      (void) fprintf(image->file,"[ /PDF /Text /ImageC");
    else
      if (IsFaxImage(image))
        (void) fprintf(image->file,"[ /PDF /Text /ImageB");
      else
        (void) fprintf(image->file,"[ /PDF /Text /ImageI");
    (void) fprintf(image->file," ]\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Font object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /Font\n");
    (void) fprintf(image->file,"/Subtype /Type1\n");
    (void) fprintf(image->file,"/Name /F%u\n",image->scene);
    (void) fprintf(image->file,"/BaseFont /Helvetica\n");
    (void) fprintf(image->file,"/Encoding /MacRomanEncoding\n");
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write XObject object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    (void) fprintf(image->file,"/Type /XObject\n");
    (void) fprintf(image->file,"/Subtype /Image\n");
    (void) fprintf(image->file,"/Name /Im%u\n",image->scene);
    if (compression == NoCompression)
      (void) fprintf(image->file,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        (void) fprintf(image->file,"/Filter [ /ASCII85Decode /%.1024s ]\n",
          compression == ZipCompression ? "FlateDecode" :
          compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
      else
        {
          (void) fprintf(image->file,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) fprintf(image->file,
            "/DecodeParms [ << >> << /K %.1024s /Columns %d /Rows %d >> ]\n",
            CCITTParam,image->columns,image->rows);
        }
    (void) fprintf(image->file,"/Width %u\n",image->columns);
    (void) fprintf(image->file,"/Height %u\n",image->rows);
    (void) fprintf(image->file,"/ColorSpace %u 0 R\n",object+2);
    (void) fprintf(image->file,"/BitsPerComponent %d\n",
      IsFaxImage(image) ? 1 : 8);
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=3*image->columns*image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                {
                  *q++=DownScale(p->red);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->blue);
                }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          if (compression == ZipCompression)
            status=ZLIBEncodeImage(image->file,number_packets,
              image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image->file,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image->file,number_packets,pixels);
          FreeMemory((char *) pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                {
                  Ascii85Encode(DownScale(p->red),image->file);
                  Ascii85Encode(DownScale(p->green),image->file);
                  Ascii85Encode(DownScale(p->blue),image->file);
                }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          Ascii85Flush(image->file);
          break;
        }
      }
    else
      if (IsFaxImage(image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              if (Latin1Compare(CCITTParam,"0") == 0)
                (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
              else
                (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          ProgressMonitor(SaveImageText,y,image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=image->columns*image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"\nendstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Colorspace object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      (void) fprintf(image->file,"/DeviceRGB\n");
    else
      if (IsFaxImage(image))
        (void) fprintf(image->file,"/DeviceGray\n");
      else
        (void) fprintf(image->file,"[ /Indexed /DeviceRGB %u %u 0 R ]\n",
          image->colors-1,object+3);
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Thumb object.
    */
    width=image->columns;
    height=image->rows;
    x=0;
    y=0;
    image->orphan=True;
    (void) ParseImageGeometry(DefaultThumbnailGeometry,&x,&y,&width,&height);
    if (image->class == PseudoClass)
      tile_image=SampleImage(image,width,height);
    else
      tile_image=ZoomImage(image,width,height);
    image->orphan=False;
    if (tile_image == (Image *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"<<\n");
    if (compression == NoCompression)
      (void) fprintf(image->file,"/Filter /ASCII85Decode\n");
    else
      if (!IsFaxImage(image))
        (void) fprintf(image->file,"/Filter [ /ASCII85Decode /%.1024s ]\n",
          compression == ZipCompression ? "FlateDecode" :
          compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
      else
        {
          (void) fprintf(image->file,
            "/Filter [ /ASCII85Decode /CCITTFaxDecode ]\n");
          (void) fprintf(image->file,
            "/DecodeParms [ << >> << /Columns %d /Rows %d >> ]\n",
            tile_image->columns,tile_image->rows);
        }
    (void) fprintf(image->file,"/Width %u\n",tile_image->columns);
    (void) fprintf(image->file,"/Height %u\n",tile_image->rows);
    (void) fprintf(image->file,"/ColorSpace %u 0 R\n",object-1);
    (void) fprintf(image->file,"/BitsPerComponent %d\n",
      IsFaxImage(tile_image) ? 1 : 8);
    (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
    (void) fprintf(image->file,">>\n");
    (void) fprintf(image->file,"stream\n");
    length=ftell(image->file);
    p=tile_image->pixels;
    if (!IsPseudoClass(tile_image) && !IsGrayImage(tile_image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Allocate pixel array.
          */
          number_packets=3*tile_image->columns*tile_image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            {
              DestroyImage(tile_image);
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            }
          /*
            Dump runlength encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) tile_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (tile_image->matte && (p->index == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                {
                  *q++=DownScale(p->red);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->blue);
                }
            }
            p++;
          }
          if (compression == ZipCompression)
            status=ZLIBEncodeImage(image->file,number_packets,
              image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image->file,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image->file,number_packets,pixels);
          FreeMemory((char *) pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (i=0; i < (int) tile_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (tile_image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                {
                  Ascii85Encode(DownScale(p->red),image->file);
                  Ascii85Encode(DownScale(p->green),image->file);
                  Ascii85Encode(DownScale(p->blue),image->file);
                }
            }
            p++;
          }
          Ascii85Flush(image->file);
          break;
        }
      }
    else
      if (IsFaxImage(tile_image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(tile_image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=Intensity(tile_image->colormap[0]) <
              Intensity(tile_image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=((tile_image->columns+7) >> 3)*tile_image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                    image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      *q++=byte;
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) tile_image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        *q++=byte << (8-bit);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,tile_image->rows))
                          ProgressMonitor(SaveImageText,y,tile_image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) tile_image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,tile_image->rows))
                          ProgressMonitor(SaveImageText,y,tile_image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Allocate pixel array.
              */
              number_packets=tile_image->columns*tile_image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                {
                  DestroyImage(tile_image);
                  PrematureExit(ResourceLimitWarning,
                    "Memory allocation failed",image);
                }
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
                p++;
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) tile_image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    DestroyImage(tile_image);
    length=ftell(image->file)-length;
    (void) fprintf(image->file,"\nendstream\n");
    (void) fprintf(image->file,"endobj\n");
    /*
      Write Length object.
    */
    xref[object++]=ftell(image->file);
    (void) fprintf(image->file,"%u 0 obj\n",object);
    (void) fprintf(image->file,"%lu\n",length);
    (void) fprintf(image->file,"endobj\n");
    if ((image->class == DirectClass) || IsFaxImage(image))
      {
        xref[object++]=0;
        xref[object++]=0;
      }
    else
      {
        /*
          Write Colormap object.
        */
        xref[object++]=ftell(image->file);
        (void) fprintf(image->file,"%u 0 obj\n",object);
        (void) fprintf(image->file,"<<\n");
        (void) fprintf(image->file,"/Filter /ASCII85Decode \n");
        (void) fprintf(image->file,"/Length %u 0 R\n",object+1);
        (void) fprintf(image->file,">>\n");
        (void) fprintf(image->file,"stream\n");
        length=ftell(image->file);
        Ascii85Initialize();
        for (i=0; i < (int) image->colors; i++)
        {
          Ascii85Encode(DownScale(image->colormap[i].red),image->file);
          Ascii85Encode(DownScale(image->colormap[i].green),image->file);
          Ascii85Encode(DownScale(image->colormap[i].blue),image->file);
        }
        Ascii85Flush(image->file);
        length=ftell(image->file)-length;
        (void) fprintf(image->file,"\nendstream\n");
        (void) fprintf(image->file,"endobj\n");
        /*
          Write Length object.
        */
        xref[object++]=ftell(image->file);
        (void) fprintf(image->file,"%u 0 obj\n",object);
        (void) fprintf(image->file,"%lu\n",length);
        (void) fprintf(image->file,"endobj\n");
      }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  /*
    Write Xref object.
  */
  length=ftell(image->file)-xref[0]+10;
  (void) fprintf(image->file,"xref\n");
  (void) fprintf(image->file,"0 %u\n",object+1);
  (void) fprintf(image->file,"0000000000 65535 f \n");
  for (i=0; i < (int) object; i++)
    (void) fprintf(image->file,"%010lu 00000 n \n",xref[i]);
  (void) fprintf(image->file,"trailer\n");
  (void) fprintf(image->file,"<<\n");
  (void) fprintf(image->file,"/Size %u\n",object+1);
  (void) fprintf(image->file,"/Info %u 0 R\n",info_id);
  (void) fprintf(image->file,"/Root %u 0 R\n",root_id);
  (void) fprintf(image->file,">>\n");
  (void) fprintf(image->file,"startxref\n");
  (void) fprintf(image->file,"%lu\n",length);
  (void) fprintf(image->file,"%%%%EOF\n");
  FreeMemory((char *) xref);
  CloseImage(image);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) putc(c,encode_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseImage(&encode_image);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P I C T I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePICTImage writes an image to a file in the Apple Macintosh
%  QuickDraw/PICT image format.
%
%  The format of the WritePICTImage routine is:
%
%      status=WritePICTImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePICTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePICTImage(const ImageInfo *image_info,Image *image)
{
#include "pict.h"

#define MaxCount  128
#define PictCropRegionOp  0x01
#define PictEndOfPictureOp  0xff
#define PictHeaderOp  0x0C00
#define PictHeaderSize  512
#define PictPixmapOp  0x9A
#define PictPICTOp  0x98
#define PictVersion  0x11

  int
    count,
    x,
    y;

  PICTPixmap
    pixmap;

  PICTRectangle
    bounds,
    crop_rectangle,
    destination_rectangle,
    frame_rectangle,
    size_rectangle,
    source_rectangle;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned char
    *buffer,
    *packed_scanline,
    *scanline;

  unsigned int
    bytes_per_line;

  unsigned long
    horizontal_resolution,
    vertical_resolution;

  unsigned short
    base_address,
    row_bytes,
    transfer_mode;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Allocate memory.
  */
  bytes_per_line=image->columns;
  if (!IsPseudoClass(image))
    bytes_per_line*=image->matte ? 4 : 3;
  buffer=(unsigned char *)
    AllocateMemory(PictHeaderSize*sizeof(unsigned char));
  packed_scanline=(unsigned char *) AllocateMemory((bytes_per_line+
    bytes_per_line/MaxCount+1)*sizeof(unsigned char));
  scanline=(unsigned char *)
    AllocateMemory(bytes_per_line*sizeof(unsigned char));
  if ((buffer == (unsigned char *) NULL) ||
      (packed_scanline == (unsigned char *) NULL) ||
      (scanline == (unsigned char *) NULL))
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize image info.
  */
  size_rectangle.top=0;
  size_rectangle.left=0;
  size_rectangle.right=image->rows;
  size_rectangle.bottom=image->columns;
  frame_rectangle=size_rectangle;
  crop_rectangle=size_rectangle;
  source_rectangle=size_rectangle;
  destination_rectangle=size_rectangle;
  horizontal_resolution=0x00480000;
  vertical_resolution=0x00480000;
  base_address=0xff;
  row_bytes=((image->class == DirectClass ? 4 : 1)*image->columns) | 0x8000;
  bounds.top=0;
  bounds.left=0;
  bounds.right=image->rows;
  bounds.bottom=image->columns;
  pixmap.version=0;
  pixmap.pack_type=(image->class == DirectClass ? 0x4 : 0x0);
  pixmap.pack_size=0;
  pixmap.horizontal_resolution=horizontal_resolution;
  pixmap.vertical_resolution=vertical_resolution;
  pixmap.pixel_type=(image->class == DirectClass ? 16 : 0);
  pixmap.bits_per_pixel=(image->class == DirectClass ? 32 : 8);
  pixmap.component_count=1;
  if (image->class == DirectClass)
    pixmap.component_count=image->matte ? 4 : 3;
  pixmap.component_size=8;
  pixmap.plane_bytes=0;
  pixmap.table=0;
  pixmap.reserved=0;
  transfer_mode=(image->class == DirectClass ? 0x40 : 0);
  /*
    Write header, header size, size bounding box, version, and reserved.
  */
  for (i=0; i < PictHeaderSize; i++)
    buffer[i]=0;
  (void) fwrite((char *) buffer,1,PictHeaderSize,image->file);
  MSBFirstWriteShort(0,image->file);
  MSBFirstWriteShort(size_rectangle.top,image->file);
  MSBFirstWriteShort(size_rectangle.left,image->file);
  MSBFirstWriteShort(size_rectangle.right,image->file);
  MSBFirstWriteShort(size_rectangle.bottom,image->file);
  MSBFirstWriteShort(PictVersion,image->file);
  MSBFirstWriteShort(0x02ff,image->file);
  MSBFirstWriteShort(PictHeaderOp,image->file);
  /*
    Write full size of the file, resolution, frame bounding box, and reserved.
  */
  MSBFirstWriteLong(0xFFFE0000UL,image->file);
  MSBFirstWriteLong(horizontal_resolution,image->file);
  MSBFirstWriteLong(vertical_resolution,image->file);
  MSBFirstWriteShort(frame_rectangle.top,image->file);
  MSBFirstWriteShort(frame_rectangle.left,image->file);
  MSBFirstWriteShort(frame_rectangle.right,image->file);
  MSBFirstWriteShort(frame_rectangle.bottom,image->file);
  MSBFirstWriteLong(0x00000000L,image->file);
  /*
    Write crop region opcode and crop bounding box.
  */
  MSBFirstWriteShort(PictCropRegionOp,image->file);
  MSBFirstWriteShort(0xa,image->file);
  MSBFirstWriteShort(crop_rectangle.top,image->file);
  MSBFirstWriteShort(crop_rectangle.left,image->file);
  MSBFirstWriteShort(crop_rectangle.right,image->file);
  MSBFirstWriteShort(crop_rectangle.bottom,image->file);
  /*
    Write picture opcode, row bytes, and picture bounding box, and version.
  */
  if (image->class == PseudoClass)
    MSBFirstWriteShort(PictPICTOp,image->file);
  else
    {
      MSBFirstWriteShort(PictPixmapOp,image->file);
      MSBFirstWriteLong((unsigned long) base_address,image->file);
    }
  MSBFirstWriteShort(row_bytes | 0x8000,image->file);
  MSBFirstWriteShort(bounds.top,image->file);
  MSBFirstWriteShort(bounds.left,image->file);
  MSBFirstWriteShort(bounds.right,image->file);
  MSBFirstWriteShort(bounds.bottom,image->file);
  /*
    Write pack type, pack size, resolution, pixel type, and pixel size.
  */
  MSBFirstWriteShort(pixmap.version,image->file);
  MSBFirstWriteShort(pixmap.pack_type,image->file);
  MSBFirstWriteLong(pixmap.pack_size,image->file);
  MSBFirstWriteLong(pixmap.horizontal_resolution,image->file);
  MSBFirstWriteLong(pixmap.vertical_resolution,image->file);
  MSBFirstWriteShort(pixmap.pixel_type,image->file);
  MSBFirstWriteShort(pixmap.bits_per_pixel,image->file);
  /*
    Write component count, size, plane bytes, table size, and reserved.
  */
  MSBFirstWriteShort(pixmap.component_count,image->file);
  MSBFirstWriteShort(pixmap.component_size,image->file);
  MSBFirstWriteLong((unsigned long) pixmap.plane_bytes,image->file);
  MSBFirstWriteLong((unsigned long) pixmap.table,image->file);
  MSBFirstWriteLong((unsigned long) pixmap.reserved,image->file);
  if (image->class == PseudoClass)
    {
      unsigned short
        red,
        green,
        blue;

      /*
        Write image colormap.
      */
      MSBFirstWriteLong(0x00000000L,image->file);  /* color seed */
      MSBFirstWriteShort(0L,image->file);  /* color flags */
      MSBFirstWriteShort((unsigned short) (image->colors-1),image->file);
      for (i=0; i < (int) image->colors; i++)
      {
        red=(unsigned int) (image->colormap[i].red*65535L)/MaxRGB;
        green=(unsigned int) (image->colormap[i].green*65535L)/MaxRGB;
        blue=(unsigned int) (image->colormap[i].blue*65535L)/MaxRGB;
        MSBFirstWriteShort((unsigned int) i,image->file);
        MSBFirstWriteShort(red,image->file);
        MSBFirstWriteShort(green,image->file);
        MSBFirstWriteShort(blue,image->file);
      }
    }
  /*
    Write source and destination rectangle.
  */
  MSBFirstWriteShort(source_rectangle.top,image->file);
  MSBFirstWriteShort(source_rectangle.left,image->file);
  MSBFirstWriteShort(source_rectangle.right,image->file);
  MSBFirstWriteShort(source_rectangle.bottom,image->file);
  MSBFirstWriteShort(destination_rectangle.top,image->file);
  MSBFirstWriteShort(destination_rectangle.left,image->file);
  MSBFirstWriteShort(destination_rectangle.right,image->file);
  MSBFirstWriteShort(destination_rectangle.bottom,image->file);
  MSBFirstWriteShort(transfer_mode,image->file);
  /*
    Write picture data.
  */
  count=0;
  x=0;
  y=0;
  p=image->pixels;
  if (image->class == PseudoClass)
    {
      register unsigned char
        *index;

      index=scanline;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          *index++=(unsigned char) p->index;
          x++;
          if (x == (int) image->columns)
            {
              count+=PICTEncodeImage(image,scanline,packed_scanline);
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
              index=scanline;
              x=0;
              y++;
            }
        }
        p++;
      }
    }
  else
    {
      register unsigned char
        *blue,
        *green,
        *index,
        *red;

      red=scanline;
      green=scanline+image->columns;
      blue=scanline+2*image->columns;
      index=scanline+3*image->columns;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          *red++=DownScale(p->red);
          *green++=DownScale(p->green);
          *blue++=DownScale(p->blue);
          if (image->matte)
            *index++=DownScale(p->index);
          x++;
          if (x == (int) image->columns)
            {
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
              red=scanline;
              green=scanline+image->columns;
              blue=scanline+2*image->columns;
              index=scanline+3*image->columns;
              count+=PICTEncodeImage(image,scanline,packed_scanline);
              x=0;
              y++;
            }
        }
        p++;
      }
    }
  if (count & 0x1)
    (void) fputc('\0',image->file);
  MSBFirstWriteShort(PictEndOfPictureOp,image->file);
  FreeMemory((char *) scanline);
  FreeMemory((char *) packed_scanline);
  FreeMemory((char *) buffer);
  CloseImage(image);
  return(True);
}

#if defined(HasPNG)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePNGImage writes an image in the Portable Network Graphics
%  encoded image format.
%
%  The format of the WritePNGImage routine is:
%
%      status=WritePNGImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNGImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
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

static void PNGTextChunk(const ImageInfo *image_info,png_info *ping_info,
  char *keyword,char *value)
{
  register int
    i;

  i=ping_info->num_text++;
  ping_info->text[i].key=keyword;
  ping_info->text[i].text=value;
  ping_info->text[i].text_length=Extent(value);
  ping_info->text[i].compression= \
    image_info->compression != NoCompression ? 0 : -1;
}

static void PNGWarning(png_struct *ping,png_const_charp message)
{
  MagickWarning(DelegateWarning,message,(char *) NULL);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  png_info
    *ping_info;

  png_struct
    *ping;

  unsigned char
    chunk[16],
    *png_pixels,
    **scanlines;

  unsigned int
    matte,
    scene;

  unsigned short
    value;

  /*
    Open image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if (image_info->adjoin)
    {
      /*
        Write the MNG signature and MHDR chunk.
      */
      (void) fwrite("\212MNG\r\n\032\n",1,8,image->file);
      MSBFirstWriteLong(0x0000000CL,image->file);
      (void) strcpy((char *) chunk,"MHDR");
      chunk[4]=(image->columns >> 24) & 0xff;
      chunk[5]=(image->columns >> 16) & 0xff;
      chunk[6]=(image->columns >> 8) & 0xff;
      chunk[7]=image->columns & 0xff;
      chunk[8]=(image->rows >> 24) & 0xff;
      chunk[9]=(image->rows >> 16) & 0xff;
      chunk[10]=(image->rows >> 8) & 0xff;
      chunk[11]=image->rows & 0xff;
      chunk[12]=0;  /* tick length */
      chunk[13]=0;
      chunk[14]=0;
      chunk[15]=1;
      (void) fwrite(chunk,1,16,image->file);
      MSBFirstWriteLong(crc32(0,chunk,16),image->file);
    }
  scene=0;
  do
  {
    /*
      Allocate the PNG structures
    */
    ping=png_create_write_struct(PNG_LIBPNG_VER_STRING,(void *) NULL,PNGError,
      PNGWarning);
    if (ping == (png_struct *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    ping_info=png_create_info_struct(ping);
    if (ping_info == (png_info *) NULL)
      {
        png_destroy_write_struct(&ping,(png_info **) NULL);
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    png_init_io(ping,image->file);
    png_pixels=(unsigned char *) NULL;
    scanlines=(unsigned char **) NULL;
    if (setjmp(ping->jmpbuf))
     {
        /*
          PNG write failed.
        */
        png_destroy_write_struct(&ping,&ping_info);
        if (scanlines != (unsigned char **) NULL)
          FreeMemory((char *) scanlines);
        if (png_pixels != (unsigned char *) NULL)
          FreeMemory((char *) png_pixels);
        CloseImage(image);
        return(False);
      }
    /*
      Prepare PNG for writing.
    */
    ping_info->width=image->columns;
    ping_info->height=image->rows;
    ping_info->bit_depth=image->depth;
    if (IsMonochromeImage(image))
      ping_info->bit_depth=1;
    ping_info->color_type=PNG_COLOR_TYPE_RGB;
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        ping_info->valid|=PNG_INFO_pHYs;
        ping_info->phys_unit_type=PNG_RESOLUTION_UNKNOWN;
        ping_info->x_pixels_per_unit=(png_uint_32) image->x_resolution;
        ping_info->y_pixels_per_unit=(png_uint_32) image->y_resolution;
        if (image->units == PixelsPerInchResolution)
          {
            ping_info->phys_unit_type=PNG_RESOLUTION_METER;
            ping_info->x_pixels_per_unit=(png_uint_32)
              (100.0*image->x_resolution/2.54);
            ping_info->y_pixels_per_unit=(png_uint_32)
              (100.0*image->y_resolution/2.54);
          }
        if (image->units == PixelsPerCentimeterResolution)
          {
            ping_info->phys_unit_type=PNG_RESOLUTION_METER;
            ping_info->x_pixels_per_unit=(png_uint_32)
              (100.0*image->x_resolution);
            ping_info->y_pixels_per_unit=(png_uint_32)
              (100.0*image->y_resolution);
          }
      }
    ping_info->valid|=PNG_INFO_bKGD;
    ping_info->background.red=DownScale(image->background_color.red);
    ping_info->background.green=DownScale(image->background_color.green);
    ping_info->background.blue=DownScale(image->background_color.blue);
    ping_info->background.gray=DownScale(Intensity(image->background_color));
    ping_info->background.index=ping_info->background.gray;
    matte=image->matte;
    if (matte)
      {
        /*
          Determine if there is one and only one transparent color.
        */
        ping_info->color_type=PNG_COLOR_TYPE_GRAY_ALPHA;
        p=image->pixels;
        for (i=0; (p->index == Opaque) && (i < (int) (image->packets-1)); i++)
        {
          if (!IsGray(*p))
            ping_info->color_type=PNG_COLOR_TYPE_RGB_ALPHA;
          p++;
        }
        if (p->index != Opaque)
          ping_info->valid|=PNG_INFO_tRNS;
        ping_info->trans_values.red=p->red;
        ping_info->trans_values.green=p->green;
        ping_info->trans_values.blue=p->blue;
        ping_info->trans_values.gray=Intensity(*p);
        ping_info->trans_values.index=DownScale(p->index);
        for ( ; i < (int) image->packets; i++)
        {
          if (!IsGray(*p))
            ping_info->color_type=PNG_COLOR_TYPE_RGB_ALPHA;
          if (p->index != ping_info->trans_values.index)
            if (ColorMatch(ping_info->trans_values,*p,0))
              ping_info->valid&=(~PNG_INFO_tRNS);
          p++;
        }
        if (ping_info->valid & PNG_INFO_tRNS)
          {
            matte=False;
            ping_info->color_type=PNG_COLOR_TYPE_RGB;
          }
      }
    if (IsGrayImage(image))
      ping_info->color_type=PNG_COLOR_TYPE_GRAY;
    else
      if (IsPseudoClass(image))
        {
          /*
            Set image palette.
          */
          ping_info->color_type=PNG_COLOR_TYPE_PALETTE;
          ping_info->num_palette=image->colors;
          ping_info->valid|=PNG_INFO_PLTE;
          ping_info->palette=(png_color *)
            AllocateMemory(image->colors*sizeof(png_color));
          if (ping_info->palette == (png_color *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          for (i=0; i < (int) image->colors; i++)
          {
            ping_info->palette[i].red=DownScale(image->colormap[i].red);
            ping_info->palette[i].green=DownScale(image->colormap[i].green);
            ping_info->palette[i].blue=DownScale(image->colormap[i].blue);
          }
          ping_info->bit_depth=1;
          while ((1 << ping_info->bit_depth) < (int) image->colors)
            ping_info->bit_depth<<=1;
          if (ping_info->valid & PNG_INFO_tRNS)
            {
              /*
                Identify which colormap entry is transparent.
              */
              ping_info->trans=(unsigned char *)
                AllocateMemory(image->colors*sizeof(unsigned char));
              if (ping_info->trans == (unsigned char *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              for (i=0; i < (int) image->colors; i++)
              {
                ping_info->trans[i]=DownScale(Opaque);
                if (ColorMatch(ping_info->trans_values,image->colormap[i],0))
                  {
                    ping_info->trans[i]=ping_info->trans_values.index;
                    break;
                  }
              }
              ping_info->num_trans=i+1;
            }
          /*
            Identify which colormap entry is the background color.
          */
          for (i=0; i < (int) (image->colors-1); i++)
            if (ColorMatch(ping_info->background,image->colormap[i],0))
              break;
          ping_info->background.index=(unsigned short) i;
        }
#if defined(PNG_WRITE_sRGB_SUPPORTED)
    if (image->rendering_intent != UndefinedIntent)
      {
        /*
          Note image rendering intent.
        */
        ping_info->valid|=PNG_INFO_sRGB;
        ping_info->srgb_intent=(int) image->rendering_intent-1;
      }
#endif
    if (image->gamma != 0.0)
      {
        /*
          Note image gamma.
        */
        ping_info->valid|=PNG_INFO_gAMA;
        ping_info->gamma=image->gamma;
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note image chromaticity.
        */
        ping_info->valid|=PNG_INFO_cHRM;
        ping_info->x_red=image->chromaticity.red_primary.x;
        ping_info->y_red=image->chromaticity.red_primary.y;
        ping_info->x_green=image->chromaticity.green_primary.x;
        ping_info->y_green=image->chromaticity.green_primary.y;
        ping_info->x_blue=image->chromaticity.blue_primary.x;
        ping_info->y_blue=image->chromaticity.blue_primary.y;
        ping_info->x_white=image->chromaticity.white_point.x;
        ping_info->y_white=image->chromaticity.white_point.y;
      }
    ping_info->interlace_type=image_info->interlace != NoInterlace;
    /*
      Initialize compression level and filtering.
    */
    png_set_compression_level(ping,Min(image_info->quality/10,9));
    if ((image_info->quality % 10) > 5)
      png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
    else
      if ((image_info->quality % 10) != 5)
        png_set_filter(ping,PNG_FILTER_TYPE_BASE,image_info->quality % 10);
      else
        if ((ping_info->valid & PNG_INFO_tRNS) ||
            (ping_info->color_type == PNG_COLOR_TYPE_GRAY) ||
            (ping_info->color_type == PNG_COLOR_TYPE_PALETTE) ||
            (image_info->quality < 50))
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_NO_FILTERS);
        else
          png_set_filter(ping,PNG_FILTER_TYPE_BASE,PNG_ALL_FILTERS);
    if (image_info->adjoin)
      {
        /*
          Write the DEFI chunk.
        */
        png_set_sig_bytes(ping,8);
        MSBFirstWriteLong(0x00000003L,image->file);
        (void) strcpy((char *) chunk,"DEFI");
        chunk[4]=scene >> 8;  /* scene */
        chunk[5]=scene & 0xff;
        chunk[6]=1;  /* visibility */
        (void) fwrite(chunk,1,7,image->file);
        MSBFirstWriteLong(crc32(0,chunk,7),image->file);
      }
    png_write_info(ping,ping_info);
    png_set_packing(ping);
    /*
      Allocate memory.
    */
    ping_info->rowbytes=image->columns;
    if (!IsPseudoClass(image))
      ping_info->rowbytes*=(matte ? 4 : 3)*(image->depth >> 3);
    png_pixels=(unsigned char *)
      AllocateMemory(ping_info->rowbytes*image->rows*sizeof(Quantum));
    scanlines=(unsigned char **)
      AllocateMemory(image->rows*sizeof(unsigned char *));
    if ((png_pixels == (unsigned char *) NULL) ||
        (scanlines == (unsigned char **) NULL))
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Initialize image scanlines.
    */
    for (i=0; i < (int) image->rows; i++)
      scanlines[i]=png_pixels+(ping_info->rowbytes*i);
    x=0;
    y=0;
    p=image->pixels;
    q=scanlines[y];
    if (IsMonochromeImage(image))
      {
        register unsigned char
          polarity;

        /*
          Convert PseudoClass image to a PNG monochrome image.
        */
        polarity=Intensity(image->colormap[0]) < (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(image->colormap[1]) > Intensity(image->colormap[0]);
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(p->index == polarity ? 1 : 0,q);
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                x=0;
                y++;
                q=scanlines[y];
              }
          }
          p++;
        }
      }
    else
      if (IsGrayImage(image))
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(Intensity(*p),q);
            x++;
            if (x == (int) image->columns)
              {
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                x=0;
                y++;
                q=scanlines[y];
              }
          }
          p++;
        }
      else
        if (!IsPseudoClass(image))
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              WriteQuantum(p->red,q);
              if (ping_info->color_type != PNG_COLOR_TYPE_GRAY_ALPHA)
                {
                  WriteQuantum(p->green,q);
                  WriteQuantum(p->blue,q);
                }
              if (matte)
                WriteQuantum(p->index,q);
              x++;
              if (x == (int) image->columns)
                {
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  x=0;
                  y++;
                  if (y < image->rows)
                    q=scanlines[y];
                }
            }
            p++;
          }
        else
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=p->index;
              x++;
              if (x == (int) image->columns)
                {
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  x=0;
                  y++;
                  if (y < image->rows)
                    q=scanlines[y];
                }
            }
            p++;
          }
    png_write_image(ping,scanlines);
    /*
      Generate text chunks.
    */
    ping_info->num_text=0;
    ping_info->text=(png_text *) AllocateMemory(256*sizeof(png_text));
    if (ping_info->text == (png_text *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    PNGTextChunk(image_info,ping_info,"Software",MagickVersion);
    SignatureImage(image);
    if (image->signature != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Signature",image->signature);
    if (image->scene != 0)
      {
        char
          scene[MaxTextExtent];

        FormatString(scene,"%u",image->scene);
        PNGTextChunk(image_info,ping_info,"Scene",scene);
      }
    if (image->delay != 0)
      {
        char
          delay[MaxTextExtent];

        FormatString(delay,"%u",image->delay);
        PNGTextChunk(image_info,ping_info,"Delay",delay);
      }
    if (image->label != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Label",image->label);
    if (image->page != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Page",image->page);
    if (image->montage != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Montage",image->montage);
    if (image->directory != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Directory",image->directory);
    if (image->comments != (char *) NULL)
      PNGTextChunk(image_info,ping_info,"Comment",image->comments);
    png_write_end(ping,ping_info);
    png_destroy_write_struct(&ping,&ping_info);
    /*
      Free PNG resources.
    */
    FreeMemory((char *) scanlines);
    FreeMemory((char *) png_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  if (image_info->adjoin)
    {
      /*
        Write the MEND chunk.
      */
      MSBFirstWriteLong(0x00000000L,image->file);
      (void) strcpy((char *) chunk,"MEND");
      (void) fwrite(chunk,1,4,image->file);
      MSBFirstWriteLong(crc32(0,chunk,4),image->file);
    }
  /*
    Free memory.
  */
  CloseImage(image);
  return(True);
}
#else
unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"PNG library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WritePNMImage writes an image to a file in the PNM rasterfile
%  format.
%
%  The format of the WritePNMImage routine is:
%
%      status=WritePNMImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePNMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePNMImage(const ImageInfo *image_info,Image *image)
{
#define MaxRawValue  255

  char
    *magick;

  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned char
    format;

  unsigned int
    scene;

  unsigned short
    index;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Promote/Demote image based on image type.
    */
    (void) IsPseudoClass(image);
    if (Latin1Compare(image_info->magick,"PPM") == 0)
      image->class=DirectClass;
    magick=(char *) image_info->magick;
    if (((Latin1Compare(magick,"PGM") == 0) && !IsGrayImage(image)) ||
        ((Latin1Compare(magick,"PBM") == 0) && !IsMonochromeImage(image)))
      {
        QuantizeInfo
          quantize_info;

        GetQuantizeInfo(&quantize_info);
        quantize_info.number_colors=MaxRGB+1;
        if (Latin1Compare(image_info->magick,"PBM") == 0)
          quantize_info.number_colors=2;
        quantize_info.dither=image_info->dither;
        quantize_info.colorspace=GRAYColorspace;
        (void) QuantizeImage(&quantize_info,image);
        SyncImage(image);
      }
    /*
      Write PNM file header.
    */
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        /*
          Full color PNM image.
        */
        format='6';
        if ((image_info->compression == NoCompression) ||
            (MaxRGB > MaxRawValue))
          format='3';
      }
    else
      {
        /*
          Colormapped PNM image.
        */
        format='6';
        if ((image_info->compression == NoCompression) ||
            (MaxRGB > MaxRawValue))
          format='3';
        if ((Latin1Compare(magick,"PPM") != 0) && IsGrayImage(image))
          {
            /*
              Grayscale PNM image.
            */
            format='5';
            if ((image_info->compression == NoCompression) ||
                (MaxRGB > MaxRawValue))
              format='2';
            if (Latin1Compare(magick,"PGM") != 0)
              if (image->colors == 2)
                {
                  format='4';
                  if (image_info->compression == NoCompression)
                    format='1';
                }
          }
      }
    if (Latin1Compare(magick,"P7") == 0)
      {
        format='7';
        (void) fprintf(image->file,"P7 332\n");
      }
    else
      (void) fprintf(image->file,"P%c\n",format);
    if (image->comments != (char *) NULL)
      {
        register char
          *p;

        /*
          Write comments to file.
        */
        (void) fprintf(image->file,"#");
        for (p=image->comments; *p != '\0'; p++)
        {
          (void) fputc(*p,image->file);
          if ((*p == '\n') && (*(p+1) != '\0'))
            (void) fprintf(image->file,"#");
        }
        (void) fputc('\n',image->file);
      }
    if (format != '7')
      (void) fprintf(image->file,"%u %u\n",image->columns,image->rows);
    /*
      Convert runlength encoded to PNM raster pixels.
    */
    x=0;
    y=0;
    p=image->pixels;
    switch (format)
    {
      case '1':
      {
        register unsigned char
          polarity;

        /*
          Convert image to a PBM image.
        */
        polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            (void) fprintf(image->file,"%d ",(int) (p->index == polarity));
            x++;
            if (x == 36)
              {
                (void) fprintf(image->file,"\n");
                x=0;
              }
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        break;
      }
      case '2':
      {
        /*
          Convert image to a PGM image.
        */
        (void) fprintf(image->file,"%d\n",MaxRGB);
        for (i=0; i < (int) image->packets; i++)
        {
          index=DownScale(Intensity(*p));
          for (j=0; j <= ((int) p->length); j++)
          {
            (void) fprintf(image->file,"%d ",index);
            x++;
            if (x == 12)
              {
                (void) fprintf(image->file,"\n");
                x=0;
              }
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        break;
      }
      case '3':
      {
        /*
          Convert image to a PNM image.
        */
        (void) fprintf(image->file,"%d\n",MaxRGB);
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            (void) fprintf(image->file,"%d %d %d ",p->red,p->green,p->blue);
            x++;
            if (x == 4)
              {
                (void) fprintf(image->file,"\n");
                x=0;
              }
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        break;
      }
      case '4':
      {
        register unsigned char
          bit,
          byte,
          polarity;

        /*
          Convert image to a PBM image.
        */
        polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
        if (image->colors == 2)
          polarity=
            Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
        bit=0;
        byte=0;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            byte<<=1;
            if (p->index == polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                (void) fputc(byte,image->file);
                bit=0;
                byte=0;
              }
            x++;
            if (x == (int) image->columns)
              {
                /*
                  Advance to the next scanline.
                */
                if (bit != 0)
                  (void) fputc(byte << (8-bit),image->file);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                bit=0;
                byte=0;
                x=0;
                y++;
             }
          }
          p++;
        }
        break;
      }
      case '5':
      {
        /*
          Convert image to a PGM image.
        */
        (void) fprintf(image->file,"%u\n",DownScale(MaxRGB));
        for (i=0; i < (int) image->packets; i++)
        {
          index=DownScale(Intensity(*p));
          for (j=0; j <= ((int) p->length); j++)
            (void) fputc(index,image->file);
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
        break;
      }
      case '6':
      {
        register unsigned char
          *q;

        unsigned char
          *pixels;

        /*
          Allocate memory for pixels.
        */
        pixels=(unsigned char *)
          AllocateMemory(image->columns*sizeof(RunlengthPacket));
        if (pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        /*
          Convert image to a PNM image.
        */
        (void) fprintf(image->file,"%u\n",DownScale(MaxRGB));
        q=pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            x++;
            if (x == (int) image->columns)
              {
                (void) fwrite((char *) pixels,1,q-pixels,image->file);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
        FreeMemory((char *) pixels);
        break;
      }
      case '7':
      {
        static const short int
          dither_red[2][16]=
          {
            {-16,  4, -1, 11,-14,  6, -3,  9,-15,  5, -2, 10,-13,  7, -4,  8},
            { 15, -5,  0,-12, 13, -7,  2,-10, 14, -6,  1,-11, 12, -8,  3, -9}
          },
          dither_green[2][16]=
          {
            { 11,-15,  7, -3,  8,-14,  4, -2, 10,-16,  6, -4,  9,-13,  5, -1},
            {-12, 14, -8,  2, -9, 13, -5,  1,-11, 15, -7,  3,-10, 12, -6,  0}
          },
          dither_blue[2][16]=
          {
            { -3,  9,-13,  7, -1, 11,-15,  5, -4,  8,-14,  6, -2, 10,-16,  4},
            {  2,-10, 12, -8,  0,-12, 14, -6,  3, -9, 13, -7,  1,-11, 15, -5}
          };

        int
          value;

        Quantum
          pixel;

        unsigned short
          *blue_map[2][16],
          *green_map[2][16],
          *red_map[2][16];

        if (!UncondenseImage(image))
          break;
        /*
          Allocate and initialize dither maps.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
          {
            red_map[i][j]=(unsigned short *)
              AllocateMemory(256*sizeof(unsigned short));
            green_map[i][j]=(unsigned short *)
              AllocateMemory(256*sizeof(unsigned short));
            blue_map[i][j]=(unsigned short *)
              AllocateMemory(256*sizeof(unsigned short));
            if ((red_map[i][j] == (unsigned short *) NULL) ||
                (green_map[i][j] == (unsigned short *) NULL) ||
                (blue_map[i][j] == (unsigned short *) NULL))
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
          }
        /*
          Initialize dither tables.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
            for (x=0; x < 256; x++)
            {
              value=x-16;
              if (x < 48)
                value=x/2+8;
              value+=dither_red[i][j];
              red_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
              value=x-16;
              if (x < 48)
                value=x/2+8;
              value+=dither_green[i][j];
              green_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
              value=x-32;
              if (x < 112)
                value=x/2+24;
              value+=(dither_blue[i][j] << 1);
              blue_map[i][j][x]=(unsigned short)
                ((value < 0) ? 0 : (value > MaxRGB) ? MaxRGB : value);
            }
        /*
          Convert image to a P7 image.
        */
        (void) fprintf(image->file,"#END_OF_COMMENTS\n");
        (void) fprintf(image->file,"%u %u %d\n",image->columns,image->rows,
          MaxRGB);
        i=0;
        j=0;
        p=image->pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=(Quantum) ((red_map[i][j][p->red] & 0xe0) |
              ((unsigned int) (green_map[i][j][p->green] & 0xe0) >> 3) |
              ((unsigned int) (blue_map[i][j][p->blue] & 0xc0) >> 6));
            WriteQuantumFile(pixel);
            p++;
            j++;
            if (j == 16)
              j=0;
          }
          i++;
          if (i == 2)
            i=0;
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
        }
        /*
          Free allocated memory.
        */
        for (i=0; i < 2; i++)
          for (j=0; j < 16; j++)
          {
            FreeMemory((char *) green_map[i][j]);
            FreeMemory((char *) blue_map[i][j]);
            FreeMemory((char *) red_map[i][j]);
          }
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P R E V I E W I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePREVIEWImage creates several tiles each with a varying
%  stength of an image enhancement function (e.g. gamma).  The image is written
%  in the MIFF format.
%
%  The format of the WritePREVIEWImage routine is:
%
%      status=WritePREVIEWImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePREVIEWImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePREVIEWImage(const ImageInfo *image_info,Image *image)
{
#define NumberTiles  9
#define PreviewImageText  "  Creating image preview...  "

  char
    *commands[NumberTiles+6],
    factor[MaxTextExtent],
    label[MaxTextExtent];

  double
    degrees,
    gamma,
    percentage,
    threshold;

  Image
    *images[NumberTiles],
    *montage_image,
    *preview_image;

  ImageInfo
    *local_info;

  int
    argc,
    x,
    y;

  MonitorHandler
    handler;

  MontageInfo
    montage_info;

  register int
    i;

  unsigned int
    colors,
    height,
    status,
    width;

  /*
    Scale the image to tile size.
  */
  width=image->columns;
  height=image->rows;
  x=0;
  y=0;
  (void) ParseImageGeometry(DefaultPreviewGeometry,&x,&y,&width,&height);
  image->orphan=True;
  preview_image=ZoomImage(image,width,height);
  image->orphan=False;
  if (preview_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  LabelImage(preview_image,DefaultTileLabel);
  /*
    Apply enhancement at varying strengths.
  */
  local_info=CloneImageInfo(image_info);
  if (local_info == (ImageInfo *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  local_info->quality=0;
  degrees=0;
  gamma=(double) (-0.2);
  colors=2;
  x=0;
  y=0;
  percentage=0.0;
  threshold=0.0;
  commands[0]=SetClientName((char *) NULL);
  for (i=0; i < NumberTiles; i++)
  {
    images[i]=
      CloneImage(preview_image,preview_image->columns,preview_image->rows,True);
    if (images[i] == (Image *) NULL)
      {
        for (x=0;  x < i; x++)
          DestroyImage(images[x]);
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      }
    argc=1;
    if (i == (NumberTiles >> 1))
      {
        commands[argc++]="-mattecolor";
        commands[argc++]=DefaultPreviewMatte;
        MogrifyImage(local_info,argc,commands,&images[i]);
        continue;
      }
    handler=SetMonitorHandler((MonitorHandler) NULL);
    switch (image_info->preview_type)
    {
      case RotatePreview:
      {
        FormatString(factor,"%.1f",degrees+=45.0);
        FormatString(label,"rotate %.1024s",factor);
        commands[argc++]="-rotate";
        commands[argc++]=factor;
        commands[argc++]="-crop";
        commands[argc++]="0x0";
        break;
      }
      case ShearPreview:
      {
        degrees+=10.0;
        FormatString(factor,"%.1fx%.1f",degrees,2.0*degrees);
        FormatString(label,"shear %.1024s",factor);
        commands[argc++]="-shear";
        commands[argc++]=factor;
        commands[argc++]="-crop";
        commands[argc++]="0x0";
        break;
      }
      case RollPreview:
      {
        x+=preview_image->columns/NumberTiles;
        y+=preview_image->rows/NumberTiles;
        FormatString(factor,"%+d%+d",x,y);
        FormatString(label,"roll %.1024s",factor);
        commands[argc++]="-roll";
        commands[argc++]=factor;
        break;
      }
      case HuePreview:
      {
        FormatString(factor,"0,0,%.1f",percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]="-modulate";
        commands[argc++]=factor;
        break;
      }
      case SaturationPreview:
      {
        FormatString(factor,"0,%.1f",percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]="-modulate";
        commands[argc++]=factor;
        break;
      }
      case BrightnessPreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"modulate %.1024s",factor);
        commands[argc++]="-modulate";
        commands[argc++]=factor;
        break;
      }
      case GammaPreview:
      {
        FormatString(factor,"%.1f",gamma+=(double) 0.4);
        FormatString(label,"gamma %.1024s",factor);
        commands[argc++]="-gamma";
        commands[argc++]=factor;
        break;
      }
      case SpiffPreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]="-contrast";
        FormatString(label,"-contast %d",i+1);
        break;
      }
      case DullPreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]="+contrast";
        FormatString(label,"+contast %d",i+1);
        break;
      }
      case GrayscalePreview:
      {
        FormatString(factor,"%u",colors);
        colors<<=1;
        FormatString(label,"colors %.1024s",factor);
        commands[argc++]="-colorspace";
        commands[argc++]="gray";
        commands[argc++]="-colors";
        commands[argc++]=factor;
        break;
      }
      case QuantizePreview:
      {
        FormatString(factor,"%u",colors<<=1);
        FormatString(label,"colors %.1024s",factor);
        commands[argc++]="-colors";
        commands[argc++]=factor;
        break;
      }
      case DespecklePreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]="-despeckle";
        FormatString(label,"despeckle %d",i+1);
        break;
      }
      case ReduceNoisePreview:
      {
        for (x=0; x < i; x++)
          commands[argc++]="-noise";
        FormatString(label,"noise %d",i+1);
        break;
      }
      case AddNoisePreview:
      {
        switch (x)
        {
          case 0: (void) strcpy(factor,"uniform"); break;
          case 1: (void) strcpy(factor,"gaussian"); break;
          case 2: (void) strcpy(factor,"multiplicative"); break;
          case 3: (void) strcpy(factor,"impulse"); break;
          case 4: (void) strcpy(factor,"laplacian"); break;
          case 5: (void) strcpy(factor,"poisson"); break;
          default: (void) strcpy(images[i]->magick,"NULL"); break;
        }
        x++;
        FormatString(label,"+noise %.1024s",factor);
        commands[argc++]="+noise";
        commands[argc++]=factor;
        break;
      }
      case SharpenPreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"sharpen %.1024s",factor);
        commands[argc++]="-sharpen";
        commands[argc++]=factor;
        break;
      }
      case BlurPreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"-blur %.1024s",factor);
        commands[argc++]="-blur";
        commands[argc++]=factor;
        break;
      }
      case ThresholdPreview:
      {
        FormatString(factor,"%d",(int) ((percentage*(MaxRGB+1))/100));
        FormatString(label,"threshold %.1024s",factor);
        commands[argc++]="-threshold";
        commands[argc++]=factor;
        break;
      }
      case EdgeDetectPreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"edge %.1024s",factor);
        commands[argc++]="-edge";
        commands[argc++]=factor;
        break;
      }
      case SpreadPreview:
      {
        FormatString(factor,"%d",i+1);
        FormatString(label,"spread %.1024s",factor);
        commands[argc++]="-spread";
        commands[argc++]=factor;
        break;
      }
      case SolarizePreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"solarize %.1024s",factor);
        commands[argc++]="-solarize";
        commands[argc++]=factor;
        break;
      }
      case ShadePreview:
      {
        if (i == 0)
          {
            (void) strcpy(label,"+shade");
            commands[argc++]="+shade";
            break;
          }
        degrees+=10.0;
        FormatString(factor,"%.1fx%.1f",degrees,degrees);
        FormatString(label,"shade %.1024s",factor);
        commands[argc++]="-shade";
        commands[argc++]=factor;
        break;
      }
      case RaisePreview:
      {
        FormatString(factor,"%d",i+1);
        FormatString(label,"raise %.1024s",factor);
        commands[argc++]="-raise";
        commands[argc++]=factor;
        break;
      }
      case SegmentPreview:
      {
        threshold+=(double) 0.4;
        FormatString(factor,"%.1fx%.1f",threshold,threshold);
        FormatString(label,"segment %.1024s",factor);
        commands[argc++]="-colors";
        commands[argc++]=factor;
        break;
      }
      case SwirlPreview:
      {
        FormatString(factor,"%.1f",degrees+=45.0);
        FormatString(label,"swirl %.1024s",factor);
        commands[argc++]="-swirl";
        commands[argc++]=factor;
        break;
      }
      case ImplodePreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"implode %.1024s",factor);
        commands[argc++]="-implode";
        commands[argc++]=factor;
        break;
      }
      case WavePreview:
      {
        degrees+=5.0;
        FormatString(factor,"%.1fx%.1f",degrees,degrees);
        FormatString(label,"wave %.1024s",factor);
        commands[argc++]="-implode";
        commands[argc++]=factor;
        break;
      }
      case OilPaintPreview:
      {
        FormatString(factor,"%i",i+1);
        FormatString(label,"paint %.1024s",factor);
        commands[argc++]="-paint";
        commands[argc++]=factor;
        break;
      }
      case CharcoalDrawingPreview:
      {
        FormatString(factor,"%.1f",percentage);
        FormatString(label,"charcoal %.1024s",factor);
        commands[argc++]="-charcoal";
        commands[argc++]=factor;
        break;
      }
      case JPEGPreview:
      default:
      {
        local_info->quality=(unsigned int) (percentage+13.0);
        FormatString(factor,"%u",local_info->quality);
        TemporaryFilename(images[i]->filename);
        status=WriteJPEGImage(local_info,images[i]);
        if (status != False)
          {
            Image
              *quality_image;

            (void) strcpy(local_info->filename,images[i]->filename);
            quality_image=ReadImage(local_info);
            (void) remove(images[i]->filename);
            if (quality_image != (Image *) NULL)
              {
                DestroyImage(images[i]);
                images[i]=quality_image;
              }
          }
        FormatString(label,"quality %.1024s\n%ldb ",factor,images[i]->filesize);
        if (images[i]->filesize >= (1 << 24))
          FormatString(label,"quality %.1024s\n%ldmb ",factor,
            images[i]->filesize/1024/1024);
        else
          if (images[i]->filesize >= (1 << 14))
            FormatString(label,"quality %.1024s\n%ldkb ",factor,
              images[i]->filesize/1024);
        break;
      }
    }
    percentage+=12.5;
    commands[argc++]="-label";
    commands[argc++]=label;
    MogrifyImage(local_info,argc,commands,&images[i]);
    (void) SetMonitorHandler(handler);
    ProgressMonitor(PreviewImageText,i,NumberTiles);
  }
  DestroyImageInfo(local_info);
  DestroyImage(preview_image);
  /*
    Create the PCD Overview image.
  */
  for (i=1; i < NumberTiles; i++)
  {
    images[i]->previous=images[i-1];
    images[i-1]->next=images[i];
  }
  GetMontageInfo(&montage_info);
  (void) strcpy(montage_info.filename,image->filename);
  CloneString(&montage_info.geometry,DefaultPreviewGeometry);
  CloneString(&montage_info.tile,DefaultPreviewPageGeometry);
  CloneString(&montage_info.font,image_info->font);
  montage_info.pointsize=image_info->pointsize;
  CloneString(&montage_info.frame,DefaultTileFrame);
  montage_info.shadow=True;
  montage_image=MontageImages(*images,&montage_info);
  DestroyMontageInfo(&montage_info);
  for (i=0;  i < i; i++)
    DestroyImage(images[i]);
  if (montage_image == (Image *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  if (montage_image->montage != (char *) NULL)
    {
      /*
        Free image directory.
      */
      FreeMemory(montage_image->montage);
      montage_image->montage=(char *) NULL;
      if (image->directory != (char *) NULL)
        {
          FreeMemory(montage_image->directory);
          montage_image->directory=(char *) NULL;
        }
    }
  status=WriteMIFFImage(image_info,montage_image);
  DestroyImage(montage_image);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePSImage translates an image to encapsulated Postscript
%  Level I for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePSImage routine is:
%
%      status=WritePSImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePSImage return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
unsigned int WritePSImage(const ImageInfo *image_info,Image *image)
{
  static const char
    *PostscriptProlog[]=
    {
      "%%BeginProlog",
      "%",
      "% Display a color image.  The image is displayed in color on",
      "% Postscript viewers or printers that support color, otherwise",
      "% it is displayed as grayscale.",
      "%",
      "/buffer 512 string def",
      "/byte 1 string def",
      "/color_packet 3 string def",
      "/pixels 768 string def",
      "",
      "/DirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet.",
      "  %",
      "  % Parameters:",
      "  %   red.",
      "  %   green.",
      "  %   blue.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/DirectClassImage",
      "{",
      "  %",
      "  % Display a DirectClass image.",
      "  %",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { DirectClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayDirectClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/GrayDirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters:",
      "  %   red",
      "  %   green",
      "  %   blue",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/GrayPseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters:",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet.",
      "  %",
      "  % Parameters:",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %",
      "  % Display a PseudoClass image.",
      "  %",
      "  % Parameters:",
      "  %   class: 0-PseudoClass or 1-Grayscale.",
      "  %",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  class 0 gt",
      "  {",
      "    currentfile buffer readline pop",
      "    token pop /depth exch def pop",
      "    /grays columns 8 add depth sub depth mul 8 idiv string def",
      "    columns rows depth",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { currentfile grays readhexstring pop } image",
      "  }",
      "  {",
      "    %",
      "    % Parameters:",
      "    %   colors: number of colors in the colormap.",
      "    %   colormap: red, green, blue color packets.",
      "    %",
      "    currentfile buffer readline pop",
      "    token pop /colors exch def pop",
      "    /colors colors 3 mul def",
      "    /colormap colors string def",
      "    currentfile colormap readhexstring pop pop",
      "    systemdict /colorimage known",
      "    {",
      "      columns rows 8",
      "      [",
      "        columns 0 0",
      "        rows neg 0 rows",
      "      ]",
      "      { PseudoClassPacket } false 3 colorimage",
      "    }",
      "    {",
      "      %",
      "      % No colorimage operator;  convert to grayscale.",
      "      %",
      "      columns rows 8",
      "      [",
      "        columns 0 0",
      "        rows neg 0 rows",
      "      ]",
      "      { GrayPseudoClassPacket } image",
      "    } ifelse",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %",
      "  % Display a DirectClass or PseudoClass image.",
      "  %",
      "  % Parameters:",
      "  %   x & y translation.",
      "  %   x & y scale.",
      "  %   label pointsize.",
      "  %   image label.",
      "  %   image columns & rows.",
      "  %   class: 0-DirectClass or 1-PseudoClass.",
      "  %   compression: 0-RunlengthEncodedCompression or 1-NoCompression.",
      "  %   hex color packets.",
      "  %",
      "  gsave",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      "  /Helvetica findfont pointsize scalefont setfont",
      (char *) NULL
    },
    *PostscriptEpilog[]=
    {
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  grestore",
      (char *) NULL
    };

  char
    date[MaxTextExtent],
    density[MaxTextExtent],
    **labels;

  const char
    **q;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  int
    length,
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  time_t
    timer;

  unsigned int
    bit,
    byte,
    count,
    height,
    page,
    polarity,
    scene,
    text_size,
    width;

  XSegment
    bounding_box;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  page=1;
  scene=0;
  do
  {
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    if (image_info->page != (char *) NULL)
      (void) ParseImageGeometry(image_info->page,&x,&y,&width,&height);
    else
      if (image->page != (char *) NULL)
        (void) ParseImageGeometry(image->page,&x,&y,&width,&height);
      else
        if (Latin1Compare(image_info->magick,"PS") == 0)
          (void) ParseImageGeometry(PSPageGeometry,&x,&y,&width,&height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (Latin1Compare(image_info->magick,"PS") == 0)
          (void) fprintf(image->file,"%%!PS-Adobe-3.0\n");
        else
          (void) fprintf(image->file,"%%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) fprintf(image->file,"%%%%Creator: (ImageMagick)\n");
        (void) fprintf(image->file,"%%%%Title: (%.1024s)\n",image->filename);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strcpy(date,ctime(&timer));
        date[Extent(date)-1]='\0';
        (void) fprintf(image->file,"%%%%CreationDate: (%.1024s)\n",date);
        bounding_box.x1=x;
        bounding_box.y1=y;
        bounding_box.x2=x+(int) x_scale;
        bounding_box.y2=y+(int) (height+text_size);
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) fprintf(image->file,"%%%%BoundingBox: (atend)\n");
        else
          (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",
            bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
        if (image->label != (char *) NULL)
          (void) fprintf(image->file,
            "%%%%DocumentNeededResources: font Helvetica\n");
        (void) fprintf(image->file,"%%%%DocumentData: Clean7Bit\n");
        (void) fprintf(image->file,"%%%%LanguageLevel: 1\n");
        if (Latin1Compare(image_info->magick,"PS") != 0)
          (void) fprintf(image->file,"%%%%Pages: 0\n");
        else
          {
            Image
              *next_image;

            unsigned int
              pages;

            /*
              Compute the number of pages.
            */
            (void) fprintf(image->file,"%%%%Orientation: Portrait\n");
            (void) fprintf(image->file,"%%%%PageOrder: Ascend\n");
            pages=1;
            if (image_info->adjoin)
              for (next_image=image->next; next_image != (Image *) NULL; )
              {
                next_image=next_image->next;
                pages++;
              }
            (void) fprintf(image->file,"%%%%Pages: %u\n",pages);
          }
        (void) fprintf(image->file,"%%%%EndComments\n");
        (void) fprintf(image->file,"\n%%%%BeginDefaults\n");
        (void) fprintf(image->file,"%%%%PageOrientation: Portrait\n");
        (void) fprintf(image->file,"%%%%EndDefaults\n\n");
        if ((Latin1Compare(image_info->magick,"EPI") == 0) ||
            (Latin1Compare(image_info->magick,"EPT") == 0) ||
            (Latin1Compare(image_info->magick,"EPSI") == 0))
          {
            Image
              *preview_image;

            /*
              Create preview image.
            */
            image->orphan=True;
            preview_image=CloneImage(image,image->columns,image->rows,True);
            image->orphan=False;
            if (preview_image == (Image *) NULL)
              PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                image);
            /*
              Dump image as bitmap.
            */
            if (!IsMonochromeImage(preview_image))
              {
                QuantizeInfo
                  quantize_info;

                GetQuantizeInfo(&quantize_info);
                quantize_info.number_colors=2;
                quantize_info.dither=image_info->dither;
                quantize_info.colorspace=GRAYColorspace;
                (void) QuantizeImage(&quantize_info,preview_image);
                SyncImage(preview_image);
              }
            polarity=Intensity(preview_image->colormap[0]) < (MaxRGB >> 1);
            if (preview_image->colors == 2)
              polarity=Intensity(preview_image->colormap[0]) >
                Intensity(preview_image->colormap[1]);
            bit=0;
            byte=0;
            count=0;
            x=0;
            p=preview_image->pixels;
            (void) fprintf(image->file,"%%%%BeginPreview: %u %u %u %u\n%%  ",
              preview_image->columns,preview_image->rows,(unsigned int) 1,
              (((preview_image->columns+7) >> 3)*preview_image->rows+35)/36);
            for (i=0; i < (int) preview_image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                byte<<=1;
                if (p->index == polarity)
                  byte|=0x01;
                bit++;
                if (bit == 8)
                  {
                    (void) fprintf(image->file,"%02x",byte & 0xff);
                    count++;
                    if (count == 36)
                      {
                        (void) fprintf(image->file,"\n%%  ");
                        count=0;
                      };
                    bit=0;
                    byte=0;
                  }
                x++;
                if (x == (int) preview_image->columns)
                  {
                    if (bit != 0)
                      {
                        byte<<=(8-bit);
                        (void) fprintf(image->file,"%02x",byte & 0xff);
                        count++;
                        if (count == 36)
                          {
                            (void) fprintf(image->file,"\n%%  ");
                            count=0;
                          };
                        bit=0;
                        byte=0;
                      };
                    x=0;
                  }
                }
                p++;
              }
              (void) fprintf(image->file,"\n%%%%EndPreview\n");
              DestroyImage(preview_image);
            }
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
          (void) fprintf(image->file,"%.255s\n",*q);
        for (i=MultilineCensus(image->label)-1; i >= 0; i--)
        {
          (void) fprintf(image->file,"  /label 512 string def\n");
          (void) fprintf(image->file,"  currentfile label readline pop\n");
          (void) fprintf(image->file,"  0 y %d add moveto label show pop\n",
            i*image_info->pointsize+12);
        }
        for (q=PostscriptEpilog; *q; q++)
          (void) fprintf(image->file,"%.255s\n",*q);
        if (Latin1Compare(image_info->magick,"PS") == 0)
          (void) fprintf(image->file,"  showpage\n");
        (void) fprintf(image->file,"} bind def\n");
        (void) fprintf(image->file,"%%%%EndProlog\n");
      }
    (void) fprintf(image->file,"%%%%Page:  1 %u\n",page++);
    (void) fprintf(image->file,"%%%%PageBoundingBox: %d %d %d %d\n",x,y,
      x+(int) width,y+(int) (height+text_size));
    if (x < bounding_box.x1)
      bounding_box.x1=x;
    if (y < bounding_box.y1)
      bounding_box.y1=y;
    if ((x+(int) width-1) > bounding_box.x2)
      bounding_box.x2=x+(int) width-1;
    if ((y+(int) (height+text_size)-1) > bounding_box.y2)
      bounding_box.y2=y+(int) (height+text_size)-1;
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"%%%%PageResources: font Helvetica\n");
    if (Latin1Compare(image_info->magick,"PS") != 0)
      (void) fprintf(image->file,"userdict begin\n");
    (void) fprintf(image->file,"%%%%BeginData:\n");
    (void) fprintf(image->file,"DisplayImage\n");
    /*
      Output image data.
    */
    labels=StringToList(image->label);
    (void) fprintf(image->file,"%d %d\n%f %f\n%u\n",x,y,x_scale,y_scale,
      image_info->pointsize);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) fprintf(image->file,"%.1024s \n",labels[i]);
          FreeMemory(labels[i]);
        }
        FreeMemory(labels);
      }
    x=0;
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        /*
          Dump DirectClass image.
        */
        (void) fprintf(image->file,"%u %u\n%d\n%d\n",
          image->columns,image->rows,(int) (image->class == PseudoClass),
          (int) (image_info->compression == NoCompression));
        switch (image_info->compression)
        {
          case RunlengthEncodedCompression:
          default:
          {
            /*
              Dump runlength-encoded DirectColor packets.
            */
            for (i=0; i < (int) image->packets; i++)
            {
              for (length=p->length; length >= 0; length-=256)
              {
                if (image->matte && (p->index == Transparent))
                  (void) fprintf(image->file,"ffffff%02x",(unsigned int)
                    Min(length,0xff));
                else
                  (void) fprintf(image->file,"%02x%02x%02x%02x",
                    DownScale(p->red),DownScale(p->green),DownScale(p->blue),
                    (unsigned int) Min(length,0xff));
                x++;
                if (x == 9)
                  {
                    x=0;
                    (void) fprintf(image->file,"\n");
                  }
              }
              p++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(SaveImageText,i,image->packets);
            }
            break;
          }
          case NoCompression:
          {
            /*
              Dump uncompressed DirectColor packets.
            */
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                if (image->matte && (p->index == Transparent))
                  (void) fprintf(image->file,"ffffff");
                else
                  (void) fprintf(image->file,"%02x%02x%02x",DownScale(p->red),
                    DownScale(p->green),DownScale(p->blue));
                x++;
                if (x == 12)
                  {
                    x=0;
                    (void) fprintf(image->file,"\n");
                  }
              }
              p++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(SaveImageText,i,image->packets);
            }
            break;
          }
        }
        (void) fprintf(image->file,"\n");
      }
    else
      if (IsGrayImage(image))
        {
          (void) fprintf(image->file,"%u %u\n1\n1\n1\n%d\n",
            image->columns,image->rows,IsMonochromeImage(image) ? 1 : 8);
          if (!IsMonochromeImage(image))
            {
              /*
                Dump image as grayscale.
              */
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  (void) fprintf(image->file,"%02x",DownScale(p->red));
                  x++;
                  if (x == 36)
                    {
                      x=0;
                      (void) fprintf(image->file,"\n");
                    }
                }
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
            }
          else
            {
              int
                y;

              /*
                Dump image as bitmap.
              */
              polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
              if (image->colors == 2)
                polarity=
                  Intensity(image->colormap[1]) > Intensity(image->colormap[0]);
              bit=0;
              byte=0;
              count=0;
              y=0;
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      (void) fprintf(image->file,"%02x",byte & 0xff);
                      count++;
                      if (count == 36)
                        {
                          (void) fprintf(image->file,"\n");
                          count=0;
                        };
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        {
                          byte<<=(8-bit);
                          (void) fprintf(image->file,"%02x",byte & 0xff);
                          count++;
                          if (count == 36)
                            {
                              (void) fprintf(image->file,"\n");
                              count=0;
                            };
                        };
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          ProgressMonitor(SaveImageText,y,image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                      y++;
                    }
                  }
                p++;
              }
            }
          if (count != 0)
            (void) fprintf(image->file,"\n");
        }
      else
        {
          /*
            Dump PseudoClass image.
          */
          (void) fprintf(image->file,"%u %u\n%d\n%d\n0\n",
            image->columns,image->rows,(int) (image->class == PseudoClass),
            (int) (image_info->compression == NoCompression));
          /*
            Dump number of colors and colormap.
          */
          (void) fprintf(image->file,"%u\n",image->colors);
          for (i=0; i < (int) image->colors; i++)
            (void) fprintf(image->file,"%02x%02x%02x\n",
              DownScale(image->colormap[i].red),
              DownScale(image->colormap[i].green),
              DownScale(image->colormap[i].blue));
          switch (image_info->compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              /*
                Dump runlength-encoded PseudoColor packets.
              */
              for (i=0; i < (int) image->packets; i++)
              {
                for (length=p->length; length >= 0; length-=256)
                {
                  (void) fprintf(image->file,"%02x%02x",(unsigned int) p->index,
                    (unsigned int) Min(length,0xff));
                  x++;
                  if (x == 18)
                    {
                      x=0;
                      (void) fprintf(image->file,"\n");
                    }
                }
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  (void) fprintf(image->file,"%02x",(unsigned int) p->index);
                  x++;
                  if (x == 36)
                    {
                      x=0;
                      (void) fprintf(image->file,"\n");
                    }
                }
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              break;
            }
          }
          (void) fprintf(image->file,"\n");
        }
    (void) fprintf(image->file,"%%%%EndData\n");
    if (Latin1Compare(image_info->magick,"PS") != 0)
      (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"%%%%PageTrailer\n");
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) fprintf(image->file,"%%%%Trailer\n");
  if (page > 1)
    (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",
      bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
  (void) fprintf(image->file,"%%%%EOF\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePSDImage writes an image in the Adobe Photoshop encoded image
%  format.
%
%  The format of the WritePSDImage routine is:
%
%      status=WritePSDImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePSDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WritePSDImage(const ImageInfo *image_info,Image *image)
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  (void) fwrite("8BPS",1,4,image->file);
  MSBFirstWriteShort(1,image->file);  /* version */
  (void) fwrite("      ",1,6,image->file);  /* reserved */
  if (image->class == PseudoClass)
    MSBFirstWriteShort(1,image->file);
  else
    MSBFirstWriteShort(image->matte ? 4 : 3,image->file);
  MSBFirstWriteLong(image->rows,image->file);
  MSBFirstWriteLong(image->columns,image->file);
  MSBFirstWriteShort(image->depth,image->file);
  MSBFirstWriteShort(image->class == PseudoClass ? 2 : 3,image->file);
  if ((image->class == DirectClass) || (image->colors > 256))
    MSBFirstWriteLong(0,image->file);
  else
    {
      /*
        Write PSD raster colormap.
      */
      MSBFirstWriteLong(768,image->file);
      for (i=0; i < (int) image->colors; i++)
        (void) fputc(DownScale(image->colormap[i].red),image->file);
      for ( ; i < 256; i++)
        (void) fputc(0,image->file);
      for (i=0; i < (int) image->colors; i++)
        (void) fputc(DownScale(image->colormap[i].green),image->file);
      for ( ; i < 256; i++)
        (void) fputc(0,image->file);
      for (i=0; i < (int) image->colors; i++)
        (void) fputc(DownScale(image->colormap[i].blue),image->file);
      for ( ; i < 256; i++)
        (void) fputc(0,image->file);
    }
  MSBFirstWriteLong(0,image->file);  /* image resource block */
  MSBFirstWriteLong(0,image->file);  /* layer and mask block */
  MSBFirstWriteShort(0,image->file);  /* compression */
  /*
    Write uncompressed pixel data as separate planes.
  */
  p=image->pixels;
  if (image->class == PseudoClass)
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
        WriteQuantumFile(p->index);
      p++;
    }
  else
    {
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          WriteQuantumFile(p->red);
        p++;
      }
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          WriteQuantumFile(p->green);
        p++;
      }
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          WriteQuantumFile(p->blue);
        p++;
      }
      p=image->pixels;
      if (image->matte)
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile(p->index);
          p++;
        }
    }
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P S 2 I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePS2Image translates an image to encapsulated Postscript
%  Level II for printing.  If the supplied geometry is null, the image is
%  centered on the Postscript page.  Otherwise, the image is positioned as
%  specified by the geometry.
%
%  The format of the WritePS2Image routine is:
%
%      status=WritePS2Image(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WritePS2Image return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
unsigned int WritePS2Image(const ImageInfo *image_info,Image *image)
{
  static const char
    *PostscriptProlog[]=
    {
      "%%%%BeginProlog",
      "%%",
      "%% Display a color image.  The image is displayed in color on",
      "%% Postscript viewers or printers that support color, otherwise",
      "%% it is displayed as grayscale.",
      "%%",
      "/buffer 512 string def",
      "/pixel_stream currentfile /ASCII85Decode filter def",
      "",
      "/DirectClassImage",
      "{",
      "  %%",
      "  %% Display a DirectClass image.",
      "  %%",
      "  /DeviceRGB setcolorspace",
      "  <<",
      "    /ImageType 1",
      "    /Width columns",
      "    /Height rows",
      "    /BitsPerComponent 8",
      "    /Decode [0 1 0 1 0 1]",
      "    /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "    compression 0 gt",
      "    { /DataSource pixel_stream }",
      "    { /DataSource pixel_stream /%.1024s filter } ifelse",
      "  >> image",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %%",
      "  %% Display a PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   colors: number of colors in the colormap.",
      "  %%",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  colors 0 eq",
      "  {",
      "    %%",
      "    %% Image is grayscale.",
      "    %%",
      "    /DeviceGray setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 1",
      "      /Decode [0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream }",
      "      {",
      "        /DataSource pixel_stream",
      "        <<",
      "           /K "CCITTParam,
      "           /Columns columns",
      "           /Rows rows",
      "        >> /CCITTFaxDecode filter",
      "      } ifelse",
      "    >> image",
      "  }",
      "  {",
      "    %%",
      "    %% Parameters:",
      "    %%   colormap: red, green, blue color packets.",
      "    %%",
      "    /colormap colors 3 mul string def",
      "    currentfile colormap readhexstring pop pop",
      "    [ /Indexed /DeviceRGB colors 1 sub colormap ] setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 255]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      compression 0 gt",
      "      { /DataSource pixel_stream }",
      "      { /DataSource pixel_stream /%.1024s filter } ifelse",
      "    >> image",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %%",
      "  %% Display a DirectClass or PseudoClass image.",
      "  %%",
      "  %% Parameters:",
      "  %%   x & y translation.",
      "  %%   x & y scale.",
      "  %%   label pointsize.",
      "  %%   image label.",
      "  %%   image columns & rows.",
      "  %%   class: 0-DirectClass or 1-PseudoClass.",
      "  %%   compression: 0-RunlengthEncodedCompression or 1-NoCompression.",
      "  %%   hex color packets.",
      "  %%",
      "  gsave",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      "  /Helvetica findfont pointsize scalefont setfont",
      (char *) NULL
    },
    *PostscriptEpilog[]=
    {
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  pixel_stream flushfile",
      "  grestore",
      (char *) NULL
    };

  char
    date[MaxTextExtent],
    density[MaxTextExtent],
    **labels;

  const char
    **q;

  CompressionType
    compression;

  double
    dx_resolution,
    dy_resolution,
    x_resolution,
    x_scale,
    y_resolution,
    y_scale;

  int
    count,
    status,
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  time_t
    timer;

  unsigned char
    *pixels;

  unsigned int
    height,
    page,
    scene,
    text_size,
    width;

  unsigned long
    number_packets;

  XSegment
    bounding_box;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  compression=image_info->compression;
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
  page=1;
  scene=0;
  do
  {
    /*
      Scale image to size of Postscript page.
    */
    text_size=0;
    if (image->label != (char *) NULL)
      text_size=MultilineCensus(image->label)*image_info->pointsize+12;
    width=image->columns;
    height=image->rows;
    x=0;
    y=text_size;
    if (image_info->page != (char *) NULL)
      (void) ParseImageGeometry(image_info->page,&x,&y,&width,&height);
    else
      if (image->page != (char *) NULL)
        (void) ParseImageGeometry(image->page,&x,&y,&width,&height);
    /*
      Scale relative to dots-per-inch.
    */
    dx_resolution=72.0;
    dy_resolution=72.0;
    x_resolution=72.0;
    (void) strcpy(density,PSDensityGeometry);
    count=sscanf(density,"%lfx%lf",&x_resolution,&y_resolution);
    if (count != 2)
      y_resolution=x_resolution;
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        x_resolution=image->x_resolution;
        y_resolution=image->y_resolution;
      }
    if (image_info->density != (char *) NULL)
      {
        count=sscanf(image_info->density,"%lfx%lf",&x_resolution,&y_resolution);
        if (count != 2)
          y_resolution=x_resolution;
      }
    x_scale=(width*dx_resolution)/x_resolution;
    width=(unsigned int) (x_scale+0.5);
    y_scale=(height*dy_resolution)/y_resolution;
    height=(unsigned int) (y_scale+0.5);
    if (page == 1)
      {
        /*
          Output Postscript header.
        */
        if (Latin1Compare(image_info->magick,"PS2") == 0)
          (void) fprintf(image->file,"%%!PS-Adobe-3.0\n");
        else
          (void) fprintf(image->file,"%%!PS-Adobe-3.0 EPSF-3.0\n");
        (void) fprintf(image->file,"%%%%Creator: (ImageMagick)\n");
        (void) fprintf(image->file,"%%%%Title: (%.1024s)\n",image->filename);
        timer=time((time_t *) NULL);
        (void) localtime(&timer);
        (void) strcpy(date,ctime(&timer));
        date[Extent(date)-1]='\0';
        (void) fprintf(image->file,"%%%%CreationDate: (%.1024s)\n",date);
        bounding_box.x1=x;
        bounding_box.y1=y;
        bounding_box.x2=x+(int) width-1;
        bounding_box.y2=y+(int) (height+text_size)-1;
        if (image_info->adjoin && (image->next != (Image *) NULL))
          (void) fprintf(image->file,"%%%%BoundingBox: (atend)\n");
        else
          (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",
            bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
        if (image->label != (char *) NULL)
          (void) fprintf(image->file,
            "%%%%DocumentNeededResources: font Helvetica\n");
        (void) fprintf(image->file,"%%%%LanguageLevel: 2\n");
        if (Latin1Compare(image_info->magick,"PS2") != 0)
          (void) fprintf(image->file,"%%%%Pages: 0\n");
        else
          {
            Image
             *next_image;

            unsigned int
              pages;

            /*
              Compute the number of pages.
            */
            (void) fprintf(image->file,"%%%%Orientation: Portrait\n");
            (void) fprintf(image->file,"%%%%PageOrder: Ascend\n");
            pages=1;
            if (image_info->adjoin)
              for (next_image=image->next; next_image != (Image *) NULL; )
              {
                next_image=next_image->next;
                pages++;
              }
            (void) fprintf(image->file,"%%%%Pages: %u\n",pages);
          }
        (void) fprintf(image->file,"%%%%EndComments\n");
        (void) fprintf(image->file,"\n%%%%BeginDefaults\n");
        (void) fprintf(image->file,"%%%%PageOrientation: Portrait\n");
        (void) fprintf(image->file,"%%%%EndDefaults\n\n");
        /*
          Output Postscript commands.
        */
        for (q=PostscriptProlog; *q; q++)
        {
          (void) fprintf(image->file,*q,
            compression == ZipCompression ? "FlateDecode" :
            compression == LZWCompression ? "LZWDecode" : "RunLengthDecode");
          (void) fprintf(image->file,"\n");
        }
        for (i=MultilineCensus(image->label)-1; i >= 0; i--)
        {
          (void) fprintf(image->file,"  /label 512 string def\n");
          (void) fprintf(image->file,"  currentfile label readline pop\n");
          (void) fprintf(image->file,"  0 y %d add moveto label show pop\n",
            i*image_info->pointsize+12);
        }
        for (q=PostscriptEpilog; *q; q++)
          (void) fprintf(image->file,"%.255s\n",*q);
        if (Latin1Compare(image_info->magick,"PS2") == 0)
          (void) fprintf(image->file,"  showpage\n");
        (void) fprintf(image->file,"} bind def\n");
        (void) fprintf(image->file,"%%%%EndProlog\n");
      }
    (void) fprintf(image->file,"%%%%Page:  1 %u\n",page++);
    (void) fprintf(image->file,"%%%%PageBoundingBox: %d %d %d %d\n",x,y,
      x+(int) width-1,y+(int) (height+text_size)-1);
    if (x < bounding_box.x1)
      bounding_box.x1=x;
    if (y < bounding_box.y1)
      bounding_box.y1=y;
    if ((x+(int) width-1) > bounding_box.x2)
      bounding_box.x2=x+(int) width-1;
    if ((y+(int) (height+text_size)-1) > bounding_box.y2)
      bounding_box.y2=y+(int) (height+text_size)-1;
    if (image->label != (char *) NULL)
      (void) fprintf(image->file,"%%%%PageResources: font Helvetica\n");
    if (Latin1Compare(image_info->magick,"PS2") != 0)
      (void) fprintf(image->file,"userdict begin\n");
    (void) fprintf(image->file,"%%%%BeginData:\n");
    (void) fprintf(image->file,"DisplayImage\n");
    /*
      Output image data.
    */
    labels=StringToList(image->label);
    (void) fprintf(image->file,"%d %d\n%f %f\n%u\n",x,y,x_scale,y_scale,
      image_info->pointsize);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          (void) fprintf(image->file,"%.1024s \n",labels[i]);
          FreeMemory(labels[i]);
        }
        FreeMemory(labels);
      }
    (void) fprintf(image->file,"%u %u\n%u\n%d\n",image->columns,image->rows,
      IsPseudoClass(image),(int) (compression == NoCompression));
    p=image->pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      switch (compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          register unsigned char
            *q;

          /*
            Allocate pixel array.
          */
          number_packets=3*image->columns*image->rows;
          pixels=(unsigned char *)
            AllocateMemory(number_packets*sizeof(unsigned char));
          if (pixels == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          /*
            Dump Packbit encoded pixels.
          */
          q=pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                  *q++=DownScale(MaxRGB);
                }
              else
                {
                  *q++=DownScale(p->red);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->blue);
                }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          if (compression == ZipCompression)
            status=ZLIBEncodeImage(image->file,number_packets,
              image_info->quality,pixels);
          else
            if (compression == LZWCompression)
              status=LZWEncodeImage(image->file,number_packets,pixels);
            else
              status=PackbitsEncodeImage(image->file,number_packets,pixels);
          if (!status)
            {
              CloseImage(image);
              return(False);
            }
          FreeMemory((char *) pixels);
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          Ascii85Initialize();
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              if (image->matte && (p->index == Transparent))
                {
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                  Ascii85Encode(DownScale(MaxRGB),image->file);
                }
              else
                {
                  Ascii85Encode(DownScale(p->red),image->file);
                  Ascii85Encode(DownScale(p->green),image->file);
                  Ascii85Encode(DownScale(p->blue),image->file);
                }
            }
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
          Ascii85Flush(image->file);
          break;
        }
      }
    else
      if (IsFaxImage(image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) < Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          x=0;
          (void) fprintf(image->file,"0\n");
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              if (Latin1Compare(CCITTParam,"0") == 0)
                (void) HuffmanEncodeImage((ImageInfo *) image_info,image);
              else
                (void) Huffman2DEncodeImage((ImageInfo *) image_info,image);
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  byte<<=1;
                  if (p->index == polarity)
                    byte|=0x01;
                  bit++;
                  if (bit == 8)
                    {
                      Ascii85Encode(byte,image->file);
                      bit=0;
                      byte=0;
                    }
                  x++;
                  if (x == (int) image->columns)
                    {
                      /*
                        Advance to the next scanline.
                      */
                      if (bit != 0)
                        Ascii85Encode(byte << (8-bit),image->file);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick(y,image->rows))
                          ProgressMonitor(SaveImageText,y,image->rows);
                      bit=0;
                      byte=0;
                      x=0;
                   }
                }
                p++;
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
      else
        {
          /*
            Dump number of colors and colormap.
          */
          (void) fprintf(image->file,"%u\n",image->colors);
          for (i=0; i < (int) image->colors; i++)
            (void) fprintf(image->file,"%02x%02x%02x\n",
              DownScale(image->colormap[i].red),
              DownScale(image->colormap[i].green),
              DownScale(image->colormap[i].blue));
          switch (compression)
          {
            case RunlengthEncodedCompression:
            default:
            {
              register unsigned char
                *q;

              /*
                Allocate pixel array.
              */
              number_packets=image->columns*image->rows;
              pixels=(unsigned char *)
                AllocateMemory(number_packets*sizeof(unsigned char));
              if (pixels == (unsigned char *) NULL)
                PrematureExit(ResourceLimitWarning,"Memory allocation failed",
                  image);
              /*
                Dump Runlength encoded pixels.
              */
              q=pixels;
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  *q++=(unsigned char) p->index;
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              if (compression == ZipCompression)
                status=ZLIBEncodeImage(image->file,number_packets,
                  image_info->quality,pixels);
              else
                if (compression == LZWCompression)
                  status=LZWEncodeImage(image->file,number_packets,pixels);
                else
                  status=PackbitsEncodeImage(image->file,number_packets,pixels);
              FreeMemory((char *) pixels);
              if (!status)
                {
                  CloseImage(image);
                  return(False);
                }
              break;
            }
            case NoCompression:
            {
              /*
                Dump uncompressed PseudoColor packets.
              */
              Ascii85Initialize();
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                  Ascii85Encode((unsigned char) p->index,image->file);
                p++;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(i,image->packets))
                    ProgressMonitor(SaveImageText,i,image->packets);
              }
              Ascii85Flush(image->file);
              break;
            }
          }
        }
    (void) fprintf(image->file,"\n");
    (void) fprintf(image->file,"%%%%EndData\n");
    if (Latin1Compare(image_info->magick,"PS2") != 0)
      (void) fprintf(image->file,"end\n");
    (void) fprintf(image->file,"%%%%PageTrailer\n");
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) fprintf(image->file,"%%%%Trailer\n");
  if (page > 1)
    (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",
      bounding_box.x1,bounding_box.y1,bounding_box.x2,bounding_box.y2);
  (void) fprintf(image->file,"%%%%EOF\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e R G B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteRGBImage writes an image to a file in red, green, and
%  blue rasterfile format.
%
%  The format of the WriteRGBImage routine is:
%
%      status=WriteRGBImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteRGBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteRGBImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    scene;

  image->depth=QuantumDepth;
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
    }
  scene=0;
  do
  {
    /*
      Convert MIFF to RGB raster pixels.
    */
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        register unsigned char
          *q;

        unsigned char
          *pixels;

        unsigned short
          value;

        /*
          Allocate memory for pixels.
        */
        pixels=(unsigned char *)
          AllocateMemory(image->columns*sizeof(RunlengthPacket));
        if (pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        /*
          No interlacing:  RGBRGBRGBRGBRGBRGB...
        */
        x=0;
        y=0;
        p=image->pixels;
        q=pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(p->red,q);
            WriteQuantum(p->green,q);
            WriteQuantum(p->blue,q);
            if (Latin1Compare(image_info->magick,"RGBA") == 0)
              WriteQuantum(image->matte ? p->index : Opaque,q);
            x++;
            if (x == (int) image->columns)
              {
                (void) fwrite((char *) pixels,1,q-pixels,image->file);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        if (!UncondenseImage(image))
          return(False);
        for (y=0; y < (int) image->rows; y++)
        {
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            WriteQuantumFile(p->red);
            p++;
          }
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            WriteQuantumFile(p->green);
            p++;
          }
          p=image->pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            WriteQuantumFile(p->blue);
            p++;
          }
          p=image->pixels+(y*image->columns);
          if (image->matte)
            for (x=0; x < (int) image->columns; x++)
            {
              WriteQuantumFile(p->index);
              p++;
            }
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
        }
        break;
      }
      case PlaneInterlace:
      case PartitionInterlace:
      {
        /*
          Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
        */
        if (image_info->interlace == PartitionInterlace)
          {
            AppendImageFormat("R",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile(p->red);
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("G",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        ProgressMonitor(SaveImageText,100,400);
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile(p->green);
          p++;
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseImage(image);
            AppendImageFormat("B",image->filename);
            OpenImage(image_info,image,WriteBinaryType);
            if (image->file == (FILE *) NULL)
              PrematureExit(FileOpenWarning,"Unable to open file",image);
          }
        ProgressMonitor(SaveImageText,200,400);
        p=image->pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            WriteQuantumFile(p->blue);
          p++;
        }
        if (image->matte)
          {
            ProgressMonitor(SaveImageText,300,400);
            p=image->pixels;
            if (image_info->interlace == PartitionInterlace)
              {
                CloseImage(image);
                AppendImageFormat("A",image->filename);
                OpenImage(image_info,image,WriteBinaryType);
                if (image->file == (FILE *) NULL)
                  PrematureExit(FileOpenWarning,"Unable to open file",image);
              }
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                WriteQuantumFile(p->index);
              p++;
            }
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        ProgressMonitor(SaveImageText,400,400);
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S G I I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSGIImage writes an image in SGI RGB encoded image format.
%
%  The format of the WriteSGIImage routine is:
%
%      status=WriteSGIImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSGIImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static int SGIEncode(unsigned char *pixels,int count,
  unsigned char *packets)
{
  short
    runlength;

  unsigned char
    *limit,
    *mark,
    *p,
    *q;

  p=pixels;
  limit=p+count*4;
  q=packets;
  while (p < limit)
  {
    mark=p;
    p+=8;
    while ((p < limit) && ((*(p-8) != *(p-4)) || (*(p-4) != *p)))
      p+=4;
    p-=8;
    count=(int) ((p-mark) >> 2);
    while (count)
    {
      runlength=(short) (count > 126 ? 126 : count);
      count-=runlength;
      *q++=0x80 | runlength;
      for ( ; runlength > 0; runlength--)
      {
        *q++=(*mark);
        mark+=4;
      }
    }
    mark=p;
    p+=4;
    while ((p < limit) && (*p == *mark))
      p+=4;
    count=(int) ((p-mark) >> 2);
    while (count)
    {
      runlength=(short) (count > 126 ? 126 : count);
      count-=runlength;
      *q++=(unsigned char) runlength;
      *q++=(*mark);
    }
  }
  *q++=0;
  return((int) (q-packets));
}

unsigned int WriteSGIImage(const ImageInfo *image_info,Image *image)
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

  SGIHeader
    iris_header;

  register int
    i,
    j,
    x,
    y,
    z;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *iris_pixels,
    *packets,
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize SGI raster file header.
    */
    iris_header.magic=0x01DA;
    if (image_info->compression == NoCompression)
      iris_header.storage=0x00;
    else
      iris_header.storage=0x01;
    iris_header.bytes_per_pixel=1;  /* one byte per pixel */
    iris_header.dimension=3;
    iris_header.columns=image->columns;
    iris_header.rows=image->rows;
    iris_header.depth=image->matte ? 4 : 3;
    if (IsGrayImage(image))
      {
        iris_header.dimension=2;
        iris_header.depth=1;
      }
    iris_header.minimum_value=0;
    iris_header.maximum_value=MaxRGB;
    for (i=0; i < (int) sizeof(iris_header.filler); i++)
      iris_header.filler[i]=0;
    /*
      Write SGI header.
    */
    MSBFirstWriteShort(iris_header.magic,image->file);
    (void) fputc(iris_header.storage,image->file);
    (void) fputc(iris_header.bytes_per_pixel,image->file);
    MSBFirstWriteShort(iris_header.dimension,image->file);
    MSBFirstWriteShort(iris_header.columns,image->file);
    MSBFirstWriteShort(iris_header.rows,image->file);
    MSBFirstWriteShort(iris_header.depth,image->file);
    MSBFirstWriteLong(iris_header.minimum_value,image->file);
    MSBFirstWriteLong(iris_header.maximum_value,image->file);
    (void) fwrite(iris_header.filler,1,sizeof(iris_header.filler),image->file);
    /*
      Allocate SGI pixels.
    */
    iris_pixels=(unsigned char *)
      AllocateMemory(4*image->columns*image->rows*sizeof(unsigned char));
    if (iris_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Convert runlength-encoded packets to uncompressed SGI pixels.
    */
    x=0;
    y=0;
    p=image->pixels;
    q=iris_pixels+(iris_header.rows-1)*(iris_header.columns*4);
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        *q++=DownScale(p->red);
        *q++=DownScale(p->green);
        *q++=DownScale(p->blue);
        *q++=(unsigned char) p->index;
        x++;
        if (x == (int) image->columns)
          {
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            y++;
            q=iris_pixels+((iris_header.rows-1)-y)*(iris_header.columns*4);
            x=0;
          }
      }
      p++;
    }
    if (image_info->compression == NoCompression)
      {
        unsigned char
          *scanline;

        /*
          Write uncompressed SGI pixels.
        */
        scanline=(unsigned char *)
          AllocateMemory(iris_header.columns*sizeof(unsigned char));
        if (scanline == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        for (z=0; z < (int) iris_header.depth; z++)
        {
          q=iris_pixels+z;
          for (y=0; y < (int) iris_header.rows; y++)
          {
            for (x=0; x < (int) iris_header.columns; x++)
            {
              scanline[x]=(*q);
              q+=4;
            }
            (void) fwrite(scanline,sizeof(unsigned char),iris_header.columns,
              image->file);
          }
        }
        FreeMemory(scanline);
      }
    else
      {
        unsigned long
          length,
          number_packets,
          offset,
          *offsets,
          *runlength;

        /*
          Convert SGI uncompressed pixels to runlength-encoded pixels.
        */
        offsets=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        packets=(unsigned char *) AllocateMemory(4*((iris_header.columns << 1)+
          10)*image->rows*sizeof(unsigned char));
        runlength=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        offset=512+4*((iris_header.rows*iris_header.depth) << 1);
        number_packets=0;
        q=iris_pixels;
        for (y=0; y < (int) iris_header.rows; y++)
        {
          for (z=0; z < (int) iris_header.depth; z++)
          {
            length=
              SGIEncode(q+z,(int) iris_header.columns,packets+number_packets);
            number_packets+=length;
            offsets[y+z*iris_header.rows]=offset;
            runlength[y+z*iris_header.rows]=length;
            offset+=length;
          }
          q+=(iris_header.columns*4);
        }
        /*
          Write out line start and length tables and runlength-encoded pixels.
        */
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          MSBFirstWriteLong(offsets[i],image->file);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          MSBFirstWriteLong(runlength[i],image->file);
        (void) fwrite(packets,sizeof(unsigned char),number_packets,image->file);
        /*
          Free memory.
        */
        FreeMemory(runlength);
        FreeMemory(packets);
        FreeMemory(offsets);
      }
    FreeMemory(iris_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S U N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteSUNImage writes an image in the SUN rasterfile format.
%
%  The format of the WriteSUNImage routine is:
%
%      status=WriteSUNImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteSUNImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteSUNImage(const ImageInfo *image_info,Image *image)
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
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

  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  SUNHeader
    sun_header;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize SUN raster file header.
    */
    sun_header.magic=0x59a66a95;
    sun_header.width=image->columns;
    sun_header.height=image->rows;
    sun_header.type=(image->class == DirectClass ? RT_FORMAT_RGB : RT_STANDARD);
    sun_header.maptype=RMT_NONE;
    sun_header.maplength=0;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        /*
          Full color SUN raster.
        */
        sun_header.depth=(image->matte ? 32 : 24);
        sun_header.length=image->columns*image->rows*(image->matte ? 4 : 3);
        sun_header.length+=image->columns & 0x01 ? image->rows : 0;
      }
    else
      if (IsMonochromeImage(image))
        {
          /*
            Monochrome SUN raster.
          */
          sun_header.depth=1;
          sun_header.length=((image->columns+7) >> 3)*image->rows;
          sun_header.length+=((image->columns/8)+(image->columns % 8 ? 1 : 0)) %
            2 ? image->rows : 0;
        }
      else
        {
          /*
            Colormapped SUN raster.
          */
          sun_header.depth=8;
          sun_header.length=image->columns*image->rows;
          sun_header.length+=image->columns & 0x01 ? image->rows : 0;
          sun_header.maptype=RMT_EQUAL_RGB;
          sun_header.maplength=image->colors*3;
        }
    /*
      Write SUN header.
    */
    MSBFirstWriteLong(sun_header.magic,image->file);
    MSBFirstWriteLong(sun_header.width,image->file);
    MSBFirstWriteLong(sun_header.height,image->file);
    MSBFirstWriteLong(sun_header.depth,image->file);
    MSBFirstWriteLong(sun_header.length,image->file);
    MSBFirstWriteLong(sun_header.type,image->file);
    MSBFirstWriteLong(sun_header.maptype,image->file);
    MSBFirstWriteLong(sun_header.maplength,image->file);
    /*
      Convert MIFF to SUN raster pixels.
    */
    p=image->pixels;
    x=0;
    y=0;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        register unsigned char
          *q;

        unsigned char
          *pixels;

        unsigned short
          value;

        /*
          Allocate memory for pixels.
        */
        pixels=(unsigned char *)
          AllocateMemory(image->columns*sizeof(RunlengthPacket));
        if (pixels == (unsigned char *) NULL)
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
        /*
          Convert DirectClass packet to SUN RGB pixel.
        */
        q=pixels;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            if (image->matte)
              *q++=DownScale(p->index);
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            x++;
            if (x == (int) image->columns)
              {
                if (image->columns & 0x01)
                  WriteQuantum(0,q);  /* pad scanline */
                (void) fwrite((char *) pixels,1,q-pixels,image->file);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=pixels;
                x=0;
                y++;
              }
          }
          p++;
        }
        FreeMemory((char *) pixels);
      }
    else
      if (IsMonochromeImage(image))
        {
          register unsigned char
            bit,
            byte,
            polarity;

          /*
            Convert PseudoClass image to a SUN monochrome image.
          */
          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
          bit=0;
          byte=0;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              byte<<=1;
              if (p->index == polarity)
                byte|=0x01;
              bit++;
              if (bit == 8)
                {
                  (void) fputc(byte,image->file);
                  bit=0;
                  byte=0;
                }
              x++;
              if (x == (int) image->columns)
                {
                  /*
                    Advance to the next scanline.
                  */
                  if (bit != 0)
                    (void) fputc(byte << (8-bit),image->file);
                  if ((((image->columns/8)+
                      (image->columns % 8 ? 1 : 0)) % 2) != 0)
                    (void) fputc(0,image->file);  /* pad scanline */
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  bit=0;
                  byte=0;
                  x=0;
               }
            }
            p++;
          }
        }
      else
        {
          /*
            Dump colormap to file.
          */
          for (i=0; i < (int) image->colors; i++)
            (void) fputc(DownScale(image->colormap[i].red),image->file);
          for (i=0; i < (int) image->colors; i++)
            (void) fputc(DownScale(image->colormap[i].green),image->file);
          for (i=0; i < (int) image->colors; i++)
            (void) fputc(DownScale(image->colormap[i].blue),image->file);
          /*
            Convert PseudoClass packet to SUN colormapped pixel.
          */
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              (void) fputc(p->index,image->file);
              x++;
              if (x == (int) image->columns)
                {
                  if (image->columns & 0x01)
                    (void) fputc(0,image->file);  /* pad scanline */
                  if (image->previous == (Image *) NULL)
                    if (QuantumTick(y,image->rows))
                      ProgressMonitor(SaveImageText,y,image->rows);
                  x=0;
                }
            }
            p++;
          }
        }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T G A I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTGAImage writes a image in the Truevision Targa rasterfile
%  format.
%
%  The format of the WriteTGAImage routine is:
%
%      status=WriteTGAImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTGAImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteTGAImage(const ImageInfo *image_info,Image *image)
{
#define TargaColormap 1
#define TargaRGB 2
#define TargaMonochrome 3
#define TargaRLEColormap  9
#define TargaRLERGB  10
#define TargaRLEMonochrome  11

  typedef struct _TargaHeader
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
  } TargaHeader;

  Image
    *flopped_image;

  int
    count,
    runlength;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q,
    *r;

  TargaHeader
    targa_header;

  unsigned char
    *targa_pixels;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Flop image.
    */
    image->orphan=True;
    flopped_image=FlopImage(image);
    image->orphan=False;
    if (flopped_image == (Image *) NULL)
      PrematureExit(ResourceLimitWarning,"Unable to flop image",image);
    /*
      Initialize TGA raster file header.
    */
    targa_header.id_length=0;
    if (flopped_image->comments != (char *) NULL)
      targa_header.id_length=Min(Extent(flopped_image->comments),255);
    targa_header.colormap_type=0;
    targa_header.colormap_index=0;
    targa_header.colormap_length=0;
    targa_header.colormap_size=0;
    targa_header.x_origin=0;
    targa_header.y_origin=0;
    targa_header.width=flopped_image->columns;
    targa_header.height=flopped_image->rows;
    targa_header.bits_per_pixel=8;
    targa_header.attributes=0;
    if (!IsPseudoClass(flopped_image))
      {
        /*
          Full color TGA raster.
        */
        targa_header.image_type=TargaRGB;
        if (image_info->compression != NoCompression)
          targa_header.image_type=TargaRLERGB;
        targa_header.bits_per_pixel=flopped_image->matte ? 32 : 24;
      }
    else
      {
        /*
          Colormapped TGA raster.
        */
        targa_header.image_type=TargaColormap;
        if (image_info->compression != NoCompression)
          targa_header.image_type=TargaRLEColormap;
        if (!IsMonochromeImage(flopped_image))
          {
            targa_header.colormap_type=1;
            targa_header.colormap_index=0;
            targa_header.colormap_length=flopped_image->colors;
            targa_header.colormap_size=24;
          }
        else
          {
            /*
              Monochrome TGA raster.
            */
            targa_header.image_type=TargaMonochrome;
            if (image_info->compression != NoCompression)
              targa_header.image_type=TargaRLEMonochrome;
          }
      }
    /*
      Write TGA header.
    */
    (void) fputc((char) targa_header.id_length,image->file);
    (void) fputc((char) targa_header.colormap_type,image->file);
    (void) fputc((char) targa_header.image_type,image->file);
    LSBFirstWriteShort(targa_header.colormap_index,image->file);
    LSBFirstWriteShort(targa_header.colormap_length,image->file);
    (void) fputc((char) targa_header.colormap_size,image->file);
    LSBFirstWriteShort(targa_header.x_origin,image->file);
    LSBFirstWriteShort(targa_header.y_origin,image->file);
    LSBFirstWriteShort(targa_header.width,image->file);
    LSBFirstWriteShort(targa_header.height,image->file);
    (void) fputc((char) targa_header.bits_per_pixel,image->file);
    (void) fputc((char) targa_header.attributes,image->file);
    if (targa_header.id_length != 0)
      (void) fwrite((char *) flopped_image->comments,1,targa_header.id_length,
        image->file);
    /*
      Convert MIFF to TGA raster pixels.
    */
    count=(unsigned int)
      (targa_header.bits_per_pixel*targa_header.width*targa_header.height) >> 3;
    if (image_info->compression != NoCompression)
      count+=(count/128)+1;
    targa_pixels=(unsigned char *) AllocateMemory(count*sizeof(unsigned char));
    if (targa_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",
        flopped_image);
    p=flopped_image->pixels+(flopped_image->packets-1);
    q=targa_pixels;
    if (!IsPseudoClass(flopped_image))
      {
        /*
          Convert DirectClass packet to TGA RGB pixel.
        */
        if (image_info->compression == NoCompression)
          for (i=0; i < (int) flopped_image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=DownScale(p->blue);
              *q++=DownScale(p->green);
              *q++=DownScale(p->red);
              if (flopped_image->matte)
                *q++=Opaque-DownScale(p->index);
            }
            p--;
            if (QuantumTick(i,flopped_image->packets))
              ProgressMonitor(SaveImageText,i,flopped_image->packets);
          }
        else
          for (i=0; i < (int) flopped_image->packets; i++)
          {
            for (runlength=p->length+1; runlength > 128; runlength-=128)
            {
              *q++=0xff;
              *q++=DownScale(p->blue);
              *q++=DownScale(p->green);
              *q++=DownScale(p->red);
              if (flopped_image->matte)
                *q++=DownScale(p->index);
            }
            r=q;
            *q++=0x80+(runlength-1);
            *q++=DownScale(p->blue);
            *q++=DownScale(p->green);
            *q++=DownScale(p->red);
            if (flopped_image->matte)
              *q++=DownScale(p->index);
            if (runlength != 1)
              p--;
            else
              {
                for ( ; i < (int) flopped_image->packets; i++)
                {
                  p--;
                  if ((p->length != 0) || (runlength == 128))
                    break;
                  *q++=DownScale(p->blue);
                  *q++=DownScale(p->green);
                  *q++=DownScale(p->red);
                  if (flopped_image->matte)
                    *q++=DownScale(p->index);
                  runlength++;
                }
                *r=runlength-1;
              }
            if (QuantumTick(i,flopped_image->packets))
              ProgressMonitor(SaveImageText,i,flopped_image->packets);
          }
      }
    else
      if (!IsMonochromeImage(flopped_image))
        {
          unsigned char
            *targa_colormap;

          /*
            Dump colormap to file (blue, green, red byte order).
          */
          if (flopped_image->colors > 256)
            {
              QuantizeInfo
                quantize_info;

              GetQuantizeInfo(&quantize_info);
              quantize_info.number_colors=256;
              quantize_info.dither=image_info->dither;
              (void) QuantizeImage(&quantize_info,flopped_image);
            }
          targa_colormap=(unsigned char *) AllocateMemory(3*
            targa_header.colormap_length*sizeof(unsigned char));
          if (targa_colormap == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              flopped_image);
          q=targa_colormap;
          for (i=0; i < (int) flopped_image->colors; i++)
          {
            *q++=DownScale(flopped_image->colormap[i].blue);
            *q++=DownScale(flopped_image->colormap[i].green);
            *q++=DownScale(flopped_image->colormap[i].red);
          }
          (void) fwrite((char *) targa_colormap,1,
            (int) 3*targa_header.colormap_length,image->file);
          FreeMemory((char *) targa_colormap);
          /*
            Convert PseudoClass packet to TGA colormapped pixel.
          */
          q=targa_pixels;
          if (image_info->compression == NoCompression)
            for (i=0; i < (int) flopped_image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=p->index;
              p--;
              if (QuantumTick(i,flopped_image->packets))
                ProgressMonitor(SaveImageText,i,flopped_image->packets);
            }
          else
            for (i=0; i < (int) flopped_image->packets; i++)
            {
              for (runlength=p->length+1; runlength > 128; runlength-=128)
              {
                *q++=0xff;
                *q++=p->index;
              }
              r=q;
              *q++=0x80+(runlength-1);
              *q++=p->index;
              if (runlength != 1)
                p--;
              else
                {
                  for ( ; i < (int) flopped_image->packets; i++)
                  {
                    p--;
                    if ((p->length != 0) || (runlength == 128))
                      break;
                    *q++=p->index;
                    runlength++;
                  }
                  *r=runlength-1;
                }
              if (QuantumTick(i,flopped_image->packets))
                ProgressMonitor(SaveImageText,i,flopped_image->packets);
            }
        }
      else
        {
          unsigned int
            polarity;

          /*
            Convert PseudoClass image to a TGA monochrome image.
          */
          polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (image->colors == 2)
            polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
          if (flopped_image->colors == 2)
            polarity=Intensity(flopped_image->colormap[0]) >
              Intensity(flopped_image->colormap[1]);
          if (image_info->compression == NoCompression)
            for (i=0; i < (int) flopped_image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=p->index == polarity ? 0 : DownScale(MaxRGB);
              p--;
              if (QuantumTick(i,flopped_image->packets))
                ProgressMonitor(SaveImageText,i,flopped_image->packets);
            }
          else
            for (i=0; i < (int) flopped_image->packets; i++)
            {
              for (runlength=p->length+1; runlength > 128; runlength-=128)
              {
                *q++=0xff;
                *q++=p->index == polarity ? 0 : DownScale(MaxRGB);
              }
              r=q;
              *q++=0x80+(runlength-1);
              *q++=p->index == polarity ? 0 : DownScale(MaxRGB);
              if (runlength != 1)
                p--;
              else
                {
                  for ( ; i < (int) flopped_image->packets; i++)
                  {
                    p--;
                    if ((p->length != 0) || (runlength == 128))
                      break;
                    *q++=p->index == polarity ? 0 : DownScale(MaxRGB);
                    runlength++;
                  }
                  *r=runlength-1;
                }
              if (QuantumTick(i,flopped_image->packets))
                ProgressMonitor(SaveImageText,i,flopped_image->packets);
            }
        }
    (void) fwrite((char *) targa_pixels,1,(int) (q-targa_pixels),image->file);
    DestroyImage(flopped_image);
    FreeMemory((char *) targa_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTIFFImage writes an image in the Tagged image file format.
%
%  The format of the WriteTIFFImage routine is:
%
%      status=WriteTIFFImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteTIFFImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

#if defined(IPTC_SUPPORT)
static void TIFFNewsProfileHandler(TIFF *tiff,int type,Image *image)
{
  register int
    i;

  unsigned char
    *profile;

  unsigned int
    length;

  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle TIFFTAG_RICHTIFFIPTC tag.
      */
      length=image->iptc_profile.length;
      profile=(unsigned char *) AllocateMemory(length*sizeof(unsigned char));
      if ((length == 0) || (profile == (unsigned char *) NULL))
        return;
      (void) memcpy((char *) profile,image->iptc_profile.info,length);
      length=image->iptc_profile.length/4;
      if (TIFFIsByteSwapped(tiff))
        TIFFSwabArrayOfLong((uint32 *) profile,length);
      TIFFSetField(tiff,type,(uint32) length,(void *) profile);
      FreeMemory((char *) profile);
      return;
    }
  /*
    Handle TIFFTAG_PHOTOSHOP tag.
  */
  length=image->iptc_profile.length;
  profile=(unsigned char *) AllocateMemory((length+13)*sizeof(unsigned char));
  if ((length == 0) || (profile == (unsigned char *) NULL))
    return;
  (void) memcpy((char *) profile,"8BIM\04\04\0\0",8);
  profile[8]=(length >> 24) & 0xff;
  profile[9]=(length >> 16) & 0xff;
  profile[10]=(length >> 8) & 0xff;
  profile[11]=length & 0xff;
  for (i=0; i < length; i++)
    profile[i+12]=image->iptc_profile.info[i];
  profile[i+12]=0x00;
  TIFFSetField(tiff,type,(uint32) length+(length & 0x01 ? 1 : 0)+12,
    (void *) profile);
  FreeMemory((char *) profile);
}
#endif

static int TIFFWritePixels(TIFF *tiff,tdata_t scanline,uint32 row,
  tsample_t sample,Image *image)
{
  int
    bytes_per_pixel,
    number_tiles,
    status,
    tile_width;

  register int
    i,
    j,
    k;

  static unsigned char
    *scanlines = (unsigned char *) NULL,
    *tile_pixels = (unsigned char *) NULL;

  if (!TIFFIsTiled(tiff))
    return(TIFFWriteScanline(tiff,scanline,row,sample));
  if (scanlines == (unsigned char *) NULL)
    scanlines=(unsigned char *)
      AllocateMemory(image->tile_info.height*TIFFScanlineSize(tiff));
  if (scanlines == (unsigned char *) NULL)
    return(-1);
  if (tile_pixels == (unsigned char *) NULL)
    tile_pixels=(unsigned char *)AllocateMemory(TIFFTileSize(tiff));
  if (tile_pixels == (unsigned char *) NULL)
    return(-1);
  /*
    Fill scanlines to tile height.
  */
  i=(row % image->tile_info.height)*TIFFScanlineSize(tiff);
  memcpy(scanlines+i,(unsigned char *) scanline,TIFFScanlineSize(tiff));
  if (((row % image->tile_info.height) != (image->tile_info.height-1)) &&
      (row != image->rows-1))
    return(0);
  /*
    Write tile to TIFF image.
  */
  status=0;
  bytes_per_pixel=
    TIFFTileSize(tiff)/(image->tile_info.height*image->tile_info.width);
  number_tiles=
    (image->columns+image->tile_info.height-1)/image->tile_info.height;
  for (i=0; i < number_tiles; i++)
  {
    tile_width=(i == (int) number_tiles-1) ?
      image->columns-(i*image->tile_info.width) : image->tile_info.width;
    for (j=0; j < ((row % image->tile_info.height)+1); j++)
      for (k=0; k < tile_width; k++)
      {
        register int
          l;

        register unsigned char
          *p,
          *q;

        p=scanlines+(j*TIFFScanlineSize(tiff)+(i*image->tile_info.width+k)*
          bytes_per_pixel);
        q=tile_pixels+
          (j*(TIFFTileSize(tiff)/image->tile_info.height)+k*bytes_per_pixel);
        for (l=0; l < bytes_per_pixel; l++)
          *q++=(*p++);
      }
      status=TIFFWriteTile(tiff,tile_pixels,i*image->tile_info.width,(row/
        image->tile_info.height)*image->tile_info.height,0,sample);
      if (status < 0)
        break;
  }
  if (row == (image->rows-1))
    {
      /*
        Free memory resources.
      */
      FreeMemory((char *) scanlines);
      scanlines=(unsigned char *) NULL;
      FreeMemory((char *) tile_pixels);
      tile_pixels=(unsigned char *) NULL;
    }
  return(status);
}

unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  ((8*1024)/TIFFScanlineSize(tiff))
#endif

  CompressionType
    compression;

  Image
    encode_image;

  int
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j,
    x;

  register unsigned char
    *q;

  TIFF
    *tiff;

  uint16
    compress_tag,
    photometric;

  unsigned char
    *scanline;

  unsigned int
    scene;

  unsigned short
    value;

  if (Latin1Compare(image_info->magick,"PTIF") == 0)
    {
      Image
        *next_image,
        *pyramid_image;

      unsigned int
        height,
        width;

      /*
        Pyramid encode TIFF image.
      */
      pyramid_image=(Image *) NULL;
      if (image->tile_info.width == 0)
        image->tile_info.width=DefaultPyramidWidth;
      if (image->tile_info.height == 0)
        image->tile_info.height=DefaultPyramidHeight;
      width=image->columns;
      height=image->rows;
      do
      {
        image->orphan=True;
        next_image=ZoomImage(image,width,height);
        image->orphan=False;
        if (next_image == (Image *) NULL)
          PrematureExit(FileOpenWarning,"Unable to pyramid encode image",image);
        if (pyramid_image == (Image *) NULL)
          pyramid_image=next_image;
        else
          {
            register Image
              *p;

            /*
              Link image into pyramid image list.
            */
            for (p=pyramid_image; p->next != (Image *) NULL; p=p->next);
            next_image->previous=p;
            p->next=next_image;
          }
        width=(width+1)/2;
        height=(height+1)/2;
        if ((width == 0) || (height == 0))
          break;
      } while (((2*width) >= image->tile_info.width) ||
               ((2*height) >= image->tile_info.height));
      image=pyramid_image;
    }
  /*
    Open TIFF file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  if ((image->file != stdout) && !image->pipe)
    (void) remove(image->filename);
  else
    {
      /*
        Write standard output or pipe to temporary file.
      */
      encode_image=(*image);
      TemporaryFilename(image->filename);
      image->temporary=True;
    }
  CloseImage(image);
  tiff=TIFFOpen(image->filename,WriteBinaryType);
  if (tiff == (TIFF *) NULL)
    return(False);
  compression=image_info->compression;
#if defined(HasLZW)
  if (compression == UndefinedCompression)
    compression=LZWCompression;
#endif
  scene=0;
  do
  {
    /*
      Initialize TIFF fields.
    */
    if (Latin1Compare(image_info->magick,"PTIF") == 0)
      if (image->previous != (Image *) NULL)
        TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
    TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
    TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);
    TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,8);
    if (image->depth > 8)
      TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,16);
    compress_tag=COMPRESSION_NONE;
    if (compression == JPEGCompression)
      compress_tag=COMPRESSION_JPEG;
    if (compression == LZWCompression)
      compress_tag=COMPRESSION_LZW;
    if (compression == RunlengthEncodedCompression)
      compress_tag=COMPRESSION_PACKBITS;
    if (compression == ZipCompression)
      compress_tag=COMPRESSION_DEFLATE;
    if (image_info->colorspace == CMYKColorspace)
      {
        photometric=PHOTOMETRIC_SEPARATED;
        TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,4);
        TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
      }
    else
      if ((Latin1Compare(image_info->magick,"TIFF24") == 0) ||
          (!IsPseudoClass(image) && !IsGrayImage(image)))
        {
          /*
            Full color TIFF raster.
          */
          photometric=PHOTOMETRIC_RGB;
          TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,(image->matte ? 4 : 3));
          if (image->matte)
            {
              uint16
                extra_samples,
                sample_info[1];

              /*
                TIFF has a matte channel.
              */
              extra_samples=1;
              sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
              TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
                &sample_info);
            }
        }
      else
        {
          /*
            Colormapped TIFF raster.
          */
          TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
          photometric=PHOTOMETRIC_PALETTE;
          if (image->colors <= 2)
            {
              if (IsMonochromeImage(image))
                photometric=PHOTOMETRIC_MINISWHITE;
              if (image->compression != NoCompression)
                {
                  if (image->compression == FaxCompression)
                    compress_tag=COMPRESSION_CCITTFAX3;
                  else
                    compress_tag=COMPRESSION_CCITTFAX4;
                }
              TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,1);
            }
          else
            if (IsGrayImage(image))
              photometric=PHOTOMETRIC_MINISBLACK;
        }
    TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
    TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);
    TIFFSetField(tiff,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
    TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    if (image_info->tile == (char *) NULL)
      TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);
    else
      if (IsSubimage(image_info->tile,False))
        TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,image->rows);
      else
        {
          TIFFSetField(tiff,TIFFTAG_TILEWIDTH,image->tile_info.width);
          TIFFSetField(tiff,TIFFTAG_TILELENGTH,image->tile_info.height);
        }
    if (photometric == PHOTOMETRIC_RGB)
      if ((image_info->interlace == PlaneInterlace) ||
          (image_info->interlace == PartitionInterlace))
        TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_SEPARATE);
    if ((image->x_resolution != 0) && (image->y_resolution != 0))
      {
        unsigned short
          units;

        /*
          Set image resolution.
        */
        units=RESUNIT_NONE;
        if (image->units == PixelsPerInchResolution)
          units=RESUNIT_INCH;
        if (image->units == PixelsPerCentimeterResolution)
          units=RESUNIT_CENTIMETER;
        TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(uint16) units);
        TIFFSetField(tiff,TIFFTAG_XRESOLUTION,(float) image->x_resolution);
        TIFFSetField(tiff,TIFFTAG_YRESOLUTION,(float) image->y_resolution);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        float
          chromaticity[6];

        /*
          Set image chromaticity.
        */
        chromaticity[0]=image->chromaticity.red_primary.x;
        chromaticity[1]=image->chromaticity.red_primary.y;
        chromaticity[2]=image->chromaticity.green_primary.x;
        chromaticity[3]=image->chromaticity.green_primary.y;
        chromaticity[4]=image->chromaticity.blue_primary.x;
        chromaticity[5]=image->chromaticity.blue_primary.y;
        TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,chromaticity);
        chromaticity[0]=image->chromaticity.white_point.x;
        chromaticity[1]=image->chromaticity.white_point.y;
        TIFFSetField(tiff,TIFFTAG_WHITEPOINT,chromaticity);
      }
#if defined(ICC_SUPPORT)
    if (image->color_profile.length > 0)
      TIFFSetField(tiff,TIFFTAG_ICCPROFILE,(uint32) image->color_profile.length,
        (void *) image->color_profile.info);
#endif
#if defined(IPTC_SUPPORT)
#if defined(PHOTOSHOP_SUPPORT)
    if (image->iptc_profile.length > 0)
      TIFFNewsProfileHandler(tiff,TIFFTAG_PHOTOSHOP,image);
#endif
    if (image->iptc_profile.length > 0)
      TIFFNewsProfileHandler(tiff,TIFFTAG_RICHTIFFIPTC,image);
#endif
    TIFFSetField(tiff,TIFFTAG_DOCUMENTNAME,image->filename);
    TIFFSetField(tiff,TIFFTAG_SOFTWARE,MagickVersion);
    if (image->number_scenes > 1)
      {
        TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
        TIFFSetField(tiff,TIFFTAG_PAGENUMBER,(unsigned short) image->scene,
          image->number_scenes);
      }
    if (image->label != (char *) NULL)
      TIFFSetField(tiff,TIFFTAG_PAGENAME,image->label);
    if (image->comments != (char *) NULL)
      TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,image->comments);
    /*
      Write image scanlines.
    */
    scanline=(unsigned char *) AllocateMemory(TIFFScanlineSize(tiff));
    if (scanline == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    p=image->pixels;
    q=scanline;
    x=0;
    y=0;
    switch (photometric)
    {
      case PHOTOMETRIC_RGB:
      {
        /*
          RGB TIFF image.
        */
        switch (image_info->interlace)
        {
          case NoInterlace:
          default:
          {
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                /*
                  Convert DirectClass packets to contiguous RGB scanlines.
                */
                *q++=DownScale(p->red);
                *q++=DownScale(p->green);
                *q++=DownScale(p->blue);
                if (image->matte)
                  *q++=DownScale(p->index);
                x++;
                if (x == (int) image->columns)
                  {
                    if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                      break;
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        ProgressMonitor(SaveImageText,y,image->rows);
                    q=scanline;
                    x=0;
                    y++;
                  }
              }
              p++;
            }
            break;
          }
          case PlaneInterlace:
          case PartitionInterlace:
          {
            /*
              Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
            */
            p=image->pixels;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                WriteQuantum(p->red,q);
                x++;
                if (x == (int) image->columns)
                  {
                    if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                      break;
                    q=scanline;
                    x=0;
                    y++;
                  }
              }
              p++;
            }
            ProgressMonitor(SaveImageText,100,400);
            p=image->pixels;
            y=0;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                WriteQuantum(p->green,q);
                x++;
                if (x == (int) image->columns)
                  {
                    if (TIFFWritePixels(tiff,(char *) scanline,y,1,image) < 0)
                      break;
                    q=scanline;
                    x=0;
                    y++;
                  }
              }
              p++;
            }
            ProgressMonitor(SaveImageText,200,400);
            p=image->pixels;
            y=0;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                WriteQuantum(p->blue,q);
                x++;
                if (x == (int) image->columns)
                  {
                    if (TIFFWritePixels(tiff,(char *) scanline,y,2,image) < 0)
                      break;
                    q=scanline;
                    x=0;
                    y++;
                  }
              }
              p++;
            }
            ProgressMonitor(SaveImageText,300,400);
            p=image->pixels;
            y=0;
            if (image->matte)
              for (i=0; i < (int) image->packets; i++)
              {
                for (j=0; j <= ((int) p->length); j++)
                {
                  WriteQuantum(p->index,q);
                  x++;
                  if (x == (int) image->columns)
                    {
                      if (TIFFWritePixels(tiff,(char *) scanline,y,3,image) < 0)
                        break;
                      q=scanline;
                      x=0;
                      y++;
                    }
                }
                p++;
              }
            ProgressMonitor(SaveImageText,400,400);
            break;
          }
        }
        break;
      }
      case PHOTOMETRIC_SEPARATED:
      {
        double
           black_generation,
           undercolor;

         int
           black,
           cyan,
           magenta,
           yellow;

        /*
          CMYK TIFF image.
        */
        undercolor=1.0;
        black_generation=1.0;
        if (image_info->undercolor != (char *) NULL)
          {
            (void) sscanf(image_info->undercolor,"%lfx%lf",&undercolor,
              &black_generation);
            if (black_generation == 1.0)
              black_generation=undercolor;
          }
        for (i=0; i < (int) image->packets; i++)
        {
          cyan=MaxRGB-p->red;
          magenta=MaxRGB-p->green;
          yellow=MaxRGB-p->blue;
          black=cyan;
          if (magenta < black)
            black=magenta;
          if (yellow < black)
            black=yellow;
          for (j=0; j <= ((int) p->length); j++)
          {
            /*
              Convert DirectClass packets to contiguous RGB scanlines.
            */
            WriteQuantum((unsigned int) (cyan-undercolor*black),q);
            WriteQuantum((unsigned int) (magenta-undercolor*black),q);
            WriteQuantum((unsigned int) (yellow-undercolor*black),q);
            WriteQuantum((unsigned int) (black_generation*black),q);
            x++;
            if (x == (int) image->columns)
              {
                if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                  break;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=scanline;
                x=0;
                y++;
              }
          }
          p++;
        }
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        unsigned short
          *blue,
          *green,
          *red;

        /*
          Colormapped TIFF image.
        */
        blue=(unsigned short *)
          AllocateMemory((1 << image->depth)*sizeof(unsigned short));
        green=(unsigned short *)
          AllocateMemory((1 << image->depth)*sizeof(unsigned short));
        red=(unsigned short *)
          AllocateMemory((1 << image->depth)*sizeof(unsigned short));
        if ((blue == (unsigned short *) NULL) ||
            (green == (unsigned short *) NULL) ||
            (red == (unsigned short *) NULL))
          PrematureExit(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Initialize TIFF colormap.
        */
        for (i=0; i < (int) image->colors; i++)
        {
          red[i]=(unsigned int) (image->colormap[i].red*65535L)/MaxRGB;
          green[i]=(unsigned int) (image->colormap[i].green*65535L)/MaxRGB;
          blue[i]=(unsigned int) (image->colormap[i].blue*65535L)/MaxRGB;
        }
        for ( ; i < (1 << image->depth); i++)
        {
          red[i]=0;
          green[i]=0;
          blue[i]=0;
        }
        TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
        FreeMemory((char *) red);
        FreeMemory((char *) green);
        FreeMemory((char *) blue);
      }
      default:
      {
        register unsigned char
          bit,
          byte,
          polarity;

        if (image->colors > 2)
          {
            /*
              Convert PseudoClass packets to contiguous grayscale scanlines.
            */
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                if (photometric == PHOTOMETRIC_PALETTE)
                  WriteQuantum(p->index,q)
                else
                  WriteQuantum(Intensity(*p),q);
                x++;
                if (x == (int) image->columns)
                  {
                    if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                      break;
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        ProgressMonitor(SaveImageText,y,image->rows);
                    q=scanline;
                    x=0;
                    y++;
                  }
              }
              p++;
            }
            break;
          }
        /*
          Convert PseudoClass packets to contiguous monochrome scanlines.
        */
        polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
        if (photometric == PHOTOMETRIC_PALETTE)
          polarity=1;
        else
          if (image->colors == 2)
            {
              polarity=
                Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
              if (photometric == PHOTOMETRIC_MINISBLACK)
                polarity=!polarity;
            }
        bit=0;
        byte=0;
        x=0;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            byte<<=1;
            if (p->index == polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
            x++;
            if (x == (int) image->columns)
              {
                /*
                  Advance to the next scanline.
                */
                if (bit != 0)
                  *q++=byte << (8-bit);
                if (TIFFWritePixels(tiff,(char *) scanline,y,0,image) < 0)
                  break;
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    ProgressMonitor(SaveImageText,y,image->rows);
                q=scanline;
                bit=0;
                byte=0;
                x=0;
                y++;
             }
          }
          p++;
        }
        break;
      }
    }
    FreeMemory((char *) scanline);
    if (image_info->verbose == True)
      TIFFPrintDirectory(tiff,stderr,False);
    TIFFWriteDirectory(tiff);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  (void) TIFFClose(tiff);
  if (image->temporary)
    {
      FILE
        *file;

      int
        c;

      /*
        Copy temporary file to standard output or pipe.
      */
      file=fopen(image->filename,ReadBinaryType);
      if (file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
      for (c=fgetc(file); c != EOF; c=fgetc(file))
        (void) putc(c,encode_image.file);
      (void) fclose(file);
      (void) remove(image->filename);
      image->temporary=False;
      CloseImage(&encode_image);
    }
  if (Latin1Compare(image_info->magick,"PTIF") == 0)
    DestroyImages(image);
  return(True);
}
#else
unsigned int WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
  MagickWarning(MissingDelegateWarning,"TIFF library is not available",
    image->filename);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T X T I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTXTImage writes the pixel values as text numbers.
%
%  The format of the WriteTXTImage routine is:
%
%      status=WriteTXTImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteTXTImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteTXTImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    scene;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Convert MIFF to TXT raster pixels.
    */
    x=0;
    y=0;
    p=image->pixels;
    for (i=0; i < (int) image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
      {
        if (image->matte)
          (void) fprintf(image->file,"%d,%d: %d,%d,%d,%d\n",x,y,
            p->red,p->green,p->blue,p->index);
        else
          {
            (void) fprintf(image->file,"%d,%d: %d,%d,%d  ",x,y,
              p->red,p->green,p->blue);
            (void) fprintf(image->file,HexColorFormat,p->red,p->green,p->blue);
          }
        (void) fprintf(image->file,"\n");
        x++;
        if (x == (int) image->columns)
          {
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                ProgressMonitor(SaveImageText,y,image->rows);
            x=0;
            y++;
          }
      }
      p++;
    }
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U I L I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteUILImage writes an image to a file in the X-Motif UIL table
%  format.
%
%  The format of the WriteUILImage routine is:
%
%      status=WriteUILImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUILImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteUILImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  double
    min_distance;

  int
    j,
    k,
    y;

  long
    mean;

  register double
    distance_squared;

  register int
    distance,
    i,
    runlength,
    x;

  register RunlengthPacket
    *p;

  register const XColorlist
    *q;

  unsigned int
    characters_per_pixel,
    colors,
    transparent;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  transparent=False;
  if (image->class == PseudoClass)
    colors=image->colors;
  else
    {
      QuantizeInfo
        quantize_info;

      unsigned char
        *matte_image;

      /*
        Convert DirectClass to PseudoClass image.
      */
      matte_image=(unsigned char *) NULL;
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          if (!UncondenseImage(image))
            return(False);
          matte_image=(unsigned char *)
            AllocateMemory(image->packets*sizeof(unsigned char));
          if (matte_image == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          p=image->pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            matte_image[i]=p->index == Transparent;
            if (matte_image[i])
              transparent=True;
            p++;
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
      colors=image->colors;
      if (transparent)
        {
          if (!UncondenseImage(image))
            return(False);
          colors++;
          p=image->pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            if (matte_image[i])
              p->index=image->colors;
            p++;
          }
        }
      if (matte_image != (unsigned char *) NULL)
        FreeMemory((char *) matte_image);
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (int) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    UIL header.
  */
  (void) fprintf(image->file,"/* UIL */\n");
  (void) fprintf(image->file,"value\n  %.1024s_ct : color_table(\n",
    BaseFilename(image->filename));
  for (i=0; i < (int) colors; i++)
  {
    ColorPacket
      *p;

    /*
      Define UIL color.
    */
    min_distance=0;
    p=image->colormap+i;
    for (q=XPMColorlist; q->name != (char *) NULL; q++)
    {
      mean=(DownScale(p->red)+q->red)/2;
      distance=DownScale(p->red)-(int) q->red;
      distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
      distance=DownScale(p->green)-(int) q->green;
      distance_squared+=4.0*distance*distance;
      distance=DownScale(p->blue)-(int) q->blue;
      distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
      if ((q == XPMColorlist) || (distance_squared <= min_distance))
        {
          min_distance=distance_squared;
          (void) strcpy(name,q->name);
        }
    }
    if (transparent)
      if (i == (int) (colors-1))
        (void) strcpy(name,"None");
    /*
      Write UIL color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    if (Latin1Compare(name,"None") == 0)
      (void) fprintf(image->file,"    background color = '%.1024s'",symbol);
    else
      (void) fprintf(image->file,"    color('%.1024s',%.1024s) = '%.1024s'",
        name,Intensity(*p) < ((MaxRGB+1)/2) ? "background" : "foreground",
        symbol);
    (void) fprintf(image->file,"%.1024s",
      (i == (int) (colors-1) ? ");\n" : ",\n"));
  }
  /*
    Define UIL pixels.
  */
  (void) fprintf(image->file,
    "  %.1024s_icon : icon(color_table = %.1024s_ct,\n",
    BaseFilename(image->filename),BaseFilename(image->filename));
  p=image->pixels;
  runlength=p->length+1;
  for (y=0; y < (int) image->rows; y++)
  {
    (void) fprintf(image->file,"    \"");
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      k=p->index % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) p->index-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      (void) fprintf(image->file,"%.1024s",symbol);
    }
    (void) fprintf(image->file,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? ");" : ","));
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SaveImageText,y,image->rows);
  }
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e U Y V Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteUYVYImage writes an image to a file in the digital UYVY
%  (16bit/pixel) format.  This format, used by AccomWSD, is not dramatically
%  higher quality than the 12bit/pixel YUV format, but has better locality.
%
%  The format of the WriteUYVYImage routine is:
%
%      status=WriteUYVYImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteUYVYImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%      Implicit assumption: number of columns is even.
%
*/
unsigned int WriteUYVYImage(const ImageInfo *image_info,Image *image)
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    full,
    y,
    u,
    v;

  /*
    Open output image file.
  */
  if (!UncondenseImage(image))
    return(False);
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Convert to YUV, at full resolution.
  */
  RGBTransformImage(image,YCbCrColorspace);
  /*
    Accumulate two pixels, then output.
  */
  full=False;
  u=0;
  v=0;
  y=0;
  full=False;
  full=False;
  p=image->pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      if (full)
        {
          (void) fputc(DownScale((u+p->green) >> 1),image->file);
          (void) fputc(DownScale(y),image->file);
          (void) fputc(DownScale((v+p->blue) >> 1),image->file);
          (void) fputc(DownScale(p->red),image->file);
          full=False;
        }
      else
        {
          y=p->red;
          u=p->green;
          v=p->blue;
          full=True;
        }
    }
    p++;
    if (QuantumTick(i,image->packets))
      ProgressMonitor(SaveImageText,i,image->packets);
  }
  TransformRGBImage(image,YCbCrColorspace);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I C A R I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVICARImage writes an image in the VICAR rasterfile format.
%  Vicar files contain a text header, followed by one or more planes of binary
%  grayscale image data.  Vicar files are designed to allow many planes to be
%  stacked together to form image cubes.  This routine only writes a single
%  grayscale plane.
%
%  Method WriteVICARImage was written contributed by
%  gorelick@esther.la.asu.edu.
%
%  The format of the WriteVICARImage routine is:
%
%      status=WriteVICARImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVICARImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteVICARImage(const ImageInfo *image_info,Image *image)
{
  char
    header[MaxTextExtent],
    label[16];

  int
    label_size,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Write header.
  */
  FormatString(header,"LBLSIZE=            FORMAT='BYTE'  TYPE='IMAGE'");
  FormatString(header+Extent(header),"  BUFSIZE=20000  DIM=2  EOL=0");
  FormatString(header+Extent(header),
    "  RECSIZE=%u  ORG='BSQ'  NL=%u  NS=%u  NB=1",image->columns,image->rows,
    image->columns);
  FormatString(header+Extent(header),
    "  N1=0  N2=0  N3=0  N4=0  NBB=0  NLB=0");
  FormatString(header+Extent(header),"  TASK='ImageMagick'");
  /*
    Compute the size of the label.
  */
  label_size=(Extent(header)+image->columns-1)/image->columns*image->columns;
  FormatString(label,"%d",label_size);
  for (i=0 ; i < Extent(label); i++)
    header[i+8]=label[i];
  /*
    Print the header and enough spaces to pad to label size.
  */
  (void) fprintf(image->file, "%-*s",label_size,header);
  /*
    Allocate memory for pixels.
  */
  pixels=(unsigned char *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if (pixels == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Write VICAR pixels.
  */
  x=0;
  y=0;
  p=image->pixels;
  q=pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      *q++=DownScale(Intensity(*p));
      x++;
      if (x == (int) image->columns)
        {
          (void) fwrite((char *) pixels,1,q-pixels,image->file);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(SaveImageText,y,image->rows);
          q=pixels;
          x=0;
          y++;
        }
    }
    p++;
  }
  FreeMemory((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e V I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteVIFFImage writes an image to a file in the VIFF image format.
%
%  The format of the WriteVIFFImage routine is:
%
%      status=WriteVIFFImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteVIFFImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteVIFFImage(const ImageInfo *image_info,Image *image)
{
#define VFF_CM_genericRGB  15
#define VFF_CM_NONE  0
#define VFF_DEP_IEEEORDER  0x2
#define VFF_DES_RAW  0
#define VFF_LOC_IMPLICIT  1
#define VFF_MAPTYP_NONE  0
#define VFF_MAPTYP_1_BYTE  1
#define VFF_MS_NONE  0
#define VFF_MS_ONEPERBAND  1
#define VFF_TYP_BIT  0
#define VFF_TYP_1_BYTE  1

  typedef struct _ViffHeader
  {
    char
      identifier,
      file_type,
      release,
      version,
      machine_dependency,
      reserve[3],
      comment[512];

    unsigned long
      rows,
      columns,
      subrows;

    long
      x_offset,
      y_offset;

    unsigned int
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

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    buffer[8],
    *viff_pixels;

  unsigned int
    scene;

  unsigned long
    packets;

  ViffHeader
    viff_header;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize VIFF image structure.
    */
    viff_header.identifier=(char) 0xab;
    viff_header.file_type=1;
    viff_header.release=1;
    viff_header.version=3;
    viff_header.machine_dependency=VFF_DEP_IEEEORDER;  /* IEEE byte ordering */
    *viff_header.comment='\0';
    if (image->comments != (char *) NULL)
      {
        (void) strncpy(viff_header.comment,image->comments,
          Min(Extent(image->comments),511));
        viff_header.comment[Min(Extent(image->comments),511)]='\0';
      }
    viff_header.rows=image->columns;
    viff_header.columns=image->rows;
    viff_header.subrows=0;
    viff_header.x_offset=(~0);
    viff_header.y_offset=(~0);
    viff_header.x_bits_per_pixel=0;
    viff_header.y_bits_per_pixel=0;
    viff_header.location_type=VFF_LOC_IMPLICIT;
    viff_header.location_dimension=0;
    viff_header.number_of_images=1;
    viff_header.data_encode_scheme=VFF_DES_RAW;
    viff_header.map_scheme=VFF_MS_NONE;
    viff_header.map_storage_type=VFF_MAPTYP_NONE;
    viff_header.map_rows=0;
    viff_header.map_columns=0;
    viff_header.map_subrows=0;
    viff_header.map_enable=1;  /* no colormap */
    viff_header.maps_per_cycle=0;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        /*
          Full color VIFF raster.
        */
        viff_header.number_data_bands=image->matte ? 4 : 3;
        viff_header.color_space_model=VFF_CM_genericRGB;
        viff_header.data_storage_type=VFF_TYP_1_BYTE;
        packets=image->columns*image->rows*viff_header.number_data_bands;
      }
    else
      {
        viff_header.number_data_bands=1;
        viff_header.color_space_model=VFF_CM_NONE;
        viff_header.data_storage_type=VFF_TYP_1_BYTE;
        packets=image->columns*image->rows;
        if (!IsGrayImage(image))
          {
            /*
              Colormapped VIFF raster.
            */
            viff_header.map_scheme=VFF_MS_ONEPERBAND;
            viff_header.map_storage_type=VFF_MAPTYP_1_BYTE;
            viff_header.map_rows=3;
            viff_header.map_columns=image->colors;
          }
        else
          if (image->colors == 2)
            {
              /*
                Monochrome VIFF raster.
              */
              viff_header.data_storage_type=VFF_TYP_BIT;
              packets=((image->columns+7) >> 3)*image->rows;
            }
      }
    /*
      Write VIFF image header (pad to 1024 bytes).
    */
    buffer[0]=viff_header.identifier;
    buffer[1]=viff_header.file_type;
    buffer[2]=viff_header.release;
    buffer[3]=viff_header.version;
    buffer[4]=viff_header.machine_dependency;
    buffer[5]=viff_header.reserve[0];
    buffer[6]=viff_header.reserve[1];
    buffer[7]=viff_header.reserve[2];
    (void) fwrite((char *) buffer,1,8,image->file);
    (void) fwrite((char *) viff_header.comment,1,512,image->file);
    MSBFirstWriteLong(viff_header.rows,image->file);
    MSBFirstWriteLong(viff_header.columns,image->file);
    MSBFirstWriteLong(viff_header.subrows,image->file);
    MSBFirstWriteLong((unsigned long) viff_header.x_offset,image->file);
    MSBFirstWriteLong((unsigned long) viff_header.y_offset,image->file);
    viff_header.x_bits_per_pixel=(63 << 24) | (128 << 16);
    MSBFirstWriteLong((unsigned long) viff_header.x_bits_per_pixel,image->file);
    viff_header.y_bits_per_pixel=(63 << 24) | (128 << 16);
    MSBFirstWriteLong((unsigned long) viff_header.y_bits_per_pixel,image->file);
    MSBFirstWriteLong(viff_header.location_type,image->file);
    MSBFirstWriteLong(viff_header.location_dimension,image->file);
    MSBFirstWriteLong(viff_header.number_of_images,image->file);
    MSBFirstWriteLong(viff_header.number_data_bands,image->file);
    MSBFirstWriteLong(viff_header.data_storage_type,image->file);
    MSBFirstWriteLong(viff_header.data_encode_scheme,image->file);
    MSBFirstWriteLong(viff_header.map_scheme,image->file);
    MSBFirstWriteLong(viff_header.map_storage_type,image->file);
    MSBFirstWriteLong(viff_header.map_rows,image->file);
    MSBFirstWriteLong(viff_header.map_columns,image->file);
    MSBFirstWriteLong(viff_header.map_subrows,image->file);
    MSBFirstWriteLong(viff_header.map_enable,image->file);
    MSBFirstWriteLong(viff_header.maps_per_cycle,image->file);
    MSBFirstWriteLong(viff_header.color_space_model,image->file);
    for (i=0; i < 420; i++)
      (void) fputc('\0',image->file);
    /*
      Convert MIFF to VIFF raster pixels.
    */
    viff_pixels=(unsigned char *) AllocateMemory(packets*sizeof(unsigned char));
    if (viff_pixels == (unsigned char *) NULL)
      PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
    p=image->pixels;
    q=viff_pixels;
    if (!IsPseudoClass(image) && !IsGrayImage(image))
      {
        unsigned long
          offset;

        /*
          Convert DirectClass packet to VIFF RGB pixel.
        */
        offset=image->columns*image->rows;
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q=DownScale(p->red);
            *(q+offset)=DownScale(p->green);
            *(q+offset*2)=DownScale(p->blue);
            if (image->matte)
              *(q+offset*3)=DownScale(p->index);
            q++;
          }
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
      }
    else
      if (!IsGrayImage(image))
        {
          unsigned char
            *viff_colormap;

          /*
            Dump colormap to file.
          */
          viff_colormap=(unsigned char *)
            AllocateMemory(image->colors*3*sizeof(unsigned char));
          if (viff_colormap == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          q=viff_colormap;
          for (i=0; i < (int) image->colors; i++)
            *q++=DownScale(image->colormap[i].red);
          for (i=0; i < (int) image->colors; i++)
            *q++=DownScale(image->colormap[i].green);
          for (i=0; i < (int) image->colors; i++)
            *q++=DownScale(image->colormap[i].blue);
          (void) fwrite((char *) viff_colormap,1,(int) image->colors*3,
            image->file);
          FreeMemory((char *) viff_colormap);
          /*
            Convert PseudoClass packet to VIFF colormapped pixels.
          */
          q=viff_pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
              *q++=p->index;
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
        }
      else
        if (image->colors == 2)
          {
            int
              x,
              y;

            register unsigned char
              bit,
              byte,
              polarity;

            /*
              Convert PseudoClass image to a VIFF monochrome image.
            */
            polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
            if (image->colors == 2)
              polarity=
                Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
            x=0;
            y=0;
            bit=0;
            byte=0;
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
              {
                byte>>=1;
                if (p->index == polarity)
                  byte|=0x80;
                bit++;
                if (bit == 8)
                  {
                    *q++=byte;
                    bit=0;
                    byte=0;
                  }
                x++;
                if (x == (int) image->columns)
                  {
                    /*
                      Advance to the next scanline.
                    */
                    if (bit != 0)
                      *q++=byte >> (8-bit);
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y,image->rows))
                        ProgressMonitor(SaveImageText,y,image->rows);
                    bit=0;
                    byte=0;
                    x=0;
                    y++;
                 }
              }
              p++;
            }
          }
        else
          {
            /*
              Convert PseudoClass packet to VIFF grayscale pixel.
            */
            for (i=0; i < (int) image->packets; i++)
            {
              for (j=0; j <= ((int) p->length); j++)
                *q++=p->red;
              p++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(SaveImageText,i,image->packets);
            }
          }
    (void) fwrite((char *) viff_pixels,1,(int) packets,image->file);
    FreeMemory((char *) viff_pixels);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteXImage writes an image to an X server.
%
%  The format of the WriteXImage routine is:
%
%      status=WriteXImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXImage return True if the image is displayed on
%      the X server.  False is returned is there is a memory shortage or if
%      the image file fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteXImage(const ImageInfo *image_info,Image *image)
{
  char
    *client_name;

  Display
    *display;


  QuantizeInfo
    quantize_info;

  unsigned long
    state;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Open X server connection.
  */
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    PrematureExit(ResourceLimitWarning,"Unable to connect to X server",image);
  /*
    Set our forgiving error handler.
  */
  XSetErrorHandler(XError);
  /*
    Get user defaults from X resource database.
  */
  client_name=SetClientName((char *) NULL);
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(resource_database,client_name,&resource_info);
  resource_info.image_info=(ImageInfo *) image_info;
  resource_info.quantize_info=(&quantize_info);
  GetQuantizeInfo(resource_info.quantize_info);
  resource_info.immutable=True;
  if (image_info->delay)
    resource_info.delay=atoi(image_info->delay);
  /*
    Display image.
  */
  state=DefaultState;
  (void) XDisplayImage(display,&resource_info,&client_name,1,&image,&state);
  XCloseDisplay(display);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X B M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXBMImage writes an image to a file in the X bitmap format.
%
%  The format of the WriteXBMImage routine is:
%
%      status=WriteXBMImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXBMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteXBMImage(const ImageInfo *image_info,Image *image)
{
  char
    name[MaxTextExtent];

  int
    x,
    y;

  register int
    i,
    j;

  register char
    *q;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    byte,
    polarity;

  unsigned int
    count;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Write X bitmap header.
  */
  (void) strcpy(name,image->filename);
  q=name;
  while ((*q != '.') && (*q != '\0'))
    q++;
  if (*q == '.')
    *q='\0';
  (void) fprintf(image->file,"#define %.1024s_width %u\n",name,image->columns);
  (void) fprintf(image->file,"#define %.1024s_height %u\n",name,image->rows);
  (void) fprintf(image->file,"static char %.1024s_bits[] = {\n",name);
  (void) fprintf(image->file," ");
  /*
    Convert MIFF to X bitmap pixels.
  */
  if (!IsMonochromeImage(image))
    {
      QuantizeInfo
        quantize_info;

      GetQuantizeInfo(&quantize_info);
      quantize_info.number_colors=2;
      quantize_info.dither=image_info->dither;
      quantize_info.colorspace=GRAYColorspace;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
    }
  polarity=Intensity(image->colormap[0]) > (MaxRGB >> 1);
  if (image->colors == 2)
    polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
  bit=0;
  byte=0;
  count=0;
  x=0;
  y=0;
  p=image->pixels;
  (void) fprintf(image->file," ");
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      byte>>=1;
      if (p->index == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
          (void) fprintf(image->file,"0x%02x, ",(unsigned int) (byte & 0xff));
          count++;
          if (count == 12)
            {
              (void) fprintf(image->file,"\n  ");
              count=0;
            };
          bit=0;
          byte=0;
        }
      x++;
      if (x == (int) image->columns)
        {
          if (bit != 0)
            {
              /*
                Write a bitmap byte to the image file.
              */
              byte>>=(8-bit);
              (void) fprintf(image->file,"0x%02x, ",(unsigned int)
                (byte & 0xff));
              count++;
              if (count == 12)
                {
                  (void) fprintf(image->file,"\n  ");
                  count=0;
                };
              bit=0;
              byte=0;
            };
          if (QuantumTick(y,image->rows))
            ProgressMonitor(SaveImageText,y,image->rows);
          x=0;
          y++;
        }
    }
    p++;
  }
  (void) fprintf(image->file,"};\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X P M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXPMImage writes an image to a file in the X pixmap format.
%
%  The format of the WriteXPMImage routine is:
%
%      status=WriteXPMImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXPMImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteXPMImage(const ImageInfo *image_info,Image *image)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    name[MaxTextExtent],
    symbol[MaxTextExtent];

  double
    min_distance;

  int
    j,
    k,
    y;

  long
    mean;

  register double
    distance_squared;

  register int
    distance,
    i,
    runlength,
    x;

  register RunlengthPacket
    *p;

  register const XColorlist
    *q;

  unsigned int
    characters_per_pixel,
    colors,
    transparent;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  transparent=False;
  if (image->class == PseudoClass)
    colors=image->colors;
  else
    {
      QuantizeInfo
        quantize_info;

      unsigned char
        *matte_image;

      /*
        Convert DirectClass to PseudoClass image.
      */
      matte_image=(unsigned char *) NULL;
      if (image->matte)
        {
          /*
            Map all the transparent pixels.
          */
          if (!UncondenseImage(image))
            return(False);
          matte_image=(unsigned char *)
            AllocateMemory(image->packets*sizeof(unsigned char));
          if (matte_image == (unsigned char *) NULL)
            PrematureExit(ResourceLimitWarning,"Memory allocation failed",
              image);
          p=image->pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            matte_image[i]=p->index == Transparent;
            if (matte_image[i])
              transparent=True;
            p++;
          }
        }
      GetQuantizeInfo(&quantize_info);
      quantize_info.dither=image_info->dither;
      (void) QuantizeImage(&quantize_info,image);
      SyncImage(image);
      colors=image->colors;
      if (transparent)
        {
          if (!UncondenseImage(image))
            return(False);
          colors++;
          p=image->pixels;
          for (i=0; i < (int) image->packets; i++)
          {
            if (matte_image[i])
              p->index=image->colors;
            p++;
          }
        }
      if (matte_image != (unsigned char *) NULL)
        FreeMemory((char *) matte_image);
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (int) colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) fprintf(image->file,"/* XPM */\n");
  (void) fprintf(image->file,"static char *magick[] = {\n");
  (void) fprintf(image->file,"/* columns rows colors chars-per-pixel */\n");
  (void) fprintf(image->file,"\"%u %u %u %d\",\n",image->columns,
    image->rows,colors,characters_per_pixel);
  for (i=0; i < (int) colors; i++)
  {
    ColorPacket
      *p;

    /*
      Define XPM color.
    */
    min_distance=0;
    p=image->colormap+i;
    FormatString(name,HexColorFormat,(unsigned int) p->red,
      (unsigned int) p->green,(unsigned int) p->blue);
    for (q=XPMColorlist; q->name != (char *) NULL; q++)
    {
      mean=(DownScale(p->red)+(int) q->red)/2;
      distance=DownScale(p->red)-(int) q->red;
      distance_squared=(2.0*256.0+mean)*distance*distance/256.0;
      distance=DownScale(p->green)-(int) q->green;
      distance_squared+=4.0*(distance*distance);
      distance=DownScale(p->blue)-(int) q->blue;
      distance_squared+=(3.0*256.0-1.0-mean)*distance*distance/256.0;
      if ((q == XPMColorlist) || (distance_squared <= min_distance))
        {
          min_distance=distance_squared;
          if (min_distance == 0.0)
            (void) strcpy(name,q->name);
        }
    }
    if (transparent)
      if (i == (int) (colors-1))
        (void) strcpy(name,"None");
    /*
      Write XPM color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (int) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    (void) fprintf(image->file,"\"%.1024s c %.1024s\",\n",symbol,name);
  }
  /*
    Define XPM pixels.
  */
  (void) fprintf(image->file,"/* pixels */\n");
  p=image->pixels;
  runlength=p->length+1;
  for (y=0; y < (int) image->rows; y++)
  {
    (void) fprintf(image->file,"\"");
    for (x=0; x < (int) image->columns; x++)
    {
      if (runlength != 0)
        runlength--;
      else
        {
          p++;
          runlength=p->length;
        }
      k=p->index % MaxCixels;
      symbol[0]=Cixel[k];
      for (j=1; j < (int) characters_per_pixel; j++)
      {
        k=(((int) p->index-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      (void) fprintf(image->file,"%.1024s",symbol);
    }
    (void) fprintf(image->file,"\"%.1024s\n",
      (y == (int) (image->rows-1) ? "" : ","));
    if (QuantumTick(y,image->rows))
      ProgressMonitor(SaveImageText,y,image->rows);
  }
  (void) fprintf(image->file,"};\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e Y U V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteYUVImage writes an image to a file in the digital YUV
%  (CCIR 601 4:1:1) format.
%
%  The format of the WriteYUVImage routine is:
%
%      status=WriteYUVImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteYUVImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteYUVImage(const ImageInfo *image_info,Image *image)
{
  Image
    *downsampled_image,
    *yuv_image;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned int
    scene;

  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
    }
  if (image_info->interlace == PartitionInterlace)
    {
      AppendImageFormat("Y",image->filename);
      OpenImage(image_info,image,WriteBinaryType);
      if (image->file == (FILE *) NULL)
        PrematureExit(FileOpenWarning,"Unable to open file",image);
    }
  scene=0;
  do
  {
    /*
      Zoom image to an even width and height.
    */
    image->orphan=True;
    yuv_image=ZoomImage(image,
      image->columns+(image->columns & 0x01 ? 1 : 0),
      image->rows+(image->rows & 0x01 ? 1 : 0));
    image->orphan=False;
    if (yuv_image == (Image *) NULL)
      PrematureExit(ResourceLimitWarning,"Unable to zoom image",image);
    /*
      Initialize Y channel.
    */
    if (image->previous == (Image *) NULL)
      ProgressMonitor(SaveImageText,100,400);
    RGBTransformImage(yuv_image,YCbCrColorspace);
    p=yuv_image->pixels;
    for (i=0; i < (int) yuv_image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
        (void) fputc(DownScale(p->red),image->file);
      p++;
    }
    DestroyImage(yuv_image);
    /*
      Downsample image.
    */
    image->orphan=True;
    downsampled_image=ZoomImage(image,
      (image->columns+(image->columns & 0x01 ? 1 : 0)) >> 1,
      (image->rows+(image->rows & 0x01 ? 1 : 0)) >> 1);
    image->orphan=False;
    if (downsampled_image == (Image *) NULL)
      PrematureExit(ResourceLimitWarning,"Unable to zoom image",image);
    /*
      Initialize U channel.
    */
    if (image->previous == (Image *) NULL)
      ProgressMonitor(SaveImageText,200,400);
    if (image_info->interlace == PartitionInterlace)
      {
        CloseImage(image);
        AppendImageFormat("U",image->filename);
        OpenImage(image_info,image,WriteBinaryType);
        if (image->file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to open file",image);
      }
    RGBTransformImage(downsampled_image,YCbCrColorspace);
    p=downsampled_image->pixels;
    for (i=0; i < (int) downsampled_image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
        (void) fputc(DownScale(p->green),image->file);
      p++;
    }
    /*
      Initialize V channel.
    */
    if (image->previous == (Image *) NULL)
      ProgressMonitor(SaveImageText,300,400);
    if (image_info->interlace == PartitionInterlace)
      {
        CloseImage(image);
        AppendImageFormat("V",image->filename);
        OpenImage(image_info,image,WriteBinaryType);
        if (image->file == (FILE *) NULL)
          PrematureExit(FileOpenWarning,"Unable to open file",image);
      }
    p=downsampled_image->pixels;
    for (i=0; i < (int) downsampled_image->packets; i++)
    {
      for (j=0; j <= ((int) p->length); j++)
        (void) fputc(DownScale(p->blue),image->file);
      p++;
    }
    DestroyImage(downsampled_image);
    if (image_info->interlace == PartitionInterlace)
      (void) strcpy(image->filename,image_info->filename);
    if (image->previous == (Image *) NULL)
      ProgressMonitor(SaveImageText,400,400);
    if (image->next == (Image *) NULL)
      break;
    image->next->file=image->file;
    image=image->next;
    ProgressMonitor(SaveImagesText,scene++,image->number_scenes);
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X W D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteXWDImage writes an image to a file in X window dump
%  rasterfile format.
%
%  The format of the WriteXWDImage routine is:
%
%      status=WriteXWDImage(image_info,image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXWDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/
unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
{
  int
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    bits_per_pixel,
    bytes_per_line,
    scanline_pad;

  unsigned long
    lsb_first;

  unsigned short
    value;

  XWDFileHeader
    xwd_header;

  /*
    Open output image file.
  */
  OpenImage(image_info,image,WriteBinaryType);
  if (image->file == (FILE *) NULL)
    PrematureExit(FileOpenWarning,"Unable to open file",image);
  /*
    Initialize XWD file header.
  */
  xwd_header.header_size=sz_XWDheader+Extent(image->filename)+1;
  xwd_header.file_version=(CARD32) XWD_FILE_VERSION;
  xwd_header.pixmap_format=(CARD32) ZPixmap;
  xwd_header.pixmap_depth=(CARD32) (image->class == DirectClass ? 24 : 8);
  xwd_header.pixmap_width=(CARD32) image->columns;
  xwd_header.pixmap_height=(CARD32) image->rows;
  xwd_header.xoffset=(CARD32) 0;
  xwd_header.byte_order=(CARD32) MSBFirst;
  xwd_header.bitmap_unit=(CARD32) (image->class == DirectClass ? 32 : 8);
  xwd_header.bitmap_bit_order=(CARD32) MSBFirst;
  xwd_header.bitmap_pad=(CARD32) (image->class == DirectClass ? 32 : 8);
  bits_per_pixel=(image->class == DirectClass ? 24 : 8);
  xwd_header.bits_per_pixel=(CARD32) bits_per_pixel;
  bytes_per_line=(CARD32) ((((xwd_header.bits_per_pixel*
    xwd_header.pixmap_width)+((xwd_header.bitmap_pad)-1))/
    (xwd_header.bitmap_pad))*((xwd_header.bitmap_pad) >> 3));
  xwd_header.bytes_per_line=(CARD32) bytes_per_line;
  xwd_header.visual_class=(CARD32)
    (image->class == DirectClass ? DirectColor : PseudoColor);
  xwd_header.red_mask=(CARD32) (image->class == DirectClass ? 0xff0000 : 0);
  xwd_header.green_mask=(CARD32) (image->class == DirectClass ? 0xff00 : 0);
  xwd_header.blue_mask=(CARD32) (image->class == DirectClass ? 0xff : 0);
  xwd_header.bits_per_rgb=(CARD32) (image->class == DirectClass ? 24 : 8);
  xwd_header.colormap_entries=(CARD32)
    (image->class == DirectClass ? 256 : image->colors);
  xwd_header.ncolors=(image->class == DirectClass ? 0 : image->colors);
  xwd_header.window_width=(CARD32) image->columns;
  xwd_header.window_height=(CARD32) image->rows;
  xwd_header.window_x=0;
  xwd_header.window_y=0;
  xwd_header.window_bdrwidth=(CARD32) 0;
  /*
    Write XWD header.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &xwd_header,sizeof(xwd_header));
  (void) fwrite((char *) &xwd_header,sz_XWDheader,1,image->file);
  (void) fwrite((char *) image->filename,1,Extent(image->filename)+1,
    image->file);
  if (image->class == PseudoClass)
    {
      XColor
        *colors;

      XWDColor
        color;

      /*
        Dump colormap to file.
      */
      colors=(XColor *) AllocateMemory(image->colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
      for (i=0; i < (int) image->colors; i++)
      {
        colors[i].pixel=i;
        colors[i].red=XUpScale(image->colormap[i].red);
        colors[i].green=XUpScale(image->colormap[i].green);
        colors[i].blue=XUpScale(image->colormap[i].blue);
        colors[i].flags=DoRed | DoGreen | DoBlue;
        colors[i].pad=0;
        if (*(char *) &lsb_first)
          {
            MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(long));
            MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(short));
          }
      }
      for (i=0; i < (int) image->colors; i++)
      {
        color.pixel=(unsigned int) colors[i].pixel;
        color.red=colors[i].red;
        color.green=colors[i].green;
        color.blue=colors[i].blue;
        color.flags=colors[i].flags;
        (void) fwrite((char *) &color,sz_XWDColor,1,image->file);
      }
      FreeMemory((char *) colors);
    }
  /*
    Allocate memory for pixels.
  */
  pixels=(unsigned char *)
    AllocateMemory(image->columns*sizeof(RunlengthPacket));
  if (pixels == (unsigned char *) NULL)
    PrematureExit(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Convert MIFF to XWD raster pixels.
  */
  scanline_pad=(unsigned int)
    (bytes_per_line-((image->columns*bits_per_pixel) >> 3));
  x=0;
  y=0;
  p=image->pixels;
  q=pixels;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      if (image->class == PseudoClass)
        WriteQuantum(p->index,q)
      else
        {
          WriteQuantum(p->red,q);
          WriteQuantum(p->green,q);
          WriteQuantum(p->blue,q);
        }
      x++;
      if (x == (int) image->columns)
        {
          for (x=0; x < (int) scanline_pad; x++)
            WriteQuantum(0,q);
          (void) fwrite((char *) pixels,1,q-pixels,image->file);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(SaveImageText,y,image->rows);
          q=pixels;
          x=0;
          y++;
        }
    }
    p++;
  }
  FreeMemory((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteImage writes an image to a file.  You can specify a
%  particular image format by prefixing the file with the image type and a
%  colon (i.e. ps:image) or specify the image type as the filename suffix
%  (i.e. image.ps).
%
%  The format of the WriteImage routine is:
%
%      status=WriteImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status: Method WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: A pointer to a Image structure.
%
%
*/
Export unsigned int WriteImage(ImageInfo *image_info,Image *image)
{
  DelegateInfo
    delegate_info;

  MagickInfo
    *magick_info;

  unsigned int
    status;

  /*
    Determine image type from filename prefix or suffix (e.g. image.jpg).
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->filename != (char *) NULL);
  assert(image != (Image *) NULL);
  (void) strcpy(image_info->filename,image->filename);
  (void) strcpy(image_info->magick,image->magick);
  SetImageInfo(image_info,True);
  SetNumberScenes(image);
  (void) strcpy(image->filename,image_info->filename);
  if ((image->next == (Image *) NULL) || image_info->adjoin)
    if ((image->previous == (Image *) NULL) && !IsTainted(image))
      if (IsAccessible(image->magick_filename))
        if (GetDelegateInfo(image->magick,image_info->magick,&delegate_info))
          {
            /*
              Let our bi-directional delegate process the image.
            */
            (void) strcpy(image->filename,image->magick_filename);
            status=
              InvokeDelegate(image_info,image,image->magick,image_info->magick);
            return(status);
          }
  /*
    Call appropriate image writer based on image type.
  */
  status=False;
  magick_info=(MagickInfo *) GetMagickInfo(image_info->magick);
  if ((magick_info != (MagickInfo *) NULL) &&
      (magick_info->encoder !=
      (unsigned int (*)(const ImageInfo *,Image *)) NULL))
    status=(magick_info->encoder)(image_info,image);
  else
    if (!GetDelegateInfo((char *) NULL,image_info->magick,&delegate_info))
      {
        MagickWarning(MissingDelegateWarning,
          "no encode delegate for this image format",image_info->magick);
        magick_info=(MagickInfo *) GetMagickInfo(image->magick);
        if ((magick_info != (MagickInfo *) NULL) &&
            (magick_info->encoder !=
            (unsigned int (*)(const ImageInfo *,Image *)) NULL))
          status=(magick_info->encoder)(image_info,image);
        else
          MagickWarning(MissingDelegateWarning,
            "no encode delegate for this image format",image->magick);
      }
    else
      {
        /*
          Let our encoding delegate process the image.
        */
        TemporaryFilename(image->filename);
        status=
          InvokeDelegate(image_info,image,(char *) NULL,image_info->magick);
        (void) remove(image->filename);
        return(status);
      }
  if (image->status)
    {
      MagickWarning(CorruptImageWarning,"An error has occurred writing to file",
        image->filename);
      return(False);
    }
  (void) strcpy(image->magick,image_info->magick);
  return(status);
}
