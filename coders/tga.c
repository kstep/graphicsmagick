/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            TTTTT   GGGG   AAA                               %
%                              T    G      A   A                              %
%                              T    G  GG  AAAAA                              %
%                              T    G   G  A   A                              %
%                              T     GGG   A   A                              %
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
  WriteTGAImage(const ImageInfo *,Image *);

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
%  The format of the ReadTGAImage method is:
%
%      Image *ReadTGAImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTGAImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadTGAImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define TGAColormap 1
#define TGARGB 2
#define TGAMonochrome 3
#define TGARLEColormap  9
#define TGARLERGB  10
#define TGARLEMonochrome  11

  typedef struct _TGAInfo
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
  } TGAInfo;

  Image
    *image;

  IndexPacket
    index;

  long
    y;

  PixelPacket
    pixel;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  size_t
    count;

  TGAInfo
    tga_info;

  unsigned char
    j,
    k,
    runlength;

  unsigned int
    status;

  unsigned long
    base,
    flag,
    offset,
    real,
    skip;
    
  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read TGA header information.
  */
  count=ReadBlob(image,1,(char *) &tga_info.id_length);
  tga_info.colormap_type=ReadBlobByte(image);
  tga_info.image_type=ReadBlobByte(image);
  do
  {
    if ((count == 0) || (tga_info.image_type == 0) ||
        (tga_info.image_type > 11))
      ThrowReaderException(CorruptImageWarning,"Not a TGA image file",image);
    tga_info.colormap_index=ReadBlobLSBShort(image);
    tga_info.colormap_length=ReadBlobLSBShort(image);
    tga_info.colormap_size=ReadBlobByte(image);
    tga_info.x_origin=ReadBlobLSBShort(image);
    tga_info.y_origin=ReadBlobLSBShort(image);
    tga_info.width=ReadBlobLSBShort(image);
    tga_info.height=ReadBlobLSBShort(image);
    tga_info.bits_per_pixel=ReadBlobByte(image);
    tga_info.attributes=ReadBlobByte(image);
    /*
      Initialize image structure.
    */
    image->matte=tga_info.bits_per_pixel == 32;
    image->columns=tga_info.width;
    image->rows=tga_info.height;
    image->depth=tga_info.bits_per_pixel <= 8 ? 8 : QuantumDepth;
    if (tga_info.colormap_type != 0)
      {
        if ((tga_info.image_type == TGARLEColormap) ||
            (tga_info.image_type == TGARLERGB))
          image->storage_class=PseudoClass;
        image->colors=tga_info.colormap_length;
      }
    if (tga_info.id_length != 0)
      {
        char
          *comment;

        /*
          TGA image comment.
        */
        comment=(char *) AcquireMemory(tga_info.id_length+1);
        if (comment == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) ReadBlob(image,tga_info.id_length,comment);
        comment[tga_info.id_length]='\0';
        (void) SetImageAttribute(image,"comment",comment);
        LiberateMemory((void **) &comment);
      }
    (void) memset(&pixel,0,sizeof(PixelPacket));
    pixel.opacity=TransparentOpacity;
    if (tga_info.colormap_type != 0)
      {
        /*
          Read TGA raster colormap.
        */
        if (!AllocateImageColormap(image,image->colors))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        for (i=0; i < (long) image->colors; i++)
        {
          switch (tga_info.colormap_size)
          {
            case 8:
            default:
            {
              /*
                Gray scale.
              */
              pixel.red=UpScale(ReadBlobByte(image));
              pixel.green=pixel.red;
              pixel.blue=pixel.red;
              break;
            }
            case 15:
            case 16:
            {
              /*
                5 bits each of red green and blue.
              */
              j=ReadBlobByte(image);
              k=ReadBlobByte(image);
              pixel.red=(unsigned long) (MaxRGB*((int) (k & 0x7c) >> 2))/31;
              pixel.green=(unsigned long)
                (MaxRGB*(((int) (k & 0x03) << 3)+((int) (j & 0xe0) >> 5)))/31;
              pixel.blue=(unsigned long) (MaxRGB*((int) (j & 0x1f)))/31;
              break;
            }
            case 24:
            case 32:
            {
              /*
                8 bits each of blue, green and red.
              */
              pixel.blue=UpScale(ReadBlobByte(image));
              pixel.green=UpScale(ReadBlobByte(image));
              pixel.red=UpScale(ReadBlobByte(image));
              break;
            }
          }
          image->colormap[i]=pixel;
        }
      }
    /*
      Convert TGA pixels to pixel packets.
    */
    base=0;
    flag=0;
    skip=False;
    real=0;
    index=0;
    runlength=0;
    offset=0;
    for (y=0; y < (long) image->rows; y++)
    {
      real=offset;
      if (((unsigned char) (tga_info.attributes & 0x20) >> 5) == 0)
        real=image->rows-real-1;
      q=SetImagePixels(image,0,real,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      for (x=0; x < (long) image->columns; x++)
      {
        if ((tga_info.image_type == TGARLEColormap) ||
            (tga_info.image_type == TGARLERGB) ||
            (tga_info.image_type == TGARLEMonochrome))
          {
            if (runlength != 0)
              {
                runlength--;
                skip=flag != 0;
              }
            else
              {
                count=ReadBlob(image,1,(char *) &runlength);
                if (count == 0)
                  ThrowReaderException(CorruptImageWarning,
                    "Unable to read image data",image);
                flag=runlength & 0x80;
                if (flag != 0)
                  runlength-=128;
                skip=False;
              }
          }
        if (!skip)
          switch (tga_info.bits_per_pixel)
          {
            case 8:
            default:
            {
              /*
                Gray scale.
              */
              index=ValidateColormapIndex(image,ReadBlobByte(image));
              if (tga_info.colormap_type != 0)
                pixel=image->colormap[index];
              else
                {
                  pixel.red=(Quantum) UpScale(index);
                  pixel.green=(Quantum) UpScale(index);
                  pixel.blue=(Quantum) UpScale(index);
                }
              break;
            }
            case 15:
            case 16:
            {
              /*
                5 bits each of red green and blue.
              */
              j=ReadBlobByte(image);
              k=ReadBlobByte(image);
              pixel.red=(Quantum) ((MaxRGB*((int) (k & 0x7c) >> 2))/31);
              pixel.green=(Quantum)
                ((MaxRGB*(((int) (k & 0x03) << 3)+((int) (j & 0xe0) >> 5)))/31);
              pixel.blue=(Quantum) ((MaxRGB*((int) (j & 0x1f)))/31);
              index=ValidateColormapIndex(image,((unsigned short) k << 8)+j);
              break;
            }
            case 24:
            case 32:
            {
              /*
                8 bits each of blue green and red.
              */
              pixel.blue=UpScale(ReadBlobByte(image));
              pixel.green=UpScale(ReadBlobByte(image));
              pixel.red=UpScale(ReadBlobByte(image));
              if (tga_info.bits_per_pixel == 32)
                pixel.opacity=MaxRGB-UpScale(ReadBlobByte(image));
              break;
            }
          }
        if (status == False)
          ThrowReaderException(CorruptImageWarning,"Unable to read image data",
            image);
        if (image->storage_class == PseudoClass)
          indexes[x]=index;
        *q++=pixel;
      }
      if (((unsigned char) (tga_info.attributes & 0xc0) >> 6) == 4)
        offset+=4;
      else
        if (((unsigned char) (tga_info.attributes & 0xc0) >> 6) == 2)
          offset+=2;
        else
          offset++;
      if (offset >= image->rows)
        {
          base++;
          offset=base;
        }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
    }
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    count=ReadBlob(image,1,(char *) &tga_info.id_length);
    tga_info.colormap_type=ReadBlobByte(image);
    tga_info.image_type=ReadBlobByte(image);
    status=((tga_info.image_type != 0) && (tga_info.image_type <= 11));
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
        MagickMonitor(LoadImagesText,TellBlob(image),SizeBlob(image));
      }
  } while (status == True);
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
%   R e g i s t e r T G A I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTGAImage adds attributes for the TGA image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTGAImage method is:
%
%      RegisterTGAImage(void)
%
*/
ModuleExport void RegisterTGAImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ICB");
  entry->decoder=ReadTGAImage;
  entry->encoder=WriteTGAImage;
  entry->description=AllocateString("Truevision Targa image");
  entry->module=AllocateString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("TGA");
  entry->decoder=ReadTGAImage;
  entry->encoder=WriteTGAImage;
  entry->description=AllocateString("Truevision Targa image");
  entry->module=AllocateString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("VDA");
  entry->decoder=ReadTGAImage;
  entry->encoder=WriteTGAImage;
  entry->description=AllocateString("Truevision Targa image");
  entry->module=AllocateString("TGA");
  (void) RegisterMagickInfo(entry);
  entry=SetMagickInfo("VST");
  entry->decoder=ReadTGAImage;
  entry->encoder=WriteTGAImage;
  entry->description=AllocateString("Truevision Targa image");
  entry->module=AllocateString("TGA");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T G A I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTGAImage removes format registrations made by the
%  TGA module from the list of supported formats.
%
%  The format of the UnregisterTGAImage method is:
%
%      UnregisterTGAImage(void)
%
*/
ModuleExport void UnregisterTGAImage(void)
{
  (void) UnregisterMagickInfo("ICB");
  (void) UnregisterMagickInfo("TGA");
  (void) UnregisterMagickInfo("VDA");
  (void) UnregisterMagickInfo("VST");
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
%  The format of the WriteTGAImage method is:
%
%      unsigned int WriteTGAImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WriteTGAImage(const ImageInfo *image_info,Image *image)
{
#define TargaColormap 1
#define TargaRGB 2
#define TargaMonochrome 3
#define TargaRLEColormap  9
#define TargaRLERGB  10
#define TargaRLEMonochrome  11

  typedef struct _TargaInfo
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
  } TargaInfo;

  const ImageAttribute
    *attribute;

  long
    count,
    y;

  register const PixelPacket
    *p;

  register IndexPacket
    *indexes;

  register long
    x;

  register long
    i;

  register unsigned char
    *q;

  TargaInfo
    targa_info;

  unsigned char
    *targa_pixels;

  unsigned int
    scene,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  scene=0;
  do
  {
    /*
      Initialize TGA raster file header.
    */
    (void) TransformRGBImage(image,RGBColorspace);
    targa_info.id_length=0;
    attribute=GetImageAttribute(image,"comment");
    if (attribute != (const ImageAttribute *) NULL)
      targa_info.id_length=Min(strlen(attribute->value),255);
    targa_info.colormap_type=0;
    targa_info.colormap_index=0;
    targa_info.colormap_length=0;
    targa_info.colormap_size=0;
    targa_info.x_origin=0;
    targa_info.y_origin=0;
    targa_info.width=image->columns;
    targa_info.height=image->rows;
    targa_info.bits_per_pixel=8;
    targa_info.attributes=0;
    if ((image->storage_class == DirectClass) || (image->colors > 256))
      {
        /*
          Full color TGA raster.
        */
        targa_info.image_type=TargaRGB;
        targa_info.bits_per_pixel=image->matte ? 32 : 24;
      }
    else
      {
        /*
          Colormapped TGA raster.
        */
        targa_info.image_type=TargaColormap;
        targa_info.colormap_type=1;
        targa_info.colormap_index=0;
        targa_info.colormap_length=image->colors;
        targa_info.colormap_size=24;
      }
    /*
      Write TGA header.
    */
    (void) WriteBlobByte(image,targa_info.id_length);
    (void) WriteBlobByte(image,targa_info.colormap_type);
    (void) WriteBlobByte(image,targa_info.image_type);
    (void) WriteBlobLSBShort(image,targa_info.colormap_index);
    (void) WriteBlobLSBShort(image,targa_info.colormap_length);
    (void) WriteBlobByte(image,targa_info.colormap_size);
    (void) WriteBlobLSBShort(image,targa_info.x_origin);
    (void) WriteBlobLSBShort(image,targa_info.y_origin);
    (void) WriteBlobLSBShort(image,targa_info.width);
    (void) WriteBlobLSBShort(image,targa_info.height);
    (void) WriteBlobByte(image,targa_info.bits_per_pixel);
    (void) WriteBlobByte(image,targa_info.attributes);
    if (targa_info.id_length != 0)
      (void) WriteBlob(image,targa_info.id_length,attribute->value);
    if (targa_info.image_type == TargaColormap)
      {
        unsigned char
          *targa_colormap;

        /*
          Dump colormap to file (blue, green, red byte order).
        */
        targa_colormap=(unsigned char *)
          AcquireMemory(3*targa_info.colormap_length);
        if (targa_colormap == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
            image);
        q=targa_colormap;
        for (i=0; i < (long) image->colors; i++)
        {
          *q++=DownScale(image->colormap[i].blue);
          *q++=DownScale(image->colormap[i].green);
          *q++=DownScale(image->colormap[i].red);
        }
        (void) WriteBlob(image,3*targa_info.colormap_length,
          (char *) targa_colormap);
        LiberateMemory((void **) &targa_colormap);
      }
    /*
      Convert MIFF to TGA raster pixels.
    */
    count=(unsigned int) (targa_info.bits_per_pixel*targa_info.width) >> 3;
    targa_pixels=(unsigned char *) AcquireMemory(count);
    if (targa_pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
        image);
    for (y=(long) (image->rows-1); y >= 0; y--)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      q=targa_pixels;
      indexes=GetIndexes(image);
      for (x=0; x < (long) image->columns; x++)
      {
        if (targa_info.image_type == TargaColormap)
          *q++=indexes[x];
        else
          {
            *q++=DownScale(p->blue);
            *q++=DownScale(p->green);
            *q++=DownScale(p->red);
            if (image->colorspace == CMYKColorspace)
              *q++=DownScale(p->opacity);
            else
              if (image->matte)
                *q++=MaxRGB-DownScale(p->opacity);
          }
        p++;
      }
      (void) WriteBlob(image,q-targa_pixels,(char *) targa_pixels);
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          MagickMonitor(SaveImageText,y,image->rows);
    }
    LiberateMemory((void **) &targa_pixels);
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
