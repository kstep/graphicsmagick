/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        M   M   OOO   N   N   OOO                            %
%                        MM MM  O   O  NN  N  O   O                           %
%                        M M M  O   O  N N N  O   O                           %
%                        M   M  O   O  N  NN  O   O                           %
%                        M   M   OOO   N   N   OOO                            %
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
Export Image *ReadMONOImage(const ImageInfo *image_info)
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
    ReaderExit(FileOpenWarning,"Unable to open file",image);
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
    ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
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
                ReaderExit(ResourceLimitWarning,"Memory allocation failed",
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
Export unsigned int WriteMONOImage(const ImageInfo *image_info,Image *image)
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
    WriterExit(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
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
