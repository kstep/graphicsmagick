/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        IIIII   CCCC   OOO   N   N                           %
%                          I    C      O   O  NN  N                           %
%                          I    C      O   O  N N N                           %
%                          I    C      O   O  N  NN                           %
%                        IIIII   CCCC   OOO   N   N                           %
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
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C O N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadIconImage reads a Microsoft icon image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadIconImage method is:
%
%      Image *ReadIconImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadIconImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to an ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadIconImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define MaxIcons  256

  typedef struct _IconEntry
  {
    unsigned char
      width,
      height,
      colors,
      reserved;

    short int
      planes,
      bits_per_pixel;

    unsigned int
      size,
      offset;
  } IconEntry;

  typedef struct _IconFile
  {
    short
      reserved,
      resource_type,
      count;

    IconEntry
      directory[MaxIcons];
  } IconFile;

  typedef struct _IconInfo
  {
    unsigned long
      size,
      width,
      height;

    unsigned short
      planes,
      bits_per_pixel;

    unsigned long
      compression,
      image_size,
      x_pixels,
      y_pixels,
      number_colors,
      colors_important;
  } IconInfo;

  IconFile
    icon_file;

  IconInfo
    icon_info;

  Image
    *image;

  int
    bit,
    byte,
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

  unsigned char
    *icon_colormap;

  unsigned int
    status;

  /*
    Open image file.
  */
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryType);
  if (status == False)
    ThrowReaderException(FileOpenWarning,"Unable to open file",image);
  icon_file.reserved=LSBFirstReadShort(image);
  icon_file.resource_type=LSBFirstReadShort(image);
  icon_file.count=LSBFirstReadShort(image);
  if ((icon_file.reserved != 0) || (icon_file.resource_type != 1) ||
      (icon_file.count > MaxIcons))
    ThrowReaderException(CorruptImageWarning,"Not a ICO image file",image);
  for (i=0; i < icon_file.count; i++)
  {
    icon_file.directory[i].width=ReadByte(image);
    icon_file.directory[i].height=ReadByte(image);
    icon_file.directory[i].colors=ReadByte(image);
    icon_file.directory[i].reserved=ReadByte(image);
    icon_file.directory[i].planes=LSBFirstReadShort(image);
    icon_file.directory[i].bits_per_pixel=LSBFirstReadShort(image);
    icon_file.directory[i].size=LSBFirstReadLong(image);
    icon_file.directory[i].offset=LSBFirstReadLong(image);
  }
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify Icon identifier.
    */
    (void) SeekBlob(image,icon_file.directory[i].offset,SEEK_SET);
    icon_info.size=LSBFirstReadLong(image);
    icon_info.width=LSBFirstReadLong(image);
    icon_info.height=LSBFirstReadLong(image);
    icon_info.planes=LSBFirstReadShort(image);
    icon_info.bits_per_pixel=LSBFirstReadShort(image);
    icon_info.compression=LSBFirstReadLong(image);
    icon_info.image_size=LSBFirstReadLong(image);
    icon_info.x_pixels=LSBFirstReadLong(image);
    icon_info.y_pixels=LSBFirstReadLong(image);
    icon_info.number_colors=LSBFirstReadLong(image);
    icon_info.colors_important=LSBFirstReadLong(image);
    image->columns=(unsigned int) icon_info.width;
    image->rows=(unsigned int) icon_info.height;
    if (!AllocateImageColormap(image,1 << icon_info.bits_per_pixel))
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    if (image_info->ping)
      {
        CloseBlob(image);
        return(image);
      }
    /*
      Read Icon raster colormap.
    */
    icon_colormap=(unsigned char *) AllocateMemory(4*image->colors);
    if (icon_colormap == (unsigned char *) NULL)
      ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
        image);
    (void) ReadBlob(image,4*image->colors,(char *) icon_colormap);
    p=icon_colormap;
    for (x=0; x < (int) image->colors; x++)
    {
      image->colormap[x].blue=UpScale(*p++);
      image->colormap[x].green=UpScale(*p++);
      image->colormap[x].red=UpScale(*p++);
      p++;
    }
    FreeMemory((void **) &icon_colormap);
    /*
      Convert Icon raster image to pixel packets.
    */
    image->columns=icon_file.directory[i].width;
    image->rows=icon_file.directory[i].height;
    switch (icon_info.bits_per_pixel)
    {
      case 1:
      {
        /*
          Convert bitmap scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < ((int) image->columns-7); x+=8)
          {
            byte=ReadByte(image);
            for (bit=0; bit < 8; bit++)
              indexes[x+bit]=(byte & (0x80 >> bit) ? 0x01 : 0x00);
          }
          if ((image->columns % 8) != 0)
            {
              byte=ReadByte(image);
              for (bit=0; bit < (int) (image->columns % 8); bit++)
                indexes[x+bit]=((*p) & (0x80 >> bit) ? 0x01 : 0x00);
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 4:
      {
        /*
          Read 4-bit Icon scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < ((int) image->columns-1); x+=2)
          {
            byte=ReadByte(image);
            indexes[x]=(byte >> 4) & 0xf;
            indexes[x+1]=(byte) & 0xf;
          }
          if ((image->columns % 2) != 0)
            {
              byte=ReadByte(image);
              indexes[x]=(byte >> 4) & 0xf;
            }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 8:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (int) image->columns; x++)
          {
            byte=ReadByte(image);
            indexes[x]=(IndexPacket) byte;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      case 16:
      {
        /*
          Convert PseudoColor scanline.
        */
        for (y=image->rows-1; y >= 0; y--)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetIndexes(image);
          for (x=0; x < (int) image->columns; x++)
          {
            byte=ReadByte(image);
            indexes[x]=(IndexPacket) byte;
            byte=ReadByte(image);
            indexes[x]|=byte << 8;
          }
          if (!SyncImagePixels(image))
            break;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
        }
        break;
      }
      default:
        ThrowReaderException(CorruptImageWarning,"Not a ICO image file",image);
    }
    SyncImage(image);
    /*
      Convert bitmap scanline to pixel packets.
    */
    image->storage_class=DirectClass;
    image->matte=True;
    for (y=image->rows-1; y >= 0; y--)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        break;
      for (x=0; x < ((int) image->columns-7); x+=8)
      {
        byte=ReadByte(image);
        for (bit=0; bit < 8; bit++)
          q[x+bit].opacity=
            (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
      }
      if ((image->columns % 8) != 0)
        {
          byte=ReadByte(image);
          for (bit=0; bit < (int) (image->columns % 8); bit++)
            q[x+bit].opacity=
              (byte & (0x80 >> bit) ? TransparentOpacity : OpaqueOpacity);
        }
      if (!SyncImagePixels(image))
        break;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          ProgressMonitor(LoadImageText,image->rows-y-1,image->rows);
    }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (i < (icon_file.count-1))
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
  }
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
%   R e g i s t e r I C O N I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterICONImage adds attributes for the Icon image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterICONImage method is:
%
%      RegisterICONImage(void)
%
*/
ModuleExport void RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("ICO");
  entry->decoder=ReadIconImage;
  entry->adjoin=False;
  entry->description=AllocateString("Microsoft icon");
  entry->module=AllocateString("ICON");
  RegisterMagickInfo(entry);
  entry=SetMagickInfo("ICON");
  entry->decoder=ReadIconImage;
  entry->adjoin=False;
  entry->description=AllocateString("Microsoft icon");
  entry->module=AllocateString("ICON");
  RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I C O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterICONImage removes format registrations made by the
%  ICON module from the list of supported formats.
%
%  The format of the UnregisterICONImage method is:
%
%      UnregisterICONImage(void)
%
*/
ModuleExport void UnregisterICONImage(void)
{
  UnregisterMagickInfo("ICO");
  UnregisterMagickInfo("ICON");
}
