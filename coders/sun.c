/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS  U   U  N   N                              %
%                            SS     U   U  NN  N                              %
%                             SSS   U   U  N N N                              %
%                               SS  U   U  N  NN                              %
%                            SSSSS   UUU   N   N                              %
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
  WriteSUNImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S U N                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSUN returns True if the image format type, identified by the
%  magick string, is SUN.
%
%  The format of the IsSUN method is:
%
%      unsigned int IsSUN(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSUN returns True if the image format type is SUN.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSUN(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (memcmp(magick,"\131\246\152\225",4) == 0)
    return(True);
  return(False);
}

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
%  Method DecodeImage unpacks the packed image pixels into
%  runlength-encoded pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(const unsigned char *compressed_pixels,
%        const size_t length,unsigned char *pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%    o length:  An integer value that is the total number of bytes of the
%      source image (as just read by ReadBlob)
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the uncompression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels.
%
%
*/
static unsigned int DecodeImage(const unsigned char *compressed_pixels,
  const size_t length,unsigned char *pixels)
{
  register const unsigned char
    *p;

  register int
    count;

  register unsigned char
    *q;

  unsigned char
    byte;

  assert(compressed_pixels != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  p=compressed_pixels;
  q=pixels;
  while ((size_t) (p-compressed_pixels) < length)
  {
    byte=(*p++);
    if (byte != 128)
      *q++=byte;
    else
      {
        /*
          Runlength-encoded packet: <count><byte>
        */
        count=(*p++);
        if (count > 0)
          byte=(*p++);
        while (count >= 0)
        {
          *q++=byte;
          count--;
        }
     }
  }
  return(True);
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
%  The format of the ReadSUNImage method is:
%
%      Image *ReadSUNImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSUNImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadSUNImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_ENCODED  2
#define RT_FORMAT_RGB  3

  typedef struct _SUNInfo
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
  } SUNInfo;

  Image
    *image;

  int
    bit,
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count;

  SUNInfo
    sun_info;

  unsigned char
    *sun_data,
    *sun_pixels;

  unsigned int
    status;

  unsigned long
    bytes_per_line;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read SUN raster header.
  */
  sun_info.magic=ReadBlobMSBLong(image);
  do
  {
    /*
      Verify SUN identifier.
    */
    if (sun_info.magic != 0x59a66a95)
      ThrowReaderException(CorruptImageWarning,"Not a SUN raster image",image);
    sun_info.width=ReadBlobMSBLong(image);
    sun_info.height=ReadBlobMSBLong(image);
    sun_info.depth=ReadBlobMSBLong(image);
    sun_info.length=ReadBlobMSBLong(image);
    sun_info.type=ReadBlobMSBLong(image);
    sun_info.maptype=ReadBlobMSBLong(image);
    sun_info.maplength=ReadBlobMSBLong(image);
    image->columns=(unsigned int) sun_info.width;
    image->rows=(unsigned int) sun_info.height;
    image->depth=sun_info.depth <= 8 ? 8 : QuantumDepth;
    if (sun_info.depth < 24)
      {
        image->storage_class=PseudoClass;
        image->colors=sun_info.maplength;
        if (sun_info.maptype == RMT_NONE)
          image->colors=1 << sun_info.depth;
        if (sun_info.maptype == RMT_EQUAL_RGB)
          image->colors=sun_info.maplength/3;
      }
    switch (sun_info.maptype)
    {
      case RMT_NONE:
      {
        if (sun_info.depth < 24)
          {
            /*
              Create linear color ramp.
            */
            if (!AllocateImageColormap(image,image->colors))
              ThrowReaderException(ResourceLimitWarning,
                "Memory allocation failed",image);
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
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        sun_colormap=(unsigned char *) AcquireMemory(image->colors);
        if (sun_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) ReadBlob(image,image->colors,(char *) sun_colormap);
        for (i=0; i < image->colors; i++)
          image->colormap[i].red=UpScale(sun_colormap[i]);
        (void) ReadBlob(image,image->colors,(char *) sun_colormap);
        for (i=0; i < image->colors; i++)
          image->colormap[i].green=UpScale(sun_colormap[i]);
        (void) ReadBlob(image,image->colors,(char *) sun_colormap);
        for (i=0; i < image->colors; i++)
          image->colormap[i].blue=UpScale(sun_colormap[i]);
        LiberateMemory((void **) &sun_colormap);
        break;
      }
      case RMT_RAW:
      {
        unsigned char
          *sun_colormap;

        /*
          Read SUN raster colormap.
        */
        sun_colormap=(unsigned char *) AcquireMemory(sun_info.maplength);
        if (sun_colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) ReadBlob(image,sun_info.maplength,(char *) sun_colormap);
        LiberateMemory((void **) &sun_colormap);
        break;
      }
      default:
        ThrowReaderException(CorruptImageWarning,
          "Colormap type is not supported",image)
    }
    sun_data=(unsigned char *) AcquireMemory(sun_info.length);
    if (sun_data == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    count=ReadBlob(image,sun_info.length,(char *) sun_data);
    if ((count == 0) && (sun_info.type != RT_ENCODED))
      ThrowReaderException(CorruptImageWarning,"Unable to read image data",
        image);
    sun_pixels=sun_data;
    if (sun_info.type == RT_ENCODED)
      {
        unsigned long
          height;

        /*
          Read run-length encoded raster pixels.
        */
        height=sun_info.height;
        bytes_per_line=2*(sun_info.width*sun_info.depth+15)/16;
        sun_pixels=(unsigned char *) AcquireMemory(bytes_per_line*height);
        if (sun_pixels == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) DecodeImage(sun_data,sun_info.length,sun_pixels);
        LiberateMemory((void **) &sun_data);
      }
    /*
      Initialize image structure.
    */
    image->matte=(sun_info.depth == 32);
    image->columns=(unsigned int) sun_info.width;
    image->rows=(unsigned int) sun_info.height;
    /*
      Convert SUN raster image to pixel packets.
    */
    p=sun_pixels;
    if (sun_info.depth == 1)
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < ((int) image->columns-7); x+=8)
        {
          for (bit=7; bit >= 0; bit--)
            indexes[x+7-bit]=((*p) & (0x01 << bit) ? 0x00 : 0x01);
          p++;
        }
        if ((image->columns % 8) != 0)
          {
            for (bit=7; bit >= (int) (8-(image->columns % 8)); bit--)
              indexes[x+7-bit]=((*p) & (0x01 << bit) ? 0x00 : 0x01);
            p++;
          }
        if ((((image->columns/8)+(image->columns % 8 ? 1 : 0)) % 2) != 0)
          p++;
        if (!SyncImagePixels(image))
          break;
        if (image->previous == (Image *) NULL)
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,y,image->rows);
      }
    else
      if (image->storage_class == PseudoClass)
        for (y=0; y < (int) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (int) image->columns; x++)
            indexes[x]=(*p++);
          if ((image->columns % 2) != 0)
            p++;
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
      else
        for (y=0; y < (int) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            if (image->matte)
              q->opacity=MaxRGB-UpScale(*p++);
            if (sun_info.type == RT_STANDARD)
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
            q++;
          }
          if (((image->columns % 2) != 0) && (image->matte == False))
            p++;
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
    if (image->storage_class == PseudoClass)
      SyncImage(image);
    LiberateMemory((void **) &sun_pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    sun_info.magic=ReadBlobMSBLong(image);
    if (sun_info.magic == 0x59a66a95)
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
  } while (sun_info.magic == 0x59a66a95);
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
%   R e g i s t e r S U N I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSUNImage adds attributes for the SUN image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSUNImage method is:
%
%      RegisterSUNImage(void)
%
*/
ModuleExport void RegisterSUNImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RAS");
  entry->decoder=ReadSUNImage;
  entry->encoder=WriteSUNImage;
  entry->magick=IsSUN;
  entry->description=AllocateString("SUN Rasterfile");
  entry->module=AllocateString("SUN");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("SUN");
  entry->decoder=ReadSUNImage;
  entry->encoder=WriteSUNImage;
  entry->description=AllocateString("SUN Rasterfile");
  entry->module=AllocateString("SUN");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S U N I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSUNImage removes format registrations made by the
%  SUN module from the list of supported formats.
%
%  The format of the UnregisterSUNImage method is:
%
%      UnregisterSUNImage(void)
%
*/
ModuleExport void UnregisterSUNImage(void)
{
  UnregisterMagickInfo("RAS");
  UnregisterMagickInfo("SUN");
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
%  The format of the WriteSUNImage method is:
%
%      unsigned int WriteSUNImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WriteSUNImage(const ImageInfo *image_info,Image *image)
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_FORMAT_RGB  3

  typedef struct _SUNInfo
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
  } SUNInfo;

  int
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *p;

  register long
    i;

  SUNInfo
    sun_info;

  unsigned int
    scene,
    status;

  unsigned long
    number_pixels;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize SUN raster file header.
    */
    TransformRGBImage(image,RGBColorspace);
    sun_info.magic=0x59a66a95;
    sun_info.width=image->columns;
    sun_info.height=image->rows;
    sun_info.type=
      (image->storage_class == DirectClass ? RT_FORMAT_RGB : RT_STANDARD);
    sun_info.maptype=RMT_NONE;
    sun_info.maplength=0;
    number_pixels=image->columns*image->rows;
    if (image->storage_class == DirectClass)
      {
        /*
          Full color SUN raster.
        */
        sun_info.depth=(image->matte ? 32 : 24);
        sun_info.length=(image->matte ? 4 : 3)*number_pixels;
        sun_info.length+=image->columns & 0x01 ? image->rows : 0;
      }
    else
      if (IsMonochromeImage(image))
        {
          /*
            Monochrome SUN raster.
          */
          sun_info.depth=1;
          sun_info.length=((image->columns+7) >> 3)*image->rows;
          sun_info.length+=((image->columns/8)+(image->columns % 8 ? 1 : 0)) %
            2 ? image->rows : 0;
        }
      else
        {
          /*
            Colormapped SUN raster.
          */
          sun_info.depth=8;
          sun_info.length=number_pixels;
          sun_info.length+=image->columns & 0x01 ? image->rows : 0;
          sun_info.maptype=RMT_EQUAL_RGB;
          sun_info.maplength=image->colors*3;
        }
    /*
      Write SUN header.
    */
    WriteBlobMSBLong(image,sun_info.magic);
    WriteBlobMSBLong(image,sun_info.width);
    WriteBlobMSBLong(image,sun_info.height);
    WriteBlobMSBLong(image,sun_info.depth);
    WriteBlobMSBLong(image,sun_info.length);
    WriteBlobMSBLong(image,sun_info.type);
    WriteBlobMSBLong(image,sun_info.maptype);
    WriteBlobMSBLong(image,sun_info.maplength);
    /*
      Convert MIFF to SUN raster pixels.
    */
    x=0;
    y=0;
    if (image->storage_class == DirectClass)
      {
        register unsigned char
          *q;

        size_t
          length;

        unsigned char
          *pixels;

        /*
          Allocate memory for pixels.
        */
        length=image->columns*sizeof(PixelPacket);
        pixels=(unsigned char *) AcquireMemory(length);
        if (pixels == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
        /*
          Convert DirectClass packet to SUN RGB pixel.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          p=GetImagePixels(image,0,y,image->columns,1);
          if (p == (PixelPacket *) NULL)
            break;
          q=pixels;
          for (x=0; x < (int) image->columns; x++)
          {
            if (image->matte)
              *q++=MaxRGB-DownScale(p->opacity);
            *q++=DownScale(p->red);
            *q++=DownScale(p->green);
            *q++=DownScale(p->blue);
            p++;
          }
          if (image->columns & 0x01)
            *q++=0;  /* pad scanline */
          (void) WriteBlob(image,q-pixels,(char *) pixels);
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(SaveImageText,y,image->rows);
        }
        LiberateMemory((void **) &pixels);
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
          polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
          if (image->colors == 2)
            polarity=
              Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            bit=0;
            byte=0;
            for (x=0; x < (int) image->columns; x++)
            {
              byte<<=1;
              if (indexes[x] == polarity)
                byte|=0x01;
              bit++;
              if (bit == 8)
                {
                  (void) WriteBlobByte(image,byte);
                  bit=0;
                  byte=0;
                }
              p++;
            }
            if (bit != 0)
              (void) WriteBlobByte(image,byte << (8-bit));
            if ((((image->columns/8)+
                (image->columns % 8 ? 1 : 0)) % 2) != 0)
              (void) WriteBlobByte(image,0);  /* pad scanline */
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(SaveImageText,y,image->rows);
          }
        }
      else
        {
          /*
            Dump colormap to file.
          */
          for (i=0; i < image->colors; i++)
            (void) WriteBlobByte(image,DownScale(image->colormap[i].red));
          for (i=0; i < image->colors; i++)
            (void) WriteBlobByte(image,DownScale(image->colormap[i].green));
          for (i=0; i < image->colors; i++)
            (void) WriteBlobByte(image,DownScale(image->colormap[i].blue));
          /*
            Convert PseudoClass packet to SUN colormapped pixel.
          */
          for (y=0; y < (int) image->rows; y++)
          {
            p=GetImagePixels(image,0,y,image->columns,1);
            if (p == (PixelPacket *) NULL)
              break;
            indexes=GetIndexes(image);
            for (x=0; x < (int) image->columns; x++)
            {
              (void) WriteBlobByte(image,indexes[x]);
              p++;
            }
            if (image->columns & 0x01)
              (void) WriteBlobByte(image,0);  /* pad scanline */
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                MagickMonitor(SaveImageText,y,image->rows);
          }
        }
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
