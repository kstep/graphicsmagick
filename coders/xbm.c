/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  BBBB   M   M                              %
%                             X X   B   B  MM MM                              %
%                              X    BBBB   M M M                              %
%                             X X   B   B  M   M                              %
%                            X   X  BBBB   M   M                              %
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
  WriteXBMImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s X B M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsXBM returns True if the image format type, identified by the
%  magick string, is XBM.
%
%  The format of the IsXBM method is:
%
%      unsigned int IsXBM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsXBM returns True if the image format type is XBM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsXBM(const unsigned char *magick,const size_t length)
{
  if (length < 7)
    return(False);
  if (LocaleNCompare((char *) magick,"#define",7) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X B M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXBMImage reads an X11 bitmap image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadXBMImage method is:
%
%      Image *ReadXBMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXBMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/

static int XBMInteger(Image *image,short int *hex_digits)
{
  int
    c,
    flag,
    value;

  value=0;
  flag=0;
  for ( ; ; )
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      {
        value=(-1);
        break;
      }
    c&=0xff;
    if (isxdigit(c))
      {
        value=(value << 4)+hex_digits[c];
        flag++;
        continue;
      }
    if ((hex_digits[c]) < 0 && flag)
      break;
  }
  return(value);
}

static Image *ReadXBMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    buffer[MaxTextExtent],
    name[MaxTextExtent];

  Image
    *image;

  int
    bit;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  short int
    hex_digits[256];

  unsigned char
    *data;

  unsigned int
    byte,
    padding,
    status,
    value,
    version;

  unsigned long
    bytes_per_line;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  /*
    Read X bitmap header.
  */
  while (ReadBlobString(image,buffer) != (char *) NULL)
    if (sscanf(buffer,"#define %s %lu",name,&image->columns) == 2)
      if ((strlen(name) >= 6) &&
          (LocaleCompare(name+strlen(name)-6,"_width") == 0))
          break;
  while (ReadBlobString(image,buffer) != (char *) NULL)
    if (sscanf(buffer,"#define %s %lu",name,&image->rows) == 2)
      if ((strlen(name) >= 7) &&
          (LocaleCompare(name+strlen(name)-7,"_height") == 0))
          break;
  image->depth=8;
  image->storage_class=PseudoClass;
  image->colors=2;
  /*
    Scan until hex digits.
  */
  version=11;
  while (ReadBlobString(image,buffer) != (char *) NULL)
  {
    if (sscanf(buffer,"static short %s = {",name) == 1)
      version=10;
    else
      if (sscanf(buffer,"static unsigned char %s = {",name) == 1)
        version=11;
      else
        if (sscanf(buffer,"static char %s = {",name) == 1)
          version=11;
        else
          continue;
    p=(unsigned char *) strrchr(name,'_');
    if (p == (unsigned char *) NULL)
      p=(unsigned char *) name;
    else
      p++;
    if (LocaleCompare("bits[]",(char *) p) == 0)
      break;
  }
  if ((image->columns == 0) || (image->rows == 0) || EOFBlob(image))
    ThrowReaderException(CorruptImageWarning,
      "XBM file is not in the correct format",image);
  /*
    Initialize image structure.
  */
  if (!AllocateImageColormap(image,image->colors))
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  padding=0;
  if ((image->columns % 16) && ((image->columns % 16) < 9)  && (version == 10))
    padding=1;
  bytes_per_line=(image->columns+7)/8+padding;
  data=(unsigned char *) AcquireMemory(bytes_per_line*image->rows);
  if (data == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  /*
    Initialize colormap.
  */
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=MaxRGB;
  image->colormap[1].green=MaxRGB;
  image->colormap[1].blue=MaxRGB;
  /*
    Initialize hex values.
  */
  hex_digits['0']=0;
  hex_digits['1']=1;
  hex_digits['2']=2;
  hex_digits['3']=3;
  hex_digits['4']=4;
  hex_digits['5']=5;
  hex_digits['6']=6;
  hex_digits['7']=7;
  hex_digits['8']=8;
  hex_digits['9']=9;
  hex_digits['A']=10;
  hex_digits['B']=11;
  hex_digits['C']=12;
  hex_digits['D']=13;
  hex_digits['E']=14;
  hex_digits['F']=15;
  hex_digits['a']=10;
  hex_digits['b']=11;
  hex_digits['c']=12;
  hex_digits['d']=13;
  hex_digits['e']=14;
  hex_digits['f']=15;
  hex_digits['x']=0;
  hex_digits[' ']=(-1);
  hex_digits[',']=(-1);
  hex_digits['}']=(-1);
  hex_digits['\n']=(-1);
  hex_digits['\t']=(-1);
  /*
    Read hex image data.
  */
  p=data;
  if (version == 10)
    for (i=0; i < (long) (bytes_per_line*image->rows); (i+=2))
    {
      value=XBMInteger(image,hex_digits);
      *p++=value;
      if (!padding || ((i+2) % bytes_per_line))
        *p++=value >> 8;
    }
  else
    for (i=0; i < (long) (bytes_per_line*image->rows); i++)
    {
      value=XBMInteger(image,hex_digits);
      *p++=value;
    }
  /*
    Convert X bitmap image to pixel packets.
  */
  p=data;
  for (y=0; y < (long) image->rows; y++)
  {
    q=SetImagePixels(image,0,y,image->columns,1);
    if (q == (PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    bit=0;
    byte=0;
    for (x=0; x < (long) image->columns; x++)
    {
      if (bit == 0)
        byte=(*p++);
      indexes[x]=byte & 0x01 ? 0 : 1;
      bit++;
      byte>>=1;
      if (bit == 8)
        bit=0;
    }
    if (!SyncImagePixels(image))
      break;
    if (QuantumTick(y,image->rows))
      MagickMonitor(LoadImageText,y,image->rows);
  }
  LiberateMemory((void **) &data);
  SyncImage(image);
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X B M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXBMImage adds attributes for the XBM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXBMImage method is:
%
%      RegisterXBMImage(void)
%
*/
ModuleExport void RegisterXBMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("XBM");
  entry->decoder=ReadXBMImage;
  entry->encoder=WriteXBMImage;
  entry->magick=IsXBM;
  entry->adjoin=False;
  entry->description=
    AllocateString("X Windows system bitmap (black and white)");
  entry->module=AllocateString("XBM");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X B M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXBMImage removes format registrations made by the
%  XBM module from the list of supported formats.
%
%  The format of the UnregisterXBMImage method is:
%
%      UnregisterXBMImage(void)
%
*/
ModuleExport void UnregisterXBMImage(void)
{
  (void) UnregisterMagickInfo("XBM");
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
%  The format of the WriteXBMImage method is:
%
%      unsigned int WriteXBMImage(const ImageInfo *image_info,Image *image)
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
static unsigned int WriteXBMImage(const ImageInfo *image_info,Image *image)
{
  char
    basename[MaxTextExtent],
    buffer[MaxTextExtent];

  int
    y;

  register const PixelPacket
    *p;

  register int
    x;

  register IndexPacket
    *indexes;

  unsigned char
    bit,
    byte,
    polarity;

  unsigned int
    count,
    status;

  /*
    Open output image file.
  */
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Write X bitmap header.
  */
  GetPathComponent(image->filename,BasePath,basename);
  FormatString(buffer,"#define %.1024s_width %lu\n",basename,image->columns);
  (void) WriteBlob(image,strlen(buffer),buffer);
  FormatString(buffer,"#define %.1024s_height %lu\n",basename,image->rows);
  (void) WriteBlob(image,strlen(buffer),buffer);
  FormatString(buffer,"static char %.1024s_bits[] = {\n",basename);
  (void) WriteBlob(image,strlen(buffer),buffer);
  (void) strcpy(buffer," ");
  (void) WriteBlob(image,strlen(buffer),buffer);
  /*
    Convert MIFF to X bitmap pixels.
  */
  if ((image->storage_class == DirectClass) ||
      !IsMonochromeImage(image,&image->exception))
    SetImageType(image,BilevelType);
  polarity=Intensity(image->colormap[0]) > (0.5*MaxRGB);
  if (image->colors == 2)
    polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
  bit=0;
  byte=0;
  count=0;
  x=0;
  y=0;
  (void) strcpy(buffer," ");
  (void) WriteBlob(image,strlen(buffer),buffer);
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    indexes=GetIndexes(image);
    for (x=0; x < (long) image->columns; x++)
    {
      byte>>=1;
      if (indexes[x] == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
          FormatString(buffer,"0x%02x, ",(unsigned int) (byte & 0xff));
          (void) WriteBlob(image,strlen(buffer),buffer);
          count++;
          if (count == 12)
            {
              (void) strcpy(buffer,"\n  ");
              (void) WriteBlob(image,strlen(buffer),buffer);
              count=0;
            };
          bit=0;
          byte=0;
        }
        p++;
      }
    if (bit != 0)
      {
        /*
          Write a bitmap byte to the image file.
        */
        byte>>=(8-bit);
        FormatString(buffer,"0x%02x, ",(unsigned int) (byte & 0xff));
        (void) WriteBlob(image,strlen(buffer),buffer);
        count++;
        if (count == 12)
          {
            (void) strcpy(buffer,"\n  ");
            (void) WriteBlob(image,strlen(buffer),buffer);
            count=0;
          };
        bit=0;
        byte=0;
      };
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  (void) strcpy(buffer,"};\n");
  (void) WriteBlob(image,strlen(buffer),buffer);
  CloseBlob(image);
  return(True);
}
