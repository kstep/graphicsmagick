/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  W   W  DDDD                               %
%                             X X   W   W  D   D                              %
%                              X    W   W  D   D                              %
%                             X X   W W W  D   D                              %
%                            X   X   W W   DDDD                               %
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
  WriteXWDImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s X W D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsXWD returns True if the image format type, identified by the
%  magick string, is XWD.
%
%  The format of the IsXWD method is:
%
%      unsigned int IsXWD(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsXWD returns True if the image format type is XWD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsXWD(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(False);
  if (memcmp(magick+1,"\000\000",2) == 0)
    {
      if (memcmp(magick+4,"\007\000\000",3) == 0)
        return(True);
      if (memcmp(magick+5,"\000\000\007",3) == 0)
        return(True);
    }
  return(False);
}

#if defined(HasX11)
#include "xwindow.h"
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
%  The format of the ReadXWDImage method is:
%
%      Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXWDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *comment;

  Image
    *image;

  IndexPacket
    index;

  int
    status,
    y;

  register IndexPacket
    *indexes;

  register int
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned long
    pixel;

  size_t
    count,
    length;

  unsigned long
    lsb_first;

  XColor
    *colors;

  XImage
    *ximage;

  XWDFileHeader
    header;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
     Read in header information.
  */
  count=ReadBlob(image,sz_XWDheader,(char *) &header);
  if (count == 0)
    ThrowReaderException(CorruptImageWarning,"Unable to read dump file header",
      image);
  image->columns=header.pixmap_width;
  image->rows=header.pixmap_height;
  image->depth=8;
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBOrderLong((char *) &header,sz_XWDheader);
  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    ThrowReaderException(CorruptImageWarning,"XWD file format version mismatch",
      image);
  if (header.header_size < sz_XWDheader)
    ThrowReaderException(CorruptImageWarning,"XWD header size is too small",
      image);
  length=header.header_size-sz_XWDheader;
  comment=(char *) AcquireMemory(length+1);
  if (comment == (char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  count=ReadBlob(image,length,comment);
  comment[length]='\0';
  (void) SetImageAttribute(image,"comment",comment);
  LiberateMemory((void **) &comment);
  if (count == 0)
    ThrowReaderException(CorruptImageWarning,
      "Unable to read window name from dump file",image);
  /*
    Initialize the X image.
  */
  ximage=(XImage *) AcquireMemory(sizeof(XImage));
  if (ximage == (XImage *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
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
    ThrowReaderException(CorruptImageWarning,"Invalid XWD header",image);
  /*
    Read colormap.
  */
  colors=(XColor *) NULL;
  if (header.ncolors != 0)
    {
      XWDColor
        color;

      colors=(XColor *)
        AcquireMemory((unsigned int) header.ncolors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      for (i=0; i < (long) header.ncolors; i++)
      {
        count=ReadBlob(image,sz_XWDColor,(char *) &color);
        if (count == 0)
          ThrowReaderException(CorruptImageWarning,
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
        for (i=0; i < (long) header.ncolors; i++)
        {
          MSBOrderLong((char *) &colors[i].pixel,sizeof(unsigned long));
          MSBOrderShort((char *) &colors[i].red,3*sizeof(unsigned short));
        }
    }
  /*
    Allocate the pixel buffer.
  */
  if (ximage->format == ZPixmap)
    length=ximage->bytes_per_line*ximage->height;
  else
    length=ximage->bytes_per_line*ximage->height*ximage->depth;
  ximage->data=(char *) AcquireMemory(length);
  if (ximage->data == (char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  count=ReadBlob(image,length,ximage->data);
  if (count == 0)
    ThrowReaderException(CorruptImageWarning,"Unable to read dump pixmap",
      image);
  /*
    Convert image to MIFF format.
  */
  image->columns=ximage->width;
  image->rows=ximage->height;
  if ((colors == (XColor *) NULL) || (ximage->red_mask != 0) ||
      (ximage->green_mask != 0) || (ximage->blue_mask != 0))
    image->storage_class=DirectClass;
  else
    image->storage_class=PseudoClass;
  image->colors=header.ncolors;
  switch (image->storage_class)
  {
    case DirectClass:
    default:
    {
      register unsigned long
        color;

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
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          for (x=0; x < (int) image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            index=(unsigned short) ((pixel >> red_shift) & red_mask);
            q->red=XDownScale(colors[index].red);
            index=(unsigned short) ((pixel >> green_shift) & green_mask);
            q->green=XDownScale(colors[index].green);
            index=(unsigned short) ((pixel >> blue_shift) & blue_mask);
            q->blue=XDownScale(colors[index].blue);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
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
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            q->red=XDownScale((color*65535L)/red_mask);
            color=(pixel >> green_shift) & green_mask;
            q->green=XDownScale((color*65535L)/green_mask);
            color=(pixel >> blue_shift) & blue_mask;
            q->blue=XDownScale((color*65535L)/blue_mask);
            q++;
          }
          if (!SyncImagePixels(image))
            break;
          if (QuantumTick(y,image->rows))
            MagickMonitor(LoadImageText,y,image->rows);
        }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert X image to PseudoClass packets.
      */
      if (!AllocateImageColormap(image,image->colors))
        ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
          image);
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=XDownScale(colors[i].red);
        image->colormap[i].green=XDownScale(colors[i].green);
        image->colormap[i].blue=XDownScale(colors[i].blue);
      }
      for (y=0; y < (int) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (int) image->columns; x++)
        {
          index=ValidateColormapIndex(image,
            (unsigned int) XGetPixel(ximage,x,y));
          indexes[x]=index;
          *q++=image->colormap[index];
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
  }
  /*
    Free image and colormap.
  */
  if (header.ncolors != 0)
    LiberateMemory((void **) &colors);
  LiberateMemory((void **) &ximage->data);
  LiberateMemory((void **) &ximage);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,"Unexpected end-of-file",image);
  CloseBlob(image);
  return(image);
}
#else
static Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  ThrowException(exception,MissingDelegateWarning,
    "X11 library is not available",image_info->filename);
  return((Image *) NULL);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X W D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXWDImage adds attributes for the XWD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXWDImage method is:
%
%      RegisterXWDImage(void)
%
*/
ModuleExport void RegisterXWDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("XWD");
  entry->decoder=ReadXWDImage;
  entry->encoder=WriteXWDImage;
  entry->magick=IsXWD;
  entry->adjoin=False;
  entry->description=AllocateString("X Windows system window dump (color)");
  entry->module=AllocateString("XWD");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X W D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXWDImage removes format registrations made by the
%  XWD module from the list of supported formats.
%
%  The format of the UnregisterXWDImage method is:
%
%      UnregisterXWDImage(void)
%
*/
ModuleExport void UnregisterXWDImage(void)
{
#if defined(HasX11)
  UnregisterMagickInfo("XWD");
#endif
}

#if defined(HasX11)
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
%  The format of the WriteXWDImage method is:
%
%      unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
{
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

  register unsigned char
    *q;

  unsigned char
    *pixels;

  unsigned int
    bits_per_pixel,
    bytes_per_line,
    scanline_pad,
    status;

  unsigned long
    lsb_first;

  XWDFileHeader
    xwd_info;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  TransformRGBImage(image,RGBColorspace);
  /*
    Initialize XWD file header.
  */
  xwd_info.header_size=(CARD32) (sz_XWDheader+strlen(image->filename)+1);
  xwd_info.file_version=(CARD32) XWD_FILE_VERSION;
  xwd_info.pixmap_format=(CARD32) ZPixmap;
  xwd_info.pixmap_depth=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.pixmap_width=(CARD32) image->columns;
  xwd_info.pixmap_height=(CARD32) image->rows;
  xwd_info.xoffset=(CARD32) 0;
  xwd_info.byte_order=(CARD32) MSBFirst;
  xwd_info.bitmap_unit=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  xwd_info.bitmap_bit_order=(CARD32) MSBFirst;
  xwd_info.bitmap_pad=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  bits_per_pixel=(image->storage_class == DirectClass ? 24 : 8);
  xwd_info.bits_per_pixel=(CARD32) bits_per_pixel;
  bytes_per_line=(CARD32) ((((xwd_info.bits_per_pixel*
    xwd_info.pixmap_width)+((xwd_info.bitmap_pad)-1))/
    (xwd_info.bitmap_pad))*((xwd_info.bitmap_pad) >> 3));
  xwd_info.bytes_per_line=(CARD32) bytes_per_line;
  xwd_info.visual_class=(CARD32)
    (image->storage_class == DirectClass ? DirectColor : PseudoColor);
  xwd_info.red_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff0000 : 0);
  xwd_info.green_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff00 : 0);
  xwd_info.blue_mask=(CARD32) (image->storage_class == DirectClass ? 0xff : 0);
  xwd_info.bits_per_rgb=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.colormap_entries=(CARD32)
    (image->storage_class == DirectClass ? 256 : image->colors);
  xwd_info.ncolors=(unsigned int)
    (image->storage_class == DirectClass ? 0 : image->colors);
  xwd_info.window_width=(CARD32) image->columns;
  xwd_info.window_height=(CARD32) image->rows;
  xwd_info.window_x=0;
  xwd_info.window_y=0;
  xwd_info.window_bdrwidth=(CARD32) 0;
  /*
    Write XWD header.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBOrderLong((char *) &xwd_info,sizeof(xwd_info));
  (void) WriteBlob(image,sz_XWDheader,(char *) &xwd_info);
  (void) WriteBlob(image,strlen(image->filename)+1,(char *) image->filename);
  if (image->storage_class == PseudoClass)
    {
      XColor
        *colors;

      XWDColor
        color;

      /*
        Dump colormap to file.
      */
      colors=(XColor *) AcquireMemory(image->colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",
          image);
      for (i=0; i < image->colors; i++)
      {
        colors[i].pixel=i;
        colors[i].red=XUpScale(image->colormap[i].red);
        colors[i].green=XUpScale(image->colormap[i].green);
        colors[i].blue=XUpScale(image->colormap[i].blue);
        colors[i].flags=DoRed | DoGreen | DoBlue;
        colors[i].pad=0;
        if (*(char *) &lsb_first)
          {
            MSBOrderLong((char *) &colors[i].pixel,sizeof(long));
            MSBOrderShort((char *) &colors[i].red,3*sizeof(short));
          }
      }
      for (i=0; i < image->colors; i++)
      {
        color.pixel=(CARD32) colors[i].pixel;
        color.red=colors[i].red;
        color.green=colors[i].green;
        color.blue=colors[i].blue;
        color.flags=colors[i].flags;
        (void) WriteBlob(image,sz_XWDColor,(char *) &color);
      }
      LiberateMemory((void **) &colors);
    }
  /*
    Allocate memory for pixels.
  */
  pixels=(unsigned char *)
    AcquireMemory(image->columns*sizeof(PixelPacket));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Convert MIFF to XWD raster pixels.
  */
  scanline_pad=(unsigned int)
    (bytes_per_line-((image->columns*bits_per_pixel) >> 3));
  for (y=0; y < (int) image->rows; y++)
  {
    p=GetImagePixels(image,0,y,image->columns,1);
    if (p == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    q=pixels;
    for (x=0; x < (int) image->columns; x++)
    {
      if (image->storage_class == PseudoClass)
        *q++=indexes[x];
      else
        {
          *q++=DownScale(p->red);
          *q++=DownScale(p->green);
          *q++=DownScale(p->blue);
        }
      p++;
    }
    for (x=0; x < (int) scanline_pad; x++)
      *q++=0;
    (void) WriteBlob(image,q-pixels,(char *) pixels);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        MagickMonitor(SaveImageText,y,image->rows);
  }
  LiberateMemory((void **) &pixels);
  CloseBlob(image);
  return(True);
}
#else
static unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
{
  ThrowBinaryException(MissingDelegateWarning,"X11 library is not available",
    image->filename);
}
#endif
