/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           CCCC   OOO   M   M  PPPP   RRRR   EEEEE   SSSSS  SSSSS            %
%          C      O   O  MM MM  P   P  R   R  E       SS     SS               %
%          C      O   O  M M M  PPPP   RRRR   EEE      SSS    SSS             %
%          C      O   O  M   M  P      R R    E          SS     SS            %
%           CCCC   OOO   M   M  P      R  R   EEEEE   SSSSS  SSSSS            %
%                                                                             %
%                                                                             %
%                  Image Compression/Decompression Methods                    %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              May  1993                                      %
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
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "defines.h"
#include "proxy.h"

/*
  Define declarations.
*/
#define LoadImageText  "  Loading image...  "
#define SaveImageText  "  Saving image...  "

/*
  Typedef declarations.
*/
typedef struct HuffmanTable
{
  int
    id,
    code,
    length,
    count;
} HuffmanTable;

typedef struct _ScanlinePacket
{
  unsigned char
    pixel;

  int
    state;
} ScanlinePacket;

/*
  Huffman coding declarations.
*/
#define TWId  23
#define MWId  24
#define TBId  25
#define MBId  26
#define EXId  27

static const HuffmanTable
  MBTable[]=
  {
    { MBId, 0x0f, 10, 64 }, { MBId, 0xc8, 12, 128 },
    { MBId, 0xc9, 12, 192 }, { MBId, 0x5b, 12, 256 },
    { MBId, 0x33, 12, 320 }, { MBId, 0x34, 12, 384 },
    { MBId, 0x35, 12, 448 }, { MBId, 0x6c, 13, 512 },
    { MBId, 0x6d, 13, 576 }, { MBId, 0x4a, 13, 640 },
    { MBId, 0x4b, 13, 704 }, { MBId, 0x4c, 13, 768 },
    { MBId, 0x4d, 13, 832 }, { MBId, 0x72, 13, 896 },
    { MBId, 0x73, 13, 960 }, { MBId, 0x74, 13, 1024 },
    { MBId, 0x75, 13, 1088 }, { MBId, 0x76, 13, 1152 },
    { MBId, 0x77, 13, 1216 }, { MBId, 0x52, 13, 1280 },
    { MBId, 0x53, 13, 1344 }, { MBId, 0x54, 13, 1408 },
    { MBId, 0x55, 13, 1472 }, { MBId, 0x5a, 13, 1536 },
    { MBId, 0x5b, 13, 1600 }, { MBId, 0x64, 13, 1664 },
    { MBId, 0x65, 13, 1728 }, { MBId, 0x00, 0, 0 }
  };

static const HuffmanTable
  EXTable[]=
  {
    { EXId, 0x08, 11, 1792 }, { EXId, 0x0c, 11, 1856 },
    { EXId, 0x0d, 11, 1920 }, { EXId, 0x12, 12, 1984 },
    { EXId, 0x13, 12, 2048 }, { EXId, 0x14, 12, 2112 },
    { EXId, 0x15, 12, 2176 }, { EXId, 0x16, 12, 2240 },
    { EXId, 0x17, 12, 2304 }, { EXId, 0x1c, 12, 2368 },
    { EXId, 0x1d, 12, 2432 }, { EXId, 0x1e, 12, 2496 },
    { EXId, 0x1f, 12, 2560 }, { EXId, 0x00, 0, 0 }
  };

static const HuffmanTable
  MWTable[]=
  {
    { MWId, 0x1b, 5, 64 }, { MWId, 0x12, 5, 128 },
    { MWId, 0x17, 6, 192 }, { MWId, 0x37, 7, 256 },
    { MWId, 0x36, 8, 320 }, { MWId, 0x37, 8, 384 },
    { MWId, 0x64, 8, 448 }, { MWId, 0x65, 8, 512 },
    { MWId, 0x68, 8, 576 }, { MWId, 0x67, 8, 640 },
    { MWId, 0xcc, 9, 704 }, { MWId, 0xcd, 9, 768 },
    { MWId, 0xd2, 9, 832 }, { MWId, 0xd3, 9, 896 },
    { MWId, 0xd4, 9, 960 }, { MWId, 0xd5, 9, 1024 },
    { MWId, 0xd6, 9, 1088 }, { MWId, 0xd7, 9, 1152 },
    { MWId, 0xd8, 9, 1216 }, { MWId, 0xd9, 9, 1280 },
    { MWId, 0xda, 9, 1344 }, { MWId, 0xdb, 9, 1408 },
    { MWId, 0x98, 9, 1472 }, { MWId, 0x99, 9, 1536 },
    { MWId, 0x9a, 9, 1600 }, { MWId, 0x18, 6, 1664 },
    { MWId, 0x9b, 9, 1728 }, { MWId, 0x00, 0, 0 }
  };

static const HuffmanTable
  TBTable[]=
  {
    { TBId, 0x37, 10, 0 }, { TBId, 0x02, 3, 1 }, { TBId, 0x03, 2, 2 },
    { TBId, 0x02, 2, 3 }, { TBId, 0x03, 3, 4 }, { TBId, 0x03, 4, 5 },
    { TBId, 0x02, 4, 6 }, { TBId, 0x03, 5, 7 }, { TBId, 0x05, 6, 8 },
    { TBId, 0x04, 6, 9 }, { TBId, 0x04, 7, 10 }, { TBId, 0x05, 7, 11 },
    { TBId, 0x07, 7, 12 }, { TBId, 0x04, 8, 13 }, { TBId, 0x07, 8, 14 },
    { TBId, 0x18, 9, 15 }, { TBId, 0x17, 10, 16 }, { TBId, 0x18, 10, 17 },
    { TBId, 0x08, 10, 18 }, { TBId, 0x67, 11, 19 }, { TBId, 0x68, 11, 20 },
    { TBId, 0x6c, 11, 21 }, { TBId, 0x37, 11, 22 }, { TBId, 0x28, 11, 23 },
    { TBId, 0x17, 11, 24 }, { TBId, 0x18, 11, 25 }, { TBId, 0xca, 12, 26 },
    { TBId, 0xcb, 12, 27 }, { TBId, 0xcc, 12, 28 }, { TBId, 0xcd, 12, 29 },
    { TBId, 0x68, 12, 30 }, { TBId, 0x69, 12, 31 }, { TBId, 0x6a, 12, 32 },
    { TBId, 0x6b, 12, 33 }, { TBId, 0xd2, 12, 34 }, { TBId, 0xd3, 12, 35 },
    { TBId, 0xd4, 12, 36 }, { TBId, 0xd5, 12, 37 }, { TBId, 0xd6, 12, 38 },
    { TBId, 0xd7, 12, 39 }, { TBId, 0x6c, 12, 40 }, { TBId, 0x6d, 12, 41 },
    { TBId, 0xda, 12, 42 }, { TBId, 0xdb, 12, 43 }, { TBId, 0x54, 12, 44 },
    { TBId, 0x55, 12, 45 }, { TBId, 0x56, 12, 46 }, { TBId, 0x57, 12, 47 },
    { TBId, 0x64, 12, 48 }, { TBId, 0x65, 12, 49 }, { TBId, 0x52, 12, 50 },
    { TBId, 0x53, 12, 51 }, { TBId, 0x24, 12, 52 }, { TBId, 0x37, 12, 53 },
    { TBId, 0x38, 12, 54 }, { TBId, 0x27, 12, 55 }, { TBId, 0x28, 12, 56 },
    { TBId, 0x58, 12, 57 }, { TBId, 0x59, 12, 58 }, { TBId, 0x2b, 12, 59 },
    { TBId, 0x2c, 12, 60 }, { TBId, 0x5a, 12, 61 }, { TBId, 0x66, 12, 62 },
    { TBId, 0x67, 12, 63 }, { TBId, 0x00, 0, 0 }
  };

static const HuffmanTable
  TWTable[]=
  {
    { TWId, 0x35, 8, 0 }, { TWId, 0x07, 6, 1 }, { TWId, 0x07, 4, 2 },
    { TWId, 0x08, 4, 3 }, { TWId, 0x0b, 4, 4 }, { TWId, 0x0c, 4, 5 },
    { TWId, 0x0e, 4, 6 }, { TWId, 0x0f, 4, 7 }, { TWId, 0x13, 5, 8 },
    { TWId, 0x14, 5, 9 }, { TWId, 0x07, 5, 10 }, { TWId, 0x08, 5, 11 },
    { TWId, 0x08, 6, 12 }, { TWId, 0x03, 6, 13 }, { TWId, 0x34, 6, 14 },
    { TWId, 0x35, 6, 15 }, { TWId, 0x2a, 6, 16 }, { TWId, 0x2b, 6, 17 },
    { TWId, 0x27, 7, 18 }, { TWId, 0x0c, 7, 19 }, { TWId, 0x08, 7, 20 },
    { TWId, 0x17, 7, 21 }, { TWId, 0x03, 7, 22 }, { TWId, 0x04, 7, 23 },
    { TWId, 0x28, 7, 24 }, { TWId, 0x2b, 7, 25 }, { TWId, 0x13, 7, 26 },
    { TWId, 0x24, 7, 27 }, { TWId, 0x18, 7, 28 }, { TWId, 0x02, 8, 29 },
    { TWId, 0x03, 8, 30 }, { TWId, 0x1a, 8, 31 }, { TWId, 0x1b, 8, 32 },
    { TWId, 0x12, 8, 33 }, { TWId, 0x13, 8, 34 }, { TWId, 0x14, 8, 35 },
    { TWId, 0x15, 8, 36 }, { TWId, 0x16, 8, 37 }, { TWId, 0x17, 8, 38 },
    { TWId, 0x28, 8, 39 }, { TWId, 0x29, 8, 40 }, { TWId, 0x2a, 8, 41 },
    { TWId, 0x2b, 8, 42 }, { TWId, 0x2c, 8, 43 }, { TWId, 0x2d, 8, 44 },
    { TWId, 0x04, 8, 45 }, { TWId, 0x05, 8, 46 }, { TWId, 0x0a, 8, 47 },
    { TWId, 0x0b, 8, 48 }, { TWId, 0x52, 8, 49 }, { TWId, 0x53, 8, 50 },
    { TWId, 0x54, 8, 51 }, { TWId, 0x55, 8, 52 }, { TWId, 0x24, 8, 53 },
    { TWId, 0x25, 8, 54 }, { TWId, 0x58, 8, 55 }, { TWId, 0x59, 8, 56 },
    { TWId, 0x5a, 8, 57 }, { TWId, 0x5b, 8, 58 }, { TWId, 0x4a, 8, 59 },
    { TWId, 0x4b, 8, 60 }, { TWId, 0x32, 8, 61 }, { TWId, 0x33, 8, 62 },
    { TWId, 0x34, 8, 63 }, { TWId, 0x00, 0, 0 }
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A S C I I 8 5 E n c o d e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ASCII85Encode encodes data in ASCII base-85 format.  ASCII base-85
%  encoding produces five ASCII printing characters from every four bytes of
%  binary data.
%
%  The format of the ASCII85Encode routine is:
%
%      ASCII85Encode(code,file)
%
%  A description of each parameter follows:
%
%    o code: a binary unsigned char to encode to ASCII 85.
%
%    o file: write the encoded ASCII character to this file.
%
%
*/
#define MaxLineExtent  36

static int
  offset,
  line_break;

static unsigned char
  ascii85_buffer[10];

static char *Ascii85Tuple(unsigned char *data)
{
  static char
    tuple[6];

  register unsigned int
    word,
    x;

  register unsigned short
    y;

  word=(((data[0] << 8) | data[1]) << 16) | (data[2] << 8) | data[3];
  if (word == 0L)
    {
      tuple[0]='z';
      tuple[1]='\0';
      return(tuple);
    }
  x=(unsigned int) (word/(85L*85*85*85));
  tuple[0]=x+'!';
  word-=x*(85L*85*85*85);
  x=(unsigned int) (word/(85L*85*85));
  tuple[1]=x+'!';
  word-=x*(85L*85*85);
  x=(unsigned int) (word/(85*85));
  tuple[2]=x+'!';
  y=(unsigned short) (word-x*(85L*85));
  tuple[3]=(y/85)+'!';
  tuple[4]=(y % 85)+'!';
  tuple[5]='\0';
  return(tuple);
}

Export void Ascii85Initialize(void)
{
  line_break=MaxLineExtent << 1;
  offset=0;
}

Export void Ascii85Flush(FILE *file)
{
  register char
    *tuple;

  assert(file != (FILE *) NULL);
  if (offset > 0)
    {
      ascii85_buffer[offset]=0;
      ascii85_buffer[offset+1]=0;
      ascii85_buffer[offset+2]=0;
      tuple=Ascii85Tuple(ascii85_buffer);
      (void) fwrite(*tuple == 'z' ? "!!!!" : tuple,offset+1,1,file);
    }
  (void) fputc('~',file);
  (void) fputc('>',file);
}

Export void Ascii85Encode(const unsigned int code,FILE *file)
{
  int
    n;

  register char
    *q;

  register unsigned char
    *p;

  assert(file != (const FILE *) NULL);
  ascii85_buffer[offset]=code;
  offset++;
  if (offset < 4)
    return;
  p=ascii85_buffer;
  for (n=offset; n >= 4; n-=4)
  {
    for (q=Ascii85Tuple(p); *q; q++)
    {
      line_break--;
      if ((line_break < 0) && (*(q+1) != '%'))
        {
          (void) fputc('\n',file);
          line_break=2*MaxLineExtent;
        }
      (void) fputc(*q,file);
    }
    p+=8;
  }
  offset=n;
  p-=4;
  for (n=0; n < 4; n++)
    ascii85_buffer[n]=(*p++);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B M P D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BMPDecodeImage unpacks the packed image pixels into
%  runlength-encoded pixel packets.
%
%  The format of the BMPDecodeImage routine is:
%
%      status=BMPDecodeImage(file,compression,number_columns,number_rows,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method BMPDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  BMP encoded pixels
%      are read from this file.
%
%    o compression:  A value of 1 means the compressed pixels are runlength
%      encoded for a 256-color bitmap.  A value of 2 means a 16-color bitmap.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
Export unsigned int BMPDecodeImage(FILE *file,const unsigned int compression,
  const unsigned int number_columns,const unsigned int number_rows,
  unsigned char *pixels)
{
  int
    byte,
    count;

  register int
    i,
    x,
    y;

  register unsigned char
    *q;

  assert(file != (FILE *) NULL);
  assert(pixels != (unsigned char *) NULL);
  for (i=0; i < (int) (number_columns*number_rows); i++)
    pixels[i]=0;
  byte=0;
  x=0;
  q=pixels;
  for (y=0; y < (int) number_rows; )
  {
    count=fgetc(file);
    if (count != 0)
      {
        /*
          Encoded mode.
        */
        byte=fgetc(file);
        for (i=0; i < count; i++)
        {
          if (compression == 1)
            *q++=(unsigned char) byte;
          else
            *q++=(i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
          x++;
        }
      }
    else
      {
        /*
          Escape mode.
        */
        count=fgetc(file);
        if (count == 0x01)
          return(True);
        switch (count)
        {
          case 0x00:
          {
            /*
              End of line.
            */
            x=0;
            y++;
            q=pixels+y*number_columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=fgetc(file);
            y+=fgetc(file);
            q=pixels+y*number_columns+x;
            break;
          }
          default:
          {
            /*
              Absolute mode.
            */
            for (i=0; i < count; i++)
            {
              if (compression == 1)
                *q++=fgetc(file);
              else
                {
                  if ((i & 0x01) == 0)
                    byte=fgetc(file);
                  *q++=(i & 0x01) ? (byte & 0x0f) : ((byte >> 4) & 0x0f);
                }
              x++;
            }
            /*
              Read pad byte.
            */
            if (compression == 1)
              {
                if (count & 0x01)
                  (void) fgetc(file);
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                (void) fgetc(file);
            break;
          }
        }
      }
    if (QuantumTick(y,number_rows))
      ProgressMonitor(LoadImageText,y,number_rows);
  }
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B M P E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method BMPEncodeImage compresses pixels using a runlength encoded format.
%
%  The format of the BMPEncodeImage routine is:
%
%      status=BMPEncodeImage(pixels,number_columns,number_rows,
%        compressed_pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method BMPEncodeImage returns the number of bytes in the
%      runlength encoded compress_pixels array.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the compression process.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%
*/
Export unsigned int BMPEncodeImage(const unsigned char *pixels,
  const unsigned int number_columns,const unsigned int number_rows,
  unsigned char *compressed_pixels)
{
  register const unsigned char
    *p;

  register int
    i,
    x,
    y;

  register unsigned char
    *q;

  /*
    Runlength encode pixels.
  */
  assert(pixels != (unsigned char *) NULL);
  assert(compressed_pixels != (unsigned char *) NULL);
  p=pixels;
  q=compressed_pixels;
  i=0;
  for (y=0; y < (int) number_rows; y++)
  {
    for (x=0; x < (int) number_columns; x+=i)
    {
      /*
        Determine runlength.
      */
      for (i=1; ((x+i) < (int) number_columns); i++)
        if ((*(p+i) != *p) || (i == 255))
          break;
      *q++=(unsigned char) i;
      *q++=(*p);
      p+=i;
    }
    /*
      End of line.
    */
    *q++=0;
    *q++=0x00;
    if (QuantumTick(y,number_rows))
      ProgressMonitor(SaveImageText,y,number_rows);
  }
  /*
    End of bitmap.
  */
  *q++=0;
  *q++=0x01;
  return((unsigned int) (q-compressed_pixels));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G I F D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GIFDecodeImage uncompresses an image via GIF-coding.
%
%  The format of the GIFDecodeImage routine is:
%
%      status=GIFDecodeImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Method GIFDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int GIFDecodeImage(Image *image)
{
#define MaxStackSize  4096
#define NullCode  (-1)

  int
    available,
    clear,
    code_mask,
    code_size,
    end_of_information,
    in_code,
    old_code;

  register int
    bits,
    code,
    count,
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *c,
    *p;

  register unsigned int
    datum;

  short
    *prefix;

  unsigned char
    data_size,
    first,
    *packet,
    *pixels,
    *pixel_stack,
    *suffix,
    *top_stack;

  unsigned long
    packets,
    max_packets;

  unsigned short
    index;

  /*
    Allocate decoder tables.
  */
  assert(image != (Image *) NULL);
  pixels=(unsigned char *)
    AllocateMemory(image->columns*image->rows*sizeof(unsigned char));
  packet=(unsigned char *) AllocateMemory(256*sizeof(unsigned char));
  prefix=(short *) AllocateMemory(MaxStackSize*sizeof(short));
  suffix=(unsigned char *) AllocateMemory(MaxStackSize*sizeof(unsigned char));
  pixel_stack=(unsigned char *)
    AllocateMemory((MaxStackSize+1)*sizeof(unsigned char));
  if ((pixels == (unsigned char *) NULL) ||
      (packet == (unsigned char *) NULL) ||
      (prefix == (short *) NULL) ||
      (suffix == (unsigned char *) NULL) ||
      (pixel_stack == (unsigned char *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        image->filename);
      return(False);
    }
  /*
    Initialize GIF data stream decoder.
  */
  data_size=fgetc(image->file);
  clear=1 << data_size;
  end_of_information=clear+1;
  available=clear+2;
  old_code=NullCode;
  code_size=data_size+1;
  code_mask=(1 << code_size)-1;
  for (code=0; code < clear; code++)
  {
    prefix[code]=0;
    suffix[code]=(unsigned char) code;
  }
  /*
    Decode GIF pixel stream.
  */
  datum=0;
  bits=0;
  c=0;
  count=0;
  first=0;
  top_stack=pixel_stack;
  p=pixels;
  for (i=0; i < (int) (image->columns*image->rows); )
  {
    if (top_stack == pixel_stack)
      {
        if (bits < code_size)
          {
            /*
              Load bytes until there is enough bits for a code.
            */
            if (count == 0)
              {
                /*
                  Read a new data block.
                */
                count=ReadDataBlock((char *) packet,image->file);
                if (count <= 0)
                  break;
                c=packet;
              }
            datum+=(*c) << bits;
            bits+=8;
            c++;
            count--;
            continue;
          }
        /*
          Get the next code.
        */
        code=datum & code_mask;
        datum>>=code_size;
        bits-=code_size;
        /*
          Interpret the code
        */
        if ((code > available) || (code == end_of_information))
          break;
        if (code == clear)
          {
            /*
              Reset decoder.
            */
            code_size=data_size+1;
            code_mask=(1 << code_size)-1;
            available=clear+2;
            old_code=NullCode;
            continue;
          }
        if (old_code == NullCode)
          {
            *top_stack++=suffix[code];
            old_code=code;
            first=(unsigned char) code;
            continue;
          }
        in_code=code;
        if (code == available)
          {
            *top_stack++=first;
            code=old_code;
          }
        while (code > clear)
        {
          *top_stack++=suffix[code];
          code=prefix[code];
        }
        first=suffix[code];
        /*
          Add a new string to the string table,
        */
        if (available >= MaxStackSize)
          break;
        *top_stack++=first;
        prefix[available]=old_code;
        suffix[available]=first;
        available++;
        if (((available & code_mask) == 0) && (available < MaxStackSize))
          {
            code_size++;
            code_mask+=available;
          }
        old_code=in_code;
      }
    /*
      Pop a pixel off the pixel stack.
    */
    top_stack--;
    *p=(*top_stack);
    p++;
    i++;
  }
  if (i < (int) (image->columns*image->rows))
    {
      for ( ; i < (int) (image->columns*image->rows); i++)
        *p++=0;
      MagickWarning(CorruptImageWarning,"Corrupt GIF image",image->filename);
    }
  /*
    Free decoder memory.
  */
  FreeMemory((char *) pixel_stack);
  FreeMemory((char *) suffix);
  FreeMemory((char *) prefix);
  FreeMemory((char *) packet);
  if (image->interlace != NoInterlace)
    {
      int
        pass,
        y;

      register int
        x;

      register unsigned char
        *q;

      static const int
        interlace_rate[4] = { 8, 8, 4, 2 },
        interlace_start[4] = { 0, 4, 2, 1 };

      unsigned char
        *interlaced_pixels;

      /*
        Interlace image.
      */
      interlaced_pixels=pixels;
      pixels=(unsigned char *)
        AllocateMemory(image->columns*image->rows*sizeof(unsigned char));
      if (pixels == (unsigned char *) NULL)
        {
          FreeMemory(interlaced_pixels);
          MagickWarning(ResourceLimitWarning,"Memory allocation failed",
            image->filename);
          return(False);
        }
      p=interlaced_pixels;
      q=pixels;
      for (pass=0; pass < 4; pass++)
      {
        y=interlace_start[pass];
        while (y < (int) image->rows)
        {
          q=pixels+(y*image->columns);
          for (x=0; x < (int) image->columns; x++)
          {
            *q=(*p);
            p++;
            q++;
          }
          y+=interlace_rate[pass];
        }
      }
      FreeMemory(interlaced_pixels);
    }
  /*
    Convert GIF pixels to runlength-encoded packets.
  */
  packets=0;
  max_packets=Max((image->columns*image->rows+2) >> 2,1);
  image->pixels=(RunlengthPacket *)
    AllocateMemory(max_packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        image->filename);
      return(False);
    }
  p=pixels;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (i=0; i < (int) (image->columns*image->rows); i++)
  {
    index=(*p++);
    if ((index == q->index) && ((int) q->length < MaxRunlength))
       q->length++;
     else
       {
         if (packets != 0)
           q++;
         packets++;
         if (packets == max_packets)
           {
             max_packets<<=1;
             image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
               image->pixels,max_packets*sizeof(RunlengthPacket));
             if (image->pixels == (RunlengthPacket *) NULL)
               {
                 MagickWarning(ResourceLimitWarning,"Memory allocation failed",
                   image->filename);
                 break;
               }
             q=image->pixels+packets-1;
           }
         q->index=index;
         q->length=0;
       }
    if (image->previous == (Image *) NULL)
      if (QuantumTick(i,image->columns*image->rows))
        ProgressMonitor(LoadImageText,i,image->columns*image->rows);
  }
  SetRunlengthPackets(image,packets);
  SyncImage(image);
  FreeMemory(pixels);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G I F E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GIFEncodeImage compresses an image via GIF-coding.
%
%  The format of the GIFEncodeImage routine is:
%
%      status=GIFEncodeImage(image,data_size)
%
%  A description of each parameter follows:
%
%    o status:  Method GIFEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int GIFEncodeImage(const Image *image,
  const unsigned int data_size)
{
#define MaxCode(number_bits)  ((1 << (number_bits))-1)
#define MaxHashTable  5003
#define MaxGIFBits  12
#if defined(HasLZW)
#define MaxGIFTable  (1 << MaxGIFBits)
#else
#define MaxGIFTable  max_code
#endif
#define GIFOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=((long) code << bits); \
  else \
    datum=(long) code; \
  bits+=number_bits; \
  while (bits >= 8) \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    packet[byte_count++]=(unsigned char) (datum & 0xff); \
    if (byte_count >= 254) \
      { \
        (void) fputc(byte_count,image->file); \
        (void) fwrite((char *) packet,1,byte_count,image->file); \
        byte_count=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxGIFBits) \
        max_code=MaxGIFTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}

  int
    bits,
    byte_count,
    i,
    next_pixel,
    number_bits;

  long
    datum;

  register int
    displacement,
    j,
    k;

  register RunlengthPacket
    *p;

  short
    clear_code,
    end_of_information_code,
    free_code,
    *hash_code,
    *hash_prefix,
    index,
    max_code,
    waiting_code;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Allocate encoder tables.
  */
  assert(image != (Image *) NULL);
  packet=(unsigned char *) AllocateMemory(256*sizeof(unsigned char));
  hash_code=(short *) AllocateMemory(MaxHashTable*sizeof(short));
  hash_prefix=(short *) AllocateMemory(MaxHashTable*sizeof(short));
  hash_suffix=(unsigned char *)
    AllocateMemory(MaxHashTable*sizeof(unsigned char));
  if ((packet == (unsigned char *) NULL) || (hash_code == (short *) NULL) ||
      (hash_prefix == (short *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    return(False);
  /*
    Initialize GIF encoder.
  */
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short) 1 << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  byte_count=0;
  datum=0;
  bits=0;
  for (i=0; i < MaxHashTable; i++)
    hash_code[i]=0;
  GIFOutputCode(clear_code);
  /*
    Encode pixels.
  */
  p=image->pixels;
  waiting_code=p->index;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=(i == 0) ? 1 : 0; j <= ((int) p->length); j++)
    {
      /*
        Probe hash table.
      */
      index=p->index & 0xff;
      k=(int) ((int) index << (MaxGIFBits-8))+waiting_code;
      if (k >= MaxHashTable)
        k-=MaxHashTable;
#if defined(HasLZW)
      if (hash_code[k] > 0)
        {
          if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
            {
              waiting_code=hash_code[k];
              continue;
            }
          if (k == 0)
            displacement=1;
          else
            displacement=MaxHashTable-k;
          next_pixel=False;
          for ( ; ; )
          {
            k-=displacement;
            if (k < 0)
              k+=MaxHashTable;
            if (hash_code[k] == 0)
              break;
            if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
              {
                waiting_code=hash_code[k];
                next_pixel=True;
                break;
              }
          }
          if (next_pixel == True)
            continue;
        }
#endif
      GIFOutputCode(waiting_code);
      if (free_code < MaxGIFTable)
        {
          hash_code[k]=free_code++;
          hash_prefix[k]=waiting_code;
          hash_suffix[k]=(unsigned char) index;
        }
      else
        {
          /*
            Fill the hash table with empty entries.
          */
          for (k=0; k < MaxHashTable; k++)
            hash_code[k]=0;
          /*
            Reset compressor and issue a clear code.
          */
          free_code=clear_code+2;
          GIFOutputCode(clear_code);
          number_bits=data_size;
          max_code=MaxCode(number_bits);
        }
      waiting_code=index;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(i,image->packets))
          ProgressMonitor(SaveImageText,i,image->packets);
    }
    p++;
  }
  /*
    Flush out the buffered code.
  */
  GIFOutputCode(waiting_code);
  GIFOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      packet[byte_count++]=(unsigned char) (datum & 0xff);
      if (byte_count >= 254)
        {
          (void) fputc(byte_count,image->file);
          (void) fwrite((char *) packet,1,byte_count,image->file);
          byte_count=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (byte_count > 0)
    {
      (void) fputc(byte_count,image->file);
      (void) fwrite((char *) packet,1,byte_count,image->file);
    }
  /*
    Free encoder memory.
  */
  FreeMemory((char *) hash_suffix);
  FreeMemory((char *) hash_prefix);
  FreeMemory((char *) hash_code);
  FreeMemory((char *) packet);
  if (i < (int) image->packets)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n D e c o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HuffmanDecodeImage uncompresses an image via Huffman-coding.
%
%  The format of the HuffmanDecodeImage routine is:
%
%      status=HuffmanDecodeImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Method HuffmanDecodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int HuffmanDecodeImage(Image *image)
{
#define HashSize  1021
#define MBHashA  293
#define MBHashB  2695
#define MWHashA  3510
#define MWHashB  1178

#define InitializeHashTable(hash,table,a,b) \
{ \
  entry=table; \
  while (entry->code != 0) \
  {  \
    hash[((entry->length+a)*(entry->code+b)) % HashSize]=(HuffmanTable *) entry; \
    entry++; \
  } \
}

#define InputBit(bit)  \
{  \
  if ((mask & 0xff) == 0)  \
    {  \
      byte=getc(image->file);  \
      mask=0x80;  \
    }  \
  runlength++;  \
  bit=byte & mask ? 1 : 0; \
  mask>>=1;  \
  if (bit)  \
    runlength=0;  \
  if (feof(image->file))  \
    break;  \
}

  const HuffmanTable
    *entry;

  HuffmanTable
    **mb_hash,
    **mw_hash;

  int
    bail,
    code,
    color,
    count,
    length,
    null_lines,
    runlength,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    bit,
    byte,
    mask,
    *scanline;

  unsigned long
    packets,
    max_packets;

  unsigned short
    index;

  /*
    Allocate buffers.
  */
  assert(image != (Image *) NULL);
  mb_hash=(HuffmanTable **) AllocateMemory(HashSize*sizeof(HuffmanTable *));
  mw_hash=(HuffmanTable **) AllocateMemory(HashSize*sizeof(HuffmanTable *));
  scanline=(unsigned char *)
    AllocateMemory(image->columns*sizeof(unsigned char));
  if ((mb_hash == (HuffmanTable **) NULL) ||
      (mw_hash == (HuffmanTable **) NULL) ||
      (scanline == (unsigned char *) NULL))
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  /*
    Initialize Huffman tables.
  */
  for (i=0; i < HashSize; i++)
  {
    mb_hash[i]=(HuffmanTable *) NULL;
    mw_hash[i]=(HuffmanTable *) NULL;
  }
  InitializeHashTable(mw_hash,TWTable,MWHashA,MWHashB);
  InitializeHashTable(mw_hash,MWTable,MWHashA,MWHashB);
  InitializeHashTable(mw_hash,EXTable,MWHashA,MWHashB);
  InitializeHashTable(mb_hash,TBTable,MBHashA,MBHashB);
  InitializeHashTable(mb_hash,MBTable,MBHashA,MBHashB);
  InitializeHashTable(mb_hash,EXTable,MBHashA,MBHashB);
  /*
    Uncompress 1D Huffman to runlength encoded pixels.
  */
  byte=0;
  mask=0;
  null_lines=0;
  runlength=0;
  while (runlength < 11)
   InputBit(bit);
  do { InputBit(bit); } while (bit == 0);
  packets=0;
  max_packets=image->packets;
  image->packets=0;
  q=image->pixels;
  SetRunlengthEncoder(q);
  for (y=0; ((y < (int) image->rows) && (null_lines < 3)); )
  {
    /*
      Initialize scanline to white.
    */
    p=scanline;
    for (x=0; x < (int) image->columns; x++)
      *p++=0;
    /*
      Decode Huffman encoded scanline.
    */
    color=True;
    code=0;
    count=0;
    length=0;
    runlength=0;
    x=0;
    for ( ; ; )
    {
      if (x >= (int) image->columns)
        {
          while (runlength < 11)
           InputBit(bit);
          do { InputBit(bit); } while (bit == 0);
          break;
        }
      bail=False;
      do
      {
        if (runlength < 11)
          InputBit(bit)
        else
          {
            InputBit(bit);
            if (bit)
              {
                null_lines++;
                if (x != 0)
                  null_lines=0;
                bail=True;
                break;
              }
          }
        code=(code << 1)+bit;
        length++;
      } while (code <= 0);
      if (bail)
        break;
      if (length > 13)
        {
          while (runlength < 11)
           InputBit(bit);
          do { InputBit(bit); } while (bit == 0);
          break;
        }
      if (color)
        {
          if (length < 4)
            continue;
          entry=mw_hash[((length+MWHashA)*(code+MWHashB)) % HashSize];
        }
      else
        {
          if (length < 2)
            continue;
          entry=mb_hash[((length+MBHashA)*(code+MBHashB)) % HashSize];
        }
      if (!entry)
        continue;
      if ((entry->length != length) || (entry->code != code))
        continue;
      switch (entry->id)
      {
        case TWId:
        case TBId:
        {
          count+=entry->count;
          if ((x+count) > (int) image->columns)
            count=image->columns-x;
          if (count > 0)
            {
              if (color)
                {
                  x+=count;
                  count=0;
                }
              else
                for ( ; count > 0; count--)
                  scanline[x++]=1;
            }
          color=!color;
          break;
        }
        case MWId:
        case MBId:
        case EXId:
        {
          count+=entry->count;
          break;
        }
        default:
          break;
      }
      code=0;
      length=0;
    }
    /*
      Transfer scanline to image pixels.
    */
    p=scanline;
    for (x=0; x < (int) image->columns; x++)
    {
      index=(unsigned short) (*p++);
      if ((index == q->index) && ((int) q->length < MaxRunlength))
        q->length++;
      else
        {
          if (packets != 0)
            q++;
          packets++;
          if (packets == max_packets)
            {
              max_packets<<=1;
              image->pixels=(RunlengthPacket *) ReallocateMemory((char *)
                image->pixels,max_packets*sizeof(RunlengthPacket));
              if (image->pixels == (RunlengthPacket *) NULL)
                {
                  MagickWarning(ResourceLimitWarning,
                    "Memory allocation failed",(char *) NULL);
                  return(False);
                }
              q=image->pixels+packets-1;
            }
          q->index=index;
          q->length=0;
        }
    }
    if (QuantumTick(y,image->rows))
      ProgressMonitor(LoadImageText,y,image->rows);
    y++;
  }
  image->packets=packets;
  image->rows=y;
  SyncImage(image);
  /*
    Free decoder memory.
  */
  FreeMemory((char *) mw_hash);
  FreeMemory((char *) mb_hash);
  FreeMemory((char *) scanline);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n E n c o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method HuffmanEncodeImage compresses an image via Huffman-coding.
%
%  The format of the HuffmanEncodeImage routine is:
%
%      status=HuffmanEncodeImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status:  Method HuffmanEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
*/
Export unsigned int HuffmanEncodeImage(const ImageInfo *image_info,
  const Image *image)
{
#define HuffmanOutputCode(entry)  \
{  \
  mask=1 << (entry->length-1);  \
  while (mask != 0)  \
  {  \
    OutputBit((entry->code & mask ? 1 : 0));  \
    mask>>=1;  \
  }  \
}

#define OutputBit(count)  \
{  \
  if(count > 0)  \
    byte=byte | bit;  \
  bit>>=1;  \
  if ((bit & 0xff) == 0)   \
    {  \
      if (Latin1Compare(image_info->magick,"FAX") == 0) \
        (void) fputc((char) byte,image->file);  \
      else \
        Ascii85Encode((unsigned int) byte,image->file); \
      byte=0;  \
      bit=0x80;  \
    }  \
}

  const HuffmanTable
    *entry;

  int
    i,
    k,
    runlength;

  register int
    j,
    n,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned short
    polarity;

  unsigned char
    bit,
    byte,
    *scanline;

  unsigned int
    mask,
    width;

  /*
    Allocate scanline buffer.
  */
  assert(image != (Image *) NULL);
  width=image->columns;
  if (Latin1Compare(image_info->magick,"FAX") == 0)
    width=Max(image->columns,1728);
  scanline=(unsigned char *) AllocateMemory((width+1)*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  byte=0;
  bit=0x80;
  if (Latin1Compare(image_info->magick,"FAX") != 0)
    Ascii85Initialize();
  else
    {
      /*
        End of line.
      */
      for (k=0; k < 11; k++)
        OutputBit(0);
      OutputBit(1);
    }
  /*
    Compress runlength encoded to 1D Huffman pixels.
  */
  polarity=0;
  if (image->colors == 2)
    polarity=(Intensity(image->colormap[0]) >
      Intensity(image->colormap[1]) ? 0 : 1);
  q=scanline;
  for (i=0; i < (int) width; i++)
    *q++=(unsigned char) polarity;
  p=image->pixels;
  q=scanline;
  x=0;
  for (i=0; i < (int) image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      *q++=(unsigned char)
        (p->index == polarity ? (int) polarity : (int) !polarity);
      x++;
      if (x < (int) image->columns)
        continue;
      /*
        Huffman encode scanline.
      */
      q=scanline;
      for (n=width; n > 0; )
      {
        /*
          Output white run.
        */
        for (runlength=0; ((*q == polarity) && (n > 0)); n--)
        {
          q++;
          runlength++;
        }
        if (runlength >= 64)
          {
            entry=MWTable+((runlength/64)-1);
            if (runlength >= 1792)
              entry=EXTable+(Min(runlength,2560)-1792)/64;
            runlength-=entry->count;
            HuffmanOutputCode(entry);
          }
        entry=TWTable+Min(runlength,63);
        HuffmanOutputCode(entry);
        if (n != 0)
          {
            /*
              Output black run.
            */
            for (runlength=0; ((*q != polarity) && (n > 0)); n--)
            {
              q++;
              runlength++;
            }
            if (runlength >= 64)
              {
                entry=MBTable+((runlength/64)-1);
                if (runlength >= 1792)
                  entry=EXTable+(Min(runlength,2560)-1792)/64;
                runlength-=entry->count;
                HuffmanOutputCode(entry);
              }
            entry=TBTable+Min(runlength,63);
            HuffmanOutputCode(entry);
          }
      }
      /*
        End of line.
      */
      for (k=0; k < 11; k++)
        OutputBit(0);
      OutputBit(1);
      x=0;
      q=scanline;
    }
    p++;
    if (image->previous == (Image *) NULL)
      if (QuantumTick(i,image->packets))
        ProgressMonitor(SaveImageText,i,image->packets);
  }
  /*
    End of page.
  */
  for (i=0; i < 6; i++)
  {
    for (k=0; k < 11; k++)
      OutputBit(0);
    OutputBit(1);
  }
  /*
    Flush bits.
  */
  if (bit != 0x80)
    {
      if (Latin1Compare(image_info->magick,"FAX") == 0)
        (void) fputc((char) byte,image->file);
      else
        Ascii85Encode((unsigned int) byte,image->file);
    }
  if (Latin1Compare(image_info->magick,"FAX") != 0)
    Ascii85Flush(image->file);
  FreeMemory((char *) scanline);
  return(True);
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n 2 D E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Huffman2DEncodeImage compresses an image via two-dimensional
%  Huffman-coding.
%
%  The format of the Huffman2DEncodeImage routine is:
%
%      status=Huffman2DEncodeImage(image_info,image)
%
%  A description of each parameter follows:
%
%    o status:  Method Huffman2DEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image: The address of a structure of type Image.
%
*/
Export unsigned int Huffman2DEncodeImage(ImageInfo *image_info,
  const Image *image)
{
  char
    *buffer;

  Image
    *huffman_image;

  ImageInfo
    *local_info;

  int
    count;

  register int
    i,
    j;

  TIFF
    *tiff;

  unsigned int
    *byte_count,
    status,
    strip_size;

  /*
    Write image as CCITTFax4 TIFF image to a temporary file.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image != (Image *) NULL);
  ((Image *) image)->orphan=True;
  huffman_image=CloneImage(image,image->columns,image->rows,True);
  ((Image *) image)->orphan=False;
  TemporaryFilename(huffman_image->filename);
  (void) strcpy(huffman_image->magick,"TIFF");
  local_info=CloneImageInfo(image_info);
  status=WriteImage(local_info,huffman_image);
  DestroyImageInfo(local_info);
  DestroyImage(huffman_image);
  if (status == False)
    return(False);
  tiff=TIFFOpen(huffman_image->filename,ReadBinaryType);
  (void) remove(huffman_image->filename);
  if (tiff == (TIFF *) NULL)
    {
      MagickWarning(FileOpenWarning,"Unable to open file",image_info->filename);
      return(False);
    }
  /*
    Allocate raw strip buffer.
  */
  TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_count);
  strip_size=byte_count[0];
  for (i=1; i < (int) TIFFNumberOfStrips(tiff); i++)
    if (byte_count[i] > strip_size)
      strip_size=byte_count[i];
  buffer=(char *) AllocateMemory(strip_size*sizeof(char));
  if (buffer == (char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      TIFFClose(tiff);
      return(False);
    }
  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  Ascii85Initialize();
  for (i=0; i < (int) TIFFNumberOfStrips(tiff); i++)
  {
    count=TIFFReadRawStrip(tiff,i,buffer,strip_size);
    for (j=0; j < count; j++)
      Ascii85Encode((unsigned int) buffer[j],image->file);
  }
  Ascii85Flush(image->file);
  FreeMemory((char *) buffer);
  TIFFClose(tiff);
  return(True);
}
#else
Export unsigned int Huffman2DEncodeImage(ImageInfo *image_info,
  const Image *image)
{
  MagickWarning(MissingDelegateWarning,"TIFF library is not available",
    image_info->filename);
  return(False);
}
#endif

#if defined(HasLZW)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LZWEncodeImage compresses an image via LZW-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary LZW bytes are encoded as ASCII base-85.
%
%  The format of the LZWEncodeImage routine is:
%
%      status=LZWEncodeImage(file,number_pixels,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method LZWEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  LZW encoded pixels
%      are written to this file.
%
%    o number_pixels:  An unsigned interger that specifies the number of
%      pixels to compress.
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
Export unsigned int LZWEncodeImage(FILE *file,const unsigned int number_pixels,
  unsigned char *pixels)
{
#define LZWClr  256  /* Clear Table Marker */
#define LZWEod  257  /* End of Data marker */
#define OutputCode(code) \
{ \
    accumulator+=((long) code) << (32-code_width-number_bits); \
    number_bits+=code_width; \
    while (number_bits >= 8) \
    { \
        Ascii85Encode((unsigned int) (accumulator >> 24),file); \
        accumulator=accumulator << 8; \
        number_bits-=8; \
    } \
}

  typedef struct _TableType
  {
    short
      prefix,
      suffix,
      next;
  } TableType;

  int
    index;

  register int
    i;

  short
    number_bits,
    code_width,
    last_code,
    next_index;

  TableType
    *table;

  unsigned long
    accumulator;

  /*
    Allocate string table.
  */
  assert(file != (FILE *) NULL);
  assert(pixels != (unsigned char *) NULL);
  table=(TableType *) AllocateMemory((1 << 12)*sizeof(TableType));
  if (table == (TableType *) NULL)
    return(False);
  /*
    Initialize variables.
  */
  accumulator=0;
  code_width=9;
  number_bits=0;
  last_code=0;
  Ascii85Initialize();
  OutputCode(LZWClr);
  for (index=0; index < 256; index++)
  {
    table[index].prefix=(-1);
    table[index].suffix=index;
    table[index].next=(-1);
  }
  next_index=LZWEod+1;
  code_width=9;
  last_code=pixels[0];
  for (i=1; i < (int) number_pixels; i++)
  {
    /*
      Find string.
    */
    index=last_code;
    while (index != -1)
      if ((table[index].prefix != last_code) ||
          (table[index].suffix != pixels[i]))
        index=table[index].next;
      else
        {
          last_code=index;
          break;
        }
    if (last_code != index)
      {
        /*
          Add string.
        */
        OutputCode(last_code);
        table[next_index].prefix=last_code;
        table[next_index].suffix=pixels[i];
        table[next_index].next=table[last_code].next;
        table[last_code].next=next_index;
        next_index++;
        /*
          Did we just move up to next bit width?
        */
        if ((next_index >> code_width) != 0)
          {
            code_width++;
            if (code_width > 12)
              {
                /*
                  Did we overflow the max bit width?
                */
                code_width--;
                OutputCode(LZWClr);
                for (index=0; index < 256; index++)
                {
                  table[index].prefix=(-1);
                  table[index].suffix=index;
                  table[index].next=(-1);
                }
                next_index=LZWEod+1;
                code_width=9;
              }
            }
          last_code=pixels[i];
      }
  }
  /*
    Flush tables.
  */
  OutputCode(last_code);
  OutputCode(LZWEod);
  if (number_bits != 0)
    Ascii85Encode((unsigned int) (accumulator >> 24),file);
  Ascii85Flush(file);
  FreeMemory(table);
  return(True);
}
#else
Export unsigned int LZWEncodeImage(FILE *file,const unsigned int number_pixels,
  unsigned char *pixels)
{
  MagickWarning(MissingDelegateWarning,"LZW library is not available",
    (char *) NULL);
  return(False);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a c k b i t s D e c o d e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PackbitsDecodeImage uncompresses an image via Macintosh Packbits
%  encoding specific to the Adobe Photoshop image format.
%
%  The format of the PackbitsDecodeImage routine is:
%
%      status=PackbitsDecodeImage(image,channel)
%
%  A description of each parameter follows:
%
%    o status: Method PackbitsDecodeImage return True if the image is
%      decoded.  False is returned if there is an error occurs.
%
%    o image: The address of a structure of type Image.
%
%    o channel:  Specifies which channel: red, green, blue, or index to
%      decode the pixel values into.
%
%
*/
Export unsigned int PackbitsDecodeImage(Image *image,const int channel)
{
  int
    count,
    pixel;

  long
    length;

  register int
    i;

  register RunlengthPacket
    *q;

  q=image->pixels;
  length=image->columns*image->rows;
  while (length > 0)
  {
    count=fgetc(image->file);
    if (count >= 128)
      count-=256;
    if (count < 0)
      {
        if (count == -128)
          continue;
        count=(-count+1);
        pixel=fgetc(image->file);
        for ( ; count > 0; count--)
        {
          switch (channel)
          {
            case 0:
            {
              q->red=(Quantum) pixel;
              if (image->class == PseudoClass)
                q->index=(unsigned short) pixel;
              break;
            }
            case 1:
            {
              q->green=(Quantum) pixel;
              break;
            }
            case 2:
            {
              q->blue=(Quantum) pixel;
              break;
            }
            case 3:
            default:
            {
              q->index=(unsigned short) pixel;
              break;
            }
          }
          q->length=0;
          q++;
          length--;
        }
        continue;
      }
    count++;
    for (i=count; i > 0; i--)
    {
      pixel=fgetc(image->file);
      switch (channel)
      {
        case 0:
        {
          q->red=(Quantum) pixel;
          if (image->class == PseudoClass)
            q->index=(unsigned short) pixel;
          break;
        }
        case 1:
        {
          q->green=(Quantum) pixel;
          break;
        }
        case 2:
        {
          q->blue=(Quantum) pixel;
          break;
        }
        case 3:
        default:
        {
          q->index=(unsigned short) pixel;
          break;
        }
      }
      q->length=0;
      q++;
      length--;
    }
  }
  /*
    Guarentee the correct number of pixel packets.
  */
  if (length > 0)
    {
      MagickWarning(CorruptImageWarning,"insufficient image data in file",
        image->filename);
      return(False);
    }
  else
    if (length < 0)
      {
        MagickWarning(CorruptImageWarning,"too much image data in file",
          image->filename);
        return(False);
      }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a c k b i t s E n c o d e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PackbitsEncodeImage compresses an image via Macintosh Packbits
%  encoding specific to Postscript Level II or Portable Document Format.  To
%  ensure portability, the binary Packbits bytes are encoded as ASCII Base-85.
%
%  The format of the PackbitsEncodeImage routine is:
%
%      status=PackbitsEncodeImage(file,number_pixels,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method PackbitsEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  LZW encoded pixels
%      are written to this file.
%
%    o number_pixels:  An unsigned integer that specifies the number of
%      pixels to compress.
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
Export unsigned int PackbitsEncodeImage(FILE *file,
  const unsigned int number_pixels,unsigned char *pixels)
{
  register int
    count,
    i,
    j;

  unsigned char
    *packbits;

  /*
    Compress pixels with Packbits encoding.
  */
  assert(file != (FILE *) NULL);
  assert(pixels != (unsigned char *) NULL);
  packbits=(unsigned char *) AllocateMemory(128*sizeof(unsigned char));
  if (packbits == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  Ascii85Initialize();
  i=number_pixels;
  while (i != 0)
  {
    switch (i)
    {
      case 1:
      {
        i--;
        Ascii85Encode(0,file);
        Ascii85Encode(*pixels,file);
        break;
      }
      case 2:
      {
        i-=2;
        Ascii85Encode(1,file);
        Ascii85Encode(*pixels,file);
        Ascii85Encode(pixels[1],file);
        break;
      }
      case 3:
      {
        i-=3;
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            Ascii85Encode((256-3)+1,file);
            Ascii85Encode(*pixels,file);
            break;
          }
        Ascii85Encode(2,file);
        Ascii85Encode(*pixels,file);
        Ascii85Encode(pixels[1],file);
        Ascii85Encode(pixels[2],file);
        break;
      }
      default:
      {
        if ((*pixels == *(pixels+1)) && (*(pixels+1) == *(pixels+2)))
          {
            /*
              Packed run.
            */
            count=3;
            while ((count < i) && (*pixels == *(pixels+count)))
            {
              count++;
              if (count >= 127)
                break;
            }
            i-=count;
            Ascii85Encode((256-count)+1,file);
            Ascii85Encode(*pixels,file);
            pixels+=count;
            break;
          }
        /*
          Literal run.
        */
        count=0;
        while ((*(pixels+count) != *(pixels+count+1)) ||
               (*(pixels+count+1) != *(pixels+count+2)))
        {
          packbits[count+1]=pixels[count];
          count++;
          if ((count >= (int) (i-3)) || (count >= 127))
            break;
        }
        i-=count;
        *packbits=count-1;
        for (j=0; j <= count; j++)
          Ascii85Encode(packbits[j],file);
        pixels+=count;
        break;
      }
    }
  }
  Ascii85Encode(128,file);  /* EOD marker */
  Ascii85Flush(file);
  FreeMemory((char *) packbits);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P C D D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PCDDecodeImage recovers the Huffman encoded luminance and
%  chrominance deltas.
%
%  The format of the PCDDecodeImage routine is:
%
%      status=PCDDecodeImage(image,luma,chroma1,chroma2)
%
%  A description of each parameter follows:
%
%    o status:  Method PCDDecodeImage returns True if all the deltas are
%      recovered without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o luma: The address of a character buffer that contains the
%      luminance information.
%
%    o chroma1: The address of a character buffer that contains the
%      chrominance information.
%
%    o chroma2: The address of a character buffer that contains the
%      chrominance information.
%
%
%
*/
Export unsigned int PCDDecodeImage(const Image *image,unsigned char *luma,
  unsigned char *chroma1,unsigned char *chroma2)
{
#define IsSync  ((accumulator & 0xffffff00) == 0xfffffe00)
#define PCDDecodeImageText  "  PCD decode image...  "
#define PCDGetBits(n) \
{  \
  accumulator=(accumulator << n) & 0xffffffff; \
  bits-=n; \
  while (bits <= 24) \
  { \
    if (p >= (buffer+0x800)) \
      { \
        (void) ReadData((char *) buffer,1,0x800,image->file); \
        p=buffer; \
      } \
    accumulator|=((unsigned int) (*p) << (24-bits)); \
    bits+=8; \
    p++; \
  } \
  if (feof(image->file)) \
    break; \
}

  typedef struct PCDTable
  {
    unsigned int
      length,
      sequence;

    unsigned char
      key;

    unsigned int
      mask;
  } PCDTable;

  int
    count;

  PCDTable
    *pcd_table[3];

  register int
    i,
    j;

  register PCDTable
    *r;

  register Quantum
    *range_limit;

  register unsigned char
    *p,
    *q;

  unsigned char
    *buffer;

  unsigned int
    accumulator,
    bits,
    length,
    pcd_length[3],
    plane,
    row;

  /*
    Initialize Huffman tables.
  */
  assert(image != (const Image *) NULL);
  assert(luma != (unsigned char *) NULL);
  assert(chroma1 != (unsigned char *) NULL);
  assert(chroma2 != (unsigned char *) NULL);
  buffer=(unsigned char *) AllocateMemory(0x800*sizeof(unsigned char));
  if (buffer == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  accumulator=0;
  bits=32;
  p=buffer+0x800;
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
  {
    PCDGetBits(8);
    length=(accumulator & 0xff)+1;
    pcd_table[i]=(PCDTable *) AllocateMemory(length*sizeof(PCDTable));
    if (pcd_table[i] == (PCDTable *) NULL)
      {
        MagickWarning(ResourceLimitWarning,"Memory allocation failed",
          (char *) NULL);
        FreeMemory((char *) buffer);
        return(False);
      }
    r=pcd_table[i];
    for (j=0; j < (int) length; j++)
    {
      PCDGetBits(8);
      r->length=(accumulator & 0xff)+1;
      if (r->length > 16)
        {
          FreeMemory((char *) buffer);
          return(False);
        }
      PCDGetBits(16);
      r->sequence=(accumulator & 0xffff) << 16;
      PCDGetBits(8);
      r->key=accumulator & 0xff;
      r->mask=(~((((unsigned int) 1) << (32-r->length))-1));
      r++;
    }
    pcd_length[i]=length;
  }
  /*
    Initialize range limits.
  */
  range_limit=(Quantum *) AllocateMemory(3*(MaxRGB+1)*sizeof(Quantum));
  if (range_limit == (Quantum *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      FreeMemory((char *) buffer);
      return(False);
    }
  for (i=0; i <= MaxRGB; i++)
  {
    range_limit[i]=0;
    range_limit[i+(MaxRGB+1)]=(Quantum) i;
    range_limit[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit+=(MaxRGB+1);
  /*
    Search for Sync byte.
  */
  do { PCDGetBits(16) } while (0);
  do { PCDGetBits(16) } while (0);
  while ((accumulator & 0x00fff000) != 0x00fff000)
    PCDGetBits(8);
  while (!IsSync)
    PCDGetBits(1);
  /*
    Recover the Huffman encoded luminance and chrominance deltas.
  */
  count=0;
  length=0;
  plane=0;
  q=luma;
  for ( ; ; )
  {
    if (IsSync)
      {
        /*
          Determine plane and row number.
        */
        PCDGetBits(16);
        row=((accumulator >> 9) & 0x1fff);
        if (row == image->rows)
          break;
        PCDGetBits(8);
        plane=accumulator >> 30;
        PCDGetBits(16);
        switch (plane)
        {
          case 0:
          {
            q=luma+row*image->columns;
            count=image->columns;
            break;
          }
          case 2:
          {
            q=chroma1+(row >> 1)*image->columns;
            count=image->columns >> 1;
            plane--;
            break;
          }
          case 3:
          {
            q=chroma2+(row >> 1)*image->columns;
            count=image->columns >> 1;
            plane--;
            break;
          }
          default:
          {
            MagickWarning(CorruptImageWarning,"Corrupt PCD image",
              image->filename);
            return(False);
          }
        }
        length=pcd_length[plane];
        if (QuantumTick(row,image->rows))
          ProgressMonitor(PCDDecodeImageText,row,image->rows);
        continue;
      }
    /*
      Decode luminance or chrominance deltas.
    */
    r=pcd_table[plane];
    for (i=0; ((i < (int) length) && ((accumulator & r->mask) != r->sequence)); i++)
      r++;
    if (r == (PCDTable *) NULL)
      {
        MagickWarning(CorruptImageWarning,
          "Corrupt PCD image, skipping to sync byte",image->filename);
        while ((accumulator & 0x00fff000) != 0x00fff000)
          PCDGetBits(8);
        while (!IsSync)
          PCDGetBits(1);
        continue;
      }
    if (r->key < 128)
      *q=range_limit[(int) *q+(int) r->key];
    else
      *q=range_limit[(int) *q+(int) r->key-256];
    q++;
    PCDGetBits(r->length);
    count--;
  }
  /*
    Free memory.
  */
  for (i=0; i < (image->columns > 1536 ? 3 : 1); i++)
    FreeMemory((char *) pcd_table[i]);
  range_limit-=(MaxRGB+1);
  FreeMemory((char *) range_limit);
  FreeMemory((char *) buffer);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P I C T D e c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PICTDecodeImage decompresses an image via Macintosh pack bits
%  decoding for Macintosh PICT images.
%
%  The format of the PICTDecodeImage routine is:
%
%      status=PICTDecodeImage(image,bytes_per_line,bits_per_pixel)
%
%  A description of each parameter follows:
%
%    o status:  Method PICTDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o bytes_per_line: This integer identifies the number of bytes in a
%      scanline.
%
%    o bits_per_pixel: The number of bits in a pixel.
%
%
*/

static unsigned char *ExpandBuffer(unsigned char *pixels,int *bytes_per_line,
  const int bits_per_pixel)
{
  register int
    i;

  register unsigned char
    *p,
    *q;

  static unsigned char
    scanline[8*256];

  p=pixels;
  q=scanline;
  switch (bits_per_pixel)
  {
    case 8:
    case 16:
    case 32:
      return(pixels);
    case 4:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 4) & 0xff;
        *q++=(*p & 15);
        p++;
      }
      *bytes_per_line*=2;
      break;
    }
    case 2:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 6) & 0x03;
        *q++=(*p >> 4) & 0x03;
        *q++=(*p >> 2) & 0x03;
        *q++=(*p & 3);
        p++;
      }
      *bytes_per_line*=4;
      break;
    }
    case 1:
    {
      for (i=0; i < *bytes_per_line; i++)
      {
        *q++=(*p >> 7) & 0x01;
        *q++=(*p >> 6) & 0x01;
        *q++=(*p >> 5) & 0x01;
        *q++=(*p >> 4) & 0x01;
        *q++=(*p >> 3) & 0x01;
        *q++=(*p >> 2) & 0x01;
        *q++=(*p >> 1) & 0x01;
        *q++=(*p & 0x01);
        p++;
      }
      *bytes_per_line*=8;
      break;
    }
    default:
      break;
  }
  return(scanline);
}

Export unsigned char* PICTDecodeImage(const Image *image,int bytes_per_line,
  const int bits_per_pixel)
{
  int
    bytes_per_pixel,
    k,
    length,
    number_pixels,
    packets,
    scanline_length,
    width;

  register int
    i,
    j;

  register unsigned char
    *p,
    *q;

  unsigned char
    *pixels,
    *scanline;

  /*
    Determine pixel buffer size.
  */
  if (bits_per_pixel <= 8)
    bytes_per_line&=0x7fff;
  width=image->columns;
  bytes_per_pixel=1;
  if (bits_per_pixel == 16)
    {
      bytes_per_pixel=2;
      width*=2;
    }
  else
    if (bits_per_pixel == 32)
      width*=image->matte ? 4 : 3;
  if (bytes_per_line == 0)
    bytes_per_line=width;
  packets=width;
  if (bytes_per_line < 8)
    packets=8*bytes_per_line;
  /*
    Allocate pixel and scanline buffer.
  */
  pixels=(unsigned char *)
    AllocateMemory((packets*image->rows+8)*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  scanline=(unsigned char *)
    AllocateMemory((bytes_per_line+100)*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  if (bytes_per_line < 8)
    {
      /*
        Pixels are already uncompressed.
      */
      for (i=0; i < (int) image->rows; i++)
      {
        q=pixels+i*width;
        number_pixels=bytes_per_line;
        (void) ReadData((char *) scanline,1,number_pixels,image->file);
        p=ExpandBuffer(scanline,&number_pixels,bits_per_pixel);
        for (j=0; j < number_pixels; j++)
          *q++=(*p++);
      }
      FreeMemory(scanline);
      return(pixels);
    }
  /*
    Uncompress RLE pixels into uncompressed pixel buffer.
  */
  for (i=0; i < (int) image->rows; i++)
  {
    q=pixels+i*width;
    if ((bytes_per_line > 250) || (bits_per_pixel > 8))
      scanline_length=MSBFirstReadShort(image->file);
    else
      scanline_length=fgetc(image->file);
    (void) ReadData((char *) scanline,1,scanline_length,image->file);
    for (j=0; j < scanline_length; )
      if ((scanline[j] & 0x80) == 0)
        {
          length=(scanline[j] & 0xff)+1;
          number_pixels=length*bytes_per_pixel;
          p=ExpandBuffer(scanline+j+1,&number_pixels,bits_per_pixel);
          for (k=0; k < number_pixels; k++)
            *q++=(*p++);
          j+=length*bytes_per_pixel+1;
        }
      else
        {
          int
            l;

          length=((scanline[j] ^ 0xff) & 0xff)+2;
          number_pixels=bytes_per_pixel;
          p=ExpandBuffer(scanline+j+1,&number_pixels,bits_per_pixel);
          for (k=0; k < length; k++)
          {
            for (l=0; l < number_pixels; l++)
              *q++=(*p++);
            p-=number_pixels;
          }
          j+=bytes_per_pixel+1;
        }
  }
  FreeMemory(scanline);
  return(pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P I C T E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method PICTEncodeImage compresses an image via Macintosh pack bits
%  encoding for Macintosh PICT images.
%
%  The format of the PICTEncodeImage routine is:
%
%      status=PICTEncodeImage(image,scanline,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method PICTEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o scanline: A pointer to an array of characters to pack.
%
%    o pixels: A pointer to an array of characters where the packed
%      characters are stored.
%
%
*/
Export unsigned int PICTEncodeImage(Image *image,const unsigned char *scanline,
  unsigned char *pixels)
{
#define MaxCount  128
#define MaxPackbitsRunlength  128

  int
    count,
    repeat_count,
    runlength;

  register const unsigned char
    *p;

  register int
    i;

  register long
    packets;

  register unsigned char
    *q;

  unsigned char
    index;

  unsigned int
    bytes_per_line;

  /*
    Pack scanline.
  */
  assert(image != (Image *) NULL);
  assert(scanline != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  bytes_per_line=image->columns;
  if (image->class == DirectClass)
    bytes_per_line*=image->matte ? 4 : 3;
  count=0;
  runlength=0;
  p=scanline+(bytes_per_line-1);
  q=pixels;
  index=(*p);
  for (i=bytes_per_line-1; i >= 0; i--)
  {
    if (index == *p)
      runlength++;
    else
      {
        if (runlength < 3)
          while (runlength > 0)
          {
            *q++=index;
            runlength--;
            count++;
            if (count == MaxCount)
              {
                *q++=MaxCount-1;
                count-=MaxCount;
              }
          }
        else
          {
            if (count > 0)
              *q++=count-1;
            count=0;
            while (runlength > 0)
            {
              repeat_count=runlength;
              if (repeat_count > MaxPackbitsRunlength)
                repeat_count=MaxPackbitsRunlength;
              *q++=index;
              *q++=257-repeat_count;
              runlength-=repeat_count;
            }
          }
        runlength=1;
      }
    index=(*p);
    p--;
  }
  if (runlength < 3)
    while (runlength > 0)
    {
      *q++=index;
      runlength--;
      count++;
      if (count == MaxCount)
        {
          *q++=MaxCount-1;
          count-=MaxCount;
        }
    }
  else
    {
      if (count > 0)
        *q++=count-1;
      count=0;
      while (runlength > 0)
      {
        repeat_count=runlength;
        if (repeat_count > MaxPackbitsRunlength)
          repeat_count=MaxPackbitsRunlength;
        *q++=index;
        *q++=257-repeat_count;
        runlength-=repeat_count;
      }
    }
  if (count > 0)
    *q++=count-1;
  /*
    Write the number of and the packed packets.
  */
  packets=(int) (q-pixels);
  MSBFirstWriteShort((unsigned short) packets,image->file);
  packets+=2;
  while (q != pixels)
  {
    q--;
    (void) fputc((char) *q,image->file);
  }
  return(packets);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R u n l e n g t h D e c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RunlengthDecodeImage unpacks the packed image pixels into
%  runlength-encoded pixel packets.  The packed image pixel memory is then
%  freed.
%
%  The format of the RunlengthDecodeImage routine is:
%
%      status=RunlengthDecodeImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method RunlengthDecodeImage return True if the image is
%      decoded.  False is returned if there is an error occurs.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int RunlengthDecodeImage(Image *image)
{
  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned long
    count;

  unsigned short
    value;

  assert(image != (Image *) NULL);
  if (image->packed_pixels == (unsigned char *) NULL)
    return(True);
  /*
    Allocate pixels.
  */
  if (image->pixels == (RunlengthPacket *) NULL)
    image->pixels=(RunlengthPacket *)
      AllocateMemory((unsigned int) image->packets*sizeof(RunlengthPacket));
  else
    image->pixels=(RunlengthPacket *) ReallocateMemory((char *) image->pixels,
      image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to unpack pixels",
        "Memory allocation failed");
      return(False);
    }
  /*
    Unpack the packed image pixels into runlength-encoded pixel packets.
  */
  p=image->packed_pixels;
  q=image->pixels;
  count=0;
  if (image->class == DirectClass)
    {
      if (image->compression == RunlengthEncodedCompression)
        for (i=0; i < (int) image->packets; i++)
        {
          ReadQuantum(q->red,p);
          ReadQuantum(q->green,p);
          ReadQuantum(q->blue,p);
          q->index=0;
          if (image->matte || (image->colorspace == CMYKColorspace))
            ReadQuantum(q->index,p);
          q->length=(*p++);
          count+=(q->length+1);
          q++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(LoadImageText,i,image->packets);
        }
      else
        for (i=0; i < (int) image->packets; i++)
        {
          ReadQuantum(q->red,p);
          ReadQuantum(q->green,p);
          ReadQuantum(q->blue,p);
          q->index=0;
          if (image->matte || (image->colorspace == CMYKColorspace))
            ReadQuantum(q->index,p);
          q->length=0;
          count++;
          q++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(LoadImageText,i,image->packets);
        }
    }
  else
    {
      register unsigned short
        index;

      if (image->compression == RunlengthEncodedCompression)
        {
          if (image->colors <= 256)
            for (i=0; i < (int) image->packets; i++)
            {
              q->index=(unsigned short) (*p++);
              q->length=(*p++);
              count+=(q->length+1);
              q++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(LoadImageText,i,image->packets);
            }
          else
            for (i=0; i < (int) image->packets; i++)
            {
              index=(*p++) << 8;
              index|=(*p++);
              q->index=index;
              q->length=(*p++);
              count+=(q->length+1);
              q++;
              if (image->previous == (Image *) NULL)
                if (QuantumTick(i,image->packets))
                  ProgressMonitor(LoadImageText,i,image->packets);
            }
        }
      else
        if (image->colors <= 256)
          for (i=0; i < (int) image->packets; i++)
          {
            q->index=(unsigned short) (*p++);
            q->length=0;
            count++;
            q++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(LoadImageText,i,image->packets);
          }
        else
          for (i=0; i < (int) image->packets; i++)
          {
            index=(*p++) << 8;
            index|=(*p++);
            q->index=index;
            q->length=0;
            count++;
            q++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(LoadImageText,i,image->packets);
          }
      SyncImage(image);
    }
  /*
    Free packed pixels memory.
  */
  FreeMemory((char *) image->packed_pixels);
  image->packed_pixels=(unsigned char *) NULL;
  /*
    Guarentee the correct number of pixel packets.
  */
  if (count > (image->columns*image->rows))
    {
      MagickWarning(CorruptImageWarning,"insufficient image data in file",
        image->filename);
      return(False);
    }
  else
    if (count < (image->columns*image->rows))
      {
        MagickWarning(CorruptImageWarning,"too much image data in file",
          image->filename);
        return(False);
      }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R u n l e n g t h E n c o d e I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RunlengthEncodeImage packs the runlength-encoded pixel packets
%  into the minimum number of bytes.
%
%  The format of the RunlengthEncodeImage routine is:
%
%      count=RunlengthEncodeImage(image)
%
%  A description of each parameter follows:
%
%    o status: Method RunlengthEncodeImage return the number of bytes the
%      image consumes.  Zero is returned if an error occurs.
%
%    o image: The address of a structure of type Image.
%
%
*/
Export unsigned int RunlengthEncodeImage(Image *image)
{
#define SpecialRunlength  255

  register int
    i,
    j,
    runlength;

  register long
    packets;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned long
    count;

  unsigned short
    value;

  assert(image != (Image *) NULL);
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      MagickWarning(CorruptImageWarning,"Unable to pack pixels",
        "no image pixels");
      return(0);
    }
  if (image->compression == RunlengthEncodedCompression)
    {
      register RunlengthPacket
        *q;

      /*
        Compress image.
      */
      p=image->pixels;
      for (i=0; i < (int) image->packets; i++)
      {
        if (p->length > SpecialRunlength)
          {
            /*
              Uncompress image to allow in-place compression.
            */
            if (!UncondenseImage(image))
              return(0);
            break;
          }
        p++;
      }
      p=image->pixels;
      runlength=p->length+1;
      packets=0;
      q=image->pixels;
      q->length=SpecialRunlength;
      if (image->matte || (image->colorspace == CMYKColorspace))
        for (i=0; i < (int) (image->columns*image->rows); i++)
        {
          if (runlength != 0)
            runlength--;
          else
            {
              p++;
              runlength=p->length;
            }
          if ((p->red == q->red) && (p->green == q->green) &&
              (p->blue == q->blue) && (p->index == q->index) &&
              (q->length < SpecialRunlength))
            q->length++;
          else
            {
              if (packets != 0)
                q++;
              packets++;
              *q=(*p);
              q->length=0;
            }
        }
      else
        for (i=0; i < (int) (image->columns*image->rows); i++)
        {
          if (runlength != 0)
            runlength--;
          else
            {
              p++;
              runlength=p->length;
            }
          if ((p->red == q->red) && (p->green == q->green) &&
              (p->blue == q->blue) && (q->length < SpecialRunlength))
            q->length++;
          else
            {
              if (packets != 0)
                q++;
              packets++;
              *q=(*p);
              q->length=0;
            }
        }
      image->packets=packets;
      image->pixels=(RunlengthPacket *) ReallocateMemory((char *) image->pixels,
        image->packets*sizeof(RunlengthPacket));
      /*
        Runlength-encode only if it consumes less memory than no compression.
      */
      if (image->class == DirectClass)
        {
          if (image->packets >= ((image->columns*image->rows*3) >> 2))
            image->compression=NoCompression;
        }
      else
        if (image->packets >= ((image->columns*image->rows) >> 1))
          image->compression=NoCompression;
    }
  /*
    Determine packed packet size.
  */
  if (image->class == PseudoClass)
    {
      image->packet_size=1;
      if (image->colors > 256)
        image->packet_size++;
    }
  else
    {
      image->packet_size=3*(image->depth >> 3);
      if (image->matte || (image->colorspace == CMYKColorspace))
        image->packet_size+=image->depth >> 3;
    }
  if (image->compression == RunlengthEncodedCompression)
    image->packet_size++;
  /*
    Allocate packed pixel memory.
  */
  packets=image->packets;
  if (image->compression != RunlengthEncodedCompression)
    packets=image->columns*image->rows;
  if (image->packed_pixels == (unsigned char *) NULL)
    image->packed_pixels=(unsigned char *) AllocateMemory((unsigned int)
      packets*image->packet_size*sizeof(unsigned char));
  else
    image->packed_pixels=(unsigned char *) ReallocateMemory((char *)
      image->packed_pixels,packets*image->packet_size*sizeof(unsigned char));
  if (image->packed_pixels == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Unable to pack pixels",
        "Memory allocation failed");
      return(0);
    }
  /*
    Packs the runlength-encoded pixel packets into the minimum number of bytes.
  */
  p=image->pixels;
  q=image->packed_pixels;
  count=0;
  if (image->class == DirectClass)
    {
      if (image->compression == RunlengthEncodedCompression)
        for (i=0; i < (int) image->packets; i++)
        {
          WriteQuantum(p->red,q);
          WriteQuantum(p->green,q);
          WriteQuantum(p->blue,q);
          if (image->matte || (image->colorspace == CMYKColorspace))
            WriteQuantum(p->index,q);
          *q++=p->length;
          count+=(p->length+1);
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
      else
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            WriteQuantum(p->red,q);
            WriteQuantum(p->green,q);
            WriteQuantum(p->blue,q);
            if (image->matte || (image->colorspace == CMYKColorspace))
              WriteQuantum(p->index,q);
          }
          count+=(p->length+1);
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
    }
  else
    if (image->compression == RunlengthEncodedCompression)
      {
        if (image->colors <= 256)
          for (i=0; i < (int) image->packets; i++)
          {
            *q++=(unsigned char) p->index;
            *q++=p->length;
            count+=(p->length+1);
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
        else
          for (i=0; i < (int) image->packets; i++)
          {
            *q++=p->index >> 8;
            *q++=(unsigned char) p->index;
            *q++=p->length;
            count+=(p->length+1);
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
      }
    else
      if (image->colors <= 256)
        for (i=0; i < (int) image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            *q++=p->index;
          count+=(p->length+1);
          p++;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(i,image->packets))
              ProgressMonitor(SaveImageText,i,image->packets);
        }
      else
        {
          register unsigned char
            xff00,
            xff;

          for (i=0; i < (int) image->packets; i++)
          {
            xff00=p->index >> 8;
            xff=p->index;
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=xff00;
              *q++=xff;
            }
            count+=(p->length+1);
            p++;
            if (image->previous == (Image *) NULL)
              if (QuantumTick(i,image->packets))
                ProgressMonitor(SaveImageText,i,image->packets);
          }
        }
  /*
    Guarentee the correct number of pixel packets.
  */
  if (count < (image->columns*image->rows))
    {
      MagickWarning(CorruptImageWarning,"insufficient image data in",
        image->filename);
      return(0);
    }
  else
    if (count > (image->columns*image->rows))
      {
        MagickWarning(CorruptImageWarning,"too much image data in",
          image->filename);
        return(0);
      }
  return((unsigned int) packets);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t R u n l e n g t h E n c o d e r                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetRunlengthEncoder initializes the runlength encoder.
%
%  The format of the SetRunlengthEncoder function is:
%
%      SetRunlengthEncoder(packet)
%
%  A description of each parameter follows:
%
%    o packet: Specifies a RunlengthPacket type.
%
%
*/
Export void SetRunlengthEncoder(RunlengthPacket *packet)
{
  packet->red=0;
  packet->green=0;
  packet->blue=0;
  packet->index=0;
  packet->length=MaxRunlength;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t R u n l e n g t h P a c k e t s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetRunlengthPackets initializes the runlength encoder.
%
%  The format of the SetRunlengthPackets function is:
%
%      SetRunlengthPackets(image,packets)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o packets: The number of runlength packets.
%
%
*/
Export void SetRunlengthPackets(Image *image,const unsigned long packets)
{
  image->packets=packets;
  image->pixels=(RunlengthPacket *) ReallocateMemory((char *) image->pixels,
    image->packets*sizeof(RunlengthPacket));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S U N D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SUNDecodeImage unpacks the packed image pixels into
%  runlength-encoded pixel packets.
%
%  The format of the SUNDecodeImage routine is:
%
%      status=SUNDecodeImage(compressed_pixels,number_columns,number_rows,
%        pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method SUNDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the uncompression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%
*/
Export unsigned int SUNDecodeImage(const unsigned char *compressed_pixels,
  const unsigned int number_columns,const unsigned int number_rows,
  unsigned char *pixels)
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
  while ((q-pixels) <= (int) (number_columns*number_rows))
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

#if defined(HasZLIB)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z L I B E n c o d e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ZLIBEncodeImage compresses an image via ZLIB-coding specific to
%  Postscript Level II or Portable Document Format.  To ensure portability, the
%  binary ZLIB bytes are encoded as ASCII base-85.
%
%  The format of the ZLIBEncodeImage routine is:
%
%      status=ZLIBEncodeImage(file,number_pixels,quality,pixels)
%
%  A description of each parameter follows:
%
%    o status:  Method ZLIBEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o file: The address of a structure of type FILE.  ZLIB encoded pixels
%      are written to this file.
%
%    o number_pixels:  An unsigned integer that specifies the number of
%      pixels to compress.
%
%    o quality: the compression level (0-100).
%
%    o pixels: The address of an unsigned array of characters containing the
%      pixels to compress.
%
%
*/
Export unsigned int ZLIBEncodeImage(FILE *file,
  const unsigned long number_pixels,const unsigned int quality,
  unsigned char *pixels)
{
  int
    status;

  register int
    i;

  unsigned char
    *compressed_pixels;

  unsigned long
    compressed_packets;

  z_stream
    stream;

  compressed_packets=(unsigned long) (1.001*number_pixels+12);
  compressed_pixels=(unsigned char *)
    AllocateMemory(compressed_packets*sizeof(unsigned char));
  if (compressed_pixels == (unsigned char *) NULL)
    {
      MagickWarning(ResourceLimitWarning,"Memory allocation failed",
        (char *) NULL);
      return(False);
    }
  stream.next_in=pixels;
  stream.avail_in=number_pixels;
  stream.next_out=compressed_pixels;
  stream.avail_out=compressed_packets;
  stream.zalloc=(alloc_func) NULL;
  stream.zfree=(free_func) NULL;
  stream.opaque=(voidpf) NULL;
  status=deflateInit(&stream,Min(quality/10,9));
  if (status == Z_OK)
    {
      status=deflate(&stream,Z_FINISH);
      if (status == Z_STREAM_END)
        status=deflateEnd(&stream);
      else
        (void) deflateEnd(&stream);
      compressed_packets=stream.total_out;
    }
  if (status)
    MagickWarning(DelegateWarning,"Unable to Zip compress image",(char *) NULL);
  else
    {
      Ascii85Initialize();
      for (i=0; i < (int) compressed_packets; i++)
        Ascii85Encode(compressed_pixels[i],file);
      Ascii85Flush(file);
    }
  FreeMemory((char *) compressed_pixels);
  return(!status);
}
#else
Export unsigned int ZLIBEncodeImage(FILE *file,
  const unsigned long number_pixels,const unsigned int quality,
  unsigned char *pixels)
{
  MagickWarning(MissingDelegateWarning,"ZLIB library is not available",
    (char *) NULL);
  return(False);
}
#endif
