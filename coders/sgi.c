/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            SSSSS   GGGG  IIIII                              %
%                            SS     G        I                                %
%                             SSS   G  GG    I                                %
%                               SS  G   G    I                                %
%                            SSSSS   GGG   IIIII                              %
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
%  Copyright (C) 2000 ImageMagick Studio, a non-profit organization dedicated %
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
  Forward declarations.
*/
static unsigned int
  WriteSGIImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s S G I                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsSGI returns True if the image format type, identified by the
%  magick string, is SGI.
%
%  The format of the IsSGI method is:
%
%      unsigned int IsSGI(const unsigned char *magick,
%        const unsigned int length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsSGI returns True if the image format type is SGI.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsSGI(const unsigned char *magick,const unsigned int length)
{
  if (length < 2)
    return(False);
  if (LocaleNCompare((char *) magick,"\001\332",2) == 0)
    return(True);
  return(False);
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
%  The format of the ReadSGIImage method is:
%
%      Image *ReadSGIImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadSGIImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
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

static Image *ReadSGIImage(const ImageInfo *image_info,ExceptionInfo *exception)
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

  int
    y,
    z;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  SGIHeader
    iris_header;

  unsigned char
    *iris_pixels;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read SGI raster header.
  */
  iris_header.magic=MSBFirstReadShort(image);
  do
  {
    /*
      Verify SGI identifier.
    */
    if (iris_header.magic != 0x01DA)
      ThrowReaderException(CorruptImageWarning,"Not a SGI RGB image",image);
    iris_header.storage=ReadByte(image);
    iris_header.bytes_per_pixel=ReadByte(image);
    if (iris_header.bytes_per_pixel != 1)
      ThrowReaderException(CorruptImageWarning,
        "Image must have 1 byte per pixel channel",image);
    iris_header.dimension=MSBFirstReadShort(image);
    iris_header.columns=MSBFirstReadShort(image);
    iris_header.rows=MSBFirstReadShort(image);
    iris_header.depth=MSBFirstReadShort(image);
    image->columns=iris_header.columns;
    image->rows=iris_header.rows;
    image->depth=iris_header.depth <= 8 ? 8 : QuantumDepth;
    if (iris_header.depth < 3)
      {
        image->class=PseudoClass;
        image->colors=256;
      }
    if (image_info->ping)
      {
        CloseBlob(image);
        return(image);
      }
    iris_header.minimum_value=MSBFirstReadLong(image);
    iris_header.maximum_value=MSBFirstReadLong(image);
    (void) ReadBlob(image,(unsigned int) sizeof(iris_header.filler),
      (char *) iris_header.filler);
    /*
      Allocate SGI pixels.
    */
    iris_pixels=(unsigned char *)
      AllocateMemory(4*iris_header.columns*iris_header.rows);
    if (iris_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    if (iris_header.storage != 0x01)
      {
        unsigned char
          *scanline;

        /*
          Read standard image format.
        */
        scanline=(unsigned char *) AllocateMemory(iris_header.columns);
        if (scanline == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        for (z=0; z < (int) iris_header.depth; z++)
        {
          p=iris_pixels+z;
          for (y=0; y < (int) iris_header.rows; y++)
          {
            (void) ReadBlob(image,iris_header.columns,(char *) scanline);
            for (x=0; x < (int) iris_header.columns; x++)
            {
              *p=scanline[x];
              p+=4;
            }
          }
        }
        FreeMemory((void **) &scanline);
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
        max_packets=(unsigned char *) AllocateMemory(2*iris_header.columns+10);
        runlength=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (max_packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          offsets[i]=MSBFirstReadLong(image);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          runlength[i]=MSBFirstReadLong(image);
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
                    (void) SeekBlob(image,(long) offset,SEEK_SET);
                  }
                (void) ReadBlob(image,(unsigned int)
                  runlength[y+z*iris_header.rows],(char *) max_packets);
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
                    (void) SeekBlob(image,(long) offset,SEEK_SET);
                  }
                (void) ReadBlob(image,(unsigned int)
                  runlength[y+z*iris_header.rows],(char *) max_packets);
                offset+=runlength[y+z*iris_header.rows];
                SGIDecode(max_packets,p+z);
              }
              p+=(iris_header.columns*4);
            }
          }
        FreeMemory((void **) &runlength);
        FreeMemory((void **) &max_packets);
        FreeMemory((void **) &offsets);
      }
    /*
      Initialize image structure.
    */
    image->matte=iris_header.depth == 4;
    image->columns=iris_header.columns;
    image->rows=iris_header.rows;
    /*
      Convert SGI raster image to pixel packets.
    */
    if (image->class == DirectClass)
      {
        /*
          Convert SGI image to DirectClass pixel packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          p=iris_pixels+((image->rows-1)-y)*(image->columns*4);
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p);
            q->green=UpScale(*(p+1));
            q->blue=UpScale(*(p+2));
            q->opacity=UpScale(*(p+3));
            p+=4;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
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
        image->colormap=(PixelPacket *)
          AllocateMemory(image->colors*sizeof(PixelPacket));
        if (image->colormap == (PixelPacket *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        for (i=0; i < (int) image->colors; i++)
        {
          image->colormap[i].red=(Quantum) UpScale(i);
          image->colormap[i].green=(Quantum) UpScale(i);
          image->colormap[i].blue=(Quantum) UpScale(i);
        }
        /*
          Convert SGI image to PseudoClass pixel packets.
        */
        for (y=0; y < (int) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          p=iris_pixels+((image->rows-1)-y)*(image->columns*4);
          for (x=0; x < (int) image->columns; x++)
          {
            index=(unsigned short) (*p);
            indexes[x]=index;
            p+=4;
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,y,image->rows);
        }
        SyncImage(image);
      }
    FreeMemory((void **) &iris_pixels);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    iris_header.magic=MSBFirstReadShort(image);
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
        ProgressMonitor(LoadImagesText,TellBlob(image),image->filesize);
      }
  } while (iris_header.magic == 0x01DA);
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
%   R e g i s t e r S G I I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterSGIImage adds attributes for the SGI image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterSGIImage method is:
%
%      RegisterSGIImage(void)
%
*/
ModuleExport void RegisterSGIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SGI");
  entry->decoder=ReadSGIImage;
  entry->encoder=WriteSGIImage;
  entry->magick=IsSGI;
  entry->description=AllocateString("Irix RGB image");
  entry->module=AllocateString("SGI");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r S G I I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterSGIImage removes format registrations made by the
%  SGI module from the list of supported formats.
%
%  The format of the UnregisterSGIImage method is:
%
%      UnregisterSGIImage(void)
%
*/
ModuleExport void UnregisterSGIImage(void)
{
  UnregisterMagickInfo("SGI");
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
%  The format of the WriteSGIImage method is:
%
%      unsigned int WriteSGIImage(const ImageInfo *image_info,Image *image)
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

  register unsigned char
    *p,
    *q;

  unsigned char
    *limit,
    *mark;

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

static unsigned int WriteSGIImage(const ImageInfo *image_info,Image *image)
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

  int
    y,
    z;

  SGIHeader
    iris_header;

  register int
    i,
    x;

  register PixelPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *iris_pixels,
    *packets;

  unsigned int
    scene,
    status;

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
      Initialize SGI raster file header.
    */
    TransformRGBImage(image,RGBColorspace);
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
    MSBFirstWriteShort(image,iris_header.magic);
    (void) WriteByte(image,iris_header.storage);
    (void) WriteByte(image,iris_header.bytes_per_pixel);
    MSBFirstWriteShort(image,iris_header.dimension);
    MSBFirstWriteShort(image,iris_header.columns);
    MSBFirstWriteShort(image,iris_header.rows);
    MSBFirstWriteShort(image,iris_header.depth);
    MSBFirstWriteLong(image,iris_header.minimum_value);
    MSBFirstWriteLong(image,iris_header.maximum_value);
    (void) WriteBlob(image,sizeof(iris_header.filler),
      (char *) iris_header.filler);
    /*
      Allocate SGI pixels.
    */
    iris_pixels=(unsigned char *) AllocateMemory(4*image->columns*image->rows);
    if (iris_pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    /*
      Convert image pixels to uncompressed SGI pixels.
    */
    for (y=0; y < (int) image->rows; y++)
    {
      p=GetImagePixels(image,0,y,image->columns,1);
      if (p == (PixelPacket *) NULL)
        break;
      q=iris_pixels+((iris_header.rows-1)-y)*(iris_header.columns*4);
      for (x=0; x < (int) image->columns; x++)
      {
        *q++=DownScale(p->red);
        *q++=DownScale(p->green);
        *q++=DownScale(p->blue);
        *q++=p->opacity;
        p++;
      }
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(SaveImageText,y,image->rows);
    }
    if (image_info->compression == NoCompression)
      {
        unsigned char
          *scanline;

        /*
          Write uncompressed SGI pixels.
        */
        scanline=(unsigned char *) AllocateMemory(iris_header.columns);
        if (scanline == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
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
            (void) WriteBlob(image,iris_header.columns,(char *) scanline);
          }
        }
        FreeMemory((void **) &scanline);
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
          Convert SGI uncompressed pixels.
        */
        offsets=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        packets=(unsigned char *)
          AllocateMemory(4*(2*iris_header.columns+10)*image->rows);
        runlength=(unsigned long *) AllocateMemory(iris_header.rows*
          iris_header.depth*sizeof(unsigned long));
        if ((offsets == (unsigned long *) NULL) ||
            (packets == (unsigned char *) NULL) ||
            (runlength == (unsigned long *) NULL))
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
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
          MSBFirstWriteLong(image,offsets[i]);
        for (i=0; i < (int) (iris_header.rows*iris_header.depth); i++)
          MSBFirstWriteLong(image,runlength[i]);
        (void) WriteBlob(image,number_packets,(char *) packets);
        /*
          Free memory.
        */
        FreeMemory((void **) &runlength);
        FreeMemory((void **) &packets);
        FreeMemory((void **) &offsets);
      }
    FreeMemory((void **) &iris_pixels);
    if (image->next == (Image *) NULL)
      break;
    image=GetNextImage(image);
    ProgressMonitor(SaveImagesText,scene++,GetNumberScenes(image));
  } while (image_info->adjoin);
  if (image_info->adjoin)
    while (image->previous != (Image *) NULL)
      image=image->previous;
  CloseBlob(image);
  return(True);
}
