/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                         GGGG  RRRR    AAA   Y   Y                           %
%                        G      R   R  A   A   Y Y                            %
%                        G  GG  RRRR   AAAAA    Y                             %
%                        G   G  R R    A   A    Y                             %
%                         GGG   R  R   A   A    Y                             %
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
^L
/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include "proxy.h"

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
Export Image *ReadGRAYImage(const ImageInfo *image_info)
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
    ReaderExit(OptionWarning,"must specify image size",image);
  /*
    Open image file.
  */
  OpenImage(image_info,image,ReadBinaryType);
  if (image->file == (FILE *) NULL)
    ReaderExit(FileOpenWarning,"Unable to open file",image);
  for (i=0; i < image->offset; i++)
    (void) fgetc(image->file);
  /*
    Allocate memory for a scanline.
  */
  packet_size=image->depth >> 3;
  scanline=(unsigned char *)
    AllocateMemory(packet_size*image->tile_info.width*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
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
      ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
    /*
      Create linear colormap.
    */
    image->class=PseudoClass;
    image->colors=1 << image->depth;
    image->colormap=(ColorPacket *)
      AllocateMemory(image->colors*sizeof(ColorPacket));
    if (image->colormap == (ColorPacket *) NULL)
      ReaderExit(ResourceLimitWarning,"Memory allocation failed",image);
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
                    ReaderExit(ResourceLimitWarning,
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
Export unsigned int WriteGRAYImage(const ImageInfo *image_info,Image *image)
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
    WriterExit(FileOpenWarning,"Unable to open file",image);
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
    TransformRGBImage(image,RGBColorspace);
    pixels=(unsigned char *)
      AllocateMemory(image->columns*sizeof(RunlengthPacket));
    if (pixels == (unsigned char *) NULL)
      WriterExit(ResourceLimitWarning,"Memory allocation failed",image);
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
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseImage(image);
  return(True);
}
