/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            RRRR   L      EEEEE                              %
%                            R   R  L      E                                  %
%                            RRRR   L      EEE                                %
%                            R R    L      E                                  %
%                            R  R   LLLLL  EEEEE                              %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s R L E                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsRLE returns True if the image format type, identified by the
%  magick string, is RLE.
%
%  The format of the ReadRLEImage method is:
%
%      unsigned int IsRLE(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsRLE returns True if the image format type is RLE.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsRLE(const unsigned char *magick,const size_t length)
{
  if (length < 2)
    return(False);
  if (memcmp(magick,"\122\314",2) == 0)
    return(True);
  return(False);
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
%  The format of the ReadRLEImage method is:
%
%      Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadRLEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadRLEImage(const ImageInfo *image_info,ExceptionInfo *exception)
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
    y;

  register IndexPacket
    *indexes;

  register int
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    number_pixels;

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
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Determine if this is a RLE file.
  */
  status=ReadBlob(image,2,(char *) magick);
  if ((status == False) || (memcmp(magick,"\122\314",2) != 0))
    ThrowReaderException(CorruptImageWarning,"Not a RLE image file",image);
  do
  {
    /*
      Read image header.
    */
    (void) ReadBlobLSBShort(image);
    (void) ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    flags=ReadBlobByte(image);
    image->matte=flags & 0x04;
    number_planes=ReadBlobByte(image);
    bits_per_pixel=ReadBlobByte(image);
    number_colormaps=ReadBlobByte(image);
    map_length=1 << ReadBlobByte(image);
    if ((number_planes == 0) || (number_planes == 2) || (bits_per_pixel != 8) ||
        (image->columns == 0))
      ThrowReaderException(CorruptImageWarning,"Unsupported RLE image file",
        image);
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < (int) number_planes; i++)
          background_color[i]=0;
        (void) ReadBlobByte(image);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < (int) number_planes; i++)
          *p++=ReadBlobByte(image);
      }
    if ((number_planes & 0x01) == 0)
      (void) ReadBlobByte(image);
    colormap=(unsigned char *) NULL;
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=(unsigned char *) AcquireMemory(number_colormaps*map_length);
        if (colormap == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        p=colormap;
        for (i=0; i < (int) number_colormaps; i++)
          for (x=0; x < (int) map_length; x++)
            *p++=XDownScale(ReadBlobLSBShort(image));
      }
    if (flags & 0x08)
      {
        char
          *comment;

        unsigned int
          length;

        /*
          Read image comment.
        */
        length=ReadBlobLSBShort(image);
        comment=(char *) AcquireMemory(length);
        if (comment == (char *) NULL)
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        (void) ReadBlob(image,length-1,comment);
        comment[length-1]='\0';
        (void) SetImageAttribute(image,"comment",comment);
        LiberateMemory((void **) &comment);
        if ((length & 0x01) == 0)
          (void) ReadBlobByte(image);
      }
    /*
      Allocate RLE pixels.
    */
    if (image->matte)
      number_planes++;
    number_pixels=image->columns*image->rows;
    rle_pixels=(unsigned char *) AcquireMemory(number_pixels*number_planes);
    if (rle_pixels == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    if ((flags & 0x01) && !(flags & 0x02))
      {
        /*
          Set background color.
        */
        p=rle_pixels;
        for (i=0; i < (int) number_pixels; i++)
        {
          if (!image->matte)
            for (x=0; x < (int) number_planes; x++)
              *p++=background_color[x];
          else
            {
              for (x=0; x < (int) (number_planes-1); x++)
                *p++=background_color[x];
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
    opcode=ReadBlobByte(image);
    do
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=ReadBlobByte(image);
          plane=(unsigned char) operand;
          if (plane == 255)
            plane=number_planes-1;
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          operand++;
          for (i=0; i < operand; i++)
          {
            pixel=ReadBlobByte(image);
            if ((y < (int) image->rows) && ((x+i) < (int) image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) ReadBlobByte(image);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=ReadBlobByte(image);
          if (opcode & 0x40)
            operand=ReadBlobLSBShort(image);
          pixel=ReadBlobByte(image);
          (void) ReadBlobByte(image);
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
      opcode=ReadBlobByte(image);
    } while (((opcode & 0x3f) != EOFOp) && (opcode != EOF));
    if (number_colormaps != 0)
      {
        unsigned int
          mask;

        /*
          Apply colormap affineation to image.
        */
        mask=(map_length-1);
        p=rle_pixels;
        if (number_colormaps == 1)
          for (i=0; i < (int) number_pixels; i++)
          {
            *p=colormap[*p & mask];
            p++;
          }
        else
          if ((number_planes >= 3) && (number_colormaps >= 3))
            for (i=0; i < (int) number_pixels; i++)
              for (x=0; x < (int) number_planes; x++)
              {
                *p=colormap[x*map_length+(*p & mask)];
                p++;
              }
      }
    /*
      Initialize image structure.
    */
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass pixel packets.
        */
        p=rle_pixels;
        for (y=0; y < (int) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            q->red=UpScale(*p++);
            q->green=UpScale(*p++);
            q->blue=UpScale(*p++);
            if (image->matte)
              q->opacity=MaxRGB-UpScale(*p++);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              MagickMonitor(LoadImageText,y,image->rows);
        }
      }
    else
      {
        /*
          Create colormap.
        */
        if (number_colormaps == 0)
          map_length=256;
        if (!AllocateImageColormap(image,map_length))
          ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
            image);
        p=colormap;
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
          if (number_colormaps > 1)
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
              Convert raster image to PseudoClass pixel packets.
            */
            for (y=0; y < (int) image->rows; y++)
            {
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              indexes=GetIndexes(image);
              for (x=0; x < (int) image->columns; x++)
                indexes[x]=(*p++);
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,y,image->rows);
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
              q=SetImagePixels(image,0,y,image->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (int) image->columns; x++)
              {
                q->red=image->colormap[*p++].red;
                q->green=image->colormap[*p++].green;
                q->blue=image->colormap[*p++].blue;
                q->opacity=MaxRGB-UpScale(*p++);
                q++;
              }
              if (!SyncImagePixels(image))
                break;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(y,image->rows))
                  MagickMonitor(LoadImageText,y,image->rows);
            }
            LiberateMemory((void **) &image->colormap);
            image->colormap=(PixelPacket *) NULL;
            image->storage_class=DirectClass;
            image->colors=0;
          }
      }
    if (number_colormaps != 0)
      LiberateMemory((void **) &colormap);
    LiberateMemory((void **) &rle_pixels);
    if (EOFBlob(image))
      ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    (void) ReadBlobByte(image);
    status=ReadBlob(image,2,(char *) magick);
    if ((status == True) && (memcmp(magick,"\122\314",2) == 0))
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
  } while ((status == True) && (memcmp(magick,"\122\314",2) == 0));
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
%   R e g i s t e r R L E I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterRLEImage adds attributes for the RLE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterRLEImage method is:
%
%      RegisterRLEImage(void)
%
*/
ModuleExport void RegisterRLEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("RLE");
  entry->decoder=ReadRLEImage;
  entry->magick=IsRLE;
  entry->adjoin=False;
  entry->description=AllocateString("Utah Run length encoded image");
  entry->module=AllocateString("RLE");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r R L E I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterRLEImage removes format registrations made by the
%  RLE module from the list of supported formats.
%
%  The format of the UnregisterRLEImage method is:
%
%      UnregisterRLEImage(void)
%
*/
ModuleExport void UnregisterRLEImage(void)
{
  UnregisterMagickInfo("RLE");
}
