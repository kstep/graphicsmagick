/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            RRRR    GGGG  BBBB                               %
%                            R   R  G      B   B                              %
%                            RRRR   G  GG  BBBB                               %
%                            R R    G   G  B   B                              %
%                            R  R    GGG   BBBB                               %
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
  Forward declarations.
*/
static unsigned int
  WriteRGBImage(const ImageInfo *,Image *);

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
%  The format of the ReadRGBImage method is:
%
%      Image *ReadRGBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRGBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadRGBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  int
    y;

  register int
    x;

  register size_t
    i;

  size_t
    count;

  unsigned char
    *scanline;

  unsigned int
    packet_size,
    status;

  image=AllocateImage(image_info);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(OptionWarning,"Must specify image size",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open image file.
      */
      status=OpenBlob(image_info,image,ReadBinaryType);
      if (status == False)
        ThrowReaderException(FileOpenWarning,"Unable to open file",image);
      for (i=0; i < image->offset; i++)
        (void) ReadBlobByte(image);
    }
  /*
    Allocate memory for a scanline.
  */
  packet_size=image->depth > 8 ? 6 : 3;
  if (LocaleCompare(image_info->magick,"RGBA") == 0)
    {
      image->matte=True;
      packet_size=image->depth > 8 ? 8 : 4;
    }
  scanline=(unsigned char *)
    AcquireMemory(packet_size*image->tile_info.width);
  if (scanline == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
    {
      /*
        Skip to next image.
      */
      image->scene++;
      for (y=0; y < (int) image->rows; y++)
        (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
    }
  x=packet_size*image->tile_info.x;
  do
  {
    /*
      Convert raster image to pixel packets.
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
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          if (!SetImagePixels(image,0,y,image->columns,1))
            break;
          if (!image->matte)
            (void) PushImagePixels(image,RGBQuantum,scanline+x);
          else
            (void) PushImagePixels(image,RGBAQuantum,scanline+x);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        packet_size=image->depth > 8 ? 2 : 1;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          if (!SetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PushImagePixels(image,RedQuantum,scanline+x);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) PushImagePixels(image,GreenQuantum,scanline+x);
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          (void) PushImagePixels(image,BlueQuantum,scanline+x);
          if (image->matte)
            {
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
              (void) PushImagePixels(image,AlphaQuantum,scanline+x);
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
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
            status=OpenBlob(image_info,image,ReadBinaryType);
            if (status == False)
              ThrowReaderException(FileOpenWarning,"Unable to open file",image);
          }
        packet_size=image->depth > 8 ? 2 : 1;
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        i=0;
        span=image->rows*(image->matte ? 4 : 3);
        for (y=0; y < (int) image->rows; y++)
        {
          if ((y > 0) || (image->previous == (Image *) NULL))
            (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          if (!SetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PushImagePixels(image,RedQuantum,scanline+x);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              MagickMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("G",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryType);
            if (status == False)
              ThrowReaderException(FileOpenWarning,"Unable to open file",image);
          }
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PushImagePixels(image,GreenQuantum,scanline+x);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              MagickMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("B",image->filename);
            status=OpenBlob(image_info,image,ReadBinaryType);
            if (status == False)
              ThrowReaderException(FileOpenWarning,"Unable to open file",image);
          }
        for (y=0; y < image->tile_info.y; y++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        for (y=0; y < (int) image->rows; y++)
        {
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PushImagePixels(image,BlueQuantum,scanline+x);
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,span))
              MagickMonitor(LoadImageText,i,span);
          i++;
        }
        count=image->tile_info.height-image->rows-image->tile_info.y;
        for (i=0; i < count; i++)
          (void) ReadBlob(image,packet_size*image->tile_info.width,scanline);
        if (image->matte)
          {
            /*
              Read matte channel.
            */
            if (image_info->interlace == PartitionInterlace)
              {
                CloseBlob(image);
                AppendImageFormat("A",image->filename);
                status=OpenBlob(image_info,image,ReadBinaryType);
                if (status == False)
                  ThrowReaderException(FileOpenWarning,"Unable to open file",
                    image);
              }
            for (y=0; y < image->tile_info.y; y++)
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
            for (y=0; y < (int) image->rows; y++)
            {
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
              if (!GetImagePixels(image,0,y,image->columns,1))
                break;
              (void) PushImagePixels(image,AlphaQuantum,scanline+x);
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,span))
                  MagickMonitor(LoadImageText,i,span);
              i++;
            }
            count=image->tile_info.height-image->rows-image->tile_info.y;
            for (i=0; i < count; i++)
              (void) ReadBlob(image,packet_size*image->tile_info.width,
                scanline);
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        break;
      }
    }
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,packet_size*image->tile_info.width,scanline);
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
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
      }
  } while (count > 0);
  LiberateMemory((void **) &scanline);
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
%   R e g i s t e r R G B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterRGBImage adds attributes for the RGB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterRGBImage method is:
%
%      RegisterRGBImage(void)
%
*/
ModuleExport void RegisterRGBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RGB");
  entry->decoder=ReadRGBImage;
  entry->encoder=WriteRGBImage;
  entry->raw=True;
  entry->description=AllocateString("Raw red, green, and blue bytes");
  entry->module=AllocateString("RGB");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("RGBA");
  entry->decoder=ReadRGBImage;
  entry->encoder=WriteRGBImage;
  entry->raw=True;
  entry->description=AllocateString("Raw red, green, blue, and matte bytes");
  entry->module=AllocateString("RGB");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterRGBImage removes format registrations made by the
%  RGB module from the list of supported formats.
%
%  The format of the UnregisterRGBImage method is:
%
%      UnregisterRGBImage(void)
%
*/
ModuleExport void UnregisterRGBImage(void)
{
  UnregisterMagickInfo("RGB");
  UnregisterMagickInfo("RGBA");
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
%  Method WriteRGBImage writes an image to a file in red, green, and blue
%  rasterfile format.
%
%  The format of the WriteRGBImage method is:
%
%      unsigned int WriteRGBImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WriteRGBImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    scene,
    status;

  /*
    Allocate memory for pixels.
  */
  packet_size=image->depth > 8 ? 6 : 3;
  if (LocaleCompare(image_info->magick,"RGBA") == 0)
    packet_size=image->depth > 8 ? 8 : 4;
  pixels=(unsigned char *) AcquireMemory(packet_size*image->columns);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  if (image_info->interlace != PartitionInterlace)
    {
      /*
        Open output image file.
      */
      status=OpenBlob(image_info,image,WriteBinaryType);
      if (status == False)
        ThrowWriterException(FileOpenWarning,"Unable to open file",image);
    }
  scene=0;
  do
  {
    /*
      Convert MIFF to RGB raster pixels.
    */
    TransformRGBImage(image,RGBColorspace);
    if (LocaleCompare(image_info->magick,"RGBA") == 0)
      if (!image->matte)
        SetImageOpacity(image,OpaqueOpacity);
    switch (image_info->interlace)
    {
      case NoInterlace:
      default:
      {
        /*
          No interlacing:  RGBRGBRGBRGBRGBRGB...
        */
        for (y=0; y < (int) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          if (LocaleCompare(image_info->magick,"RGBA") != 0)
            {
              (void) PopImagePixels(image,RGBQuantum,pixels);
              (void) WriteBlob(image,packet_size*image->columns,pixels);
            }
          else
            {
              (void) PopImagePixels(image,RGBAQuantum,pixels);
              (void) WriteBlob(image,packet_size*image->columns,pixels);
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        break;
      }
      case LineInterlace:
      {
        /*
          Line interlacing:  RRR...GGG...BBB...RRR...GGG...BBB...
        */
        for (y=0; y < (int) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,RedQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
          (void) PopImagePixels(image,GreenQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
          (void) PopImagePixels(image,BlueQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
          if (LocaleCompare(image_info->magick,"RGBA") == 0)
            {
              (void) PopImagePixels(image,AlphaQuantum,pixels);
              (void) WriteBlob(image,image->columns,pixels);
            }
          if (QuantumTick(y,image->rows))
            MagickMonitor(SaveImageText,y,image->rows);
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
            status=OpenBlob(image_info,image,WriteBinaryType);
            if (status == False)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
          }
        for (y=0; y < (int) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,RedQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("G",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryType);
            if (status == False)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
          }
        MagickMonitor(SaveImageText,100,400);
        for (y=0; y < (int) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,GreenQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (image_info->interlace == PartitionInterlace)
          {
            CloseBlob(image);
            AppendImageFormat("B",image->filename);
            status=OpenBlob(image_info,image,WriteBinaryType);
            if (status == False)
              ThrowWriterException(FileOpenWarning,"Unable to open file",image);
          }
        MagickMonitor(SaveImageText,200,400);
        for (y=0; y < (int) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,BlueQuantum,pixels);
          (void) WriteBlob(image,image->columns,pixels);
        }
        if (LocaleCompare(image_info->magick,"RGBA") == 0)
          {
            MagickMonitor(SaveImageText,300,400);
            if (image_info->interlace == PartitionInterlace)
              {
                CloseBlob(image);
                AppendImageFormat("A",image->filename);
                status=OpenBlob(image_info,image,WriteBinaryType);
                if (status == False)
                  ThrowWriterException(FileOpenWarning,"Unable to open file",
                    image);
              }
            for (y=0; y < (int) image->rows; y++)
            {
              if (!GetImagePixels(image,0,y,image->columns,1))
                break;
              (void) PopImagePixels(image,AlphaQuantum,pixels);
              (void) WriteBlob(image,image->columns,pixels);
            }
          }
        if (image_info->interlace == PartitionInterlace)
          (void) strcpy(image->filename,image_info->filename);
        MagickMonitor(SaveImageText,400,400);
        break;
      }
    }
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    MagickMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  LiberateMemory((void **) &pixels);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
