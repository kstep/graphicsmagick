/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   SSSSS  DDDD                               %
%                            P   P  SS     D   D                              %
%                            PPPP    SSS   D   D                              %
%                            P         SS  D   D                              %
%                            P      SSSSS  DDDD                               %
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
  WritePSDImage(const ImageInfo *,Image *);

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
%  Method DecodeImage uncompresses an image via Macintosh encoding specific to
%  the Adobe Photoshop image format.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,const long channel)
%
%  A description of each parameter follows:
%
%    o status: Method DecodeImage return True if the image is
%      decoded.  False is returned if there is an error occurs.
%
%    o image,image: The address of a structure of type Image.
%
%    o channel:  Specifies which channel: red, green, blue, or index to
%      decode the pixel values into.
%
%
*/
static unsigned int DecodeImage(Image *image,const long channel)
{
  int
    count;

  long
    number_pixels;

  Quantum
    pixel;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  x=0;
  number_pixels=image->columns*image->rows;
  while (number_pixels > 0)
  {
    count=ReadBlobByte(image);
    if (count >= 128)
      count-=256;
    if (count < 0)
      {
        if (count == -128)
          continue;
        pixel=ReadBlobByte(image);
        for (count=(-count+1); count > 0; count--)
        {
          q=GetImagePixels(image,x % image->columns,x/image->columns,1,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          switch (channel)
          {
            case -1:
            {
              q->opacity=MaxRGB-UpScale(pixel);
              break;
            }
            case 0:
            {
              q->red=UpScale(pixel);
              if (image->storage_class == PseudoClass)
                {
                  *indexes=pixel;
                  *q=image->colormap[pixel];
                }
              break;
            }
            case 1:
            {
              if (image->storage_class == PseudoClass)
                q->opacity=UpScale(pixel);
              else
                q->green=UpScale(pixel);
              break;
            }
            case 2:
            {
              q->blue=UpScale(pixel);
              break;
            }
            case 3:
            {
              q->opacity=UpScale(pixel);
              break;
            }
            case 4:
            {
              if (image->colorspace == CMYKColorspace)
                *indexes=UpScale(pixel);
              break;
            }
            default:
              break;
          }
          if (!SyncImagePixels(image))
            break;
          x++;
          number_pixels--;
        }
        continue;
      }
    count++;
    for (i=count; i > 0; i--)
    {
      pixel=ReadBlobByte(image);
      q=GetImagePixels(image,x % image->columns,x/image->columns,1,1);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetIndexes(image);
      switch (channel)
      {
        case -1:
        {
          q->opacity=MaxRGB-UpScale(pixel);
          break;
        }
        case 0:
        {
          q->red=UpScale(pixel);
          if (image->storage_class == PseudoClass)
            {
              *indexes=pixel;
              *q=image->colormap[pixel];
            }
          break;
        }
        case 1:
        {
          if (image->storage_class == PseudoClass)
            q->opacity=UpScale(pixel);
          else
            q->green=UpScale(pixel);
          break;
        }
        case 2:
        {
          q->blue=UpScale(pixel);
          break;
        }
        case 3:
        {
          q->opacity=UpScale(pixel);
          break;
        }
        case 4:
        {
          if (image->colorspace == CMYKColorspace)
            *indexes=UpScale(pixel);
          break;
        }
        default:
          break;
      }
      if (!SyncImagePixels(image))
        break;
      x++;
      number_pixels--;
    }
  }
  /*
    Guarentee the correct number of pixel packets.
  */
  if (number_pixels > 0)
    ThrowBinaryException(CorruptImageWarning,
      "insufficient image data in file",image->filename)
  else
    if (number_pixels < 0)
      ThrowBinaryException(CorruptImageWarning,"too much image data in file",
        image->filename);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P S D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPSD returns True if the image format type, identified by the
%  magick string, is PSD.
%
%  The format of the IsPSD method is:
%
%      unsigned int IsPSD(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPSD returns True if the image format type is PSD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPSD(const unsigned char *magick,const size_t length)
{
  if (length < 4)
    return(False);
  if (LocaleNCompare((char *) magick,"8BPS",4) == 0)
    return(True);
  return(False);
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
%  The format of the ReadPSDImage method is:
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
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPSDImage(const ImageInfo *image_info,ExceptionInfo *exception)
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
    RectangleInfo
      page,
      mask;

    unsigned short
      channels;

    ChannelInfo
      channel_info[24];

    char
      blendkey[4];

    Quantum
      opacity;

    unsigned char
      clipping,
      flags;

    Image
      *image;
  } LayerInfo;

  typedef struct _PSDInfo
  {
    char
      signature[4];

    unsigned short
      channels,
      version;

    unsigned char
      reserved[6];

    unsigned long
      rows,
      columns;

    unsigned short
      depth,
      mode;
  } PSDInfo;

  char
    type[4];

  Image
    *image;

  IndexPacket
    *indexes;

  LayerInfo
    *layer_info;

  long
    j,
    number_layers,
    y;

  PSDInfo
    psd_info;

  Quantum
    pixel;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  size_t
    count,
    length,
    size;

  unsigned char
    *scanline;

  unsigned int
    packet_size,
    status;

  unsigned short
    compression;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read image header.
  */
  count=ReadBlob(image,4,(char *) psd_info.signature);
  psd_info.version=ReadBlobMSBShort(image);
  if ((count == 0) || (LocaleNCompare(psd_info.signature,"8BPS",4) != 0) ||
      (psd_info.version != 1))
    ThrowReaderException(CorruptImageWarning,"Not a PSD image file",image);
  (void) ReadBlob(image,6,(char *) psd_info.reserved);
  psd_info.channels=ReadBlobMSBShort(image);
  psd_info.rows=ReadBlobMSBLong(image);
  psd_info.columns= ReadBlobMSBLong(image);
  psd_info.depth=ReadBlobMSBShort(image);
  psd_info.mode=ReadBlobMSBShort(image);
  /*
    Initialize image.
  */
  SetImage(image,TransparentOpacity);
  image->matte=psd_info.channels >= 4;
  if (psd_info.mode == CMYKMode)
    {
      image->colorspace=CMYKColorspace;
      image->matte=psd_info.channels >= 5;
    }
  image->columns=psd_info.columns;
  image->rows=psd_info.rows;
  image->depth=psd_info.depth <= 8 ? 8 : QuantumDepth;
  if ((psd_info.mode == BitmapMode) || (psd_info.mode == GrayscaleMode))
    {
      if (!AllocateImageColormap(image,256))
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      image->matte=psd_info.channels >= 2;
    }
  length=ReadBlobMSBLong(image);
  if (length != 0)
    {
      /*
        Read PSD raster colormap.
      */
      if (!AllocateImageColormap(image,length/3))
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      for (i=0; i < (long) image->colors; i++)
        image->colormap[i].red=UpScale(ReadBlobByte(image));
      for (i=0; i < (long) image->colors; i++)
        image->colormap[i].green=UpScale(ReadBlobByte(image));
      for (i=0; i < (long) image->colors; i++)
        image->colormap[i].blue=UpScale(ReadBlobByte(image));
      image->matte=psd_info.channels >= 2;
    }
  length=ReadBlobMSBLong(image);
  if (length != 0)
    {
      unsigned char
        *data;

      /*
        Image resources.
      */
      data=(unsigned char *) AcquireMemory(length);
      if (data == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitWarning,
          "8BIM resource memory allocation failed",image);
      count=ReadBlob(image,length,(char *) data);
      if ((count == 0) || (LocaleNCompare((char *) data,"8BIM",4) != 0))
        ThrowReaderException(CorruptImageWarning,"Not a PSD image file",image);
      image->iptc_profile.info=data;
      image->iptc_profile.length=length;
    }
  layer_info=(LayerInfo *) NULL;
  number_layers=0;
  length=ReadBlobMSBLong(image);
  if (length == 8)
    length=ReadBlobMSBLong(image);
  if (length == 0)
    length=ReadBlobMSBLong(image);
  else
    {
      /*
        Layer and mask block.
      */
      size=ReadBlobMSBLong(image);
      number_layers=ReadBlobMSBShort(image);
      number_layers=AbsoluteValue(number_layers);
      layer_info=(LayerInfo *) AcquireMemory(number_layers*sizeof(LayerInfo));
      if (layer_info == (LayerInfo *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      (void) memset(layer_info,0,number_layers*sizeof(LayerInfo));
      for (i=0; i < number_layers; i++)
      {
        layer_info[i].page.y=ReadBlobMSBLong(image);
        layer_info[i].page.x=ReadBlobMSBLong(image);
        layer_info[i].page.height=(ReadBlobMSBLong(image)-layer_info[i].page.y);
        layer_info[i].page.width=(ReadBlobMSBLong(image)-layer_info[i].page.x);
        layer_info[i].channels=ReadBlobMSBShort(image);
        for (j=0; j < layer_info[i].channels; j++)
        {
          layer_info[i].channel_info[j].type=ReadBlobMSBShort(image);
          layer_info[i].channel_info[j].size=ReadBlobMSBLong(image);
        }
        count=ReadBlob(image,4,(char *) type);
        if ((count == 0) || (LocaleNCompare(type,"8BIM",4) != 0))
          ThrowReaderException(CorruptImageWarning,"Not a PSD image file",
            image);
        (void) ReadBlob(image,4,(char *) layer_info[i].blendkey);
        layer_info[i].opacity=MaxRGB-UpScale(ReadBlobByte(image));
        layer_info[i].clipping=ReadBlobByte(image);
        layer_info[i].flags=ReadBlobByte(image);
        (void) ReadBlobByte(image);  /* filler */
        size=ReadBlobMSBLong(image);
        if (size != 0)
          {
            length=ReadBlobMSBLong(image);
            if (length != 0)
              {
                /*
                  Layer mask info.
                */
                layer_info[i].mask.y=ReadBlobMSBLong(image);
                layer_info[i].mask.x=ReadBlobMSBLong(image);
                layer_info[i].mask.height=
                  (ReadBlobMSBLong(image)-layer_info[i].mask.y);
                layer_info[i].mask.width=
                  (ReadBlobMSBLong(image)-layer_info[i].mask.x);
                for (j=0; j < (long) (length-16); j++)
                  (void) ReadBlobByte(image);
              }
            for (j=0; j < (long) (size-length-4); j++)
              (void) ReadBlobByte(image);
          }
        /*
          Allocate layered image.
        */
        layer_info[i].image=CloneImage(image,layer_info[i].page.width,
          layer_info[i].page.height,True,&image->exception);
        if (layer_info[i].image == (Image *) NULL)
          {
            for (j=0; j < i; j++)
              DestroyImage(layer_info[j].image);
            ThrowReaderException(ResourceLimitWarning,
              "Memory allocation failed",image)
          }
        SetImage(layer_info[i].image,TransparentOpacity);
        if (psd_info.mode == CMYKMode)
          layer_info[i].image->colorspace=CMYKColorspace;
        for (j=0; j < layer_info[i].channels; j++)
          if (layer_info[i].channel_info[j].type == -1)
            layer_info[i].image->matte=True;
      }
      /*
        Read pixel data for each layer.
      */
      for (i=0; i < number_layers; i++)
      {
        *layer_info[i].image->blob=(*image->blob);
        layer_info[i].image->file=image->file;
        for (j=0; j < layer_info[i].channels; j++)
        {
          if (layer_info[i].channel_info[j].size < 8)
            {
              long
                k;

              /*
                A layer without data.
              */
              SetImage(layer_info[i].image,OpaqueOpacity);
              for (k=0; k < (long) layer_info[i].channel_info[j].size; k++)
                (void) ReadBlobByte(layer_info[i].image);
              continue;
            }
          compression=ReadBlobMSBShort(layer_info[i].image);
          if (compression == 1)
            {
              /*
                Read RLE compressed data.
              */
              for (y=0; y < (long) layer_info[i].image->rows; y++)
                (void) ReadBlobMSBShort(layer_info[i].image);
              (void) DecodeImage(layer_info[i].image,
                layer_info[i].channel_info[j].type);
              continue;
            }
          /*
            Read uncompressed pixel data as separate planes.
          */
          packet_size=1;
          if (layer_info[i].image->storage_class == PseudoClass)
            {
              if (layer_info[i].image->colors > 256)
                packet_size++;
            }
          else
            if (layer_info[i].image->depth > 8)
              packet_size++;
          scanline=(unsigned char *)
            AcquireMemory(packet_size*layer_info[i].image->columns+1);
          if (scanline == (unsigned char *) NULL)
            ThrowReaderException(ResourceLimitWarning,
              "Memory allocation failed",image);
          for (y=0; y < (long) layer_info[i].image->rows; y++)
          {
            q=GetImagePixels(layer_info[i].image,0,y,
              layer_info[i].image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            (void) ReadBlob(layer_info[i].image,packet_size*
              layer_info[i].image->columns,(char *) scanline);
            indexes=GetIndexes(layer_info[i].image);
            for (x=0; x < (long) layer_info[i].image->columns; x++)
            {
              pixel=scanline[x];
              switch (layer_info[i].channel_info[j].type)
              {
                case -1:
                {
                  q->opacity=MaxRGB-UpScale(pixel);
                  break;
                }
                case 0:
                {
                  q->red=UpScale(pixel);
                  if (image->storage_class == PseudoClass)
                    {
                      indexes[x]=pixel;
                      *q=image->colormap[pixel];
                    }
                  break;
                }
                case 1:
                {
                  if (image->storage_class == PseudoClass)
                    q->opacity=UpScale(pixel);
                  else
                    q->green=UpScale(pixel);
                  break;
                }
                case 2:
                {
                  q->green=UpScale(pixel);
                  break;
                }
                case 3:
                {
                  q->opacity=UpScale(pixel);
                  break;
                }
                case 4:
                {
                  if (image->colorspace == CMYKColorspace)
                    indexes[x]=UpScale(pixel);
                  break;
                }
                default:
                  break;
              }
              q++;
            }
            if (!SyncImagePixels(layer_info[i].image))
              break;
          }
          LiberateMemory((void **) &scanline);
        }
        *image->blob=(*layer_info[i].image->blob);
        image->file=layer_info[i].image->file;
        if (layer_info[i].opacity != OpaqueOpacity)
          {
            /*
              Correct for opacity level.
            */
            for (y=0; y < (long) layer_info[i].image->rows; y++)
            {
              q=GetImagePixels(layer_info[i].image,0,y,
                layer_info[i].image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < (long) layer_info[i].image->columns; x++)
              {
                q->opacity=((unsigned long)
                  (q->opacity*layer_info[i].opacity)/MaxRGB);
                if (layer_info[i].image->colorspace == CMYKColorspace)
                  indexes[x]=((unsigned long)
                    (indexes[x]*layer_info[i].opacity)/MaxRGB);
                q++;
              }
              if (!SyncImagePixels(layer_info[i].image))
                break;
            }
          }
        if (layer_info[i].image->colorspace == CMYKColorspace)
          {
            /*
              Correct CMYK levels.
            */
            for (y=0; y < (long) layer_info[i].image->rows; y++)
            {
              q=GetImagePixels(layer_info[i].image,0,y,
                layer_info[i].image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) layer_info[i].image->columns; x++)
              {
                q->red=MaxRGB-q->red;
                q->green=MaxRGB-q->green;
                q->blue=MaxRGB-q->blue;
                q->opacity=MaxRGB-q->opacity;
                q++;
              }
              if (!SyncImagePixels(layer_info[i].image))
                break;
            }
          }
        layer_info[i].image->file=(FILE *) NULL;
      }
      for (i=0; i < 4; i++)
        (void) ReadBlobByte(image);
      for (i=0; i < number_layers; i++)
      {
        if (i > 0)
          layer_info[i].image->previous=layer_info[i-1].image;
        if (i < (number_layers-1))
          layer_info[i].image->next=layer_info[i+1].image;
        layer_info[i].image->page=layer_info[i].page;
      }
    }
  if (number_layers > 0)
    {
      DestroyImage(image);
      image=layer_info[0].image;
      LiberateMemory((void **) &layer_info);
      return(image);
    }
  compression=ReadBlobMSBShort(image);
  if (compression == 1)
    {
      /*
        Read Packbit encoded pixel data as separate planes.
      */
      for (i=0; i < (long) (image->rows*psd_info.channels); i++)
        (void) ReadBlobMSBShort(image);
      for (i=0; i < psd_info.channels; i++)
        (void) DecodeImage(image,i);
    }
  else
    {
      /*
        Read uncompressed pixel data as separate planes.
      */
      packet_size=1;
      if (image->storage_class == PseudoClass)
        {
          if (image->colors > 256)
            packet_size++;
        }
      else
        if (image->depth > 8)
          packet_size++;
      scanline=(unsigned char *) AcquireMemory(packet_size*image->columns);
      if (scanline == (unsigned char *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      for (i=0; i < psd_info.channels; i++)
      {
        for (y=0; y < (long) image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          count=ReadBlob(image,packet_size*image->columns,(char *) scanline);
          if ((count == 0) || (q == (PixelPacket *) NULL))
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            pixel=scanline[x];
            switch (image->matte ? i-1 : i)
            {
              case -1:
              {
                q->opacity=MaxRGB-UpScale(pixel);
                break;
              }
              case 0:
              {
                q->red=UpScale(pixel);
                if (image->storage_class == PseudoClass)
                  {
                    indexes[x]=pixel;
                    *q=image->colormap[pixel];
                  }
                break;
              }
              case 1:
              {
                if (image->storage_class == PseudoClass)
                  q->opacity=UpScale(pixel);
                else
                  q->green=UpScale(pixel);
                break;
              }
              case 2:
              {
                q->blue=UpScale(pixel);
                break;
              }
              case 3:
              {
                q->opacity=UpScale(pixel);
                break;
              }
              case 4:
              {
                if (image->colorspace == CMYKColorspace)
                  *indexes=UpScale(pixel);
                break;
              }
              default:
                break;
            }
            q++;
          }
          if (!SyncImagePixels(image))
            break;
        }
      }
      LiberateMemory((void **) &scanline);
    }
  if (image->colorspace == CMYKColorspace)
    {
      /*
        Correct CMYK levels.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=GetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (long) image->columns; x++)
        {
          q->red=MaxRGB-q->red;
          q->green=MaxRGB-q->green;
          q->blue=MaxRGB-q->blue;
          q->opacity=MaxRGB-q->opacity;
          q++;
        }
        if (!SyncImagePixels(image))
          break;
      }
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P S D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPSDImage adds attributes for the PSD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPSDImage method is:
%
%      RegisterPSDImage(void)
%
*/
ModuleExport void RegisterPSDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PSD");
  entry->decoder=ReadPSDImage;
  entry->encoder=WritePSDImage;
  entry->magick=IsPSD;
  entry->adjoin=False;
  entry->description=AllocateString("Adobe Photoshop bitmap");
  entry->module=AllocateString("PSD");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P S D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPSDImage removes format registrations made by the
%  PSD module from the list of supported formats.
%
%  The format of the UnregisterPSDImage method is:
%
%      UnregisterPSDImage(void)
%
*/
ModuleExport void UnregisterPSDImage(void)
{
  (void) UnregisterMagickInfo("PSD");
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
%  The format of the WritePSDImage method is:
%
%      unsigned int WritePSDImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WritePSDImage(const ImageInfo *image_info,Image *image)
{
  int
    y;

  register long
    i;

  unsigned char
    *pixels;

  unsigned int
    packet_size,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  packet_size=image->depth > 8 ? 6 : 3;
  if (image->matte)
    packet_size+=image->depth > 8 ? 2 : 1;
  pixels=(unsigned char *)
    AcquireMemory(packet_size*image->columns*sizeof(PixelPacket));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  (void) WriteBlob(image,4,"8BPS");
  (void) WriteBlobMSBShort(image,1);  /* version */
  (void) WriteBlob(image,6,"      ");  /* reserved */
  if (image->storage_class == PseudoClass)
    (void) WriteBlobMSBShort(image,image->matte ? 2 : 1);
  else
    if (image->colorspace != CMYKColorspace)
      (void) WriteBlobMSBShort(image,image->matte ? 4 : 3);
    else
      (void) WriteBlobMSBShort(image,image->matte ? 5 : 4);
  (void) WriteBlobMSBLong(image,image->rows);
  (void) WriteBlobMSBLong(image,image->columns);
  (void) WriteBlobMSBShort(image,
    (image->storage_class == PseudoClass ? 8 : image->depth));
  if (((image_info->colorspace != UndefinedColorspace) ||
       (image->colorspace != CMYKColorspace)) &&
       (image_info->colorspace != CMYKColorspace))
    {
      (void) TransformRGBImage(image,RGBColorspace);
      (void) WriteBlobMSBShort(image,
        image->storage_class == PseudoClass ? 2 : 3);
    }
  else
    {
      if (image->colorspace != CMYKColorspace)
        (void) RGBTransformImage(image,CMYKColorspace);
      (void) WriteBlobMSBShort(image,4);
    }
  if ((image->storage_class == DirectClass) || (image->colors > 256))
    (void) WriteBlobMSBLong(image,0);
  else
    {
      /*
        Write PSD raster colormap.
      */
      (void) WriteBlobMSBLong(image,768);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,DownScale(image->colormap[i].red));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,DownScale(image->colormap[i].green));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
      for (i=0; i < (long) image->colors; i++)
        (void) WriteBlobByte(image,DownScale(image->colormap[i].blue));
      for ( ; i < 256; i++)
        (void) WriteBlobByte(image,0);
    }
  (void) WriteBlobMSBLong(image,0);  /* image resource block */
  (void) WriteBlobMSBLong(image,0);  /* layer and mask block */
  (void) WriteBlobMSBLong(image,0);
  (void) WriteBlobMSBShort(image,0);  /* compression */
  /*
    Write uncompressed pixel data as separate planes.
  */
  if (image->storage_class == PseudoClass)
    for (y=0; y < (long) image->rows; y++)
    {
      if (!GetImagePixels(image,0,y,image->columns,1))
        break;
      if (!image->matte)
        (void) PopImagePixels(image,IndexQuantum,pixels);
      else
        (void) PopImagePixels(image,IndexAlphaQuantum,pixels);
      (void) WriteBlob(image,image->columns,pixels);
    }
  else
    {
      packet_size=image->depth > 8 ? 2 : 1;
      for (y=0; y < (long) image->rows; y++)
      {
        if (!GetImagePixels(image,0,y,image->columns,1))
          break;
        (void) PopImagePixels(image,RedQuantum,pixels);
        (void) WriteBlob(image,packet_size*image->columns,pixels);
      }
      for (y=0; y < (long) image->rows; y++)
      {
        if (!GetImagePixels(image,0,y,image->columns,1))
          break;
        (void) PopImagePixels(image,GreenQuantum,pixels);
        (void) WriteBlob(image,packet_size*image->columns,pixels);
      }
      for (y=0; y < (long) image->rows; y++)
      {
        if (!GetImagePixels(image,0,y,image->columns,1))
          break;
        (void) PopImagePixels(image,BlueQuantum,pixels);
        (void) WriteBlob(image,packet_size*image->columns,pixels);
      }
      if (image->colorspace == CMYKColorspace)
        for (y=0; y < (long) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,BlackQuantum,pixels);
          (void) WriteBlob(image,packet_size*image->columns,pixels);
        }
      if (image->matte)
        for (y=0; y < (long) image->rows; y++)
        {
          if (!GetImagePixels(image,0,y,image->columns,1))
            break;
          (void) PopImagePixels(image,AlphaQuantum,pixels);
          (void) WriteBlob(image,packet_size*image->columns,pixels);
        }
    }
  LiberateMemory((void **) &pixels);
  CloseBlob(image);
  return(True);
}
