/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            PPPP   DDDD   BBBB                               %
%                            P   P  D   D  B   B                              %
%                            PPPP   D   D  BBBB                               %
%                            P      D   D  B   B                              %
%                            P      DDDD   BBBB                               %
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
%  Copyright (C) 2002 ImageMagick Studio, a non-profit organization dedicated %
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
#include <time.h>
#include "studio.h"

/*
  Typedef declarations.
*/
typedef struct _PDBInfo
{
  char
    name[32];

  short int
    attributes,
    version;

  unsigned long
    create_time,
    modify_time,
    archive_time,
    modify_number,
    application_info,
    sort_info;

  char
    type[4],  /* database type identifier "vIMG" */
    id[4];    /* database creator identifier "View" */

  unsigned long
    seed,
    next_record;

  short int
    number_records;
} PDBInfo;

typedef struct _PDBImage
{
  char
    name[32],
    version,
    type;

  unsigned long
    reserved_1,
    note;

  short int
    x_last,
    y_last;

  unsigned long
    reserved_2;

  short int
    x_anchor,
    y_anchor,
    width,
    height;
} PDBImage;
/*
  Forward declarations.
*/
static unsigned int
  WritePDBImage(const ImageInfo *,Image *);

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
%  Method DecodeImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.
%
%  The format of the DecodeImage method is:
%
%      unsigned int DecodeImage(Image *image,unsigned char *pixels,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method DecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the decoding process.
%
%
*/
static unsigned int DecodeImage(Image *image,unsigned char *pixels,
  const size_t length)
{
  int
    count,
    pixel;

  register int
    i;

  register unsigned char
    *p;

  p=pixels;
  while (p < (pixels+length))
  {
    pixel=ReadBlobByte(image);
    if (pixel <= 0x80)
      {
        count=pixel+1;
        for (i=0; i < count; i++)
          *p++=ReadBlobByte(image);
        continue;
      }
    count=pixel+1-0x80;
    pixel=ReadBlobByte(image);
    for (i=0; i < count; i++)
      *p++=(unsigned char) pixel;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P D B                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPDB returns True if the image format type, identified by the
%  magick string, is PDB.
%
%  The format of the ReadPDBImage method is:
%
%      unsigned int IsPDB(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPDB returns True if the image format type is PDB.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsPDB(const unsigned char *magick,const size_t length)
{
  if (length < 68)
    return(False);
  if (LocaleNCompare((char *) (magick+60),"vIMGView",8) == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P D B I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPDBImage reads an Pilot image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPDBImage method is:
%
%      Image *ReadPDBImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPDBImage returns a pointer to the image after
%      reading. A null image is returned if there is a memory shortage or if
%      the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadPDBImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    record_type,
    tag[3];

  Image
    *image;

  IndexPacket
    index;

  long
    offset,
    y;

  PDBImage
    pdb_image;

  PDBInfo
    pdb_info;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  size_t
    count;

  unsigned char
    *pixels;

  unsigned int
    bits_per_pixel,
    status;

  unsigned long
    packets;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,"Unable to open file",image);
  /*
    Determine if this is a PDB image file.
  */
  count=ReadBlob(image,32,pdb_info.name);
  pdb_info.attributes=ReadBlobMSBShort(image);
  pdb_info.version=ReadBlobMSBShort(image);
  pdb_info.create_time=ReadBlobMSBLong(image);
  pdb_info.modify_time=ReadBlobMSBLong(image);
  pdb_info.archive_time=ReadBlobMSBLong(image);
  pdb_info.modify_number=ReadBlobMSBLong(image);
  pdb_info.application_info=ReadBlobMSBLong(image);
  pdb_info.sort_info=ReadBlobMSBLong(image);
  (void) ReadBlob(image,4,pdb_info.type);
  (void) ReadBlob(image,4,pdb_info.id);
  if ((count == 0) || (memcmp(pdb_info.type,"vIMG",4) != 0) ||
      (memcmp(pdb_info.id,"View",4) != 0))
    ThrowReaderException(CorruptImageWarning,"Not a supported PDB image file",
      image);
  pdb_info.seed=ReadBlobMSBLong(image);
  pdb_info.next_record=ReadBlobMSBLong(image);
  pdb_info.number_records=ReadBlobMSBShort(image);
  if (pdb_info.next_record != 0)
    ThrowReaderException(CorruptImageWarning,
      "Multiple Record List PDB Unsupported",image);
  /*
    Read record header.
  */
  offset=(long) ReadBlobMSBLong(image);
  (void) ReadBlob(image,3,tag);
  record_type=ReadBlobByte(image);
  if (((record_type != 0x00) && (record_type != 0x01)) ||
      (memcmp(tag,"\x40\x6f\x80",3) != 0))
    ThrowReaderException(CorruptImageError,"Corrupt PDB image file",image);
  if ((offset-TellBlob(image)) == 6)
    {
      (void) ReadBlobByte(image);
      (void) ReadBlobByte(image);
    }
  if (pdb_info.number_records > 1)
    {
      offset=(long) ReadBlobMSBLong(image);
      (void) ReadBlob(image,3,tag);
      record_type=ReadBlobByte(image);
      if (((record_type != 0x00) && (record_type != 0x01)) ||
          (memcmp(tag,"\x40\x6f\x80",3) != 0))
        ThrowReaderException(CorruptImageError,"Corrupt PDB image file",image);
      if ((offset-TellBlob(image)) == 6)
        {
          (void) ReadBlobByte(image);
          (void) ReadBlobByte(image);
        }
    }
  /*
    Read image header.
  */
  (void) ReadBlob(image,32,pdb_image.name);
  pdb_image.version=ReadBlobByte(image);
  pdb_image.type=ReadBlobByte(image);
  pdb_image.reserved_1=ReadBlobMSBLong(image);
  pdb_image.note=ReadBlobMSBLong(image);
  pdb_image.x_last=ReadBlobMSBShort(image);
  pdb_image.y_last=ReadBlobMSBShort(image);
  pdb_image.reserved_2=ReadBlobMSBLong(image);
  pdb_image.x_anchor=ReadBlobMSBShort(image);
  pdb_image.y_anchor=ReadBlobMSBShort(image);
  pdb_image.width=ReadBlobMSBShort(image);
  pdb_image.height=ReadBlobMSBShort(image);
  /*
    Initialize image structure.
  */
  image->columns=pdb_image.width;
  image->rows=pdb_image.height;
  image->depth=8;
  image->storage_class=PseudoClass;
  bits_per_pixel=pdb_image.type == 0 ? 2 : pdb_image.type == 2 ? 4 : 1;
  if (!AllocateImageColormap(image,1 << bits_per_pixel))
    ThrowReaderException(ResourceLimitError,"Memory allocation failed",image);
  if (image_info->ping)
    {
      CloseBlob(image);
      return(image);
    }
  packets=(bits_per_pixel*image->columns/8)*image->rows;
  pixels=(unsigned char *) AcquireMemory(packets+256);
  if (pixels == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",image);
  switch (pdb_image.version)
	{
    case 0:
    {
      image->compression=NoCompression;
      (void) ReadBlob(image,packets,(char *) pixels);
      break;
    }
    case 1:
    {
      image->compression=RunlengthEncodedCompression;
      (void) DecodeImage(image,pixels,packets);
      break;
    }
    default:
      ThrowReaderException(CorruptImageWarning,"Unknown compression type",image)
  }
  p=pixels;
  switch (bits_per_pixel)
  {
    case 1:
    {
      int
        bit;

      /*
        Read 1-bit PDB image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < ((long) image->columns-7); x+=8)
        {
          for (bit=0; bit < 8; bit++)
          {
            index=(*p & (0x80 >> bit) ? 0x00 : 0x01);
            indexes[x+bit]=index;
            *q++=image->colormap[index];
          }
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    case 2:
    {
      /*
        Read 2-bit PDB image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x+=4)
        {
          index=ConstrainColormapIndex(image,3-((*p >> 6) & 0x03));
          indexes[x]=index;
          *q++=image->colormap[index];
          index=ConstrainColormapIndex(image,3-((*p >> 4) & 0x03));
          indexes[x+1]=index;
          *q++=image->colormap[index];
          index=ConstrainColormapIndex(image,3-((*p >> 2) & 0x03));
          indexes[x+2]=index;
          *q++=image->colormap[index];
          index=ConstrainColormapIndex(image,3-((*p) & 0x03));
          indexes[x+3]=index;
          *q++=image->colormap[index];
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    case 4:
    {
      /*
        Read 4-bit PDB image.
      */
      for (y=0; y < (long) image->rows; y++)
      {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL)
          break;
        indexes=GetIndexes(image);
        for (x=0; x < (long) image->columns; x+=2)
        {
          index=ConstrainColormapIndex(image,15-((*p >> 4) & 0x0f));
          indexes[x]=index;
          *q++=image->colormap[index];
          index=ConstrainColormapIndex(image,15-((*p) & 0x0f));
          indexes[x+1]=index;
          *q++=image->colormap[index];
          p++;
        }
        if (!SyncImagePixels(image))
          break;
        if (QuantumTick(y,image->rows))
          MagickMonitor(LoadImageText,y,image->rows);
      }
      break;
    }
    default:
      ThrowReaderException(CorruptImageError,"Not a PDB image file",image)
  }
  LiberateMemory((void **) &pixels);
  if (EOFBlob(image))
    ThrowReaderException(CorruptImageError,"Unexpected end-of-file",image);
  if ((offset-TellBlob(image)) == 0)
    {
      char
        *comment;

      int
        c;

      register char
        *p;

      unsigned int
        length;

      /*
        Read comment.
      */
      c=ReadBlobByte(image);
      length=MaxTextExtent;
      comment=(char *) AcquireMemory(length+1);
      p=comment;
      if (comment != (char *) NULL)
        for ( ; c != EOF; p++)
        {
          if ((p-comment) >= (int) length)
            {
              length<<=1;
              length+=MaxTextExtent;
              ReacquireMemory((void **) &comment,length+1);
              if (comment == (char *) NULL)
                break;
              p=comment+strlen(comment);
            }
          *p=c;
          *(p+1)='\0';
          c=ReadBlobByte(image);
        }
      if (comment == (char *) NULL)
        ThrowReaderException(ResourceLimitError,"Memory allocation failed",
          image);
      (void) SetImageAttribute(image,"comment",comment);
      LiberateMemory((void **) &comment);
    }
  CloseBlob(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P D B I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPDBImage adds attributes for the PDB image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPDBImage method is:
%
%      RegisterPDBImage(void)
%
*/
ModuleExport void RegisterPDBImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("PDB");
  entry->decoder=ReadPDBImage;
  entry->encoder=WritePDBImage;
  entry->magick=IsPDB;
  entry->description=AcquireString("Palm Database ImageViewer Format");
  entry->module=AcquireString("PDB");
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P D B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPDBImage removes format registrations made by the
%  PDB module from the list of supported formats.
%
%  The format of the UnregisterPDBImage method is:
%
%      UnregisterPDBImage(void)
%
*/
ModuleExport void UnregisterPDBImage(void)
{
  (void) UnregisterMagickInfo("PDB");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P D B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WritePDBImage writes an image
%
%  The format of the WritePDBImage method is:
%
%      unsigned int WritePDBImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WritePDBImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o image:  A pointer to a Image structure.
%
%
*/

static unsigned char *EncodeRLE(unsigned char *destination,
  unsigned char *source,unsigned int literal,unsigned int repeat)
{
  if (literal > 0)
    *destination++=literal-1;
  memcpy(destination,source,literal);
  destination+=literal;
  if (repeat > 0)
    {
      *destination++=0x80 | (repeat-1);
      *destination++=source[literal];
    }
  return(destination);
}

static unsigned int WritePDBImage(const ImageInfo *image_info,Image *image)
{
  char
    bits;

  long
    y;

  PDBImage
    pdb_image;

  PDBInfo
    pdb_info;

  register long
    x;

  unsigned char
    *buffer,
    *p,
    *q,
    *scanline;

  unsigned int
    bits_per_pixel,
    status,
    literal,
    repeat;

  unsigned long
    packet_size,
    packets;

  const ImageAttribute
    *comment;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryType,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenWarning,"Unable to open file",image);
  (void) TransformRGBImage(image,RGBColorspace);
  bits_per_pixel=image_info->depth;
  if (GetImageType(image,&image->exception) == BilevelType)
    bits_per_pixel=1;
  if ((bits_per_pixel != 1) && (bits_per_pixel != 2))
    bits_per_pixel=4;
  memset(pdb_info.name,0,32);
  strncpy(pdb_info.name,image_info->filename,32);
  pdb_info.attributes=0;
  pdb_info.version=0;
  pdb_info.create_time=time(NULL);
  pdb_info.modify_time=pdb_info.create_time;
  pdb_info.archive_time=0;
  pdb_info.modify_number=0;
  pdb_info.application_info=0;
  pdb_info.sort_info=0;
  memcpy(pdb_info.type,"vIMG",4);
  memcpy(pdb_info.id,"View",4);
  pdb_info.seed=0;
  pdb_info.next_record=0;
  comment=GetImageAttribute(image,"comment");
  pdb_info.number_records=(comment == (ImageAttribute *) NULL ? 1 : 2);
  (void) WriteBlob(image,32,pdb_info.name);
  (void) WriteBlobMSBShort(image,pdb_info.attributes);
  (void) WriteBlobMSBShort(image,pdb_info.version);
  (void) WriteBlobMSBLong(image,pdb_info.create_time);
  (void) WriteBlobMSBLong(image,pdb_info.modify_time);
  (void) WriteBlobMSBLong(image,pdb_info.archive_time);
  (void) WriteBlobMSBLong(image,pdb_info.modify_number);
  (void) WriteBlobMSBLong(image,pdb_info.application_info);
  (void) WriteBlobMSBLong(image,pdb_info.sort_info);
  (void) WriteBlob(image,4,pdb_info.type);
  (void) WriteBlob(image,4,pdb_info.id);
  (void) WriteBlobMSBLong(image,pdb_info.seed);
  (void) WriteBlobMSBLong(image,pdb_info.next_record);
  (void) WriteBlobMSBShort(image,pdb_info.number_records);
  (void) strncpy(pdb_image.name, pdb_info.name, 32);
  pdb_image.version=1;  /* RLE Compressed */
  switch(bits_per_pixel)
	{
    case 1: pdb_image.type=(char) 0xff; break;  /* monochrome */
    case 2: pdb_image.type=(char) 0x00; break;  /* 2 bit gray */
    default: pdb_image.type=(char) 0x02;  /* 4 bit gray */
  }
  pdb_image.reserved_1=0;
  pdb_image.note=0;
  pdb_image.x_last=0;
  pdb_image.y_last=0;
  pdb_image.reserved_2=0;
  pdb_image.x_anchor=(short) 0xffff;
  pdb_image.y_anchor=(short) 0xffff;
  pdb_image.width = image->columns;
  if (image->columns % 16)
    pdb_image.width=16*(image->columns/16+1);
  pdb_image.height=image->rows;
  packets=(bits_per_pixel*image->columns/8)*image->rows;
  p=(unsigned char *) AcquireMemory(packets+packets/128);
  if (p == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  buffer=(unsigned char *) AcquireMemory(256);
  if (buffer == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  packet_size=image->depth > 8 ? 2: 1;
  scanline=(unsigned char *) AcquireMemory(packet_size*image->columns);
  if (scanline == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitWarning,"Memory allocation failed",image);
  (void) TransformRGBImage(image,RGBColorspace);
  /*
    Convert to GRAY raster scanline.
  */
  bits=8/bits_per_pixel-1;  /* start at most significant bits */
  literal=0;
  repeat=0;
  q=p;
  buffer[0]=0x00;
  for (y=0; y < (long) image->rows; y++)
  {
    if (!AcquireImagePixels(image,0,y,image->columns,1,&image->exception))
      break;
    (void) PopImagePixels(image,GrayQuantum,scanline);
    for (x=0; x < pdb_image.width; x++)
    {
      if (x < image->columns)
        buffer[literal+repeat]|=(0xff-scanline[x*packet_size]) >>
          (8-bits_per_pixel) << bits*bits_per_pixel;
      bits--;
      if (bits < 0)
        {
          if (((literal+repeat) > 0) &&
              (buffer[literal+repeat] == buffer[literal+repeat-1]))
            {
              if (repeat == 0)
                {
                  literal--;
                  repeat++;
                }
              repeat++;
              if (0x7f < repeat)
                {
                  q=EncodeRLE(q,buffer,literal,repeat);
                  literal=0;
                  repeat=0;
                }
            }
          else
            {
              if (repeat >= 2)
                literal+=repeat;
              else
                {
                  q=EncodeRLE(q,buffer,literal,repeat);
                  buffer[0]=buffer[literal+repeat];
                  literal=0;
                }
              literal++;
              repeat=0;
              if (0x7f < literal)
                {
                  q=EncodeRLE(q,buffer,(literal < 0x80 ? literal : 0x80),0);
                  CloneMemory(buffer,buffer+literal+repeat,0x80);
                  literal-=0x80;
                }
            }
        bits=8/bits_per_pixel-1;
        buffer[literal+repeat]=0x00;
      }
    }
    if (QuantumTick(y,image->rows))
      MagickMonitor(SaveImageText,y,image->rows);
  }
  q=EncodeRLE(q,buffer,literal,repeat);
  LiberateMemory((void **) &scanline);
  LiberateMemory((void **) &buffer);
  /*
    Write the Image record header.
  */
  (void) WriteBlobMSBLong(image,TellBlob(image)+8*pdb_info.number_records);
  (void) WriteBlobByte(image,0x40);
  (void) WriteBlobByte(image,0x6f);
  (void) WriteBlobByte(image,0x80);
  (void) WriteBlobByte(image,0);
  if (pdb_info.number_records > 1)
    {
      /*
        Write the comment record header.
      */
      (void) WriteBlobMSBLong(image,TellBlob(image)+8+58+q-p);
      (void) WriteBlobByte(image,0x40);
      (void) WriteBlobByte(image,0x6f);
      (void) WriteBlobByte(image,0x80);
      (void) WriteBlobByte(image,1);
    }
  /*
	 Write the Image data.
  */
  (void) WriteBlob(image,32,pdb_image.name);
  (void) WriteBlobByte(image,pdb_image.version);
  (void) WriteBlobByte(image,pdb_image.type);
  (void) WriteBlobMSBLong(image,pdb_image.reserved_1);
  (void) WriteBlobMSBLong(image,pdb_image.note);
  (void) WriteBlobMSBShort(image,pdb_image.x_last);
  (void) WriteBlobMSBShort(image,pdb_image.y_last);
  (void) WriteBlobMSBLong(image,pdb_image.reserved_2);
  (void) WriteBlobMSBShort(image,pdb_image.x_anchor);
  (void) WriteBlobMSBShort(image,pdb_image.y_anchor);
  (void) WriteBlobMSBShort(image,pdb_image.width);
  (void) WriteBlobMSBShort(image,pdb_image.height);
  (void) WriteBlob(image,q-p,p);
  LiberateMemory((void **) &p);
  if (pdb_info.number_records > 1)
    WriteBlobString(image,comment->value);
  CloseBlob(image);
  return(True);
}
